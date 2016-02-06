#pragma once

#include "work_steal_context.h"
#include "work_steal_queue.h"
#include "template_helper.h"

namespace work_steal {

template <typename WORK_EXEC, typename ITERATOR>
void exec_task(context &c, WORK_EXEC &&w, ITERATOR &&data_begin, ITERATOR &&data_end) {
    iterator_queue<WORK_EXEC, typename base_type<ITERATOR>::type> q(&c, w, data_begin, data_end);

}
}
