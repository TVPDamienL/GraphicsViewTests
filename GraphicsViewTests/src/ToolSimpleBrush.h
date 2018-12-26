#pragma once


#include "ToolBase.h"
#include "CurveBase.h"

#include <QColor>


class cToolSimpleBrush :
    public ToolBase
{
    Q_OBJECT

public:
    virtual ~cToolSimpleBrush();
    cToolSimpleBrush( QObject* iParent = Q_NULLPTR );


public:
    Qt::ItemFlags   flags( const QModelIndex& iIndex ) const override;
    void  buildTool();

public:
    void  ApplyProfile( bool iApply );
    bool  ApplyProfile() const;


private:

public:
    // Overrides
    virtual  void  DrawDot( QImage* iImage, int x, int y, float iPressure, float iRotation ) override;
    virtual  void  DrawLine( QImage* iImage, int x1, int y1, int x2, int y2 ) override;


private:
    cCurveBase< float > mProfile;
    bool                mApplyProfile;
};

