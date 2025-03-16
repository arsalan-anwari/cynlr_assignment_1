#pragma once

#include <atomic>
#include <thread>
#include <vector>
#include <functional>
#include <unordered_map>

#include "task.hpp"

namespace cynlr {

enum class schedular_response {
    success, stop, error
};

struct scheduler {

    auto add_task(task_like auto& task) -> task_handle {
    
        auto handle = std::make_shared<std::stop_source>();
        std::stop_token stop_token = handle->get_token();

        threads.emplace_back([&task, stop_token]() {
            create_task(task, stop_token);
        });

        task_handles.push_back(handle);
        return handle;

    }
    
    auto start(time_ns poll_rate, const std::function<schedular_response()>& main_loop_task) -> void {
        schedular_response result;
        for(;;){
            result = main_loop_task();
            if (result != schedular_response::success){
                stop();
                return;
            }
            std::this_thread::sleep_for(std::chrono::nanoseconds(poll_rate));
        }
    };

    auto stop() -> void {
        for(auto handle: task_handles){
            handle->request_stop();
        }
    }

    auto stop_task(task_handle handle){
        handle->request_stop();
        handle.reset();
    }

private:
    std::vector<std::jthread> threads;
    std::vector<task_handle> task_handles;
};



}