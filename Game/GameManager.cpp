#include "GameManager.h"


void GameManager::check_test_result() {
	std::ifstream result_file("adv-world.result");
	std::ifstream current_run_file("current_run.result");

	std::vector<std::string> result_lines;
	std::vector<std::string> current_lines;
	std::string line;

	while (std::getline(result_file, line))
		result_lines.push_back(line);

	while (std::getline(current_run_file, line))
		current_lines.push_back(line);

	result_file.close();
	current_run_file.close();

	bool passed = true;
	std::string fail_reason;

	size_t min_lines = result_lines.size();
	if (current_lines.size() < min_lines)
		min_lines = current_lines.size();

	for (size_t i = 0; i < min_lines; ++i) {
		if (result_lines[i] != current_lines[i]) {
			passed = false;
			fail_reason = "Difference at line " + std::to_string(i + 1);
			std::cout << "Expected: " << result_lines[i] << std::endl;
			std::cout << "Got:      " << current_lines[i] << std::endl;
			break;
		}
	}

	if (passed)
		if (result_lines.size() != current_lines.size()) {
			passed = false;
			fail_reason = "Number of lines is different between files.";
		}

	if (passed)
		std::cout << "Test passed successfully!" << std::endl;
	else
		std::cout << "Test failed: " << fail_reason << std::endl;

	std::remove("current_run.result");
}

int GameManager::run(const settings& setts) {

	Riddle riddle;
	if (!riddle.loadRiddles("riddles.txt")) {
		std::cerr << "Failed to load riddles. Exiting game." << std::endl;
		std::cout << "Press Enter to exit..." << std::endl;
		_getch();
		return 0;
	}

	while (true) {
		Game_Status response;
		OpenScreen theScreen;
		Game* g;

		if (!setts.load_mode) {
			response = theScreen.runGame();
			g = new Game_by_user(setts.save_mode);
		}
		else {
			response = Game_Status::No_color;
			g = new Game_by_input(setts.silent_mode);
		}

		if (response == Game_Status::Exit) {
			cls();
			std::cout << "Game exited. Goodbye!" << std::endl;
			return 0;
		}
		else if (response == Game_Status::Load_game) {
			cls();

			std::string filename = theScreen.get_selected_saved_file();
			if (filename != "" && g->loadGame(filename))
				g->run();
			else {
				std::cout << "Error: save file not found!" << std::endl;
				std::cout << "Press any key to return to main menu..." << std::endl;
				_getch();
			}
		}
		else {
			cls();
			if (response == Game_Status::Color)
				Drawer::COLOR_MODE = true;
			else
				Drawer::COLOR_MODE = false;

			g->run();
		}

		delete g;

		if (setts.load_mode) {
			cls();
			if (setts.silent_mode)
				check_test_result();
			std::cout << "Game ended. Goodbye!" << std::endl;
			return 0;
		}
	}
}

