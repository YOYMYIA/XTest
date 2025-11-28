#ifndef XPLAT_GEN_CORE_INL_H
#define XPLAT_GEN_CORE_INL_H
#include <type_traits>
#include <utility>

// #ifndef XPLAT_GEN_CORE_H
// #error This file may only be included from Gen/Core.h
// #endif

namespace xplat
{
    namespace gen
    {

        /**
         * @brief Trait to check if a candidate type is compatible with an expected type
         * Matches an expected signature with a candidate signature.
         * @tparam Candidate The candidate type to check
         * @tparam Expected The expected type to check against
         * @return True if the candidate type is compatible with the expected type, false otherwise
         * @return False if the candidate type is not compatible with the expected type
         */
        template <class Candidate, class Expected>
        class IsCompatibleSignature
        {
        public:
            static constexpr bool value = false;
        };

        /**
         * @brief Trait to check if a candidate type is compatible with an expected signature
         * @tparam Candidate The candidate type to check
         * @tparam ExpectedSignature The expected signature to check against
         */
        template <class Candidate, class ExpectedReturn, class... ArgTypes>
        class IsCompatibleSignature<Candidate, ExpectedReturn(ArgTypes...)>
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
            static constexpr bool testArgs(int *)
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
        struct XBounded
        {
            using SelfType = Self;
            // This is a function to get the self of this class.
            const Self &self() const { return *static_cast<const Self *>(this); }

            Self &self() { return *static_cast<Self *>(this); }
        };

        template <class Self>
        class Operator : public XBounded<Self>
        {
        public:
            /**
             * Must be implemented by child class to compose the source and value into a result generator.
             */
            template <class Source, class Value, class ResultGen = void>
            ResultGen compose(Source &&source, Value &&value) const;

        protected:
            Operator() = default;
            Operator(Operator &&) noexcept = default;
            Operator(const Operator &) = delete;
            Operator &operator=(Operator &&) noexcept = default;
            Operator &operator=(const Operator &) = default;
        };


        /**
         * That is a Core of a generator.
         * implement apply(), foreach() model
         * apply() is used to apply a function to the result generator.
         * foreach() is used to iterate over the result generator.
         * apply() and foreach() are used to modify the result generator.
         */


        template <class Value, class Self>
        class GenImpl : public XBounded<Self>
        {
        protected:
            GenImpl() = default;
            GenImpl(GenImpl &&) = default;
            GenImpl(const GenImpl &) = default;
            GenImpl &operator=(GenImpl &&) = default;
            GenImpl &operator=(const GenImpl &) = default;

        public:
            typedef Value valueType;
            typedef typename std::decay<Value>::type StorageType;

            /**
             * Apply the generator to a handler.
             * Send all values produced by this generator to given handler until the handler returns false.
             */
            template <class Handler>
            bool apply(Handler &&handler) const;

            template <class Body>
            void foreach (Body &&body) const
            {
                this->self().apply(
                    [&](Value value) -> bool
                    {
                        static_assert(!infinite, "infinite generator cannot be used in foreach");
                        body(std::forward<Value>(value));
                        return true;
                    });
            }

            static constexpr bool infinite = false;
        };

        namespace detail
        {
            /**
             * This Class for building up a pipline of operators.
             * This type is usually used to compose two operators together.
             * Example:
             * auto valuesOf = filter([] (Optional<int> & o) {return o.hasValue();})
             *              | map([] (Optional<int> & o)->int& {return o.value();})
             * auto valuesIncluded = from(optionals) | valuesOf | as<vector>();
             */
            template <class First, class Second>
            class Composed : public Operator<Composed<First, Second>>
            {
                First first_;
                Second second_;

            public:
                Composed() = default;

                Composed(First first, Second second)
                    : first_(std::move(first)), second_(std::move(second)) {}

                template <
                    class Source,
                    class Value,
                    class FirstRet =
                        decltype(std::declval<First>().compose(std::declval<Source>())),
                    class SecondRet =
                        decltype(std::declval<Second>().compose(std::declval<FirstRet>()))>
                SecondRet compose(const GenImpl<Value, Source> &source) const
                {
                    return second_.compose(first_.compose(source.self()));
                }
            };

            template <class Value, class First, class Second>
            class Chain : public GenImpl<Value, Chain<Value, First, Second>>
            {
                First first_;
                Second second_;

            public:
                explicit Chain (First first, Second second)
                    : first_(std::move(first)), second_(std::move(second)) {}

                template <class Handler>
                bool apply(Handler&& handler) const
                {
                    return first_.apply(std::forward<Handler>(handler)) && 
                    second_.apply(std::forward<Handler>(handler));
                }

                template <class Body>
                void foreach(Body&& body) const
                {
                    first_.foreach(std::forward<Body>(body));
                    second_.foreach(std::forward<Body>(body));
                }

                static constexpr bool infinite = First::infinite || Second::infinite;
            };

        } // namespace detail

        /**
         * operator|() For composing two operators without binding it to a particular generator.
         */

        template <
            class Left,
            class Right,
            class Composed = detail::Composed<Left, Right>>
        Composed operator|(const Operator<Left> &left, const Operator<Right> &right)
        {
            return Composed(left.self(), right.self());
        }

        template <
            class Left,
            class Right,
            class Composed = detail::Composed<Left, Right>>
        Composed operator|(const Operator<Left> &left, Operator<Right> &right)
        {
            return Composed(left.self(), std::move(right.self()));
        }

        template <
            class Left,
            class Right,
            class Composed = detail::Composed<Left, Right>>
        Composed operator|(Operator<Left> &left, const Operator<Right> &right)
        {
            return Composed(std::move(left.self()), right.self());
        }

        template <
            class Left,
            class Right,
            class Composed = detail::Composed<Left, Right>>
        Composed operator|(Operator<Left> &left, Operator<Right> &right)
        {
            return Composed(std::move(left.self()), std::move(right.self()));
        }

        template <
            class LeftValue,
            class Left,
            class RightValue,
            class Right,
            class Chain = detail::Chain<LeftValue, Left, Right>>
        Chain operator+(const GenImpl<LeftValue, Left>& left, const GenImpl<RightValue, Right>& right)
        {
            static_assert(std::is_same<LeftValue, RightValue>::value, 
                         "Left and right generators must have the same value type");
            return Chain(left.self(), right.self());
        }

        template <
            class LeftValue,
            class Left,
            class RightValue,
            class Right,
            class Chain = detail::Chain<LeftValue, Left, Right>>
        Chain operator+(const GenImpl<LeftValue, Left>& left, const GenImpl<RightValue, Right>&& right)
        {
            static_assert(std::is_same<LeftValue, RightValue>::value, 
                         "Left and right generators must have the same value type");
            return Chain(left.self(), std::move(right.self()));
        }

        template <
            class LeftValue,
            class Left,
            class RightValue,
            class Right,
            class Chain = detail::Chain<LeftValue, Left, Right>>
        Chain operator+(const GenImpl<LeftValue, Left>&& left, const GenImpl<RightValue, Right>& right)
        {
            static_assert(std::is_same<LeftValue, RightValue>::value, 
                         "Left and right generators must have the same value type");
            return Chain(std::move(left.self()), right.self());
        }

        template <
            class LeftValue,
            class Left,
            class RightValue,
            class Right,
            class Chain = detail::Chain<LeftValue, Left, Right>>
        Chain operator+(const GenImpl<LeftValue, Left>&& left, const GenImpl<RightValue, Right>&& right)
        {
            static_assert(std::is_same<LeftValue, RightValue>::value, 
                         "Left and right generators must have the same value type");
            return Chain(std::move(left.self()), std::move(right.self()));
        }


        /**
         * operator|() which enables foreach-like usage:
         *   gen | [](Value v) -> void {...};
         */

        template <class Value, class Gen, class Handler> 
        typename std::enable_if<
        IsCompatibleSignature<Handler, void(Value)>::value>::type
        operator|(const GenImpl<Value, Gen>& gen, Handler&& handler)
         {
            static_assert(
                !Gen::infinite, "Cannot pull all values from an infinite sequence.");
            gen.self().foreach(std::forward<Handler>(handler));
        }


        /**
         * operator|() which enables foreach-like usage with 'break' support:
         *   gen | [](Value v) -> bool { return shouldContinue(); };
         */

        template <class Value, class Gen, class Handler>
        typename std::
            enable_if<IsCompatibleSignature<Handler, bool(Value)>::value, bool>::type
            operator|(const GenImpl<Value, Gen>& gen, Handler&& handler) 
        {
            return gen.self().apply(std::forward<Handler>(handler));
        }


        /**
         * operator|() for composing generators with operators, similar to boosts' range
         * adaptors:
         *   gen | map(square) | sum
         */
        template <class Value, class Gen, class Op>
        auto operator|(const GenImpl<Value, Gen>& gen, const Operator<Op>& op)
            -> decltype(op.self().compose(gen.self())) 
        {
            return op.self().compose(gen.self());
        }

        template <class Value, class Gen, class Op>
        auto operator|(GenImpl<Value, Gen>&& gen, const Operator<Op>& op)
            -> decltype(op.self().compose(std::move(gen.self()))) 
        {
            return op.self().compose(std::move(gen.self()));
        }

    } // namespace gen
} // namespace xplat

#endif // XPLAT_GEN_CORE_INL_H