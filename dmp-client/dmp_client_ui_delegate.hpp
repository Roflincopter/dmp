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
	virtual void query_parse_error(){}
	virtual void client_stopped(){}
	virtual void add_radio_succes(message::AddRadioResponse){}
	virtual void add_radio_failed(message::AddRadioResponse){}
	
	virtual void search_results_start(message::SearchResponse){};
	virtual void search_results_end(){}
	
	virtual void new_search_begin(){}
	virtual void new_search_end(){}
	
	virtual void add_radio_start(){}
	virtual void add_radio_end(){}
	
	virtual void set_radios_start(){}
	virtual void set_radios_end(){}
	
	virtual void playlist_update_start(message::PlaylistUpdate){}
	virtual void playlist_update_end(message::PlaylistUpdate){}
	
	virtual void set_play_paused_state(bool state){};
	
	virtual ~DmpClientUiDelegate(){}
};
