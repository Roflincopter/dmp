#pragma once

#include "message_callbacks.hpp"

struct MessageSwitch
{
	std::array<std::function<void()>, static_cast<uint32_t>(message::Type::LAST)> table;

	template<typename Conn, int... Indices>
	MessageSwitch(message::DmpCallbacks& cbs, indices<Indices...>, Conn& conn)
		: table({
					[&conn, &cbs]()
					{
						typedef typename message::type_to_message<static_cast<message::Type>(Indices)>::type message_t;
						conn.template async_receive<message_t>(cbs);
						return;
					}
					...
				})
	{}

	void handle_message(message::Type index) {
		table[static_cast<uint32_t>(index)]();
	}

};

template <typename Conn>
MessageSwitch make_message_switch(message::DmpCallbacks& cbs, Conn& conn)
{
	typedef typename build_indices<static_cast<uint32_t>(message::Type::LAST)>::type indices_type;

	return MessageSwitch(cbs, indices_type{}, conn);
}
