#pragma once

#include "CurveBase.h"

static
void
TestCurves()
{
    cCurveBase< int >* maCurve = new cCurveBase< int >();

    maCurve->SetValueAtTime( 1, 10 );
    maCurve->SetValueAtTime( 2, 100 );

    int interp = maCurve->GetValueAtTime( 0.5 );
    int bp = 0;
}