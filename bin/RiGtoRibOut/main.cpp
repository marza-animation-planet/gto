//******************************************************************************
// Copyright (c) 2001-2003 Tweak Inc. All rights reserved.
//******************************************************************************

#include <RiGto/RiGtoPlugin.h>
#include <RiGto/RiGtoException.h>
#include <RiGtoStub/Stubs.h>
#include <iostream>

//******************************************************************************
using namespace RiGto;
using namespace RiGtoStub;

//******************************************************************************
void *ConvertParameters( const char *initialData )
{
    static DataBase *globalDataBase = NULL;

    void *ret = NULL;
    if ( globalDataBase == NULL )
    {
        globalDataBase = new DataBase();
    }

    ret = ( void * )( new Plugin( *globalDataBase,
                                  initialData, false ) );

    if( ret == NULL )
    {
        TWK_FAKE_EXCEPTION( Exception, "Unable to create plugin instance" );
    }
    return ret;
}


//******************************************************************************
void Subdivide( void *blindData, float detailSize )
{
    if( blindData == NULL )
    {
        TWK_FAKE_EXCEPTION( Exception, "Entropy gave NULL pointer"
                            " to Subdivide()!" );
    }    
    
    (( Plugin * )blindData)->declareRi();
}

//******************************************************************************
void Free( void *blindData )
{
    if( blindData == NULL )
    {
        TWK_FAKE_EXCEPTION( Exception, "Entropy gave a NULL pointer"
                            " to Free()!" );
    }    
    Plugin *plugin = ( Plugin * )blindData;
    delete plugin;
}

//******************************************************************************
int main( int argc, char *argv[] )
{
    if ( argc < 2 )
    {
        std::cout << "USAGE: " << argv[0] << " cfg cfg cfg ... "
                  << std::endl;
        return -1;
    }

    std::string cfg( "" );
    for ( size_t i = 1; i < argc; ++i )
    {
        cfg += std::string( argv[i] );
        cfg += std::string( " " );
    }
    // std::cout << "Using cfg string: " << cfg << std::endl;

    // Open out file
    // std::cout << "RiBegin" << std::endl;
    Stub_RiBegin( NULL );

    // Do the thing
    // std::cout << "Convert Parameters" << std::endl;
    void *data = ConvertParameters( cfg.c_str() );

    // std::cout << "Subdivide" << std::endl;
    Subdivide( data, 1.0f );

    // std::cout << "Free" << std::endl;
    Free( data );

    // Close the file.
    Stub_RiEnd();

    return 0;
}


