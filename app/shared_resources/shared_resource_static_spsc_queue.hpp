#pragma once 

#include <array>
#include <concepts>
#include <atomic>
#include <span>
#include <expected>

#include "shared_resource_base.hpp"
#include "shared_resource_types.hpp"

#include "../traits/type_format.hpp"
#include "../traits/type_args.hpp"
#include "../logging.hpp"
#include "../concepts.hpp"

namespace cynlr {

template<typename T, usize N, typename... Args> requires power_of_two<usize, N>
struct shared_resource<resource_type::static_spsc_queue, T, std::integral_constant<usize, N>, Args...> : 
    public type_args_trait<Args...>,
    public type_format_trait
{
    static constexpr resource_type type = resource_type::static_spsc_queue;

    shared_resource(Args&&... args) : 
        type_args_trait<Args...>(args...)
    {
        // Log(std::format("Generic case shared_resource<resource_type::static_spsc_queue, {}, {}> with Args:" , type_info<T>::name(), buffer.size()));
        // this->print_args();
    }

    auto receive(T& out) -> std::expected<void, resource_type_error> {
        usize current_tail = tail.load(std::memory_order_relaxed);
		if (current_tail != head.load(std::memory_order_relaxed)) {
			out = buffer[current_tail];
			tail.store((current_tail + 1) & (N - 1), std::memory_order_release);
			return {};
		}
		return std::unexpected{resource_type_error::failed_transaction_rx}; // Queue is empty
    }

    auto transmit(const T in) -> std::expected<void, resource_type_error>{
        usize current_head = head.load(std::memory_order_relaxed);
		usize next_head = (current_head + 1) & (N - 1);
		if (next_head != tail.load(std::memory_order_relaxed)) {
			buffer[current_head] = in;
			head.store(next_head, std::memory_order_release);
			return {};
		}
		return std::unexpected{resource_type_error::failed_transaction_tx}; // Queue is full
    }

private:
    std::array<T, N> buffer;
    std::atomic<usize> head{0};
    std::atomic<usize> tail{0};
};

} // namespace cynlr