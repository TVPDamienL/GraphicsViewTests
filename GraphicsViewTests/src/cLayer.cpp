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


    //QImage* output = new QImage( 1920, 1080, QImage::Format_ARGB32_Premultiplied );
    //output->fill( Qt::blue );

    //QTransform trans;

    //const QPoint translation( 10, 10 );
    ////const float rotation = 0;
    //const float rotation = 45.0F / 180.0F * 3.14F;
    //const float scale = 0.5;
    //const float cosRot = cos( rotation );
    //const float sinRot = sin( rotation );

    //trans.setMatrix( cosRot * scale, sinRot * scale, 0, -sinRot * scale, cosRot * scale, 0, translation.x(), translation.y(), 1 );



    //_GPU->LoadSelectionOriginalBuffer( mImage );
    //_GPU->LoadSelectionOutputImage( output );
    //_GPU->PerformTransformation( trans, QPoint( 0, 0 ) );

    //delete  mImage;
    //mImage = output;

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
