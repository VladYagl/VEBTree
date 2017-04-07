#include <iostream>
#include <set>

#include "veb.cpp"

using namespace std;

const int TREE_SIZE = 20;
const int ITERATIONS = 100000;

int main() {
    srand(15);
    for (int i = 0; i < 100; i++) {
        std::cout << "\n\n\n";
    }
    VEBTree<TREE_SIZE> *a = new VEBTree<TREE_SIZE>();
    std::set<int_value> s;

    for (int i = 0; i < ITERATIONS; i++) {
        int_value x = rand() % (1 << TREE_SIZE);
        /* cout << endl << "******begin i = " << i << " x = " << x; */
        try {
            if (rand() % 50 == 0) {
                /* cout << " remove"; */
                a->remove(x);
                s.erase(x);
                /* cout << "+" << endl; */
            } else {
                /* cout << " add"; */
                a->add(x);
                s.insert(x);
                /* cout << "+" << endl; */
            }
            auto temp = s.lower_bound(x);
            auto curr = temp;
            auto temp2 = temp;
            auto next = temp;
            if (next != s.end() && *next == x) ++next;
            auto prev = temp2;
            if (curr != s.begin()) --prev;

            if (curr != s.begin() && a->prev(x) != *prev) {
                std::cout << "prev mistake i = " << i << " x = " << x << ' ' << a->prev(x) << "!=" << *prev << std::endl;
                for (auto i : s) {
                    std::cout << i << ' ';
                }
                std::cout << std::endl << std::endl;
                a->print();
                std::cout << std::endl << std::endl;
                return 0;
            }

            if (next != s.end() && a->next(x) != *next) {
                std::cout << "next mistake i = " << i << " x = " << x << ' ' << a->next(x) << "!=" << *next << std::endl;
                for (auto i : s) {
                    std::cout << i << ' ';
                }
                std::cout << std::endl << std::endl;
                a->print();
                std::cout << std::endl << std::endl;
                return 0;
            }

            /* cout << "******end i = " << i << endl; */
        } catch (Exception* e) {
            e->print();
            return 0;
        }
    }

    std::cout << "OK\n";

    return 0;
}
