#pragma once


#include <QImage>

#include <vector>


class cToolSelection
{
public:
    cToolSelection();
    ~cToolSelection();

public:
    void  AddPathPoint( const QPoint& iPoint );

    // Will cut out the selected part, and put it in mImageSelected
    virtual  void  DoSelectionOnImage( QImage* ioImage );
    virtual  void  MergeSelectionInImage( QImage* ioImage );

    void            MoveSelectionToPoint( const QPoint& iPoint );
    const QRect&    GetSelectionBBox() const;



protected:
    std::vector< QPoint >   mPath;
    QRect                   mSelectionBBox;

    QImage*                 mImageSelected;
};



