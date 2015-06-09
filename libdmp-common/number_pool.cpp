#include "number_pool.hpp"

#include <limits>
#include <stdexcept>

NumberPool::NumberPool(int lower, int upper)
{
	free.insert(NumberInterval(lower, upper));
}

int NumberPool::allocate_number()
{
	NumberInterval first = *(free.begin());
	int free_id = first.left();
	free.erase(free.begin());
	if (first.left() < std::numeric_limits<int>::max()) {
		if (first.left() + 1 <= first.right()) {
			free.insert(NumberInterval(first.left() + 1 , first.right()));
		}
	} else {
		std::runtime_error("Left boundry would overflow, this should not happen");
	}
	return free_id;
}

void NumberPool::free_number(int id)
{
	NumberIntervals_t::iterator it = free.find(NumberInterval(id,id));
	if (it != free.end()  && it->left() <= id && it->right() > id) {
		return ;
	}
	it = free.upper_bound(NumberInterval(id,id));
	if (it == free.end()) {
		return ;
	} else {
		NumberInterval free_interval = *(it);

		if (id + 1 != free_interval.left()) {
			free.insert(NumberInterval(id, id));
		} else {
			if (it != free.begin()) {
				NumberIntervals_t::iterator it_2 = it;
				--it_2;
				if (it_2->right() + 1 == id ) {
					NumberInterval free_interval_2 = *(it_2);
					free.erase(it);
					free.erase(it_2);
					free.insert(
						NumberInterval(free_interval_2.left(), free_interval.right()));
				} else {
					free.erase(it);
					free.insert(NumberInterval(id, free_interval.right()));
				}
			} else {
					free.erase(it);
					free.insert(NumberInterval(id, free_interval.right()));
			}
		}
	}
}
