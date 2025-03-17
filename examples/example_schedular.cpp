#include "task.hpp"
#include "scheduler.hpp"

using namespace cynlr;

// Example Task 1
struct TaskA {
	auto task_settings() -> task_settings { return {"Task A", 1'000'000'000}; };

    std::expected<task_response, task_error> setup() {
        Log("TaskA init");
        return task_response::success;
    }
    
    std::expected<task_response, task_error> loop() {
		Log(std::format("TaskA running iteration [{}]", counter++));
		return task_response::success;
    }
    
    std::expected<task_response, task_error> stop() { 
		Log("TaskA exit");
		return task_response::success; 
	}

private:
    usize counter{0};
};

struct TaskB {
	auto task_settings() -> task_settings { return {"Task B", 0}; };

    std::expected<task_response, task_error> setup() {
        Log("TaskB init");
        return task_response::success;
    }
    
    std::expected<task_response, task_error> loop() {
		Log(std::format("TaskB running iteration [{}]", counter++));
        std::this_thread::sleep_for(std::chrono::nanoseconds(5'000'000'000)); // simulate some heavy operation
		return task_response::yield;
    }
    
    std::expected<task_response, task_error> stop() { 
		Log("TaskB exit");
		return task_response::success; 
	}

private:
    usize counter{0};
};

struct TaskC {
	auto task_settings() -> task_settings { return {"Task C", 5'000'000'000}; };

    std::expected<task_response, task_error> setup() {
        Log("TaskC init");
        return task_response::success;
    }
    
    std::expected<task_response, task_error> loop() {
		Log(std::format("TaskC running iteration [{}]", counter++));
		return counter > 10 ? task_response::stop : task_response::success;
    }
    
    std::expected<task_response, task_error> stop() { 
		Log("TaskC exit");
		return task_response::success; 
	}

private:
    usize counter{0};
};

// Main Function
int main() {
    TaskA taskA;
    TaskB taskB;
    TaskC taskC;

    scheduler sched;
    auto handleA = sched.add_task(taskA);
    auto handleB = sched.add_task(taskB);
    auto handleC = sched.add_task(taskC);

    usize counter{0};

    sched.start(1'000'000'000,
        [&sched, &counter, handleB](){
        if (counter == 5){ 
            Log("Stopping TaskB from main thread");
            sched.stop_task(handleB); 
        }
        if (counter > 55u){
            Log("Stopping schedular from main thread");
            return schedular_response::stop;
        }
        Log(std::format("Main thread running iteration [{}]", counter));
        counter++;
        return schedular_response::success;
    });

    return 0;
}
