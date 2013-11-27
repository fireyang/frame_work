/*
 * ring_mem_buffer.h
 *
 *  Created on: 2013-6-26
 *      Author: dukefu
 */

#ifndef RING_MEM_BUFFER_H_
#define RING_MEM_BUFFER_H_

#include <atomic>
#include <functional>
using namespace std;

namespace kingnet {
namespace common {

class ring_mem_buffer {
public:
	ring_mem_buffer();
	virtual ~ring_mem_buffer();

	void init_memory(unsigned int buffer_size);
	int push_data(char* buf, unsigned int size);

	int consume_all_data();

	void set_consume_func(function<int (char*, unsigned int)> func);
private:
	unsigned int size_;
	atomic<unsigned int> r_pos_;
	atomic<unsigned int> w_pos_;
	char* buff_;
	function<int (char*, unsigned int)> func_;
};

} /* namespace common */
} /* namespace kingnet */
#endif /* RING_MEM_BUFFER_H_ */
