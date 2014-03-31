##Overview
The idea behind this project is to combine the force of [Apache Thrift](http://thrift.apache.org/) (an excellent framework for building language agnostic RPC frameworks) with different caching policies to build a proxy server that could serve its clients much faster, as opposed to a setup wherein clients have to interact with actual servers. The performance of this server has further been explored using different caching policies. 

*A point to note here is that the proxy server serves stored content - server’s design focuses on serving static content, which implies that the content being served might not be a “fresh” copy.*

##Cache Design Description
Following caching policies were implmented: **Random** Caching, **FIFO** Caching and **Largest Size First** Caching. These server implementations were compared against one another and were also compared with an implementation with no caching. Barring the no caching model, all the other caches have been implemented using an ‘unordered\_map’ data-structure from C++’s standard library. This structure was chosen over ‘map’ data-structure, because in case of ‘map’, the keys are stored in a sorted fashion (where, the manner of sorting can be incorporated in code). Sorting would have added extra overhead to time-complexity of these caches, hence  ‘unordered\_map’ - which has no binding construct for keeping keys sorted, and also has constant time-complexity for lookups, on an average. Besides an ‘unordered_map’,  other data-structures for maintaining metadata to aid data replacement policies. By default, these cache implementations are supposed to be 1024 KB in size (i.e. if a user doesn’t provide any input).
Whenever an HTML page request arrives from client, it is checked against the ‘unordered\_map’ (the requested-page serving as key in the lookup). If there is a match, the corresponding HTML body is returned to client -  this scenario is also referred to as a ‘Cache Hit’. In case the key is not found (situation commonly referred to as ‘Cache Miss’), server __CURL__-s the content from web and tries to populate its cache (with the fetched HTML body for the requested URL), before returning the fetched content to client. Depending on the space availabilities on cache, the cache is updated with newly fetched content - i.e. if there is enough space, else employs one of the following policies to make room.

**1. Random Caching**

Metadata, in this case, is maintained using a __vector__ (a standard C++ library data-structure). A vector is employed due to its dynamic nature (can expand and shrink as per requirements). In this vector, the keys (used in ‘unordered_map’) are maintained. Using a random number generator (i.e. rand()), an index of an element is found (from vector). This randomly generated element is the one chosen for eviction - in order to make room for newly fetched content. 
This eviction policy does not give importance to either cache affinity or fairness. By randomly evicting data from cache, more cache misses are highly probable. This mechanism is used to gauge-at the proxy server’s performance as opposed to ones offered by other (prominent) schemes.

**2. FIFO Caching**

Metadata, in this case, is maintained using a __deque__ (a standard C++ library data-structure). A deque follows FIFO - first-in-first-out, structure while populating/removing content. Deque (like most of the other containers) can dynamically shrink and grow. On a ‘Cache Miss’, the keys are pushed at the back of queue - the ordering is of significance in FIFO. Similarly, at the time of eviction, the data which arrived first is popped out of the deque - first entry in the deque is popped, which is then used for removing the actual element from cache. Considering elements are being removed and added to fixed spots in the deque, the time-complexity incurred for its maintenance is nominal.
The eviction policy, in this case, pays attention to fairness but not to cache affinity. This scheme would work well if the emphasis has to be upon fairness of operation, but considering our implementation intends to serve as a proxy server - where no such presumptions hold, this scheme might fare poorly.

**3. Largest-Size-First Caching**

Metadata, in this case, is maintained using a __priority queue__ (a standard C++ library data-structure). A priority queue fits this problem best, as it can easily maintain entries in a certain order (i.e. based on a defined criteria). The criteria for ordering, in our problem, is the size of data fetched from the internet. Internally, a priority queue is maintained on top of a heap - making the insertions (via push()) have logarithmic time-complexity and deletions (via pop()) have constant time-complexities, on an average. A priority queue, like its contemporaries, is a dynamic data structure. Entry with the largest data size is always kept on top, and this data-structure is used for finding the next element to evict from cache.
An assumption is made about the manner in which clients would request data from server - the clients are highly improbable to request for hefty data frequently. With this assumption, the caching policy evicts data in a non-increasing order of HTML body-content’s length. This scheme seems to be problematic as the very assumption which forms the basis of this policy could be violated - the clients might end up requesting for large contents more often than anticipated, leading to frequent cache misses, further leading to increased latency. Also, this scheme does not lay any emphasis on either fairness or cache affinity.
The eviction process (via all these policies) is performed till enough room is made to incorporate new data.

##Running Experiments

All the experiments were conducted on **Ubuntu 12.04 x86-64b** machines.

0. Make sure that you've installed Apache Thrift. **If not**:[1](http://thrift.apache.org/tutorial/),[2](http://thrift.apache.org/download)
1. Change current working directory to the extracted gen-cpp folder.
2. On each terminal : `export LD_LIBRARY_PATH=/usr/local/lib`

   *(The better option is to append this commnad to __.bashrc__)*

3. Run **make**
4. Performing step-4 will yield following binaries:

   **server_FIFOCaching**  -- proxy server with FIFO replacement policy
   
   **server_MAXSCaching** -- proxy server with ‘Largest Size First’ replacement policy
   
   **server_NoCaching** -- proxy server without a cache
   
   **server_RandomCaching** -- proxy server with random replacement policy
   
   **client** -- client
   
5. For generating workloads -- `python generateWorkloads.py`

   *(This generates two workloads to tinker with - workloadA and workloadB)*
   
6. For conducting experiments:

   In order to execute any of the server binaries mentioned above: `./server_* <cache_size>`

   a. *The cache size will be processed in KBs - for e.g. giving a value of 10 will make the server have a cache size of 10 KB.*
   
   b. *If no \<cache\_size\> is provided, the code will default it to 1024 KB.*
   
   c. *If the payloads were generated using Step-6, the minimum cache size needed for running experiments = 256 KB.)*
  
7. In order to execute client binary: `./client <IP address> <workload_file_name>`

   *(If testing locally, you can give the \<IP address\> as __localhost__)*

8. For **finding average times** taken by server to serve HTML content (to be done on client): 

   a. `./client <IP address> <workload_file_name> | tee <file_name>`

   b. `python getAvgTimes.py <file_name>`
   
9. For **finding cache hit/miss ratio** (to be done on server):

   a. `./server_* <cache_size> | tee <file_name>`

   b. Hit ratio is computed by dividing output of `cat <file_name> | grep “Hit” | wc -l` with total number of requests in workload.
   
   *(In current setup, there were 100 URLs/workload)*
