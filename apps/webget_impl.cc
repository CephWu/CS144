#include "socket.hh"
#include <iostream>
#include <string>

using namespace std;

void get_URL( const string& host, const string& path )
{
  string msg1 = "GET " + path + " HTTP/1.1" + "\r\n";
  string msg2 = "Host: " + host + "\r\n";
  string msg3 = "Connection: close\r\n";
  string msg4 = "\r\n";
 
  TCPSocket tcpSocket;
  tcpSocket.connect(Address( host, "http" )); 
  tcpSocket.write(msg1 + msg2 + msg3 + msg4);
  tcpSocket.shutdown(SHUT_WR); 

  string buf;
  while (tcpSocket.eof() == false) 
  {
    tcpSocket.read( buf ); 
    cout << buf;
  }

  tcpSocket.close();
  return;
}