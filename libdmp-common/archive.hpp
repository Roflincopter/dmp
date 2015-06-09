#pragma once

#include <boost/mpl/bool.hpp>
#include <boost/archive/detail/common_oarchive.hpp>
#include <boost/archive/detail/register_archive.hpp>
#include <boost/archive/basic_archive.hpp>

#include <stdint.h>
#include <cstddef>
#include <vector>
#include <array>
#include <map>
#include <string>
#include <utility>
#include <type_traits>
#include <istream>

namespace boost { namespace archive { class save_access; } }

template <typename T>
struct is_vector : public std::false_type {};

template <typename T>
struct is_vector<std::vector<T>> : public std::true_type {};

template <typename T>
struct is_array : public std::false_type {};

template <typename T, size_t I>
struct is_array<std::array<T, I>> : public std::true_type {};

template <typename T>
struct is_map : public std::false_type {};

template <typename T, typename U>
struct is_map<std::map<T, U>> : public std::true_type {};

template <typename T>
struct is_pair : public std::false_type {};

template <typename T, typename U>
struct is_pair<std::pair<T, U>> : public std::true_type {};

typedef uint32_t serialized_enum_t;

class OArchive : public boost::archive::detail::common_oarchive<OArchive>
{
	std::ostream& os;

public:
	
	OArchive(std::ostream& os)
	: os(os)
	{}

	friend class boost::archive::save_access;

	typedef boost::mpl::bool_<true> is_saving;
	typedef boost::mpl::bool_<false> is_loading;
	
	template<typename T>
	OArchive& operator<<(T const& t) {
		return *this & t;
	}
	
	OArchive& operator&(const boost::archive::tracking_type) {return *this;}
	OArchive& operator&(const boost::archive::version_type) {return *this;}
	OArchive& operator&(const boost::archive::object_id_type) {return *this;}
	OArchive& operator&(const boost::archive::object_reference_type) {return *this;}
	OArchive& operator&(const boost::archive::class_id_type) {return *this;}
	OArchive& operator&(const boost::archive::class_id_optional_type) {return *this;}
	OArchive& operator&(const boost::archive::class_id_reference_type) {return *this;}
	OArchive& operator&(const boost::archive::class_name_type&) {return *this;}
	
	template<typename T>
	typename std::enable_if<std::is_fundamental<T>::value && !std::is_same<T, uint8_t>::value, OArchive&>::type
	operator&(T & t) {
		os << t << " ";
		return *this;
	}
	
	template<typename T>
	typename std::enable_if<std::is_fundamental<T>::value && std::is_same<T, uint8_t>::value, OArchive&>::type
	operator&(T & t) {
		os << static_cast<unsigned int>(t) << " ";
		return *this;
	}
	
	
	template<typename T>
	typename std::enable_if<std::is_same<std::string, T>::value, OArchive&>::type
	operator&(T& t) {
		os << t.size() << " " << t << " ";
		return *this;
	}
	
	template <typename T>
	typename std::enable_if<
			std::is_class<T>::value &&
			!is_vector<T>::value &&
			!is_array<T>::value &&
			!is_map<T>::value &&
			!is_pair<T>::value &&
			!std::is_enum<T>::value &&
			!std::is_same<std::string, T>::value
		,
		OArchive&
	>::type
	operator&(T& t) {
		boost::serialization::serialize(*this, t, 0);
		return *this;
	}
	
	template<typename T>
	typename std::enable_if<is_vector<T>::value || is_map<T>::value || is_array<T>::value, OArchive& >::type
	operator&(T& t) {
		size_t count = t.size();
		os << count << " ";
		
		for(auto& v : t) {
			*this & v;
		}
		
		return *this;
	}
	
	template<typename T>
	typename std::enable_if<is_pair<T>::value, OArchive&>::type
	operator&(T& t) {
		typename std::decay<typename T::first_type>::type x1 = t.first;
		*this & x1 & t.second;
		return *this;
	}
	
	template<typename T>
	typename std::enable_if<std::is_enum<T>::value, OArchive&>::type
	operator&(T& t) {
		os << static_cast<serialized_enum_t>(t) << " ";
		return *this;
	}
};

class IArchive : public boost::archive::detail::common_oarchive<IArchive>
{
	std::istream& is;

public:
	
	IArchive(std::istream& is)
	: is(is)
	{}

	friend class boost::archive::save_access;

	typedef boost::mpl::bool_<false> is_saving;
	typedef boost::mpl::bool_<true> is_loading;
	
	template<typename T>
	IArchive& operator<<(T const& t) {
		return *this & t;
	}
	
	IArchive& operator&(const boost::archive::tracking_type) {return *this;}
	IArchive& operator&(const boost::archive::version_type) {return *this;}
	IArchive& operator&(const boost::archive::object_id_type) {return *this;}
	IArchive& operator&(const boost::archive::object_reference_type) {return *this;}
	IArchive& operator&(const boost::archive::class_id_type) {return *this;}
	IArchive& operator&(const boost::archive::class_id_optional_type) {return *this;}
	IArchive& operator&(const boost::archive::class_id_reference_type) {return *this;}
	IArchive& operator&(const boost::archive::class_name_type&) {return *this;}
	
	template<typename T>
	typename std::enable_if<std::is_fundamental<T>::value && !std::is_same<T, uint8_t>::value, IArchive&>::type
	operator&(T& t) {
		is >> t >> std::ws;
		return *this;
	}
	
	template<typename T>
	typename std::enable_if<std::is_fundamental<T>::value && std::is_same<T, uint8_t>::value, IArchive&>::type
	operator&(T& t) {
		unsigned int x;
		is >> x >> std::ws;
		t = static_cast<uint8_t>(x);
		return *this;
	}
	
	template<typename T>
	typename std::enable_if<std::is_same<std::string, T>::value, IArchive&>::type
	operator&(T& t) {
		size_t size;
		is >> size >> std::ws;
		
		char buffer[size + 1];
		is.read(buffer, size);
		buffer[size] = '\0';
		is >> std::ws;
		
		t = std::string(buffer);
		return *this;
	}
	
	template <typename T>
	typename std::enable_if<
			std::is_class<T>::value &&
			!is_vector<T>::value &&
			!is_array<T>::value &&
			!is_map<T>::value &&
			!is_pair<T>::value &&
			!std::is_enum<T>::value &&
			!std::is_same<std::string, T>::value
		,
		IArchive&
	>::type
	operator&(T& t) {
		boost::serialization::serialize(*this, t, 0);
		return *this;
	}
	
	template<typename T>
	typename std::enable_if<is_vector<T>::value, IArchive&>::type
	operator&(T& t) {
		size_t size;
		is >> size >> std::ws;
		
		t.reserve(size);
		for(size_t i = 0; i < size; ++i) {
			typename T::value_type x;
			*this & x;
			t.push_back(x);
		}
		
		return *this;
	}
	
	template<typename T>
	typename std::enable_if<is_array<T>::value, IArchive&>::type
	operator&(T& t) {
		size_t size;
		is >> size >> std::ws;
		
		for(size_t i = 0; i < size; ++i) {
			typename T::value_type x;
			*this & x;
			t[i] = x;
		}
		
		return *this;
	}
	
	template<typename T>
	typename std::enable_if<is_map<T>::value, IArchive&>::type
	operator&(T& t) {
		size_t size;
		
		is >> size >> std::ws;
		
		for(size_t i = 0; i < size; ++i) {
			std::pair<typename std::decay<typename T::value_type::first_type>::type, typename std::decay<typename T::value_type::second_type>::type> x;
			*this & x;
			t.insert(x);
		}
		
		return *this;
	}
	
	template<typename T>
	typename std::enable_if<is_pair<T>::value, IArchive&>::type
	operator&(T& t) {
		typename T::first_type x1;
		typename T::second_type x2;
		*this & x1 & x2;
		t = std::make_pair(x1, x2);
		
		return *this;
	}
	
	template<typename T>
	typename std::enable_if<std::is_enum<T>::value, IArchive&>::type
	operator&(T& t) {
		serialized_enum_t x;
		is >> x >> std::ws;
		t = static_cast<T>(x);
		return *this;
	}
};

BOOST_SERIALIZATION_REGISTER_ARCHIVE(OArchive)
BOOST_SERIALIZATION_REGISTER_ARCHIVE(IArchive)