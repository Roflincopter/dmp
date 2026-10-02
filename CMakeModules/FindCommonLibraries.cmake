find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# Boost.System is header-only since 1.69 and its stub library is gone in
# recent releases, so it is not requested as a component.
find_package(Boost 1.74 CONFIG REQUIRED COMPONENTS filesystem program_options regex serialization)

find_package(ICU 60 REQUIRED COMPONENTS uc i18n data)

pkg_check_modules(TagLib REQUIRED IMPORTED_TARGET taglib>=1.11)
pkg_check_modules(GStreamer REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.16)
pkg_check_modules(Sodium REQUIRED IMPORTED_TARGET libsodium>=1.0.18)
