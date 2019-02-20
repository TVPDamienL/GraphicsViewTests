#pragma once

#include "Undoable.h"


#include <QImage>

#include <vector>


class cLayer :
    cUndoable
{
public:
    virtual ~cLayer();
    cLayer( unsigned int iWidth, unsigned int iHeight );

    QImage* Image();


public:
    void            WriteUndoHistory(); // Takes a snap of the current image and saves it in the history
    virtual bool    Undo() override;
    virtual bool    Redo() override;

private:
    QImage*     mImage;


    std::vector< QImage* >  mDrawingHistory;        // Dirty whole image saving for now
    int                     mCurrentHistoryIndex;   // Where are we in the history
};


