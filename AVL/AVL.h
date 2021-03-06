
#ifndef AVL_H
#define AVL_H

#include <iostream>
#include <stdlib.h>
#include <stack>
#include "../data_structs/queueLL.h"

using std::stack;

template<typename T>
class AVL{
    private:
        class Vertex{
            public:
                int height;
                Vertex() :
                    height(0), right(nullptr), left(nullptr){}

                Vertex(const T& data) :
                    height(0), right(nullptr), left(nullptr), data(data){}

                Vertex(const T& data, Vertex* left, Vertex* right) :
                    height(0), right(right), left(left), data(data){}

                Vertex* right;
                Vertex* left;
                T data;
        };

        enum WeightStatus{BALANCED, LEFT_HEAVY, RIGHT_HEAVY, ERROR};

        Vertex* root;

        //keeps track of the vertex when a find operation has been executed
        Vertex* finder;

        int numberOfElements;

        ///Helper Functions///
        void insert(const T& entry, Vertex*& current);
        void remove(const T& target, Vertex*& current);

        void clear(Vertex*& current);
        void displayInOrder(const Vertex* current);
        void displayPreOrder(const Vertex* current);
        void displayPostOrder(const Vertex* current);

        Vertex* find(Vertex* current, const T& target) const;
        const Vertex* getMin(const Vertex* current) const;
        const Vertex* getMax(const Vertex* current) const;
        int getHeight(const Vertex* current) const;
        WeightStatus getWeight(const Vertex* current) const;

        void balance(Vertex*& current);
        void leftRotation(Vertex*& current);
        void rightRotation(Vertex*& current);
        void updateHeight(Vertex*& current);

        int max(int a, int b) const;

    public:
        class Iterator{
            private:
                stack<Vertex*> parents;
                Vertex* current = nullptr;
            public:
                Iterator(){}
                Iterator(Vertex* const begin){
                    current = begin;
                }

                T& getData() const{
                    return current->data;
                }

                //in order traversal through tree
                T& next(){
                    //go all the way to the left - current is null after the loop
                    while(current){
                        parents.push(current);
                        current = current->left;
                    }
                    //get parent
                    current = parents.top();
                    parents.pop();

                    //save current
                    Vertex* temp = current;

                    //go to the right of the tree
                    current = current->right;
                    return temp->data;
                }
                bool hasNext(){
                    return current != nullptr || !parents.empty();
                }
                //the stack gets reset - only the current Vertex is copied
                Iterator operator=(const Iterator& src){
                    if(this == &src)
                        return *this;
                    this->current = src.current;
                    return *this;
                }

                bool operator==(const Iterator& i){
                    return current == i.current;
                }
                bool operator!=(const Iterator& i){
                    return current != i.current;
                }
        };

        AVL();
        AVL(const AVL& src);
        ~AVL();

        void insert(const T& entry);
        void remove(const T& target);

        bool find(const T& target);

        Iterator get(const T& target);

        //Should only be called if find() returned true;
        const T& getFoundData() const;

        const T& getMin() const;
        const T& getMax() const;
        int getHeight() const;

        void display();         //inOrderDisplay - generic name
        void displayInOrder();
        void displayPreOrder();
        void displayPostOrder();
        void displayLevelOrder();
        const T& getRootValue();

        int size();
        bool isEmpty();
        void clear();

        const Iterator begin() const;
        const Iterator end() const;
};

template<typename T>
AVL<T>::AVL() :
    root(nullptr), finder(nullptr), numberOfElements(0)
{}

template<typename T>
AVL<T>::AVL(const AVL& src){
    root = nullptr;
    finder = nullptr;

    Iterator src_itr;
    src_itr = src.begin();
    while(src_itr.hasNext())
        this->insert(src_itr.next());
}

template<typename T>
AVL<T>::~AVL(){
    clear();
}

template<typename T>
void AVL<T>::insert(const T& entry){
    insert(entry, root);
}

template<typename T>
void AVL<T>::remove(const T& target){
    remove(target, root);
}

template<typename T>
bool AVL<T>::find(const T& target){
    finder = find(root, target);
    return (finder);
}

template<typename T>
typename AVL<T>::Iterator AVL<T>::get(const T& target){
    return Iterator(find(root, target));
}

template<typename T>
const T& AVL<T>::getFoundData() const {
    return finder->data;
}

template<typename T>
const T& AVL<T>::getMin() const{
    return getMin(root)->data;
}

template<typename T>
const T& AVL<T>::getMax() const{
    return getMax(root)->data;
}

template<typename T>
int AVL<T>::getHeight() const{
    return getHeight(root);
}

template<typename T>
void AVL<T>::display(){
    displayInOrder(root);
}

//left-parent-right
template<typename T>
void AVL<T>::displayInOrder(){
    displayInOrder(root);
}

//parent-left-right
template<typename T>
void AVL<T>::displayPreOrder(){
    displayPreOrder(root);
}

//left right parent
template<typename T>
void AVL<T>::displayPostOrder(){
    displayPostOrder(root);
}

template<typename T>
void AVL<T>::displayLevelOrder(){
    QueueLL<Vertex*> container;
    container.enqueue(root);
    while(!container.empty()){
        Vertex* current = container.dequeue();
        if(current != nullptr){

            //display parent
            std::cout << current->data << std::endl;

            //enqueue children for future printing
            container.enqueue(current->left);
            container.enqueue(current->right);
        }
    }
}

template<typename T>
const T& AVL<T>::getRootValue(){
    return root->data;
}

template<typename T>
int AVL<T>::size(){
    return numberOfElements;
}

template<typename T>
bool AVL<T>::isEmpty(){
    return root == nullptr;
}

template<typename T>
void AVL<T>::clear(){
    clear(root);
}

template<typename T>
const typename AVL<T>::Iterator AVL<T>::begin() const{
    return Iterator(root);
}

template<typename T>
const typename AVL<T>::Iterator AVL<T>::end() const{
    return Iterator(nullptr);
}

///Helper Functions///
template<typename T>
void AVL<T>::insert(const T& entry, Vertex*& current){
    //empty slot found
    if(current == nullptr){
        current = new Vertex(entry);
        ++numberOfElements;
    }

    //larger values go to right of tree
    else if(entry > current->data)
        insert(entry, current->right);

    //smaller values go to the left
    else
        insert(entry, current->left);

    updateHeight(current);

    //balance every vertex(if needed) along the traversed path
    balance(current);
}

template<typename T>
void AVL<T>::remove(const T& target, Vertex*& current){
    //search for the target
    if(current != nullptr){
        //found
        if(target == current->data){
            --numberOfElements;
            //full node case
            if(current->left && current->right){
                int r = rand() % 2;
                //replace data with maximum vertex data in left substree
                if(r == 0){
                    Vertex* max = const_cast<Vertex*>(getMax(current->left));
                    current->data = max->data;
                    delete max;
                    max = nullptr;
                }
                //replace data with minimum vertex data in right substree
                else{
                    Vertex* min = const_cast<Vertex*>(getMin(current->right));
                    current->data = min->data;
                    delete min;
                    min = nullptr;
                }
            }
            //left child exists - create link from current's parent to current's child
            else if(current->left){
                Vertex* child = current->left;
                delete current;
                current = child;
            }
            //right child exists - create link from current's parent to current's child
            else if(current->right){
                Vertex* child = current->right;
                delete current;
                current = child;
            }
            //leaf
            else{
                delete current;
                current = nullptr;
            }
        }
        //search left
        else if(target > current->data)
            remove(target, current->right);

        //search right
        else
            remove(target, current->left);
    }
    updateHeight(current);

    //balance every vertex(if needed) along the traversed path
    balance(current);
}

template<typename T>
typename AVL<T>::Vertex* AVL<T>::find(Vertex* current, const T& target) const{

    //return a default T
    if(current == nullptr)
        return current;

    //found
    if(target == current->data)
        return current;

    //search left
    else if(target < current->data)
        return find(current->left, target);

    //search right
    else
        return find(current->right, target);
}

//return the left most vertex
template<typename T>
const typename AVL<T>::Vertex* AVL<T>::getMin(const Vertex* current) const{
    if(current == nullptr)
        return current;
    while(current->left != nullptr)
        current = current->left;
    return current;
}

//return the right most vertex
template<typename T>
const typename AVL<T>::Vertex* AVL<T>::getMax(const Vertex* current) const{
    if(current == nullptr)
        return current;
    while(current->right != nullptr)
        current = current->right;
    return current;
}

template<typename T>
int AVL<T>::getHeight(const Vertex* current) const{
    if(current == nullptr)
        return -1;
    return current->height;
}

template<typename T>
typename AVL<T>::WeightStatus AVL<T>::getWeight(const Vertex* current) const{
    if(current == nullptr)
        return ERROR;

    int l = getHeight(current->left);
    int r = getHeight(current->right);
    int diff = l - r;

    if(diff > 1 ) return LEFT_HEAVY;
    if(diff < -1) return RIGHT_HEAVY;
    return BALANCED;
}

template<typename T>
void AVL<T>::balance(Vertex*& current){
    if(current != nullptr){
        WeightStatus w = getWeight(current);

        //left heavy
        if(w == LEFT_HEAVY){
            w = getWeight(current->left);

            //left-left heavy
            if(w == LEFT_HEAVY || w == BALANCED)
                rightRotation(current);

            //left-right heavy
            else{
                leftRotation(current->left);
                rightRotation(current);
            }
        }
        //right heavy
        else if(w == RIGHT_HEAVY){
            w = getWeight(current->right);

            //right-right heavy
            if(w == RIGHT_HEAVY || w == BALANCED)
                leftRotation(current);

            //right-left heavy
            else{
                rightRotation(current->right);
                leftRotation(current);
            }
        }
    }
}

template<typename T>
void AVL<T>::leftRotation(Vertex*& current){
    if(current != nullptr){
        Vertex* newLeftChild = current;
        Vertex* newCurrent = current->right;
        Vertex* newLeftRightChild = current->right->left;

        current = newCurrent;                       //height changes
        current->left = newLeftChild;               //height changes
        current->left->right = newLeftRightChild;   //height doesn't change

        //update heights
        updateHeight(current->left);
        updateHeight(current);
    }
}

template<typename T>
void AVL<T>::rightRotation(Vertex*& current){
    if(current != nullptr){
        Vertex* newRightChild = current;
        Vertex* newCurrent = current->left;
        Vertex* newRightLeftChild = current->left->right;

        current = newCurrent;
        current->right = newRightChild;
        current->right->left = newRightLeftChild;

        //update heights
        updateHeight(current->right);
        updateHeight(current);
    }
}

template<typename T>
void AVL<T>::updateHeight(Vertex*& current){
    if(current != nullptr){
        int l = getHeight(current->left);
        int r = getHeight(current->right);
        current->height = 1 + max(l, r);
    }
}

template<typename T>
void AVL<T>::displayInOrder(const Vertex* current){
    if(current != nullptr){
        //print left subtree
        displayInOrder(current->left);
        //print parent
        std::cout << current->data << std::endl;
        //print right subtree
        displayInOrder(current->right);
    }
}

template<typename T>
void AVL<T>::displayPreOrder(const Vertex* current){
    if(current != nullptr){
        std::cout << current->data << std::endl;
        displayInOrder(current->left);
        displayInOrder(current->right);
    }
}

template<typename T>
void AVL<T>::displayPostOrder(const Vertex* current){
    if(current != nullptr){
        displayInOrder(current->left);
        displayInOrder(current->right);
        std::cout << current->data << std::endl;
    }
}

template<typename T>
void AVL<T>::clear(Vertex*& current){
    if(current != nullptr){
        //go to children
        clear(current->right);
        clear(current->left);

        //delete parent
        delete current;
        current = nullptr;
    }
}

template<typename T>
int AVL<T>::max(int a, int b) const{
    return (a > b) ? a : b;
}

#endif

