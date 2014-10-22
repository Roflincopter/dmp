
#include "message.hpp"

namespace message {

std::ostream& operator<<(std::ostream& os, PlaybackAction const& a) {
	return os << static_cast<Type_t>(a);
}

}
