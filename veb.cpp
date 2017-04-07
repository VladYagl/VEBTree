#include <array>

#include "veb.h"

template <unsigned int S>
class VEBTree : AbstractVEBTree<S> {
   private:
    std::array<VEBTree<S / 2>*, 1ULL << ((S + 1) / 2)> children;
    VEBTree<(S + 1) / 2>* aux;

    int_value min, max;

    static int_value high(int_value x) {
        return (x) >> (S / 2);
    }

    static int_value low(int_value x) {
        return x & ((1ULL << (S / 2)) - 1);
    }

    static int_value merge(int_value high, int_value low) {
        return low | (high << (S / 2));
    }

   public:
    VEBTree() : min(NO), max(NO) {
        max = NO;
        if (S > 1) {
            aux = new VEBTree<(S + 1) / 2>();
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
            if (x == max || x == min) return;
            if (x < min) std::swap(min, x);
            if (max < x) std::swap(max, x);

            if (S > 1) {
                if (children[high(x)] == nullptr) {
                    children[high(x)] = new VEBTree<S / 2>();
                }

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
            if (min == x) min = NO;
        } else if (min == x) {
            if (S == 1 || aux->empty()) {
                min = max;
                return;
            }
            min = merge(aux->getMin(), children[aux->getMin()]->getMin());
            children[aux->getMin()]->remove(children[aux->getMin()]->getMin());
            if (children[aux->getMin()]->empty()) {
                aux->remove(aux->getMin());
            }
        } else if (max == x) {
            if (S == 1 || aux->empty()) {
                max = min;
                return;
            }
            max = merge(aux->getMax(), children[aux->getMax()]->getMax());
            children[aux->getMax()]->remove(children[aux->getMax()]->getMax());
            if (children[aux->getMax()]->empty()) {
                aux->remove(aux->getMax());
            }
        } else if (S == 1 || aux->empty()) {
            return;
        } else {
            if (children[high(x)] != nullptr) {
                children[high(x)]->remove(low(x));
                if (children[high(x)]->empty()) {
                    aux->remove(high(x));
                }
            }
        }
    }

    int_value next(int_value x) const override {
        if (empty()) return NO;
        if (x < min) return min;
        if (S == 1 || aux->empty()) return max;

        if (children[high(x)] != nullptr && !children[high(x)]->empty() && children[high(x)]->getMax() > low(x)) {
            return merge(high(x), children[high(x)]->next(low(x)));
        }

        int_value next_high = aux->next(high(x));
        if (next_high == NO) {
            return NO;
        } else if (next_high <= high(x)) {
            return max;
        } else {
            return merge(next_high, children[next_high]->getMin());
        }
    }

    int_value prev(int_value x) const override {
        if (empty()) return NO;
        if (max < x) return max;
        if (S == 1 || aux->empty()) return min;

        if (children[high(x)] != nullptr && !children[high(x)]->empty() && children[high(x)]->getMin() < low(x)) {
            return merge(high(x), children[high(x)]->prev(low(x)));
        }

        int_value prev_high = aux->prev(high(x));
        if (prev_high == NO) {
            return NO;
        } else if (prev_high >= high(x)) {
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
