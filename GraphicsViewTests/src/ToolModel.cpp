#include "ToolModel.h"

#include <QVector2D>
#include <QDebug>


cToolModel::~cToolModel()
{
}


cToolModel::cToolModel( QObject * iParent ) :
    ToolBase( iParent )
{
    mToolSize = 5;
    mColor = Qt::red;
    mStep = 10.0F; // Because we stay in squared numerics, to avoid the sqrt, so this would be a 5 pixel step

    _BuildTool();
}


Qt::ItemFlags
cToolModel::flags( const QModelIndex & iIndex ) const
{
    return  ToolBase::flags( iIndex) | Qt::ItemIsEditable;
}


void
cToolModel::_BuildTool()
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

}


void
cToolModel::DrawDot( QImage* iImage, int x, int y )
{
    uchar* data = iImage->bits();

    int topLeftX = x - mToolSize/2;
    int topLeftY = y - mToolSize/2;


    // Constrains
    topLeftX = topLeftX < 0 ? 0 : topLeftX;
    topLeftY = topLeftY < 0 ? 0 : topLeftY;

    if( topLeftX + mToolSize >= iImage->width() )
        topLeftX = iImage->width() - mToolSize - 1;
    if( topLeftY + mToolSize >= iImage->height() )
        topLeftY = iImage->height() - mToolSize - 1;
    // ==========


    unsigned int index = 0;
    for( unsigned int y = topLeftY; y < topLeftY + mToolSize ; ++y )
    {
        for( unsigned int x = topLeftX; x < topLeftX + mToolSize; ++x )
        {
            index = y * iImage->width()*4 + x*4;

            data[index]     = mColor.blue();    // B
            data[index+1]   = mColor.green();   // G
            data[index+2]   = mColor.red();     // R
            data[index+3]   = mColor.alpha();   // A
        }
    }
}


void
cToolModel::DrawLine( QImage * iImage, int x1, int y1, int x2, int y2 )
{
    DrawDot( iImage, x1, y1 );
    DrawDot( iImage, x2, y2 );

    // All inbetweens
}




