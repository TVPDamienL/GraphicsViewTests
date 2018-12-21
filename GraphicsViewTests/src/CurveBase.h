#pragma once

#include <vector>


#define  CURVE_PRECISION 0.001F


template< typename tType >
class cCurveBase
{
public:
    virtual ~cCurveBase();
    cCurveBase();


    // Curve manipulation
public:
    void    SetValueAtTime( double iTime, tType iValue );
    tType   GetValueAtTime( double iTime ) const;
    void    AddKeyAtTime( double iTime, tType iValue );


    // Index and key searching
public:
    int  ExactKeyIndexByTime( double iTime ) const;
    // This will return the insertion index for given time, so, what's the index i should insert a key, that has iTime as time value
    int _GetInsertionIndexByTime_( double iTime ) const; // Name isn't best here


private:
    struct cKeyframe
    {
        double  mTime;
        tType   mValue;
        // cTangent mTangent;
    };

    std::vector< cKeyframe > mKeys;
};


// ===============================================================
// ===============================================================
// ===============================================================
// ===============================================================


template< typename tType >
cCurveBase< tType >::~cCurveBase()
{
}


template< typename tType >
cCurveBase< tType >::cCurveBase()
{
}


template< typename tType >
void
cCurveBase< tType >::SetValueAtTime( double iTime, tType iValue )
{
    int index = ExactKeyIndexByTime( iTime );
    if( index >= 0 )
    {
        mKeys[ index ].mValue = iValue;
    }
    else
    {
        AddKeyAtTime( iTime, iValue );
    }
}


template< typename tType >
tType
cCurveBase< tType >::GetValueAtTime( double iTime ) const
{
    int index = ExactKeyIndexByTime( iTime );
    if( index >= 0 )
    {
        return  mKeys[ index ].mValue;
    }
    else
    {
        if( mKeys.empty() )
        {
            return  tType();
        }
        else if( mKeys.size() == 1 )
        {
            return  mKeys[ 0 ].mValue;
        }
        else
        {
            int indexLeftKey = _GetInsertionIndexByTime_( iTime ) - 1;

            if( indexLeftKey >= mKeys.size() - 1 ) // Asked for time on or after last key -> return last key value
            {
                return  mKeys.back().mValue;
            }
            else if( indexLeftKey < 0 ) // Asked for a value before first key
            {
                return  mKeys[ 0 ].mValue;
            }
            else
            {
                // Interpolate from indexLeftKey to indexLeftKey + 1
                double  timeLeft  = mKeys[ indexLeftKey ].mTime;
                double  timeRight = mKeys[ indexLeftKey + 1 ].mTime;
                tType   keyLeft     = mKeys[ indexLeftKey ].mValue;
                tType   keyRight    = mKeys[ indexLeftKey + 1 ].mValue;

                return  ( ( iTime - timeLeft ) * ( keyRight - keyLeft ) )
                        /
                        ( timeRight - timeLeft )
                        + keyLeft;
            }
        }
    }

    return  tType();
}


template< typename tType >
void
cCurveBase< tType >::AddKeyAtTime( double iTime, tType iValue )
{
    if( ExactKeyIndexByTime( iTime ) >= 0 )
        return;

    cKeyframe key;
    key.mTime = iTime;
    key.mValue = iValue;

    int insertionIndex = _GetInsertionIndexByTime_( iTime );
    mKeys.insert( mKeys.begin() + insertionIndex, key );
}


template< typename tType >
int
cCurveBase< tType >::ExactKeyIndexByTime( double iTime ) const
{
    for( int i = 0; i < mKeys.size(); ++i )
    {
        if( abs( mKeys[ i ].mTime - iTime ) < CURVE_PRECISION )
            return  i;
    }

    return  -1;
}


template< typename tType >
int
cCurveBase< tType >::_GetInsertionIndexByTime_( double iTime ) const
{
    for( int i = 0; i < mKeys.size(); ++i )
    {
        if( mKeys[ i ].mTime > iTime )
            return  i;
    }

    return  mKeys.size();
}
