#pragma once

#include "dmp-library.hpp"
#include "debug_macros.hpp"

#include <QMetaType>
#include <QDataStream>

#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <string>
#include <sstream>

Q_DECLARE_METATYPE(std::string)
Q_DECLARE_METATYPE(dmp_library::LibraryEntry::Duration)
Q_DECLARE_METATYPE(dmp_library::LibraryEntry)


inline void init_meta_types() {
	qRegisterMetaType<std::string>("StdString");
	qRegisterMetaTypeStreamOperators<std::string>("StdString");
	qRegisterMetaType<dmp_library::LibraryEntry::Duration>("DmpLibrary_LibraryEntry_Duration");
	qRegisterMetaTypeStreamOperators<dmp_library::LibraryEntry::Duration>("DmpLibrary_LibraryEntry_Duration");
	qRegisterMetaType<dmp_library::LibraryEntry>("DmpLibrary LibraryEntry");
	qRegisterMetaTypeStreamOperators<dmp_library::LibraryEntry>("DmpLibrary_LibraryEntry");
}

inline QDataStream& operator<<(QDataStream& os, std::string const& string) {
	return os << QString::fromStdString(string);
}

inline QDataStream& operator>>(QDataStream& is, std::string& string) {
	QString str;
	is >> str;
	string = str.toStdString();
	return is;
}

inline QDataStream& operator<<(QDataStream& os, dmp_library::LibraryEntry::Duration const& duration) {
	std::stringstream ss;
	boost::archive::text_oarchive oar(ss);
	oar & duration;
	return os << QString::fromStdString(ss.str());
}

inline QDataStream& operator>>(QDataStream& is, dmp_library::LibraryEntry::Duration& duration) {
	QString str;
	is >> str;
	std::stringstream ss(str.toStdString());
	boost::archive::text_iarchive iar(ss);
	iar & duration;
	return is;
}

inline QDataStream& operator<<(QDataStream& os, dmp_library::LibraryEntry const& entry) {
	std::stringstream ss;
	boost::archive::text_oarchive oar(ss);
	oar & entry;
	return os << QString::fromStdString(ss.str());
}

inline QDataStream& operator>>(QDataStream& is, dmp_library::LibraryEntry& entry) {
	QString str;
	is >> str;
	std::stringstream ss(str.toStdString());
	boost::archive::text_iarchive iar(ss);
	iar & entry;
	return is;
}

