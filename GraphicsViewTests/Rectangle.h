#pragma once

#include "ShapeBase.h"
#include "qcompilerdetection.h"  // for Q_NULLPTR
#include "qobjectdefs.h"         // for Q_OBJECT
#include "qpoint.h"              // for QPoint
#include "qrect.h"               // for QRect
#include "qstring.h"             // for QString

class QImage;
class QObject;
struct sPointData;

class cShapeRectangle :
    public cShapeBase
{
    Q_OBJECT

public:
    enum eMode
    {
        kFill,
        kStroke
    };


public:
    ~cShapeRectangle();
    cShapeRectangle( QObject* iParent = Q_NULLPTR );

public:
    // Overrides
    virtual  void       StartDrawing( QImage* iImage, sPointData iPointData ) override;
    virtual  QRect      MoveDrawing( sPointData iPointData ) override;
    virtual  QRect      EndDrawing( sPointData iPointData ) override;
    virtual  void       CancelDrawing() override;

public:
    void    SetMode( eMode iMode );

private:
    eMode           mMode = kFill;
};

