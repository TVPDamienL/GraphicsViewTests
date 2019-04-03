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
    cPaintToolBase( iParent )
{
    // Some debug values to work with
    mToolSize = 5;
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
    mProfile.SetValueAtTime( 0.5, 1.0 );
    mProfile.SetValueAtTime( 1.0, 1.0 );
}


int
cToolSimpleBrush::getSize() const
{
    return  cPaintToolBase::getSize();
}


void
cToolSimpleBrush::setSize( int iSize )
{
    cPaintToolBase::setSize( iSize );
    PrepareTool();
}


QColor
cToolSimpleBrush::getColor() const
{
    return  cPaintToolBase::getColor();
}


void
cToolSimpleBrush::setColor( const QColor & iColor )
{
    cPaintToolBase::setColor( iColor );
    PrepareTool();
}


float
cToolSimpleBrush::getStep() const
{
    return  cPaintToolBase::getStep();
}


void
cToolSimpleBrush::setStep( float iStep )
{
    cPaintToolBase::setStep( iStep );
}


float
cToolSimpleBrush::getOpacity() const
{
    return  cPaintToolBase::getOpacity();
}


void
cToolSimpleBrush::setOpacity( float iOpacity )
{
    cPaintToolBase::setOpacity( iOpacity );
    PrepareTool();
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
cToolSimpleBrush::StartDrawing( QImage* iImage, sPointData iPointData )
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
    mStampBuffer = new float[ imageSize ];
    QRect rect2( 0, 0, iImage->bytesPerLine()/4, iImage->height() );
    MTHardFillF( mStampBuffer, iImage->bytesPerLine()/4, iImage->height(), rect2, Qt::transparent );

    PrepareTool();

    mDirtyArea = QRect( 0, 0, 0, 0 );

    //_GPU->LoadPaintContext( mDrawingContext );
    //_GPU->LoadPaintAlpha( mAlphaMask );
    //_GPU->LoadBrushTip( mTipRendered );
}


QRect
cToolSimpleBrush::MoveDrawing( sPointData iPointData )
{
    PathAddPoint( iPointData );
    DrawPathFromLastRenderedPoint();

    return  GetDirtyAreaAndReset();
}


void
cToolSimpleBrush::DrawDot( float iX, float iY, float iPressure, float iRotation )
{
    const int baseDiameter = mToolSize * 2;
    const float radius = mToolSize * iPressure;
    const float diam = radius*2;

    const float scale = diam / baseDiameter;

    const float minX = iX - radius;
    const float minY = iY - radius;
    const float maxX = minX + diam;
    const float maxY = minY + diam;

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

    //trans.scale( remainingScale, remainingScale );
    //qDebug() << "Scale" << remainingScale;
    trans.scale( scale, scale );

    auto transfo = QTransform() * trans * QTransform::fromTranslate( minX, minY );

    //TransformNearestNeighbourDirectOutputNormalBlendFParallel( mMipMapF[ indexMip ], mToolSize * startingScale, mToolSize * startingScale,
    //                                                           _mFloatBuffer, mDrawingContext->bytesPerLine()/4, mDrawingContext->height(),
    //                                                           mDrawingContext, transfo, QPoint( 0, 0 ) );

    //QPointF mousePos( iX, iY );
    //QPoint mousePosI( iX, iY );
    //QPointF subPixelOffset = mousePosI - mousePos; // Invert the sign to have the offset from the source image perspective
    const int minXI = int( minX );
    const int minYI = int( minY );
    QPointF subPixelOffset( minXI - minX, minYI - minY ); // Invert the sign to have the offset from the source image perspective

    //MTDownscaleBoxAverageDirectAlphaFDry( mMipMapF[ indexMip ], mipMapSizeAtIndex, mipMapSizeAtIndex,
    MTDownscaleBoxAverageDirectAlphaFDry( mMipMapF[ 0 ], baseDiameter, baseDiameter,
                                       mDryBuffer, mDrawingContext->bytesPerLine()/4, mDrawingContext->height(),
                                       mStampBuffer,
                                       _mFloatBuffer,
                                       mDrawingContext,
                                       0, 0, 0, transfo, QPoint( 0, 0 ), subPixelOffset, mOpacity );

    // Normal drawing
    //BENCHSTART( 10 )
    //MTDownscaleBoxAverageDirectAlphaF( mMipMapF[ indexMip ], mipMapSizeAtIndex, mipMapSizeAtIndex,
    //                                   _mFloatBuffer, mDrawingContext->bytesPerLine()/4, mDrawingContext->height(),
    //                                   _mFloatBuffer, mDrawingContext->bytesPerLine()/4, mDrawingContext->height(),
    //                                   mDrawingContext,
    //                                   0, 0, 0, transfo, QPoint( 0, 0 ) );
    //BENCHEND( 10 )


    //BENCHSTART( 10 )
    //MTDownscaleBoxAverageDirectAlphaF( mTipRenderedF, baseDiameter, baseDiameter,
    //                                                           _mFloatBuffer, mDrawingContext->bytesPerLine()/4, mDrawingContext->height(),
    //                                                           mDrawingContext,
    //                                                            0, 0, 0, transfo, QPoint( 0, 0 ) );
    //BENCHEND( 10 )

    mDirtyArea = mDirtyArea.united( QRect( startingX, startingY, endingX - startingX + 1, endingY - startingY + 1 ) );
}


void
cToolSimpleBrush::DrawLine( const QPointF& iP1, const QPointF& iP2, float iPressure1, float iRotation1, float iPressure2, float iRotation2 )
{
}


QRect
cToolSimpleBrush::EndDrawing( sPointData iPointData )
{
    //_GPU->ClearPaintToolBuffers();
    //IMAGEDEBUG->ShowImage( _mFloatBuffer, mDrawingContext->width(), mDrawingContext->height() );
    //IMAGEDEBUG->ShowImage( mStampBuffer, mDrawingContext->width(), mDrawingContext->height() );

    if( Vectorial() )
    {
        mPath = SimplifyLine( mPath, 20000000, 0.1 );
        DrawFullPath();
    }

    return  cPaintToolBase::EndDrawing( iPointData );
}


void
cToolSimpleBrush::CancelDrawing()
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
    MTHardFillF( mStampBuffer, mDrawingContext->bytesPerLine()/4, mDrawingContext->height(), mDrawingContext->rect(), Qt::transparent );

    mDirtyArea = mDrawingContext->rect();
}


void
cToolSimpleBrush::PrepareTool()
{
    int toolDiameter = mToolSize * 2;

    QRect rect( 0, 0, toolDiameter, toolDiameter );
    delete  mTipRenderedF;
    mTipRenderedF = new float[ toolDiameter * 4 * toolDiameter ];
    MTHardFillF( mTipRenderedF, toolDiameter, toolDiameter, rect, Qt::transparent );
    _RenderTip( mToolSize, mToolSize, 1.0, 0.0 );
    _BuildMipMap();
}


void
cToolSimpleBrush::_RenderTip( int iX, int iY, float iPressure, float iRotation )
{
    float* data = mTipRenderedF;
    float* pixelRow = data;
    unsigned int width = mToolSize * 2;
    unsigned int height = mToolSize * 2;

    float originR = mColor.red()    * mOpacity;
    float originG = mColor.green()  * mOpacity;
    float originB = mColor.blue()   * mOpacity;
    float originA = mColor.alpha()  * mOpacity;

    float finalR = originR;
    float finalG = originG;
    float finalB = originB;
    float finalA = originA;

    const int bytesPerLine = width * 4;

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
        pixelRow = data + y * bytesPerLine + bboxMinX * 4;

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
                    finalR = originR * mult;
                    finalG = originG * mult;
                    finalB = originB * mult;
                    finalA = originA * mult;
                }

                BlendPixelNormalF( &pixelRow, finalR, finalG, finalB, finalA );
            }
            else
            {
                pixelRow += 4;
            }
        }
    }
}


void
cToolSimpleBrush::_BuildMipMap()
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

    float* tipRenderedCopy = new float[ width * 4 * height ];
    memcpy( tipRenderedCopy, mTipRenderedF, sizeof( float ) * width * 4 * height );

    mMipMapF.push_back( tipRenderedCopy );
    //ws.push_back( width );
    //hs.push_back( height );

    while( width > 1 && height > 1 )
    {
        mMipMapF.push_back( DownscaleBoxAverageIntoImageF( mMipMapF.last(), width, height, t, &width, &height ) );
        //ws.push_back( width );
        //hs.push_back( height );
    }
}




