#pragma once

#include "../task_types.hpp"

namespace cynlr {

    template<
        task_mode Mode,
        data_pool_like DataPoolType,
        std::floating_point FilterWeightDataType,
        threshold_data_like ThresholdDataType,
        usize FilterWindowRadius
    >
    struct task_filter_threshold;


} // namespace cynlr