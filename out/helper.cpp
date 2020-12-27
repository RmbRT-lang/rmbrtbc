// Helper code starts here.

// Some standard includes.
#include <inttypes.h>
#include <stddef.h>
#include <new>
#include <coroutine>
#include <variant>
#include <exception>

#include <coroutine>
#include <exception>
#include <future>
#include <thread>
#include <type_traits>
#include <tuple>

// Enable the use of std::future<T> as a coroutine type
// by using a std::promise<T> as the promise type.
template <typename T, typename... Args>
requires(!std::is_void_v<T> && !std::is_reference_v<T>)
struct std::coroutine_traits<std::future<T>, Args...> {
	struct promise_type : std::promise<T> {
		std::future<T> get_return_object() noexcept {
			return this->get_future();
		}

		std::suspend_never initial_suspend() const noexcept { return {}; }
		std::suspend_never final_suspend() const noexcept { return {}; }

		void return_value(const T &value)
		noexcept(std::is_nothrow_copy_constructible_v<T>) {
			this->set_value(value);
		}
		void return_value(T &&value)
		noexcept(std::is_nothrow_move_constructible_v<T>) {
			this->set_value(std::move(value));
		}
		void unhandled_exception() noexcept {
			this->set_exception(std::current_exception());
		}
	};
};

// Same for std::future<void>.
template <typename... Args>
struct std::coroutine_traits<std::future<void>, Args...> {
	struct promise_type : std::promise<void> {
		std::future<void> get_return_object() noexcept {
			return this->get_future();
		}

		std::suspend_never initial_suspend() const noexcept { return {}; }
		std::suspend_never final_suspend() const noexcept { return {}; }

		void return_void() noexcept {
			this->set_value();
		}
		void unhandled_exception() noexcept {
			this->set_exception(std::current_exception());
		}
	};
};

// Allow co_await'ing std::future<T> and std::future<void>
// by naively spawning a new thread for each co_await.
template <typename T>
auto operator co_await(std::future<T> &&future) noexcept
	requires(!std::is_reference_v<T>) {
	struct awaiter : std::future<T> {
		bool await_ready() const noexcept {
			using namespace std::chrono_literals;
			return this->wait_for(0s) != std::future_status::timeout;
		}
		void await_suspend(std::coroutine_handle<> cont) const {
			std::thread([this, cont] {
			this->wait();
			cont();
			}).detach();
		}
		T await_resume() { return this->get(); }
	};
	return awaiter{std::move(future)};
}

template<class T>
auto operator co_await(std::future<T> &future) noexcept {
	return operator co_await(std::move(future));
}

// Helpers for implementing language features.
namespace __rl
{
	template<class Ret, class ...Args>
	using function_t = Ret(Args...);

	template<class PEnum, class IEnum>
	class EnumConstant {
	public:
		IEnum __rl_v;
		typedef ::std::underlying_type_t<IEnum> int_t;
		constexpr EnumConstant(IEnum v): __rl_v(v)
		{}
		constexpr operator IEnum() const { return __rl_v; }
		constexpr operator int_t() const { return (int_t) __rl_v; }
	};

	template<class T>
	struct auto_type_wrapper { typedef T type; };
	template<class PEnum, class IEnum>
	struct auto_type_wrapper<EnumConstant<PEnum, IEnum>> {
		typedef std::remove_pointer_t<PEnum> type;
	};
	template<class PEnum, class IEnum>
	struct auto_type_wrapper<EnumConstant<PEnum, IEnum> &> {
		typedef std::remove_pointer_t<PEnum> type;
	};
	template<class PEnum, class IEnum>
	struct auto_type_wrapper<EnumConstant<PEnum, IEnum> &&> {
		typedef std::remove_pointer_t<PEnum> type;
	};
	template<class PEnum, class IEnum>
	struct auto_type_wrapper<EnumConstant<PEnum, IEnum> const &> {
		typedef std::remove_pointer_t<PEnum> type;
	};
	template<class PEnum, class IEnum>
	struct auto_type_wrapper<EnumConstant<PEnum, IEnum> const &&> {
		typedef std::remove_pointer_t<PEnum> type;
	};

	template<class T>
	using auto_t = auto_type_wrapper<T>::type;
	template<class T>
	auto_t<T &&> mk_auto(T &&v) { return static_cast<auto_t<T &&>>(v); }

	template<class Ret, size_t kSize>
	using array = Ret[kSize];
	template<class Ret>
	using unsized_array = Ret[];

	struct voidthrow_t {};

	nullptr_t const null {};

	template<class T, class ...Args>
	inline T __rl_cast(Args&&...args)
	{
		return T(std::forward<Args>(args)...);
	}

	template<class T, class ...Args>
	inline void __rl_constructor(
		T &self,
		Args&&...args)
	{
		new (&self) T(std::forward<Args>(args)...);
	}
	template<class T, class ...Args>
	inline void __rl_p_constructor(
		T *self,
		Args&&...args)
	{
		new (self) T(std::forward<Args>(args)...);
	}

	template<class T>
	inline void __rl_destructor(T &v)
	{
		v.~T();
	}
	template<class T>
	inline void __rl_destructor(T const& v)
	{
		v.~T();
	}

	template<class T>
	inline T & deref(T &v) { return v; }
	template<class T>
	inline T const& deref(T const& v) { return v; }
	template<class T>
	inline T && deref(T &&v) { return (T&&)v; }
	template<class T>
	inline T const&& deref(T const&& v) { return (T const&&)v; }
	template<class T>
	inline auto deref(T * v) -> decltype(deref(*v)) { return deref(*v); }
	template<class T>
	inline auto deref(T const * v) -> decltype(deref(*v)) { return deref(*v); }

	template<class Type, class Enum, size_t count>
	class EnumWrapper
	{
	protected:
		typedef ::std::underlying_type_t<Enum> int_t;
		typedef ::std::make_unsigned_t<int_t> uint_t;

		Enum value;
	public:
		constexpr EnumWrapper(EnumConstant<Type *, Enum> v):
			value(v.__rl_v) { }
		constexpr EnumWrapper(Enum v):
			value(v) { }
		explicit constexpr EnumWrapper(int_t v):
			value((Enum)v) { }
		constexpr EnumWrapper() = default;

		constexpr operator int_t() const {
			return (int_t) value;
		}
		static constexpr size_t size() { return count; }
		constexpr bool valid() const
		{
			return (uint_t)value < (uint_t)count;
		}
		constexpr bool operator==(Type const& rhs) const
		{
			return (int_t)static_cast<EnumWrapper<Type, Enum, count> const&>(rhs).value
				== (int_t)value;
		}
		constexpr bool operator!=(Type const& rhs) const
		{
			return (int_t)static_cast<EnumWrapper<Type, Enum, count> const&>(rhs).value
				!= (int_t)value;
		}
		constexpr bool operator<(Type const& rhs) const
		{
			return (int_t)static_cast<EnumWrapper<Type, Enum, count> const&>(rhs).value
				< (int_t)value;
		}
		constexpr bool operator>(Type const& rhs) const
		{
			return (int_t)static_cast<EnumWrapper<Type, Enum, count> const&>(rhs).value
				> (int_t)value;
		}
		constexpr bool operator<=(Type const& rhs) const
		{
			return (int_t)static_cast<EnumWrapper<Type, Enum, count> const&>(rhs).value
				<= (int_t)value;
		}
		constexpr bool operator>=(Type const& rhs) const
		{
			return (int_t)static_cast<EnumWrapper<Type, Enum, count> const&>(rhs).value
				>= (int_t)value;
		}
	};


	class CreateTuple {};
	CreateTuple const createTuple;

	template<class ...Types>
	class Tuple : public std::tuple<Types...>
	{
	public:
		using std::tuple<Types...>::operator=;

		inline Tuple()
		{
			static_assert((std::is_default_constructible_v<Types> && ...));
		}

		template<class ...Types2>
		inline Tuple(
			std::enable_if_t<
				(...&&std::is_constructible_v<Types, Types2&&>),
				CreateTuple>,
			Types2&&... args):
			std::tuple<Types...>(static_cast<Types2&&>(args)...)
		{
		}

		template<class ...Types2>
		inline Tuple(Tuple<Types2...> const& rhs):
			std::tuple<Types...>(static_cast<std::tuple<Types2...> const&>(rhs))
		{
			static_assert(std::is_constructible_v<std::tuple<Types...>, std::tuple<Types2...> const&>);
		}
		template<class ...Types2>
		inline Tuple(Tuple<Types2...> && rhs):
			std::tuple<Types...>(static_cast<std::tuple<Types2...> &&>(rhs))
		{
			static_assert(std::is_constructible_v<std::tuple<Types...>, std::tuple<Types2...> &&>);
		}
	};

	template<class ...Types>
	Tuple<Types &&...> mk_tuple(Types&&...values) {
		return Tuple<Types&&...>(createTuple, static_cast<Types&&>(values)...);
	}

	template<class ...Types>
	struct auto_type_wrapper<Tuple<Types...>> {
		typedef Tuple<auto_t<::std::decay_t<Types>>...> type;
	};
	template<class ...Types>
	struct auto_type_wrapper<Tuple<Types...> &&> {
		typedef Tuple<auto_t<::std::decay_t<Types>>...> type;
	};
	template<class ...Types>
	struct auto_type_wrapper<Tuple<Types...> const&> {
		typedef Tuple<auto_t<::std::decay_t<Types>>...> type;
	};

	template<class Symbol>
	struct SymbolBase
	{
		template<class T, class ...Ts>
		inline Tuple<Symbol, T&&, Ts&&...> operator()(T&& arg, Ts&&...args) const {
			return mk_tuple(Symbol(), std::forward<T>(arg), std::forward<Ts>(args)...);
		}
	};

	struct TupleCtorHelper {} const tupleCtorHelper {};
}
#define __rl_assert_stringify_code(code...) #code
#define __rl_assert(expr, code, file, line, col) do { \
	if(!(expr)) \
		throw file ":" #line ":" #col ": assertion failed: '" __rl_assert_stringify_code code "'"; \
} while(0)

// Helpers for TEST.

#define __RL_TEST(name) __RL_TEST_IMPL(name, __COUNTER__)
#define __RL_TEST_IMPL_PASTE(a,b) a##b
#define __RL_TEST_IMPL(name, counter) \
void __RL_TEST_IMPL_PASTE(__rl_test_, counter)(); \
	int __RL_TEST_IMPL_PASTE(_, counter) = \
		::__rl::test::detail::test(name, &__RL_TEST_IMPL_PASTE(__rl_test_, counter)); \
	void __RL_TEST_IMPL_PASTE(__rl_test_, counter)()

namespace __rl::test
{
	namespace detail {
		int successes = 0;
		int failures = 0;

		extern "C" void * stderr;
		extern "C" int fprintf(void * file, char const * fmt, ...);
	}

	void status(int &successes, int &failures)
	{
		successes = detail::successes;
		failures = detail::failures;
	}

	namespace detail
	{
		int test(char const * name, void (*test_fn)())
		{
			try {
				test_fn();
				++detail::successes;
				return 1;
			} catch(char const * e)
			{
				fprintf(stderr, "FAILURE \"%s\":\n%s\n", name, e);
			} catch(...)
			{
				fprintf(stderr, "FAILURE \"%s\"\n", name);
			}
			++detail::failures;
			return 0;
		}
	}
}



// Helpers for new/delete.

template<class T, class ...Args>
inline T * __rlc_new(
	Args&&... args)
{
	return new T(
		std::forward<Args>(args)...);
}

template<class T>
inline void __rlc_delete(T * p)
{
	delete p;
}

template<class T, ::size_t kSize>
constexpr size_t  size(T (&)[kSize]) { return kSize; }

// Declare special size types.
typedef unsigned long int UM;
typedef signed long int SM;

// Allow some C++ keywords to be used as identifiers.
typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef char __rlc_char;
typedef int __rlc_int;
typedef short __rlc_short;
typedef long __rlc_long;
typedef float __rlc_float;
typedef double __rlc_double;
typedef bool __rlc_bool;

#define TRUE true
#define FALSE false

typedef ::nullptr_t NULL_t;
const NULL_t __rlc_NULL{};

#define __rl_unnamed __rl_unnamed_impl(__COUNTER__)
#define __rl_unnamed_impl(x) __rl_unnamed_paste(x)
#define __rl_unnamed_paste(x) __rl_unnamed_ ## x

#define __rl_do_while_loop(postloopstmt) \
	__rl_do_while_loop_impl(postloopstmt, __COUNTER__)

#define __rl_do_while_loop_impl_paste(a, b) a##b

#define __rl_do_while_loop_impl(postloopstmt, counter) \
	goto __rl_do_while_loop_impl_paste(__rlc_skip_,counter); \
	do { \
		postloopstmt; \
	__rl_do_while_loop_impl_paste(__rlc_skip_,counter):


#define __cpp_std std

// Generated code starts here.
