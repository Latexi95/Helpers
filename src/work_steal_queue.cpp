#include "../include/work_steal_queue.h"
using namespace work_steal;
queue::queue(context *ctx) :
    _context(ctx),
    _next(nullptr)
{
    _context->add_queue(this);
}

queue::~queue()
{
    _context->remove_queue(this);
}

