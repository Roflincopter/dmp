#pragma once

#include "message.hpp"

#include <string>

struct DmpClientUiDelegate
{
	virtual void query_parse_error(dmp_library::ParseError e){}
	virtual void search_results(message::SearchResponse search_response){}
	virtual void bye_ack_received(){}
	virtual void new_search(){}

	virtual ~DmpClientUiDelegate(){}
};
