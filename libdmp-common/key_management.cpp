#include "key_management.hpp"

#include "dmp_config.hpp"

#include <sodium.h>

#include <boost/filesystem.hpp>
#include <fstream>
#include <iomanip>

std::ostream& operator<<(std::ostream& os, std::vector<uint8_t> const& key) {
	for(auto&& byte : key) {
		os << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(byte) << " ";
	}
	return os;
}

std::istream& operator>>(std::istream& is, std::vector<uint8_t>& key) {
	for(auto& byte : key) {
		unsigned int hex;
		is >> std::hex >> hex >> std::ws;
		byte = static_cast<uint8_t>(hex);
	}
	return is;
}

KeyPair load_key_pair(bool nocreate)
{
	boost::filesystem::path private_key_path = config::get_private_key_path();
	boost::filesystem::path public_key_path = config::get_public_key_path();
	
	std::vector<uint8_t> private_key(crypto_box_secretkeybytes());
	std::vector<uint8_t> public_key(crypto_box_publickeybytes());
	
	if(!(boost::filesystem::exists(private_key_path) && boost::filesystem::exists(public_key_path))) {
		if(nocreate) {
			throw std::runtime_error("Key files did not exists and was asked not to create them.");
		} else {
			crypto_box_keypair(&public_key[0], &private_key[0]);
			
			std::ofstream private_key_stream(private_key_path.string());
			private_key_stream.close();
			boost::filesystem::permissions(private_key_path,
				boost::filesystem::owner_read | boost::filesystem::owner_write
			);
			private_key_stream.open(private_key_path.string());
			private_key_stream << private_key;
			
			std::ofstream public_key_stream(public_key_path.string());
			public_key_stream.close();
			boost::filesystem::permissions(public_key_path,
				boost::filesystem::owner_read | boost::filesystem::owner_write |
				boost::filesystem::group_read |
				boost::filesystem::others_read
			);
			public_key_stream.open(public_key_path.string());
			public_key_stream << public_key;
		}
	} else {
		std::ifstream private_key_stream(private_key_path.string());
		private_key_stream >> private_key;
		
		std::ifstream public_key_stream(public_key_path.string());
		public_key_stream >> public_key;
	}
	return KeyPair{private_key, public_key};
}


std::ostream& operator<<(std::ostream& os, const KeyPair& kp) {
	return os << "<<<" <<  kp.private_key << ">>> <<<" << kp.public_key << ">>>" << std::endl;
}
