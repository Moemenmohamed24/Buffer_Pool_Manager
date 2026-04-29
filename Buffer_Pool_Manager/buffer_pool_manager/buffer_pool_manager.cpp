
#include "common\macros.h"
#include "Buffer_Pool_Manager/LRU-K_Replacer/lru_k_replacer.h" 
#include "Buffer_Pool_Manager/common/config.h"
#include "buffer_pool_manager.h"
#include "Buffer_Pool_Manager/Disk_Scheduler/disk_Scheduler.h"
#include "buffer_pool_manager.h"

namespace bustub
{
  
  //BUSTUB_PAGE_SIZE = 4096 bytes = 4KB
  FrameHeader::FrameHeader(short int frame_id) : frame_id_(frame_id) , data_(BUSTUB_PAGE_SIZE,0) {Reset();} 
  
  
  //.data() returns: The address of the first element in the vector That is, the pointer to the first byte of the page.
  //read only 
  auto FrameHeader::GetData() const ->const char*
  {
    return data_.data();
  }
  
  auto FrameHeader::GetDataMut() -> char *
  {
    return data_.data();
  }
  
  
  //Reset is performed: When creating the frame And after each eviction
  void FrameHeader::Reset()
  {
    std::fill(data_.begin(),data_.end(),0);    
    pin_count_.store(0);
    is_dirty_ = false;
    
  }
  
  BufferPoolManager::BufferPoolManager(int num_frames,DiskManager *disk_manager,LogManager *log_manager = nullptr) :
      
      num_frames_(num_frames),       
      
      next_page_id_(0),
      
      bpm_latch_(std::make_shared<std::shared_mutex>()),
      replacer_(std::make_shared<LRUKReplacer>(num_frames)),
      disk_scheduler_(std::make_shared<DiskScheduler>(disk_manager)),
      log_manager_(log_manager) {
      
      
      
      // Not strictly necessary in constractor
      std::scoped_lock latch(bpm_latch_);
      next_page_id_.store(0);
      
      page_table_.reserve(num_frames);
      
      frames_data.reserve(num_frames);
      
      page_table_.reserve(num_frames);
      
      for(size_t i = 0; i < num_frames;i++)
      {
        
        frames_data.push_back(std::make_shared<FrameHeader>(i));
        free_frames_.push_back(static_cast<int>(i));        
      }
      
      }
  
  BufferPoolManager::~BufferPoolManager() = default;  
  
  
  size_t BufferPoolManager::size()  {return num_frames_;}
  
  
  short BufferPoolManager::NewPage()
  {
    FrameHeader frameHeader;
    LRUKReplacer lRUKReplacer;
    //fetch_add is returns the final old  value and increases one than above it 
    short new_page_id =  next_page_id_.fetch_add(1);
    size_t free_frame;
    if(free_frames_.empty())
    {
      //get the frame_id_ that will evacuates
      auto optional = lRUKReplacer.Evict(); 
      
      //if Evict return null
      if(!optional)
      {
        return 0;
      }
      else
      {
        free_frame = frameHeader.frame_id_ = optional.value();
      } 
      
      //get old_page_id that that have accuntability of frame_id_ in buffer bool  
      int old_page_id = page_table_[free_frame];
      
      //useing frame_id_ in frames_data that contains all matadata in buffer bool  for each frame_id
      if(frames_data[free_frame]->is_dirty_)
      {
        DiskScheduler diskScheduler;
        //loss data

        //promise → will be sent to the worker thread
        //future → with you
        auto promise  = diskScheduler.CreatePromise();
        auto future = promise.get_future();
        DiskRequest Request ;
        Request.is_write_ = true;
        Request.page_id_ = old_page_id;
        Request.callback_ = std::move(promise);        
        disk_scheduler_->Schedule(Request);
        
        //I won't continue until the disk operation is finished because the disk write might not be complete yet.
        //`future.get() = synchronization point`
        future.get();
        //If the value is not yet ready → waits.
        //If the set_value is ready → returns the value immediately.
      } 
      
      BufferPoolManager::DeletePage(old_page_id);
      
      //reassigen  the new value of the new page that enter to buffer bool  
      frames_data[free_frame]->Reset();
      frames_data[free_frame]->is_dirty_ = false;
      frames_data[free_frame]->frame_id_ = free_frame;
      frames_data[free_frame]->pin_count_ = 1;
      
      
      //update page_table_;
      page_table_[free_frame] = new_page_id;
      //frame_id_
      replacer_->RecordAccess(free_frame);
      replacer_->SetEvictable(free_frame,false);
      
      
    }
    else
    { 
      size_t free_frame = free_frames_.front();
      free_frames_.pop_front();
      
      frames_data[free_frame]->Reset();
      frames_data[free_frame]->is_dirty_ = false;
      frames_data[free_frame]->frame_id_ = free_frame;
      frames_data[free_frame]->pin_count_ = 1;
      
      
      page_table_.insert({free_frame,new_page_id});
      
      replacer_->RecordAccess(free_frame);
      replacer_->SetEvictable(free_frame,false);
    }
    
    return new_page_id;
  }
  
  bool BufferPoolManager::DeletePage(short page_id)
  {
    
    if(page_table_.find(page_id) == page_table_.end())
    {
      DiskScheduler disk_Scheduler;
      disk_Scheduler.DeallocatePage(page_id);
      return true;   
      
    }
    
    short frame_id = page_table_[page_id];
    
    if(frames_data[frame_id]->pin_count_ > 0)
    {
      return false; 
    }
    
    LRUKReplacer LRU_K_Replacer;
    
    LRU_K_Replacer.Remove(frame_id);
    
    page_table_.erase(page_id);
    
    //make Reset about all informations or matedata that relate this frame_id 
    frames_data[frame_id]->Reset();
    
    free_frames_.push_back(frame_id);
    
    DiskScheduler disk_Scheduler;
    disk_Scheduler.DeallocatePage(page_id);
    
    return true;
  }
  
  auto BufferPoolManager::CheckedWritePage(short page_id /*, AccessType access_type*/) -> std::optional<WritePageGuard>  
  {
  
    FrameHeader frameHeader;
    
    //It takes exclusive lock. It blocks all other readers and writers.
    //It unlocks when you go outside the scope.
    std::unique_lock<std::shared_mutex>latch(*bpm_latch_);
    
    size_t free_frame;
    if(page_table_.find(page_id) != page_table_.end())
    {
      
      free_frame = frames_data[page_id]->frame_id_; 
      frames_data[page_id]->pin_count_++; 
      replacer_->SetEvictable(page_id,false);
      
    }
    else if(page_table_.find(page_id) == page_table_.end() && !free_frames_.empty())
    {      
      
      free_frame = free_frames_.front();
      free_frames_.pop_front();
      
      page_table_.insert({free_frame,page_id});
      
      frames_data[free_frame]->Reset();
      frames_data[free_frame]->pin_count_ = 1;
      frames_data[free_frame]->frame_id_ = free_frame;
      frames_data[free_frame]->is_dirty_ = false;
      
    }
    else
    {
      
      auto victim_opt  = replacer_->Evict();
      
      if(!victim_opt )
      {
        return nullopt;
      }
      free_frame = victim_opt.value();
      frames_data[free_frame]->Reset();
      frames_data[free_frame]->pin_count_ = 1;
      frames_data[free_frame]->frame_id_ = free_frame;
      frames_data[free_frame]->is_dirty_ = false;
      
    }
    
    return WritePageGuard(page_id,frames_data[free_frame],bpm_latch_,disk_scheduler_,replacer_);
  }
  
  auto BufferPoolManager::CheckedReadPage(short page_id) -> std::optional<ReadPageGuard>
  {
    
    FrameHeader frameHeader;      
    std::shared_lock<std::shared_mutex>latch(*bpm_latch_);
    
    size_t free_frame;
    if(page_table_.find(page_id) != page_table_.end())
    {
      
      free_frame = frames_data[page_id]->frame_id_; 
      frames_data[page_id]->pin_count_++; 
      replacer_->SetEvictable(page_id,false);
      
    }
    else if(page_table_.find(page_id) == page_table_.end() && !free_frames_.empty())
    {      
      
      free_frame = free_frames_.front();
      free_frames_.pop_front();
      
      page_table_.insert({free_frame,page_id});
      
      frames_data[free_frame]->Reset();
      frames_data[free_frame]->pin_count_ = 1;
      frames_data[free_frame]->frame_id_ = free_frame;
      frames_data[free_frame]->is_dirty_ = false;
      
    }
    else
    {
      
      auto victim_opt  = replacer_->Evict();
      
      if(!victim_opt )
      {
        return nullopt;
      }
      free_frame = victim_opt.value();
      frames_data[free_frame]->Reset();
      frames_data[free_frame]->pin_count_ = 1;
      frames_data[free_frame]->frame_id_ = free_frame;
      frames_data[free_frame]->is_dirty_ = false;
      
    }  
    
    return  ReadPageGuard(page_id,frames_data[free_frame],bpm_latch_,disk_scheduler_,replacer_);
  }

  auto BufferPoolManager::WritePage(short page_id) -> WritePageGuard
  {
    auto guard_opt = CheckedWritePage(page_id);
  
  if(!guard_opt.has_value())
  {
  //This message will appear as a problem or error for the programmer or user, not as part of the normal results.
    fmt::println(stderr, "\n`CheckedWritePage` failed to bring in page {}\n", page_id);    
    std::abort();
  }
  
  return std::move(guard_opt).value();
  // std::move transfers ownership of the WritePageGuard to the caller WritePage
// This is necessary because the lock inside the guard (unique_lock) is non-copyable
  }

  auto BufferPoolManager::ReadPage(short page_id) -> ReadPageGuard
  {
    auto guard_opt = CheckedReadPage(page_id);
  
  if(!guard_opt.has_value())
  {
  //This message will appear as a problem or error for the programmer or user, not as part of the normal results.
    fmt::println(stderr, "\n`CheckedReadPage` failed to bring in page {}\n", page_id);    
    std::abort();
  }
  
  return std::move(guard_opt).value();  
  // std::move transfers ownership of the WritePageGuard to the caller ReadPage
  // This is necessary because the lock inside the guard (unique_lock) is non-copyable
  }
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
};


