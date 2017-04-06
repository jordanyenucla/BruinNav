// MyMap.h

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.
#ifndef MYMAP_H
#define MYMAP_H

#include <string>
#include <queue>
#include "support.h"
template<typename KeyType, typename ValueType>
class MyMap
{
public:
	MyMap();
	~MyMap();
	void clear();
	int size() const;
	void associate(const KeyType& key, const ValueType& value);

	  // for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	  // for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
	}

	  // C++11 syntax for preventing copying and assignment
	MyMap(const MyMap&) = delete;
	MyMap& operator=(const MyMap&) = delete;

private:
	struct Node {
		KeyType key;
		ValueType value;
		Node* left;
		Node* right;
	};
	Node* m_root;
	int m_size;
};

template<typename KeyType, typename ValueType>
MyMap<typename KeyType, typename ValueType>::MyMap()
{
	m_root = nullptr;
	m_size = 0;
}

template<typename KeyType, typename ValueType>
MyMap<typename KeyType, typename ValueType>::~MyMap()
{
	clear();
}

template<typename KeyType, typename ValueType>
void MyMap<typename KeyType, typename ValueType>::clear()
{
	if (m_root == nullptr) {
		return;
	}
	queue<Node*> q;
	q.push(m_root);
	while (!q.empty()) {
		Node* traverse = q.front();
		q.pop();
		if (traverse->left != nullptr) {
			q.push(traverse->left);
		}
		if (traverse->right != nullptr) {
			q.push(traverse->right);
		}
		delete traverse;
	}
	m_root = nullptr;
}

template<typename KeyType, typename ValueType>
int MyMap<typename KeyType, typename ValueType>::size() const
{
	return m_size;
}

template<typename KeyType, typename ValueType>
void MyMap<typename KeyType, typename ValueType>::associate(const KeyType& key, const ValueType& value)
{
	ValueType* finder = find(key); //create a value type that is equal to the returned value for find
	if (finder== nullptr) {
		Node* traverse = m_root;
		for (;;) {
			if (traverse == nullptr)
				break;
			if (traverse->key > key) {
				if (traverse->left != nullptr) {
					traverse = traverse->left;
				} //if key is less than node's key, go left
				else
					break;
			}
			if (traverse->key < key) {
				if (traverse->right != nullptr) {
					traverse = traverse->right;
				} //if key is greater then node's key, go right
				else
					break;
			}
		}
		//add in all the info
		Node* add = new Node;
		add->left = nullptr;
		add->right = nullptr;
		add->value = value;
		add->key = key;
		m_size++;

		if (traverse == nullptr) {
			m_root = add;
		} //if is is empty then set head to first node
		else {
			if (traverse->key > key) {
				traverse->left = add;
			}
			else
				traverse->right = add;
		}

	}
	else {
		*finder= value; //if it's not nullptr, then that means there already exists a key, so change key's value to parameter's value
	}
}


template<typename KeyType, typename ValueType>
const ValueType* MyMap<typename KeyType, typename ValueType>::find(const KeyType& key) const
{
	if (m_root == nullptr)
		return nullptr;

	Node* traverse = m_root;
	for (;;) {
		if (traverse->key == key) {
			break;
		} //if keys match, then break
		else if (traverse->key > key) {
			if (traverse->left != nullptr) {
				traverse = traverse->left;
			} //if not nullptr, then set traverse to left to continue traversing
			else {
				break;
			} //if nullptr then break
		}
		else if (traverse->key < key) {
			if (traverse->right != nullptr) {
				traverse = traverse->right;
			} //if not nullptr, then set traverse to right to continue traversing
			else {
				break;
			} //if nullptr, then breal
		}
	}

	if (traverse->key == key) {
		ValueType* result = &traverse->value;
		return result;
	} //if the key is found, return the value
	else
		return nullptr;
}
#endif
