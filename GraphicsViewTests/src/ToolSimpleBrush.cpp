#include "ToolSimpleBrush.h"

#include <QDebug>

#include "BenchmarkStuff.h"
#include "Image.Utilities.h"
#include "Image.UtilitiesMultiThreaded.h"
#include "Blending.h"
#include "GPUForThisApp.h"


cToolSimpleBrush::~cToolSimpleBrush()
{
    delete  mTipRendered;
}


cToolSimpleBrush::cToolSimpleBrush( QObject * iParent ) :
    cPaintToolBase( iParent )
{
    // Some debug values to work with
    mToolSize = 10;
    mColor = Qt::red;
    mStep = 0.05;
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

    delete  mTipRendered;
    mTipRendered = new QImage( mToolSize, mToolSize, QImage::Format::Format_RGBA8888_Premultiplied );
    mTipRendered->fill( Qt::transparent );
    _DrawDot( mTipRendered, mToolSize/2, mToolSize/2, 1.0, 0.0 );
    mDirtyArea = QRect( 0, 0, 0, 0 );

    _GPU->LoadPaintContext( mDrawingContext );
    //_GPU->LoadPaintAlpha( mAlphaMask );
    _GPU->LoadBrushTip( mTipRendered );
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
    // Can't work because too many calls :
    // If step is small and mouse moves are large : each move = 100+ dots, plus a lot of moves per seconds, this means hundreds of calls per seconds
    // This means a LOT of readBuffer -> Not efficient, would need to never read buffer = use some sort of openGL display


    //_GPU->Paint( iX, iY, iPressure, iRotation );

    //_mToolSizeAfterPressure = Max( mToolSize * iPressure, 1.0F );

    //int diam = mToolSize * iPressure;
    //int radius = diam / 2;

    //int minX = iX - radius;
    //int maxX = minX + diam;
    //int minY = iY - radius;
    //int maxY = minY + diam;

    //// Basic out of bounds elimination
    //if( minX >= mDrawingContext->width() || minY >= mDrawingContext->height() )
    //    return;
    //if( maxX < 0 || maxY < 0 )
    //    return;

    //int startingX = minX < 0 ? 0 : minX;
    //int endingX = maxX >= mDrawingContext->width() ? mDrawingContext->width() - 1 : maxX;
    //int startingY = minY < 0 ? 0 : minY;
    //int endingY = maxY >= mDrawingContext->height() ? mDrawingContext->height() - 1 : maxY;

    //mDirtyArea = mDirtyArea.united( QRect( startingX, startingY, endingX - startingX + 1, endingY - startingY + 1 ) );

    //return;

    uchar* data = mDrawingContext->bits();
    uchar* dataScanline = data;

    const uchar* alphaData = 0;
    const uchar* alphaScanline = 0;

    int bytesPerLine = mDrawingContext->bytesPerLine();

    _mToolSizeAfterPressure = Max( mToolSize * iPressure, 1.0F );
    float diam = mToolSize * iPressure;
    float radius = diam / 2;

    const float minX = iX - radius;
    const float maxX = minX + diam;
    const float minY = iY - radius;
    const float maxY = minY + diam;

    const float minXNoPressure = iX - mToolSize/2;
    const float minYNoPressure = iY - mToolSize/2;

    const float offetX = minX - minXNoPressure;
    const float offetY = minY - minYNoPressure;

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
    trans.scale( iPressure, iPressure );

    auto transfo = QTransform() * trans * QTransform::fromTranslate( minX, minY );

    MTDownscaleBoxAverageDirectAlpha( mTipRendered, mDrawingContext, 0, transfo, QPoint( 0, 0 ) );

    mDirtyArea = mDirtyArea.united( QRect( startingX, startingY, endingX - startingX + 1, endingY - startingY + 1 ) );
}


void
cToolSimpleBrush::DrawLine( const QPoint& iP1, const QPoint& iP2, float iPressure1, float iRotation1, float iPressure2, float iRotation2 )
{
}


QRect
cToolSimpleBrush::EndDrawing( sPointData iPointData )
{
    _GPU->ClearPaintToolBuffers();

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
    uint8_t originR = mColor.red();
    uint8_t originG = mColor.green();
    uint8_t originB = mColor.blue();
    uint8_t originA = mColor.alpha();

    uint8_t finalR = mColor.red();
    uint8_t finalG = mColor.green();
    uint8_t finalB = mColor.blue();
    uint8_t finalA = mColor.alpha();

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
                int xpos = x*4;

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




