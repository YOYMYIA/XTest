#ifndef XPLAT_GEN_BASE_H
#define XPLAT_GEN_BASE_H

//#include <type_traits>
#include <utility>

namespace xplat
{

namespace gen
{

class Less 
{
public:
    template <class First, class Second>
    auto operator()(const First& first, const Second& second) const
        -> decltype(first < second)
    {
        return first < second;
    }
};

class Greater
{
public:
    template <class First, class Second>
    auto operator()(const First& first, const Second& second) const
        -> decltype(first > second)
    {
        return first > second;
    }
};

template <int n>
class Get
{
public:
    template <class Value>
    auto operator()(Value && value) const
        -> decltype(std::get<n>(std::forward<Value>(value)))
    {
        return std::get<n>(std::forward<Value>(value));
    }
};

template <class Class, class Result>
class MemberFunction
{
public:
    using MemberPtr = Result (Class::*)(); // member function pointer type

private:
    MemberPtr member_;

public:
    explicit MemberFunction(MemberPtr menber) : member_(menber) {}

    Result operator()(Class && object) const{return (object.*member_)();}
    Result operator()(Class & object) const{return (object.*member_)();}
    Result operator()(Class* object) const {return (object->*member_)();}
};

template <class Class, class Result>
class ConstMemberFunction
{
public:
    using MemberPtr = Result (Class::*)() const; // member function pointer type

private:
    MemberPtr member_;

public:
    explicit ConstMemberFunction(MemberPtr menber) : member_(menber) {}
    
    Result operator()(Class const & object) const{return (object.*member_)();}
    Result operator()(Class* object) const {return (object->*member_)();}
};

template <class Class, class FieldType>
class Field
{
public:
    using FieldPtr = FieldType Class::*; // field pointer type

private:
    FieldPtr field_;

public:
    explicit Field(FieldPtr field) : field_(field) {}

    FieldType operator()(Class && object) const{return std::move(object.*field_);}
    FieldType operator()(Class & object) const{return object.*field_;}
    FieldType operator()(Class* object) const {return object->*field_;}
    const FieldType operator()(const Class & object) const{return object.*field_;}
    const FieldType operator()(const Class* object) const {return object->*field_;}
};

class Move
{
public:
    template <class Value>
    auto operator()(Value && value) const
        -> decltype(std::move(std::forward<Value>(value)))
    {
        return std::move(std::forward<Value>(value));
    }
};

/**
    @brief Negate a predicate
    @tparam Predicate The predicate to negate
*/

template <class Predicate>
class Negate
{

    Predicate predicate_;
public:
    Negate() = default;
    Negate(Predicate predicate) : predicate_(predicate) {}
    
    template<class T>
    bool operator()(T && value) const
    {
        return !predicate_(std::forward<T>(value));
    }

};

template <class Predicate>
Negate<Predicate> negate(Predicate predicate)
{
    return Negate<Predicate>(std::move(predicate));
}

} //namespace gen

} //namespace xplat


#endif