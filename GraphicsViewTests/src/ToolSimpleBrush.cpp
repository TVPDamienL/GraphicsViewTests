#include "ToolSimpleBrush.h"

#include <QDebug>

#include "BenchmarkStuff.h"
#include "Image.Utilities.h"
#include "Image.UtilitiesMultiThreaded.h"
#include "Image.ToolPaintUtilities.h"
#include "Blending.h"
#include "GPUForThisApp.h"
#include "ImageDebugger.h"
#include "Image.LineSimplification.h"


cToolSimpleBrush::~cToolSimpleBrush()
{
}


cToolSimpleBrush::cToolSimpleBrush( QObject * iParent ) :
    cToolStamp( iParent )
{
    // Some debug values to work with
    mToolSize = 10;
    mColor = Qt::red;
    mStep = 1.0;
    mStep = 0.1;
    mOpacity = 0.1F;
    mOpacity = 1.F;
    mApplyProfile = true;

    buildTool();
}


Qt::ItemFlags
cToolSimpleBrush::flags( const QModelIndex & iIndex ) const
{
    return  cPaintToolBase::flags( iIndex) | Qt::ItemIsEditable;
}


void
cToolSimpleBrush::buildTool()
{
    mProfile.SetValueAtTime( 0.0, 0.0 );
    mProfile.SetValueAtTime( 0.5, 1.0 );
    mProfile.SetValueAtTime( 1.0, 1.0 );
}


void
cToolSimpleBrush::ApplyProfile( bool iApply )
{
    mApplyProfile = iApply;
}


bool
cToolSimpleBrush::ApplyProfile() const
{
    return  mApplyProfile;
}


void
cToolSimpleBrush::RenderTips( int iX, int iY )
{
    int bufferSize =  mToolSize*2 * mToolSize*2;
    float* tipRendered = new float[ bufferSize ];
    memset( tipRendered, 0, sizeof(float) * bufferSize );

    float* pixelRow = tipRendered;
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
        pixelRow = tipRendered + y * bytesPerLine + bboxMinX;

        for( int x = bboxMinX; x <= bboxMaxX ; ++x )
        {
            const int dx = x - iX;
            const int dy = y - iY;
            if( dx * dx + dy * dy <= radiusSq )
            {
                if( mApplyProfile )
                {
                    float distance = Distance2PointsSquared( QPoint( iX, iY ), QPoint( iX + dx, iY + dy ) );
                    float distanceParam = 1.0F - ( distance / radiusSq ); // 1 - distanceRatio so it goes outwards, otherwise, it's a reversed gradient
                    float mult = mProfile.GetValueAtTime( distanceParam );
                    finalA = originA * mult;
                }

                *pixelRow = finalA ; ++pixelRow;
            }
            else
            {
                ++pixelRow;
            }
        }
    }

    AddTip( tipRendered );
}






