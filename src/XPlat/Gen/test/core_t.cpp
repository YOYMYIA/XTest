#include <iostream>
#include "Gen/Core.h"
#include <type_traits>

int main()
{
    auto f1 = []() -> int
    { return 0; };

    bool val = xplat::gen::IsCompatibleSignature<decltype(f1), int()>::value;
    bool val_t = std::is_same<decltype(f1), int()>::value;
    std::cout << val << std::endl;
    std::cout << val_t << std::endl;
    std::cout << typeid(decltype(f1)).name() << std::endl;
    std::cout << typeid(int()).name() << std::endl;
    return 0;
}
