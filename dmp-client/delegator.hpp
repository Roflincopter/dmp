#pragma once

template <typename Delegate>
struct Delegator_impl
{
	std::vector<std::weak_ptr<Delegate>> observers;
};

template<typename... Delegators>
struct Delegator_impl_gen;

template <>
struct Delegator_impl_gen<>
{};

template <typename Delegate, typename... Delegates>
struct Delegator_impl_gen<Delegate, Delegates...> : protected Delegator_impl<Delegate>, protected Delegator_impl_gen<Delegates...>
{};

template <typename... Delegates>
struct Delegator : protected Delegator_impl_gen<Delegates...>
{
	template <typename Delegate>
	void add_delegate(std::weak_ptr<Delegate> delegate)
	{
		static_cast<Delegator_impl<Delegate>*>(this)->observers.push_back(delegate);
	}

	template <typename Delegate>
	void remove_delegates(std::weak_ptr<Delegate> delegate)
	{
		auto delegates = static_cast<Delegator_impl<Delegate>*>(this)->observers;
		if(auto it = delegates.find(delegate.lock()) != delegates.end())
		{
			delegates.erase(it);
		}
	}

	template <typename Delegate, typename U, typename... Args>
	void call_on_delegates(U member_fun, Args... args)
	{
		for(auto&& delegate : static_cast<Delegator_impl<Delegate>*>(this)->observers)
		{
			auto s = delegate.lock();
			((*s).*member_fun)(args...);
		}
	}
};
