#pragma once


#include <stop_token>
#include <format>

#include "types.hpp"
#include "logging.hpp"
#include "tasks/task_types.hpp"
#include "tasks/task_data_generator.hpp"
#include "tasks/task_filter_threshold.hpp"

namespace {

inline auto task_error_log(cstr name, cynlr::task_error error) -> void {
    cynlr::Log(std::format("Task [{}] stopped due to reason: {}", name, task_error_string(error)));
}
    
} 


namespace cynlr {

auto create_task(task_like auto& task, std::stop_token token) -> void {
    std::expected<task_response, task_error> result;

    result = task.setup();
    task_settings settings = task.task_settings();

    if (!result.has_value()){

        task_error_log(settings.task_name, result.error()); 
        result = task.stop();
        if (!result.has_value())
            task_error_log(settings.task_name, result.error());
        return;  

    } else {
        for(;;){

            if (token.stop_requested()){
                task_error_log(settings.task_name, task_error::task_terminated);
                task.stop();
                return; 
            }

            auto start_time = std::chrono::high_resolution_clock::now();
            result = task.loop();
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();

            if (result.has_value()){

                const task_response response = result.value();
                switch(response){
                    case task_response::stop : {
                        result = task.stop();
                        if (!result.has_value())
                            task_error_log(settings.task_name, result.error());
                        return;  
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

            }
            else{
                task_error_log(settings.task_name, result.error()); 
                result = task.stop();
                if (!result.has_value())
                    task_error_log(settings.task_name, result.error());
                return;
            }
        }

    }

    result = task.stop();
    if (!result.has_value())
        task_error_log(settings.task_name, result.error());  
}

} // namespace cynlr