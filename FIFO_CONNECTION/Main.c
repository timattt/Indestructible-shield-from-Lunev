#include "Base/Base.h"
#include "FileReader/FileReader.h"
#include "ConsoleWriter/ConsoleWriter.h"

int Task1(int argc, char * argv[]) {
	if (argc < 2) {
		ERROR("Too few arguments!");
	}
	if (argv[1][0] == 'r') {
		if (argc < 3) {
			ERROR("Too few arguments!");
		}
		RUN_FILE_READER(argv[2]);
	}
	if (argv[1][0] == 'w') {
		RUN_CONSOLE_WRITER();
	}

	return 0;
}
