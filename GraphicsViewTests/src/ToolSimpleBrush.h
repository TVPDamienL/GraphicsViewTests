#pragma once


#include "ToolStamp.h"
#include "CurveBase.h"

#include <QColor>
#include <qobject.h>

class cToolSimpleBrush :
    public cToolStamp
{
    Q_OBJECT

public:
    virtual ~cToolSimpleBrush();
    cToolSimpleBrush( QObject* iParent = Q_NULLPTR );


public:
    Qt::ItemFlags   flags( const QModelIndex& iIndex ) const override;
    virtual void  buildTool() override;


public:
    void    ApplyProfile( bool iApply );
    bool    ApplyProfile() const;


protected:
    virtual void  RenderTip( int x, int y ) override;


private:
    cCurveBase< float > mProfile;
    bool                mApplyProfile;
};

