#include "GameManager.h"
#include "utils.h"

void parse_cmd_args(int argc, char* argv[], settings& setts) {

	hideCursor();
	cls();

	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];
		if (arg == "-save")
			setts.save_mode = true;
		else if (arg == "-load")
			setts.load_mode = true;
		else if (arg == "-silent")
			setts.silent_mode = true;
	}

	if (!setts.load_mode)
		setts.silent_mode = false;
}

int main(int argc, char* argv[]) {
	settings setts;
	parse_cmd_args(argc, argv, setts);

	GameManager gm;
	return gm.run(setts);

	return 0;
}

