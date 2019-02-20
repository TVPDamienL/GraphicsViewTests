#include "cLayer.h"


#include "GPUForThisApp.h"
#include "BenchmarkStuff.h"


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

    //mImage = _GPU->GaussianBlurGPU1D2Pass( mImage );


    //qDebug() << " CPU ================ ";
    //BENCHSTART
        //mImage = GaussianBlurCPPDoublePass( mImage );
    //BENCHEND


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
