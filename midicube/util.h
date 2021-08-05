/*
 * util.h
 *
 *  Created on: 29 Jun 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_UTIL_H_
#define MIDICUBE_UTIL_H_

#include <array>
#include <vector>
#include <mutex>
#include <boost/smart_ptr/detail/spinlock.hpp>
#include <thread>
#include <chrono>
#include <emmintrin.h>


template<class T, std::size_t N> class CircularBuffer {
private:
	std::array<T, N> buffer;
	std::size_t start;
	std::size_t end;

public:
	T at(size_t pos) {
		return buffer[(start + pos) %buffer.size()];
	};

	T peek_first() {
		if (size() > 0) {
			return buffer[start];
		}
		throw std::runtime_error("Buffer is empty!");
	}

	void push_back(T t) {
		buffer[end] = t;
		++end;
		end %= buffer.size();
		if (end == start) {
			start++;
		}
	};

	T pop_first() {
		if (size() > 0) {
			T t = buffer[start++];
			start %= buffer.size();
			return t;
		}
		throw std::runtime_error("Buffer is empty!");
	}

	std::size_t size(){
		return end < start ? (buffer.size() - start + end) : (end - start);
	};

	std::size_t buffer_size() {
		return buffer.size();
	};
};

struct ScaleItem {
	double prog;
	double value;
};

class FixedScale {
private:
	double start = 0;
	std::vector<ScaleItem> items;
	double end = 1;
public:
	FixedScale(double start, std::vector<ScaleItem>items, double end);
	double value(double progress) const;
	double progress(double value) const;
};

template<typename T>
struct BufferEntry {
	unsigned int content_id = 0;
	T* buffer;
	std::mutex lock;
};

template<typename T>
class MultiBuffer {
private:
	BufferEntry<T>* buffer;

public:
	const size_t buffer_amount;
	const size_t size;

	MultiBuffer(size_t s, size_t n) : buffer_amount(n), size(s) {
		buffer = new BufferEntry<T>[buffer_amount];
		for (size_t i = 0; i < buffer_amount; ++i) {
			buffer[i].buffer = new T[size];
		}
	}

	inline BufferEntry<T>& operator [](size_t n) {
		return buffer[n];
	}

	~MultiBuffer() {
		for (size_t i = 0; i < buffer_amount; ++i) {
			delete buffer[i].buffer;
			buffer[i].buffer = nullptr;
		}
	}

};

using namespace std::chrono_literals;

//Based on https://timur.audio/using-locks-in-real-time-audio-processing-safely
class SpinLock {
private:
	std::atomic_flag flag = ATOMIC_FLAG_INIT;

public:

	SpinLock() {

	}

	SpinLock(SpinLock& lock) = delete;


	inline void lock() {
		//Spin
		for (int i = 0; i < 5; ++i) {
			if (try_lock()) {
				return;
			}
		}
		//Spin with pause
		for (int i = 0; i < 10; ++i) {
			if (try_lock()) {
				return;
			}
			_mm_pause();
		}
		//Spin with yield
		while (true) {
			for(int i = 0; i < 3000; ++i) {
				if (try_lock()) {
					return;
				}

				_mm_pause();
				_mm_pause();
				_mm_pause();
				_mm_pause();
				_mm_pause();
				_mm_pause();
				_mm_pause();
				_mm_pause();
				_mm_pause();
				_mm_pause();
			}
			std::this_thread::yield();
		}
	}

	inline bool try_lock() {
		return !flag.test_and_set(std::memory_order_acquire);
	}

	inline void unlock() {
		flag.clear(std::memory_order_acquire);
	}

};

#endif /* MIDICUBE_UTIL_H_ */
