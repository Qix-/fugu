#include "fg/fg.h"

#include <iostream>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

int main(int argc, char** argv) {
	if (argc!=3){
		std::cout << "Usage: " << argv[0] << " <script-directory> <script>";
		return 1;
	}

	// Create a new universe
	fg::Universe u = fg::Universe();
	boost::filesystem::path p(argv[1]);
	if (!boost::filesystem::is_directory(p))
		p.remove_filename();
	u.addScriptDirectory(p.string() + "/?.lua");
	// Load a script and update it for a few time steps
	u.loadScript(argv[2]);

	for (int i=0;i<10;i++){
		u.update(0.01);
	}

	return 0;
}
