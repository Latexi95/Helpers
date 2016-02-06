#pragma once

#include <memory>
#include <mutex>
#include <condition_variable>
#include <utility>

namespace channel_private {

template <typename T, std::size_t S>
struct shared_data
{
	shared_data() : read_index(0), data_size(0) {}
	T data[S];
	std::size_t read_index;
	std::size_t data_size;
	std::mutex mutex;
	std::condition_variable cv;
};

template <typename T>
struct shared_data<T, 1>
{
	shared_data() : has_data(false) {}
	T data;
	bool has_data;
	std::mutex mutex;
	std::condition_variable cv;
};

}

template <typename T, std::size_t S = 1>
class channel
{
	template<typename T2, std::size_t BUFF_SIZE>
	friend channel<T2, BUFF_SIZE> make_channel();
public:
	channel() = default;
	~channel() = default;
	channel(const channel<T, S> &o) = default;
	channel(channel<T, S> &&o) = default;

	channel &operator=(const channel<T, S> &o) = default;

	void create();
	void send(const T &v);
	void send(T &&v);

	T get();

	channel<T, S> &operator<<(const T &v);
	channel<T, S> &operator>>(T &v);

private:
	channel(const std::shared_ptr<channel_private::shared_data<T, S>> &d) : data(d) {}
	std::shared_ptr<channel_private::shared_data<T, S>> data;
};

template <typename T>
class channel<T, 1>
{
	template<typename T2, std::size_t BUFF_SIZE>
	friend channel<T2, BUFF_SIZE> make_channel();
public:
	channel() = default;
	~channel() = default;
	channel(const channel<T, 1> &o) = default;
	channel(channel<T, 1> &&o) = default;

	channel &operator=(const channel<T, 1> &o) = default;

	void create();
	void send(const T &v);
	void send(T &&v);

	T get();

	channel<T, 1> &operator<<(const T &v);
	channel<T, 1> &operator>>(T &v);

private:
	channel(const std::shared_ptr<channel_private::shared_data<T, 1>> &d) : data(d) {}
	std::shared_ptr<channel_private::shared_data<T, 1>> data;
};

template <typename T, std::size_t S>
void channel<T, S>::create() {
	data = std::make_shared<channel_private::shared_data<T, S>>();
}

template <typename T, std::size_t S>
void channel<T, S>::send(T &&v) {
	std::unique_lock<std::mutex> lk(data->mutex);
	if (data->data_size == S) {
		data->cv.wait(lk, [this]() {
			return data->data_size < S;
		});
	}

	std::size_t write_index = data->read_index + data->data_size;
	if (write_index >= S) write_index -= S;
	data->data[write_index] = std::move(v);
	++data->data_size;
	lk.unlock();
	data->cv.notify_all();
}

template <typename T, std::size_t S>
void channel<T, S>::send(const T &v) {
	std::unique_lock<std::mutex> lk(data->mutex);
	if (data->data_size == S) {
		data->cv.wait(lk, [this]() {
			return data->data_size < S;
		});
	}

	std::size_t write_index = data->read_index + data->data_size;
	if (write_index >= S) write_index -= S;
	data->data[write_index] = v;
	++data->data_size;
	lk.unlock();
	data->cv.notify_all();
}

template <typename T, std::size_t S>
T channel<T, S>::get() {
	std::unique_lock<std::mutex> lk(data->mutex);
	if (data->data_size  == 0) {
		data->cv.wait(lk, [this]() {
			return data->data_size > 0;
		});
	}
	--data->data_size;
	T v = std::move(data->data[data->read_index]);
	++data->read_index;
	if (data->read_index == S) data->read_index = 0;
	lk.unlock();
	data->cv.notify_all();
	return v;
}

template <typename T, std::size_t S>
channel<T, S> &channel<T, S>::operator<<(const T &v) {
	this->send(v);
	return *this;
}

template <typename T, std::size_t S>
channel<T, S> &channel<T, S>::operator>>(T &v) {
	v = std::move(this->get());
	return *this;
}


template <typename T>
void channel<T, 1>::send(T &&v) {
	std::unique_lock<std::mutex> lk(data->mutex);
	if (data->has_data) {
		data->cv.wait(lk, [this]() {
			return !data->has_data;
		});
	}
	data->has_data = true;
	data->data = std::move(v);
	lk.unlock();
	data->cv.notify_all();
}

template <typename T>
void channel<T, 1>::send(const T &v) {
	std::unique_lock<std::mutex> lk(data->mutex);
	if (data->has_data) {
		data->cv.wait(lk, [this]() {
			return !data->has_data;
		});
	}
	data->has_data = true;
	data->data = v;
	lk.unlock();
	data->cv.notify_all();
}

template <typename T>
T channel<T, 1>::get() {
	std::unique_lock<std::mutex> lk(data->mutex);
	if (!data->has_data) {
		data->cv.wait(lk, [this]() {
			return data->has_data;
		});
	}
	data->has_data = false;
	T v = std::move(data->data);
	lk.unlock();
	data->cv.notify_all();
	return v;
}

template <typename T>
channel<T, 1> &channel<T, 1>::operator<<(const T &v) {
	this->send(v);
	return *this;
}

template <typename T>
channel<T, 1> &channel<T, 1>::operator>>(T &v) {
	v = std::move(this->get());
	return *this;
}




template <typename T, std::size_t S = 1>
channel<T, S> make_channel()
{
	auto data = std::make_shared<channel_private::shared_data<T, S>>();
	return channel<T, S>(data);
}



