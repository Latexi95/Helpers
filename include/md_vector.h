#ifndef MD_VECTOR_H
#define MD_VECTOR_H
#include <initializer_list>
#include <vector>
#include <array>

enum class md_vector_order {
    row_major,
    column_major
};

namespace {

template <typename T, std::size_t DIM_COUNT, md_vector_order ORDER>
struct md_vector_base;

template <std::size_t DIM_COUNT, std::size_t INDEX>
struct map_helper_row_major {
    template <typename HELPER_A, typename HELPER_B, typename FUNC>
    static void map(const std::size_t *dims_a, HELPER_A &&a, const std::size_t *dims_b, HELPER_B &&b, FUNC &&f) {
        std::size_t size = std::min(dims_a[DIM_COUNT - INDEX - 1], dims_b[DIM_COUNT - INDEX - 1]);
        for (std::size_t i = 0; i < size; ++i) {
             map_helper_row_major<DIM_COUNT, INDEX-1>::map(dims_a, a[i], dims_b, b[i], f);
        }
    }
};



template <std::size_t DIM_COUNT>
struct map_helper_row_major<DIM_COUNT, 0> {
    template <typename HELPER_A, typename HELPER_B, typename FUNC>
    static void map(const std::size_t *dims_a, HELPER_A &&a, const std::size_t *dims_b, HELPER_B &&b, FUNC &&f) {
        std::size_t size = std::min(dims_a[DIM_COUNT - 1], dims_b[DIM_COUNT - 1]);
        for (std::size_t i = 0; i < size; ++i) {
            f(a[i], b[i]);
        }
    }
};

template <std::size_t DIM_COUNT, std::size_t INDEX>
struct map_helper_column_major {
    template <typename HELPER_A, typename HELPER_B, typename FUNC>
    static void map(const std::size_t *dims_a, HELPER_A &&a, const std::size_t *dims_b, HELPER_B &&b, FUNC &&f) {
        std::size_t size = std::min(dims_a[DIM_COUNT - INDEX - 1], dims_b[DIM_COUNT - INDEX - 1]);
        for (std::size_t i = 0; i < size; ++i) {
             map_helper_column_major<DIM_COUNT, INDEX-1>::map(dims_a, a[i], dims_b, b[i], f);
        }
    }
};



template <std::size_t DIM_COUNT>
struct map_helper_column_major<DIM_COUNT, 0> {
    template <typename HELPER_A, typename HELPER_B, typename FUNC>
    static void map(const std::size_t *dims_a, HELPER_A &&a, const std::size_t *dims_b, HELPER_B &&b, FUNC &&f) {
        std::size_t size = std::min(dims_a[DIM_COUNT - 1], dims_b[DIM_COUNT - 1]);
        for (std::size_t i = 0; i < size; ++i) {
            f(a[i], b[i]);
        }
    }
};


template <typename T, std::size_t DIM_COUNT, std::size_t INDEX>
struct access_helper_row_major {
    typedef access_helper_row_major<T, DIM_COUNT, INDEX - 1> result_type;
    access_helper_row_major(const std::size_t *dims,
                            T *data,
                            const std::size_t offset) :
        dims_(dims), data_(data), offset_(offset) {}

    result_type operator[](std::size_t idx) {
        std::size_t mult = 1;
        for (std::size_t i = DIM_COUNT - INDEX; i < DIM_COUNT; ++i) {
            mult *= dims_[i];
        }
        return result_type(dims_, data_, offset_ + mult * idx);
    }

    const std::size_t *dims_;
    T *data_;
    const std::size_t offset_;
};

template <typename T, std::size_t DIM_COUNT>
struct access_helper_row_major<T, DIM_COUNT, 0> {
    typedef T &result_type;

    access_helper_row_major(const std::size_t *dims,
                            T *data,
                            const std::size_t offset) :
        dims_(dims), data_(data), offset_(offset) {}

    T &operator[](std::size_t idx) {
        return data_[offset_ + idx];
    }

    const std::size_t *dims_;
    T *data_;
    const std::size_t offset_;
};

template <typename T, std::size_t DIM_COUNT, std::size_t INDEX>
struct access_helper_column_major {
    typedef access_helper_column_major<T, DIM_COUNT, INDEX - 1> result_type;
    access_helper_column_major(const std::size_t *dims,
                               T *data,
                               const std::size_t offset) :
        dims_(dims), data_(data), offset_(offset) {}

    result_type operator[](std::size_t idx) {
        std::size_t mult = 1;
        for (std::size_t i = INDEX; i < DIM_COUNT - 1; ++i) {
            mult *= dims_[i];
        }
        return result_type(dims_, data_, offset_ + mult * idx);
    }

    const std::size_t *dims_;
    T *data_;
    const std::size_t offset_;
};

template <typename T, std::size_t DIM_COUNT>
struct access_helper_column_major<T, DIM_COUNT, 0> {
    typedef T &result_type;

    access_helper_column_major(const std::size_t *dims,
                               T *data,
                               const std::size_t offset) :
        dims_(dims), data_(data), offset_(offset) {}

    T &operator[](std::size_t idx) {
        std::size_t mult = 1;
        for (std::size_t i = 0; i < DIM_COUNT - 1; ++i) {
            mult *= dims_[i];
        }
        return data_[offset_ + idx * mult];
    }

    const std::size_t *dims_;
    T *data_;
    const std::size_t offset_;
};


template <typename T, std::size_t DIM_COUNT>
struct md_vector_data {
    md_vector_data() { dimensions_.fill(0); }
    md_vector_data(const md_vector_data &o) = default;
    md_vector_data(md_vector_data &&o) : dimensions_(o.dimensions_), data_(std::move(o.data_)) {}

protected:
    std::array<std::size_t, DIM_COUNT> dimensions_;
    std::vector<T> data_;
};

template <typename T, std::size_t DIM_COUNT>
struct md_vector_base<T, DIM_COUNT, md_vector_order::row_major> : public md_vector_data<T, DIM_COUNT> {
    typename access_helper_row_major<T, DIM_COUNT, DIM_COUNT - 1>::result_type operator[](std::size_t idx) {
        return access_helper_row_major<T, DIM_COUNT, DIM_COUNT - 1>(this->dimensions_.data(), this->data_.data(), 0)[idx];
    }
    typename access_helper_row_major<const T, DIM_COUNT, DIM_COUNT - 1>::result_type operator[](std::size_t idx) const {
        return access_helper_row_major<const T, DIM_COUNT, DIM_COUNT - 1>(this->dimensions_.data(), this->data_.data(), 0)[idx];
    }

protected:
    void resize_impl(const std::array<std::size_t, DIM_COUNT> &sizes) {
        if (std::equal(this->dimensions_.begin(), this->dimensions_.end(), sizes.begin())) {
            return;
        }

        std::vector<T> new_data;
        std::size_t total_size = 1;
        for (auto s : sizes) total_size *= s;
        new_data.resize(total_size);
        if (!this->data_.empty()) {
            map_helper_row_major<DIM_COUNT, DIM_COUNT - 1>::map(
                        this->dimensions_.data(),
                        access_helper_row_major<T, DIM_COUNT, DIM_COUNT - 1>(this->dimensions_.data(), this->data_.data(), 0),
                        sizes.data(),
                        access_helper_row_major<T, DIM_COUNT, DIM_COUNT - 1>(sizes.data(), new_data.data(), 0),
                        [](T &a, T &b) {
                b = std::move(a);
            });
        }
        this->dimensions_ = sizes;
        this->data_ = std::move(new_data);
    }
};

template <typename T, std::size_t DIM_COUNT>
struct md_vector_base<T, DIM_COUNT, md_vector_order::column_major> : public md_vector_data<T, DIM_COUNT> {
    typename access_helper_column_major<T, DIM_COUNT, DIM_COUNT - 1>::result_type operator[](std::size_t idx) {
        return access_helper_column_major<T, DIM_COUNT, DIM_COUNT - 1>(this->dimensions_.data(), this->data_.data(), 0)[idx];
    }
    typename access_helper_column_major<const T, DIM_COUNT, DIM_COUNT - 1>::result_type operator[](std::size_t idx) const {
        return access_helper_column_major<const T, DIM_COUNT, DIM_COUNT - 1>(this->dimensions_.data(), this->data_.data(), 0)[idx];
    }


protected:
    void resize_impl(const std::array<std::size_t, DIM_COUNT> &sizes) {
        if (std::equal(this->dimensions_.begin(), this->dimensions_.end(), sizes.begin())) {
            return;
        }

        std::vector<T> new_data;
        std::size_t total_size = 1;
        for (auto s : sizes) total_size *= s;
        new_data.resize(total_size);
        if (!this->data_.empty()) {
            map_helper_column_major<DIM_COUNT, DIM_COUNT - 1>::map(
                        this->dimensions_.data(),
                        access_helper_column_major<T, DIM_COUNT, DIM_COUNT - 1>(this->dimensions_.data(), this->data_.data(), 0),
                        sizes.data(),
                        access_helper_column_major<T, DIM_COUNT, DIM_COUNT - 1>(sizes.data(), new_data.data(), 0),
                        [](T &a, T &b) {
                b = std::move(a);
            });
        }

        this->dimensions_ = sizes;
        this->data_ = std::move(new_data);
    }
};
}

template <typename T, std::size_t DIM_COUNT, md_vector_order ORDER = md_vector_order::row_major>
class md_vector : public md_vector_base<T, DIM_COUNT, ORDER> {
public:
    static const std::size_t dimension_count = DIM_COUNT;
    static const md_vector_order data_order = ORDER;

    typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;
    typedef typename std::size_t size_type;
    typedef typename std::vector<T>::reverse_iterator reverse_iterator;
    typedef typename std::vector<T>::const_reverse_iterator const_reverse_iterator;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;
    typedef typename std::vector<T>::difference_type difference_type;

    std::size_t size() const { return this->data_.size(); }
    std::size_t size(std::size_t dim_idx) const { return this->dimensions_[dim_idx]; }

    void fill(const T &v) {
        for (T &d : this->data_) {
            d = v;
        }
    }

    void clear() {
        this->data_.clear();
        this->dimensions_.fill(0);
    }

    bool empty() const { return this->data_.empty(); }

    using md_vector_base<T, DIM_COUNT, ORDER>::operator [];

    iterator begin() { return this->data_.begin(); }
    const_iterator begin() const { return this->data_.begin(); }
    const_iterator cbegin() const { return this->data_.cbegin(); }
    iterator end() { return this->data_.end(); }
    const_iterator end() const { return this->data_.end(); }
    const_iterator cend() const { return this->data_.cend(); }
    reverse_iterator rbegin() { return this->data_.rbegin(); }
    const_reverse_iterator rbegin() const { return this->data_.rbegin(); }
    const_reverse_iterator crbegin() const { return this->data_.crbegin(); }
    reverse_iterator rend() { return this->data_.rend(); }
    const_reverse_iterator rend() const { return this->data_.rend(); }
    const_reverse_iterator crend() const { return this->data_.crend(); }

    T *data() { return this->data_.data(); }
    const T *data() const { return this->data_.data(); }

    void resize(const std::array<std::size_t, DIM_COUNT> &sizes) {
        this->resize_impl(sizes);
    }

    void resize_fill(const std::array<std::size_t, DIM_COUNT> &sizes, const T &v) {
        this->dimensions_ = sizes;
        std::size_t total_size = 1;
        for (auto s : sizes) total_size *= s;
        this->data_.resize(total_size);
        std::fill(this->data_.begin(), this->data_.end(), v);
    }

    template <typename... PARAMS>
    void resize_fill_emplace(const std::array<std::size_t, DIM_COUNT> &sizes, PARAMS... params) {
        this->data_.clear();
        std::size_t total_size = 1;
        for (auto s : sizes) total_size *= s;
        this->data_.reserve(total_size);
        for (std::size_t i = 0; i < total_size; ++i) {
            this->data_.emplace_back(params...);
        }
    }
};

#endif // MD_VECTOR_H
