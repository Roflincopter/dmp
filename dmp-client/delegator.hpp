#pragma once

#include <vector>
#include <memory>

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
		Delegator_impl<Delegate>::observers.template push_back(delegate);
	}

	template <typename Delegate>
	void remove_delegates(std::weak_ptr<Delegate> delegate)
	{
		auto delegates = Delegator_impl<Delegate>::observers;
		if(auto it = delegates.find(delegate.lock()) != delegates.end())
		{
			delegates.erase(it);
		}
	}

	template <typename Delegate, typename... Args>
	void call_on_delegates(void(Delegate::* member_fun)(Args...), Args... args)
	{
		for(auto&& delegate : Delegator_impl<Delegate>::observers)
		{
			auto s = delegate.lock();
			((*s).*member_fun)(args...);
		}
	}
};
