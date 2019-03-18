#include "ToolSimpleBrush.h"

#include <QDebug>

#include "BenchmarkStuff.h"
#include "Image.Utilities.h"
#include "Image.UtilitiesMultiThreaded.h"
#include "Image.ToolPaintUtilities.h"
#include "Blending.h"
#include "GPUForThisApp.h"
#include "ImageDebugger.h"


cToolSimpleBrush::~cToolSimpleBrush()
{
    delete  mTipRendered;
}


cToolSimpleBrush::cToolSimpleBrush( QObject * iParent ) :
    cPaintToolBase( iParent )
{
    // Some debug values to work with
    mToolSize = 50;
    mColor = Qt::red;
    mStep = 1.5;
    mOpacity = 1.F;
    mApplyProfile = true;

    mTipRendered = 0;
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

    int toolDiameter = mToolSize * 2 + 1; // To get an odd diameter, so center is a single pixel

    delete  mTipRendered;
    mTipRendered = new QImage( toolDiameter, toolDiameter, QImage::Format::Format_ARGB32_Premultiplied );
    //mTipRendered->fill( Qt::transparent );
    MTHardFill( mTipRendered, mTipRendered->rect(), Qt::transparent );
    _DrawDot( mTipRendered, mToolSize, mToolSize, 1.0, 0.0 );


    QRect rect( 0, 0, toolDiameter, toolDiameter );
    delete  mTipRenderedF;
    mTipRenderedF = new float[ toolDiameter * 4 * toolDiameter ];
    MTHardFillF( mTipRenderedF, toolDiameter, toolDiameter, rect, Qt::transparent );
    _DrawDotF( mToolSize, mToolSize, 1.0, 0.0 );


    _BuildMipMap();
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
cToolSimpleBrush::DrawDot( int iX, int iY, float iPressure, float iRotation )
{
    //iPressure = 0.16;
    _mToolSizeAfterPressure = Max( mToolSize * iPressure, 1.0F );


    const int baseDiameter = mToolSize * 2 + 1; // To get the odd diameter
    const int radius = mToolSize * iPressure;
    const int diam = radius*2 + 1;

    const float scale = float(diam) / float(baseDiameter);

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

    auto trans = QTransform();
    int indexMip = std::min( log2( 1/scale ), float( mMipMapF.count() - 1 ) );
    float startingScale = indexMip == 0 ? 1 : std::pow( 0.5, indexMip );
    float remainingScale = scale / startingScale;

    const int mipMapSizeAtIndex = baseDiameter * startingScale;
    const int finalSize = mipMapSizeAtIndex * remainingScale;

    if( finalSize % 2 == 0  )
        remainingScale = float(finalSize + 1) / float(mipMapSizeAtIndex);

    trans.scale( remainingScale, remainingScale );
    auto transfo = QTransform() * trans * QTransform::fromTranslate( minX, minY );

    //TransformNearestNeighbourDirectOutputNormalBlendFParallel( mMipMapF[ indexMip ], mToolSize * startingScale, mToolSize * startingScale,
    //                                                           _mFloatBuffer, mDrawingContext->bytesPerLine()/4, mDrawingContext->height(),
    //                                                           mDrawingContext, transfo, QPoint( 0, 0 ) );



    MTDownscaleBoxAverageDirectAlphaF( mMipMapF[ indexMip ], baseDiameter * startingScale, baseDiameter * startingScale,
                                                               _mFloatBuffer, mDrawingContext->bytesPerLine()/4, mDrawingContext->height(),
                                                               mDrawingContext,
                                                                0, 0, 0, transfo, QPoint( 0, 0 ) );

    mDirtyArea = mDirtyArea.united( QRect( startingX, startingY, endingX - startingX + 1, endingY - startingY + 1 ) );
}


void
cToolSimpleBrush::DrawLine( const QPoint& iP1, const QPoint& iP2, float iPressure1, float iRotation1, float iPressure2, float iRotation2 )
{
}


QRect
cToolSimpleBrush::EndDrawing( sPointData iPointData )
{
    //_GPU->ClearPaintToolBuffers();

    return  cPaintToolBase::EndDrawing( iPointData );
}


float
cToolSimpleBrush::_GetStepInPixelValue() const
{
    return  std::max( mStep * _mToolSizeAfterPressure, 1.0F );
}


void
cToolSimpleBrush::_DrawDot( QImage * iImage, int iX, int iY, float iPressure, float iRotation )
{
    uchar* data = iImage->bits();
    uchar* pixelRow = data;
    unsigned int width = iImage->width();
    unsigned int height = iImage->height();
    uint8_t originR = mColor.red()      * mOpacity;
    uint8_t originG = mColor.green()    * mOpacity;
    uint8_t originB = mColor.blue()     * mOpacity;
    uint8_t originA = mColor.alpha()    * mOpacity;

    uint8_t finalR = originR;
    uint8_t finalG = originG;
    uint8_t finalB = originB;
    uint8_t finalA = originA;

    const int bytesPerLine = iImage->bytesPerLine();

    const int r = (mToolSize/2) * iPressure;
    const int r2 = r*r;

    int bboxMinX = iX - r;
    int bboxMaxX = iX + r;
    int bboxMinY = iY - r;
    int bboxMaxY = iY + r;

    const float radiusSq = mToolSize * mToolSize / 4;

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
            if( dx * dx + dy * dy <= r2 )
            {
                if( mApplyProfile )
                {
                    float distance = Distance2PointsSquared( QPoint( iX, iY ), QPoint( iX + dx, iY + dy ) );
                    float distanceParam = 1.0F - ( distance / radiusSq ); // 1 - distanceRatio so it goes outwards, otherwise, it's a reversed gradient
                    uint8_t mult = mProfile.GetValueAtTime( distanceParam ) * 255.F;
                    finalR = BlinnMult( originR, mult );
                    finalG = BlinnMult( originG, mult );
                    finalB = BlinnMult( originB, mult );
                    finalA = BlinnMult( originA, mult );
                }

                BlendPixelNormal( &pixelRow, finalR, finalG, finalB, finalA );
            }
            else
            {
                pixelRow += 4;
            }
        }
    }
}


void
cToolSimpleBrush::_DrawDotF( int iX, int iY, float iPressure, float iRotation )
{
    float* data = mTipRenderedF;
    float* pixelRow = data;
    unsigned int width = mToolSize * 2 + 1;
    unsigned int height = mToolSize * 2 + 1;

    float originR = mColor.red()    * mOpacity;
    float originG = mColor.green()  * mOpacity;
    float originB = mColor.blue()   * mOpacity;
    float originA = mColor.alpha()  * mOpacity;

    float finalR = originR;
    float finalG = originG;
    float finalB = originB;
    float finalA = originA;

    const int bytesPerLine = width * 4;

    const int r = mToolSize * iPressure;

    int bboxMinX = iX - r;
    int bboxMaxX = iX + r;
    int bboxMinY = iY - r;
    int bboxMaxY = iY + r;

    const float radiusSq = mToolSize * mToolSize / 4;

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

    int width = mToolSize*2+1;
    int height = mToolSize*2+1;
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




