#pragma once

#include <cmath>
#include <span>
#include "../types.hpp"

namespace cynlr{

    template<typename DataType, typename FilterWeightDataType>
    auto filter_window(std::span<DataType> data, std::span<const FilterWeightDataType> filter_weights, usize front_offset, usize back_offset) -> void {
        FilterWeightDataType sum;
        DataType elem;
        usize iter_size = data.size() - front_offset - back_offset;
    
        // Ensure that the filter weights match the window size (i.e., filter_weights.size() should be equal to window size)
        usize window_size = filter_weights.size();
        for (usize i = front_offset; i < data.size() - back_offset; i++) {
            sum = static_cast<FilterWeightDataType>(0);
            
            // Sum the weighted data values for the window
            for (usize j = 0; j < window_size; j++) {
                sum += data[i - window_size / 2 + j] * filter_weights[j];
            }
    
            elem = static_cast<DataType>(std::floor(sum));
            data[i] = elem;
        }
    }



} // namespace cynlr