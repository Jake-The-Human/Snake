#include "snake.h"

#include <cassert>

int main() {
    Snake s(1);
    s.push_back(2);
    s.pop_back();
    s.push_back(3);
    s.push_back(4);
    s.push_back(6);
    s.pop_front();
    assert(s.size() == 3);
    s.test();
    
    Snake temp;
    temp.push_back(1);
    assert(temp.size() == 1);
    
    Snake temp2;
    temp2.push_front(1);
    assert(temp2.size() == 1);
    return 0;
}