#include "snake.h"

#include <iostream>

Snake::Snake(GridIndex head)
{
    mHead = new Node(head);
    mTail = mHead;
    mLength++;
}


Snake::~Snake()
{
    delete_snake();
}


void
Snake::delete_snake()
{
    Node* temp{ mHead };
    while (temp != nullptr)
    {
        mHead = mHead->mNext;
        delete temp;
        temp = mHead;
    }
}

void 
Snake::push_back(GridIndex index)
{
    if(mHead == nullptr)
    {
        push_front(index);
    }
    else
    {
        mTail->mNext = new Node(index, nullptr, mTail);
        mTail = mTail->mNext;
        mLength++;
    }
}


void
Snake::pop_back()
{
    if(mTail == nullptr)
        return;
    Node* temp = mTail;
    mTail = temp->mPrev;
    delete temp;
    mTail->mNext = nullptr;
    mLength--;
    return;
}


void
Snake::push_front(GridIndex index)
{
    if(mHead == nullptr)
    {
        mHead = new Node(index);
        mTail = mHead;
    }
    else
    {
        Node* temp{ new Node(index, mHead, nullptr) };
        mHead->mPrev = temp;
        mHead = temp;
    }
    
    mLength++;
}


void
Snake::pop_front()
{
    if(mHead == nullptr)
        return;

    Node* temp{ mHead };
    mHead = mHead->mNext;
    delete temp;
    mHead->mPrev = nullptr;
    mLength--;

    return;
}


bool 
Snake::collision(GridIndex index)
{
    Node* temp{ mHead };
    while(temp != nullptr)
    {
        // printf("collision");
        if(temp->mIndex == index)
            break;
        temp = temp->mNext;
    }

    return (temp != nullptr);
}


void
Snake::test()
{
    std::cout << size() << std::endl;

    Node* temp = mHead;
    while(temp != nullptr)
    {
        std::cout << temp->mIndex << ',';
        temp = temp->mNext;
    }
    std::cout << std::endl;

    temp = mTail;
    while(temp != nullptr)
    {
        std::cout << temp->mIndex << ',';
        temp = temp->mPrev;
    }
    std::cout << std::endl;
}

void 
Snake::foreach(std::function<void(int)> func) const
{
    Node* temp{mHead};
    while(temp != nullptr)
    {
        // printf("foreach");
        func(temp->mIndex);

        temp = temp->mNext;
    }
}
