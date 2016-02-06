#include "../include/work_steal_context.h"
#include "work_steal_queue.h"
#include <cassert>
using namespace work_steal;
context::context() :
    _first(nullptr),
    _last(nullptr)
{
}

context::~context()
{
    _m.lock();
    assert(_first == nullptr &&
           "work_steal_context destroyed before all assossiated work_steal_queues where destroyed");
    _m.unlock();
}

bool context::try_do_work()
{
    work w;
    if (!try_steal(w)) {
        return false;
    }

    w.exec();
    return true;
}

bool context::try_steal(work &w)
{
    std::unique_lock<std::mutex> lock(_m);
    queue *it = _first;
    while (it) {
        if (it->try_steal(w)) return true;
        it = it->_next;
    }
    return false;
}

void context::add_queue(queue *q)
{
    std::unique_lock<std::mutex> lock(_m);
    if (!_first) {
        _first = q;

    }

    _last->_next = q;
    _last = q;
}

void context::remove_queue(queue *q)
{
    std::unique_lock<std::mutex> lock(_m);

    if (_first == q) {
        _first = q->_next;
        if (!_first) {
            _last == nullptr;
        }
        return;
    }

    queue *it = _first;
    while (it) {
        if (it->_next == q) {
            it->_next = q->_next;
            if (_last == q) {
                _last = it;
            }
            return;
        }
        it = it->_next;
    }
    assert(0 && "Tried remove a work_steal_queue from a wrong context");
}
