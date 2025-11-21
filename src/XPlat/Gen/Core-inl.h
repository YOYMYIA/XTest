#ifndef XPLAT_GEN_CORE_INL_H
#define XPLAT_GEN_CORE_INL_H
#include <type_traits>

namespace xplat{
namespace gen{

/**

 * @brief Trait to check if a candidate type is compatible with an expected type
 * Matches an expected signature with a candidate signature.
 * @tparam Candidate The candidate type to check
 * @tparam Expected The expected type to check against
 * @return True if the candidate type is compatible with the expected type, false otherwise
 * @return False if the candidate type is not compatible with the expected type
 */
template <class Candidate, class Expected>
struct IsCompatibleSignature
{
    static constexpr bool value = false;
};

/**

/**
 * @brief Trait to check if a candidate type is compatible with an expected signature
 * @tparam Candidate The candidate type to check
 * @tparam ExpectedSignature The expected signature to check against
 */
template <class Candidate, class ExpectedReturn, class ... ArgTypes>
class IsCompatibleSignature< Candidate, ExpectedReturn(ArgTypes...)>
{
    /**
     * @tparam F The function type to check
     * @tparam ActualReturn The actual return type of the function
     * decltype is used to get the return type of the function
     * std::declval is used to get the arguments of the function
     */
    template <
        class F,
        class ActualReturn = 
            decltype(std::declval<F>()(std::declval<ArgTypes>()...)),
        bool good = std::is_same<ActualReturn, ExpectedReturn>::value>
    static constexpr bool testArgs(int*) 
    {
        return good;
    }
    
    template <class F>
    static constexpr bool testArgs(...) 
    {
        return false;
    }
public:
    static constexpr bool value = testArgs<Candidate>(nullptr);
};

/**
 *  Helper type for the curiously recurring template pattern
 */

template <class Self>
struct XBounded {
  using SelfType = Self;
  const Self& self() const { return *static_cast<const Self*>(this); }

  Self& self() { return *static_cast<Self*>(this); }
};

namespace detail{

} //namespace detail
} //namespace gen
} //namespace xplat

#endif // XPLAT_GEN_CORE_INL_H