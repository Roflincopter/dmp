#pragma once

#include "message.hpp"

#include <string>

template <typename T, typename U, typename... Args>
void call_on_delegates(T delegates, U member_ptr, Args... arguments)
{
	for(auto w : delegates)
	{
		auto s = w.lock();
		((*s).*member_ptr)(arguments...);
	}
}

struct DmpClientUiDelegate
{
	virtual void query_parse_error(dmp_library::ParseError e){}
	virtual void search_results(message::SearchResponse){}
	virtual void client_stopped(){}
	virtual void new_search(){}
	virtual void add_radio_succes(message::AddRadioResponse){}
	virtual void add_radio_failed(message::AddRadioResponse){}
	virtual void test(std::string, int){}
	virtual void radios_update(message::Radios){}
	virtual void radio_added(message::AddRadio){}
	virtual void playlist_updated(message::PlaylistUpdate){}

	virtual ~DmpClientUiDelegate(){}
};
