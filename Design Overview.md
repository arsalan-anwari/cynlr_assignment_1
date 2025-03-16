# Design overview Document

The design decisions for this project are based on the assumption that the system will be ran on an embedded linux controller with limited memory, a processor supporting SIMD instructions and the availability of a small GPU. Due to portability of this assignment I will stick to the desktop equivalent features. 

In the assignment it became clear that this project will be the base for two other blocks later to be supported in assignment 2. I will therefor add some extra boiler plate code in anticipation for this next assignment. This makes it easier for me to make changes later. 

I have taken into account that the code is scalable where performance is not critical but still robust in places where its needed. This might reduce the total re-usability of the codebase, but in my personal experience performance critical code rarely gets or needs to be changed if written correctly. 

This overview is written from a hypothesis point of view, but different alternatives have been benchmarked to derive these claims. These benchmarks can be found in the folder `benchmark/alternative_hypothesis`. Any code examples in this overview is also a simplification of the actual code and in no way represent the final product, it simply serves to clarify the points made. 

In the next chapters I will go through each design decision and how it compares to alternatives. 

## Task Scheduler

### Virtual threads vs regular threads

The assignment explicitly mentioned that the two Blocks (herby referred to as Tasks) needed to be ran concurrently and their execution being independent from each other. This constraint limits the use of virtual threads (`<coroutine>`) in a single thread. Virtual threads are more lightweight and have the benefit of reduced latency to switch between contexts. The main bottleneck of normal threads in regards to latency is the implicit need for synchronization methods like barriers, locks, mutexes and semaphores when exchanging data. However regular threads have more fine grained control and can be made to be equally or more performant by using lockless data structures.

Another reason for using normal threads over virtual threads was that it reduces the overhead needed to communicate with the new tasks in the future assignment as you don't need to make major modifications how tasks communicate with each other. For example if Task 3 need to also receive data from Task 1, it can simply subscribe to the same shared reader queue. Managing coordination between virtual threads in code can become more difficult and unreadable when the tasks have co-dependence. 

For these reasons it was decided to use regular threads to represent the context of a task. 

### Virtual methods vs static polymorphism

The most logical and readable way to represent a Task is to create an interface which different Task implementations can inherit from like below.
```c++
class Task {
public:
    virtual void init() = 0;
    virtual void run() = 0;
    virtual void exit() = 0
};

class Task1 : public Task {
public:
    void init() override;
    void run() override;
    void exit() override;
};

class Schedular {
// ...

private:
    std::vector<Task> tasks;
};
```

This method however is not the most performant as you are suffering a performance penalty when you are using virtual functions. Virtual functions are implemented using a `vtable` where each call to the method requires a lookup in the `vtable` to find the function address (equivalent to 1 or more pointer de-aliasing). It can also lead to stalls in branch predictions by the CPU, alongside making it harder for the compiler to perform optimizations like inlining or constant propagation. For most non-deterministic systems like GUI applications or Video games this is not much of an issue, but for high performance systems that need to be deterministic this is not an option. 

Therefor a different approach was chosen to decouple the inheritance model and use a `Higher order function` to represent a `Thread`. This is a functional programming term for a `Functor` which takes one or more `Functor-like` object. Then, it returns a function as an output:

```c++
struct Task1 {
    void init();
    void run();
    void exit();
    bool running;
};

struct Task2 {
    void init();
    void run();
    void exit();
    bool running;
};

template<typename T>
void Thread(T& task){
    task.init();
    while(task.running){
        task.run();
    }
    task.exit();
}

class Scheduler {
private:
    std::vector<std::thread> threads;

public:
    template <typename T>
    void addTask(T& task) {
        threads.emplace_back(Thread<T>, std::ref(task));
    }
// ...
};
```

This is a simplified example, but as you can see this design allows for flexibility how each task is defined (new custom methods and logic can be added) whilst still having the restriction that the Task needs to conform to three base methods. This design also does not use any virtual functions or produces indirect function calling. In many cases the compiler can even inline the calls to the methods of `Task1` or `Task2` as there any no function pointers used (`<functional>`).

This method can be made much safer at compile time using Pre-Conditions (`<concepts>`, `static_assert()`) or at run time using Post-Conditions (`<expected>`, `<optional>`, `<exception>`, `assert()`).

Because the system needs to be deterministic this method was used to represent tasks. 

## Data Representation

In the description of the assignment it was stated that the line camera was scanning an roll of a fixed width and infinite height represented as a 2D grid of columns `m` and rows `n`. Each element of the grid represents an 8bit intensity pixels where a boolean is used a meta data for each pixels to flag is a pixel is defective or not. 

### Row-major vs Column-major

Logically it would make sense to represent the data as a 2D column-major array but an 1D array with row-major order is better for latency because it accesses memory sequentially, optimizing cache usage. Column-major order accesses memory non-sequentially, causing more cache misses and higher latency. Therefor a 1D array with row-major ordering was used. 

### Array of Structs vs Structs of Arrays

The pixel data and meta data could logically be explained using a AoS structure like: 
```c++
struct pixel {
    uint8_t intensity;
    bool defective;
};

std::vector<pixel> data;
```

But using a Struct of Arrays (SoA) is better than Array of Structs (AoS) for performance because SoA allows for more efficient memory access patterns. In SoA, similar data types are stored contiguously, improving cache utilization and enabling vectorized operations, which leads to faster processing and reduced latency. So you cna create a structure like:
```c++
struct pixel_data{
    std::vector<uint8_t> intensities;
    std::vector<bool> defective_mask;
}
```

Therefor a SoA was used to store all pixels and meta data. 

### Pixel Metadata. 

Due to the fact that the rows `m` are fixed, it is possible to compact the amount of storage needed for the boolean mask for the defective pixels. Multiple pixel could be represented as the bits in an `uint64_t` reducing the memory needed to store all boolean elements. This in fact is what happens by default with the `std::vector<bool>` specialization. Unlike with other data types when using a `std::vector` booleans are compacted as a bit mask and then bitwise operations are used to access individual elements. This a design choice dating back to `C++98` to reduce the memory when using booleans with containers. This is because a boolean normally is represented as an 8bit value because this is the smaller amount of memory you can allocate and define in a 64 bit architecture. 

Therefor it might seem logical to represent the defective mask as a `std::vector<bool>`. But it should be noted that element access can be significantly slower when the vector size is not divisible by a power of 2 or when the number of elements in the vector is large. This is because additional logic needs to be used to calculate which bit in which `uint64_t` of the underlying vector represent the boolean at that index. 

Another downside from an architectural and future proofing aspect of using boolean to represent the meta data, is that its not very flexible when the meta data represents a state. For example instead of only detecting defective or correct pixels, a future implementation might also need to flag pixels to ignore in a calculation. To do this only using booleans you would have to create a new vector each time you need a new state, significantly adding up the total needed memory in the application as an `std::vector` also requires some variables and data to operate. 

Therefor a better approach would be to represent the metadata as an sized enum like:
```c++
enum class pixel_meta : uint8_t{
    DEFECTIVE, CORRECT, IGNORE, //...
}; 

std::vector<pixel_meta> meta_data;
```

The benefit being that you can compare pixel states as named variables and have the guarantee that the state only used a fixed amount of memory. Another benefit is that as the application grows and you introduce more states than can be represented as an 8bit value, you can use a bigger sized data type. 

You could even use bitwise operations if you want to define a pixel that has multiple state that can be true like this:
```c++
enum class pixel_meta : uint8_t {
    DEFECTIVE = 1 << 0, // 00000001
    CORRECT   = 1 << 1, // 00000010
    IGNORE   = 1 << 2, // 00000100
    // Add more states as needed
};

// Define a pixel with multiple states
uint8_t pixel_state = static_cast<uint8_t>(pixel_meta::DEFECTIVE) | static_cast<uint8_t>(pixel_meta::IGNORE);

if (
    (pixel_state & (static_cast<uint8_t>(pixel_meta::DEFECTIVE) | static_cast<uint8_t>(pixel_meta::IGNORE)))
    == (static_cast<uint8_t>(pixel_meta::DEFECTIVE) | static_cast<uint8_t>(pixel_meta::IGNORE))
) {
    std::cout << "Pixel is both DEFECTIVE and IGNORED" << std::endl;
}
```

So for there reasons it was decided that a sized enum type was better suited to represent the pixel meta data. 

## Data Encapsulation vs External Data Class

In the assignment it was stated that data needed to be shared between different tasks in a uni-directional manner from `Task 1` to `Task 2`. It was never explicitly stated that there was a need for bi-directional data access between two or more Tasks, but this could implicitly be speculated from the two remaining tasks to be implemented in assignment 2. 

Having data tightly coupled with the logic generally is better for the latency within the same Task, as the data has a higher chance of being in a cache when instructions of a function require it. But this has the downside that its less flexible and scalable when data access patterns between one or more tasks become complex. This is because the access management needs to be done within the logic of the task, making it that you need to apply changes in multiple places of tasks for it work correctly.

Another concern is that the latency will be higher when another Task needs to access or modify the data as the function of that Task might not be closely located as the data of the other Task in memory.

Therefor it was decided to create a External Data Class with which different Tasks could subscribe to and access is handled through `Getters` and `Setters` with the ability to send or receive data in bursts when available. All of the synchronization, and locking/unlocking will be handled internally by the External Data Class like this:

```c++
template<typename T>
class DataPool{
public:
    enum class mode { READ, WRITE, RW, DMA_READ, DMA_WRITE, DMA_RW }

    const T& get(size_t id, size_t offset); // READ, RW
    void set(size_t id, T&& value); / // WRITE, RW
    const std::span<T> read(size_t id, size_t offset, size_t size); // DMA_READ, DMA_RW
    void write(size_t id, size_t offset, std::span<T> data); // DMA_WRITE, DMA_RW

    template<mode M>
    size_t subscribe(); // return id based on mode

    void unsubscribe(size_t id);
// ...
}
```

This API makes it easier in the future to change how different Tasks access the same shared data without having to modify this behavior in logic of the task itself. 