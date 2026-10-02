#pragma once

#include "library-entry.hpp"

#include <QString>
#include <QMetaType>
#include <QDataStream>
#include <QtGlobal>
#include <QItemSelectionModel>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <string>
#include <sstream>

Q_DECLARE_METATYPE(std::string)
Q_DECLARE_METATYPE(dmp_library::LibraryEntry::Duration)
Q_DECLARE_METATYPE(dmp_library::LibraryEntry)


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

// Qt 6 picks up the QDataStream operators above automatically when the type is
// registered; Qt 5 needs them registered explicitly.
inline void init_meta_types() {
	qRegisterMetaType<std::string>("StdString");
	qRegisterMetaType<dmp_library::LibraryEntry::Duration>("DmpLibrary_LibraryEntry_Duration");
	qRegisterMetaType<dmp_library::LibraryEntry>("DmpLibrary LibraryEntry");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	qRegisterMetaTypeStreamOperators<std::string>("StdString");
	qRegisterMetaTypeStreamOperators<dmp_library::LibraryEntry::Duration>("DmpLibrary_LibraryEntry_Duration");
	qRegisterMetaTypeStreamOperators<dmp_library::LibraryEntry>("DmpLibrary_LibraryEntry");
#endif
}
