#pragma once

#include <tuple>

#include "type_format.hpp"
#include "../logging.hpp"

namespace cynlr {

template<typename... Args>
struct type_args_trait {

    type_args_trait(Args... args) :
        args_(std::make_tuple(args...)) 
    {}
        
    auto print_args() -> void { print_args_(args_); };

private:
    std::tuple<Args...> args_;

    template <usize Index = 0, typename TupleType>
    auto print_args_(const TupleType& tuple) -> void {
        if constexpr (Index < std::tuple_size<TupleType>::value) {
            
            // Print the element and its type
            using index_type = std::decay_t<decltype(std::get<Index>(tuple))>;
            cstr type_str = type_format_trait::type_info<index_type>::name();

            Log(std::format("[{}] : {} -> {}", Index, std::get<Index>(tuple), type_str));  

            print_args_<Index + 1>(tuple); // Recursively print the next element in the tuple
        }
    };

};


} // namespace cynlr