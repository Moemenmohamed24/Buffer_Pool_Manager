# Buffer Pool Manager - BusTub DBMS
## Overview ##

This project is an implementation of a Buffer Pool Manager, which is a core component in any Database Management System (DBMS).

The buffer pool acts as an in-memory cache that manages pages between main memory (RAM) and disk storage, allowing the database to efficiently handle large datasets that do not fit entirely in memory.

In this project, we simulate a disk-oriented DBMS architecture similar to real-world systems.



# Objectives:

Manage fixed-size pages (4 KB each) between disk and memory
Improve performance using caching techniques
Handle concurrency with thread-safe data structures
Implement page replacement and disk scheduling mechanisms




# Main Components
# 1 )  LRU-K Replacement Policy

**Responsibilities**
- Track page access history
- Compute backward k-distance
- Evict the frame with largest backward distance
- Handle frames with fewer than K accesses (+∞ priority)

**Internal Functions (not shown in diagram)**
- RecordAccess(frame_id)
- Evict()
- Remove(frame_id)
- SetEvictable(frame_id, bool)
- Size()

![Uploading _مخطط _.drawio.png…]()

#  2 ) Disk Scheduler

Responsible for managing disk I/O requests asynchronously.

Uses a background worker thread
Processes read/write requests using a queue
Utilizes std::promise and std::future for synchronization
Ensures thread-safe communication between components

#  3 )  Buffer Pool Manager

The core of the project.

Fetches pages from disk into memory
Maintains a page table (mapping page IDs → frames)
Tracks pin count to prevent eviction of active pages
Handles dirty pages and ensures data consistency
Uses:
LRU-K replacer → for eviction decisions
Disk scheduler → for I/O operations

# Key Concepts:
Page vs Frame
Page: Logical data (stored on disk)
Frame: Physical memory slot holding a page
Pin Count
Prevents eviction while a page is in use
Dirty Page
Modified page that must be written back to disk before eviction
Thread Safety
All components are designed to support concurrent access

# Technologies Used:
C++17
STL Containers
Multithreading (mutexes, latches)
std::promise / std::future
GoogleTest (for unit testing)




