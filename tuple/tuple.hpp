#include <type_traits>
template<std::size_t index, typename Tuple>
struct TupleElement;


template <typename... T>
class Tuple;
// double, int, char : Head = double, Tail = <int, char>
template <typename Head, typename... Tail>
class Tuple<Head, Tail...> : public Tuple<Tail...> {
public:
    Tuple() : Tuple<Tail...>(), value() {}

    Tuple(Head val, Tail... vals) : Tuple<Tail...>(vals...), value(val) {}

    
    using first_type = Head;
    using tail_type = Tuple<Tail...>;

    void swap(Tuple& other) noexcept {
        std::swap(other, *this);
    }
    template <std::size_t n, typename ret>
    constexpr ret& get_field() {
        if constexpr (n == 0) {
            return value;
        } else 
            return tail_type::template get_field<n - 1, ret>();
        
    }

    template <std::size_t n, typename ret>
    constexpr const ret& get_field() const {
        if constexpr (n == 0) {
            return value;
        } else 
            return tail_type::template get_field<n - 1, ret>();
        
    }

    template<std::size_t index, typename Tup>
    friend struct TupleElement;

    // template <std::size_t index, typename ... Types>
    // friend TupleElement<index, Tuple<Types...>>::TypeT& get (Tuple<Types...>& tuple); 

    // template <typename H, typename ... Types>
    // friend H& get (Tuple<Types...>& tuple);
private:
    Head value;    
};

template<>
class Tuple<> { };


template< typename... Types >
Tuple<Types...> make_tuple( Types&&... args ) {
    return Tuple<Types...>(std::forward<Types>(args)...);
}

template <typename... Types>
struct std::tuple_size<Tuple<Types...>> : std::integral_constant<std::std::size_t, sizeof...(Types)> {};

template <typename... Types>
struct std::tuple_size<Tuple<Types...>&> : std::integral_constant<std::std::size_t, sizeof...(Types)> {};

template <typename... Types>
struct std::tuple_size<const Tuple<Types...>> : std::integral_constant<std::std::size_t, sizeof...(Types)> {};
            
template <typename... Types>
struct std::tuple_size<const Tuple<Types...>&> : std::integral_constant<std::std::size_t, sizeof...(Types)> {};

template<typename T>
inline constexpr std::std::size_t tuple_size_v = std::tuple_size<T>::value;



template<typename Head, typename ... Tail> 
struct TupleElement<0, Tuple<Head, Tail...>> {
    using type = Head;
    // using TupleT = Tuple<Head, Tail...>;
};

template <std::size_t index, typename Head, typename ... Tail> 
struct TupleElement<index, Tuple<Head, Tail...>> : public TupleElement<index - 1, Tuple<Tail...>> 
{ };

// template <std::size_t index, typename ... Types>
// typename TupleElement<index, Tuple<Types...>>::TypeT& get (Tuple<Types...>& tuple) 
// {
//     return (static_cast<TupleElement<index, Tuple<Types...>>::TupleT &>(tuple)).value;
// }

template <std::size_t index, typename... Types>
struct std::tuple_element<index, Tuple<Types...>> : TupleElement<index, Tuple<Types...>> {};

template <std::size_t index, typename... Types>
struct std::tuple_element<index, Tuple<Types...>&> : TupleElement<index, Tuple<Types...>> {};

template <std::size_t index, typename... Types>
struct std::tuple_element<index, const Tuple<Types...>> : TupleElement<index, Tuple<Types...>> {};

template <std::size_t index, typename... Types>
struct std::tuple_element<index, const Tuple<Types...>&> : TupleElement<index, Tuple<Types...>> {};

template <std::size_t index, typename... Types>
using tuple_element_t = typename std::tuple_element<index, Types...>::type;



template <std::size_t n>
constexpr decltype(auto) get_from_elem(auto&& t) {
    using type = std::remove_reference_t<tuple_element_t<n, decltype(t)>>;
    return t.template get_field<n, type>();
}

template <typename T, std::size_t n = 0>
constexpr decltype(auto) get_from_elem(auto&& t) {
    using type = std::remove_reference_t<tuple_element_t<n, decltype(t)>>;
    if constexpr (std::is_same_v<type, std::remove_reference_t<T>>) {
        return t.template get_field<n, type>();
    } else {
        return get_from_elem<T, n + 1>(t);
    }
}

template<std::size_t T, typename... Types >
constexpr auto& get(Tuple<Types...>& t) noexcept {
    return get_from_elem<T>(t);
}

template<std::size_t T, typename... Types >
constexpr auto&& get(Tuple<Types...>&& t) noexcept {
    return get_from_elem<T>(t);
}

template<std::size_t T, typename... Types >
constexpr const auto& get(const Tuple<Types...>& t) noexcept {
    return get_from_elem<T>(t);
}

template<std::size_t T, typename... Types >
constexpr const auto&& get(const Tuple<Types...>&& t) noexcept {
    return get_from_elem<T>(t);
}

template<typename T, typename... Types >
constexpr auto& get(Tuple<Types...>& t) noexcept {
    return get_from_elem<T>(t);
}

template<typename T, typename... Types >
constexpr auto&& get(Tuple<Types...>&& t) noexcept {
    return get_from_elem<T>(t);
}

template<typename T, typename... Types >
constexpr const auto& get(const Tuple<Types...>& t) noexcept {
    return get_from_elem<T>(t);
}

template<typename T, typename... Types >
constexpr const auto&& get(const Tuple<Types...>&& t) noexcept {
    return get_from_elem<T>(t);
}



template <std::size_t n = 0, typename... T1, typename... T2>
std::strong_ordering operator<=>(const Tuple<T1...>& t1, const Tuple<T2...>& t2) {
    if constexpr (n >= std::min(std::tuple_size<decltype(t1)>::value, std::tuple_size<decltype(t2)>::value)) {
        return std::tuple_size<decltype(t1)>::value <=> std::tuple_size<decltype(t2)>::value;
    } else {
        if (get<n>(t1) != get<n>(t2)) {
            return get<n>(t1) <=> get<n>(t2);
        } else {
            return operator<=><n + 1>(t1, t2);
        }
    }
}

template <typename... T1, typename... T2>
auto operator==(const Tuple<T1...>& t1, const Tuple<T2...>& t2) {
  return (t1 <=> t2) == 0;
}
template <std::size_t n = 0, typename... Args>
auto& operator>>(std::istream& in, Tuple<Args...>& t) {
    if constexpr (n < std::tuple_size_v<decltype(t)>) {
        in >> get<n>(t);
        operator>> <n + 1>(in, t);
    }
    return in;
}

template <std::size_t n = 0, typename... Args>
auto& operator<<(std::ostream& out, Tuple<Args...>& t) {
    if constexpr (n < std::tuple_size_v<decltype(t)>) {
        out << get<n>(t);
        if constexpr (n + 1 < std::tuple_size_v<decltype(t)>) {
            out << ' ';
        }
        operator<< <n + 1>(out, t);
    }
    return out;
}




template <std::size_t n, typename... T>
auto add_elem(Tuple<T...>) {
    return Tuple<T..., std::integral_constant<std::size_t, n>>();
}

template <std::size_t n>
struct n_list {
    using type = decltype(add_elem<n - 1>(typename n_list<n - 1>::type()));
};

template <>
struct n_list<0> {
    using type = Tuple<>;
};

template <typename... T1, typename... T2, typename... T3, typename... T4>
constexpr auto tuple_cat_for_two(const Tuple<T1...>& t1, const Tuple<T2...>& t2, Tuple<T3...>, Tuple<T4...>) {
    return Tuple<T1..., T2...>(get<T3::value>(t1)..., get<T4::value>(t2)...);
}

template <typename... T1, typename... T2>
constexpr auto operator+(const Tuple<T1...>& t1, const Tuple<T2...>& t2) {
    return tuple_cat_for_two(t1, t2, typename n_list<sizeof...(T1)>::type()
                                    ,typename n_list<sizeof...(T2)>::type());
}
constexpr auto tuple_cat(auto&&... t) {
    return (t + ... + Tuple<>());
}
