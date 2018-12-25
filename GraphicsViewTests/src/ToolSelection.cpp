#include "ToolSelection.h"


cToolSelection::~cToolSelection()
{
}


cToolSelection::cToolSelection()
{
}


void
cToolSelection::AddPathPoint( const QPoint & iPoint )
{
    mPath.push_back( iPoint );
}


void
cToolSelection::DoSelectionOnImage( QImage * ioImage )
{
}


void
cToolSelection::MergeSelectionInImage( QImage * ioImage )
{
}


void
cToolSelection::MoveSelectionToPoint( const QPoint & iPoint )
{
    mSelectionBBox.moveTo( iPoint );
    // Should we move all points from path as well ?
}


const QRect&
cToolSelection::GetSelectionBBox() const
{
    return  mSelectionBBox;
}


