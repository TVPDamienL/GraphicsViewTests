#pragma once

#include "cHUDObject.h"

#include "cHUDHandle.h"
#include "cSelection.h"

class cHUDTransform :
    public cHUDObject
{
public:
    ~cHUDTransform();
    cHUDTransform( cHUDView* iParentView, cHUDObject* iParentObject );

public:
    virtual  void   Draw( QPainter* iPainter ) override;
    virtual  void   SetFrame( const QRect& iFrame ) override;

    virtual  bool   ContainsPoint( const QPoint& iPoint ) const override;

    virtual  bool  Event( QEvent* iEvent ) override;

public:
    void                SetSelection( cSelection* iSelection );
    void                SelectionChangedEvent( cBaseData* iSender, int iArg );

private:
    void            _LayoutChildren();
    cHUDHandle*     _GetHandleAtPoint( const QPoint& iPoint );

private:
    int             mHandleSize = 10;
    cHUDHandle*     mFocusedHandle = 0;
    cSelection*     mSelection;
};

