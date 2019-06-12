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


void
cToolStamp::buildTool()
{
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
    ClearTool(); // Deletes all tips and mip maps, causing a recreation when draw start happens
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
    ClearTool(); // Deletes all tips and mip maps, causing a recreation when draw start happens
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
    ClearTool(); // Deletes all tips and mip maps, causing a recreation when draw start happens
}


void
cToolStamp::StartDrawing( QImage* iImage, sPointData iPointData )
{
    cPaintToolBase::StartDrawing( iImage, iPointData );

    const int imageSize = iImage->width() * 4 * iImage->height();

    // Initialize DryBuffer
    delete[] mDryBuffer;
    mDryBuffer = new float[ imageSize ];
    float* data = _mFloatBuffer;
    float* scan = data;

    for( int y = 0; y < iImage->height(); ++y )
    {
        scan = data + y * iImage->width() * 4;
        const int floatIndex = y * iImage->width() * 4;

        for( int x = 0; x < iImage->width() * 4; ++x )
        {
            mDryBuffer[ floatIndex + x ] = *scan; ++scan;
        }
    }

    // Initialize StampBuffer
    delete[] mStampBuffer;
    mStampBuffer = new float[ imageSize ];
    memset( mStampBuffer, 0, sizeof(float) * imageSize );

    // Init color stamp
    const int toolDiameter = mToolSize*2;
    mColorStampFOrigin = new float[ toolDiameter * 4 * toolDiameter ];

    //delete mColorStampF;
    //mColorStampF = new float[ toolDiameter * 4 * toolDiameter ];

    MTHardFillF( mColorStampFOrigin, toolDiameter, toolDiameter, QRect(  0, 0, toolDiameter, toolDiameter ), mColor );
    //MTHardFillF( mColorStampF, toolDiameter, toolDiameter, QRect(  0, 0, toolDiameter, toolDiameter ), mColor );
    //MTHardFillF( mColorStampF, toolDiameter, toolDiameter, QRect(  0, 0, toolDiameter/2, toolDiameter ), mColor );
    //MTHardFillF( mColorStampF, toolDiameter, toolDiameter, QRect(  toolDiameter/2, 0, toolDiameter/2, toolDiameter ), Qt::black );
    //MTHardFillF( mColorStampF, toolDiameter, toolDiameter, QRect(  0, 0, toolDiameter, toolDiameter ), Qt::transparent );

    mColorUniform = true;

    //QImage* colorPatern = new QImage( "Resources/ColorPatern.png" );
    //for( int y = 0; y < colorPatern->height(); ++y )
    //{
    //    uchar* scanner = colorPatern->bits() + y * colorPatern->width() * 4;

    //    for( int x = 0; x < colorPatern->width(); ++x )
    //    {
    //        const int floatIndex = y * colorPatern->width() * 4 + x*4;

    //        mColorStampF[ floatIndex + 2 ] = *scanner; ++scanner;
    //        mColorStampF[ floatIndex + 1 ] = *scanner; ++scanner;
    //        mColorStampF[ floatIndex + 0 ] = *scanner; ++scanner;
    //        mColorStampF[ floatIndex + 3 ] = *scanner; ++scanner;
    //    }
    //}



    // If no tips are there, we prepare the tool == we build tips and mip maps
    if( mTipRenderedF.size() == 0 )
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
    const int   intWidth = maxX - minX;
    const int   intHeight = maxY - minY;

    // Basic out of bounds elimination
    if( minX >= mDrawingContext->width() || minY >= mDrawingContext->height() )
        return;
    if( maxX < 0 || maxY < 0 )
        return;

    const int startingX = int( minX ) < 0 ? 0 : int( minX );
    const int endingX = int( maxX + 1 ) >= mDrawingContext->width() ? mDrawingContext->width() - 1 : int( maxX + 1 );
    const int startingY = int( minY ) < 0 ? 0 : int( minY );
    const int endingY = int( maxY + 1 ) >= mDrawingContext->height() ? mDrawingContext->height() - 1 : int( maxY + 1 );

    mDirtyArea = mDirtyArea.united( QRect( startingX, startingY, endingX - startingX + 1, endingY - startingY + 1 ) );

    auto trans = QTransform();
    int indexMip = std::min( log2( 1/scale ), float( mMipMapF[mCurrentTipIndex].count() - 1 ) );
    float startingScale = indexMip == 0 ? 1 : std::pow( 0.5, indexMip );
    float remainingScale = scale / startingScale;

    const int mipMapSizeAtIndex = baseDiameter * startingScale;

    trans.scale( remainingScale, remainingScale );

    auto transfo = QTransform() * trans * QTransform::fromTranslate( minX, minY );

    const int minXI = int( minX );
    const int minYI = int( minY );


    // Grab color from canvas
    // Not using dirtyArea because dirtyarea is clipped already, and we need the negative part to be there, so we can blend colorStamp with the right offset
    if( mMixColorActivated )
    {
        MTBlendImagesF( _mFloatBuffer, mDrawingContext->width(), mDrawingContext->height(), QRect( iX - mToolSize, iY - mToolSize, baseDiameter, baseDiameter ),
                        mColorStampF, baseDiameter, baseDiameter, QPoint( 0,0 ),
                        mMipMapF[mCurrentTipIndex][0], baseDiameter, baseDiameter, QPoint( 0,0 ), 0.7 );
    }

    // Doing diff in int is the way to go, this is what prevent an offset to the left, shifting paint to the right
    const QPointF colorOffset = QPoint( minX, minY ) - QPoint( iX - mToolSize, iY - mToolSize );// - QPointF( 0.1,0.1 );

    // Put paint on canvas
    MTDownscaleBoxAverageDirectAlphaFDry( mMipMapF[mCurrentTipIndex][ indexMip ], mipMapSizeAtIndex, mipMapSizeAtIndex,
                                          mColorStampF, baseDiameter, baseDiameter,
                                          (mColorUniform && !mMixColorActivated),
                                          mDryBuffer, mDrawingContext->bytesPerLine()/4, mDrawingContext->height(),
                                          mStampBuffer,
                                          _mFloatBuffer,
                                          mDrawingContext,
                                          mAlphaMask, transfo, QPoint( 0, 0 ), mOpacity,
                                          (mDryActivated || mMixColorActivated),
                                          colorOffset );

    // Ink back original color
    if( mColorReinjection )
    {
        MTBlendImagesF( mColorStampFOrigin, baseDiameter, baseDiameter, QRect( 0, 0, baseDiameter, baseDiameter ),
                        mColorStampF, baseDiameter, baseDiameter, QPoint( 0, 0 ),
                        mMipMapF[mCurrentTipIndex][0], baseDiameter, baseDiameter, QPoint( 0, 0 ), 0.2*iPressure );
    }

    if( mStyle == kLinearLoop )
    {
        mCurrentTipIndex = (mCurrentTipIndex + 1) % mTipRenderedF.size();
    }
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

    int baseDiameter = mToolSize*2;

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
    memset( mStampBuffer, 0, sizeof(float) * mDrawingContext->width() * 4 * mDrawingContext->height() );

    mDirtyArea = mDrawingContext->rect();
}


void
cToolStamp::AddTip( float * iTip )
{
    mTipRenderedF.push_back( iTip );
}


void
cToolStamp::SetTipSwapStyle( eTipSwapStyle iStyle )
{
    mStyle = iStyle;
}


void
cToolStamp::ClearTool()
{
    ClearTips();
    _ClearMipMaps();
}


void
cToolStamp::PrepareTool()
{
    qDebug() << "Clear";
    ClearTool();

    const int toolDiameter = mToolSize*2;
    delete mColorStampF;
    mColorStampF = new float[ toolDiameter * 4 * toolDiameter ];
    MTHardFillF( mColorStampF, toolDiameter, toolDiameter, QRect(  0, 0, toolDiameter, toolDiameter ), mColor );

    RenderTips( mToolSize, mToolSize );
    _BuildMipMaps();
}



void
cToolStamp::ClearTips()
{
    for( auto tip : mTipRenderedF )
        delete  tip;

    mTipRenderedF.clear();
}


void
cToolStamp::_BuildMipMaps()
{
    for( auto tip : mTipRenderedF )
    {
        QVector< const float * > mip;
        //QVector< int > ws;
        //QVector< int > hs;

        int width = mToolSize*2;
        int height = mToolSize*2;
        QTransform t = QTransform::fromScale( 0.5, 0.5 );

        float* tipRenderedCopy = new float[ width * height ];
        memcpy( tipRenderedCopy, tip, sizeof( float ) * width * height );

        mip.push_back( tipRenderedCopy );
        //ws.push_back( width );
        //hs.push_back( height );

        while( width > 1 && height > 1 )
        {
            mip.push_back( DownscaleBoxAverageIntoImageFGray( mip.last(), width, height, t, &width, &height ) );
            //ws.push_back( width );
            //hs.push_back( height );
        }

        //IMAGEDEBUG->ShowImagesGray( mMipMapF, ws, hs );

        mMipMapF.push_back( mip );
    }
}


void
cToolStamp::_ClearMipMaps()
{
    for( auto mip : mMipMapF )
    for( auto tip : mip )
        delete  tip;

    mMipMapF.clear();
}


void
cToolStamp::DEBUG()
{
    IMAGEDEBUG->ShowImage( mColorStampF, 300, 300 );
}




