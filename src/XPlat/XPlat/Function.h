#ifndef XPLAT_FUNCTION_H
#define XPLAT_FUNCTION_H

//#include <functional>
#include <type_traits>
#include <XPlat/Traits.h>

namespace xplat
{
    template <typename FunctionType>
    class Function;

    template <typename ReturnType, typename ...Args>
    Function<ReturnType(Args...) const> constCastFunction(
        Function<ReturnType(Args...)> &&) noexcept;

    template <typename ReturnType, typename ...Args>
    Function<ReturnType(Args...) const noexcept> constCastFunction(
        Function<ReturnType(Args...) noexcept> &&) noexcept;

namespace detail
{

namespace function
{
    // define the operation type for the function
    enum class Op {MOVE, NUKE, HEAP};

    union Data
    {
        struct BigTrivialLayout
        {
            void* big;
            std::size_t size;
            std::size_t align;
        };

        void* big;
        BigTrivialLayout big_t;
        std::aligned_storage<6 * sizeof(void *)>::type tiny;
    };

    struct CoerceTag {};

    template <typename T>
    using FunctionNullptrTest = decltype(
        static_cast<bool>(static_cast<T const&>(T(nullptr))== nullptr));

    template <typename T>
    constexpr bool IsNullptrCompatible = is_detected_v<FunctionNullptrTest, T>;




}// namespace function


} // namespace detail


} // namespace xplat

#endif // XPLAT_FUNCTION_H