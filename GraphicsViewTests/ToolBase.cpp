#include "ToolBase.h"

#include <QVector2D>


ToolBase::~ToolBase()
{
}


ToolBase::ToolBase(QObject *parent)
    : QStandardItemModel(parent)
{
    mStep = 10.0F;
    mLastRenderedPathIndex = 0;
    mRequiredPathLength = mStep;
}


// ===========================


int
ToolBase::getSize() const
{
    return  itemFromIndex( index( 0, 0 ) )->data().toInt();
}


void
ToolBase::setSize( int iSize )
{
    itemFromIndex( index( 0, 0 ) )->setData( iSize );
    mToolSize = iSize;
}


float
ToolBase::getStep() const
{
    return  mStep;
}


void
ToolBase::setStep( float iStep )
{
    mStep = iStep;
    itemFromIndex( index( 2, 0 ) )->setData( iStep );
}


// ===========================


void
ToolBase::StartDrawing()
{
    mRequiredPathLength = 0;
    mLastRenderedPathIndex = 0;
}


void
ToolBase::DrawFullPath( QImage * iImage )
{
    if( mPath.size() <= 0 )
        return;

    mRequiredPathLength = 0;
    mLastRenderedPathIndex = 0;

    DrawPathFromLastRenderedPoint( iImage );

    mPath.clear(); // We draw the path, we clear it
}


void
ToolBase::DrawPathFromLastRenderedPoint( QImage * iImage )
{
    if( mPath.size() <= 0 )
        return;


    for( mLastRenderedPathIndex; mLastRenderedPathIndex < mPath.size() - 1; ++mLastRenderedPathIndex )
    {
        // Setting base variables : starting point, ending point and their distance
        QPoint p1 = mPath[ mLastRenderedPathIndex ];
        QPoint p2 = mPath[ mLastRenderedPathIndex + 1 ];
        float distance = Distance2Points( p1, p2 );


        // Two identical points -> SKIP
        if( abs( distance ) < 0.01 )
            continue;

        // Not enough distance on that path segment to draw a dot, so we'll see on the next one, after decreasing the requiredToStep
        if( mRequiredPathLength - distance > 0 )
        {
            mRequiredPathLength -= distance;
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
        if( abs( mRequiredPathLength - mStep ) > 0.1F )
        {
            startingPoint = __DrawDotVectorTruc_RequiresAName_( iImage, p1, stepVectorNormalizedAsPF * mRequiredPathLength );
            remainingDistance -= mRequiredPathLength;
        }

        // Now, we go step by step
        int count = 1; // Counting how many split have been drawn

        // We split the segment using mStep, and draw dots on each step while there is room on the segment
        while( remainingDistance >= mStep )
        {
            __DrawDotVectorTruc_RequiresAName_( iImage, startingPoint, stepVectorNormalizedAsPF * mStep * count );
            remainingDistance -= mStep;
            ++count;
        }

        // Here, we probably have space between the last dot and the next point, not enough for a step, but still space, so we remember it for next iteration
        mRequiredPathLength = mStep - remainingDistance;

    }
}


void
ToolBase::EndDrawing()
{
    mPath.clear();
}


// ===========================


void
ToolBase::PathAddPoint( const QPoint & iPoint )
{
    mPath.push_back( iPoint );
}


// ===========================
// ===========================


QPoint
ToolBase::__DrawDotVectorTruc_RequiresAName_( QImage* iImage, const QPoint & iStart, const QPointF & iVector )
{
    // This takes a starting point, offsets by a vector, draws a dot there and return the offset point's coordinates
    QPoint stepVector = QPoint( std::roundf( iVector.x() ), std::roundf( iVector.y() ) );
    QPoint stepPosition = iStart + stepVector;

    DrawDot( iImage, stepPosition.x(), stepPosition.y() );

    return  stepPosition;
}


