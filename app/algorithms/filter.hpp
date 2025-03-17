#pragma once

#include <iostream>
#include <cmath>
#include <span>
#include "../types.hpp"

namespace cynlr{

    template<typename DataType, typename FilterWeightDataType, usize WindowSize>
    auto filter_window(std::span<DataType> data, std::span<const FilterWeightDataType> filter_weights, usize front_offset, usize back_offset) -> void {
        usize iter_size = data.size() - front_offset - back_offset;
        DataType sum;

        if constexpr( WindowSize == 9 ){
            for(size_t i = front_offset; i < data.size() - back_offset; i++){
                sum = 
                    static_cast<DataType>(
                            (data[i - 4] * filter_weights[0]) 
                            + (data[i - 4 + 1] * filter_weights[1])
                            + (data[i - 4 + 2] * filter_weights[2])
                            + (data[i - 4 + 3] * filter_weights[3])
                            + (data[i - 4 + 4] * filter_weights[4])
                            + (data[i - 4 + 5] * filter_weights[5])
                            + (data[i - 4 + 6] * filter_weights[6])
                            + (data[i - 4 + 7] * filter_weights[7])
                            + (data[i - 4 + 8] * filter_weights[8])
                    );
                
                data[i] = sum;
            }
        }else{
            for (usize i = front_offset; i < data.size() - back_offset; i++) {
                sum = static_cast<DataType>(0);
                
                for (usize j = 0; j < WindowSize; j++) {
                    sum += data[i - WindowSize / 2 + j] * filter_weights[j];
                }
        
                data[i] = sum;
            }
        }
    }

    template<typename DataType, typename FilterWeightDataType, usize WindowSize>
    auto filter_window(std::span<DataType> data, std::vector<DataType>& out, std::span<const FilterWeightDataType> filter_weights, usize front_offset, usize back_offset) -> void {
        usize iter_size = data.size() - front_offset - back_offset;
        DataType sum;

        if constexpr( WindowSize == 9 ){
            for(size_t i = front_offset; i < data.size() - back_offset; i++){
                sum = 
                    static_cast<DataType>(
                            (data[i - 4] * filter_weights[0]) 
                            + (data[i - 4 + 1] * filter_weights[1])
                            + (data[i - 4 + 2] * filter_weights[2])
                            + (data[i - 4 + 3] * filter_weights[3])
                            + (data[i - 4 + 4] * filter_weights[4])
                            + (data[i - 4 + 5] * filter_weights[5])
                            + (data[i - 4 + 6] * filter_weights[6])
                            + (data[i - 4 + 7] * filter_weights[7])
                            + (data[i - 4 + 8] * filter_weights[8])
                    );
                
                data[i] = sum;
                out.push_back(sum);
            }
        }else{
            
            for (usize i = front_offset; i < data.size() - back_offset; i++) {
                sum = static_cast<DataType>(0);
                
                for (usize j = 0; j < WindowSize; j++) {
                    sum += data[i - WindowSize / 2 + j] * filter_weights[j];
                }
        
                data[i] = sum;
                out.push_back(sum);
            }
        }
    }

    



} // namespace cynlr