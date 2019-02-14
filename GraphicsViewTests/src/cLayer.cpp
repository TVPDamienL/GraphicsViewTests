#include "cLayer.h"


cLayer::~cLayer()
{
    delete  mImage;
    for( auto image : mDrawingHistory )
        delete image;
}


cLayer::cLayer( unsigned int iWidth, unsigned int iHeight ) :
    mCurrentHistoryIndex( 0 )
{
    //mImage = new QImage( iWidth, iHeight, QImage::Format_ARGB32_Premultiplied );
    mImage = new QImage( "Resources/lapinGrid.png" );

    //mImage->fill( 0xFF333333 );
    //mImage->fill( 0x00000000 ); // Clear with transparent


    // Manual version
    //uchar*          data = emptyImage->bits();
    //unsigned int    size = emptyImage->width() * emptyImage->height() * 4;
    //for( int i = 0; i < size; i += 4 )
    //{
    //    Format is read reversed : BGRA
    //    data[ i ]       = 255;    //B
    //    data[ i + 1 ]   = 50;     //G
    //    data[ i + 2 ]   = 50;     //R
    //    data[ i + 3 ]   = 255;    //A
    //}

    WriteUndoHistory(); // First undo state, the empty image
}


QImage*
cLayer::Image()
{
    return  mImage;
}


void
cLayer::WriteUndoHistory()
{
    // If we are not on the last image of the history, we erase history from afterCurrent to last
    if( mCurrentHistoryIndex < int( mDrawingHistory.size() - 1 ) )
    {
        for( int i = mCurrentHistoryIndex + 1; i < mDrawingHistory.size(); ++i )
            delete  mDrawingHistory[ i ];

        mDrawingHistory.erase( mDrawingHistory.begin() + mCurrentHistoryIndex + 1, mDrawingHistory.end() );
    }

    // Here we copy the current image, so we won't change it afterward
    mDrawingHistory.push_back( new QImage( *mImage ) );
    // Now we are at the end of the history
    mCurrentHistoryIndex = int( mDrawingHistory.size() ) - 1;
}


bool
cLayer::Undo()
{
    if( mCurrentHistoryIndex <= 0 )
        return  false;

    // Need to update the drawing buffer, so that if we draw from now, we'll draw in a buffer, not the actual snap in history
    delete  mImage;
    mImage = new QImage( *mDrawingHistory[ --mCurrentHistoryIndex ] );

    return  true;
}


bool
cLayer::Redo()
{
    if( mCurrentHistoryIndex >= mDrawingHistory.size() - 1 )
        return  false;

    // Need to update the drawing buffer, so that if we draw from now, we'll draw in a buffer, not the actual snap in history
    delete  mImage;
    mImage = new QImage( *mDrawingHistory[ ++mCurrentHistoryIndex ] );

    return  true;
}
