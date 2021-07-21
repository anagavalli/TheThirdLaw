#ifndef __STACK_H__
#define __STACK_H__

// Array based stack implementation https://www.cs.bu.edu/teaching/c/stack/array/
// if using template class need to put explictly in h file so compiler knows how to create actual class
template <typename T>
class Stack {
private:
	// can assign new, but deletes automatically
	//std::shared_ptr<T[]> _data;
	std::vector<T> _data;
	int _top;
	int _maxSize;



public:
	Stack(int size) {
		//_data = std::shared_ptr<T[]>(new T[size]());
		_data.resize(size);
		_maxSize = size;
		_top = -1;
	}

	bool isEmpty() {
		return (_top < 0);
	}

	bool isFull() {
		return (_top >= _maxSize - 1);
	}

	bool push(T entry) {
		if (isFull()) {
		return false;
		}

		_data[++_top] = entry;

		return true;
	}

	bool pop(T *entry) {
		if (isEmpty()) {
			return false;
		}

		*entry = _data[_top--];

		return true;
	}


	void dispose() {
		//_data = nullptr;
		_data.clear();
		_data.shrink_to_fit();
	}

	~Stack(void) { dispose(); }

	/*bool isFull();
	bool isEmpty();
	void dispose();
	Stack(int size);
	~Stack(void) { dispose(); }
	bool push(int entry);
	bool pop(int *entry);*/
};

#endif

// testing code
/*	std::shared_ptr<Stack<int>> stack = std::shared_ptr<Stack<int>>(new Stack<int>(4));
	assert(stack->push(65));
	assert(stack->push(98));
	int a, b, c, d;
	assert(stack->pop(&a));
	assert(stack->pop(&b));
	CULog("%d %d\n", a, b);

	assert(!stack->pop(&a));

	assert(stack->push(1));
	assert(stack->push(2));
	assert(stack->push(3));
	assert(stack->push(4));
	assert(!stack->push(5));

	assert(stack->pop(&a));
	assert(stack->pop(&b));
	assert(stack->pop(&c));
	assert(stack->pop(&d));
	CULog("%d %d %d %d\n", a, b, c, d);

	assert(!stack->pop(&a));
	assert(stack->push(10));*/