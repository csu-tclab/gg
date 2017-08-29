/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstdio>
#include <cassert>
#include <thread>
#include <exception>
#include <sstream>
#include <memory>
#include <array>

#include "system_runner.hh"
#include "child_process.hh"
#include "exception.hh"
#include "file_descriptor.hh"

using namespace std;

int ezexec( const string & filename, const vector<string> & args,
            const vector<string> & env, const bool use_environ,
            const bool path_search )
{
  if ( args.empty() ) {
    throw runtime_error( "ezexec: empty args" );
  }

  if ( geteuid() == 0 or getegid() == 0 ) {
    if ( environ ) {
      throw runtime_error( "BUG: root's env not cleared" );
    }

    if ( path_search ) {
      throw runtime_error( "BUG: root should not search PATH" );
    }
  }

  /* copy the arguments to mutable structures */
  vector<char *> argv;
  vector<vector<char>> argv_data;

  for ( auto & x : args ) {
    vector<char> new_str;
    for ( auto & ch : x ) {
      new_str.push_back( ch );
    }
    new_str.push_back( 0 ); /* null-terminate */

    argv_data.push_back( new_str );
  }

  for ( auto & x : argv_data ) {
    argv.push_back( &x[ 0 ] );
  }

  argv.push_back( 0 ); /* null-terminate */

  /* copy the env variables to mutable structures */
  vector<char *> envp;
  vector<vector<char>> envp_data;

  if ( not use_environ ) {
    for ( auto & x : env ) {
      vector<char> new_str;
      for ( auto & ch : x ) {
        new_str.push_back( ch );
      }
      new_str.push_back( 0 ); /* null-terminate */

      envp_data.push_back( new_str );
    }

    for ( auto & x : envp_data ) {
      envp.push_back( &x[ 0 ] );
    }

    envp.push_back( 0 ); /* null-terminate */
  }

  return ( path_search ? execvpe : execve )( filename.c_str(), &argv[ 0 ],
                                             use_environ ? environ : &envp[ 0 ] );
}

void run( const string & filename, const vector<string> & args,
          const vector<string> & env, const bool use_environ,
          const bool path_search )
{
  ChildProcess command_process( args[ 0 ],
    [&]()
    {
      return ezexec( filename, args, env, use_environ, path_search );
    }
  );

  while ( !command_process.terminated() ) {
    command_process.wait();
  }

  if ( command_process.exit_status() != 0 ) {
    command_process.throw_exception();
  }
}

string check_output( const string & command )
{
  string output;

  unique_ptr<FILE, decltype(&pclose)> readpipe( popen( command.c_str(), "r" ), pclose );
  array<char, 4096> buffer;

  while ( !feof( readpipe.get() ) ) {
    size_t len = fread( buffer.data(), sizeof buffer[ 0 ], buffer.size(), readpipe.get() );
    output.append( buffer.data(), len );
  }

  return output;
}

string command_str( const vector<string> & command,
                    const vector<string> & environment )
{
  ostringstream oss;

  for ( const auto & e : environment ) { oss << e << " "; }
  for ( const auto & c : command ) { oss << c << " "; }

  return oss.str();
}
