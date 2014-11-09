#pragma once

struct PlaylistUiDelegate
{
	virtual void playlist_update_start(message::PlaylistUpdate){}
	virtual void playlist_update_end(message::PlaylistUpdate){}
	
	virtual void current_radio_change_start(){}
	virtual void current_radio_change_end(){}
	
	virtual void set_radios_start(){}
	virtual void set_radios_end(){}
	
	virtual void add_radio_start(){}
	virtual void add_radio_end(){}

	virtual void remove_radio_start(){}
	virtual void remove_radio_end(){}
};
