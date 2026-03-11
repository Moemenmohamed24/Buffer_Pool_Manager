#include <memory>
#include <mutex>
#include "Buffer_Pool_Manager/Disk_Scheduler/disk_Scheduler.h"
#include "Buffer_Pool_Manager/LRU-K_Replacer/lru_k_replacer.h"



#include "buffer_pool_manager\buffer_pool_manager.h"
#include "Disk_Scheduler\disk_Scheduler.h"
#include "page_guard\page_guard.h"

class FrameHeader;
namespace bustub {
  
  class ReadPageGuard 
  {  
  
  public:
  
  ReadPageGuard() = default;
  
  //delete meaning , this is constractor don't make any copy from existing object to another creat object 
  //this is achived Only one owner for the resource 
  //delete privent  copy constructor like e1 = e1
  ReadPageGuard(const ReadPageGuard&) = delete; 
  
  //delete meaning ,don't copy ownership of a resource of object to an existing object
  auto operator = (const ReadPageGuard&) ->ReadPageGuard & = delete;
  
  
  
  //move wonership from that to a new object is created
  //don't make copy 
  ReadPageGuard(const ReadPageGuard&& that) noexcept;
  
  //move wonership from that to this (object already exists).
  auto operator = (const ReadPageGuard&& that) noexcept -> ReadPageGuard &;
  //noexcept -> ReadPageGuard & = return reference this
  
  
  
  
  auto GetPageId() const;
  //const this meaning reading only
  auto GetData() const -> char* ;
  
  //template to match any type
  template<class T> 
  
  //As return pointer type of const T*
  auto As() const -> const T* {  
  return reinterpret_cast<const * T>(GetData());
  }  
  // reinterpret_cast Converts a pointer (const char*) to a pointer of type (const T*) , Without modifying the data 
  //const 1) to previnting change the element in As
  //const T* 2) to previnting change the value in the pointer readonly in ReadPageGuard because const T*
  
  auto IsDirty() -> bool;
  
  void Flush();
  void Drop();
  
  ~ReadPageGuard();
  
  private:
  //private Because: Creating a Guard requires using BufferPoolManagere
  ReadPageGuard(short int page_id,std::shared_ptr<FrameHeader> frame_,std::shared_ptr<std::mutex> bpm_latch,
    std::shared_ptr<DiskScheduler>disk_scheduler,std::shared_ptr<LRUKReplacer>replacer_);
  
  //use in notation in Flush and Drop 
  short int page_id_;
  
  
  
  // shared_ptr  Participation here is not just ownership,
  //The frame_ is shared so that the page remains alive as long as there is a guard.
  std::shared_ptr<FrameHeader> frame_;
  
  
  
  std::shared_ptr<std::mutex> bpm_latch_;
  
  
  
  std::shared_ptr<DiskScheduler>disk_scheduler_; 
  
  
  std::shared_ptr<LRUKReplacer>replacer_
  
  bool is_valid_{false};
};
  //<--------------------------------------------------------------------------------------------------------->
  class WritePageGuard 
  {
  
  public:
  
  
  WritePageGuard() = default;
  
  //delete meaning , this is constractor don't make any copy from existing object to another creat object 
  //this is achived Only one owner for the resource 
  //delete privent  copy constructor like e1 = e1
  WritePageGuard(const WritePageGuard &) = delete;  
  
  //delete meaning ,don't copy ownership of a resource of object to an existing object
  auto operator = (const WritePageGuard &) -> WritePageGuard & = delete;
  
  
  
  //move wonership from that to a new object is created
  //don't make copy 
  WritePageGuard(const WritePageGuard&&that) = noexcept;  
  
  
  
  //move wonership from that to this (object already exists).  
  auto operator = (const WritePageGuard&&that) noexcept -> WritePageGuard &;  
  //noexcept -> WritePageGuard & = return reference this
  
  auto GetPageId() const;
  //const this meaning reading only
  auto GetData() const -> char* ;
  
  
  
  template<class T> 
  
  
  auto As() const -> const T* 
  {
    return reinterpret_cast<const T*>(GetData());
  }
  
  auto GetDataMut() -> char *;
  
  template<class T>
  auto AsMut() const -> const -> T*
  {
    return reinterpret_cast<T *>(GetDataMut());
  }
  
  auto IsDirty() -> bool;
  
  void Flush();
  void Drop();
  
  ~WritePageGuard();
  
  private:
  
  WritePageGuard(short int page_id,std::shared_ptr<FrameHeader> frame_,std::shared_ptr<std::mutex> bpm_latch,
  std::shared_ptr<DiskScheduler>disk_scheduler,std::shared_ptr<LRUKReplacer>replacer_);
  
  
  //use in notation in Flush and Drop 
  short int page_id_;
  
  
  
  // shared_ptr  Participation here is not just ownership,
  //The frame_ is shared so that the page remains alive as long as there is a guard.
  std::shared_ptr<FrameHeader> frame_;
  
  
  
  std::shared_ptr<std::mutex> bpm_latch_;
  
  
  
  std::shared_ptr<DiskScheduler>disk_scheduler_; 
  
  
  std::shared_ptr<LRUKReplacer>replacer_
  
  //this is flag of the validation of guard  
  bool is_valid_{false};
  };
}



