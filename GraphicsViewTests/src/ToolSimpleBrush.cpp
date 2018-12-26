#include "ToolSimpleBrush.h"


cToolSimpleBrush::~cToolSimpleBrush()
{
}


cToolSimpleBrush::cToolSimpleBrush( QObject * iParent ) :
    ToolBase( iParent )
{
    // Some debug values to work with
    mToolSize = 10;
    mColor = Qt::red;
    mStep = 10.0F; // Because we stay in squared numerics, to avoid the sqrt, so this would be a 5 pixel step
    mApplyProfile = true;

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
cToolSimpleBrush::DrawDot( QImage* iImage, int iX, int iY, float iPressure, float iRotation )
{
    uchar* data = iImage->bits();
    unsigned int width = iImage->width();
    unsigned int height = iImage->height();
    int R = mColor.red();
    int G = mColor.green();
    int B = mColor.blue();
    int A = mColor.alpha();

    int r = (mToolSize/2) * iPressure;
    for( int dy = -r; dy <= r; ++dy)
    {
        for( int dx = -r; dx <= r; ++dx )
        {
            if( dx * dx + dy * dy <= r * r )
            {
                float mult = 1.0F;

                if( mApplyProfile )
                {
                    float distance = Distance2PointsSquared( QPoint( iX, iY ), QPoint( iX + dx, iY + dy ) );
                    float distanceParam = 1.0F - ( distance / float(( mToolSize * mToolSize / 4 )) ); // 1 - distanceRatio so it goes outwards, otherwise, it's a reversed gradient
                    mult = mProfile.GetValueAtTime( distanceParam );
                }

                // Color is premultiplied here with Qt, so gotta apply to all colors
                _DrawPixel( data, width, height, iX + dx, iY + dy, R * mult, G * mult, B * mult, A * mult );
            }
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




