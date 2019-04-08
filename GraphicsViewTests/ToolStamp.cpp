#include "ToolStamp.h"

#include <QDebug>

#include "BenchmarkStuff.h"
#include "Image.Utilities.h"
#include "Image.UtilitiesMultiThreaded.h"
#include "Image.ToolPaintUtilities.h"
#include "Blending.h"
#include "GPUForThisApp.h"
#include "ImageDebugger.h"
#include "Image.LineSimplification.h"


cToolStamp::~cToolStamp()
{
}


cToolStamp::cToolStamp( QObject * iParent ) :
    cPaintToolBase( iParent )
{
}


Qt::ItemFlags
cToolStamp::flags( const QModelIndex & iIndex ) const
{
    return  cPaintToolBase::flags( iIndex) | Qt::ItemIsEditable;
}


int
cToolStamp::getSize() const
{
    return  cPaintToolBase::getSize();
}


void
cToolStamp::setSize( int iSize )
{
    cPaintToolBase::setSize( iSize );
    PrepareTool();
}


QColor
cToolStamp::getColor() const
{
    return  cPaintToolBase::getColor();
}


void
cToolStamp::setColor( const QColor & iColor )
{
    cPaintToolBase::setColor( iColor );
    PrepareTool();
}


float
cToolStamp::getOpacity() const
{
    return  cPaintToolBase::getOpacity();
}


void
cToolStamp::setOpacity( float iOpacity )
{
    cPaintToolBase::setOpacity( iOpacity );
    PrepareTool();
}


void
cToolStamp::StartDrawing( QImage* iImage, sPointData iPointData )
{
    cPaintToolBase::StartDrawing( iImage, iPointData );

    const int imageSize = iImage->bytesPerLine() * iImage->height();

    // Initialize DryBuffer
    delete[] mDryBuffer;
    mDryBuffer = new float[ imageSize ];
    float* data = _mFloatBuffer;
    float* scan = data;

    for( int y = 0; y < iImage->height(); ++y )
    {
        scan = data + y * iImage->bytesPerLine();
        const int floatIndex = y * iImage->width() * 4;

        for( int x = 0; x < iImage->width() * 4; ++x )
        {
            mDryBuffer[ floatIndex + x ] = *scan; ++scan;
        }
    }

    // Initialize StampBuffer
    delete[] mStampBuffer;
    mStampBuffer = new float[ iImage->width() * iImage->height() ];
    memset( mStampBuffer, 0, sizeof(float) * iImage->width() * iImage->height() );

    PrepareTool();

    mDirtyArea = QRect( 0, 0, 0, 0 );
}


QRect
cToolStamp::MoveDrawing( sPointData iPointData )
{
    PathAddPoint( iPointData );
    DrawPathFromLastRenderedPoint();

    return  GetDirtyAreaAndReset();
}


void
cToolStamp::DrawDot( float iX, float iY, float iPressure, float iRotation )
{
    const int baseDiameter = mToolSize * 2;
    const float radius = mToolSize * iPressure;
    const float diam = radius*2;

    const float scale = diam / baseDiameter;

    const float minX = iX - radius;
    const float minY = iY - radius;
    const float maxX = minX + diam;
    const float maxY = minY + diam;

    const float red = mColor.red();
    const float green = mColor.green();
    const float blue = mColor.blue();

    // Basic out of bounds elimination
    if( minX >= mDrawingContext->width() || minY >= mDrawingContext->height() )
        return;
    if( maxX < 0 || maxY < 0 )
        return;

    int startingX = int( minX ) < 0 ? 0 : int( minX );
    int endingX = int( maxX + 1 ) >= mDrawingContext->width() ? mDrawingContext->width() - 1 : int( maxX + 1 );
    int startingY = int( minY ) < 0 ? 0 : int( minY );
    int endingY = int( maxY + 1 ) >= mDrawingContext->height() ? mDrawingContext->height() - 1 : int( maxY + 1 );

    auto trans = QTransform();
    int indexMip = std::min( log2( 1/scale ), float( mMipMapF.count() - 1 ) );
    float startingScale = indexMip == 0 ? 1 : std::pow( 0.5, indexMip );
    float remainingScale = scale / startingScale;

    const int mipMapSizeAtIndex = baseDiameter * startingScale;

    trans.scale( remainingScale, remainingScale );

    auto transfo = QTransform() * trans * QTransform::fromTranslate( minX, minY );

    const int minXI = int( minX );
    const int minYI = int( minY );
    QPointF subPixelOffset( minXI - minX, minYI - minY ); // Invert the sign to have the offset from the source image perspective

    MTDownscaleBoxAverageDirectAlphaFDry( mMipMapF[ indexMip ], mipMapSizeAtIndex, mipMapSizeAtIndex,
                                          mDryBuffer, mDrawingContext->bytesPerLine()/4, mDrawingContext->height(),
                                          mStampBuffer,
                                          _mFloatBuffer,
                                          mDrawingContext,
                                          mAlphaMask, transfo, QPoint( 0, 0 ), subPixelOffset, mOpacity,
                                          red, green, blue );

    mDirtyArea = mDirtyArea.united( QRect( startingX, startingY, endingX - startingX + 1, endingY - startingY + 1 ) );
}


void
cToolStamp::DrawLine( const QPointF& iP1, const QPointF& iP2, float iPressure1, float iRotation1, float iPressure2, float iRotation2 )
{
}


QRect
cToolStamp::EndDrawing( sPointData iPointData )
{
    if( Vectorial() )
    {
        mPath = SimplifyLine( mPath, 20000000, 0.1 );
        DrawFullPath();
    }

    return  cPaintToolBase::EndDrawing( iPointData );
}


void
cToolStamp::CancelDrawing()
{
    float* scan = mDryBuffer;
    float* scanFloat = _mFloatBuffer;
    uchar* dcScan = mDrawingContext->bits();

    // Resetting float buffer and output image
    for( int y = 0; y < mDrawingContext->height(); ++y )
    {
        for( int x = 0; x < mDrawingContext->width() * 4; ++x )
        {
            *scanFloat = *scan; ++scanFloat;
            *dcScan = uchar( *scan ); ++scan; ++dcScan;
        }
    }

    // Initialize StampBuffer
    memset( mStampBuffer, 0, sizeof(float) * mDrawingContext->width() * mDrawingContext->height() );

    mDirtyArea = mDrawingContext->rect();
}


void
cToolStamp::PrepareTool()
{
    const int toolDiameter = mToolSize * 2;
    const int bufferSize = toolDiameter * toolDiameter;

    QRect rect( 0, 0, toolDiameter, toolDiameter );
    delete  mTipRenderedF;
    mTipRenderedF = new float[ bufferSize ];
    memset( mTipRenderedF, 0, sizeof(float) * bufferSize );

    RenderTip( mToolSize, mToolSize );
    _BuildMipMap();
}


void
cToolStamp::_BuildMipMap()
{
    for( auto* image : mMipMapF )
    {
        delete[] image;
    }

    mMipMapF.clear();

    //QVector< int > ws;
    //QVector< int > hs;

    int width = mToolSize*2;
    int height = mToolSize*2;
    QTransform t = QTransform::fromScale( 0.5, 0.5 );

    float* tipRenderedCopy = new float[ width * height ];
    memcpy( tipRenderedCopy, mTipRenderedF, sizeof( float ) * width * height );

    mMipMapF.push_back( tipRenderedCopy );
    //ws.push_back( width );
    //hs.push_back( height );

    while( width > 1 && height > 1 )
    {
        mMipMapF.push_back( DownscaleBoxAverageIntoImageFGray( mMipMapF.last(), width, height, t, &width, &height ) );
        //ws.push_back( width );
        //hs.push_back( height );
    }

    //IMAGEDEBUG->ShowImagesGray( mMipMapF, ws, hs );
}




