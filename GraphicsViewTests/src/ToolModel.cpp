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
    // Debug
    {
        //mStep = 6.0F;
        //mPath.clear();
        //mPath.push_back( QPoint( 58, 62 ) );
        //mPath.push_back( QPoint( 64, 71 ) );
        //mPath.push_back( QPoint( 126, 151 ) );
        //mPath.push_back( QPoint( 232, 211 ) );
        //mPath.push_back( QPoint( 297, 211 ) );
        //mPath.push_back( QPoint( 381, 186 ) );
        //mPath.push_back( QPoint( 410, 174 ) );
        //mPath.push_back( QPoint( 416, 170 ) );
    }



    if( mPath.size() <= 1 )
    {
        DrawDot( iImage, mPath.back().x(), mPath.back().y() );
    }
    else
    {
        // Draw the very first point always, then use step
        DrawDot( iImage, mPath[0].x(), mPath[0].y() );
        float requiredToStep = mStep;

        for( int i = 0; i < mPath.size() - 1; ++i )
        {
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
            QVector2D stepVectorNormalized( p2 - p1 );
            stepVectorNormalized.normalize();
            QPointF stepVectorNormalizedAsPF = stepVectorNormalized.toPointF();


            // From here, we split segment and draw dots
            // ==================================
            float remainingDistance = distance;
            int count = 1;
            QPoint startingPoint = p1;

            // If spare steps from previous it, we apply it, and then use this point as first
            if( ( requiredToStep - mStep ) < 0.1F )
            {
                startingPoint = __DrawDotVectorTruc_( iImage, p1, stepVectorNormalizedAsPF, requiredToStep );
                remainingDistance -= requiredToStep;
                requiredToStep = mStep;
            }

            // We split the segment using mStep, and draw dots on each step
            while( remainingDistance >= mStep )
            {
                __DrawDotVectorTruc_( iImage, startingPoint, stepVectorNormalizedAsPF, mStep * count );
                remainingDistance -= requiredToStep;
                ++count;

            }

            requiredToStep -= remainingDistance;
        }
    }


    //setColor( Qt::green );

    //for( auto point : mPath )
    //{
    //    //DrawDot( iImage, point.x(), point.y() );
    //}

    //setColor( Qt::red );

    mPath.clear(); // We draw the path, we clear it
}


QPoint
cToolModel::__DrawDotVectorTruc_( QImage* iImage, const QPoint & iStart, const QPointF & iVector, float iAmount )
{
    QPointF stepVectorF = iVector * iAmount;
    QPoint stepVector = QPoint( std::roundf( stepVectorF.x() ), std::roundf( stepVectorF.y() ) );
    QPoint stepPosition = iStart + stepVector;

    DrawDot( iImage, stepPosition.x(), stepPosition.y() );

    return  stepPosition;
}




