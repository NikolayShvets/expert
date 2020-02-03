#include <QDebug>
#include "mapwidget.h"

// ���������� ���, ���������� �� ���� ����� ����������� �����
const int MAPDEPTH = 32;
const int BMCELLSIZE = MAPDEPTH / 8;

// -----------------------------------------------------------------------------
// ����� ����������� �������� �����
// -----------------------------------------------------------------------------
// �����������
// -----------------------------------------------------------------------------
MapWidget::MapWidget(QWidget *parent) : QWidget(parent), MapHandle(0), BitMap(0),
    WidthBM(0), HeightBM(0), BitMapSize(0)
{
}

// -----------------------------------------------------------------------------
// ����������
// -----------------------------------------------------------------------------
MapWidget::~MapWidget()
{
  if (BitMap)
    FreeTheMemory(BitMap);
}

// -----------------------------------------------------------------------------
// ��������� ��������� �� �������� �����
// -----------------------------------------------------------------------------
void MapWidget::SetMapHandle(HMAP mapHandle)
{
  if (MapHandle != mapHandle)
    MapHandle = mapHandle;
}

// -----------------------------------------------------------------------------
// ������� �����������
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
// ��������� �������� ������ ������� ����������� � ���� ���������� ������������ ������
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
// ����������� ����
// -----------------------------------------------------------------------------
void MapWidget::DrawMap(QPainter* p, int cx, int cy, int cw, int ch)
{
  if (MapHandle)
  {
    // ������� ��������� �����
    RECT rectDraw;
    rectDraw.left = cx;
    rectDraw.top = cy;
    rectDraw.right = cx + cw;
    rectDraw.bottom = cy + ch;

    if (BitMap)
    {
      memset(BitMap, 0x0, BitMapSize);

      XIMAGEDESC xImageDesc;
      xImageDesc.Point = BitMap;                     // ����� ������ ������� ��������
      xImageDesc.Width = cw;                         // ������ ������ � ��������
      xImageDesc.Height = ch;                        // ����� �����
      xImageDesc.Depth = MAPDEPTH;                   // ������ �������� � ����� (8,15,16,24,32)
      xImageDesc.CellSize = BMCELLSIZE;              // ������ ��������(�������) � ������
      xImageDesc.RowSize = cw * xImageDesc.CellSize; // ������ ������ � ������

      // ��������� �������� ����� �� �����
      if (mapPaintToXImage(MapHandle, &xImageDesc, 0, 0, &rectDraw))
      {
        QImage image((uchar *)BitMap, cw, ch, QImage::Format_RGB32);
        p->drawImage(cx, cy, image, 0, 0, cw, ch);
      }
    }
  }
}
