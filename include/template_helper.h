#pragma once
#include <utility>

template <typename T>
struct base_type {
    typedef typename std::remove_cv<std::remove_reference<T>::type>::type type;
};



