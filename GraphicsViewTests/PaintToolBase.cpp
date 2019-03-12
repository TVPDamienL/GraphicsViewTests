#include "PaintToolBase.h"

#include "Math.Fast.h"

#include <QDebug>

#include "algorithm"       // for min
#include "cmath"           // for abs, roundf
#include "corecrt_math.h"  // for roundf
#include "cstdlib"         // for abs
#include "qlogging.h"      // for qDebug
#include "qvector2d.h"     // for QVector2D
#include "type_traits"     // for move

cPaintToolBase::~cPaintToolBase()
{
}


cPaintToolBase::cPaintToolBase( QObject *parent ) :
    ToolBase( parent ),
    mAlphaMask( 0 )
{
    mStep = 0.1;
    mLastRenderedPathIndex = 0;
    mRequiredStepLength = _GetStepInPixelValue();
}


// ===========================


int
cPaintToolBase::getSize() const
{
    return  mToolSize;
    //return  itemFromIndex( index( 0, 0 ) )->data().toInt();
}


void
cPaintToolBase::setSize( int iSize )
{
    //itemFromIndex( index( 0, 0 ) )->setData( iSize );
    mToolSize = iSize;
}


QColor
cPaintToolBase::getColor() const
{
    // This is so much easier in the end ...
    return  mColor;

    //QVariant dataColor = itemFromIndex( index( 1, 0 ) )->data();
    //return  dataColor.value< QColor >();
}


void
cPaintToolBase::setColor( const QColor & iColor )
{
    //itemFromIndex( index( 1, 0 ) )->setData( iColor );
    mColor = iColor;
}


float
cPaintToolBase::getStep() const
{
    return  mStep;
}


void
cPaintToolBase::setStep( float iStep )
{
    mStep = iStep;
    //itemFromIndex( index( 2, 0 ) )->setData( iStep );
}


float
cPaintToolBase::getOpacity() const
{
    return  mOpacity ;
}


void
cPaintToolBase::setOpacity ( float iOpacity  )
{
    mOpacity  = iOpacity ;
}


// ===========================


void
cPaintToolBase::StartDrawing( QImage* iDC, sPointData iPointData )
{
    mRequiredStepLength = 0;
    mLastRenderedPathIndex = 0;
    mDrawingContext = iDC;

    // If new context, that isn't the same as before, we copy the image data into a float buffer, to enhance precision to the highest amount
    if( _mPreviousDrawingContext != iDC )
    {
        delete[] _mFloatBuffer;
        _mFloatBuffer = new float[ iDC->bytesPerLine() * iDC->height() ];
        uchar* data = iDC->bits();
        uchar* scan = data;

        for( int y = 0; y < iDC->height(); ++y )
        {
            scan = data + y * iDC->bytesPerLine();
            const int floatIndex = y * iDC->width() * 4;

            for( int x = 0; x < iDC->width() * 4; ++x )
            {
                uchar comp = *scan;
                _mFloatBuffer[ floatIndex + x ] = *scan; ++scan;
            }
        }
    }

    _mPreviousDrawingContext = iDC;
}


QRect
cPaintToolBase::MoveDrawing( sPointData iPointData )
{
    return  mDirtyArea;
}


void
cPaintToolBase::DrawFullPath()
{
    if( mPath.size() <= 0 )
        return;

    mRequiredStepLength = 0;
    mLastRenderedPathIndex = 0;

    DrawPathFromLastRenderedPoint();

    mPath.clear(); // We draw the path, we clear it
}


void
cPaintToolBase::DrawPathFromLastRenderedPoint()
{
    if( mPath.size() <= 0 )
        return;

    for( mLastRenderedPathIndex; mLastRenderedPathIndex < mPath.size() - 1; ++mLastRenderedPathIndex )
    {
        // Setting base variables : starting point, ending point and their distance
        QPoint p1 = mPath[ mLastRenderedPathIndex ].mPosition;
        QPoint p2 = mPath[ mLastRenderedPathIndex + 1 ].mPosition;
        float  pressure_p1 = mPath[ mLastRenderedPathIndex ].mPressure;
        float  pressure_p2 = mPath[ mLastRenderedPathIndex + 1 ].mPressure;
        float  rotation_p1 = mPath[ mLastRenderedPathIndex ].mRotation;
        float  rotation_p2 = mPath[ mLastRenderedPathIndex + 1 ].mRotation;
        float distance = Distance2Points( p1, p2 );
        float subPression = pressure_p2 - pressure_p1;
        float subRotation = rotation_p2 - rotation_p1;

        // Two identical points -> SKIP
        if( abs( distance ) < 0.01 )
            continue;

        // Not enough distance on that path segment to draw a dot, so we'll see on the next one, after decreasing the requiredToStep
        if( mRequiredStepLength - distance > 0 )
        {
            mRequiredStepLength -= distance;
            continue;
        }

        // Getting the directionnal vector, that'll be used to find all points on the segment
        QVector2D stepVectorNormalized = QVector2D( p2 - p1 ).normalized();
        QPointF stepVectorNormalizedAsPF = stepVectorNormalized.toPointF();


        // From here, we split segment and draw dots
        // ==================================
        // ==================================

        float remainingDistance = distance; // The distance left of the segment, that still needs split
        //qDebug() << "Remaining : " << remainingDistance;
        QPoint startingPoint = p1;


        // If spare steps from previous it, we apply it, and then use this point as first
        if( abs( mRequiredStepLength - _GetStepInPixelValue() ) > 0.1F )
        {
            startingPoint = __DrawDotVectorTruc_RequiresAName_( p1, stepVectorNormalizedAsPF * mRequiredStepLength, pressure_p1, rotation_p1 );
            remainingDistance -= mRequiredStepLength;
        }

        // Now, we go step by step
        int count = 1; // Counting how many split have been drawn


                       // We split the segment using mStep, and draw dots on each step while there is room on the segment
        while( remainingDistance >= _GetStepInPixelValue() )
        {
            float ratio = 1.0 - remainingDistance/distance;

            float pressure = pressure_p1 + subPression * ratio;
            float rotation = rotation_p1 + subRotation * ratio;
            __DrawDotVectorTruc_RequiresAName_( startingPoint, stepVectorNormalizedAsPF * _GetStepInPixelValue() * count, pressure, rotation );
            remainingDistance -= _GetStepInPixelValue();
            ++count;
        }

        // Here, we probably have space between the last dot and the next point, not enough for a step, but still space, so we remember it for next iteration
        mRequiredStepLength = _GetStepInPixelValue() - remainingDistance;
    }
}


QRect
cPaintToolBase::EndDrawing( sPointData iPointData )
{
    mPath.clear();
    return  mDirtyArea;
}


// ===========================


void
cPaintToolBase::PathAddPoint( sPointData iPoint )
{
    mPath.push_back( iPoint );
}


QRect
cPaintToolBase::GetDirtyArea() const
{
    return  mDirtyArea;
}


QRect
cPaintToolBase::GetDirtyAreaAndReset()
{
    QRect area = mDirtyArea;
    mDirtyArea = QRect( 0, 0, 0, 0 );
    return  area;
}


void
cPaintToolBase::SetAlphaMask( QImage* iImage )
{
    mAlphaMask = iImage;
}


void
cPaintToolBase::ClearAlphaMask()
{
    mAlphaMask = 0;
}


// ===========================
// ===========================


QPoint
cPaintToolBase::__DrawDotVectorTruc_RequiresAName_( const QPoint& iStart, const QPointF& iVector,  float iPressure, float iRotation )
{
    // This takes a starting point, offsets by a vector, draws a dot there and return the offset point's coordinates
    QPoint stepVector = QPoint( std::roundf( iVector.x() ), std::roundf( iVector.y() ) );
    QPoint stepPosition = iStart + stepVector;

    DrawDot( stepPosition.x(), stepPosition.y(), iPressure, iRotation );

    return  stepPosition;
}


float
cPaintToolBase::_GetStepInPixelValue() const
{
    return  std::max( mStep * mToolSize, 1.0F );
}
