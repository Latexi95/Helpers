#pragma once

namespace work_steal {
class queue;
typedef void (queue:: *work_exec_func)(void *);
struct work {
    void exec() {
        (_q->*_func)(_d);
    }

    work_exec_func _func;
    void *_d;
    queue *_q;
};

}

