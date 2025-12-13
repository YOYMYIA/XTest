#ifndef XPLAT_TRAITS_H
#define XPLAT_TRAITS_H

//#include <tuple>
#include <type_traits>

namespace xplat {

// C++ 20 std::type_identify && std::type_identity_t
template <typename T> struct type_identify {
  using type = T;
};

template <typename T> using type_identity_t = typename type_identify<T>::type;

// define a type-list; A generic value type and value
template <typename...> struct tag_t {};
template <typename... T> inline constexpr tag_t<T...> tag{};

// A value-list is a list of values of the same type
template <auto...> struct vtag_t {};
template <auto... V> inline constexpr vtag_t<V...> vtag{};

template <std::size_t I>
using index_constant = std::integral_constant<std::size_t, I>;

namespace detail {
template <typename Int> constexpr Int parse_uic(char const *str) noexcept {
  Int result = 0;
  while (*str) {
    auto const c = *str++;
    if (c >= '0' && c <= '9') {
      result = result * 10 + (c - '0');
    }
  }
  return result;
}

} // namespace detail

inline namespace literals {
// C++ 20 std::integer_constant_literals
// Define a user-defined literal operator for integer constants
// The operator takes a string of digits and returns an index_constant
inline namespace integer_constant_literals {
template <char... Digits> constexpr auto operator""_uzic() noexcept {
  constexpr char digits[] = {Digits..., '\0'};
  return index_constant<detail::parse_uic<std::size_t>(digits)>{};
}
} // namespace integer_constant_literals
} // namespace literals

// always_false
/**
 * A common use-case is for exhaustive if branches:
 * template <typename T>
 * void foo(T)
 * {
 *     if constexpr(always_false<T>)
 *     {
 *         return;
 *     }
 *     else
 *     {
 *         // T is a valid type
 *         // do something with T
 *     }
 * }
 */
template <typename...> inline constexpr bool always_false = false;

namespace detail {
template <typename Void, typename T> struct require_sizeof_ {
  static_assert(always_false<T>, "This type is incomplete");
};

template <typename T> struct require_sizeof_<decltype(void(sizeof(T))), T> {
  template <typename V> using apply_t = V;

  static constexpr std::size_t size = sizeof(T);
};
} // namespace detail

// require_sizeof
template <typename T>
static constexpr std::size_t require_sizeof =
    detail::require_sizeof_<void, T>::size;

// is_complete
template <typename T> struct is_complete {
  static_assert(always_false<T>, "This type is incomplete");
};

template <typename T> constexpr bool is_complete_v = is_complete<T>::value;

// is_unbounded_arrray_v
template <typename T> inline constexpr bool is_unbounded_array_v = false;
template <typename T> inline constexpr bool is_unbounded_array_v<T[]> = true;
template <typename T>
struct is_unbounded_array : std::bool_constant<is_unbounded_array_v<T>> {};

// is_bounded_array_v
template <typename T> inline constexpr bool is_bounded_array_v = false;
template <typename T, std::size_t S>
inline constexpr bool is_bounded_array_v<T[S]> = true;
template <typename T>
struct is_bounded_array : std::bool_constant<is_bounded_array_v<T>> {};

// is_instantiation_of_v
// A trait variable and type to check if a given type is
// an instantiation of a given class template
template <template <typename...> class, typename>
inline constexpr bool is_instantiation_of_v = false;
template <template <typename...> class C, typename... T>
inline constexpr bool is_instantiation_of_v<C, C<T...>> = true;
template <template <typename...> class C, typename... T>
struct is_instantiation_of
    : std::bool_constant<is_instantiation_of_v<C, T...>> {};

// member_pointer_traits
// Works for both member-object-pointer and member-function-pointer.
template <typename T> struct member_pointer_traits;
template <typename M, typename O> struct member_pointer_traits<M O::*> {
  using member_type = M;
  using object_type = O;
};


template <typename P>
using member_pointer_member_t = typename member_pointer_traits<P>::member_type;
template <typename P>
using member_pointer_object_t = typename member_pointer_traits<P>::object_type;

namespace detail {
struct is_constexpr_default_constructible_ {

  template <typename T>
  static constexpr auto make(tag_t<T>) -> decltype(void(T()), 0) {
    return (void(T()), 0);
  }

  // SFINTE detector for constexpr default constructible
  template <typename T, int = make(tag<T>)>
  static std::true_type sfinae(T*);
  static std::false_type sfinae(void*);
  
  template <typename T>
  static constexpr bool apply =
    !require_sizeof<T> || decltype(sfinae(static_cast<T*>(nullptr)))::value;
};
}  // namespace detail

/**
 * A trait variable and type which determines whether the type paramter is
 * constexpr default constructible
*/
template <typename T>
inline constexpr bool is_constexpr_default_constructible_v =
    detail::is_constexpr_default_constructible_::apply<T>;


template <typename T>
struct is_constexpr_default_constructible
    : std::bool_constant<is_constexpr_default_constructible_v<T>> {};


/**
 * _t:
 * A type used to instead of : using decalyed = typename std::decay<T>::type;
*/
template <typename T>
using _t = typename T::type;

template <typename T>
struct remove_cvref{
  using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
};

// remove_cvref_t similar to C++20 std::remove_cvref_t
template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

namespace detail {

template <typename Src>
struct copy_cvref_ {
  template <typename Dst>
  using apply = Dst;
};

template <typename Src>
struct copy_cvref_<Src const> {
  template <typename Dst>
  using apply = Dst const;
};

template <typename Src>
struct copy_cvref_<Src volatile> {
  template <typename Dst>
  using apply = Dst volatile;
};

template <typename Src>
struct copy_cvref_<Src const volatile> {
  template <typename Dst>
  using apply = Dst const volatile;
};

template <typename Src>
struct copy_cvref_<Src&> {
  template <typename Dst>
  // This is nested template application syntax in C++.
  // It means: when Src is a reference type (e.g. T&), apply the cv-qualifiers
  // recursively and pass Dst as a reference (Dst&) as well.
  using apply = typename copy_cvref_<Src>::template apply<Dst>&;
};

template <typename Src>
struct copy_cvref_<Src&&> {
  template <typename Dst>
  using apply = typename copy_cvref_<Src>::template apply<Dst>&&;
};


}  // namespace detail

// To using a toolbox, we need to copy the cv-qualifiers of the source type to the destination type.
template <typename Src, typename Dst>
using copy_cvref_t =
    typename detail::copy_cvref_<Src>::template apply<remove_cvref_t<Dst>>;

namespace detail {

template <typename Src>
struct copy_ref_ {
  template <typename Dst>
  using apply = Dst;
};

template <typename Src>
struct copy_ref_<Src &> {
  template <typename Dst>
  using apply = Dst&;
};

template <typename Src>
struct copy_ref_<Src &&> {
  template <typename Dst>
  using apply = Dst&&;
};

template <typename Src, typename Dst>
using copy_const_t = std::conditional_t<
    std::is_const_v<std::remove_reference_t<Src>>,
    Dst const,
    Dst>;
}  // namespace detail

template <typename Src, typename Dst>
using like_t = typename detail::copy_ref_<Src>::template apply<
    detail::copy_const_t<Src, std::remove_reference_t<Dst>>>;

template <typename Src, typename Dst>
struct like {
  using type = like_t<Src, Dst>;
};


/**
type_t
type_t is useful for contorlling chass-template and function-template partial
specialization.

*/
namespace traits_detail {

template <class T, class...>
struct type_t_ {
  using type = T;
};
};  // namespace traits_detail

template <class T, class... Ts>
using type_t = typename traits_detail::type_t_<T, Ts...>::type;

// void_t is a type-list of void
template <class... Ts>
using void_t = type_t<void, Ts...>;

/**
 * nonesuch
 * A tag type which traits may use to indicate lack of a result type.
 * Similar to void but Different that no functions may be defined with this
 * return type and no complete expressions may evaluate with this expression
 * type.
 */
struct nonesuch {
  ~nonesuch() = delete;
  nonesuch(nonesuch const &) = delete;
  void operator=(nonesuch const &) = delete;
};

namespace detail {
template <typename Void, typename D, template <typename...> class, typename...>
struct detected_ {
  using value_t = std::false_type;
  using type = D;
};

template <typename D, template <typename...> class T, typename... A>
struct detected_<std::void_t<A...>, D, T, A...> {
  using value_t = std::true_type;
  using type = T<A...>;
};
}; // namespace detail

template <typename D, template <typename...> class T, typename... A>
using detected_or = detail::detected_<void, D, T, A...>;

template <typename D, template <typename...> class T, typename... A>
using detected_or_t = typename detected_or<D, T, A...>::type;

template <template <typename...> class T, typename... A>
using detected_t = detected_or<nonesuch, T, A...>;

template <template <typename...> class T, typename... A>
inline constexpr bool is_detected_v =
    detected_or<nonesuch, T, A...>::value_t::value;

template <template <typename...> class T, typename... A>
struct is_detected : detected_or<nonesuch, T, A...>::value_t {};

template <typename T>
using aligned_storage_for_t =
    typename std::aligned_storage<sizeof(T), alignof(T)>::type;

namespace fallback {
template <typename From, typename To>
inline constexpr bool is_nothrow_convertible_v =
    (std::is_void<From>::value && std::is_void<To>::value) ||
    (std::is_convertible<From, To>::value &&
     std::is_nothrow_constructible<From, To>::value);

template <typename From, typename To>
struct is_nothrow_convertible
    : std::bool_constant<is_nothrow_convertible_v<From, To>> {};

} // namespace fallback

using fallback::is_nothrow_convertible;
using fallback::is_nothrow_convertible_v;

/**
 * Is Relocatable<T>::value
 * Memory a value of type T by using memcpy
 * ( As opposed to memcpying a pointer to T, approach of calling
 * the copy constructor and then destorying the old temporary)
 *
 *
 * The detect_##name is a SFINAE detector
 * If T does not have name, this line casues a substitution failure
 * It uses std::is_same to check if T::name is strictly std::true_type
 * The is_detected_v that returns true if T is valid and fase otherwise.

 * Logic Flow:
 * If T::name is exits: It inherits form name##_is_true<T>
 * If T::name does not exit: It inherits form std::false_type
 */

namespace traits_detail {
#define XPLAT_HAS_TRYE_XXX(name)                                               \
  template <typename T> using detect_##name = typename T::name;                \
  template <class T>                                                           \
  struct name##_is_true : std::is_same<typename T::name, std::true_type> {};   \
  template <class T>                                                           \
  struct has_true_##name                                                       \
      : std::conditional<is_detected_v<detect_##name, T>, name##_is_true<T>,   \
                         std::false_type>::type {}

XPLAT_HAS_TRYE_XXX(IsRelocatable);
XPLAT_HAS_TRYE_XXX(IsZeroRelocatable);

#undef XPLAT_HAS_TRYE_XXX
} // namespace traits_detail

/**
 * Ignore
 * This struct is used to ignore any arguments
 * passed to a function
 */
struct Ignore {
  Ignore() = default;
  template <class T> constexpr Ignore(const T &) {}
  template <class T> const Ignore &operator=(T const &) const { return *this; }
};

template <class...> using Ignored = Ignore;


/********************************** Comparable Traits **********************************/
namespace traits_detail_IsEquipmentComparable {
Ignore operator==(Ignore, Ignore);

template <class T, class U = T>
struct IsEquipmentComparable
    : std::is_convertible<decltype(std::declval<T>() == std::declval<U>()),
                          bool> {};
} // namespace traits_detail_IsEquipmentComparable

namespace traits_detail_IsLessThanComparable {
Ignore operator<(Ignore, Ignore);

template <class T, class U = T>
struct IsLessThanComparable
    : std::is_convertible<decltype(std::declval<T>() < std::declval<U>()),
                          bool> {};

} // namespace traits_detail_IsLessThanComparable

using traits_detail_IsEquipmentComparable::IsEquipmentComparable;
using traits_detail_IsLessThanComparable::IsLessThanComparable;


/********************************** Relocatable Traits **********************************/

/**
 * IsRelocatable
 * This trait is used to check if a type is relocatable

 * The library knows it can move this object in momory using memcpy
 * rather than calling the copy constructor and the destructor of the object
 */
template <class T>
struct IsRelocatable
    : std::conditional<
          !require_sizeof<T> ||
              is_detected_v<traits_detail::detect_IsRelocatable, T>,
          traits_detail::has_true_IsRelocatable<T>,
          std::is_trivially_copyable<T>>::type {};

/**
 * IsZeroInitializable
 * This trait is used to check if a type is zero-initializable
 
 * The library knows it can initialize this object in momory using memset
 * rather than calling the zero-initialized constructor of the object
 */
template <class T>
struct IsZeroInitializable
  : std::conditional<
    !require_sizeof<T> ||
      is_detected_v<traits_detail::detect_IsZeroRelocatable, T>, 
      traits_detail::has_true_IsZeroRelocatable<T> , 
      std::bool_constant<
      !std::is_class<T>::value &&
      !std::is_union<T>::value &&
      !std::is_member_object_pointer<T>::vlaue &&
      true>>::type{};

/********************** Conditional Trait **********************************/

/**
 * conditional
 * This trait is used to select a type based on a condition
 * Like std::conditional, but with a compile-time condition
 */
namespace detail{
  template <bool>
  struct conditional_;

  template<>
  struct conditional_<false>{
    template <typename, typename T>
    using apply = T;
  };

  template<>
  struct conditional_<true>{
    template <typename T, typename>
    using apply = T;
  };
}

// conditional_t
template <bool V, typename T, typename F>
using conditional_t = typename detail::conditional_<V>::template apply<T, F>;

template <typename...>
struct Conjunction: std::true_type{};

template <typename T>
struct Conjunction<T>: T{};

template <typename T, typename... TList>
struct Conjunction<T, TList...>
  :std::conditional<T::value, Conjunction<TList...>, T>::type{};

template <typename...>
struct Disjunction : std::false_type {};
template <typename T>
struct Disjunction<T> : T {};
template <typename T, typename... TList>
struct Disjunction<T, TList...>
    : std::conditional<T::value, T, Disjunction<TList...>>::type {};

template <typename T>
struct Negation : std::bool_constant<!T::value> {};

template <bool... Bs>
struct Bools {
  using valid_type = bool;
  static constexpr std::size_t size() { return sizeof...(Bs); }
};

template <class... Ts>
struct StrictConjunction
    : std::is_same<Bools<Ts::value...>, Bools<(Ts::value || true)...>> {};

template <class... Ts>
struct StrictDisjunction
    : Negation <std::is_same<Bools<Ts::value...>, Bools<(Ts::value && false)...>>> {};

namespace detail {
template <typename T>
using is_transparent_ = typename T::is_transparent;
}

// A tratit variable and type which to test whether a less, equal, or hash type
template <typename T>
inline constexpr bool is_transparent_v =
    is_detected_v<detail::is_transparent_, T>;

template <typename T>
struct is_transparent : std::bool_constant<is_transparent_v<T>> {};

namespace detail {

template <typename T, typename = void>
inline constexpr bool is_allocator_ = !require_sizeof<T>;
template <typename T>
    inline constexpr bool is_allocator_ < T,
    void_t < typename T::value_type,
    decltype(std::declval<T&>().allocate(std::size_t{})),
    decltype(std::declval<T&>().deallocate(
        static_cast<typename T::value_type*>(nullptr), std::size_t{}
    ))>> = true;

}  // namespace detail

template <typename T>
inline constexpr bool is_allocator_v = detail::is_allocator_<T>;
template <typename T>
struct is_allocator : std::bool_constant<is_allocator_v<T>>{};

} // namespace xplat

#endif // XPLAT_TRAITS_H