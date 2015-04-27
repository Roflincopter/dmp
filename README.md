DMP
===

Distributed Music Player


Building on Mac OSX
===================

First we install the dependencies (for the client)

	brew install icu4c taglib boost qt5 gstreamer libsodium gst-plugins-good
	brew install gst-plugins-ugly --build-from-source


Then we can run cmake as usual. BUT: linking qt5 does not work out of the box (there
is no ```macosx-clang``` binary in ```/usr/local/?```). But one can set the following
cmake options:

	Qt5Core_DIR      /usr/local/Cellar/qt5/5.4.1/lib/cmake/Qt5Core
	Qt5Gui_DIR       /usr/local/Cellar/qt5/5.4.1/lib/cmake/Qt5Gui
	Qt5Widgets_DIR   /usr/local/Cellar/qt5/5.4.1/lib/cmake/Qt5Widgets

