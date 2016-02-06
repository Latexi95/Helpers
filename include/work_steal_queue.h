#pragma once
#include "work.h"
#include <vector>
#include <mutex>
#include "work_steal_context.h"
#include "work.h"
namespace work_steal {

class queue {
public:
    friend class context;

    queue(context *ctx);
    virtual ~queue();
    virtual bool try_steal(work &w) = 0;
protected:
    virtual void work_exec_function(void *);

    context *_context;
    queue *_next;
};

class context;
template <typename WORK_EXEC, typename ITERATOR>
class iterator_queue {
public:
    iterator_queue(context *ctx, WORK_EXEC w, ITERATOR begin, ITERATOR end) :
        queue(ctx),
        _w(w), _i(begin), _end(end) {}
    ~iterator_queue() {}

    virtual bool try_steal(work &w) override;

    bool do_work();
private:
    virtual void work_exec_function(void *);

    WORK_EXEC _w;
    ITERATOR _i;
    ITERATOR _end;
    std::mutex _m;

};

template <typename WORK_EXEC, typename ITERATOR>
bool iterator_queue<WORK_EXEC, ITERATOR>::try_steal(work &w)
{
    std::unique_lock<std::mutex> lock(_m);
    if (_i == _end) return false;
    w._d = &(*_i);
    w._func = &iterator_queue<WORK_EXEC, ITERATOR>::work_exec_function;
    w._q = this;
    ++_i;
    return true;
}

template <typename WORK_EXEC, typename ITERATOR>
bool iterator_queue<WORK_EXEC, ITERATOR>::do_work()
{

}

template <typename WORK_EXEC, typename ITERATOR>
void iterator_queue<WORK_EXEC, ITERATOR>::work_exec_function(void *ptr)
{
    _w(*((decltype(&(*_i)))ptr));
}

}
