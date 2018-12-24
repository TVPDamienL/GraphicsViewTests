#pragma once

#include "Undoable.h"

#include <vector>


class cLayer :
    cUndoable
{
public:
    virtual ~cLayer();
    cLayer( unsigned int iWidth, unsigned int iHeight );

    void            SetData( unsigned char* iData );
    unsigned char*  Data();


public:
    void  RegisterUndo();
    virtual bool  Undo() override;
    virtual bool  Redo() override;

private:
    unsigned char* _CopyData();

private:
    unsigned char* mData; // The image
    unsigned int mWidth;
    unsigned int mHeight;


    std::vector< unsigned char* >   mUndoStack; // Dirty whole image saving for now
    int                             mUndoStackCurrentIndex;
};


