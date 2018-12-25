#pragma once


#include "ToolSelection.h"


class cToolSelectionRectangle :
    cToolSelection
{
public:
    cToolSelectionRectangle();
    ~cToolSelectionRectangle();

public:
    void  Select( const QRect& iRectangle );

public:
    virtual  void  DoSelectionOnImage( QImage* ioImage ) override;
    virtual  void  MergeSelectionInImage( QImage* ioImage ) override;
};



