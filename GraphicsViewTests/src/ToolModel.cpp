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
cToolModel::DrawDot( int x, int y, float iPressure, float iRotation )
{
    uchar* data = mDrawingContext->bits();

    int toolSize = mToolSize * iPressure;

    int topLeftX = x - toolSize / 2;
    int topLeftY = y - toolSize / 2;


    // Constrains
    topLeftX = topLeftX < 0 ? 0 : topLeftX;
    topLeftY = topLeftY < 0 ? 0 : topLeftY;

    if( topLeftX + toolSize >= mDrawingContext->width() )
        topLeftX = mDrawingContext->width() - toolSize - 1;
    if( topLeftY + toolSize >= mDrawingContext->height() )
        topLeftY = mDrawingContext->height() - toolSize - 1;
    // ==========


    unsigned int index = 0;
    for( unsigned int y = topLeftY; y < topLeftY + mToolSize ; ++y )
    {
        for( unsigned int x = topLeftX; x < topLeftX + mToolSize; ++x )
        {
            index = y * mDrawingContext->width()*4 + x*4;

            data[index]     = mColor.blue();    // B
            data[index+1]   = mColor.green();   // G
            data[index+2]   = mColor.red();     // R
            data[index+3]   = mColor.alpha();   // A
        }
    }
}


void
cToolModel::DrawLine( int x1, int y1, int x2, int y2 )
{
    DrawDot( x1, y1, 1, 0 );
    DrawDot( x2, y2, 1, 0 );

    // All inbetweens
}




