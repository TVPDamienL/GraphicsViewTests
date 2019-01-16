#include "ToolBase.h"

#include <QVector2D>
#include <QDebug>

ToolBase::~ToolBase()
{
}


ToolBase::ToolBase( QObject *parent ) :
    QStandardItemModel( parent ),
    mAlphaMask( 0 )
{
    mStep = 10.0F;
    mLastRenderedPathIndex = 0;
    mRequiredStepLength = mStep;
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


QColor
ToolBase::getColor() const
{
    QVariant dataColor = itemFromIndex( index( 1, 0 ) )->data();
    return  dataColor.value< QColor >();
}


void
ToolBase::setColor( const QColor & iColor )
{
    itemFromIndex( index( 1, 0 ) )->setData( iColor );
    mColor = iColor;
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
    mRequiredStepLength = 0;
    mLastRenderedPathIndex = 0;
}


void
ToolBase::DrawFullPath( QImage * iImage )
{
    if( mPath.size() <= 0 )
        return;

    mRequiredStepLength = 0;
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
        QPoint p1 = mPath[ mLastRenderedPathIndex ].mPosition;
        QPoint p2 = mPath[ mLastRenderedPathIndex + 1 ].mPosition;
        float  pressure_p1 = mPath[ mLastRenderedPathIndex ].mPressure;
        float  pressure_p2 = mPath[ mLastRenderedPathIndex + 1 ].mPressure;
        float  rotation_p1 = mPath[ mLastRenderedPathIndex ].mRotation;
        float  rotation_p2 = mPath[ mLastRenderedPathIndex + 1 ].mRotation;
        float distance = Distance2Points( p1, p2 );

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
        QPoint startingPoint = p1;


        // If spare steps from previous it, we apply it, and then use this point as first
        if( abs( mRequiredStepLength - mStep ) > 0.1F )
        {
            startingPoint = __DrawDotVectorTruc_RequiresAName_( iImage, p1, stepVectorNormalizedAsPF * mRequiredStepLength, pressure_p1, rotation_p1 );
            remainingDistance -= mRequiredStepLength;
        }

        // Now, we go step by step
        int count = 1; // Counting how many split have been drawn


        // We split the segment using mStep, and draw dots on each step while there is room on the segment
        while( remainingDistance >= mStep )
        {
            float pressure = std::abs(pressure_p2 - pressure_p1) * (1.0 - remainingDistance/distance) + std::min(pressure_p1, pressure_p2);
            float rotation = std::abs(rotation_p2 - rotation_p1) * (1.0 - remainingDistance/distance) + std::min(rotation_p1, rotation_p2);
            __DrawDotVectorTruc_RequiresAName_( iImage, startingPoint, stepVectorNormalizedAsPF * mStep * count, pressure, rotation );
            remainingDistance -= mStep;
            ++count;
        }

        // Here, we probably have space between the last dot and the next point, not enough for a step, but still space, so we remember it for next iteration
        mRequiredStepLength = mStep - remainingDistance;

    }
}


void
ToolBase::EndDrawing()
{
    mPath.clear();
}


// ===========================


void
ToolBase::PathAddPoint( sPointData iPoint )
{
    mPath.push_back( iPoint );
}


void
ToolBase::SetAlphaMask( QImage* iImage )
{
    mAlphaMask = iImage;
}


void
ToolBase::ClearAlphaMask()
{
    mAlphaMask = 0;
}


void
ToolBase::_DrawPixel( uchar * iData, unsigned int iImageWidth, unsigned int iImageHeight, int iX, int iY, int iR, int iG, int iB, int iA )
{
    int x = iX < 0 ? 0 : iX;
    x = x >= iImageWidth ? iImageWidth - 1 : x;

    int y = iY < 0 ? 0 : iY;
    y = y >= iImageHeight ? iImageHeight - 1 : y;

    int index = y * iImageWidth * 4 + x * 4;
    int r = iR;
    int g = iG;
    int b = iB;
    int a = iA;

    if( mAlphaMask && mAlphaMask->width() == iImageWidth && mAlphaMask->height() == mAlphaMask->height() )
    {
        float alphaMaskTransparency = float(mAlphaMask->bits()[ index + 3 ]) / 255.F;
        r *= alphaMaskTransparency;
        g *= alphaMaskTransparency;
        b *= alphaMaskTransparency;
        a *= alphaMaskTransparency;
    }

    float transparencyAmountInverse = 1.F - ( float( a ) / 255.F );

    // BGRA format and premultiplied alpha
    // Premultiplied allows this simple equation, basically we do a weighted sum of source and destination, weighted by the src's alpha
    // So we basically keep as much dst as src is transparent -> the more src is transparent, the more we want dst's color, so -> mult by 1-alpha, alpha between 0 and 1
    iData[ index ]      = b + iData[ index ]      * transparencyAmountInverse;
    iData[ index + 1 ]  = g + iData[ index + 1 ]  * transparencyAmountInverse;
    iData[ index + 2 ]  = r + iData[ index + 2 ]  * transparencyAmountInverse;
    iData[ index + 3 ]  = a + iData[ index + 3 ]  * transparencyAmountInverse;

}


// ===========================
// ===========================


QPoint
ToolBase::__DrawDotVectorTruc_RequiresAName_( QImage* iImage, const QPoint& iStart, const QPointF& iVector,  float iPressure, float iRotation )
{
    // This takes a starting point, offsets by a vector, draws a dot there and return the offset point's coordinates
    QPoint stepVector = QPoint( std::roundf( iVector.x() ), std::roundf( iVector.y() ) );
    QPoint stepPosition = iStart + stepVector;

    DrawDot( iImage, stepPosition.x(), stepPosition.y(), iPressure, iRotation );

    return  stepPosition;
}


