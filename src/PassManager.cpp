//
//  Copyright (c) 2015-2017 Philipp Paulweber
//  All rights reserved.
//
//  Developed by: Philipp Paulweber
//                https://github.com/ppaulweber/libpass
//
//  This file is part of libpass.
//
//  libpass is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  libpass is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with libpass. If not, see <http://www.gnu.org/licenses/>.
//

#include "PassManager.h"

#include "PassLogger.h"

using namespace libpass;

char PassManager::id = 0;

static PassRegistration< PassManager > PASS( "PassManager",
    "displays the pass manager calculated pass dependency graph", "pm-dump",
    0 );

PassManager::PassManager( void )
: m_default_pass( nullptr )
, m_default_result()
{
    add( &id );
}

void PassManager::add( Pass::Id id )
{
    auto result = m_managed.emplace( id );
    if( not result.second )
    {
        const auto pass = PassRegistry::passInfo( id );
        throw std::domain_error( "pass '" + pass.name() + "' already managed" );
    }
}

void PassManager::setDefaultPass( Pass::Id defaultPass )
{
    m_default_pass = defaultPass;
}

void PassManager::setDefaultResult( const PassResult& pr )
{
    m_default_result = pr;
}

u1 PassManager::run( const std::function< void( void ) >& flush )
{
    libstdhl::Log::Chronograph swatch( true );
    PassLogger log( &id, stream() );

    // find all selected passes and calculate 'usage graph'
    for( auto id : m_managed )
    {
        const auto pass = PassRegistry::passInfo( id );

        if( pass.isArgSelected() )
        {
            log.debug( pass.name() + ": selected" );

            auto result = m_selected.emplace( id );
            if( not result.second )
            {
                throw std::domain_error( "already processed '"
                                         + std::string( pass.argString() )
                                         + "'" );
            }
        }

        auto result = m_usages.emplace( id, PassUsage() );
        assert( result.second );
        auto& pu = result.first->second;

        { // only create pass to fetch the internal usage info and release it
            pass.constructInternalPass()->usage( pu );
        }

        for( auto v : pu.provides() )
        {
            m_provides[ v ].emplace( id );
        }
    }

    log.debug( "selection: done (took: " + std::string( swatch ) + ")" );

    if( m_selected.size() == 0 )
    {
        if( m_default_pass )
        {
            m_selected.emplace( m_default_pass );
        }
        else
        {
            log.error( "no pass was selected" );
            return false;
        }
    }

    std::vector< Pass::Id > schedule;

    for( auto id : m_selected )
    {
        std::vector< Pass::Id > stack = { id };

        while( stack.size() != 0 )
        {
            schedule.push_back( stack.back() );

            stack.pop_back();

            const auto sched_id = schedule.back();
            const auto& pu = m_usages[ sched_id ];

            // log.debug( "schedule: '" + PassRegistry::passInfo( sched_id
            // ).name()
            //            + "' (depends on: "
            //            + std::to_string( pu.requires().size() )
            //            + ") (provided ref: "
            //            + std::to_string( m_provides[ sched_id ].size() )
            //            + ")" );

            for( auto u : pu.requires() )
            {
                stack.push_back( u );
                // log.debug( "    <-- " + PassRegistry::passInfo( u ).name() );
            }

            for( auto u : m_provides[ sched_id ] )
            {
                stack.push_back( u );
                // log.debug( "    --> " + PassRegistry::passInfo( u ).name() );
            }
        }
    }

    std::reverse( std::begin( schedule ), std::end( schedule ) );

    log.debug( "scheduling: done (took: " + std::string( swatch ) + ")" );

    m_result = m_default_result;
    u1 first = true;

    for( auto id : schedule )
    {
        const auto pass = PassRegistry::passInfo( id );

        if( m_result.result( id ) and first )
        {
            first = false;
            log.debug(
                "'" + pass.name() + "': skipping, result already present!" );
            continue;
        }

        first = false;

        auto p = pass.constructPass();
        p->setStream( stream() );

        p->initialize();

        log.debug( "'" + pass.name() + "': running" );

        if( flush )
        {
            flush();
        }

        const u1 statusRun = p->run( m_result );

        log.debug( "'" + pass.name() + "': done (took: " + std::string( swatch )
                   + ")" );

        u1 statusVerify = false;

        if( statusRun )
        {
            statusVerify = p->verify();
        }

        p->finalize();

        if( not statusRun or not statusVerify )
        {
            if( flush )
            {
                flush();
            }

            return false;
        }
    }

    log.debug( "running passes: done (took: " + std::string( swatch ) + ")" );

    if( flush )
    {
        flush();
    }

    return true;
}

const PassResult& PassManager::result( void ) const
{
    return m_result;
}

u1 PassManager::run( PassResult& pr )
{
    PassLogger log( &id, stream() );

    log.debug( "" );

    std::ostream& stream = std::cout;

    for( auto v : m_usages )
    {
        const auto id = v.first;
        const auto pu = v.second;
        const auto pass = PassRegistry::passInfo( id );

        stream << pass.name() << "\n";

        for( auto u : pu.requires() )
        {
            stream << "    <-- " << PassRegistry::passInfo( u ).name() << "\n";
        }

        for( auto u : m_provides[ id ] )
        {
            stream << "    --> " << PassRegistry::passInfo( u ).name() << "\n";
        }

        stream << "\n";
    }

    return true;
}

//
//  Local variables:
//  mode: c++
//  indent-tabs-mode: nil
//  c-basic-offset: 4
//  tab-width: 4
//  End:
//  vim:noexpandtab:sw=4:ts=4:
//
