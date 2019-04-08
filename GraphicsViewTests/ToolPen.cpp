#include "ToolPen.h"

#include "Image.UtilitiesMultiThreaded.h"
#include "Image.ToolPaintUtilities.h"
#include "Blending.h"
#include "GPUForThisApp.h"
#include "ImageDebugger.h"
#include "Image.LineSimplification.h"

#include <time.h>

cToolPen::~cToolPen()
{
}


cToolPen::cToolPen( QObject * iParent ) :
    cToolStamp( iParent )
{
    // Some debug values to work with
    mToolSize = 150;
    mColor = Qt::green;
    mStep = 0.1;
    mOpacity = 1.F;

    buildTool();
}


Qt::ItemFlags
cToolPen::flags( const QModelIndex & iIndex ) const
{
    return  cPaintToolBase::flags( iIndex) | Qt::ItemIsEditable;
}


void
cToolPen::buildTool()
{
    srand( time( NULL ) );

    mProfile.SetValueAtTime( 0.0, 0.0 );
    mProfile.SetValueAtTime( 0.5, 1.0 );
    mProfile.SetValueAtTime( 1.0, 1.0 );
}


void
cToolPen::RenderTip( int iX, int iY )
{
    float* data = mTipRenderedF;
    float* pixelRow = data;
    unsigned int width = mToolSize * 2;
    unsigned int height = mToolSize * 2;

    const float originA = mColor.alpha()  * mOpacity;
    float finalA = originA;

    const int bytesPerLine = width;
    const int r = mToolSize;

    int bboxMinX = iX - r;
    int bboxMaxX = iX + r;
    int bboxMinY = iY - r;
    int bboxMaxY = iY + r;

    const float radiusSq = r*r;

    bboxMinX = bboxMinX < 0 ? 0 : bboxMinX;
    bboxMaxX = bboxMaxX >= width ? width - 1 : bboxMaxX;

    bboxMinY = bboxMinY < 0 ? 0 : bboxMinY;
    bboxMaxY = bboxMaxY >= height ? height - 1 : bboxMaxY;

    const unsigned int iterationCount = (bboxMaxX - bboxMinX) * (bboxMaxY - bboxMinY);


    for( int y = bboxMinY; y <= bboxMaxY ; ++y )
    {
        pixelRow = data + y * bytesPerLine + bboxMinX;

        for( int x = bboxMinX; x <= bboxMaxX ; ++x )
        {
            const int dx = x - iX;
            const int dy = y - iY;
            if( dx * dx + dy * dy <= radiusSq )
            {
                const float rnd = float(rand() % 101) / 100.F;
                *pixelRow = finalA * rnd; ++pixelRow;
            }
            else
            {
                ++pixelRow;
            }
        }
    }
}






