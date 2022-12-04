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
  Scheduler sch = Scheduler::getInstance([parameters]);
  WathdogTask wdt(TaskType::TIMED(100), [stackSize]);
  sch.createTask(wdt);
  UITask ui(TaskType::STANDARD, [stackSize]);
  scheduler.create(ui);
  PIDTask pid(TaskType::TIMED(50), [stackSize]);
  scheduler.start();
  // will never reach
  return 1;
}
```
      * 
