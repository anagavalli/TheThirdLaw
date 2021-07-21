#include "ObjectContainer.h"

ObjectContainer::ObjectContainer(int maxCount) {
	// allocate arrays (things inside parenthesis are normal allocation statements, wrap with shared ptr so they will be freed automatically)
	//_objects = std::shared_ptr<Element[]> (new Element[maxCount]);
	//_slots = std::shared_ptr<int[]>(new int[maxCount]);
	_objects.resize(maxCount); // pre-allocate the maximum amount of space so don't need to dynamically reallocate memory upon expansion
	_slots.resize(maxCount);
	_freeList =  std::shared_ptr<Stack<int>>(new Stack<int>(maxCount));
	_count = 0;
	_maxCount = maxCount;

	// populate free list with every entry (place lower entries on the top)
	for (int i = maxCount-1; i >= 0; i--) {
		_freeList->push(i);
	}

}

/** iterate through actual data container which is contigious (don't go through indirection table/slot array) 
// it's assumed that you don't go out of bounds! so we don't have to check everytime here.*/
std::shared_ptr<PhysicalObject> ObjectContainer::get(int idx){
//int ObjectContainer::get(int idx) {
	//if (idx < _count) { 
	return _objects[idx].data;
	//}
}


// get data from storage (looking for specific object)
std::shared_ptr<PhysicalObject> ObjectContainer::lookup(int idx, int version) {
//int ObjectContainer::lookup(int idx, int version) {

	// resolve indirection
	int objIndex;
	if (idx < _maxCount) { // don't need to check _count (actually messes up), version lets us know if invalid entry
		//CULog("idx %d\n", idx);
		objIndex = _slots[idx];
	}
	else {
		return nullptr;
		//return -1;
	}

	// check to make sure we haven't deleted what we were looking for
	if (_objects[objIndex].version == version) {
		return _objects[objIndex].data;
	}
	else {
		return nullptr;
		//return -1;
	}
}

// store item in storage, replacement is not allowed. must delete then add
bool ObjectContainer::put(std::shared_ptr<PhysicalObject> object, int *idx, int *version) {
//bool ObjectContainer::put(int object, int *idx, int *version) {
	// max sure we are not over capacity
	if (_count >= _maxCount) {
		CULog("over capacity in container\n");
		return false;
	}


	// find open index in slot/indirection table
	int slotIndex;
	if (_freeList->pop(&slotIndex)) {
		//CULog("new entry %d set\n", slotIndex);
		// add new entry to indirection table
		_slots[slotIndex] = _count;

		// append to object array (place in first empty index)
		_objects[_count].indirectIndex = slotIndex;
		_objects[_count].version++;
		_objects[_count].data = object;

		*idx = _objects[_count].indirectIndex;
		*version = _objects[_count].version;

		_count++;

		return true;
	}
	else {
		return false;
	}

}

// remove item from storage
bool ObjectContainer::remove(int idx, int version) {

	// make sure there is something to remove
	if (idx >= _maxCount) {
		return false;
	}


	// resolve indirection
	int objIndex = _slots[idx];

	// check to make sure we haven't deleted what we were looking for
	if (_objects[objIndex].version == version) {
		// check if last element of array (we can't swap when there is only one item)
		if (_count > 1) {
			//CULog("Before removal indirect: %d %d %d %d data: %d %d %d %d version: %d %d %d %d\n", _slots[0],_slots[1],_slots[2],_slots[3],
			//	_objects[0].data,_objects[1].data,_objects[2].data,_objects[3].data, _objects[0].version, _objects[1].version, _objects[2].version, _objects[3].version);

			// swap removed data with valid data at the end of array to keep data packed
			std::swap(_objects[objIndex].data, _objects[_count - 1].data);
			std::swap(_objects[objIndex].version, _objects[_count - 1].version);
			std::swap(_objects[objIndex].indirectIndex, _objects[_count - 1].indirectIndex);

			//CULog("swap %d %d w/ %d %d", _count-1, _objects[_count - 1].indirectIndex, objIndex, _objects[objIndex].indirectIndex);

			// update indirection table b/c valid data at the end of the list moved
			int slotIndex = _objects[objIndex].indirectIndex;
			_slots[slotIndex] = objIndex;
			_slots[idx] = _count - 1; //idx still points to valid data (if we get() after remove())

		}

		// free idx of removed data
		_freeList->push(idx);
		

		// make a note that data at these location were modified (i.e. removed) by updating version
		_objects[_count - 1].version++; //idx still points to valid data (if we get() after remove())
		//_objects[objIndex].version++; // data that is still valid should not be considered changed?

		_count--;

		//CULog("after removal indirect: %d %d %d %d data: %d %d %d %d version: %d %d %d %d\n", _slots[0], _slots[1], _slots[2], _slots[3],
		//	_objects[0].data, _objects[1].data, _objects[2].data, _objects[3].data, _objects[0].version, _objects[1].version, _objects[2].version, _objects[3].version);

		return true;
	}
	else {
		return false;
	}

}

void ObjectContainer::dispose() {
	// make sure to get rid of reserved memory
	_objects.clear();
	_objects.shrink_to_fit();
	_slots.clear();
	_slots.shrink_to_fit();
	//_objects = nullptr;
	//_slots = nullptr;
	_freeList = nullptr;
}

//test with ints
/* std::shared_ptr<ObjectContainer> arr = std::shared_ptr<ObjectContainer>(new ObjectContainer(4));
	int i1, v1, i2, v2, i3, v3, i4, v4, i5, v5;
	// test 1 item
	assert(arr->put(5, &i1, &v1));
	assert(arr->getSize() == 1);
	assert(5 == arr->lookup(i1, v1));
	assert(5 == arr->lookup(i1, v1));
	assert(5 == arr->get(0));
	assert(arr->remove(i1, v1));
	assert(-1 == arr->lookup(i1, v1));
	assert(!arr->remove(i1, v1));
	assert(arr->getSize() == 0);

	//test 4 items
	assert(arr->put(1, &i1, &v1));
	assert(arr->put(2, &i2, &v2));
	assert(arr->put(3, &i3, &v3));
	assert(arr->put(4, &i4, &v4));
	assert(!arr->put(10, &i5, &v5));
	assert(arr->getSize() == 4);
	assert(1 == arr->get(0));
	assert(2 == arr->get(1));
	assert(3 == arr->get(2));
	assert(4 == arr->get(3));

	// test versioning
	assert(1 == arr->lookup(i1, v1));
	assert(-1 == arr->lookup(i1, 10));
	assert(-1 == arr->lookup(i2, v1));
	assert(2 == arr->lookup(i2, v2));
	assert(3 == arr->lookup(i3, v3));
	assert(4 == arr->lookup(i4, v4));
	assert(3 == arr->lookup(i3, v3));
	assert(-1 == arr->lookup(6, 0));
	assert(-1 == arr->lookup(6, 1));

	// test replacement (actually cannot do direct replacement, need to delete then add)
	assert(!arr->put(30, &i3, &v3));
	assert(3 == arr->lookup(i3, v3));
	assert(arr->getSize() == 4);

	// test deletion
	// remove->put
	assert(!arr->remove(i2, v1));
	assert(arr->remove(i2, v2));
	assert(arr->getSize() == 3);
	assert(1 == arr->get(0));
	assert(4 == arr->get(1));
	assert(3 == arr->get(2));
	assert(-1 == arr->lookup(i2, v2));
	assert(arr->put(5, &i5, &v5));
	// make sure we have replaced and that the rest of the array is correct
	assert(5 == arr->lookup(i5, v5));
	assert(1 == arr->lookup(i1, v1));
	assert(3 == arr->lookup(i3, v3));
	assert(4 == arr->lookup(i4, v4));
	assert(arr->getSize() == 4);
	assert(1 == arr->get(0));
	assert(4 == arr->get(1));
	assert(3 == arr->get(2));
	assert(5 == arr->get(3));

	// remove->get
	assert(arr->remove(i1, v1));
	assert(arr->getSize() == 3);
	assert(5 == arr->get(0));
	assert(4 == arr->get(1));
	assert(3 == arr->get(2));
	assert(-1 == arr->lookup(i1, v1));
	assert(5 == arr->lookup(i5, v5));
	assert(3 == arr->lookup(i3, v3));
	assert(4 == arr->lookup(i4, v4));
	assert(arr->put(10, &i1, &v1));
	assert(5 == arr->lookup(i5, v5));
	assert(10 == arr->lookup(i1, v1));
	assert(3 == arr->lookup(i3, v3));
	assert(4 == arr->lookup(i4, v4));
	assert(arr->getSize() == 4);
	assert(5 == arr->get(0));
	assert(4 == arr->get(1));
	assert(3 == arr->get(2));
	assert(10 == arr->get(3));

	// remove 2 -> get
	assert(arr->remove(i1, v1));
	assert(arr->remove(i3, v3));
	assert(arr->getSize() == 2);
	assert(5 == arr->lookup(i5, v5));
	assert(-1 == arr->lookup(i1, v1));
	assert(-1 == arr->lookup(i3, v3));
	assert(4 == arr->lookup(i4, v4));
	assert(5 == arr->get(0));
	assert(4 == arr->get(1));
	assert(arr->remove(i5, v5));
	assert(arr->remove(i4, v4));
	assert(!arr->remove(i4, v4));	*/
