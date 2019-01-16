#pragma once

#include "ToolBase.h"
#include "cSelection.h"

class cToolSelectionTest :
    public ToolBase
{
    Q_OBJECT

public:
    ~cToolSelectionTest();
    cToolSelectionTest( QObject* iParent = Q_NULLPTR );

public:
    Qt::ItemFlags   flags( const QModelIndex& iIndex ) const override;

public:
    // Overrides
    virtual  void  StartDrawing();

    virtual  void  DrawDot( QImage* iImage, int x, int y, float iPressure, float iRotation ) override;
    virtual  void  DrawLine( QImage* iImage, int x1, int y1, int x2, int y2 ) override;

public:
    void SetSelection( cSelection* iSelection );

private:
    cSelection* mTheSelection;
};

