#include "../UTILS/Utils.h"
#include "ProcessManager.h"

int main(int argc, char * argv[]) {
	if (argc < 2) {
		ERROR("Too few arguments");
	}

	n = (atoi(argv[1]));

	make_steps();

	return 0;
}
