#include "compiler.h"

#ifndef PQUEUE_HH
#define PQUEUE_HH

template<class Type> class PQueueItem;

template<class Type>
class PQueue {
public:
	PQueue();
	virtual ~PQueue();

	bool IsEmpty();
    uword Length() const;
	void Append(const Type&);
    void Prepend(const Type&);
	Type& Front();
    void Pop();
	void Remove(const Type&);

	void ResetIterator();
	Type& Next();

private:
	uword myLength;
	PQueueItem<Type>* first;
	PQueueItem<Type>* last;
	PQueueItem<Type>* iter;
};

template<class Type>
class PQueueItem {
public:
	PQueueItem(const Type);
    ~PQueueItem();
	Type& getItem();

	PQueueItem<Type>* next;
private:
	Type myItem;
};

#include "queue.icc"

#endif

