


#include "disk_Scheduler.h"
#include "Buffer_Pool_Manager/disk_manager/disk_manager.h"


namespace bustub {
  
  //the starter of the thread and thread turn on  StartWorkerThread , it handeling the regeusts 
  DiskScheduler::DiskScheduler(DiskManager *disk_manager) : disk_manager_ (disk_manager)
  {
    
    
    background_thread_.emplace([this] {StartWorkerThread();});
    
    //or background_thread_ = std::thread() but emplace faster and do not make copy
    //[this] beacuse thread want object that  StartWorkerThread work in it 
    //thread use [this] to call StartWorkerThread
  }
  
  DiskScheduler::~DiskScheduler()
  {
    //informing worker thread that running  StartWorkerThread to stop itseif    
    request_queue_.Put(std::nullopt);
    if(background_thread_.has_value())
    background_thread_->join();
  }//join waits thread to ended befor complet destructor 
  
  void DiskScheduler::Schedule(DiskRequest &diskRequest)
  {
    request_queue_.Put(std::move(diskRequest));
  }  
  // mov:-> make Treat this object as something we can move from, not copy
  // and we usd mov becuase promise non-copyable , mov allow transfer all data from object to another
  
  
  
  
  
  
  void DiskScheduler::StartWorkerThread()
  {
    while(true)
    {
      //return the optinial value set of valus or std::nullopt
      auto reguest_object = request_queue_.Get();
      
      //check if returned value or std::nullopt make break of the while
      if(!reguest_object.has_value())
      break;
      
      //This gives me a copy of the value inside the optional, if it exists.
      DiskRequest disk_Request = std::move(reguest_object.value());
      
      if(disk_Request.is_write_)
      {
        disk_manager_->WritePage(disk_Request.page_id_,disk_Request.data);
      }
      else 
      {
        disk_manager_->ReadPage(disk_Request.page_id_,disk_Request.data);
      }
      
      //this is indication to the process is ended
      disk_Request.callback_.set_value(true);
      
      
    }
    
    
  }
  



}
