#include <QLayout>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "maptype.h"

long int SystemMessageBox(void * parm, HWND hWnd, const WCHAR *message,
                       const WCHAR *caption, long int flag);

// -----------------------------------------------------------------------------
// Класс открытия карты и управления ресурсами
// -----------------------------------------------------------------------------
// Конструктор
// -----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  MapHandle(0),
  OriginalGridColor(0),
  OriginalGridStep(0),
  OriginalGridThickness(0),
  CoordinatesType(PLANEPOINT)
{
  mapSetMessageBoxCall(SystemMessageBox, 0);

  CodecKOI8R = QTextCodec::codecForName("KOI8-R");
  ui->setupUi(this);

  CreateMenuActionGroups();

  connect(ui->actionCreateScript, SIGNAL(triggered(bool)), this, SLOT(CreateScript()));
  connect(ui->actionOpenMap, SIGNAL(triggered(bool)), this, SLOT(OpenMap()));
  connect(ui->actionCloseMap, SIGNAL(triggered(bool)), this, SLOT(CloseMap()));
  connect(ui->actionMemoryLimit, SIGNAL(triggered(bool)), this, SLOT(SetMemoryLimit()));
  connect(ui->actionListCountLimit, SIGNAL(triggered(bool)), this, SLOT(SetListCountLimit()));
  connect(ui->actionInc, SIGNAL(triggered(bool)), this, SLOT(PrecisionInc()));
  connect(ui->actionDec, SIGNAL(triggered(bool)), this, SLOT(PrecisionDec()));
  connect(ui->actionViewGrid, SIGNAL(triggered(bool)), this, SLOT(ViewGridTriggered(bool)));
  connect(ui->actionTurnOn30Degrees, SIGNAL(triggered(bool)), this, SLOT(TurnOn30DegreesTriggered()));
  connect(ui->actionRestoreViewAfterTurning, SIGNAL(triggered(bool)), this, SLOT(RestoreViewAfterTurningTriggered()));
  connect(ui->actionMoveWindowCenterToPoint, SIGNAL(triggered(bool)), this, SLOT(MoveWindowCenterToPointTriggered()));
  connect(ui->actionMoveWindowCenterToObject, SIGNAL(triggered(bool)), this, SLOT(MoveWindowCenterToObjectTriggered()));
  connect(ui->actionSetScale100000, SIGNAL(triggered(bool)), this, SLOT(SetScale100000Triggered()));
  connect(ui->actionSetScale200000, SIGNAL(triggered(bool)), this, SLOT(SetScale200000Triggered()));
}

// -----------------------------------------------------------------------------
// Деструктор
// -----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
  delete CurrentPrecision;
  delete ui;
}

// -----------------------------------------------------------------------------
// Собственная обработка собыйти
// -----------------------------------------------------------------------------
bool MainWindow::event(QEvent *event)
{
  bool ret = QWidget::event(event);

  if (event->type() == QEvent::Polish)
    polish();
  return ret;
}

// -----------------------------------------------------------------------------
// Создание сценария
// -----------------------------------------------------------------------------
void MainWindow::CreateScript()
{
    cw = new ConnectionWindow(this);
    cw->setModal(true);
    cw->exec();
}

// -----------------------------------------------------------------------------
// Открытие карты
// -----------------------------------------------------------------------------

void MainWindow::OpenMap()
{
    QString fileName = QFileDialog::getOpenFileName(this, 0, 0,
    CodecKOI8R->toUnicode("Карты (*.map);; Пользовательские карты (*.sit);; Хранилище (*.sitx);; Проекты (*.mpt);; Матричные карты (*.mtw *.mtq);; Растровые карты (*.rsw)"));
    if (! fileName.isEmpty())
    {
        MapHandle = mapOpenAnyData(fileName.utf16());
        if (MapHandle)
        {
            Map = new MapWidget();
            Map->setMouseTracking(true);
            Map->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            ui->scrollArea->setWidget(Map);
            Map->SetMapHandle(MapHandle);

            long int mapWidth, mapHeight;
            mapGetPictureSize(MapHandle, &mapWidth, &mapHeight);

            // Изменение размеров содержимого
            Map->resize(mapWidth, mapHeight);

            SaveMapViewParams();
        }
    }
}

// -----------------------------------------------------------------------------
// Закрытие карты
// -----------------------------------------------------------------------------
void MainWindow::CloseMap()
{
  if (MapHandle)
  {
    mapCloseData(MapHandle);
    MapHandle = 0;

    if (Map)
    {
      delete Map;
      Map = 0;
    }

    ResetMapViewParams();
  }
}

// -----------------------------------------------------------------------------
// Установить ограничение на объем используемой памяти
// -----------------------------------------------------------------------------
void MainWindow::SetMemoryLimit()
{
  if (MapHandle != 0)
  {
    bool result = false;
    unsigned long int limit = QInputDialog::getInt(this, CodecKOI8R->toUnicode("Лимит памяти"),
                              CodecKOI8R->toUnicode("Задайте программе лимит памяти (Мб)"),
                              200, result);
      mapSetMemoryLimit(MapHandle, limit * 1024);
  }
}

// -----------------------------------------------------------------------------
// Установить ограничение на количество одновремено открытых листов
// -----------------------------------------------------------------------------
void MainWindow::SetListCountLimit()
{
  if (MapHandle != 0)
  {
    bool result = false;
    long int limit = QInputDialog::getInt(this, CodecKOI8R->toUnicode("Лимит листов"),
                                          CodecKOI8R->toUnicode("Задайте число листов, открытых одновременно"),
                                          4, result);
      mapSetActiveListCountLimit(MapHandle, limit);
  }
}

// -----------------------------------------------------------------------------
// Сдвиг карты на (dx, dy)
// -----------------------------------------------------------------------------
void MainWindow::ShiftLeftTopCorner(long int dx,long int dy)
{
  if (MapHandle == 0)
    return;

  long int X,Y;
  long int mapW, mapH;
  mapGetPictureSize(MapHandle, &mapW, &mapH);

  X = ui->scrollArea->horizontalScrollBar()->value() + dx;
  Y = ui->scrollArea->verticalScrollBar()->value() + dy;
  if (X > mapW - ui->scrollArea->viewport()->width())
    X = mapW - ui->scrollArea->viewport()->width();
  if (Y > mapH - ui->scrollArea->viewport()->height())
    Y = mapH - ui->scrollArea->viewport()->height();
  if (X < 0)
    X = 0;
  if (Y < 0)
    Y = 0;

  ui->scrollArea->horizontalScrollBar()->setValue(X);
  ui->scrollArea->verticalScrollBar()->setValue(Y);
}

void MainWindow::GetLeftTop(long &x, long &y)
{
  if (MapHandle)
  {
    x = ui->scrollArea->horizontalScrollBar()->value();
    y = ui->scrollArea->verticalScrollBar()->value();
  }
}

void MainWindow::SetCenter(long x, long y)
{
  if (MapHandle)
  {
    long mapWidth = 0;
    long mapHeight = 0;
    mapGetPictureSize(MapHandle, &mapWidth, &mapHeight);

    x -= ui->scrollArea->viewport()->width() / 2;
    y -= ui->scrollArea->viewport()->height() / 2;

    if (x > mapWidth - ui->scrollArea->viewport()->width())
      x = mapWidth - ui->scrollArea->viewport()->width();
    if (y > mapHeight - ui->scrollArea->viewport()->height())
      y = mapHeight - ui->scrollArea->viewport()->height();
    if (x < 0)
      x = 0;
    if (y < 0)
      y = 0;

    ui->scrollArea->horizontalScrollBar()->setValue(x);
    ui->scrollArea->verticalScrollBar()->setValue(y);
  }
}

void MainWindow::GetCenter(long &x, long &y)
{
  if (MapHandle)
  {
    x = ui->scrollArea->horizontalScrollBar()->value() + ui->scrollArea->viewport()->width() / 2;
    y = ui->scrollArea->verticalScrollBar()->value() + ui->scrollArea->viewport()->height() / 2;
  }
}

// -----------------------------------------------------------------------------
// Изменение масштаба
// -----------------------------------------------------------------------------
void MainWindow::ChangeViewScale(float change)
{
  if (MapHandle == 0)
    return;

  long int X,Y;
  long int mapW, mapH;

  // Вычислим текущий центр
  X = ui->scrollArea->horizontalScrollBar()->value() + ui->scrollArea->viewport()->width() / 2;
  Y = ui->scrollArea->verticalScrollBar()->value() + ui->scrollArea->viewport()->height() / 2;
  mapChangeViewScale(MapHandle, &X, &Y, change);
  mapGetPictureSize(MapHandle, &mapW, &mapH);

  // Изменение размеров содержимого
  Map->resize(mapW, mapH);

  // Вычислим новый центр
  X = X - ui->scrollArea->viewport()->width() / 2;
  if (X < 0)
    X = 0;
  Y = Y - ui->scrollArea->viewport()->height() / 2;
  if (Y < 0)
    Y = 0;

  ui->scrollArea->horizontalScrollBar()->setValue(X);
  ui->scrollArea->verticalScrollBar()->setValue(Y);

  Map->update();
}

void MainWindow::SetViewScale(float scale)
{
  if (MapHandle)
  {
    long centerX = 0;
    long centerY = 0;
    GetCenter(centerX, centerY);

    mapSetViewScale(MapHandle, &centerX, &centerY, scale);

    long mapWidth = 0;
    long mapHeight = 0;
    mapGetPictureSize(MapHandle, &mapWidth, &mapHeight);

    Map->resize(mapWidth, mapHeight);
    SetCenter(centerX, centerY);

    Map->update();
  }
}

void MainWindow::CreateMenuActionGroups()
{
  // Группа шага сетки
  gridStepActionGroup = new QActionGroup(this);
  ui->gridStepLittleAction->setData(QVariant(STEP_LITTLE));
  gridStepActionGroup->addAction(ui->gridStepLittleAction);
  ui->gridStepBigAction->setData(QVariant(STEP_BIG));
  gridStepActionGroup->addAction(ui->gridStepBigAction);
  ui->gridStepLittleAction->setChecked(true);
  connect(gridStepActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(GridStepTriggered()));

  // Группа толщины сетки
  gridThicknessActionGroup = new QActionGroup(this);
  ui->gridThinAction->setData(QVariant(THIN));
  gridThicknessActionGroup->addAction(ui->gridThinAction);
  ui->gridThickAction->setData(QVariant(THICK));
  gridThicknessActionGroup->addAction(ui->gridThickAction);
  ui->gridThinAction->setChecked(true);
  connect(gridThicknessActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(GridThicknessTriggered()));

  // Группа цвета сетки
  gridColorActionGroup = new QActionGroup(this);
  ui->gridColorBlackAction->setData(QVariant(COLOR_BLACK));
  gridColorActionGroup->addAction(ui->gridColorBlackAction);
  ui->gridColorGreenAction->setData(QVariant(COLOR_GREEN));
  gridColorActionGroup->addAction(ui->gridColorGreenAction);
  ui->gridColorBlackAction->setChecked(true);
  connect(gridColorActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(GridColorTriggered()));

  // Группа типа системы координат
  coordinatesTypeActionGroup = new QActionGroup(this);
  ui->actionSetCoordinatesInMeters->setData(QVariant(PLANEPOINT));
  coordinatesTypeActionGroup->addAction(ui->actionSetCoordinatesInMeters);
  ui->actionSetCoordinatesInRadians->setData(QVariant(GEORADWGS84));
  coordinatesTypeActionGroup->addAction(ui->actionSetCoordinatesInRadians);
  ui->actionSetCoordinatesInDegrees->setData(QVariant(GEOGRADWGS84));
  coordinatesTypeActionGroup->addAction(ui->actionSetCoordinatesInDegrees);
  ui->actionSetCoordinatesInDegreesMinutesSeconds->setData(QVariant(GEOGRADMINWGS84));
  coordinatesTypeActionGroup->addAction(ui->actionSetCoordinatesInDegreesMinutesSeconds);
  connect(coordinatesTypeActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(CoordinatesTypeTriggered()));
}

void MainWindow::SaveMapViewParams()
{
  GRIDPARMEX gridParmEx;
  memset(&gridParmEx, 0, sizeof(gridParmEx));
  mapGetGridParmEx(MapHandle, &gridParmEx);

  OriginalGridColor = gridParmEx.Image.Color;
  OriginalGridStep = gridParmEx.Step;
  OriginalGridThickness = gridParmEx.Image.Thick;
}

void MainWindow::ResetMapViewParams()
{
  OriginalGridStep = 0;
  OriginalGridColor = 0;
  OriginalGridThickness = 0;
}

void MainWindow::UpdateCurrentCoordinates(POINT pointInPicture)
{
  DOUBLEPOINT pointInPlane;
  pointInPlane.x = pointInPicture.x;
  pointInPlane.y = pointInPicture.y;
  mapPictureToPlane(MapHandle, &pointInPlane.x, &pointInPlane.y);

  DOUBLEPOINT pointInWGS84;
  pointInWGS84.x = pointInPlane.x;
  pointInWGS84.y = pointInPlane.y;
  mapPlaneToGeoWGS84(MapHandle, &pointInWGS84.x, &pointInWGS84.y);

  double height = mapGetPrecisionHeightTriangle(MapHandle, pointInPlane.x, pointInPlane.y);
  HANDLE egmMatrixHandle = 0;
  if (height > ERRORHEIGHT)
    egmMatrixHandle = mapOpenEgmPro(0, 0);

  mapSetCurrentPointFormat(MapHandle, CoordinatesType);

  WCHAR coordinatesStringWide[128];   coordinatesStringWide[0] = 0;
  mapPlaneToPointFormatStringPro(MapHandle, &pointInPlane.x, &pointInPlane.y, &height, egmMatrixHandle,
                                 coordinatesStringWide, sizeof(coordinatesStringWide));

  QString coordinatesQString = QString::fromUtf16(coordinatesStringWide);
  CurrentCoordinates->setText(coordinatesQString);
}

// -----------------------------------------------------------------------------
// Обработка нажатий клавиш
// -----------------------------------------------------------------------------
void MainWindow::keyPressEvent(QKeyEvent *e)
{
  switch(e->key())
  {
    case Qt::Key_Left:
      ShiftLeftTopCorner(-16,0);
      return;
    case Qt::Key_Up:
      ShiftLeftTopCorner(0,-16);
      return;
    case Qt::Key_Right:
      ShiftLeftTopCorner(16,0);
      return;
    case Qt::Key_Down:
      ShiftLeftTopCorner(0,16);
      return;
    case Qt::Key_Comma:
      ChangeViewScale(0.5);
      return;
    case Qt::Key_Period:
      ChangeViewScale(2);
      return;
  }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
  if (MapHandle)
  {
    long left = 0;
    long top = 0;
    GetLeftTop(left, top);

    POINT currentPointInPicture;
    currentPointInPicture.x = left + e->pos().x();
    currentPointInPicture.y = top + e->pos().y();

    UpdateCurrentCoordinates(currentPointInPicture);
  }

  QMainWindow::mouseMoveEvent(e);
}

// -----------------------------------------------------------------------------
// Настройка строки статуса
// -----------------------------------------------------------------------------
void MainWindow::polish()
{
  QWidget *statusBarWidget = new QWidget(this);
  QHBoxLayout *statusBarLayout = new QHBoxLayout(statusBarWidget);

  // текущие координаты
  CurrentCoordinates = new QLabel(this);
  statusBarLayout->addWidget(CurrentCoordinates);

  // разделитель
  QLabel *verticalDelimiter = new QLabel(this);
  verticalDelimiter->setText(" | ");
  statusBarLayout->addWidget(verticalDelimiter);

  // точность экрана
  CurrentPrecision = new QLabel(this);
  CurrentPrecision->setText(CodecKOI8R->toUnicode("Точность текущего режима экрана в точках на метр ") +
                            QString::number(mapGetScreenPrecision()));
  statusBarLayout->addWidget(CurrentPrecision);

  ui->statusBar->addWidget(statusBarWidget);
}

void MainWindow::SetLeftTopCorner(long x, long y)
{
  if (MapHandle)
  {
    long mapWidth = 0;
    long mapHeight = 0;
    mapGetPictureSize(MapHandle, &mapWidth, &mapHeight);

    if (x > mapWidth - ui->scrollArea->viewport()->width())
      x = mapWidth - ui->scrollArea->viewport()->width();
    if (y > mapHeight - ui->scrollArea->viewport()->height())
      y = mapHeight - ui->scrollArea->viewport()->height();
    if (x < 0)
      x = 0;
    if (y < 0)
      y = 0;

    ui->scrollArea->horizontalScrollBar()->setValue(x);
    ui->scrollArea->verticalScrollBar()->setValue(y);
  }
}

// -----------------------------------------------------------------------------
// Увеличить точность
// -----------------------------------------------------------------------------
void MainWindow::PrecisionInc()
{
  mapSetScreenPrecision(mapGetScreenPrecision() * 2.0);
  CurrentPrecision->setText(CodecKOI8R->toUnicode("Точность текущего режима экрана в точках на метр ") +
                            QString::number(mapGetScreenPrecision()));
}

// -----------------------------------------------------------------------------
// Уменьшить точность
// -----------------------------------------------------------------------------
void MainWindow::PrecisionDec()
{
  mapSetScreenPrecision(mapGetScreenPrecision() * 0.5);
  CurrentPrecision->setText(CodecKOI8R->toUnicode("Точность текущего режима экрана в точках на метр ") +
                            QString::number(mapGetScreenPrecision()));
}

void MainWindow::SetScale100000Triggered()
{
  if (MapHandle)
  {
    SetViewScale(100000);
    Map->update();
  }
}

void MainWindow::SetScale200000Triggered()
{
  if (MapHandle)
  {
    SetViewScale(200000);
    Map->update();
  }
}

void MainWindow::TurnOn30DegreesTriggered()
{
  if (MapHandle)
  {
    double currentTurnAngle = mapGetTurnAngle(MapHandle);
    double newTurnAngle = currentTurnAngle + 30 * M_PI/180;

    mapSetupTurn(MapHandle, newTurnAngle, 0.0);
    Map->update();
  }
}

void MainWindow::RestoreViewAfterTurningTriggered()
{
  if (MapHandle)
  {
    mapSetupTurn(MapHandle, 0.0, 0.0);
    Map->update();
  }
}

void MainWindow::MoveWindowCenterToPointTriggered()
{
  if (MapHandle)
  {
    DOUBLEPOINT newCenter;
    newCenter.x = 6210000.0;
    newCenter.y = 7465000.0;

    mapPlaneToPicture(MapHandle, &newCenter.x, &newCenter.y);

    SetCenter(newCenter.x, newCenter.y);
  }
}

void MainWindow::MoveWindowCenterToObjectTriggered()
{
  if (MapHandle)
  {
    HSELECT selectHandle = mapCreateMapSelectContext(MapHandle);

    mapSelectLocal(selectHandle, -1, 0);
    mapSelectLocal(selectHandle, LOCAL_TITLE, 1);

    long searchSemanticCode = 9;
    const char *searchSemanticValue = "НОГИНСК";
    mapSelectSemanticAppend(selectHandle, CMEQUAL, searchSemanticCode, searchSemanticValue);

    HOBJ objectHandle = mapCreateObject(MapHandle);
    if (mapSeekSelectObject(MapHandle, objectHandle, selectHandle, WO_FIRST))
    {
      DFRAME objectFrame;
      memset(&objectFrame, 0, sizeof(objectFrame));
      mapObjectFrame(objectHandle, &objectFrame);

      DOUBLEPOINT objectCenter;
      objectCenter.x = (objectFrame.X1 + objectFrame.X2) / 2;
      objectCenter.y = (objectFrame.Y1 + objectFrame.Y2) / 2;

      mapPlaneToPicture(MapHandle, &objectCenter.x, &objectCenter.y);
      SetCenter(objectCenter.x, objectCenter.y);
    }

    mapFreeObject(objectHandle);
    mapDeleteSelectContext(selectHandle);
  }
}

void MainWindow::ViewGridTriggered(bool isActive)
{
  if (MapHandle)
  {
    mapSetGridActive(MapHandle, isActive);
    Map->update();
  }
}

void MainWindow::GridStepTriggered()
{
  if (MapHandle)
  {
    GRIDPARMEX gridParmEx;
    memset(&gridParmEx, 0, sizeof(gridParmEx));

    mapGetGridParmEx(MapHandle, &gridParmEx);

    QAction *checkedAction = gridStepActionGroup->checkedAction();
    if (checkedAction->data().toInt() == STEP_LITTLE)
      gridParmEx.Step = OriginalGridStep;
    else
      gridParmEx.Step = OriginalGridStep * 4;

    mapSetGridParmEx(MapHandle, &gridParmEx);

    Map->update();
  }
}

void MainWindow::GridThicknessTriggered()
{
  if (MapHandle)
  {
    GRIDPARMEX gridParmEx;
    memset(&gridParmEx, 0, sizeof(gridParmEx));

    mapGetGridParmEx(MapHandle, &gridParmEx);

    QAction *checkedAction = gridThicknessActionGroup->checkedAction();
    if (checkedAction->data().toInt() == THIN)
      gridParmEx.Image.Thick = OriginalGridThickness;
    else
      gridParmEx.Image.Thick = OriginalGridThickness * 4;

    mapSetGridParmEx(MapHandle, &gridParmEx);

    Map->update();
  }
}

void MainWindow::CoordinatesTypeTriggered()
{
  QAction *checkedAction = coordinatesTypeActionGroup->checkedAction();
  CoordinatesType = checkedAction->data().toInt();
}

void MainWindow::GridColorTriggered()
{
  if (MapHandle)
  {
    GRIDPARMEX gridParmEx;
    memset(&gridParmEx, 0, sizeof(gridParmEx));

    mapGetGridParmEx(MapHandle, &gridParmEx);

    QAction *checkedAction = gridColorActionGroup->checkedAction();
    if (checkedAction->data().toInt() == COLOR_BLACK)
      gridParmEx.Image.Color = OriginalGridColor;
    else
      gridParmEx.Image.Color = 0x0000FF00;

    mapSetGridParmEx(MapHandle, &gridParmEx);

    Map->update();
  }
}

// -----------------------------------------------------------------------------
// Системный MessageBox
// -----------------------------------------------------------------------------
// Функция обратного вызова MessageBox для libmapacces.so
//   hWnd    - идентификатор владельца окна
//   message - Message
//   caption - Caption
// Exit
//   такие коды возврата надо отдавать в libacces
//   IDOK      =  1;
//   IDCANCEL  =  2;
//   IDABORT   =  3;
//   IDRETRY   =  4;
//   IDIGNORE  =  5;
//   IDYES     =  6;
//   IDNO      =  7;
//   IDCLOSE   =  8; этого нет
//   IDHELP    =  9; этого нет
// -----------------------------------------------------------------------------
long int SystemMessageBox(void * parm, HWND hWnd, const WCHAR *message,
                       const WCHAR *caption, long int flag)
{
  int ident;
  int buttoncount = 1;
  int ret = 0;
  int retmessage = 0;
  char * button_OK_RU[] = {"Да"};
  char * button_OKCANCEL_RU[] = {"Продолжить","Отменить"};
  char * button_ABORTRETRYIGNORE_RU[] = {"Прервать","Повторить","Пропустить"};
  char * button_YESNOCANCEL_RU[] = {"Да","Нет","Отменить"};
  char * button_YESNO_RU[] = {"Да","Нет"};
  char * button_RETRYCANCEL_RU[] = {"Повторить","Отменить"};
  char * button_OK_EN[] = {"Ok"};
  char * button_OKCANCEL_EN[] = {"Ok","Cancel"};
  char * button_ABORTRETRYIGNORE_EN[] = {"Abort","Retry","Ignore"};
  char * button_YESNOCANCEL_EN[] = {"Yes","No","Cancel"};
  char * button_YESNO_EN[] = {"Yes","No"};
  char * button_RETRYCANCEL_EN[] = {"Retry","Cancel"};
  char ** name_button = button_OK_RU;
  int  maxcountbut = 3;

  QString *qnamebut[maxcountbut];
  QString qcaption;
  QString qmessage;
  QTextCodec *codec = QTextCodec::codecForName("KOI8-R");

  if (caption)
    qcaption = QString::fromUtf16(caption);

  if (message)
    qmessage = QString::fromUtf16(message);

  for (int i = 0; i < maxcountbut; i++)
    qnamebut[i] = 0;

  // Получаем типы кнопок
  ident = flag & 0x7;

  switch (ident)
  {
    case MB_OK :
      name_button = button_OK_RU;
      if (mapGetMapAccessLanguage() == ML_ENGLISH)
        name_button = button_OK_EN;
      buttoncount = 1;
      break;
    case MB_OKCANCEL :
      name_button = button_OKCANCEL_RU;
      if (mapGetMapAccessLanguage() == ML_ENGLISH)
        name_button = button_OKCANCEL_EN;
      buttoncount = 2;
      break;
    case MB_YESNOCANCEL :
      name_button = button_YESNOCANCEL_RU;
      if (mapGetMapAccessLanguage() == ML_ENGLISH)
        name_button = button_YESNOCANCEL_EN;
      buttoncount = 3;
      break;
    case MB_YESNO :
      name_button = button_YESNO_RU;
      if (mapGetMapAccessLanguage() == ML_ENGLISH)
        name_button = button_YESNO_EN;
      buttoncount = 2;
      break;
    case MB_ABORTRETRYIGNORE :
      name_button = button_ABORTRETRYIGNORE_RU;
      if (mapGetMapAccessLanguage() == ML_ENGLISH)
        name_button = button_ABORTRETRYIGNORE_EN;
      buttoncount = 3;
      break;
    case MB_RETRYCANCEL :
      name_button = button_RETRYCANCEL_RU;
      if (mapGetMapAccessLanguage() == ML_ENGLISH)
        name_button = button_RETRYCANCEL_EN;
      buttoncount = 2;
      break;
  }

  for (int i = 0; i < buttoncount; i++)
  {
    if (codec)
      qnamebut[i] = new QString(codec->toUnicode(name_button[i]));
    else
      qnamebut[i] = new QString(QString::fromLocal8Bit(name_button[i]));
  }

  // Получаем типы иконок
  int identicon = flag & 0x70;
  int icon = QMessageBox::Warning;
  switch (identicon)
  {
    case MB_ICONERROR:
      icon = QMessageBox::Critical;
      break;
    case MB_ICONQUESTION:
      icon = QMessageBox::Question;
      break;
    case MB_ICONWARNING:
      icon = QMessageBox::Warning;
      break;
    case MB_ICONINFORMATION:
      icon = QMessageBox::Information;
      break;
    default:
      icon = QMessageBox::Warning;
      break;
  }

  QMessageBox *messbox = 0;
  switch (ident)
  {
    case MB_OK :
      messbox = new QMessageBox(qcaption, qmessage, (QMessageBox::Icon)icon,
                                QMessageBox::Ok | QMessageBox::Default,
                                QMessageBox::NoButton, QMessageBox::NoButton, 0);
      messbox->setButtonText(QMessageBox::Ok, *qnamebut[0]);
      retmessage = messbox->exec();
      break;
    case MB_OKCANCEL :
    case MB_YESNO :
    case MB_RETRYCANCEL :
      messbox = new QMessageBox(qcaption, qmessage, (QMessageBox::Icon)icon,
                                QMessageBox::Yes | QMessageBox::Default,
                                QMessageBox::No | QMessageBox::Escape,
                                QMessageBox::NoButton, 0);
      messbox->setButtonText(QMessageBox::Yes, *qnamebut[0]);
      messbox->setButtonText(QMessageBox::No, *qnamebut[1]);
      retmessage = messbox->exec();
      break;
    case MB_YESNOCANCEL :
    case MB_ABORTRETRYIGNORE :
      messbox = new QMessageBox(qcaption, qmessage, (QMessageBox::Icon)icon,
                                QMessageBox::Yes | QMessageBox::Default,
                                QMessageBox::No, QMessageBox::Cancel |
                                QMessageBox::Escape, 0);
      messbox->setButtonText(QMessageBox::Yes, *qnamebut[0]);
      messbox->setButtonText(QMessageBox::No, *qnamebut[1]);
      messbox->setButtonText(QMessageBox::Cancel, *qnamebut[2]);
      retmessage = messbox->exec();
      break;
  }

  for (int i = 0; i < buttoncount; i++)
    delete qnamebut[i];

  switch (ident)
  {
    case MB_OK :
      ret = IDOK;
      break;
    case MB_OKCANCEL :
      switch (retmessage)
      {
        case QMessageBox::Yes:
          ret = IDOK;
          break;
        case QMessageBox::No:
          ret = IDCANCEL;
          break;
      }
      break;
    case MB_YESNOCANCEL :
      switch (retmessage)
      {
        case QMessageBox::Yes:
          ret = IDYES;
          break;
        case QMessageBox::No:
          ret = IDNO;
          break;
        case QMessageBox::Cancel:
          ret = IDCANCEL;
          break;
      }
      break;
    case MB_YESNO :
      switch (retmessage)
      {
        case QMessageBox::Yes:
          ret = IDYES;
          break;
        case QMessageBox::No:
          ret = IDNO;
          break;
      }
      break;
    case MB_ABORTRETRYIGNORE :
      switch (retmessage)
      {
        case QMessageBox::Yes:
          ret = IDABORT;
          break;
        case QMessageBox::No:
          ret = IDRETRY;
          break;
        case QMessageBox::Cancel:
          ret = IDIGNORE;
          break;
      }
      break;
    case MB_RETRYCANCEL :
      switch (retmessage)
      {
        case QMessageBox::Yes:
          ret = IDRETRY;
          break;
        case QMessageBox::No:
          ret = IDCANCEL;
          break;
      }
      break;

  }

  return ret;
}
