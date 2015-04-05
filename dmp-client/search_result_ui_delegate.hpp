#pragma once

struct SearchResultUiDelegate {

	virtual void search_results_start(message::SearchResponse){}
	virtual void search_results_end(){}
	
	virtual void new_search_begin(){}
	virtual void new_search_end(){}

	virtual ~SearchResultUiDelegate(){}
};
