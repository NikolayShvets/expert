#include <QDebug>
#include "mapwidget.h"

// Количество бит, выделяемое на одну точку изображения карты
const int MAPDEPTH = 32;
const int BMCELLSIZE = MAPDEPTH / 8;

// -----------------------------------------------------------------------------
// Класс отображения открытой карты
// -----------------------------------------------------------------------------
// Конструктор
// -----------------------------------------------------------------------------
MapWidget::MapWidget(QWidget *parent) : QWidget(parent), MapHandle(0), BitMap(0),
    WidthBM(0), HeightBM(0), BitMapSize(0)
{
}

// -----------------------------------------------------------------------------
// Деструктор
// -----------------------------------------------------------------------------
MapWidget::~MapWidget()
{
  if (BitMap)
    FreeTheMemory(BitMap);
}

// -----------------------------------------------------------------------------
// Установка указателя на открытую карту
// -----------------------------------------------------------------------------
void MapWidget::SetMapHandle(HMAP mapHandle)
{
  if (MapHandle != mapHandle)
    MapHandle = mapHandle;
}

// -----------------------------------------------------------------------------
// Событие перерисовки
// -----------------------------------------------------------------------------
void MapWidget::paintEvent(QPaintEvent *event)
{
  if (MapHandle != 0)
  {
    int x = event->rect().left();
    int y = event->rect().top();
    int width= event->rect().right() - event->rect().left() + 1;
    int height = event->rect().bottom() - event->rect().top() + 1;
    if (CheckMemoryNeeded(width, height))
    {
      WidthBM = width;
      HeightBM = height;
    }

    QPainter p;
    p.begin(this);
    DrawMap(&p, x, y, width, height);
    p.end();
  }
}

// -----------------------------------------------------------------------------
// Проверяем габариты нового участка изображения и если необходимо перевыделяем память
// -----------------------------------------------------------------------------
bool MapWidget::CheckMemoryNeeded(int width, int height)
{
  if (width > WidthBM || height > HeightBM || (width > WidthBM && height > HeightBM))
  {
    size_t size = width * BMCELLSIZE * height;
    if (size > BitMapSize)
    {
      if (BitMap)
        FreeTheMemory(BitMap);
      BitMap = AllocateTheMemory(size);
      if (BitMap)
      {
        BitMapSize = size;
        return true;
      }
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
// Перерисовка окна
// -----------------------------------------------------------------------------
void MapWidget::DrawMap(QPainter* p, int cx, int cy, int cw, int ch)
{
  if (MapHandle)
  {
    // Область отрисовки карты
    RECT rectDraw;
    rectDraw.left = cx;
    rectDraw.top = cy;
    rectDraw.right = cx + cw;
    rectDraw.bottom = cy + ch;

    if (BitMap)
    {
      memset(BitMap, 0x0, BitMapSize);

      XIMAGEDESC xImageDesc;
      xImageDesc.Point = BitMap;                     // Адрес начала области пикселов
      xImageDesc.Width = cw;                         // Ширина строки в пикселах
      xImageDesc.Height = ch;                        // Число строк
      xImageDesc.Depth = MAPDEPTH;                   // Размер элемента в битах (8,15,16,24,32)
      xImageDesc.CellSize = BMCELLSIZE;              // Размер элемента(пиксела) в байтах
      xImageDesc.RowSize = cw * xImageDesc.CellSize; // Ширина строки в байтах

      // Отобразим фрагмент карты на экран
      if (mapPaintToXImage(MapHandle, &xImageDesc, 0, 0, &rectDraw))
      {
        QImage image((uchar *)BitMap, cw, ch, QImage::Format_RGB32);
        p->drawImage(cx, cy, image, 0, 0, cw, ch);
      }
    }
  }
}
