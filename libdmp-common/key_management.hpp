#pragma once

#include <vector>
#include <cstdint>
#include <ostream>

struct KeyPair {
	std::vector<uint8_t> private_key;
	std::vector<uint8_t> public_key;
	
	friend std::ostream& operator<<(std::ostream& os, KeyPair const& kp);
};

KeyPair load_key_pair(bool nocreate = false);