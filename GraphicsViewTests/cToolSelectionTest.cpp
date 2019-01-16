#include "cToolSelectionTest.h"


cToolSelectionTest::~cToolSelectionTest()
{
}


cToolSelectionTest::cToolSelectionTest( QObject* iParent ) :
    ToolBase( iParent )
{
    mColor = Qt::black;
    mToolSize = 10;
    mStep = 4.0F;
}


Qt::ItemFlags
cToolSelectionTest::flags( const QModelIndex & iIndex ) const
{
    return  ToolBase::flags( iIndex) | Qt::ItemIsEditable;
}


void
cToolSelectionTest::StartDrawing()
{
    ToolBase::StartDrawing();
    mTheSelection->SetActive( true );
}


void
cToolSelectionTest::DrawDot( QImage * iImage, int iX, int iY, float iPressure, float iRotation )
{
    uchar* data = mTheSelection->GetSelectionMask()->bits();
    unsigned int width = iImage->width();
    unsigned int height = iImage->height();

    int r = (mToolSize/2) * iPressure;
    for( int dy = -r; dy <= r; ++dy)
    {
        for( int dx = -r; dx <= r; ++dx )
        {
            if( dx * dx + dy * dy <= r * r )
            {
                float mult = 1.0F;

                // Color is premultiplied here with Qt, so gotta apply to all colors
                _DrawPixel( data, width, height, iX + dx, iY + dy, 0, 0, 0, 255 );
            }
        }
    }
}


void
cToolSelectionTest::DrawLine( QImage * iImage, int x1, int y1, int x2, int y2 )
{
    // nothing
}


void
cToolSelectionTest::SetSelection( cSelection * iSelection )
{
    mTheSelection = iSelection;
}




