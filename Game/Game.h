#pragma once
#include "Room.h"

struct inst {
	int tick;
	char key;
};

class Game {
private:
	std::vector<Room> rooms;
	Player player1, player2;
	int active_room = -1;
	bool is_valid_game = true;
	bool is_loaded_game = false;

protected:
	long tick = 0;
	std::ofstream game_progression;
	std::ofstream game_result;
	inst next_inst;
	bool silent_mode = false;
	bool load_mode;
	bool save_mode;

	virtual void record_initial_state() {};
	virtual int get_sleep_time() const = 0;
public:
	Game();

	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

	~Game() {
		game_progression.close();
		game_result.close();
	}

	void run();

	void select_room(int num_room);

	bool is_silent_mode() const { return silent_mode; }
	bool is_load_mode() const { return load_mode; }
	bool is_save_mode() const { return save_mode; }

	void saveGame(const std::string& filename) const;
	bool loadGame(const std::string& filename);

	virtual char my_getch(long tick) = 0;
	void check_test_result();	

	static unsigned long random_seed;
	static std::ifstream instructions;
};

class Game_by_input : public Game {

public:
	Game_by_input(bool _silent_mode);	
	char my_getch(long tick) override;
	void record_initial_state() override;
	int get_sleep_time() const override;
};

class Game_by_user : public Game {

public:
	Game_by_user(bool _save_mode);
	char my_getch(long tick) override;
	void record_initial_state() override;
	int get_sleep_time() const override;
};
