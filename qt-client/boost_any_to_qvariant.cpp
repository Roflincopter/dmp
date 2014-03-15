
#include "boost_any_to_qvariant.hpp"

template<>
QVariant convert<std::string>(boost::any const& x)
{
	return QVariant::fromValue<QString>(QString::fromStdString(boost::any_cast<std::string>(x)));
}

template<>
QVariant convert<dmp_library::LibraryEntry::Duration>(boost::any const& x)
{
	return QVariant::fromValue<QString>(QString::fromStdString(boost::any_cast<dmp_library::LibraryEntry::Duration>(x).to_string()));
}
