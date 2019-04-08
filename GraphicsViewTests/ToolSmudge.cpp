#include "ToolSmudge.h"

#include <QDebug>

#include "BenchmarkStuff.h"
#include "Image.Utilities.h"
#include "Image.UtilitiesMultiThreaded.h"
#include "Blending.h"
#include "GPUForThisApp.h"


cToolSmudge::~cToolSmudge()
{
    delete  mAlphaMask;
}


cToolSmudge::cToolSmudge( QObject * iParent ) :
    cPaintToolBase( iParent )
{
    // Some debug values to work with
    mToolSize = 100;
    mColor = Qt::red;
    mStep = 0.0; // Will be clamped to 1 pixel, which is what we want

    mAlphaMask = 0;
    buildTool();
}


Qt::ItemFlags
cToolSmudge::flags( const QModelIndex & iIndex ) const
{
    return  cPaintToolBase::flags( iIndex) | Qt::ItemIsEditable;
}


void
cToolSmudge::buildTool()
{
    QStandardItem* sizeItem = new QStandardItem( "Size" );
    sizeItem->setData( mToolSize );
    setItem( 0, 0, sizeItem );
    QStandardItem* color = new QStandardItem( "Color" );
    color->setData( mColor );
    setItem( 1, 0, color );
    QStandardItem* step = new QStandardItem( "Step" );
    step->setData( mStep );
    setItem( 2, 0, step );


    mProfile.SetValueAtTime( 0.0, 0.0 );
    mProfile.SetValueAtTime( 1.0, 1.0 );
}


void
cToolSmudge::StartDrawing( QImage* iImage, sPointData iPointData )
{
    cPaintToolBase::StartDrawing( iImage, iPointData );

    delete  mAlphaMask;
    mAlphaMask = new float[ mToolSize * mToolSize ];
    _GenerateMask();
    mDirtyArea = QRect( 0, 0, 0, 0 );
}


QRect
cToolSmudge::MoveDrawing( sPointData iPointData )
{
    PathAddPoint( iPointData );
    DrawPathFromLastRenderedPoint();

    return  GetDirtyAreaAndReset();
}


void
cToolSmudge::DrawDot( float iX, float iY, float iPressure, float iRotation )
{
    if( _mFloatExtract )
    {
        _mToolSizeAfterPressure = Max( mToolSize * iPressure, 1.0F );

        uchar* data = mDrawingContext->bits();
        uchar* dataScanline = data;
        const int dataBPL = mDrawingContext->bytesPerLine();

        float* floatData = _mFloatBuffer;
        float* floatScanline = 0;
        const int floatCompCount = mDrawingContext->width() * 4;



        const float* alphaScanline = 0;
        const int alphaBPL = mToolSize;

        const float* sampleData = _mFloatExtract;
        const float* sampleScanline = 0;
        const int sampleBPL = mToolSize * 4;

        float diam = mToolSize;// * iPressure;
        float radius = diam / 2;


        const int minX = iX - radius;
        const int maxX = minX + diam;
        const int minY = iY - radius;
        const int maxY = minY + diam;

        // Basic out of bounds elimination
        if( minX >= mDrawingContext->width() || minY >= mDrawingContext->height() )
            return;
        if( maxX < 0 || maxY < 0 )
            return;

        int startingX = minX < 0 ? 0 : minX;
        int endingX = maxX >= mDrawingContext->width() ? mDrawingContext->width() - 1 : maxX;
        int startingY = minY < 0 ? 0 : minY;
        int endingY = maxY >= mDrawingContext->height() ? mDrawingContext->height() - 1 : maxY;

        const int offset = minX * 4;

        for( int y = startingY; y < endingY; ++y )
        {
            sampleScanline = sampleData + (y - minY) * sampleBPL;
            alphaScanline = mAlphaMask + (y - minY) * alphaBPL;
            dataScanline = data + y * dataBPL + offset;
            floatScanline = floatData + y * floatCompCount + offset;

            for( int x = startingX; x < endingX; ++x )
            {
                float alpha     = *alphaScanline;
                float newRed    = *(sampleScanline+2) * alpha;
                float newGreen  = *(sampleScanline+1) * alpha;
                float newBlue   = *(sampleScanline+0) * alpha;
                float newAlpha  = *(sampleScanline+3) * alpha;

                float transparencyAmountInverse = (255.F - newAlpha) / 255.F;

                *floatScanline = newBlue    + *floatScanline * transparencyAmountInverse;
                uchar b = std::round( *floatScanline );
                ++floatScanline;

                *floatScanline = newGreen   + *floatScanline * transparencyAmountInverse;
                uchar g = std::round( *floatScanline );
                ++floatScanline;

                *floatScanline = newRed     + *floatScanline * transparencyAmountInverse;
                uchar r = std::round( *floatScanline );
                ++floatScanline;

                *floatScanline = newAlpha   + *floatScanline * transparencyAmountInverse;
                uchar a = std::round( *floatScanline );
                ++floatScanline;

                BlendPixelNone( &dataScanline, r, g, b, a );

                sampleScanline += 4;
                ++alphaScanline;
            }
        }

        mDirtyArea = mDirtyArea.united( QRect( startingX, startingY, endingX - startingX + 1, endingY - startingY + 1 ) );
    }

    _SampleFromCanvas( iX, iY, mToolSize );
}


void
cToolSmudge::DrawLine( const QPointF& iP1, const QPointF& iP2, float iPressure1, float iRotation1, float iPressure2, float iRotation2 )
{
}


QRect
cToolSmudge::EndDrawing( sPointData iPointData )
{
    delete mCanvasSample;
    mCanvasSample = 0;
    delete _mFloatExtract;
    _mFloatExtract = 0;

    return  cPaintToolBase::EndDrawing( iPointData );
}


float
cToolSmudge::_GetStepInPixelValue() const
{
    return  1.0F;
}


void
cToolSmudge::_GenerateMask()
{
    float* pixelRow = mAlphaMask;

    const uint width = mToolSize;
    const uint height = mToolSize;


    const uint r = (mToolSize / 2);
    const uint r2 = r*r;

    const uint centerX = r;
    const uint centerY = r;


    for( int y = 0; y < height ; ++y )
    {
        for( int x = 0; x < width ; ++x )
        {
            const int dx = x - centerX;
            const int dy = y - centerY;
            if( dx * dx + dy * dy <= r2 )
            {
                int xpos = x*4;
                float distance = Distance2PointsSquared( QPoint( centerX, centerY ), QPoint( centerX + dx, centerY + dy ) );
                float distanceParam = 1.0F - ( distance / r2 ); // 1 - distanceRatio so it goes outwards, otherwise, it's a reversed gradient
                *pixelRow = mProfile.GetValueAtTime( distanceParam ) * mForce;
            }
            else
            {
                *pixelRow = 0.0F;
            }

            ++pixelRow;
        }
    }
}


void
cToolSmudge::_SampleFromCanvas( int ix, int iy, int size )
{
    float diam = mToolSize * 1.0;
    float radius = diam / 2;

    const int minX = ix - radius;
    const int maxX = minX + mToolSize;
    const int minY = iy - radius;
    const int maxY = minY + mToolSize;

    delete  _mFloatExtract;
    _mFloatExtract = new float[ mToolSize * mToolSize * 4 ];

    float* scanExtract = _mFloatExtract;
    int extractTotalWidth = mToolSize * 4;

    float* scanBuffer = _mFloatBuffer;
    int bufferTotalWidth = mDrawingContext->bytesPerLine();

    for( int y = minY; y < maxY; ++y )
    {
        scanExtract = _mFloatExtract + (y - minY) * extractTotalWidth;
        scanBuffer = _mFloatBuffer + y * bufferTotalWidth + minX * 4;

        for( int x = minX; x < maxX; ++x )
        {
            *scanExtract = *scanBuffer; ++scanExtract; ++scanBuffer;
            *scanExtract = *scanBuffer; ++scanExtract; ++scanBuffer;
            *scanExtract = *scanBuffer; ++scanExtract; ++scanBuffer;
            *scanExtract = *scanBuffer; ++scanExtract; ++scanBuffer;
        }
    }
}




