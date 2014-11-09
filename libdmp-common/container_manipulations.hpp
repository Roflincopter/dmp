#pragma once

template <typename T>
void remove_element(T container, typename T::value_type const& element){
	container.erase(element);
}

template<typename V>
void remove_element(std::vector<V>& vector, V const& element) {
	auto const_end = vector.cend();
	for(auto it = vector.cbegin(); it != const_end; ++it) {
		if(*it == element) {
			vector.erase(it);
			break;
		}
	}
}
