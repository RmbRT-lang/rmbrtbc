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

extern "C" int fprintf(void * file, char const * fmt, ...);
extern "C" int printf(char const * fmt, ...);

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
