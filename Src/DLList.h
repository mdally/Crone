#ifndef _DLLIST_H_
#define _DLLIST_H_

#include <MemoryPool\C-11\MemoryPool.h>

template <typename T>
class DLListNode {
	public:
		DLListNode() : next(nullptr), prev(nullptr) {};
		DLListNode(const T& _data) : data(_data), next(nullptr), prev(nullptr) {};

		T data;
		DLListNode<T>* next;
		DLListNode<T>* prev;
};

template <typename T>
class DLList {

	public:
		DLList() : head(nullptr), tail(nullptr) {};
		~DLList() {};

		DLListNode<T>* addToTail(const T& data);
		DLListNode<T>* addAfterNode(const T& newData, DLListNode<T>* node);
		void removeNode(DLListNode<T>* node);

		DLListNode<T>* head;
		DLListNode<T>* tail;
	private:
		MemoryPool<DLListNode<T>> nodePool;
};

template <typename T>
DLListNode<T>* DLList<T>::addToTail(const T& data) {
	DLListNode<T>* newNode = nodePool.newElement(data);

	if (head == nullptr) {
		head = newNode;
		tail = newNode;
	}
	else {
		tail->next = newNode;
		newNode->prev = tail;

		newNode->next = head;
		head->prev = newNode;

		tail = newNode;
	}

	return newNode;
}

template <typename T>
DLListNode<T>* DLList<T>::addAfterNode(const T& newData, DLListNode<T>* node) {
	DLListNode<T>* newNode = nodePool.newElement(newData);

	node->next->prev = newNode;
	newNode->next = node->next;

	newNode->prev = node;
	node->next = newNode;

	if (node == tail) tail = newNode;

	return newNode;
}

template <typename T>
void DLList<T>::removeNode(DLListNode<T>* node) {
	if (node->next)
		node->next->prev = node->prev;
	if (node->prev)
		node->prev->next = node->next;

	if (head == node)
		head = node->next;
	if (tail == node)
		tail = node->prev;

	if (node->next != nullptr && node->next == node->next->next) {
		node->next->next = nullptr;
		node->next->prev = nullptr;
	}

	nodePool.deleteElement(node);
}

#endif