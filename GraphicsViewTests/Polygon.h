#include "ShapeBase.h"


class QImage;
class QObject;
struct sPointData;

class cShapePolygon :
    public cShapeBase
{
public:
    enum eMode
    {
        kFill,
        kStroke
    };


public:
    ~cShapePolygon();
    cShapePolygon( QObject* iParent = Q_NULLPTR );

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
    QRect           mBBox;
    QImage*         mWorkingBuffer = 0;
};