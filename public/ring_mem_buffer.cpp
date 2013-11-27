/*
 * ring_mem_buffer.cpp
 *
 *  Created on: 2013-6-26
 *      Author: dukefu
 */

#include "ring_mem_buffer.h"
#include <string.h>

namespace kingnet {
namespace common {

ring_mem_buffer::ring_mem_buffer() {
	// TODO Auto-generated constructor stub
	buff_ = 0;
}

ring_mem_buffer::~ring_mem_buffer() {
	// TODO Auto-generated destructor stub
	if (buff_) {
		delete buff_;
		buff_ = 0;
	}
}
void ring_mem_buffer::init_memory(unsigned int buffer_size)
{
	size_ = buffer_size;
	r_pos_.store(0,  memory_order_relaxed);
	w_pos_.store(0,  memory_order_relaxed);
	if (size_ > 0) {
		buff_ = new char [size_];
		memset(buff_, 0, size_);
	}else{
		buff_ = 0;
	}
}
int ring_mem_buffer::push_data(char* buf, unsigned int size)
{
	unsigned int t_pos = w_pos_.load( memory_order_acquire);


	if (size_ >= t_pos+size) {
		memcpy(buff_+t_pos, buf, size);
		t_pos += size;
	}else{
		unsigned int tail_size = size_ - t_pos;
		unsigned int over_size = size - tail_size;
		//if exceeds the read pointer, forbiden to write
		unsigned int t_r_pos = r_pos_.load(memory_order_acquire);
		if (over_size >= t_r_pos) {
			return -1;
		}


		if (tail_size > 0) {
			memcpy(buff_+t_pos, buf, tail_size);
		}
		if (over_size > 0) {
			memcpy(buff_, buf+tail_size, over_size);
		}
		t_pos = over_size;
	}

	w_pos_.store(t_pos, memory_order_release);

	return 0;
}

int ring_mem_buffer::consume_all_data()
{
	int ret = 0;
	try{
		unsigned int r_pos = r_pos_.load(memory_order_acquire);
		unsigned int w_pos = w_pos_.load( memory_order_acquire);
		if (r_pos < w_pos) {
			ret = func_(buff_+r_pos, w_pos-r_pos);
		}else if (r_pos > w_pos) {
			ret = func_(buff_+r_pos, size_ - r_pos);
			if (ret == 0 && w_pos != 0) {
				ret = func_(buff_, w_pos);
			}
		}
		r_pos_.store(w_pos, memory_order_release);
	}catch(...){

	}
	return ret;
}

void ring_mem_buffer::set_consume_func(function<int (char*, unsigned int)> func)
{
	func_ = func;
}



} /* namespace common */
} /* namespace kingnet */
