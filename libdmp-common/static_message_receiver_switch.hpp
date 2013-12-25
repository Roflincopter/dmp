#pragma once

#include "message_callbacks.hpp"

template<typename T, int... Indices>
void handle_message(message::DmpCallbacks& cbs, message::Type index, indices<Indices...>, T& conn)
{
	static std::function<void()> table[] =
	{
		[&conn, &cbs]()
		{
			typedef typename message::type_to_message<static_cast<message::Type>(Indices)>::type message_t;
			conn.template async_receive<message_t>(cbs);
			return;
		}
		...
	};

	table[static_cast<uint32_t>(index)]();
}

template<typename T>
void handle_message(message::DmpCallbacks& cbs, message::Type index, T& conn)
{
	typedef typename build_indices<static_cast<uint32_t>(message::Type::LAST)>::type indices_type;

	handle_message(cbs, index, indices_type{}, conn);
}
