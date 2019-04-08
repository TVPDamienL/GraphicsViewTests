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

    // Represents the amount of random
    mProfile.SetValueAtTime( 0.0, 100.0 );
    mProfile.SetValueAtTime( 1.0, 0.0 );
}


void
cToolPen::RenderTips( int iX, int iY )
{
    for( int i = 0; i < 5; ++i )
    {
        int bufferSize =  mDrawingContext->width() * mDrawingContext->height();
        float* tipRendered = new float[ bufferSize ];
        memset( tipRendered, 0, sizeof(float) * bufferSize );

        float* data = tipRendered;
        float* pixelRow = data;
        unsigned int width = mToolSize * 2;
        unsigned int height = mToolSize * 2;

        const float originA = mColor.alpha()  * mOpacity;

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
                    float distance = Distance2PointsSquared( QPoint( iX, iY ), QPoint( iX + dx, iY + dy ) );
                    float distanceParam = 1.0F - ( distance / radiusSq ); // 1 - distanceRatio so it goes outwards, otherwise, it's a reversed gradient
                    float val = mProfile.GetValueAtTime( distanceParam );

                    float alphaToBlend = originA;
                    if( ( rand() % 100 ) < int(val) ) // If doRandom
                    {
                        const float rnd = float(rand() % 101) / 100.F;
                        alphaToBlend *= rnd * 0.01;
                    }

                    *pixelRow = alphaToBlend; ++pixelRow;
                }
                else
                {
                    ++pixelRow;
                }
            }
        }

        AddTip( tipRendered );
    }
}






