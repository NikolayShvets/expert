#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QActionGroup>
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QTextCodec>
#include <QScrollBar>

#include "mapapi.h"
#include "mapwidget.h"
//#include "connectionwindow.h"
#include "dialog.h"

namespace Ui {
class MainWindow;
}

// -----------------------------------------------------------------------------
// Класс открытия карты и управления ресурсами
// -----------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  // Конструктор
  explicit MainWindow(QWidget *parent = 0);

  // Деструктор
  ~MainWindow();

  // Собственная обработка собыйти
  bool event(QEvent *event);

private:

  // Интерфейс пользователя
  Ui::MainWindow *ui;
  Dialog *dw;

  // Перекодировщик текстов
  QTextCodec * CodecKOI8R;

public slots:

  // Открытие карты
  void OpenMap();

  // Закрытие карты
  void CloseMap();

  // Установить ограничение на объем используемой памяти
  void SetMemoryLimit();

  // Установить ограничение на количество одновремено открытых листов
  void SetListCountLimit();

  // Увеличить точность
  void PrecisionInc();

  // Уменьшить точность
  void PrecisionDec();

  // Установить масштаб 1:100 000 / 1:200 000
  void SetScale100000Triggered();
  void SetScale200000Triggered();

  // Повернуть карту на 30 градусов
  void TurnOn30DegreesTriggered();
  void RestoreViewAfterTurningTriggered();

  // Переместить центр окна в заданную точку/центр объекта
  void MoveWindowCenterToPointTriggered();
  void MoveWindowCenterToObjectTriggered();

  void ViewGridTriggered(bool isActive);
  void GridColorTriggered();
  void GridStepTriggered();
  void GridThicknessTriggered();

  // Изменить тип выводимых координат
  void CoordinatesTypeTriggered();

  // Создание сценария
  void CreateScript();

protected:

  virtual void keyPressEvent(QKeyEvent *e);
  virtual void mouseMoveEvent(QMouseEvent *e);

private:

  enum GRID_STEP
  {
    STEP_LITTLE = 0,
    STEP_BIG = 1
  };

  enum GRID_COLOR
  {
    COLOR_BLACK = 0,
    COLOR_GREEN = 1
  };

  enum GRID_THICKNESS
  {
    THIN = 0,
    THICK = 1
  };

  void polish();

  // Перемещение/сдвиг карты
  void SetLeftTopCorner(long x, long y);
  void ShiftLeftTopCorner(long dx, long dy);
  void GetLeftTop(long &x, long &y);

  void SetCenter(long x, long y);
  void GetCenter(long &x, long &y);

  // Изменение масштаба
  void ChangeViewScale(float change);
  void SetViewScale(float scale);

  void CreateMenuActionGroups();

  void SaveMapViewParams();
  void ResetMapViewParams();

  void UpdateCurrentCoordinates(POINT pointInPicture);

  QActionGroup *gridColorActionGroup;
  QActionGroup *gridStepActionGroup;
  QActionGroup *gridThicknessActionGroup;
  QActionGroup *coordinatesTypeActionGroup;

  // Виджет для помещения в строку статуса
  QLabel *CurrentCoordinates;
  QLabel *CurrentPrecision;

  // Указатель на открытую карту
  HMAP MapHandle;

  // Объект отображения открытой карты на экран
  MapWidget * Map;

  long int OriginalGridColor;
  long int OriginalGridStep;
  long int OriginalGridThickness;

  int CoordinatesType;
};

#endif // MAINWINDOW_H
