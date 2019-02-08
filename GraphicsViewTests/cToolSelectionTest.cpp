#include "cToolSelectionTest.h"

#include "Blending.h"

cToolSelectionTest::~cToolSelectionTest()
{
}


cToolSelectionTest::cToolSelectionTest( QObject* iParent ) :
    cPaintToolBase( iParent )
{
    mColor = Qt::black;
    mToolSize = 50;
    mStep = 4.0F;
}


Qt::ItemFlags
cToolSelectionTest::flags( const QModelIndex & iIndex ) const
{
    return  cPaintToolBase::flags( iIndex) | Qt::ItemIsEditable;
}


void
cToolSelectionTest::StartDrawing( QImage* iImage, sPointData iPointData )
{
    cPaintToolBase::StartDrawing( iImage, iPointData );
}


QRect
cToolSelectionTest::MoveDrawing( sPointData iPointData )
{
    PathAddPoint( iPointData );
    DrawPathFromLastRenderedPoint();

    return  QRect();
}


void
cToolSelectionTest::DrawDot( int iX, int iY, float iPressure, float iRotation )
{

    uchar* data = mTheSelection->GetSelectionMask()->bits();
    uchar* pixelRow = data;
    unsigned int width = mTheSelection->GetSelectionMask()->width();
    unsigned int height = mTheSelection->GetSelectionMask()->height();
    uint8_t R = mColor.red();
    uint8_t G = mColor.green();
    uint8_t B = mColor.blue();
    uint8_t A = mColor.alpha();

    const int bytesPerLine = mTheSelection->GetSelectionMask()->bytesPerLine();

    const int r = (mToolSize/2) * iPressure;
    const int r2 = r*r;

    int bboxMinX = iX - r;
    int bboxMaxX = iX + r;
    int bboxMinY = iY - r;
    int bboxMaxY = iY + r;

    bboxMinX = bboxMinX < 0 ? 0 : bboxMinX;
    bboxMaxX = bboxMaxX >= width ? width - 1 : bboxMaxX;

    bboxMinY = bboxMinY < 0 ? 0 : bboxMinY;
    bboxMaxY = bboxMaxY >= height ? height - 1 : bboxMaxY;

    for( int y = bboxMinY; y <= bboxMaxY ; ++y )
    {
        pixelRow = data + y * bytesPerLine + bboxMinX * 4;
        const int dy = y - iY;
        const int dy2 = dy * dy;

        for( int x = bboxMinX; x <= bboxMaxX ; ++x )
        {
            const int dx = x - iX;
            if( dx * dx + dy2 <= r2 )
            {
                BlendPixelNormal( &pixelRow, R, G, B, A );
            }
            else
            {
                pixelRow += 4;
            }
        }
    }
}


void
cToolSelectionTest::DrawLine( int x1, int y1, int x2, int y2 )
{
    // nothing
}


QRect
cToolSelectionTest::EndDrawing( sPointData iPointData )
{
    mTheSelection->ProcessEdgeDetection();
    mTheSelection->ExtractPixelsFromImageToBuffer();
    mTheSelection->SetActive( true );
    mTheSelection->EmitPainted();
    return  cPaintToolBase::EndDrawing( iPointData );
}


void
cToolSelectionTest::SetSelection( cSelection * iSelection )
{
    mTheSelection = iSelection;
}




