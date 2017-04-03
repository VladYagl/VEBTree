#include <array>
#include <iostream>
#include <string>
#include <vector>

#include "veb.h"

template <unsigned int S>
class VEBTree : AbstractVEBTree<S> {
   private:
    const static int_value half = (1ULL << ((S + 1) / 2)) - 1;

    std::array<VEBTree<S / 2>*, 1ULL << (S / 2)> children;
    VEBTree<S / 2>* aux;

    int_value min, max;

    static int_value high(int_value x) {
        return (x & (~half)) >> ((S + 1) / 2);
    }

    static int_value low(int_value x) {
        return x & half;
    }

    static int_value merge(int_value high, int_value low) {
        return low | high;
    }

   public:
    VEBTree() {
        min = NO;
        max = NO;
        if (S > 1) {
            aux = new VEBTree<S / 2>();
            for (auto& child : children) {
                child = new VEBTree<S / 2>();  // TODO: maybe some lazy shit???
            }
        }
    }

    bool empty() const {
        return min == NO;
    }

    void add(int_value x) {
        std::cout << "   add start"<<x<<"\n";
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
            std::cout << "   add recursive, current S = " << S << '\n';
            if (x < min) std::swap(min, x);
            if (max < x) std::swap(max, x);

            if (S > 2) {
                std::cout << "this is if " << half << ' ' << x << ' ' << high(x) << ' ' << low(x) << "\n";
                if (children[high(x)]->empty()) aux->add(high(x));
                children[high(x)]->add(low(x));
                std::cout << "this is end if \n";
            }
        }
        std::cout << "  add end\n";
    }

    void remove(int_value x) {
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

    int_value next(int_value x) const {
        if (empty()) return NO;
        if (x < min) return min;
        if (aux->empty()) return max;

        if (!children[high(x)]->empty() && children[high(x)]->getMax() > low(x)) return merge(high(x), children[high(x)]->next(low(x)));

        int_value next_high = aux->next(high(x));
        if (next_high == NO) {
            return NO;
        } else {
            return merge(next_high, children[next_high]->getMax());
        }
    }

    int_value prev(int_value x) const {
        if (empty()) return NO;
        if (max < x) return max;
        if (aux->empty()) return min;

        if (!children[high(x)]->empty() && children[high(x)]->getMin() < low(x)) return merge(high(x), children[high(x)]->prev(low(x)));

        int_value prev_high = aux->prev(high(x));
        if (prev_high == NO) {
            return NO;
        } else {
            return merge(prev_high, children[prev_high]->getMin());
        }
    }

    int_value getMin() const {
        return min;
    }

    int_value getMax() const {
        return max;
    }
};

int main() {
    VEBTree<10> tree;

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

    VEBTree<10> a;
    for (int i = 0; i < 50; i += 10) {
        std::cout << "add " << i << std::endl;
        a.add(i);
        std::cout << "prev " << a.prev(i) << std::endl;
    }

    return 0;
}
