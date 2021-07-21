/** Implements a Slot Map as in https://gamedev.stackexchange.com/questions/33888/what-is-the-most-efficient-container-to-store-dynamic-game-objects-in/33905#33905 */


#ifndef __OBJECT_CONTAINER_H__
#define __OBJECT_CONTAINER_H__

#include "PhysicalObject.h"
#include "Stack.h"


// store index and version (idea is to increment version id when delete so know something changed there)
// store in simpleobstacle?
class Element {
public:
	int indirectIndex;
	int version = 0; //important to init to 0!

	// actual data stored
	std::shared_ptr<PhysicalObject> data;
	//int data;
};





class ObjectContainer {
private:
	// bad practice to store pointers (b/c need extra lookup), but no choice in cugl (engine stores actual object)
	//std::shared_ptr<Element[]> _objects; // where we store actual stuff (should be void* so can put anything in here!)
	//std::shared_ptr<int[]> _slots; //indirection table. this is needed when you swap and pop in the primary array and indices will change

	std::vector<Element> _objects;
	std::vector<int> _slots;
	std::shared_ptr<Stack<int>> _freeList; // where free indices are in the slot list

	int _count;

	int _maxCount;

public:

	void dispose();

	ObjectContainer(int maxCount);

	virtual ~ObjectContainer(void) { dispose(); }

	std::shared_ptr<PhysicalObject> get(int idx);
	std::shared_ptr<PhysicalObject> lookup(int idx, int version);
	bool put(std::shared_ptr<PhysicalObject>, int *idx, int *version);
	//int get(int idx);
	//int lookup(int idx, int version);
	//bool put(int data, int *idx, int *version);
	bool remove(int idx, int version);
	int getSize() const { return _count; }
	
};




#endif