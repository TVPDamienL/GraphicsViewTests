#include "ToolSelectionRectangle.h"


cToolSelectionRectangle::~cToolSelectionRectangle()
{
}


cToolSelectionRectangle::cToolSelectionRectangle() :
    cToolSelection()
{
    mImageSelected = 0;
}


void
cToolSelectionRectangle::Select( const QRect & iRectangle )
{
    mSelectionBBox = iRectangle;
}


void
cToolSelectionRectangle::DoSelectionOnImage( QImage * ioImage )
{
    if( mSelectionBBox.isEmpty() )
        return;

    delete  mImageSelected;
    mImageSelected = new QImage( mSelectionBBox.width(), mSelectionBBox.height(), QImage::Format_ARGB32_Premultiplied );

    uchar* selectionData = mImageSelected->bits();
    uchar* srcData = ioImage->bits();

    for( int x = mSelectionBBox.x(); x < mSelectionBBox.x() + mSelectionBBox.width(); ++x )
        for( int y = mSelectionBBox.y(); y < mSelectionBBox.y() + mSelectionBBox.height(); ++y )
        {
            unsigned int selIndex = (y - mSelectionBBox.y() * mSelectionBBox.width() * 4 ) + ( x - mSelectionBBox.x() ) * 4;
            unsigned int srcIndex = y * ioImage->width() * 4 + x * 4;

            selectionData[ selIndex ]   = srcData[ srcIndex ];
            selectionData[ selIndex+1 ] = srcData[ srcIndex+1 ];
            selectionData[ selIndex+2 ] = srcData[ srcIndex+2 ];
            selectionData[ selIndex+3 ] = srcData[ srcIndex+3 ];

            srcData[ srcIndex ]        = 0;
            srcData[ srcIndex + 1 ]    = 0;
            srcData[ srcIndex + 2 ]    = 0;
            srcData[ srcIndex + 3 ]    = 0;
        }
}


void
cToolSelectionRectangle::MergeSelectionInImage( QImage * ioImage )
{
    if( mSelectionBBox.isEmpty() )
        return;


    uchar* selectionData = mImageSelected->bits();
    uchar* srcData = ioImage->bits();

    for( int x = mSelectionBBox.x(); x < mSelectionBBox.x() + mSelectionBBox.width(); ++x )
        for( int y = mSelectionBBox.y(); y < mSelectionBBox.y() + mSelectionBBox.height(); ++y )
        {
            unsigned int selIndex = ( y - mSelectionBBox.y() * mSelectionBBox.width() * 4 ) + ( x - mSelectionBBox.x() ) * 4;
            unsigned int srcIndex = y * ioImage->width() * 4 + x * 4;

            srcData[ srcIndex ]     = selectionData[ selIndex ];
            srcData[ srcIndex + 1 ] = selectionData[ selIndex + 1 ];
            srcData[ srcIndex + 2 ] = selectionData[ selIndex + 2 ];
            srcData[ srcIndex + 3 ] = selectionData[ selIndex + 3 ];
        }

    delete  mImageSelected;
}


