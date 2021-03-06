#pragma once

struct RadioListUiDelegate {
	virtual void set_radio_states(){}
	
	virtual void clear_start(){}
	virtual void clear_end(){}
	
	virtual void set_radios_start(){}
	virtual void set_radios_end(){}
	
	virtual void add_radio_start(){}
	virtual void add_radio_end(){}

	virtual void remove_radio_start(){}
	virtual void remove_radio_end(){}

	virtual ~RadioListUiDelegate(){}
};
