#include "simpleTests.h"
#include <iostream>
#include <string>
#include <fstream>
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
#include <sys/time.h>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace  ::testFunctions;


typedef unsigned long long timestamp_t;

timestamp_t get_timestamp ()
{
  struct timeval now;
  gettimeofday (&now, NULL);
  return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}

int main(int argc, char **argv) 
{
  const char* p;
  const char* f;
  if (argc==3)
  {
    p=argv[1];
    f=argv[2];
  }
  else
  {
    cout<<"You need to provide a vaild IP address and a file name (workload)!\nIf testing locally, you can provide the address -- localhost"<<endl;
    return 1;
  }

  boost::shared_ptr<TSocket> socket(new TSocket(p, 9090));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

  simpleTestsClient client(protocol);
  
  string line;
  ifstream inputfile (f);
  if (inputfile.is_open())
  {
    while (getline(inputfile,line))
    {
      transport->open();
      
      string a;
      timestamp_t start = get_timestamp();
      client.sendToServer(line);
      client.sendToClient(a);
      timestamp_t end = get_timestamp();
      cout<<"Bytes received from proxy server (for "<<line<<") -- "<<a.length()<<"; Time Elapsed = "<<(end-start)/1000000.0L<<endl;

      transport->close();
    }
    inputfile.close();
    return 0;
  }
  else
  {
    cout << "Unable to open file"<<endl; 
    return 1;
  } 
}

