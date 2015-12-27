#pragma once

struct SearchBarUiDelegate {
	virtual void query_parse_error(){}

	virtual void clear_start(){}
	virtual void clear_end(){}

	virtual ~SearchBarUiDelegate(){}
};
