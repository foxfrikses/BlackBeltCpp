#include <iostream>
#include <cstdlib>
#include <limits>
#include <inttypes.h>

int main()
{
    static constexpr int64_t maxValue = std::numeric_limits<int64_t>::max();
    static constexpr int64_t minValue = std::numeric_limits<int64_t>::min();

    std::int64_t num1, num2;
    std::cin >> num1 >> num2;

    if (num1 > 0 && num2 > 0) {
        if (maxValue - num1 < num2) {
            std::cout << "Overflow!";
            return 0;
        }
    } else if (num1 < 0 && num2 < 0) {
        if (minValue - num1 > num2) {
            std::cout << "Overflow!";
            return 0;
        }
    }

    std::cout << num1 + num2;
    return 0;
}

