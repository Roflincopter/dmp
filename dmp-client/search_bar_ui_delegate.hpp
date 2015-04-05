#pragma once

struct SearchBarUiDelegate {
	virtual void query_parse_error(){}

	virtual ~SearchBarUiDelegate(){}
};
