//******************************************************************************
// Copyright (c) 2001-2002 Tweak Inc. All rights reserved.
//******************************************************************************

#include <RiGto/RiGtoReader.h>
#include <RiGto/RiGtoObject.h>
#include <RiGto/RiGtoSubd.h>
#include <RiGto/RiGtoNURBS.h>
#include <RiGto/RiGtoStrand.h>
#include <RiGto/RiGtoPoly.h>
#include <RiGto/RiGtoException.h>
#include <Gto/Protocols.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

namespace RiGto {

//******************************************************************************
Reader::Reader( Set &set, ReaderPhase rp )
  : Gto::Reader(),
    m_set( set ),
    m_readerPhase( rp )
{
    // Nothing
}

//******************************************************************************
Reader::~Reader()
{
    // Nothing
}

//******************************************************************************
Reader::Request Reader::object( const std::string &name,
                                const std::string &protocol,
                                const unsigned int protocolVersion,
                                const ObjectInfo &header )
{
    bool noSubds = false;
    const char *noSubdsEnv = getenv( "TWK_RI_GTO_NO_SUBDS" );
    if ( noSubdsEnv != NULL &&
         !( strcmp( noSubdsEnv, "0" ) == 0 ||
            strcmp( noSubdsEnv, "FALSE" ) == 0 ||
            strcmp( noSubdsEnv, "False" ) == 0 ||
            strcmp( noSubdsEnv, "false" ) == 0 ) )
    {
        noSubds = true;
    }
    
    if ( m_readerPhase == READER_REF )
    {
        if ( m_set.object( name ) != NULL )
        {
            TWK_FAKE_EXCEPTION( Exception,
                                "Duplicate object names in GTO ref file" );
        }
    }
    
    if ( protocol == GTO_PROTOCOL_TRANSFORM )
    {
        if ( m_readerPhase == READER_REF )
        {
            Object *newObj = new Object( name, protocolVersion, this );
            m_set.addObject( newObj );
            return Request( true, ( void * )newObj );
        }
        else
        {
            void *obj = ( void * )( m_set.object( name ) );
            if ( obj != NULL )
            {
                return Request( true, obj );
            }
            else
            {
                return Request( false );
            }
        }
    }
    else if ( protocol == GTO_PROTOCOL_CATMULL_CLARK )
    {
        if ( m_readerPhase == READER_REF )
        {
            if ( noSubds )
            {
                std::cout << "WARNING: Treating subd surface: "
                          << name << " as a polymesh."
                          << std::endl;
                
                Poly *newObj = new Poly( name, protocolVersion, this );
                m_set.addObject( newObj );
                return Request( true, ( void * )newObj );
            }
            else
            {
                Subd *newObj = new Subd( name, protocolVersion, this );
                m_set.addObject( newObj );
                return Request( true, ( void * )newObj );
            }
        }
        else
        {
            void *obj = ( void * )( m_set.object( name ) );
            if ( obj != NULL )
            {
                return Request( true, obj );
            }
            else
            {
                return Request( false );
            }
        }
    }
    else if ( protocol == GTO_PROTOCOL_NURBS )
    {
        if ( m_readerPhase == READER_REF )
        {
            NURBS *newObj = new NURBS( name, protocolVersion, this );
            m_set.addObject( newObj );
            return Request( true, ( void * )newObj );
        }
        else
        {
            void *obj = ( void * )( m_set.object( name ) );
            if ( obj != NULL )
            {
                return Request( true, obj );
            }
            else
            {
                return Request( false );
            }
        }
    }
    else if ( protocol == GTO_PROTOCOL_STRAND )
    {
        if ( m_readerPhase == READER_REF )
        {
            Strand *newObj = new Strand( name, protocolVersion, this );
            m_set.addObject( newObj );
            return Request( true, ( void * )newObj );
        }
        else
        {
            void *obj = ( void * )( m_set.object( name ) );
            if ( obj != NULL )
            {
                return Request( true, obj );
            }
            else
            {
                return Request( false );
            }
        }
    }
    else if ( protocol == GTO_PROTOCOL_POLYGON )
    {
        if ( m_readerPhase == READER_REF )
        {
            Poly *newObj = new Poly( name, protocolVersion, this );
            m_set.addObject( newObj );
            return Request( true, ( void * )newObj );
        }
        else
        {
            void *obj = ( void * )( m_set.object( name ) );
            if ( obj != NULL )
            {
                return Request( true, obj );
            }
            else
            {
                return Request( false );
            }
        }
    }
    else
    {
        return Request( false );
    }
}

//******************************************************************************
Reader::Request Reader::component( const std::string &name,
                                   const ComponentInfo &header )
{
    const Object *object = ( const Object * )( header.object->objectData );
    void *ret = object->component( name, m_readerPhase );
    if ( ret == NULL )
    {
        return Request( false );
    }
    else
    {
        return Request( true, ret );
    }
}

//******************************************************************************
Reader::Request Reader::property( const std::string &name,
                                  const PropertyInfo &header )
{
    const Object *object =
        ( const Object * )( header.component->object->objectData );
    void *componentData = ( void * )( header.component->componentData );
    void *ret = object->property( name, componentData, m_readerPhase );
    if ( ret == NULL )
    {
        return Request( false );
    }
    else
    {
        return Request( true, ret );
    }
}

//******************************************************************************
void *Reader::data( const PropertyInfo &pinfo,
                    size_t numBytes )
{
    Object *object =
        ( Object * )( pinfo.component->object->objectData );
    void *componentData = ( void * )( pinfo.component->componentData );
    void *propertyData = ( void * )( pinfo.propertyData );
    return object->data( componentData,
                         propertyData,
                         pinfo.size,
                         pinfo.width,
                         numBytes,
                         m_readerPhase );
}

//******************************************************************************
void Reader::dataRead( const PropertyInfo &pinfo )
{
    Object *object =
        ( Object * )( pinfo.component->object->objectData );
    void *componentData = ( void * )( pinfo.component->componentData );
    void *propertyData = ( void * )( pinfo.propertyData );
    object->dataRead( componentData, propertyData, m_readerPhase );
}

} // End namespace RiGto
