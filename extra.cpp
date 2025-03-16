// #include "task.hpp"
// #include "schedular.hpp"

// // Example Task 1
// struct TaskA {
// 	auto task_name() -> cstr { return "Task A"; };

//     std::expected<void, cynlr::task_error> init() {
//         cynlr::Log("TaskA init");
//         return std::unexpected(cynlr::task_error::failed_init);
//     }
    
//     std::expected<void, cynlr::task_error> loop() {
// 		cynlr::Log("TaskA running");
// 		std::this_thread::sleep_for(std::chrono::milliseconds(500));
// 		return {};
//     }
    
//     std::expected<void, cynlr::task_error> exit() { 
// 		cynlr::Log("TaskA exit");
// 		std::this_thread::sleep_for(std::chrono::milliseconds(50));
// 		return {}; 
// 	}
// };

// // Example Task 2
// template<typename T>
// struct TaskB {
// 	auto task_name() -> cstr { return "Task A"; };

//     std::expected<void, cynlr::task_error> init() {
// 		cynlr::Log("TaskB init");
// 		return {};
// 	}
    
//     std::expected<void, cynlr::task_error> loop() {
// 		cynlr::Log(std::format("Task B running with counter = {}", counter));
// 		std::this_thread::sleep_for(std::chrono::milliseconds(250));
// 		if (counter++ > 10){
// 			return std::unexpected{cynlr::task_error::loop_stopped};
// 		}
// 		return {};
//     }
    
//     std::expected<void, cynlr::task_error> exit() { 
// 		cynlr::Log("TaskB exit");
// 		std::this_thread::sleep_for(std::chrono::milliseconds(50));
// 		return {}; 
// 	}

// private:
// 	T counter = 0;
// };

// // Main Function
// int main() {
//     TaskA task1;
//     TaskB<int> task2;

//     cynlr::schedular scheduler;
//     scheduler.add_task(task1);
//     scheduler.add_task(task2);

// 	std::this_thread::sleep_for(std::chrono::seconds(10)); // Let tasks run for 10 sec
    
// 	// cynlr::Log("Forcefully stopping all tasks....");
//     // scheduler.stop();

//     return 0;
// }

// #include <iostream>
// #include "shared_resource.hpp"

// using namespace cynlr;

// int main(){

// 	shared_resource<resource_type::static_batch_buffer, int, csize<2>, float, int, bool> res(45.245f, 20053, true);
// 	shared_resource<resource_type::static_batch_buffer, int, csize<2'000>, float, float, float> res2(45.245f, 5233.223, 0.333);
// 	shared_resource<resource_type::static_batch_buffer, int, csize<12'000>, bool, cstr> res3(false, "Hello");
// }
