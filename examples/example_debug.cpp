#include "data_pool.hpp"
#include "task.hpp"
#include "types.hpp"
#include "scheduler.hpp"

using namespace cynlr;

int main(){
    static constexpr usize application_run_time_s = 10u;
    static constexpr pool_type data_pool_mode = pool_type::producer_consumer_debug_static;
    
    using data_pool_type = data_pool<data_pool_mode, u8, 32u, 2u>;
    data_pool_type pool;
    
    std::array<f32, 9> filter_weights = {0.00025177, 0.008666992, 0.078025818, 0.24130249, 0.343757629, 0.24130249, 0.078025818, 0.008666992, 0.000125885};
    
    task_data_generator<task_mode::debug, data_pool_type> task1(pool);
    task_filter_threshold<task_mode::debug, data_pool_type, f32, u8, 4u> task2(pool, filter_weights, 175u);

    scheduler sched;
    sched.add_task(task1);
    sched.add_task(task2);

    std::this_thread::sleep_for(std::chrono::seconds(application_run_time_s));

    sched.stop();
    return 0;
}
