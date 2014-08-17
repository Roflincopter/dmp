#pragma once

struct PlaylistUiDelegate
{
	virtual void playlist_update_start(message::PlaylistUpdate){}
	virtual void playlist_update_end(message::PlaylistUpdate){}
};
