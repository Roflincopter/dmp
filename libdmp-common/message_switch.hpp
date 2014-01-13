#pragma once

#include "message_callbacks.hpp"

struct MessageSwitch
{
	#ifdef __GNUC__
	template <int index, typename Conn>
	static constexpr std::function<void()>&& create_message_receiver(message::DmpCallbacks& cbs, Conn& conn)
	{
		return std::move(std::function<void()>([&conn, &cbs]
		{
			typedef typename message::type_to_message<static_cast<message::Type>(index)>::type message_t;
			conn.template async_receive<message_t>(cbs);
			return;
		}));
	}
	#endif //__GNUC__

	std::array<std::function<void()>, static_cast<message::Type_t>(message::Type::LAST)> table;

	template<typename Conn, int... Indices>
	MessageSwitch(message::DmpCallbacks& cbs, indices<Indices...>, Conn& conn)
		: table(
		#ifdef __GNUC__
			{
				//Workaround for gcc bug: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=47226
				MessageSwitch::create_message_receiver<Indices>(cbs, conn)
				...
			}
		#else
			{
				[&conn, &cbs]()
				{
					typedef typename message::type_to_message<static_cast<message::Type>(Indices)>::type message_t;
					conn.template async_receive<message_t>(cbs);
					return;
				}
				...
			}
		#endif //__GNUC__
		)

	{}

	void handle_message(message::Type index) {
		table[static_cast<message::Type_t>(index)]();
	}
};

template <typename Conn>
MessageSwitch make_message_switch(message::DmpCallbacks& cbs, Conn& conn)
{
	typedef typename build_indices<static_cast<message::Type_t>(message::Type::LAST)>::type indices_type;

	return MessageSwitch(cbs, indices_type{}, conn);
}
