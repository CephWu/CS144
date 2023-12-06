#include <string>
using std::string;

void get_URL( const string& host, const string& path );

int main()
{
  get_URL( "cs144.keithw.org", "/nph-hasher/xyzzy" );
}
