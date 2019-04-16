#include "cMainWindow.h"
#include <QtWidgets/QApplication>

#include <iostream>
#include <CL/cl.hpp>

#include "ThreadMachine.h"




// Checks
#include "BenchmarkStuff.h"
#include <thread>
#include "Thread.h"




static
void
OpenCLTests()
{
}

class MTTest
{
public:
    MTTest()
    {
        Marray = new int[ mSize ];
        memset( Marray, 0, sizeof( int ) * mSize );
    }


    void
    MonoThread()
    {
        for( int i = 0; i < mSize; ++i )
        {
            Marray[i] = 5;
        }
    }


    void
    Kernel1()
    {
        for( int i = 0; i < mSubSize; ++i )
        {
            Marray[i] = 1;
        }
    }


    void
    Kernel2()
    {
        for( int i = mSubSize*1; i < mSubSize*2; ++i )
        {
            Marray[i] = 1;
        }
    }


    void
    Kernel3()
    {
        for( int i = mSubSize*2; i < mSubSize*3; ++i )
        {
            Marray[i] = 1;
        }
    }


    void
    Kernel4()
    {
        for( int i = mSubSize*3; i < mSubSize*4; ++i )
        {
            Marray[i] = 1;
        }
    }


    void
    MultiThread1()
    {
        std::vector< std::thread > ar;

        ar.push_back( std::thread( &MTTest::Kernel1, this ) );
        ar.push_back( std::thread( &MTTest::Kernel2, this ) );
        ar.push_back( std::thread( &MTTest::Kernel3, this ) );
        ar.push_back( std::thread( &MTTest::Kernel4, this ) );

        for( auto& t : ar )
            t.join();
    }


    void
    MultiThreadProc()
    {
        auto h1 = cThreadProcessor::Instance()->AffectFunctionToThreadAndStart( [ this ]( cRange r1, cRange r2 ){ Kernel1(); }, false );
        auto h2 = cThreadProcessor::Instance()->AffectFunctionToThreadAndStart( [ this ]( cRange r1, cRange r2 ){ Kernel2(); }, false );
        auto h3 = cThreadProcessor::Instance()->AffectFunctionToThreadAndStart( [ this ]( cRange r1, cRange r2 ){ Kernel3(); }, false );
        auto h4 = cThreadProcessor::Instance()->AffectFunctionToThreadAndStart( [ this ]( cRange r1, cRange r2 ){ Kernel4(); }, false );

        if( h1.GetThread() )
            h1.GetThread()->WaitEndOfTask();
        if( h2.GetThread() )
            h2.GetThread()->WaitEndOfTask();
        if( h3.GetThread() )
            h3.GetThread()->WaitEndOfTask();
        if( h4.GetThread() )
            h4.GetThread()->WaitEndOfTask();
    }


    bool
    SanityCheck( int value )
    {
        for( int i = 0; i < mSize; ++i )
        {
            if( Marray[i] != value )
                return  false;
        }

        return  true;
    }

    int* Marray;
    int mSize = 1000000;
    int mSubSize = 250000;
};




int main(int argc, char *argv[])
{
    //QApplication::setAttribute( Qt::AA_EnableHighDpiScaling ); // DPI support
    //QCoreApplication::setAttribute( Qt::AA_UseHighDpiPixmaps ); //HiDPI pixmaps
    //qputenv( "QT_SCALE_FACTOR", "2" );

    //OpenCLTests();


    //MTTest test;

    //qDebug() << "Mono -------------------";
    //BENCHSTART( 10 )
    //test.MonoThread();
    //BENCHEND( 10 )

    //if( !test.SanityCheck( 5 ) )
    //    qDebug() << "ERROR";


    //qDebug() << "Multi -------------------";
    //BENCHSTART( 10 )
    //test.MultiThread1();
    //BENCHEND( 10 )

    //if( !test.SanityCheck( 1 ) )
    //    qDebug() << "ERROR";

    //cThreadProcessor::Instance();

    //qDebug() << "Multi with processor -------------------";
    //BENCHSTART( 10 )
    //test.MultiThreadProc();
    //BENCHEND( 10 )

    //if( !test.SanityCheck( 1 ) )
    //    qDebug() << "ERROR";


    QApplication a(argc, argv);
    cMainWindow w;
    w.show();
    return a.exec();
}