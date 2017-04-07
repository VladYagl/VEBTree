#include <array>
#include <iostream>
#include <string>
#include <vector>

#include "veb.h"

struct Exception {
    bool aux = false;
    Exception* child = nullptr;
    std::string name = "Exception";
    int_value x;
    unsigned int S;

    Exception(std::string name, int_value x, unsigned int S) : name(name), x(x), S(S) {
    }

    ~Exception() {
        delete child;
    }

    void print() {
        std::cout << std::endl;
        std::cout << name << std::endl << "   ";
        print_info();
        std::cout << std::endl;
    }

   private:
    int_value high(int_value x) {
        return (x) >> (S / 2);
    }

    int_value low(int_value x) {
        return x & ((1ULL << (S / 2)) - 1);
    }

    void print_info() {
        std::cout << "  (x = " << x << " S = " << S << " high = " << high(x) << " low = " << low(x) << " size = " << (1ULL << ((S + 1) / 2)) << ") " << child
                  << std::endl;
        if (child != nullptr) {
            if (aux) {
                std::cout << "aux";
            } else {
                std::cout << "   ";
            }
            child->print_info();
        }
    }
};

template <unsigned int S>
class VEBTree : AbstractVEBTree<S> {
   private:
    bool initialized = false;

    std::array<VEBTree<S / 2>*, 1ULL << ((S + 1) / 2)> children;
    VEBTree<(S + 1) / 2>* aux;

    int_value min, max;

    // DEBUG
   public:
    // DEBUG

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
            for (auto& child : children) {
                child = new VEBTree<S / 2>();  // TODO: maybe some lazy shit???
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
        /* std::cout << std::endl; */
        /* print(); */
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
                // DEBUG
                if (high(x) >= children.size()) {
                    throw new Exception("Add error", x, S);
                }
                // DEBUG

                if (children[high(x)]->empty()) {
                    // DEBUG
                    try {
                        aux->add(high(x));
                    } catch (Exception* e) {
                        Exception* tmp = new Exception("Add error", x, S);
                        tmp->child = e;
                        tmp->aux = true;
                        throw tmp;
                    }
                    // DEBUG
                    /* aux->add(high(x)); */
                }
                // DEBUG
                try {
                    children[high(x)]->add(low(x));
                } catch (Exception* e) {
                    Exception* tmp = new Exception("Add error", x, S);
                    tmp->child = e;
                    throw tmp;
                }
                // DEBUG
                /* children[high(x)]->add(low(x)); */
            }
        }
    }

    void remove(int_value x) override {
        /* std::cout << std::endl; */
        /* print(); */
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
            children[high(x)]->remove(low(x));
            if (children[high(x)]->empty()) {
                aux->remove(high(x));
            }
        }
    }

    int_value next(int_value x) const override {
        /* std::cout << "get_next : S = " << S << " x = " << x << std::endl; */
        int_value result = next_impl(x);
        /* std::cout << "result = " << result << std::endl; */
        return result;
    }

    int_value next_impl(int_value x) const {
        if (empty()) return NO;
        if (x < min) return min;
        if (S == 1 || aux->empty()) return max;

        // DEBUG
        if (high(x) >= children.size()) {
            throw new Exception("Next error", x, S);
        }
        // DEBUG

        if (!children[high(x)]->empty() && children[high(x)]->getMax() > low(x)) {
            // DEBUG
            try {
                return merge(high(x), children[high(x)]->next(low(x)));
            } catch (Exception* e) {
                Exception* tmp = new Exception("Next error", x, S);
                tmp->child = e;
                throw tmp;
            }
            // DEBUG
            /* return merge(high(x), children[high(x)]->next(low(x))); */
        }

        // DEBUG
        /* aux->print(1, "   aux   "); */
        int_value next_high = NO;
        try {
            next_high = aux->next(high(x));
        } catch (Exception* e) {
            Exception* tmp = new Exception("Next error", x, S);
            tmp->child = e;
            tmp->aux = true;
            throw tmp;
        }
        /* std::cout <<  "   next_high = " << next_high << " x = " << x << " high(x) = " << high(x) << std::endl; */
        // DEBUG
        /* int_value next_high = aux->next(high(x)); */
        if (next_high == NO) {
            return NO;
        } else if (next_high <= high(x)) {
            return max;
        } else {
            try {
                return merge(next_high, children[next_high]->getMin());
            } catch (Exception* e) {
                Exception* tmp = new Exception("Next error", x, S);
                tmp->child = e;
                throw tmp;
            }
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

    // DEBUG
    void print(int_value h, std::string tab) {
        std::cout << tab << "(" << h << ")S = " << S << " min = " << h + min << " max = " << h + max << std::endl;
        if (S > 1) {
            for (int i = 0; i < children.size(); i++) {
                if (!children[i]->empty()) {
                    children[i]->print(h + (i << (S / 2)), tab + "  ");
                }
            }
        }
    }

    void print() {
        print(0, "");
    }
    // DEBUG
};
