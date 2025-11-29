#ifndef XPLAT_TRAITS_H
#define XPLAT_TRAITS_H

#include <type_traits>
#include <tuple>


namespace xplat
{

    //C++ 20 std::type_identify && std::type_identity_t
    template <typename T>
    struct type_identify
    {
        using type = T;
    };

    template <typename T>
    using type_identity_t = typename type_identify<T>::type;

    //define a type-list; A generic value type and value
    template <typename...>
    struct tag_t{};
    template <typename... T>
    inline constexpr tag_t<T...> tag{};


    // A value-list is a list of values of the same type
    template <auto...>
    struct vtag_t{};
    template <auto... V>
    inline constexpr vtag_t<V...> vtag{};

    template <std::size_t I>
    using index_constant = std::integral_constant<std::size_t, I>;

    namespace detail
    {
        template <typename Int>
        constexpr Int parse_uic(char const* str) noexcept
        {
            Int result = 0;
            while(*str)
            {
                auto const c = *str++;
                if(c >= '0' && c <= '9')
                {
                    result = result * 10 + (c - '0');
                }
            }
            return result;
        }
        
    }// namespace detail

    inline namespace literals
    {
        // C++ 20 std::integer_constant_literals
        // Define a user-defined literal operator for integer constants
        // The operator takes a string of digits and returns an index_constant
        inline namespace integer_constant_literals
        {
            template <char... Digits>
            constexpr auto operator""_uzic() noexcept
            {
                constexpr char digits[] = {Digits..., '\0'};
                return index_constant<detail::parse_uic<std::size_t>(digits)>{};
            }
        }
    }



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
    template <typename...>
    inline constexpr bool always_false = false;

    namespace detail 
    {
        template < typename Void, typename T>
        struct require_sizeof_
        {
            static_assert(always_false<T>, 
            "This type is incomplete");
        };

        template <typename T>
        struct require_sizeof_<decltype(void(sizeof(T))),T>
        {
            template <typename V>
            using apply_t = V;

            static constexpr std::size_t size = sizeof(T);
        };
    } // namespace detail

    // require_sizeof
    template <typename T>
    static constexpr std::size_t require_sizeof = detail::require_sizeof_<void, T>::size;

    // is_complete
    template <typename T>
    struct is_complete
    {
        static_assert(always_false<T>, "This type is incomplete");    
    };

    template <typename T>
    constexpr bool is_complete_v = is_complete<T>::value;

    // is_unbounded_arrray_v
    template <typename T>
    inline constexpr bool is_unbounded_array_v = false;
    template <typename T>
    inline constexpr bool is_unbounded_array_v<T[]> = true;
    template <typename T>
    struct is_unbounded_array : std::bool_constant<is_unbounded_array_v<T>> {};

    // is_bounded_array_v
    template <typename T>
    inline constexpr bool is_bounded_array_v = false;
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
    struct is_instantiation_of : std::bool_constant<is_instantiation_of_v<C, T...>> {};

    // member_pointer_traits
    // Works for both member-object-pointer and member-function-pointer.
    template <typename T>
    struct member_pointer_traits;
    template <typename M, typename O>
    struct member_pointer_traits<M O::*>
    {
        using member_type = M;
        using object_type = O;
    };

    template <typename P>
    using member_pointer_member_t = typename member_pointer_traits<P>::member_type;
    template <typename P>
    using member_pointer_object_t = typename member_pointer_traits<P>::object_type;


    namespace detail
    {
        struct is_constexpr_default_constructible_
        {
            template <typename T>
            static constexpr auto make(tag_t<T>)->decltype(void(T()), 0)
            {
                return (void(T()), 0);
            }
        };

        // template <typename T, int = make(tag<T>)>
        // static std::true_type sfinae(T*);
        // static std::false_type sfinae(void*);
        // template <typename T>
        // static constexpr bool apply =
        //     !require_sizeof<T> || decltype(sfinae(static_cast<T*>(nullptr)))::value;
    } // namespace detail

    /**
     * nonesuch
     * A tag type which traits may use to indicate lack of a result type.
     * Similar to void but Different that no functions may be defined with this return type
     * and no complete expressions may evaluate with this expression type.
    */
    struct nonesuch {
        ~nonesuch() = delete;
        nonesuch(nonesuch const&) = delete;
        void operator=(nonesuch const&) = delete;
    };

    namespace detail
    {
        template <typename Void, typename D, template <typename...> class, typename...>
        struct detected_
        {
            using value_t = std::false_type;
            using type = D;
        };

        template <typename D, template <typename...> class T, typename... A>
        struct detected_<std::void_t<A...>, D, T, A...>
        {
            using value_t = std::true_type;
            using type = T<A...>;
        };
    };

    template <typename D, template <typename...> class T, typename... A>
    using detected_or = detail::detected_<void, D, T, A...>;

    template <typename D, template <typename...> class T, typename... A>
    using detected_or_t = typename detected_or<D, T, A...>::type;

    template < template <typename...> class T, typename... A>
    using detected_t = detected_or<nonesuch, T, A...>;

    template <template <typename...> class T, typename... A>
    inline constexpr bool is_detected_v =
    detected_or<nonesuch, T, A...>::value_t::value;

    template <template <typename...> class T, typename... A>
    struct is_detected : detected_or<nonesuch, T, A...>::value_t {};
}

#endif // XPLAT_TRAITS_H