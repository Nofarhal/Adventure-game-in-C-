#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include "Room.h"
#include <sstream>

namespace ElementTypes {
	const std::string PLAYER1 = "Player1";
	const std::string PLAYER2 = "Player2";
	const std::string ENEMY = "Enemy";
	const std::string KEY = "Key";
	const std::string TORCH = "Torch";
	const std::string HEART = "Heart";
	const std::string BOMB = "Bomb";
	const std::string OBSTACLE = "Obstacle";
	const std::string BREAKABLE_WALL = "Breakable_wall";
	const std::string RIDDLE = "Riddle";
	const std::string SPIKES = "Spikes";
	const std::string LIGHT_SWITCH = "Light_switch";
	const std::string SUMMON_ENEMY = "Summon_enemy";
	const std::string DYNAMIC_WALL = "Dynamic_wall";
	const std::string PRESSURE_PLATE = "Pressure_plate";
	const std::string SPRING = "Spring";
	const std::string DOOR_SWITCH = "Door_switch";
	const std::string DOOR_KEYS = "Door_keys";
	const std::string REQUIRED_KEYS = "Required_keys";
	const std::string REQUIRED_SWITCHES = "Required_switches";
	const std::string DARK = "Dark";
	const std::string LOCKED = "Locked";
	const std::string DOOR_DESTINATION = "Door_dest";
}

namespace MenuOptions {
	const int CONTINUE_ANYWAY = 1;
	const int RETURN_TO_MENU = 2;
}

namespace MapSymbols {
	const char LEGEND = 'L';
	const char WALL = 'W';
	const char EMPTY = ' ';
}

namespace DirectionStrings {
	const std::string UP = "UP";
	const std::string DOWN = "DOWN";
	const std::string LEFT = "LEFT";
	const std::string RIGHT = "RIGHT";
}

class ScreenManager {
private:
	bool ignore_legend_warnings = false;
	const int INV_WIDTH = 35;
	const int INV_HEIGHT = 2;

	bool isValidPos(int x, int y) const;
	Direction stringToDirection(const std::string& dirStr) const;

	bool loadRoom_elements(std::ifstream& file, Room& room, std::string& filename);
	bool manage_legend(std::string& filename, std::vector<std::string> mapData, Room& room);

public:
	bool print_extra_files(std::string filename) const;
	bool loadRoomScreen(std::string& filename, Room& room);
};