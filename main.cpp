#include <iostream>
#include "array.hpp"
using namespace std;
typedef long long ll;



int main() {
    Array<int> arr = {1, 2, 3, 4, 5};
    std::back_insert_iterator<Array<int>> it(arr);
    std::cout << *it;
    return 0;
}
