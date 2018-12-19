#include "ToolBase.h"

#include <QVector2D>

ToolBase::ToolBase(QObject *parent)
    : QStandardItemModel(parent)
{
}


ToolBase::~ToolBase()
{
}


void
ToolBase::DrawPath( QImage * iImage )
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


void
ToolBase::PathAddPoint( const QPoint & iPoint )
{
    mPath.push_back( iPoint );
}


QPoint
ToolBase::__DrawDotVectorTruc_RequiresAName_( QImage* iImage, const QPoint & iStart, const QPointF & iVector )
{
    // This takes a starting point, offsets by a vector, draws a dot there and return the offset point's coordinates
    QPoint stepVector = QPoint( std::roundf( iVector.x() ), std::roundf( iVector.y() ) );
    QPoint stepPosition = iStart + stepVector;

    DrawDot( iImage, stepPosition.x(), stepPosition.y() );

    return  stepPosition;
}


