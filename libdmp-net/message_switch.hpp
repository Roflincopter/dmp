#pragma once

#include "index_list.hpp"
#include "message.hpp"

#include <array>
#include <functional>

namespace message { struct DmpCallbacks; }

struct MessageSwitch
{
	template <int index>
	std::function<void(Connection&, message::DmpCallbacks const&)> create_message_receiver()
	{
		return [](Connection& conn, message::DmpCallbacks const& cbs)
		{
			typedef typename message::type_to_message<static_cast<message::Type>(index)>::type message_t;
			conn.async_receive<message_t>(cbs);
			return;
		};
	}

	std::array<std::function<void(Connection&, message::DmpCallbacks const&)>, static_cast<message::Type_t>(message::Type::LAST)> table;

	template<int... Indices>
	MessageSwitch(indices<Indices...>)
		: table(
			//not using labda because of gcc bug: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=47226
			{
				{
					MessageSwitch::create_message_receiver<Indices>()
					...
				}
			}
		)
	{}
	
	void handle_message(Connection& conn, message::Type index, message::DmpCallbacks const& cbs) const {
		if(static_cast<message::Type_t>(index) >= table.size()) {
			std::cout << "Out of bounds index detected" << std::endl;
		} else {
			if(auto f = table[static_cast<message::Type_t>(index)]) {
				f(conn, cbs);
			} else {
				std::cout << static_cast<message::Type_t>(index) << std::endl;
				std::cout << sizeof(table) << std::endl;
				std::cout << "Ignored message of type: " << type_to_string(index) << std::endl;
			}
		}
	}
};

inline MessageSwitch make_message_switch()
{
	typedef typename build_indices<static_cast<message::Type_t>(message::Type::LAST)>::type indices_type;

	return MessageSwitch(indices_type{});
}
