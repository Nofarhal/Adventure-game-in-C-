#pragma once
#include <string>
#include <vector>
#include <conio.h>
#include <fstream>

constexpr int GAME_SPEED = 15; // ms
constexpr int FAST_GAME_SPEED = 5; // ms
constexpr int TORCH_RADIUS = 5;
static constexpr int MAX_X = 79;
static constexpr int MAX_Y = 24;

constexpr char FINISH_GAME = -1;

enum class Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	STAY = -1
};

struct Position {
	int x;
	int y;

	Position() : x(-1), y(-1) {}

	Position(int _x, int _y) : x(_x), y(_y) {}

	bool operator== (Position const& other) const {
		return (this->x == other.x && this->y == other.y);
	}

	bool operator!= (Position const& other) const {
		return !(*this == other);
	}

	Position operator + (const Position& other) const {
		return Position(x + other.x, y + other.y);
	}

	Position operator - (const Position& other) const {
		return Position(x - other.x, y - other.y);
	}
	bool operator< (const Position& other) const {
		if (y != other.y) return y < other.y;
		return x < other.x;
	}

	int operator* (const Position& other) const {
		return x * other.x + y * other.y;
	}

	bool is_in_radius(const Position& center, int radius) const;
};

void gotoxy(int x, int y);
void hideCursor();
void showCursor();
void cls();
Direction opposite_dir(Direction dir);
bool should_act(long tick, int speed);

namespace keys {
	// Player 1 controls
	constexpr char P1_UP = 'w';
	constexpr char P1_DOWN = 'x';
	constexpr char P1_LEFT = 'a';
	constexpr char P1_RIGHT = 'd';
	constexpr char P1_STAY = 's';
	constexpr char DROP_P1_ITEM = 'e';
	// Player 2 controls
	constexpr char P2_UP = 'i';
	constexpr char P2_DOWN = 'm';
	constexpr char P2_LEFT = 'j';
	constexpr char P2_RIGHT = 'l';
	constexpr char P2_STAY = 'k';
	constexpr char DROP_P2_ITEM = 'o';
	// Riddle options
	constexpr char OPTION_A = 'a';
	constexpr char OPTION_B = 'b';
	constexpr char OPTION_C = 'c';
	constexpr char OPTION_D = 'd';
	constexpr char DUMMY_OPTION = 'z';
	// Pause menu
	constexpr char QUIT_GAME = 'h';
	constexpr char ESC = 27;
	constexpr char SAVE_AND_QUIT = 's';
}

bool is_legal_char(char ch);


namespace game_consts {
	constexpr int RIDDLE_SCORE_REWARD = 10;
	constexpr int WINNING_SCORE = 50;
	constexpr int MAX_HEALTH = 3;
}

namespace colors {
	constexpr const char* GREEN = "\033[32m";
	constexpr const char* CYAN = "\033[36m";
	constexpr const char* WHITE = "\033[37m";
	constexpr const char* YELLOW = "\033[33m";
	constexpr const char* BLUE = "\033[34m";
	constexpr const char* TURQUOISE = "\033[96m";
	constexpr const char* RED = "\033[31m";
	constexpr const char* PINK = "\033[35m";
}

bool toInt(const std::string& s, int& result);