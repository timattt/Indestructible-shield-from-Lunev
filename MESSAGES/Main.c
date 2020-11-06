#include "../UTILS/Utils.h"
#include "ProcessManager.h"

int Task2(int argc, char * argv[]) {
	if (argc < 2) {
		ERROR("Too few arguments");
	}

	n = STRING_TO_INT(argv[1]);

	make_steps();

	return 0;
}
