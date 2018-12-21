#pragma once



template< typedef tType >
class cCurveBase
{
    struct eKeyframe
    {
        int     mTime;
        tType   mValue;
        // cTangent mTangent;
    };


public:
    virtual ~cCurveBase();
    cCurveBase();



protected:
    std::vector< eKeyframe > mKeys;

};

