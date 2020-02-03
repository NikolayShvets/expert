#ifndef SHOWMAP_H
#define SHOWMAP_H

#include <QPaintEvent>
#include <QPainter>
#include <QWidget>

#include "mapapi.h"

// -----------------------------------------------------------------------------
// ����� ����������� �������� �����
// -----------------------------------------------------------------------------
class MapWidget : public QWidget
{
  Q_OBJECT

public:

  // �����������
  explicit MapWidget(QWidget *parent = 0);

  // ����������
  ~MapWidget();

  // ��������� ��������� �� �������� �����
  void SetMapHandle(HMAP mapHandle);

protected:

  // ������� �����������
  virtual void paintEvent(QPaintEvent *event);

private:

  // ������������ ������ ���� ����������
  bool CheckMemoryNeeded(int width, int height);

  // ����������� ����
  void DrawMap(QPainter *p, int cx, int cy, int cw, int ch);

  // ��������� �� �������� �����
  HMAP MapHandle;

  // ������� ������ � ������� ���������� ��������� �����
  char * BitMap;
  size_t BitMapSize;
  long WidthBM, HeightBM;
};

#endif // SHOWMAP_H
