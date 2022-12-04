# TODO planned changes

* **Create issues for development tasks**
* Create UML class diagram of proposed design
* Revise build
  * Use CMake, Docker, Github Workflow for CI
* Remove hardcoded dependencies on hardware
  * Determine memory locations (stack, heap) dynamically
* Provide HAL with required abstractions
  * tick/millisecond/microsecond counters for scheduling/delay/elapsed
  * resetWatchdog()
  * sleep (low power mode)
* Allow operating in host environment (linux, windows, macos)
* Main (master) process is NOT a scheduler task. Once scheduler is initialized and tasks are created, main process calls scheduler to start - never returns
* Tasks
  * Tasks are instantiated from classes, implementing a Runnable interface ``int run()``. Scheduler reference is injected.
  * Task types:
    * Standard (run until done). May be restarted?
    * Scheduled at fixed intervals, e.g. every 100 ms. Scheduler (re-)starts after interval.
  * Introduce task state
    * READY: created. Ready to be started
    * RUNNING: actively running or waiting to be scheduled
    * WAITING: waiting for resource
    * SLEEPING: caused by task call to sleep
    * SUSPENDED: timed/scheduled task, waiting to be restarted on timed schedule
    * DONE: finished running. May be manually restarted
* Scheduler
    * Task can request scheduler to sleep(milliseconds). Scheduler will start timer, count down at each scheduling opportunity before scheduling to run.
    * Allocate task object with stack on heap using new, deallocate in destructor. Mostly, deallocation will never happen.
    * Allow task deletion
    * Allow task restart
    * Low power sleep. If all tasks have been scheduled without running: enter low power sleep mode via HAL.
    * Consider implementing await() as scheduler method with lambda
    * Logging
      * Scheduler events (task creation, start, done)
      * Output task status (single or all tasks): state, free stack
    * API inspired by FreeRTOS
 
```C++
int main() {
  // Schedulers schedule tasks round-robin.
  // Only when all tasks are idle (sleeping, waiting or done) will time be allocated to child scheduler.
  // Only the primary (highest priority) scheduler can be started directly. When started, it will start any children (recusively).
  // No more than one child scheduler can be created from any scheduler.
  
  Scheduler scheduler1(); // primary scheduler. High priority.
  Scheduler scheduler2 = scheduler1.child(); // Medium priority.
  Scheduler idleScheduler = scheduler2.child(); // Low priority.
  
  WathdogTask<256> wdt(100ms); // 256 byte stack, 100 ms scheduler interval
  scheduler1.addTask(wdt);

  PIDTask<512> pid(50ms);
  scheduler1.addtask(pid);
  
  UITask<512> ui();
  scheduler2.addTask(ui);
  
  StatisticsTask<256> stat(10s); // reports statistics about schedulers, tasks, stack watermarks etc every 10 seconds
  scheduler2.addTask(stat);
  
  IdleTask<128> idle(); // Idle task may perform housekeeping and even cause hardware to enter low-power sleep mode
  idleScheduler.addTask(idle);
  
  scheduler.start(); // Start cascades to child schedulers
  
  // will never reach this point ...
  return 1; // ... but if it happens, it would be an error
}
```
      * 
