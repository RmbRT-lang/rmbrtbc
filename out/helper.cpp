// Helper code starts here.

// Some standard includes.
#include <inttypes.h>
#include <stddef.h>
#include <new>

// Helpers for implementing language features.
namespace __rl
{
	template<class Ret, class ...Args>
	using function_t = Ret(Args...);

	template<class Ret, size_t kSize>
	using array = Ret[kSize];
	template<class Ret>
	using unsized_array = Ret[];

	struct voidthrow_t {};

	nullptr_t const null {};

	template<class T, class ...Args>
	inline void __rl_constructor(
		T &self,
		Args&&...args)
	{
		new (&self) T(std::forward<Args>(args)...);
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
}

// Helpers for TEST.

#define __RL_TEST(name) __RL_TEST_IMPL(name, __COUNTER__)
#define __RL_TEST_IMPL_PASTE(a,b) a##b
#define __RL_TEST_IMPL(name, counter) \
void __RL_TEST_IMPL_PASTE(__rl_test_, counter)(); \
	int __RL_TEST_IMPL_PASTE(_, counter) = \
		::__rl::test::detail::test(name, &__RL_TEST_IMPL_PASTE(__rl_test_, counter)); \
	void __RL_TEST_IMPL_PASTE(__rl_test_, counter)()

extern void * stdout;
extern void * stderr;


extern "C" int fprintf(void * file, char const * fmt, ...);
extern "C" int printf(char const * fmt, ...);


namespace __rl::test
{
	namespace detail {
		int successes = 0;
		int failures = 0;
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
				fprintf(stdout, "SUCCESS \"%s\"\n", name);
				++detail::successes;
				return 1;
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
typedef char __rl_char;
#define char __rl_char
typedef int __rl_int;
#define int __rl_int
typedef short __rl_short;
#define short __rl_short
typedef long __rl_long;
#define long __rl_long
typedef float __rl_float;
#define float __rl_float
typedef double __rl_double;
#define double __rl_double
typedef bool __rl_bool;
#define bool __rl_bool

#define TRUE true
#define FALSE false

#define ASSIGN operator=
#define LOG_NOT operator!
#define SUBSCRIPT operator[]
#define NOTYPE
#define CONVERT(...) operator __VA_ARGS__ ()

typedef ::nullptr_t NULL_t;

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
