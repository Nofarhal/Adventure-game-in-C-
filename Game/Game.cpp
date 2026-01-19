#include "Game.h"
#include <windows.h>

unsigned long Game::random_seed;
std::ifstream Game::instructions;

Game::Game() : player1(0, 0), player2(1, 1) {
	ScreenManager screenManager;

	std::vector<std::string> level_files;

	WIN32_FIND_DATAA findFileData;
	HANDLE hFind = FindFirstFileA("*.*", &findFileData);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				std::string filename = findFileData.cFileName;
				if (filename.find(".screen") != std::string::npos) {
					level_files.push_back(filename);
				}
			}
		} while (FindNextFileA(hFind, &findFileData) != 0);
		FindClose(hFind);
	}

	std::sort(level_files.begin(), level_files.end());

	for (size_t i = 0; i < level_files.size(); i++) {
		rooms.emplace_back(i, &tick, this);
		if (!screenManager.loadRoomScreen(level_files[i], rooms[i])) {
			is_valid_game = false;
			std::cout << "Press any key to exit..." << std::endl;
			_getch();
			return;
		}
	}

	select_room(0);
	random_seed = time(nullptr);
}

void Game::select_room(int num_room) {
	active_room = num_room;
	rooms[active_room].initialize(&player1, &player2);
}

void Game::run() {
	if (!is_valid_game)
		return;
	
	record_initial_state();

	ScreenManager screenManager;

	while (true) {

		int sleep_time = get_sleep_time();

		int next_room = rooms[active_room].run(&game_progression, &game_result, silent_mode, sleep_time);

		if (next_room == -1)
			break;

		else if (next_room == (int)rooms.size()) {
			int score = player1.get_score() + player2.get_score();
			game_result << "Game ended at: " << (int)tick << std::endl;
			game_result << "Player 1 Score: " << player1.get_score() << std::endl;
			game_result << "Player 2 Score: " << player2.get_score() << std::endl;

			cls();
			if (!screenManager.print_extra_files("winning_room.txt")) 
				std::cout << "YOU WIN!!\nError loading winning screen file" << std::endl;
			Sleep(3000);
			return;
		}

		game_result << "Room complete, room changed to: " << next_room << " at tick " << (int)tick << std::endl;

		select_room(next_room);
	}
}


void Game::saveGame(const std::string& filename) const {
	std::ofstream file(filename);
	if (!file)
		return;

	file << active_room << ' ' << Drawer::COLOR_MODE <<'\n';
	player1.save_player(file);
	player2.save_player(file);

	for (int i = 0; i < rooms.size(); i++)
		rooms[i].save_room(file);

	file.close();
}

bool Game::loadGame(const std::string& filename) {
	std::ifstream file(filename);
	if (!file)
		return false;

	bool load_color_mode;
	is_loaded_game = true;

	file >> active_room >> load_color_mode;

	Drawer::COLOR_MODE = load_color_mode;

	player1.load_player(file);
	player2.load_player(file);

	Position p1_pos = player1.getPosition();
	Position p2_pos = player2.getPosition();
	bool p1_left = player1.get_left();
	bool p2_left = player2.get_left();

	for (int i = 0; i < rooms.size(); i++) {
		rooms[i].initialize(&player1, &player2);
		rooms[i].load_room(file);
	}

	player1.set_position(p1_pos);
	player2.set_position(p2_pos);
	player1.set_left(p1_left);
	player2.set_left(p2_left);

	file.close();
	return true;
}




Game_by_input::Game_by_input(bool _silent_mode) : Game() {
	silent_mode = _silent_mode;
	load_mode = true;
	instructions = std::ifstream("adv-world.steps");
	if (silent_mode) {
		save_mode = true;
		game_result = std::ofstream("current_run.result");
	}

	next_inst = inst(0, 0);

	instructions >> random_seed;
}

char Game_by_input::my_getch(long tick) {
	if (next_inst.tick == tick || next_inst.tick + 100 < tick) {
		char ch = next_inst.key;
		if (!instructions.eof())
			instructions >> next_inst.tick >> next_inst.key;
		else
			return FINISH_GAME;

		return std::tolower(ch);
	}

	return 0;
}

void Game_by_input::record_initial_state() {
	if (silent_mode) {
		game_result << "Game started at: " << (int)tick << std::endl;
		game_result << "Seed: " << random_seed << std::endl;
	}
}

int Game_by_input::get_sleep_time() const {
	if (silent_mode)
		return 0;
	return FAST_GAME_SPEED;
}





Game_by_user::Game_by_user(bool _save_mode) : Game() {
	save_mode = _save_mode;

	if (save_mode) {
		game_progression = std::ofstream("adv-world.steps");
		game_result = std::ofstream("adv-world.result");
	}
}

char Game_by_user::my_getch(long tick) {
	if (!_kbhit())
		return 0;

	char ch = _getch();
	if (save_mode && is_legal_char(ch))
		game_progression << tick << " " << ch << std::endl;

	return std::tolower(ch);
}

void Game_by_user::record_initial_state() {
	if (save_mode) {
		if (!game_progression) {
			std::cerr << "Error opening game progression file for writing." << std::endl;
			return;
		}
	}

	game_result << "Game started at: " << (int)tick << std::endl;
	game_result << "Seed: " << random_seed << std::endl;

	game_progression << random_seed << '\n';
}


int Game_by_user::get_sleep_time() const {
	return GAME_SPEED;
}