
#include "dmp_qt_meta_types.hpp"
#include "dmp_client_gui.hpp"

#include <boost/program_options/parsers.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include <QApplication>

#include <iostream>
#include <string>
#include <memory>

int main(int argc, char* argv[]) {

#ifdef __linux
	//signal(SIGINT, core_dump);
#endif

	using boost::program_options::value;
	boost::program_options::options_description desc;
	desc.add_options()
		("help,h", "produce help message")
		;

	boost::program_options::positional_options_description pos_desc;
	pos_desc.add("server", 1).add("port", 1);

	boost::program_options::variables_map vm;

	try {
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), vm);

		if(vm.count("help")) {
			std::cout << "Usage: " << std::string(argv[0]);

			for (int i=0; unsigned(i) < pos_desc.max_total_count(); ++i) {
				std::cout << " <" << pos_desc.name_for_position(i) << ">";
			}

			std::cout << " [Options]" << std::endl << std::endl;

			std::cout << "Options:" << std::endl;
			std::cout << desc << std::endl;
			return 0;
		}

		boost::program_options::notify(vm);

	} catch(boost::program_options::error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	int retval = 0;
	{
		int x = 0;
		QApplication qapp(x, nullptr);
		init_meta_types();

		auto gui = std::make_shared<DmpClientGui>();

		gui->show();

		retval = qapp.exec();
	}
	return retval;
}
