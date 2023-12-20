#include "socket.hh"
#include <iostream>
#include <string>

using std::string;

void get_URL( const string& host, const string& path )
{
  TCPSocket socket;
  socket.connect( Address { host, "http" } );
  socket.write( "GET " + path + " HTTP/1.1\r\n" );
  socket.write( "Host: " + host + "\r\n" );
  socket.write( "Connection: close\r\n\r\n" );
  string data;
  string datapart;
  while ( !socket.eof() ) {
    socket.read( datapart );
    data += datapart;
  }
  std::cout << data;
}