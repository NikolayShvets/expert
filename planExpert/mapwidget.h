#ifndef SHOWMAP_H
#define SHOWMAP_H

#include <QPaintEvent>
#include <QPainter>
#include <QWidget>

#include "mapapi.h"

// -----------------------------------------------------------------------------
// Класс отображения открытой карты
// -----------------------------------------------------------------------------
class MapWidget : public QWidget
{
  Q_OBJECT

public:

  // Конструктор
  explicit MapWidget(QWidget *parent = 0);

  // Деструктор
  ~MapWidget();

  // Установка указателя на открытую карту
  void SetMapHandle(HMAP mapHandle);

protected:

  // Событие перерисовки
  virtual void paintEvent(QPaintEvent *event);

private:

  // Перевыделяет память если необходимо
  bool CheckMemoryNeeded(int width, int height);

  // Перерисовка окна
  void DrawMap(QPainter *p, int cx, int cy, int cw, int ch);

  // Указатель на открытую карту
  HMAP MapHandle;

  // Область памяти в которую происходит рисование карты
  char * BitMap;
  size_t BitMapSize;
  long WidthBM, HeightBM;
};

#endif // SHOWMAP_H
