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

    buildTool();
}


Qt::ItemFlags
cToolModel::flags( const QModelIndex & iIndex ) const
{
    return  ToolBase::flags( iIndex) | Qt::ItemIsEditable;
}


int
cToolModel::getSize() const
{
    return  itemFromIndex( index( 0, 0 ) )->data().toInt();
}


void
cToolModel::setSize( int iSize )
{
    itemFromIndex( index( 0, 0 ) )->setData( iSize );
    mToolSize = iSize;
}


QColor
cToolModel::getColor() const
{
    QVariant dataColor = itemFromIndex( index( 1, 0 ) )->data();
    return  dataColor.value< QColor >();
}


void
cToolModel::setColor( const QColor & iColor )
{
    itemFromIndex( index( 1, 0 ) )->setData( iColor );
    mColor = iColor;
}


float
cToolModel::getStep() const
{
    return  mStep;
}


void
cToolModel::setStep( float iStep )
{
    mStep = iStep;
    itemFromIndex( index( 2, 0 ) )->setData( iStep );
}


void
cToolModel::buildTool()
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


void
cToolModel::DrawPath( QImage * iImage )
{
    if( mPath.size() <= 0 )
        return;


    // Draw the very first point always, then use step
    DrawDot( iImage, mPath[0].x(), mPath[0].y() );

    // How many units required to perform a full step ?
    // Used to transfert over each iterations
    float requiredToStep = mStep;

    for( int i = 0; i < mPath.size() - 1; ++i )
    {
        // Setting base variables : starting point, ending point and their distance
        QPoint p1 = mPath[ i ];
        QPoint p2 = mPath[ i + 1 ];
        float distance = Distance2Points( p1, p2 );

        // Two identical points -> SKIP
        if( abs( distance ) < 0.01 )
            continue;

        // Not enough distance on that path segment to draw a dot, so we'll see on the next one, after decreasing the requiredToStep
        if( requiredToStep - distance > 0 )
        {
            requiredToStep -= distance;
            continue;
        }


        // Getting the directionnal vector, that'll be used to find all points on the segment
        QVector2D stepVectorNormalized = QVector2D( p2 - p1 ).normalized();
        QPointF stepVectorNormalizedAsPF = stepVectorNormalized.toPointF();


        // From here, we split segment and draw dots
        // ==================================
        // ==================================

        float remainingDistance = distance; // The distance left of the segment, that still needs split
        QPoint startingPoint = p1;


        // If spare steps from previous it, we apply it, and then use this point as first
        if( abs( requiredToStep - mStep ) > 0.1F )
        {
            startingPoint = __DrawDotVectorTruc_RequiresAName_( iImage, p1, stepVectorNormalizedAsPF * requiredToStep );
            remainingDistance -= requiredToStep;
        }

        // Now, we go step by step
        int count = 1; // Counting how many split have been drawn

        // We split the segment using mStep, and draw dots on each step
        while( remainingDistance >= mStep )
        {
            __DrawDotVectorTruc_RequiresAName_( iImage, startingPoint, stepVectorNormalizedAsPF * mStep * count );
            remainingDistance -= mStep;
            ++count;

        }

        // Here, we probably have space between the last dot and the next point, not enough for a step, but still space, so we remember it for next iteration
        requiredToStep = mStep - remainingDistance;
    }



    // Debug to show mPath points
    //setColor( Qt::green );

    //for( auto point : mPath )
    //{
    //    //DrawDot( iImage, point.x(), point.y() );
    //}

    //setColor( Qt::red );

    mPath.clear(); // We draw the path, we clear it
}


QPoint
cToolModel::__DrawDotVectorTruc_RequiresAName_( QImage* iImage, const QPoint & iStart, const QPointF & iVector )
{
    // This takes a starting point, offsets by a vector, draws a dot there and return the offset point's coordinates
    QPoint stepVector = QPoint( std::roundf( iVector.x() ), std::roundf( iVector.y() ) );
    QPoint stepPosition = iStart + stepVector;

    DrawDot( iImage, stepPosition.x(), stepPosition.y() );

    return  stepPosition;
}




