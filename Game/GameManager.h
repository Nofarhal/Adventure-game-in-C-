#pragma once
#include "OpenScreen.h"
#include "Game.h"

struct settings {
	bool save_mode = false;
	bool load_mode = false;
	bool silent_mode = false;
};

class GameManager {
public:
	int run(const settings& setts);
	void check_test_result();
};
	