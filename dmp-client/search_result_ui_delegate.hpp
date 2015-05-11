#pragma once

struct SearchResultUiDelegate {

	virtual void search_results_start(int){}
	virtual void search_results_end(){}
	
	virtual void new_search_begin(){}
	virtual void new_search_end(){}

	virtual ~SearchResultUiDelegate(){}
};
