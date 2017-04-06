#include <array>
#include <iostream>
#include <string>
#include <vector>

#include "veb.h"

template <unsigned int S>
class VEBTree : AbstractVEBTree<S> {
private:
	int ss = S;
	const static int_value half = (1ULL << (S / 2)) - 1;
	const static unsigned int HALF = (S + 1) / 2;

	std::array<VEBTree<S / 2 + 1>*, 1ULL << HALF> children;
	VEBTree<S / 2 + 1>* aux;

	int_value min, max;

	static int_value high(int_value x) {
		return (x) >> (S / 2);
	}

	static int_value low(int_value x) {
		return x & half;
	}

	static int_value merge(int_value high, int_value low) {
		return low | (high << (S / 2));
	}

public:
	VEBTree() {
		min = NO;
		max = NO;
		if (S > 1) {
			aux = new VEBTree<S / 2 + 1>();
			for (auto& child : children) {
				child = new VEBTree<S / 2 + 1>();  // TODO: maybe some lazy shit???
			}
		}
	}

	virtual ~VEBTree() {
		for (auto& i : children) {
			delete i;
		}
		delete aux;
	}

	bool empty() const {
		return min == NO;
	}

	void add(int_value x) override {
		if (empty()) {
			min = x;
			max = x;
		} else if (max == min) {
			if (x < min) {
				min = x;
			} else {
				max = x;
			}
		} else {
			if (x < min) std::swap(min, x);
			if (max < x) std::swap(max, x);

			if (S > 1) {
				if (children[high(x)]->empty()) {
					aux->add(high(x));
				}
				children[high(x)]->add(low(x));
			}
		}
	}

	void remove(int_value x) override {
		if (empty()) return;
		if (min == max) {
			min = NO;
		} else if (min == x) {
			if (aux->empty()) {
				min = max;
				return;
			}
			min = merge(aux->getMin(), children[aux->getMin()]->getMin());
		} else if (max == x) {
			if (aux->empty()) {
				max = min;
				return;
			}
			max = merge(aux->getMax(), children[aux->getMax()]->getMax());
		} else if (aux->empty()) {
			return;
		} else {
			children[high(x)]->remove(low(x));
			if (children[high(x)]->empty()) {
				aux->remove(high(x));
			}
		}
	}

	int_value next(int_value x) const override {
		if (empty()) return NO;
		if (x < min) return min;
		if (S == 1 || aux->empty()) return max;

		if (!children[high(x)]->empty() && children[high(x)]->getMax() > low(x)) {
			return merge(high(x), children[high(x)]->next(low(x)));
		}

		int_value next_high = aux->next(high(x));
		if (next_high == NO) {
			return NO;
		} else if (next_high == high(x)) {
			return max;
		} else {
			return merge(next_high, children[next_high]->getMin());
		}
	}

	int_value prev(int_value x) const override {
		if (empty()) return NO;
		if (max < x) return max;
		if (S == 1 || aux->empty()) return min;

		if (!children[high(x)]->empty() && children[high(x)]->getMin() < low(x)) {
			return merge(high(x), children[high(x)]->prev(low(x)));
		}

		int_value prev_high = aux->prev(high(x));
		if (prev_high == NO) {
			return NO;
		} else if (prev_high == high(x)) {
			return min;
		} else {
			return merge(prev_high, children[prev_high]->getMax());
		}
	}

	int_value getMin() const override {
		return min;
	}

	int_value getMax() const override {
		return max;
	}
};

#include <set>

int main() {
	/*VEBTree<10> tree;

	std::cout << tree.empty() << std::endl;

	tree.add(5);
	tree.add(11);
	tree.add(10);

	std::cout << tree.empty() << std::endl;

	std::cout << tree.next(5) << std::endl;
	std::cout << tree.prev(11) << std::endl;
	tree.remove(10);
	std::cout << tree.next(5) << std::endl;
	std::cout << tree.prev(11) << std::endl;

	std::cout << "start shirv test" << std::endl;
*/
	VEBTree<5> a;
	std::set<int_value> s;

	for (int i = 0; i < 500; i++) {
		int_value x = (rand() * RAND_MAX + rand()) % (1 << 5);
		a.add(x);
		s.insert(x);
		auto temp = s.find(x);
		auto curr = temp;
		auto temp2 = temp;
		auto next = ++temp;
		auto prev = --temp2;
		auto pp = a.prev(x);
		auto nn = a.next(x);
		if (curr != s.begin() && a.prev(x) != *prev) {
			std::cout << x << std::endl;
		}
		if (next != s.end() && a.next(x) != *next) {
			std::cout << x << std::endl;
		}
	}

	std::cout << "OK\n";

	return 0;
}
