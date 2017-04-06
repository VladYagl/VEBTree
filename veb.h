#ifndef VEB_H
#define VEB_H

typedef unsigned long long int_value;

int_value const NO = -1;

template<unsigned int S>
class AbstractVEBTree {
    public:
	virtual ~AbstractVEBTree() {}

	virtual void add(int_value x) = 0;
        virtual void remove(int_value x) = 0;
        virtual int_value next(int_value x) const = 0;
        virtual int_value prev(int_value x) const = 0;
        virtual int_value getMin() const = 0;
        virtual int_value getMax() const = 0;
};

template<unsigned int S>
class VEBTree;

#endif
