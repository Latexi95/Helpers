#pragma once
#include <mutex>
#include <vector>
namespace work_steal {


class work;
class queue;
class context {
    friend class queue;
    context();
    ~context();

    bool try_do_work();

    bool try_steal(work &w);
private:
    void add_queue(queue *q);
    void remove_queue(queue *q);
    std::mutex _m;
    queue *_first;
    queue *_last;
};

}
