#pragma once

#include <atomic>

namespace dmp_library {

struct LoadInfo {
	std::atomic<bool> should_stop;

	std::atomic<uint32_t> nr_folders;
	std::atomic<uint32_t> current_folder;
	
	std::atomic<uint32_t> current_max_tracks;
	std::atomic<uint32_t> current_track;
	
	std::atomic<bool> reading_cache;
	std::string cache_file;
};

}