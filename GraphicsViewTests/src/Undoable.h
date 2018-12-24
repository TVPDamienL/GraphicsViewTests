#pragma once


class cUndoable
{
public:
    virtual bool  Undo() = 0;
    virtual bool  Redo() = 0;
};


