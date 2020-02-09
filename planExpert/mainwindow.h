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
// ����� �������� ����� � ���������� ���������
// -----------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  // �����������
  explicit MainWindow(QWidget *parent = 0);

  // ����������
  ~MainWindow();

  // ����������� ��������� �������
  bool event(QEvent *event);

private:

  // ��������� ������������
  Ui::MainWindow *ui;
  Dialog *dw;

  // �������������� �������
  QTextCodec * CodecKOI8R;

public slots:

  // �������� �����
  void OpenMap();

  // �������� �����
  void CloseMap();

  // ���������� ����������� �� ����� ������������ ������
  void SetMemoryLimit();

  // ���������� ����������� �� ���������� ����������� �������� ������
  void SetListCountLimit();

  // ��������� ��������
  void PrecisionInc();

  // ��������� ��������
  void PrecisionDec();

  // ���������� ������� 1:100 000 / 1:200 000
  void SetScale100000Triggered();
  void SetScale200000Triggered();

  // ��������� ����� �� 30 ��������
  void TurnOn30DegreesTriggered();
  void RestoreViewAfterTurningTriggered();

  // ����������� ����� ���� � �������� �����/����� �������
  void MoveWindowCenterToPointTriggered();
  void MoveWindowCenterToObjectTriggered();

  void ViewGridTriggered(bool isActive);
  void GridColorTriggered();
  void GridStepTriggered();
  void GridThicknessTriggered();

  // �������� ��� ��������� ���������
  void CoordinatesTypeTriggered();

  // �������� ��������
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

  // �����������/����� �����
  void SetLeftTopCorner(long x, long y);
  void ShiftLeftTopCorner(long dx, long dy);
  void GetLeftTop(long &x, long &y);

  void SetCenter(long x, long y);
  void GetCenter(long &x, long &y);

  // ��������� ��������
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

  // ������ ��� ��������� � ������ �������
  QLabel *CurrentCoordinates;
  QLabel *CurrentPrecision;

  // ��������� �� �������� �����
  HMAP MapHandle;

  // ������ ����������� �������� ����� �� �����
  MapWidget * Map;

  long int OriginalGridColor;
  long int OriginalGridStep;
  long int OriginalGridThickness;

  int CoordinatesType;
};

#endif // MAINWINDOW_H
