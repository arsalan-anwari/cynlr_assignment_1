#pragma once

#include <expected>
#include <concepts>
#include <memory>
#include <stop_token>

#include "../types.hpp"

namespace cynlr {

enum class task_error : u8 {
    failed_init, failed_exit, task_terminated
};

enum class task_response : u8 {
    yield, stop, success
};

enum class task_mode : u8 {
    realtime, testing, debug
};

struct task_settings {
    cstr task_name;
    time_ns deadline;
};

constexpr auto task_error_string(task_error error) -> cstr {
    switch(error) {
        case task_error::failed_init:   return "Failed to initialize task";
        case task_error::failed_exit:   return "Failed to exit task";
        case task_error::task_terminated:  return "Task has been terminated";
        default:           return "Unkown error";
    }
}

template <typename T>
concept task_like = requires(T t) {
    { t.setup() } -> std::same_as<std::expected<task_response, task_error>>; 
    { t.loop() } -> std::same_as<std::expected<task_response, task_error>>; 
    { t.stop() } -> std::same_as<std::expected<task_response, task_error>>;
    { t.task_settings() } -> std::convertible_to<task_settings>;
};

using task_handle = std::shared_ptr<std::stop_source>;

} // namespace cynlr