// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "simpleTests.h"
#include <iostream>
#include <tr1/unordered_map>
#include <iterator>
#include <stdlib.h>
#include <vector>
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include <curl/curl.h>

using namespace std;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace std::tr1;
using boost::shared_ptr;
using namespace  ::testFunctions;

string data;
size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up)
  { 
      for (int c = 0; c<size*nmemb; c++)
      {
          data.push_back(buf[c]);
      }
      return size*nmemb;
  }

string getBody(string a)
{
  CURL* curl=curl_easy_init();
  CURLcode res;
  data.clear();
  curl_easy_setopt(curl, CURLOPT_URL, a.c_str());
  curl_easy_setopt(curl, CURLOPT_HEADER, 0);
  // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeCallback);
  res=curl_easy_perform(curl);
  if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
  curl_easy_cleanup(curl);
  return data;
}

class simpleTestsHandler : virtual public simpleTestsIf 
{
private:
  string valueToReturn;
  int currentCacheSize_Random;
  int maximumCacheSize_Random;
  unordered_map<string,string> Random_cache;
  vector<string> Random_cache_meta;

public:
  simpleTestsHandler(int a) 
  {
    if (a<=0) a=1024;
    currentCacheSize_Random=0;
    maximumCacheSize_Random=a*1024;
  }

  void ping() 
  {
    printf("ping\n");
  }

  bool sendToServer(const std::string& a) 
  {
    cout<<endl<<"Request for -- "<<a<<"\t";
    unordered_map<string,string>::iterator it=Random_cache.find(a);    
    valueToReturn.clear();

    if (it==Random_cache.end()) /*The entry does not exist*/
    {
        valueToReturn=getBody(a);
        if (valueToReturn.length()>maximumCacheSize_Random)
        {
          cout<<"The requested content is much larger than cache size!"<<endl;
          valueToReturn="";
          return false;
        }

        cout<<"Cache Miss!"<<endl;
        int requiredSize=valueToReturn.length()+currentCacheSize_Random;
        
        // cout<<endl<<"Current Cache size = "<<currentCacheSize_Random<<endl;
        
        if(requiredSize>=maximumCacheSize_Random)
          cout<<endl<<"Cache requires flushing now!"<<endl;

        while(!Random_cache_meta.empty() && requiredSize>=maximumCacheSize_Random)
        {
            int index=rand()%Random_cache_meta.size();
            string temp=Random_cache_meta.at(index);
            int len=Random_cache[temp].length();
            cout<<"Removing "<<temp<<endl;
            Random_cache.erase(temp);
            Random_cache_meta.erase(Random_cache_meta.begin()+index);
            currentCacheSize_Random-=len;
            requiredSize-=len;
        }

        Random_cache.insert(pair<string,string>(a,valueToReturn));
        currentCacheSize_Random+=valueToReturn.length();
        Random_cache_meta.push_back(a);
    }
    else
    {
      cout<<"Cache Hit!"<<endl;
      valueToReturn=Random_cache[it->first];
    }
    return true;
  }

  void sendToClient(std::string& _return) {
    _return=valueToReturn;
    valueToReturn.clear();
  }

};

int main(int argc, char **argv) 
{
  int port = 9090;
  int cacheSize;
  if (argc==2)
      cacheSize=atoi(argv[1]);
  else
      cacheSize=0;
  shared_ptr<simpleTestsHandler> handler(new simpleTestsHandler(cacheSize));
  shared_ptr<TProcessor> processor(new simpleTestsProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

