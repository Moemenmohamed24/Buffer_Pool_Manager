
#pragma once

#include <list>
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

#include "Buffer_Pool_Manager/page_guard/page_guard.h" 
#include "Buffer_Pool_Manager/LRU-K_Replacer/lru_k_replacer.h"
#include "common/config.h"
#include "Buffer_Pool_Manager/Disk_Scheduler/disk_Scheduler.h"

#include "recovery/log_manager.h"
#include "storage_page\page.h"


namespace bustub 
{
  
  class BufferPoolManager;
  class ReadPageGuard;
  class WritePageGuard;
  class LRUKReplacer;
  
  class FrameHeader 
  { 
  
  friend class BufferPoolManager;
  friend class ReadPageGuard;
  friend class WritePageGuard;
  
  public:
  FrameHeader(short int frame_id);
  
  private:
  
  
  auto GetData() const -> const char *;
  auto GetDataMut() ->char *;
  
  
  void Reset();
  
  
  //number of frame in Buffer_bool or brief The frame ID / index of the frame this header represents.
  const int frame_id_;
  
  
  //make exclusive lock and one thread in write case , make shared lock thread in read case based on the genaral case read or wirte 
  std::shared_mutex rwlatch_;
  
  
  // Thread-safe counter for tracking pin references.
  // std::atomic ensures increment/decrement operations
  // are performed atomically to prevent race conditions
  // when accessed by multiple threads.
  std::atomic<short int>pin_count_;  
  
  bool is_dirty_;  
  
  std::vector<char>data_;  
  
  };
  
  
  
  class BufferPoolManager 
  {
  
  BufferPoolManager(int num_frames,DiskManager *disk_manager,LogManager *log_manager = nullptr)
  
  ~BufferPoolManager();
  
  
  //return number of frame in bufferbool
  int size();
  
  short NewPage();
  
  bool DeletePage(short page_id);
  
  //accountable to preparation data of page and get page in ram ,
  // so that return object guard that contains the data was have prepared it(writecheker)
  auto CheckedWritePage(short page_id /*,AccessType access_type  = AccessType::Unknown*/) -> std::optional<WritePageGuard>;
  
  
  auto CheckedReadPage(short page_id/*,AccessType access_type  = AccessType::Unknown*/) -> std::optional<ReadPageGuard>;
  
  auto WritePage(short page_id/*,AccessType access_type  = AccessType::Unknown*/) -> WritePageGuard;
  
  
  auto ReadPage(short page_id/*,AccessType access_type  = AccessType::Unknown*/) -> ReadPageGuard;
  
  
  auto FlushPageUnsafe(short page_id) -> bool;
  
  auto FlushPage(short page_id) -> bool;
  
  
  void FlushAllPagesUnsafe();
  
  void FlushAllPages();
  
  //returned the number of pin in specific page
  auto GetPinCount(int page_id) -> std::optional<int>; 
  
  
  private:
  
  const int num_frames_;
  
  std::atomic<short>next_page_id_; 
  
  //The main lock is on the buffer pool , it Protects -> page_table_  free_frames_  replacer_  frames_ 
  std::shared_ptr<std::shared_mutex>bpm_latch_;  
  //shared_ptr to make multi threads when read , and one thread when write to prevent  data racing
  
  
  //matadata of the page , each frame has Page data  pin count  dirty flag  page_id
  std::vector<FrameHeader> frames_data;
  
  
  //It is used to quickly find the frame_id using the page_id from the page_table, 
  //so that if it exists, it can be retrieved and used to search in frames_data for its information.
  std::unordered_map<short,int>page_table_;
  //short = frame -> key , int = id page -> value
  

  //A list of empty frames ready for use.  
  //If there is a free frame, use it directly without eviction.
  std::list<short>free_frames_;
  
  std::shared_ptr<LRUKReplacer>replacer_;
  
  std::shared_ptr<DiskScheduler> disk_scheduler_;
  
  //LogManager *log_manager_ __attribute__((__unused__));
  
  
  };
  
  
  
  
  
  
  
  
  
};