#pragma once

#include <expected>
#include <optional>
#include "../types.hpp"

namespace cynlr{

    enum class resource_type : u8 {
        dynamic_spsc_queue,
        static_spsc_queue,
        dynamic_spmc_queue,
        static_spmc_queue,
        dynamic_mpmc_queue,
        static_mpmc_queue,
        static_batch_queue,
        static_batch_buffer
    };

    enum class resource_type_error : u8 {
        failed_transaction_tx,
        failed_transaction_rx,
        invalid_size_tx,
        invalid_size_rx
    };

    constexpr auto resource_type_error_string(resource_type_error error) -> cstr {
        switch(error) {
            case resource_type_error::failed_transaction_tx:   return "Failed to send packet to resource";
            case resource_type_error::failed_transaction_rx:   return "Failed to receive packet from resource";
            case resource_type_error::invalid_size_tx:  return "Packet to large to send to resource";
            case resource_type_error::invalid_size_rx:  return "Packet to large to receive from resource";
            default:           return "Unkown error";
        }
    }

    template<typename T, typename DataType>
    concept shared_resource_like = requires(T resource, DataType in, DataType& out) {
        { T::type } -> std::convertible_to<resource_type>;
        { resource.receive(out) } -> std::same_as<std::expected<void, resource_type_error>>;
        { resource.transmit(in) } -> std::same_as<std::expected<void, resource_type_error>>;
    };


//     { pool.get_resource(name) } -> std::same_as<Resource&>;
// } && shared_resource_like<Resource, DataType>;

}