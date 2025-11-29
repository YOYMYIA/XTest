#include <Io/async/AsynBase.h>
#include <iostream>

int main()
{
    using namespace xplat::literals::integer_constant_literals;
    constexpr auto size = 1024_uzic;
    //std::cout << xplat::literals::integer_constant_literals::"123"_uzic();
    std::cout << size.value << std::endl;
    return 0;
}