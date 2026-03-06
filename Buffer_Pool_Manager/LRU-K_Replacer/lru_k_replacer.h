

#include <list>
#include <unordered_map>
#include <mutex> 
#include <optional>
//#include "common/macros.h"
using namespace std;

namespace bustub {

class LRUNode
{
  public:
  
  list<int>History;
  short int _K;
  bool is_evictable = false;
  short int frame_id_t; 
  
};


class LRUKReplacer
{
  //num_frame size of the main buffer
  LRUKReplacer();
  LRUKReplacer(short int num_frame );
  /*, short int k*/
  
  //DISALLOW_COPY_AND_MOVE(LRUKReplacer);
  
  ~LRUKReplacer();
  
  public:  
  //std::optional<int>; return normal value if it is there , if there is not val return null
  auto Evict() -> std::optional<int>;
  
  void RecordAccess(short int frame_id_t);
  
  void SetEvictable(short int frame_id_t,bool isevictable);
  
  
  void Remove(short int frame_id_t);
  
  int Size();
  
  
  private:
  //store matadata not actually
  unordered_map<int,LRUNode> node_store_;
  long long current_timestamp_{0};
  //replacer_size_ of the is_evictable frame;
  short int replacer_size_;
  //short int K_;
  mutex latch;
  
  
  
  
  





}; 

}