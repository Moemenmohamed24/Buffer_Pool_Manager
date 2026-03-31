

#include "Buffer_Pool_Manager/LRU-K_Replacer/lru_k_replacer.h"
#include<vector>

namespace bustub {

LRUKReplacer::LRUKReplacer(short int num_frames) 
    : replacer_size_(0) {}


auto LRUKReplacer::Evict() -> std::optional<int> {
    scoped_lock<mutex> lock(latch);
    vector<int> candidates_with_max_distance;
    long max_backward_k_distance = numeric_limits<long>::min();
    bool found_evictable_frame = false;
    
    for (const auto& [frame_id, metadata] : node_store_) {
        if (!metadata.is_evictable) {
            continue;
        }
        
        found_evictable_frame = true;
        
        const long backward_k_distance = current_timestamp_ - metadata.History.front();
        
        if (backward_k_distance > max_backward_k_distance) {
            // Found new maximum - start fresh
            max_backward_k_distance = backward_k_distance;
            candidates_with_max_distance.clear();
            candidates_with_max_distance.push_back(frame_id);
        } 
        else if (backward_k_distance == max_backward_k_distance) {
            // Found another frame with same maximum distance
            candidates_with_max_distance.push_back(frame_id);
        }
    }
    
    if (!found_evictable_frame) {
        return std::nullopt;
    }
    auto victim = candidates_with_max_distance.front();
    node_store_.erase(victim);
    replacer_size_--;    // Return first candidate (earliest added to vector)
    return victim;
}

void LRUKReplacer::RecordAccess(short int frame_id_t)
{
  scoped_lock<mutex> lock(latch);
  // stack object
  
  
  current_timestamp_++;
  
  auto node = node_store_.find(frame_id_t);  
  
  if(node !=  node_store_.end())
  {
    //return node to update      
    node->second.History.clear();
    node->second.History.push_back(current_timestamp_);
  }
  else
  {
    // stack object
    LRUNode node;        
    node.is_evictable = false;
    node.History.push_back(current_timestamp_);
    node_store_.insert({frame_id_t,node});
    
  }  
  
  
}

void LRUKReplacer::SetEvictable(short int frame_id_t, bool isevictable)
{
  //this is in any functions , not spesifc function 
  scoped_lock<mutex> lock(latch);
  auto node =  node_store_.find(frame_id_t);    
  if(node == node_store_.end())
  {
    return;
  }
    node->second.is_evictable = isevictable; 
  
}
void LRUKReplacer::Remove(short int frame_id_t)
{
  scoped_lock<mutex> lock(latch);
  auto node_page = node_store_.find(frame_id_t);
  
  if(node_page != node_store_.end())
  {
    if(!node_page->second.is_evictable)
    {
      replacer_size_--;
    }
    node_store_.erase(node_page);
    
  }
  
}
int LRUKReplacer::Size()
{
  return replacer_size_;
}
}
