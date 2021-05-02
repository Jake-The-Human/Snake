#ifndef SNAKE_H
#define SNAKE_H

#include <functional>

using GridIndex = int;

struct Node {
    GridIndex mIndex{ 0 };
    //FoodType mFood();// need to do some thing with this
    Node* mNext{ nullptr };
    Node* mPrev{ nullptr };

    Node(GridIndex i) : mIndex(i) {}
    Node(GridIndex i, Node* next, Node* prev) : mIndex(i), mNext(next), mPrev(prev) {}
};

class Snake {
    public:
        Snake() = default;
        Snake(GridIndex head);

        Snake(const Snake&) = delete;
        Snake(Snake&&) = default;
        
        ~Snake();
        void delete_snake();

        inline unsigned size() const { return mLength; }
        inline bool empty() const { return mHead == nullptr; }
        inline GridIndex get_head() const { return mHead->mIndex; }

        void push_back(GridIndex);
        void pop_back();

        void push_front(GridIndex);
        void pop_front();
        
        bool collision(GridIndex);

        void foreach(std::function<void(int)>) const;
        void test();

    private:
        Node* mHead{ nullptr };
        Node* mTail{ nullptr };

        unsigned mLength{ 0 };
        // unsigned mPlayHead{ 0 };
};

#endif