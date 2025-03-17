#pragma once

#include <chrono>
#include <stop_token>
#include <format>

#include "types.hpp"
#include "logging.hpp"

#include "tasks/task_types.hpp"

#include "tasks/task_data_generator/task_data_generator_debug.hpp"
#include "tasks/task_data_generator/task_data_generator_testing.hpp"
#include "tasks/task_data_generator/task_data_generator_profile.hpp"

#include "tasks/task_filter_threshold/task_filter_threshold_testing.hpp"
#include "tasks/task_filter_threshold/task_filter_threshold_debug.hpp"
#include "tasks/task_filter_threshold/task_filter_threshold_profile.hpp"

namespace {

inline auto task_error_log(cstr name, cynlr::task_error error) -> void {
    cynlr::Log(std::format("Task [{}] stopped due to reason: {}", name, task_error_string(error)));
}
    
} 


namespace cynlr {

auto create_task(task_like auto& task, std::stop_token token) -> void {
    std::expected<task_response, task_error> result;
    task_state state = task_state::starting;
    task_settings settings = task.task_settings();

    for(;;){

        switch(state){
            case task_state::starting : {
                result = task.setup();
                state = result.has_value() ? task_state::running : task_state::error;
                break;
            }

            case task_state::running : {
                
                if (token.stop_requested()){
                    task_error_log(settings.task_name, task_error::task_terminated);
                    state = task_state::stopped; 
                }

                auto start_time = std::chrono::high_resolution_clock::now();
                result = task.loop();
                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();

                if(!result.has_value()){
                    state = task_state::error;
                    break;
                }

                const task_response response = result.value();
                switch(response){
                    case task_response::stop : {
                        state = task_state::stopped;
                        break;
                    }
                    case task_response::yield : {
                        std::this_thread::yield();
                        break;
                    }
                    case task_response::success : {
                        if (duration < settings.deadline){
                            std::this_thread::sleep_for(std::chrono::nanoseconds(settings.deadline - duration));
                        }
                    }
                }

                break;
    
            }

            case task_state::error : {
                task_error_log(settings.task_name, result.error());
                state = task_state::stopped;
                break;
            }

            case task_state::stopped : {
                result = task.stop();
                if (!result.has_value())
                    task_error_log(settings.task_name, result.error());
                
                return;
            }
                
        };

    }

}

} // namespace cynlr