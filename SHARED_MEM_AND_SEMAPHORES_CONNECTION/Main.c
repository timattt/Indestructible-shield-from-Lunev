#include "../UTILS/Utils.h"
#include "ConsoleWriter/ConsoleWriter.h"
#include "FileReader/FileReader.h"

int Task3(int argc, char * argv[]) {
	if (argc < 2) {
		ERROR("too fex arguments");
	}

	if (argv[1][0] == 'r') {
		if (argc < 3) {
			ERROR("too fex arguments");
		}
		INPUT_FILE_NAME = argv[2];
		rDO_STEPS();
	}
	if (argv[1][0] == 'w') {
		wDO_STEPS();
	}

	return 0;
}
