#include "ToolModel.h"



cToolModel::~cToolModel()
{
    delete  mPen;
    delete  mBrush;
}


cToolModel::cToolModel( QObject * iParent ) :
    QAbstractListModel( iParent )
{
    mToolSize = 5;
    mAntiAliasingOn = true;

    mBrush = new QBrush();
    mBrush->setStyle( Qt::SolidPattern );
    mBrush->setColor( mAPen );

    mPen  = new QPen();
    mPen->setWidth( mToolSize );
    mPen->setBrush( *mBrush );
    mPen->setCapStyle( Qt::RoundCap );
    mPen->setJoinStyle( Qt::RoundJoin );
}


int
cToolModel::rowCount( const QModelIndex & iIndex ) const
{
    return  2;
}


QVariant
cToolModel::data( const QModelIndex & iIndex, int iRole ) const
{
    if( iRole == Qt::DisplayRole || iRole == Qt::EditRole )
    {
        if( iIndex.row() == 0 )
            return  mToolSize;
        else if( iIndex.row() == 1 )
            return  mAntiAliasingOn;
        else if( iIndex.row() == 2 )
            return  mAPen;
    }

    return QVariant();
}


QVariant
cToolModel::headerData( int iSection, Qt::Orientation iOrientation, int iRole ) const
{
    if( iSection == 0 && iRole == Qt::DisplayRole )
        return  "ToolValue";

    return QVariant();
}


bool
cToolModel::setData( const QModelIndex & iIndex, const QVariant & iValue, int iRole )
{
    if( iRole == Qt::EditRole )
    {
        if( iIndex.row() == 0 )
        {
            mToolSize = iValue.toInt();
            mPen->setWidth( mToolSize );
            dataChanged( iIndex, iIndex );
        }
        else if( iIndex.row() == 1 )
        {
            mAntiAliasingOn = iValue.toBool();
            dataChanged( iIndex, iIndex );
        }
        else if( iIndex.row() == 2 )
        {
            mAPen = iValue.value<QColor>();
            mBrush->setColor( mAPen );
            mPen->setBrush( *mBrush );
            dataChanged( iIndex, iIndex );
        }
    }

    return false;
}


Qt::ItemFlags
cToolModel::flags( const QModelIndex & iIndex ) const
{
    return  QAbstractListModel::flags( iIndex) | Qt::ItemIsEditable;
}


int
cToolModel::getSize() const
{
    return  data( createIndex( 0, 0 ) ).toInt();
}


void
cToolModel::setSize( int iSize )
{
    setData( createIndex( 0, 0 ), iSize );
}


QColor
cToolModel::getColor() const
{
    QVariant dataColor = data( createIndex( 2, 0 ) );
    return  dataColor.value< QColor >();
}


void
cToolModel::setColor( const QColor & iColor )
{
    setData( createIndex( 2, 0 ), iColor );
}


QPainter *
cToolModel::getNewPainter( QPaintDevice* iPaintDevice )
{
    QPainter* painter = new QPainter( iPaintDevice );

    if( mAntiAliasingOn )
        painter->setRenderHint( QPainter::Antialiasing );

    painter->setPen( *mPen );

    return  painter;
}


QPen *
cToolModel::getPen()
{
    return  mPen;
}


QBrush *
cToolModel::getBrush()
{
    return  mBrush;
}


QPixmap*
cToolModel::getToolHUD()
{
    QPixmap* brushHUD = new QPixmap( mToolSize, mToolSize );
    brushHUD->fill( Qt::transparent );
    QPainter painter( brushHUD );

    painter.setBrush( Qt::transparent );
    painter.setPen( Qt::lightGray );

    if( mToolSize == 1 )
        painter.drawPoint( 0, 0 );
    else
        painter.drawEllipse( 0, 0, mToolSize-1, mToolSize-1 );

    return  brushHUD;
}
