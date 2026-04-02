

#include "page_guard.h" 
#include "Buffer_Pool_Manager/LRU-K_Replacer/lru_k_replacer.h"

#include<memory>
#include "common\macros.h" 
#include "page_guard.h"

namespace bustub {
  
  
  ReadPageGuard::ReadPageGuard(short int page_id,std::shared_ptr<FrameHeader> frame,std::shared_ptr<std::shared_mutex> bpm_latch,
    std::shared_ptr<DiskScheduler>disk_scheduler,std::shared_ptr<LRUKReplacer>replacer) 
      page_id_(page_id), 
      frame_(std::move(frame)),
      bpm_latch_(std::move(bpm_latch)),
      disk_scheduler_(std::move(disk_scheduler)),
      replacer_(std::move(replacer)) {}
  
  //std::move :- You're telling the compiler: I don't need this variable anymore that in the right
  
  
  
  
  //Move Constructor is called when we move data from one object to another that is currently being created.
  ReadPageGuard::ReadPageGuard(const ReadPageGuard&&) noexcept {};
  
  
  //Move Assignment Operator is called when data is move from one object to another existing object.
  auto ReadPageGuard::operator = (const ReadPageGuard&&) noexcept -> ReadPageGuard & {return *this};
  // {return *this} = return reference to make Chaining to assignment 
  
  
  
  
  short int ReadPageGuard::GetPageId() const
  {
    BUSTUB_ENSURE(is_valid_,"tried to use an invalid read guard");
    return page_id_;
    
  }
  
  
  auto ReadPageGuard::GetData() const -> char *
  {
    BUSTUB_ENSURE(is_valid_,"tried to use an invalid read guard");
    return frame_->GetData(); 
  }
  
  
  auto ReadPageGuard::IsDirty() -> bool
  {
    BUSTUB_ENSURE(is_valid_,"tried to use an invalid read guard");
    return frame_->is_dirty_;
  }
  
  
  void ReadPageGuard::Flush()
  {
    
    BUSTUB_ENSURE(is_valid_,"tried to use an invalid read guard");
    
    // The promise/future signals task completion in the worker thread; Flush() just sends the request.
    std::promise<bool> pirmisson;
    DiskRequest reguest;
    
    reguest.is_write_ = true;
    
    reguest.data = frame_->GetData();
    
    reguest.page_id_ = page_id;
    reguest.callback_ = std::mov(pirmisson);
    
    DiskScheduler::Schedule(reguest);
    
  }
  
  
  void ReadPageGuard::Drop()
  {
    
    
  }
  
  
  ReadPageGuard::~ReadPageGuard()
  {
    Drop();
  }
  
  //<-------------------------------------------------------------------------------------------------------->
  
  
  WritePageGuard::WritePageGuard(short int page_id, std::shared_ptr<FrameHeader> frame,
                                std::shared_ptr<LRUKReplacer> replacer, std::shared_ptr<std::shared_mutex> bpm_latch,
                                std::shared_ptr<DiskScheduler> disk_scheduler)
  
  
  WritePageGuard::WritePageGuard(const WritePageGuard&&) noexcept {};
  
  auto WritePageGuard::operator=(const WritePageGuard &&that) noexcept -> WritePageGuard & { return *this }
  
  
  
  auto WritePageGuard::GetPageId() const {
    
    BUSTUB_ENSURE(is_valid_,"tried to use an invalid read guard");    
    return page_id;    
  }
  
  
  auto WritePageGuard::GetData() const -> char *
  {
    BUSTUB_ENSURE(is_valid_,"tried to use an invalid read guard");    
        return frame_->GetData(); 
  }
  
  
  auto WritePageGuard::GetDataMut() -> char *
  {
    BUSTUB_ENSURE(is_valid_,"tried to use an invalid read guard"); 
    return frame_->GetDataMut();
    
  }
  

  auto WritePageGuard::IsDirty() -> bool
  {
    BUSTUB_ENSURE(is_valid_,"tried to use an invalid read guard");
    return frame_->is_dirty_;
  }
  
  
  void WritePageGuard::Flush() {
    
    BUSTUB_ENSURE(is_valid_,"tried to use an invalid read guard");
    
    std::promise<bool>pirmisson;
    DiskRequest reguest;
    
    reguest.is_write_ = true;    
    reguest.page_id_ = page_id;    
    reguest.data = frame_->GetData();
    reguest.callback_ = std::mov(pirmisson);
    
    DiskScheduler::Schedule(reguest);
    
  }
  void WritePageGuard::Drop() {
  
  
  
  }
  WritePageGuard::~WritePageGuard() {
    Drop();
  };
}
