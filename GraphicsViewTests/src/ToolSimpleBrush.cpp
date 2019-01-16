#include "ToolSimpleBrush.h"


cToolSimpleBrush::~cToolSimpleBrush()
{
    delete  mTipRendered;
}


cToolSimpleBrush::cToolSimpleBrush( QObject * iParent ) :
    ToolBase( iParent )
{
    // Some debug values to work with
    mToolSize = 10;
    mColor = Qt::red;
    mStep = 10.0F; // Because we stay in squared numerics, to avoid the sqrt, so this would be a 5 pixel step
    mApplyProfile = true;

    mTipRendered = 0;
    buildTool();
}


Qt::ItemFlags
cToolSimpleBrush::flags( const QModelIndex & iIndex ) const
{
    return  ToolBase::flags( iIndex) | Qt::ItemIsEditable;
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
cToolSimpleBrush::StartDrawing()
{
    delete  mTipRendered;
    mTipRendered = new QImage( mToolSize, mToolSize, QImage::Format::Format_RGBA8888_Premultiplied );
    mTipRendered->fill( Qt::transparent );
    _DrawDot( mTipRendered, mToolSize/2, mToolSize/2, 1.0, 0.0 );
    ToolBase::StartDrawing();
}


void
cToolSimpleBrush::DrawDot( QImage* iImage, int iX, int iY, float iPressure, float iRotation )
{
    uchar* data = iImage->bits();
    uchar* dataReader = 0;
    uchar* dataTip = mTipRendered->bits();

    unsigned int bytesPerLine = iImage->bytesPerLine();
    unsigned int bytesPerLineTip = mTipRendered->bytesPerLine();

    int size = mToolSize * iPressure;
    QImage* scaledTip = mTipRendered;

    if( size != mToolSize )
        scaledTip = new QImage( mTipRendered->scaled( size, size, Qt::AspectRatioMode::KeepAspectRatio ) );

    int radius = scaledTip->width() / 2;

    float maxInverse = 1/255.F;

    int minX = iX - radius;
    int maxX = iX + radius;
    int minY = iY - radius;
    int maxY = iY + radius;

    int startingX = minX < 0 ? 0 : minX;
    int endingX = maxX >= iImage->width() ? iImage->width() - 1 : maxX;
    int startingY = minY < 0 ? 0 : minY;
    int endingY = maxY >= iImage->height() ? iImage->height() - 1 : maxY;


    for( int x = startingX; x < endingX ; ++x )
    {
        for( int y = startingY; y < endingY ; ++y )
        {
            int index = y * bytesPerLine + x * 4;
            int indexTip = (y - minY) * bytesPerLineTip + (x - minX ) * 4;

            dataReader = data + index;

            int srcB = dataTip[ indexTip ];
            int srcG = dataTip[ indexTip+1 ];
            int srcR = dataTip[ indexTip+2 ];
            int srcA = dataTip[ indexTip+3 ];

            float transparencyAmountInverse = 1.F - ( float( srcA ) * maxInverse );

            dataReader[ 0 ]  = srcB + dataReader[ 0 ]  * transparencyAmountInverse;
            dataReader[ 1 ]  = srcG + dataReader[ 1 ]  * transparencyAmountInverse;
            dataReader[ 2 ]  = srcR + dataReader[ 2 ]  * transparencyAmountInverse;
            dataReader[ 3 ]  = srcA + dataReader[ 3 ]  * transparencyAmountInverse;

        }
    }
}


void
cToolSimpleBrush::DrawLine( QImage * iImage, int x1, int y1, int x2, int y2 )
{
    DrawDot( iImage, x1, y1, 1, 0 );
    DrawDot( iImage, x2, y2, 1, 0 );

    // All inbetweens
}


void
cToolSimpleBrush::_DrawDot( QImage * iImage, int iX, int iY, float iPressure, float iRotation )
{
    uchar* data = iImage->bits();
    uchar* test = 0;
    unsigned int width = iImage->width();
    unsigned int height = iImage->height();
    int originR = mColor.red();
    int originG = mColor.green();
    int originB = mColor.blue();
    int originA = mColor.alpha();

    int finalR = mColor.red();
    int finalG = mColor.green();
    int finalB = mColor.blue();
    int finalA = mColor.alpha();

    unsigned int bytesPerLine = iImage->bytesPerLine();

    int r = (mToolSize/2) * iPressure;
    int r2 = r*r;

    int bboxMinX = iX - r;
    int bboxMaxX = iX + r;
    int bboxMinY = iY - r;
    int bboxMaxY = iY + r;

    float maxInverse = 1/255.F;


    bboxMinX = bboxMinX < 0 ? 0 : bboxMinX;
    bboxMaxX = bboxMaxX >= width ? width - 1 : bboxMaxX;

    bboxMinY = bboxMinY < 0 ? 0 : bboxMinY;
    bboxMaxY = bboxMaxY >= height ? height - 1 : bboxMaxY;

    unsigned int iterationCount = (bboxMaxX - bboxMinX) * (bboxMaxY - bboxMinY);

    for( int x = bboxMinX; x <= bboxMaxX ; ++x )
    {
        for( int y = bboxMinY; y <= bboxMaxY ; ++y )
        {
            int dx = x - iX;
            int dy = y - iY;
            if( dx * dx + dy * dy <= r2 )
            {
                int index = y * bytesPerLine + x * 4;
                test = data + index;


                if( mApplyProfile )
                {
                    float distance = Distance2PointsSquared( QPoint( iX, iY ), QPoint( iX + dx, iY + dy ) );
                    float distanceParam = 1.0F - ( distance / float(( mToolSize * mToolSize / 4 )) ); // 1 - distanceRatio so it goes outwards, otherwise, it's a reversed gradient
                    float mult = mProfile.GetValueAtTime( distanceParam );
                    finalR = float( originR ) * mult;
                    finalG = float( originG ) * mult;
                    finalB = float( originB ) * mult;
                    finalA = float( originA ) * mult;
                }

                if( mAlphaMask && mAlphaMask->width() == width && mAlphaMask->height() == mAlphaMask->height() )
                {
                    float alphaMaskTransparency = float(mAlphaMask->bits()[ index + 3 ]) * maxInverse;
                    finalR *= alphaMaskTransparency;
                    finalG *= alphaMaskTransparency;
                    finalB *= alphaMaskTransparency;
                    finalA *= alphaMaskTransparency;
                }

                float transparencyAmountInverse = 1.F - ( float( finalA ) * maxInverse );

                // BGRA format and premultiplied alpha
                // Premultiplied allows this simple equation, basically we do a weighted sum of source and destination, weighted by the src's alpha
                // So we basically keep as much dst as src is transparent -> the more src is transparent, the more we want dst's color, so -> mult by 1-alpha, alpha between 0 and 1
                test[ 0 ]  = finalB + test[ 0 ]  * transparencyAmountInverse;
                test[ 1 ]  = finalG + test[ 1 ]  * transparencyAmountInverse;
                test[ 2 ]  = finalR + test[ 2 ]  * transparencyAmountInverse;
                test[ 3 ]  = finalA + test[ 3 ]  * transparencyAmountInverse;
            }
        }
    }
}




