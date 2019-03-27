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
    mStep = 1.0;
    mLastRenderedPathIndex = 0;
    mLeftover = _GetHalfStepInPixelValue( 1.0 );
    mTruc = false;
}


// ===========================


int
cPaintToolBase::getSize() const
{
    return  mToolSize;
}


void
cPaintToolBase::setSize( int iSize )
{
    mToolSize = iSize;
}


QColor
cPaintToolBase::getColor() const
{
    return  mColor;
}


void
cPaintToolBase::setColor( const QColor & iColor )
{
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
    mPath.clear();
    mLeftover = 0;
    mTruc = false;
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

    mLeftover = 0;
    mTruc = false;
    mLastRenderedPathIndex = 0;

    DrawPathFromLastRenderedPoint();
}


void
cPaintToolBase::DrawPathFromLastRenderedPoint()
{
    DrawPathFromPointToPoint( mLastRenderedPathIndex, mPath.size() - 1 );
    mLastRenderedPathIndex = mPath.size() - 1;
}


void
cPaintToolBase::DrawPathFromPointToPoint( int a, int b )
{
    if( mPath.size() <= 1 )
        return;
    mTruc = false;

    for( int i = a; i < b; ++i )
    {
        // Setting base variables : starting point, ending point and their distance
        QPoint p1 = mPath[ i ].mPosition;
        QPoint p2 = mPath[ i + 1 ].mPosition;
        float  pressure_p1 = mPath[ i ].mPressure;
        float  pressure_p2 = mPath[ i + 1 ].mPressure;
        float  rotation_p1 = mPath[ i ].mRotation;
        float  rotation_p2 = mPath[ i + 1 ].mRotation;
        float  distance = Distance2Points( p1, p2 );
        float  subPression = pressure_p2 - pressure_p1;
        float  subRotation = rotation_p2 - rotation_p1;

        // Two identical points -> SKIP
        if( abs( distance ) < 0.01 )
            continue;

        // Not enough distance on that path segment to draw a dot, so we'll see on the next one, after decreasing the requiredToStep
        if( mLeftover - distance > 0 )
        {
            mLeftover -= distance;
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
        QPointF stepSum = stepVectorNormalizedAsPF * mLeftover;
        remainingDistance -= mLeftover;

        while( remainingDistance >= 0 )
        {
            float ratio = 1.0 - (remainingDistance)/distance;
            float pressure = pressure_p1 + subPression * ratio;

            startingPoint = __DrawDotVectorTruc_RequiresAName_( startingPoint, stepSum, pressure, rotation_p1 );

            float pixelStep = _GetHalfStepInPixelValue( pressure ) * 2;
            stepSum = stepVectorNormalizedAsPF * pixelStep;
            remainingDistance -= pixelStep;
        }

        mLeftover = -remainingDistance; // because  remaining will be negative
    }
}


QRect
cPaintToolBase::EndDrawing( sPointData iPointData )
{
    return  mDirtyArea;
}


void
cPaintToolBase::CancelDrawing()
{
}


// ===========================


void
cPaintToolBase::PathAddPoint( sPointData iPoint )
{
    if( mPath.size() > 0 && iPoint == mPath.back() )
        return;

    //iPoint.mPosition.setY( 950 );
    mPath.push_back( iPoint );
}


const std::vector<sPointData>&
cPaintToolBase::Path() const
{
    return  mPath;
}


void
cPaintToolBase::SetPath( const std::vector<sPointData>& iPath )
{
    mPath = iPath;
}


void
cPaintToolBase::PathSetPoint( int index, sPointData & point )
{
    mPath[ index ] = point;
}


const sPointData&
cPaintToolBase::PathGetPoint( int index )
{
    return  mPath[ index ];
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


//float
//cPaintToolBase::_GetStepInPixelValue( float iPressure ) const
//{
//    return  std::max( mStep * mToolSize*2 * iPressure, 1.0F );
//}


float
cPaintToolBase::_GetHalfStepInPixelValue( float iPressure ) const
{
    return  std::max( mStep * mToolSize * iPressure, 1.0F );
}
