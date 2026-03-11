


#pragma once

#include <future>  // NOLINT
#include <optional>
#include <thread>  // NOLINT
#include <vector>
#include "commonn/channel.h"
#include "disk_manager/disk_manager.h"

namespace bustub {

struct DiskRequest 
{
  bool is_write_;
  
  
  char* data;
  
  
  short int page_id_;

  //use in one thread to returned the value when it fineshed the mission,
  //it uses to send the final value.
  std::promise<bool> callback_;
  
};


class DiskScheduler 
{
  public:
  
  DiskScheduler(DiskManager *disk_manager);
  
  ~DiskScheduler();
  
  
  void Schedule(DiskRequest& diskRequest);
  
  
  void StartWorkerThread();
  //DiskSchedulerPromise is alias name 
  using DiskSchedulerPromise  = std::promise<bool>;
  
                      //this is type of the return   
  auto CreatePromise() -> DiskSchedulerPromise { return { };};
  
  
  
  void DeallocatePage(short int page_id) { disk_manager->DeletePage(page_id);} 
  
  
  
  
  
  private:  
  DiskManager *disk_manager_ ;  

  //container of the Scheduler of reguests;
  Channel<std::optional<DiskRequest>> request_queue_;  
  
  // Background thread that handles disk operations asynchronously.
  // Instead of blocking the program to wait for disk I/O, this thread processes requests in the background.
  // Each request from the BufferPoolManager or other components is handled here.
  std::optional<std::thread> background_thread_;
  //optional meaning return std::thread if there is definitely  thread or return std::nullopt if there is no thread
  
  
};


}