#pragma once

#include "PaintToolBase.h"
#include "cSelection.h"

class cToolSelectionTest :
    public cPaintToolBase
{
    Q_OBJECT

public:
    ~cToolSelectionTest();
    cToolSelectionTest( QObject* iParent = Q_NULLPTR );

public:
    Qt::ItemFlags   flags( const QModelIndex& iIndex ) const override;

public:
    // Overrides
    virtual  void  StartDrawing( QImage* iImage, sPointData iPointData ) override;

    virtual  QRect  MoveDrawing( sPointData iPointData ) override;

    virtual  void  DrawDot( float x, float y, float iPressure, float iRotation ) override;
    virtual  void  DrawLine( const QPointF& iP1, const QPointF& iP2, float iPressure1, float iRotation1, float iPressure2, float iRotation2 ) override;

    virtual  QRect EndDrawing( sPointData iPointData ) override;

public:
    void SetSelection( cSelection* iSelection );

private:
    cSelection* mTheSelection;
};

