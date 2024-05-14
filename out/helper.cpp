// Helper code starts here.

// Some standard includes.
#include <inttypes.h>
#include <stddef.h>
#include <new>
#include <coroutine>
#include <variant>
#include <exception>

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

	template<class T>
	class Deferrer
	{
		T m_fn;
	public:
		inline Deferrer(T const& fn):
			m_fn(fn)
		{
		}
		inline ~Deferrer()
		{
			m_fn();
		}
	};

	inline void sleep_thread() { std::this_thread::yield(); }
	inline void sleep_thread(double seconds) {
		std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
	}

	inline auto sleep_coroutine() {
		struct yielder {
			bool await_ready() { return false; }
			void await_suspend(std::coroutine_handle<> cont) const {
				std::thread([cont] {
					cont();
				}).detach();
			}
			void await_resume() {}
		};
		return yielder{};
	}

	inline auto sleep_coroutine(double seconds) {
		struct sleeper {
			double m_seconds;
			sleeper(double seconds): m_seconds(seconds)
			{}

			bool await_ready() { return m_seconds <= 0; }
			void await_suspend(std::coroutine_handle<> cont) const {
				std::thread([this, cont] {
					std::this_thread::sleep_for(std::chrono::duration<double>(m_seconds));
					cont();
				}).detach();
			}
			void await_resume() {}
		};

		return sleeper{seconds};
	}

	struct timeout_throw_t{};

	template<class Result>
	class ProcessHandle : std::future<Result>
	{
		std::mutex m;
		Result * m_result;

		Result & get_result()
		{
			if(!m_result)
			{
				if(!this->valid())
					throw std::runtime_error("invalid process handle.");
				m_result = new Result(this->get());
			}
			return *m_result;
		}
	public:
		ProcessHandle(std::future<Result> &&f):
			std::future<Result>(std::move(f)),
			m_result(nullptr)
		{
			if(!this->valid())
				throw std::runtime_error("invalid process handle.");
		}
		ProcessHandle(ProcessHandle&& move):
			m_result(nullptr)
		{
			*this = std::move(move);
		}
		~ProcessHandle() { if(m_result) delete m_result; }

		ProcessHandle<Result> &operator=(ProcessHandle<Result> &&mv)
		{
			if(this == &mv)
				return *this;
			std::lock_guard l2{m};
			if(this->valid())
				throw std::runtime_error("cannot assign to non-empty process handle");
			std::lock_guard l{mv.m};

			if(this->m_result)
				delete(this->m_result);
			this->m_result = mv.m_result;
			static_cast<std::future<Result> &>(*this) = std::move(mv);
			mv.m_result = nullptr;
			return *this;
		}

		inline operator bool() const
		{
			std::lock_guard l{m};
			if(!this->valid())
				throw std::runtime_error("invalid process handle.");
			using namespace std::chrono_literals;
			return this->wait_for(0s) != std::future_status::timeout;
		}
		inline bool operator[](double seconds) const
		{
			std::lock_guard l{m};
			if(!this->valid())
				throw std::runtime_error("invalid process handle.");
			return this->wait_for(std::chrono::duration<double>(seconds)) != std::future_status::timeout;
		}

		inline Result &operator()()
		{
			std::lock_guard l{m};
			return get_result();
		}

		Result &operator()(double seconds)
		{
			if(!(*this)[seconds])
				throw timeout_throw_t{};
			std::lock_guard l{m};
			return get_result();
		}

		struct __rl_identifier {};
		void const * __rl_get_derived(__rl_identifier const *) const
		{ return this; }
	};

	template<>
	class ProcessHandle<void> : std::future<void>
	{
	public:
		ProcessHandle(std::future<void> &&f): std::future<void>(std::move(f))
		{
		}

		inline operator bool() const
		{
			using namespace std::chrono_literals;
			return this->wait_for(0s) != std::future_status::timeout;
		}
		bool operator[](double seconds) const
		{
			return this->wait_for(std::chrono::duration<double>(seconds)) != std::future_status::timeout;
		}

		inline void operator()()
		{
			this->get();
		}
		inline void operator()(double seconds) {
			if(!(*this)[seconds])
				throw timeout_throw_t{};
		}
	};

	template<class Fn, class ...Args>
	[[nodiscard]]
	inline auto spawn_process(Fn &&fn, Args&&...args) ->
		ProcessHandle<
			std::invoke_result_t<std::decay_t<Fn>, std::decay_t<Args>...>>
	{
		return std::async(
			std::launch::async,
			std::forward<Fn>(fn), std::forward<Args>(args)...);
	}

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

	struct voidthrow_t {};

	nullptr_t const null {};

	struct default_init_t
	{
		constexpr operator bool() const { return false; }
		constexpr operator char() const { return 0; }
		constexpr operator float() const { return 0; }
		constexpr operator double() const { return 0; }
		constexpr operator uint8_t() const { return 0; }
		constexpr operator uint16_t() const { return 0; }
		constexpr operator uint32_t() const { return 0; }
		constexpr operator uint64_t() const { return 0; }
		constexpr operator int8_t() const { return 0; }
		constexpr operator int16_t() const { return 0; }
		constexpr operator int32_t() const { return 0; }
		constexpr operator int64_t() const { return 0; }
		template<class T> constexpr operator T*() const { return NULL; }
	} const default_init;


	struct bare_init_t
	{
		constexpr operator bool() const { bool v; return v; }
		constexpr operator char() const { char v; return v; }
		constexpr operator float() const { float v; return v; }
		constexpr operator double() const { double v; return v; }
		constexpr operator uint8_t() const { uint8_t v; return v; }
		constexpr operator uint16_t() const { uint16_t v; return v; }
		constexpr operator uint32_t() const { uint32_t v; return v; }
		constexpr operator uint64_t() const { uint64_t v; return v; }
		constexpr operator int8_t() const { int8_t v; return v; }
		constexpr operator int16_t() const { int16_t v; return v; }
		constexpr operator int32_t() const { int32_t v; return v; }
		constexpr operator int64_t() const { int64_t v; return v; }
		template<class T> constexpr operator T*() const { T * v; return v; }
	} const bare_init;

	template<class Ret, size_t kSize>
	class array
	{
	private:
		std::decay_t<Ret> m_entries[kSize];
	public:
		typedef Ret arr_t[kSize];

		static constexpr char const * __rl_type_name_v = "[]";

		constexpr array() {}
		constexpr array(::__rl::default_init_t)
		{
			for(size_t i = 0; i < kSize; i++)
				new (&m_entries[i]) Ret(::__rl::default_init);
		}
		constexpr array(::__rl::bare_init_t)
		{
			for(size_t i = 0; i < kSize; i++)
				new (&m_entries[i]) Ret(::__rl::bare_init);
		}
		template<class ...Args>
		constexpr array(Args&&...args): m_entries{std::forward<Args>(args)...}
		{
		}

		constexpr auto __rl_value_of() const { return m_entries; };
		constexpr auto __rl_value_of() -> Ret(&)[] { return m_entries; };

		static constexpr size_t size() { return kSize; }
		constexpr size_t __rl_count() const { return kSize; }

		constexpr Ret &operator[](size_t i) { return m_entries[i]; }
		constexpr Ret const& operator[](size_t i) const { return m_entries[i]; }

		constexpr Ret &operator*() { return m_entries[0]; }
		constexpr Ret const& operator*() const { return m_entries[0]; }
	};

	template<class Ret>
	using unsized_array = Ret[];

	template<class T> inline T __rl_cast() { return ::__rl::default_init; }
	template<class T, class ...Args>
	inline T __rl_cast(Args&&...args)
	{
		return T(std::forward<Args>(args)...);
	}

	template<class T>
	inline void __rl_constructor(T &self)
	{
		new (&self) T(::__rl::default_init);
	}

	template<class T>
	inline void __rl_virtual_constructor(T &self)
	{
		self.__rl_virtual_constructor(static_cast<T::__rl_identifier const *>(nullptr), ::__rl::default_init);
	}

	template<class T>
	inline void __rl_virtual_p_constructor(T * self)
	{
		self->__rl_virtual_constructor(static_cast<T::__rl_identifier const *>(nullptr), ::__rl::default_init);
	}

	template<class T, class ...Args>
	inline void __rl_constructor(
		T &self,
		Args&&...args)
	{
		new (&self) T(std::forward<Args>(args)...);
	}


	template<class ...Args> inline void __rl_virtual_constructor(int &self, Args&&...args) { __rl_constructor(self, std::forward<Args>(args)...); }
	template<class ...Args> inline void __rl_virtual_constructor(unsigned &self, Args&&...args) { __rl_constructor(self, std::forward<Args>(args)...); }
	template<class ...Args> inline void __rl_virtual_constructor(short &self, Args&&...args) { __rl_constructor(self, std::forward<Args>(args)...); }
	template<class ...Args> inline void __rl_virtual_constructor(unsigned short &self, Args&&...args) { __rl_constructor(self, std::forward<Args>(args)...); }
	template<class ...Args> inline void __rl_virtual_constructor(bool &self, Args&&...args) { __rl_constructor(self, std::forward<Args>(args)...); }
	template<class ...Args> inline void __rl_virtual_constructor(char &self, Args&&...args) { __rl_constructor(self, std::forward<Args>(args)...); }
	template<class T, class ...Args> inline void __rl_virtual_constructor(T * &self, Args&&...args) { __rl_constructor(self, std::forward<Args>(args)...); }

	template<class T, class ...Args>
	inline void __rl_virtual_constructor(
		T &self,
		Args&&...args)
	{
		self.__rl_virtual_constructor(static_cast<T::__rl_identifier const *>(nullptr), std::forward<Args>(args)...);
	}

	template<class T, class ...Args>
	inline void __rl_virtual_p_constructor(
		T * self,
		Args&&...args)
	{
		__rl_virtual_constructor(*self, std::forward<Args>(args)...);
	}

	template<class T>
	inline void __rl_p_constructor(T *self)
	{
		new (self) T(::__rl::default_init);
	}
	template<class T, class ...Args>
	inline void __rl_p_constructor(
		T *self,
		Args&&...args)
	{
		new (self) T(std::forward<Args>(args)...);
	}

	template<class T, class ...Args>
	inline void __rl_p_constructor(
		T &self,
		Args&&...args)
	{
		new (&*self) std::decay_t<decltype(*self)> (std::forward<Args>(args)...);
	}

	template<class T>
	inline void __rl_p_constructor(T &self)
	{
		new (&*self) std::decay_t<decltype(*self)> (__rl::default_init);
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
	inline void __rl_p_destructor(T const& v)
	{
		__rl_destructor(*v);
	}

	template<class T>
	inline constexpr int8_t cmp(T const * lhs, T const * rhs)
	{
		auto diff = lhs - rhs;
		if(diff)
			return diff < 0 ? -1 : 1;
		return 0;
	}

	inline constexpr int8_t cmp(bool a, bool b)
	{
		if(a) return b ? 0 : 1;
		return b ? -1 : 0;
	}

	template<class T, class U, class = std::enable_if_t<std::is_integral_v<std::common_type_t<T, U>>>>
	inline constexpr int8_t cmp(T lhs, U rhs)
	{
		std::make_signed_t<T> diff = lhs - rhs;
		if(diff)
			return diff < 0 ? -1 : 1;
		return 0;
	}

	template<class T, class U, class = std::enable_if_t<!std::is_integral_v<T> && !std::is_pointer_v<T>>>
	inline int8_t cmp(T const& lhs, U const& rhs)
	{
		return lhs.__rl_cmp(rhs);
	}


	template<class T, class U>
	inline constexpr T rotl(T v, U sh)
	{
		return (v << sh) | (v >> (8 * sizeof(T)) - sh);
	}

	template<class T, class U>
	inline constexpr T rotr(T v, U sh)
	{
		return (v >> sh) | (v << (8 * sizeof(T)) - sh);
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
		constexpr EnumWrapper(__rl::bare_init_t): EnumWrapper() {}
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

		constexpr int8_t __rl_cmp(Type const& x) const
		{
			return __rl::cmp((int_t)value, (int_t)x.value);
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

		inline Tuple(__rl::default_init_t) {
			__rl_visit([](auto &x) { __rl_constructor(x, default_init); });
		}
		inline Tuple(__rl::bare_init_t) {
			__rl_visit([](auto &x) { __rl_constructor(x, bare_init); });
		}

		template<class ...Types2>
		inline Tuple(
			std::enable_if_t<
				(...&&(std::is_constructible_v<Types, Types2&&>)),
				CreateTuple>,
			Types2&&... args):
			std::tuple<Types...>(static_cast<Types2&&>(args)...)
		{
		}

		static constexpr size_t __rl_arity = sizeof...(Types);

		static constexpr char const * __rl_type_name_v = "Tuple";

		template<class TupleU,
			size_t i = 0,
			class = std::enable_if_t<__rl_arity == TupleU::__rl_arity>>
		inline int8_t __rl_cmp(TupleU const& rhs) const
		{
			if(auto c = __rl::cmp(std::get<i>(*this), std::get<i>(rhs)))
				return c;

			if constexpr(i == __rl_arity-1)
				return 0;
			else
				return this->template __rl_cmp<TupleU, i+1>(rhs);
		}

		Tuple<std::decay_t<Types>...> operator+() const {
			return *this;
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

		template<class Visitor, class ...Args>
		inline void __rl_visit(Visitor &&v, Args&&...args)
		{ this->template __rl_visit_all<0>(v, args...); }
		template<class Visitor, class ...Args>
		inline void __rl_visit(Visitor &&v, Args&&...args) const
		{ this->template __rl_visit_all<0>(v, args...); }

		template<class Visitor, class ...Args>
		inline void __rl_visit_reflect(Visitor &&v, Args&&...args)
		{ this->template __rl_visit_all_reflect<0>(v, args...); }
		template<class Visitor, class ...Args>
		inline void __rl_visit_reflect(Visitor &&v, Args&&...args) const
		{ this->template __rl_visit_all_reflect<0>(v, args...); }

	private:
		template<unsigned i, class Visitor, class ...Args>
		inline void __rl_visit_all(Visitor &&visit, Args&&...args)
		{
			visit(std::get<i>(*this), args...);
			if constexpr(i+1 < sizeof...(Types))
				__rl_visit_all<i+1>(visit, args...);
		}

		template<unsigned i, class Visitor, class ...Args>
		inline void __rl_visit_all(Visitor &&visit, Args&&...args) const
		{
			visit(std::get<i>(*this), args...);
			if constexpr(i+1 < sizeof...(Types))
				__rl_visit_all<i+1>(visit, args...);
		}

		template<unsigned i, class Visitor, class ...Args>
		inline void __rl_visit_all_reflect(Visitor &&visit, Args&&...args)
		{
			visit(i, std::get<i>(*this), args...);
			if constexpr(i+1 < sizeof...(Types))
				__rl_visit_all_reflect<i+1>(visit, args...);
		}
		template<unsigned i, class Visitor, class ...Args>
		inline void __rl_visit_all_reflect(Visitor &&visit, Args&&...args) const
		{
			visit(i, std::get<i>(*this), args...);
			if constexpr(i+1 < sizeof...(Types))
				__rl_visit_all_reflect<i+1>(visit, args...);
		}
	};

	template<class ...Types>
	Tuple<Types &&...> mk_tuple(Types&&...values) {
		return Tuple<Types&&...>(createTuple, static_cast<Types&&>(values)...);
	}

	constexpr default_init_t mk_tuple() { return default_init; };

	inline constexpr int8_t structure(int8_t x) { return x; }
	inline constexpr int16_t structure(int16_t x) { return x; }
	inline constexpr int32_t structure(int32_t x) { return x; }
	inline constexpr int64_t structure(int64_t x) { return x; }
	inline constexpr uint8_t structure(uint8_t x) { return x; }
	inline constexpr uint16_t structure(uint16_t x) { return x; }
	inline constexpr uint32_t structure(uint32_t x) { return x; }
	inline constexpr uint64_t structure(uint64_t x) { return x; }
	inline constexpr double structure(double x) { return x; }
	inline constexpr float structure(float x) { return x; }

	template<class...T>
	inline constexpr Tuple<T...> const& structure(Tuple<T...> const& x) { return x; }

	template<class T>
	inline constexpr T const * structure(T const * x) { return x; }

	template<class T>
	inline auto structure(T const& x) { return x.__rl_structure(); }

	template<class T0, class T1, class ...T>
	inline Tuple<T0&&, T1&&, T&&...> single_ctor_arg(
		T0&&arg0,
		T1&&arg1,
		T&&...arg)
	{
		return mk_tuple(
			std::forward<T0>(arg0),
			std::forward<T1>(arg1),
			std::forward<T>(arg)...);
	}
	template<class T>
	constexpr T&& single_ctor_arg(T&&arg)
	{
		return std::forward<T&&>(arg);
	}
	constexpr default_init_t single_ctor_arg() { return default_init; }
/*
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
	};*/

	template<class Symbol>
	struct SymbolBase
	{
		inline constexpr Symbol operator()() const {
			return Symbol{};
		}
		template<class T, class ...Ts>
		inline Tuple<Symbol, T&&, Ts&&...> operator()(T&& arg, Ts&&...args) const {
			return mk_tuple(Symbol(), std::forward<T>(arg), std::forward<Ts>(args)...);
		}
	};

	struct TupleCtorHelper {} const tupleCtorHelper {};

	namespace constant
	{
		struct __t_less : public __rl::SymbolBase<__t_less> {} const __v_less{};
		struct __t_greater : public __rl::SymbolBase<__t_greater> {} const __v_greater{};
		struct __t_less_greater : public __rl::SymbolBase<__t_less_greater> {} const __v_less_greater{};
		struct __t_question_mark : public __rl::SymbolBase<__t_question_mark> {} const __v_question_mark{};
		struct __t_exclamation_mark : public __rl::SymbolBase<__t_exclamation_mark> {} const __v_exclamation_mark{};
		struct __t_less_minus : public __rl::SymbolBase<__t_less_minus> {} const __v_less_minus{};
	}

	template<class T>
	class atomic : std::atomic<T>
	{
	public:
		using std::atomic<T>::atomic;

		inline T operator()(constant::__t_less) const
		{ return this->load(std::memory_order_acquire); }
		inline T operator()(constant::__t_greater) const
		{ return this->load(std::memory_order_release); }
		inline T operator()(constant::__t_question_mark) const
		{ return this->load(std::memory_order_relaxed); }
		inline T operator()(constant::__t_exclamation_mark) const
		{ return this->load(std::memory_order_seq_cst); }
		inline T operator()(constant::__t_less_minus) const
		{ return this->load(std::memory_order_consume); }

		inline void operator=(std::tuple<constant::__t_greater, T const&> v)
		{ this->store(std::get<1>(v), std::memory_order_release); }
		inline void operator=(std::tuple<constant::__t_question_mark, T const&> v)
		{ this->store(std::get<1>(v), std::memory_order_relaxed); }
		inline void operator=(std::tuple<constant::__t_exclamation_mark, T const&> v)
		{ this->store(std::get<1>(v), std::memory_order_seq_cst); }

		inline T operator()(std::tuple<constant::__t_greater, T const&> v)
		{ return this->exchange(std::get<1>(v), std::memory_order_release); }
		inline T operator()(std::tuple<constant::__t_less_greater, T const&> v)
		{ return this->exchange(std::get<1>(v), std::memory_order_acq_rel); }
		inline T operator()(std::tuple<constant::__t_question_mark, T const&> v)
		{ return this->exchange(std::get<1>(v), std::memory_order_relaxed); }
		inline T operator()(std::tuple<constant::__t_exclamation_mark, T const&> v)
		{ return this->exchange(std::get<1>(v), std::memory_order_seq_cst); }
	};

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
			}
			++detail::failures;
			return 0;
		}
	}
}

namespace __rl
{
	template<class T, ::size_t kSize>
	constexpr size_t  count(T (&)[kSize]) { return kSize; }

	template<class T>
	inline auto count(T &&v) { return v.__rl_count(); }

	inline void const * real_addr(char const&v) { return &v; }
	inline void const * real_addr(signed const&v) { return &v; }
	inline void const * real_addr(unsigned const&v) { return &v; }
	inline void const * real_addr(signed short const&v) { return &v; }
	inline void const * real_addr(unsigned short const&v) { return &v; }
	inline void const * real_addr(signed long const&v) { return &v; }
	inline void const * real_addr(unsigned long const&v) { return &v; }
	inline void const * real_addr(signed char const&v) { return &v; }
	inline void const * real_addr(unsigned char const&v) { return &v; }
	inline void const * real_addr(float const&v) { return &v; }
	inline void const * real_addr(double const&v) { return &v; }

	template<class T> inline void const * real_addr(T * const& v) { return &v; }
	template<class T> inline void const * real_addr(T * &v) { return &v; }
	template<class T> inline void const * real_addr(T const* const& v) { return &v; }
	template<class T> inline void const * real_addr(T const* & v) { return &v; }
	template<class T> inline void const * real_addr(T const&v)
	{
		return v.__rl_get_derived(static_cast<T::__rl_identifier const *>(nullptr));
	}



	inline constexpr size_t real_sizeof(char const&v) { return sizeof(v); }
	inline constexpr size_t real_sizeof(signed const&v) { return sizeof(v); }
	inline constexpr size_t real_sizeof(unsigned const&v) { return sizeof(v); }
	inline constexpr size_t real_sizeof(signed short const&v) { return sizeof(v); }
	inline constexpr size_t real_sizeof(unsigned short const&v) { return sizeof(v); }
	inline constexpr size_t real_sizeof(signed long const&v) { return sizeof(v); }
	inline constexpr size_t real_sizeof(unsigned long const&v) { return sizeof(v); }
	inline constexpr size_t real_sizeof(signed char const&v) { return sizeof(v); }
	inline constexpr size_t real_sizeof(unsigned char const&v) { return sizeof(v); }
	inline constexpr size_t real_sizeof(float const&v) { return sizeof(v); }
	inline constexpr size_t real_sizeof(double const&v) { return sizeof(v); }

	template<class T> inline constexpr size_t real_sizeof(T * const& v) { return sizeof(v); }
	template<class T> inline constexpr size_t real_sizeof(T * &v) { return sizeof(v); }
	template<class T, size_t n> inline constexpr size_t real_sizeof(array<T, n> const& x) { return sizeof(x); }
	template<class T> inline unsigned real_sizeof(T const&v) { return v.__rl_real_sizeof(static_cast<T::__rl_identifier const *>(nullptr)); }



	inline void copy_rtti(char const&v, void *) { }
	inline void copy_rtti(signed const&v, void *) { }
	inline void copy_rtti(unsigned const&v, void *) { }
	inline void copy_rtti(signed short const&v, void *) { }
	inline void copy_rtti(unsigned short const&v, void *) { }
	inline void copy_rtti(signed long const&v, void *) { }
	inline void copy_rtti(unsigned long const&v, void *) { }
	inline void copy_rtti(signed char const&v, void *) { }
	inline void copy_rtti(unsigned char const&v, void *) { }
	inline void copy_rtti(float const&v, void *) { }
	inline void copy_rtti(double const&v, void *) { }

	template<class T> inline void copy_rtti(T * const& v, void *) { }
	template<class T> inline void copy_rtti(T * &v, void *) { }
	template<class T> inline void copy_rtti(T const&v, void * copy) { return v.__rl_copy_rtti(static_cast<T::__rl_identifier const *>(nullptr), copy); }



	template<class T>
	concept HasCustomValueOf = requires(T x) { x.__rl_value_of(); };

	template<class T, bool = HasCustomValueOf<T>>
	struct value_of_t { };

	template<class T>
	struct value_of_t<T, true> {
		typedef typename value_of_t<
			decltype(std::declval<T>().__rl_value_of())
		>::type type;
	};

	template<class T>
	struct value_of_t<T, false> {
		typedef T type;
	};

	template<class T>
	concept is_plain = std::is_same_v<T, std::decay_t<T>>;
	template<class T>
	concept is_plain_value_of = is_plain<decltype(std::declval<T&&>().__rl_value_of())>;

	template<bool decay, class T>
	using decay_if_t = std::conditional_t<decay, std::decay_t<T>, T>;

	template<class T, bool decay = false>
	decay_if_t<
		!HasCustomValueOf<T> && decay,
		typename value_of_t<T&&>::type
	> value_of(T &&v)
	{
		if constexpr(HasCustomValueOf<T>)
		{
			return value_of<decltype(v.__rl_value_of()), is_plain_value_of<T>>(v.__rl_value_of());
		} else {
			return std::forward<T>(v);
		}
	}

	template<class T, class U>
	inline T __rl_dynamic_cast(U *v) {
		if constexpr(std::is_polymorphic<U>() || std::is_convertible<U*, T>())
			return dynamic_cast<T>(v);
		else
			return nullptr;
	}
	template<class T, class U>
	inline T __rl_dynamic_cast(U &v) {
		return dynamic_cast<T>(v);
	}

	template<class T>
	struct TypeNumber { enum { value = T::__rl_type_number_v }; };

	template<> struct TypeNumber<uint8_t> { enum { value = 1 }; };
	template<> struct TypeNumber<int8_t> { enum { value = 2 }; };
	template<> struct TypeNumber<uint16_t> { enum { value = 3 }; };
	template<> struct TypeNumber<int16_t> { enum { value = 4 }; };
	template<> struct TypeNumber<uint32_t> { enum { value = 5 }; };
	template<> struct TypeNumber<int32_t> { enum { value = 6 }; };
	template<> struct TypeNumber<uint64_t> { enum { value = 7 }; };
	template<> struct TypeNumber<int64_t> { enum { value = 8 }; };
	template<> struct TypeNumber<float> { enum { value = 9 }; };
	template<> struct TypeNumber<double> { enum { value = 10 }; };
	template<> struct TypeNumber<long double> { enum { value = 11 }; };
	template<> struct TypeNumber<nullptr_t> { enum { value = 12 }; };
	template<> struct TypeNumber<bool> { enum { value = 13 }; };

	enum { last_native_type_number = 13 };

	template<class T>
	constexpr unsigned type_number() { return TypeNumber<T>::value; }
	template<class T> constexpr unsigned type_number(T const&) { return TypeNumber<T>::value; }
	template<class T> constexpr unsigned type_number(T &) { return TypeNumber<T>::value; }
	template<class T> constexpr unsigned type_number(T const*) { return TypeNumber<T>::value; }
	template<class T> constexpr unsigned type_number(T *) { return TypeNumber<T>::value; }
	template<class T> constexpr unsigned type_number(T const&&) { return TypeNumber<T>::value; }
	template<class T> constexpr unsigned type_number(T &&) { return TypeNumber<T>::value; }

	template<class T>
	inline unsigned deriving_type_number(T const& type)
	{
		if constexpr(std::is_polymorphic<T>())
			return type.__rl_type_number(static_cast<T::__rl_identifier const *>(nullptr));
		else
			return type_number<T>();
	}

	template<class T>
	inline unsigned deriving_type_number(T const * type)
	{
		if constexpr(std::is_polymorphic<T>())
			return type->__rl_type_number(static_cast<T::__rl_identifier const *>(nullptr));
		else
			return type_number<T>();
	}

	template<class T>
	inline unsigned deriving_type_number(T * type)
	{
		if constexpr(std::is_polymorphic<T>())
			return type->__rl_type_number(static_cast<T::__rl_identifier const *>(nullptr));
		else
			return type_number<T>();
	}

	template<class T> struct TypeName
	{
		static constexpr char const * value = T::__rl_type_name_v;
	};

	template<> struct TypeName<char> { static constexpr char const * value = "CHAR"; };
	template<> struct TypeName<uint8_t> { static constexpr char const * value = "U1"; };
	template<> struct TypeName<int8_t> { static constexpr char const * value = "S1"; };
	template<> struct TypeName<uint16_t> { static constexpr char const * value = "U2"; };
	template<> struct TypeName<int16_t> { static constexpr char const * value = "S2"; };
	template<> struct TypeName<uint32_t> { static constexpr char const * value = "U4"; };
	template<> struct TypeName<int32_t> { static constexpr char const * value = "S4"; };
	template<> struct TypeName<uint64_t> { static constexpr char const * value = "U8"; };
	template<> struct TypeName<int64_t> { static constexpr char const * value = "S8"; };
	template<> struct TypeName<float> { static constexpr char const * value = "SINGLE"; };
	template<> struct TypeName<double> { static constexpr char const * value = "DOUBLE"; };
	template<> struct TypeName<long double> { static constexpr char const * value = "QUADRUPLE"; };
	template<> struct TypeName<nullptr_t> { static constexpr char const * value = "NULL"; };
	template<> struct TypeName<bool> { static constexpr char const * value = "BOOL"; };

	template<class T>
	constexpr char const * type_name() { return TypeName<T>::value; }
	template<class T> constexpr char const * type_name(T const&) { return TypeName<T>::value; }
	template<class T> constexpr char const * type_name(T &) { return TypeName<T>::value; }
	template<class T> constexpr char const * type_name(T const*) { return TypeName<T>::value; }
	template<class T> constexpr char const * type_name(T *) { return TypeName<T>::value; }
	template<class T> constexpr char const * type_name(T const&&) { return TypeName<T>::value; }
	template<class T> constexpr char const * type_name(T &&) { return TypeName<T>::value; }

	template<class Fn, class Obj, class ...Args>
	inline Fn &&visit(Fn &&fn, Obj &&obj, Args &&... args) requires requires {
		obj.__rl_visit(std::forward<Fn>(fn), std::forward<Args>(args)...);
	}
	{
		obj.__rl_visit(std::forward<Fn>(fn), std::forward<Args>(args)...);
		return std::forward<Fn>(fn);
	}

	template<class Fn, class Obj, class ...Args>
	inline Fn &&visit(Fn &&fn, Obj &&obj, Args &&... args)
	{
		throw __rl::Tuple<char const*, char const*>(createTuple,
			"VISIT invalid", ::__rl::type_name(obj));
	}

	template<class Fn, class Obj, class ...Args>
	inline Fn &&visit_reflect(Fn &&fn, Obj &&obj, Args &&... args) requires requires {
		obj.__rl_visit_reflect(std::forward<Fn>(fn), std::forward<Args>(args)...);
	}
	{
		obj.__rl_visit_reflect(std::forward<Fn>(fn), std::forward<Args>(args)...);
		return std::forward<Fn>(fn);
	}

	template<class Fn, class Obj, class ...Args>
	inline Fn &&visit_reflect(Fn &&fn, Obj &&obj, Args &&... args)
	{
		throw __rl::mk_tuple("VISIT* invalid", ::__rl::type_name(obj));
	}

	template<class T>
	inline char const * deriving_type_name(T const& type)
	{
		if constexpr(std::is_polymorphic<T>())
			return type.__rl_type_name(static_cast<T::__rl_identifier const *>(nullptr));
		else
			return type_name<T>();
	}

	template<class T>
	inline char const * deriving_type_name(T const * type)
	{
		if constexpr(std::is_polymorphic<T>())
			return type->__rl_type_name(static_cast<T::__rl_identifier const *>(nullptr));
		else
			return type_name<T>();
	}

	template<class T>
	inline char const * deriving_type_name(T * type)
	{
		if constexpr(std::is_polymorphic<T>())
			return type->__rl_type_name(static_cast<T::__rl_identifier const *>(nullptr));
		else
			return type_name<T>();
	}

	template<class T>
	class AutoDynCastable
	{
		T cast;
	public:
		AutoDynCastable(T cast): cast((T)cast)
		{}

		AutoDynCastable(AutoDynCastable &&) = delete;
		AutoDynCastable(AutoDynCastable const&) = delete;

		template<class U>
		constexpr operator U &() const requires requires(T v){
			dynamic_cast<U &>((T)v);
			requires(!std::is_rvalue_reference_v<T>);
		} {
			return dynamic_cast<U &>((T)cast);
		}

		template<class U>
		constexpr operator U const&() const requires requires(T v){
			dynamic_cast<U const&>((T)v);
			requires(!std::is_rvalue_reference_v<T>);
		} {
			return dynamic_cast<U const&>((T)cast);
		}

		template<class U>
		constexpr operator U &&() const requires requires(T v){
			dynamic_cast<U &&>((T)v);
			requires(std::is_rvalue_reference_v<T>);
		} {
			return dynamic_cast<U &&>((T)cast);
		}

		template<class U>
		constexpr operator U const&&() const requires requires(T v){
			dynamic_cast<U const&&>((T)v);
			requires(std::is_rvalue_reference_v<T>);
		} {
			return dynamic_cast<U const&&>((T)cast);
		}

		template<class U>
		constexpr operator U *() const requires requires(T v){
			dynamic_cast<U *>((T)v);
		} {
			return dynamic_cast<U *>((T)cast);
		}

		template<class U>
		constexpr operator U const*() const requires requires(T v){
			dynamic_cast<U const*>((T)v);
		} {
			return dynamic_cast<U const*>((T)cast);
		}
	};

	template<class T>
	[[nodiscard]] AutoDynCastable<T&&> auto_dyn_cast(T &&v) { return (T&&)v; }
}

// Declare special size types.
#define UM unsigned long int
#define SM signed long int
#define BOOL bool
#define CHAR char
#define UCHAR unsigned char
#define INT int
#define UINT unsigned
#define SINGLE float
#define DOUBLE double
#define U1 ::uint8_t
#define U2 ::uint16_t
#define U4 ::uint32_t
#define U8 ::uint64_t
#define S1 ::int8_t
#define S2 ::int16_t
#define S4 ::int32_t
#define S8 ::int64_t

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
