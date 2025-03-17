#pragma once

#include <span>
#include "../types.hpp"
#include "../tasks/task_types.hpp"

namespace cynlr {

    template<typename DataType, threshold_data_like ThresholdDataType>
    auto threshold_copy(std::span<const DataType> in, std::vector<ThresholdDataType>& thresholded_data, DataType threshold, usize front_offset, usize back_offset) -> void {
        usize iter_size = in.size() - front_offset - back_offset;
        usize end_iter = front_offset + iter_size;
        for(usize i = front_offset; i < end_iter; i++){
            thresholded_data.push_back(
                in[i] >= threshold ? 
                static_cast<ThresholdDataType>(1u) : 
                static_cast<ThresholdDataType>(0u)
            );
        }
    }



} // namespace cynlr