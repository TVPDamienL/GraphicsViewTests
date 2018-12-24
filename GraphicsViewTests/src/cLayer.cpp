#include "cLayer.h"


cLayer::~cLayer()
{
    delete[]  mData;
    for( auto ptr : mUndoStack )
        delete[] ptr;
}


cLayer::cLayer( unsigned int iWidth, unsigned int iHeight ) :
    mWidth( iWidth ),
    mHeight( iHeight ),
    mUndoStackCurrentIndex( 0 )
{
    unsigned int size = mWidth * mHeight * 4;
    mData = new unsigned char[ size ];

    for( int i = 0; i < size; i += 4 )
    {
        mData[ i ]      = 255;
        mData[ i+1 ]    = 50;
        mData[ i+2 ]    = 50;
        mData[ i+3 ]    = 255;
    }

    RegisterUndo();
}


void
cLayer::SetData( unsigned char * iData )
{
    mData = iData;
}


unsigned char*
cLayer::Data()
{
    return  mData;
}


void
cLayer::RegisterUndo()
{
    if( mUndoStackCurrentIndex < mUndoStack.size() - 1 )
    {
        for( int i = 0; i < mUndoStack.size() - 1; ++i )
            delete[] mUndoStack[ i ];

        mUndoStack.erase( mUndoStack.begin() + mUndoStackCurrentIndex, mUndoStack.end() );
    }

    mUndoStack.push_back( _CopyData() );
}


bool
cLayer::Undo()
{
    if( mUndoStackCurrentIndex <= 0 )
        return  false;

    mData = mUndoStack[ --mUndoStackCurrentIndex ];

    return  true;
}


bool
cLayer::Redo()
{
    if( mUndoStackCurrentIndex > mUndoStack.size() - 1 )
        return  false;

    mData = mUndoStack[ ++mUndoStackCurrentIndex ];

    return  true;
}


unsigned char *
cLayer::_CopyData()
{
    unsigned int size = mWidth * mHeight * 4;

    unsigned char* copy = new unsigned char[ size ];
    for( int i = 0; i < size; i += 4 )
    {
        copy[ i ]       = mData[ i ];
        copy[ i + 1 ]   = mData[ i + 1 ];
        copy[ i + 2 ]   = mData[ i + 2 ];
        copy[ i + 3 ]   = mData[ i + 3 ];
    }

    return  copy;
}
