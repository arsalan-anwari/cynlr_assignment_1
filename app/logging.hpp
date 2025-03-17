#pragma once

#include <mutex>
#include <iostream>
#include <string_view>

namespace {

inline std::mutex log_lock;

}

namespace cynlr {

inline auto Log(std::string_view text) -> void {
	log_lock.lock();
	std::cout << text << std::endl;
	log_lock.unlock();
};

} // namespace cynlr