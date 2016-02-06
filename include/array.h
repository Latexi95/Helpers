#pragma once
#include <initializer_list>
#include <cassert>
#include <algorithm>

namespace ArrayInternal {
	template <std::size_t FD, std::size_t... D>
	struct Multiplicate {
			static const std::size_t Result = FD * Multiplicate<D...>::Result;
	};
	template<std::size_t FD>
	struct Multiplicate<FD> {
			static const std::size_t Result = FD;
	};

	template<typename T, std::size_t FD, std::size_t... D>
	struct SubscriptHelper {
			static const std::size_t Multiplicator = Multiplicate<D...>::Result;
			typedef SubscriptHelper<T, D...> Result;
			SubscriptHelper(T *d) : data(d){
			}
			SubscriptHelper<T, D...> operator[](std::size_t i) {
				assert(i < FD);
				return SubscriptHelper<T, D...>(data + i * Multiplicator);
			}
		private:
			T *data;
	};
	template<typename T, std::size_t FD>
	struct SubscriptHelper<T, FD> {
			typedef T& Result;
			SubscriptHelper(T *d) : data(d){
			}
			T &operator[](std::size_t i) {
				assert(i < FD);
				return data[i];
			}
		private:
			T *data;
	};


	template <typename T, std::size_t FD, std::size_t... D>
	struct InitializerHelper {
			typedef std::initializer_list<typename InitializerHelper<T, D...>::Type> Type;
			static void initialize(T *insertIt, const Type &initializer) {
				assert(initializer.size() <= FD);
				std::size_t i = 0;
				for (auto & e : initializer) {
					InitializerHelper<T, D...>::initialize(insertIt + i * Multiplicate<D...>::Result, e);
					++i;
				}
			}
	};

	template <typename T, std::size_t FD>
	struct InitializerHelper<T, FD> {
			typedef std::initializer_list<T> Type;
			static void initialize(T *insertIt, const std::initializer_list<T> &initializer) {
				assert(initializer.size() <= FD);
				for (const T &e : initializer) {
					*insertIt++ = e;
				}
			}
	};
}


template <typename T, std::size_t FD, std::size_t... D>
class Array {
	public:
		typedef T value_type;
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef T *iterator;
		typedef const T *const_iterator;

		Array() {}
		Array(const Array &array) { std::copy(array.begin(), array.end(), begin()); }
		Array(const typename ArrayInternal::InitializerHelper<T, FD, D...>::Type &init) {
			ArrayInternal::InitializerHelper<T, FD, D...>::initialize(dataArray, init);
		}

		static const std::size_t ElementCount = ArrayInternal::Multiplicate<FD, D...>::Result;
		typename ArrayInternal::SubscriptHelper<T, FD, D...>::Result operator [](std::size_t i) { return ArrayInternal::SubscriptHelper<T, FD, D...>(dataArray)[i];}
		typename ArrayInternal::SubscriptHelper<const T, FD, D...>::Result operator [](std::size_t i) const { return ArrayInternal::SubscriptHelper<const T, FD, D...>(dataArray)[i];}

		Array &operator=(const Array &array) { std::copy(array.begin(), array.end(), begin()); }
		bool operator==(const Array &array) const { return std::equal(array.begin(), array.end(), begin()); }
		bool operator!=(const Array &array) const { return !std::equal(array.begin(), array.end(), begin()); }

		iterator begin() { return dataArray; }
		iterator end() { return dataArray + ElementCount; }
		const_iterator begin() const { return cbegin(); }
		const_iterator end() const { return cend(); }
		const_iterator cbegin() const { return dataArray; }
		const_iterator cend() const { return dataArray + ElementCount; }

		T *data() { return dataArray; }
		const T *data() const { return dataArray; }

		bool empty() const { return FD == 0; }

		T &front() { return dataArray[0]; }
		const T &front() const { return dataArray[0]; }
		T &back() { return dataArray[ElementCount - 1]; }
		const T &back() const { return dataArray[ElementCount - 1]; }

		void fill(const T &v) { for (T &e : *this) e = v;}


	private:
		T dataArray[ElementCount];
};


