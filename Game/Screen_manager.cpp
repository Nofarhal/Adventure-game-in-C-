#include "Screen_manager.h"

bool ScreenManager::isValidPos (int x, int y) const {
	return (x >= 0 && x <= MAX_X && y >= 0 && y <= MAX_Y);
}

Direction ScreenManager::stringToDirection(const std::string& dirStr) const {
	if (dirStr == DirectionStrings::UP) return Direction::UP;
	if (dirStr == DirectionStrings::DOWN) return Direction::DOWN;
	if (dirStr == DirectionStrings::LEFT) return Direction::LEFT;
	if (dirStr == DirectionStrings::RIGHT) return Direction::RIGHT;
	return Direction::STAY;
}

bool ScreenManager::print_extra_files(std::string filename) const
{
	std::ifstream in(filename);

	if (!in) {
		std::cerr << "Could not open the file " << filename << " for reading." << std::endl;
		return false;
	}

	std::string line;
	while (getline(in, line)) {
		std::cout << line << std::endl;
	}

	return true;
}

bool ScreenManager::loadRoomScreen(std::string& filename, Room& room) {
	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Could not open the file " << filename << " for reading." << std::endl;
		return false;
	}

	std::vector<std::string> mapData;
	std::string line;
	int row = 0;

	while (row <= MAX_Y && getline(file, line)) {
		if (line.find("Elements") == 0)
			break;

		if (line.size() <= MAX_X) 
			line.resize(MAX_X + 1 , ' ');

		mapData.push_back(line);
		row++;
	}

	while (mapData.size() <= MAX_Y) {
		std::string empty_line(MAX_X + 1, ' ');
		mapData.push_back(empty_line);
	}

	if (!manage_legend(filename, mapData, room))
		return false;

	room.setMapData(mapData);

	if (!loadRoom_elements(file, room, filename)) 
		return false;
	
	file.close();
	return true;
}

bool ScreenManager::loadRoom_elements(std::ifstream& file, Room& room, std::string& filename) {
	std::string line;

	while (getline(file, line)) {
		if (line.empty() || line == "Elements")
			continue;

		std::stringstream ss(line);
		std::string kewWord;
		ss >> kewWord;

		int x, y, id, target_door, target_wall, hit_count, amount;
		std::string dir;

		if (kewWord == ElementTypes::PLAYER1 || kewWord == ElementTypes::PLAYER2 || kewWord == ElementTypes::ENEMY ||
			kewWord == ElementTypes::KEY || kewWord == ElementTypes::TORCH || kewWord == ElementTypes::HEART || kewWord == ElementTypes::BOMB || kewWord == ElementTypes::OBSTACLE || kewWord == ElementTypes::BREAKABLE_WALL) {

			ss >> x >> y;

			if (!isValidPos(x, y)) {
				std::cerr << "ERROR: Invalid position (" << x << "," << y << ") for " << kewWord << " in file " << filename << std::endl;
				return false;
			}

			if (kewWord == ElementTypes::PLAYER1 )
				room.setPlayerStartPos(1, Position(x, y));
			else if (kewWord == ElementTypes::PLAYER2)
				room.setPlayerStartPos(2, Position(x, y));
			else if (kewWord == ElementTypes::ENEMY)
				room.setEnemy(Position(x, y));
			else if (kewWord == ElementTypes::KEY)
				room.addItem(Item::KEY, Position(x, y));
			else if (kewWord == ElementTypes::TORCH)
				room.addItem(Item::TORCH, Position(x, y));
			else if (kewWord == ElementTypes::HEART)
				room.addItem(Item::HEART, Position(x, y));
			else if (kewWord == ElementTypes::BOMB)
				room.addItem(Item::BOMB, Position(x, y));
			else if (kewWord == ElementTypes::OBSTACLE)
				room.add_obstacle(x, y);
			else if (kewWord == ElementTypes::BREAKABLE_WALL) {
				ss >> hit_count;
				room.addBreakableWall(Position(x, y), hit_count);
			}
		}

		else if (kewWord == ElementTypes::RIDDLE || kewWord == ElementTypes::SPIKES || kewWord == ElementTypes::DYNAMIC_WALL || kewWord == ElementTypes::LIGHT_SWITCH || kewWord == ElementTypes::SUMMON_ENEMY || kewWord == ElementTypes::PRESSURE_PLATE ||
			kewWord == ElementTypes::SPRING || kewWord == ElementTypes::DOOR_SWITCH) {

			ss >> x >> y;

			if (!isValidPos(x, y)) {
				std::cerr << "ERROR: Invalid position (" << x << "," << y << ") for " << kewWord << " in file " << filename << std::endl;
				return false;
			}

			if (kewWord == ElementTypes::RIDDLE)
				room.addArtifact(Artifact(ARTIF_TYPE::RIDDLE, Position(x, y), room.get_owner()));
			else if (kewWord == ElementTypes::SPIKES)
				room.addArtifact(Artifact(ARTIF_TYPE::SPIKES, Position(x, y), room.get_owner()));
			else if (kewWord == ElementTypes::LIGHT_SWITCH)
				room.addArtifact(Artifact(ARTIF_TYPE::LIGHT_SWITCH, Position(x, y), room.get_owner()));
			else if (kewWord == ElementTypes::SUMMON_ENEMY)
				room.addArtifact(Artifact(ARTIF_TYPE::SUMMON_ENEMY, Position(x, y), room.get_owner()));
			else if (kewWord == ElementTypes::DYNAMIC_WALL) {
				ss >> id;
				room.addDynamicWall(x, y, id);
			}
			else if (kewWord == ElementTypes::PRESSURE_PLATE) {
				ss >> target_wall;
				room.addArtifact(Artifact(ARTIF_TYPE::PRESSURE_PLATE, Position(x, y), target_wall));
			}
			else if (kewWord == ElementTypes::SPRING) {
				ss >> dir;
				room.addArtifact(Artifact(ARTIF_TYPE::SPRING, Position(x, y), stringToDirection(dir)));
			}
			else if (kewWord == ElementTypes::DOOR_SWITCH) {
				ss >> target_door;
				room.addArtifact(Artifact(ARTIF_TYPE::DOOR_SWITCH, Position(x, y), target_door));
			}
			
		}

		else if (kewWord == ElementTypes::DOOR_KEYS) {
			int door_id, key_amount;
			ss >> door_id >> key_amount;
			room.set_required_keys(door_id, key_amount);
		}

		else if (kewWord == ElementTypes::REQUIRED_SWITCHES) {
			ss >> id >> amount;
			room.set_required_switches(id, amount);
		}

		else if (kewWord == ElementTypes::DOOR_DESTINATION) {
			int door_id, target_room;
			ss >> door_id >> target_room;
			room.set_door_destination(door_id, target_room);
		}

		else if (kewWord == ElementTypes::DARK)
			room.setDark(true);

		else if (kewWord == ElementTypes::LOCKED)
			room.setLocked(true);
	}

	return true;
}

bool ScreenManager::manage_legend(std::string& filename, std::vector<std::string> mapData, Room& room) {
	bool found_legend = false;
	for (int x = 0; x <= MAX_X; x++) {
		for (int y = 0; y <= MAX_Y; y++) {

			if (mapData[y][x] != MapSymbols::LEGEND)
				continue;

			bool pos_error = false;
			std::string error_msg = "";

			if (y >= MAX_Y) {
				pos_error = true;
				error_msg = "Legend position too low";
			}
			else {
				for (int i = 0; i < INV_HEIGHT; i++) {
					for (int j = 0; j < INV_WIDTH; j++) {
						int check_x = x + j;
						int check_y = y + i;

						if (!(check_x < mapData[check_y].size() && check_y < mapData.size()))
							continue;

						if (mapData[check_y][check_x] != MapSymbols::WALL)
							continue;
							
						pos_error = true;
						error_msg = "Legend position overlaps with map data";
						i = INV_HEIGHT;
						break;
					}
				}
			}

			if (pos_error && !ignore_legend_warnings) {
				std::cout << "\n\n";
				std::cerr << "WARNING in file: " << filename << std::endl;
				std::cerr << "Bad Legend Placement at (" << x << "," << y << "):" << std::endl;
				std::cerr << ">> " << error_msg << std::endl;
				std::cerr << "Inventory size is " << INV_WIDTH << "x" << INV_HEIGHT << " chars." << std::endl;
				std::cout << "\nChoose an action:" << std::endl;
				std::cout << "1. Continue anyway (Risk glitches)" << std::endl;
				std::cout << "2. Return to Menu (To fix the file)" << std::endl;

				char choice = ' ';
				while (choice != MenuOptions::CONTINUE_ANYWAY && choice != MenuOptions::RETURN_TO_MENU)
					choice = _getch();

				if (choice == MenuOptions::RETURN_TO_MENU)
					return false;
				else
					ignore_legend_warnings = true;
			}

			found_legend = true;
			room.set_legend_pos(y, x);
			break;
		}

		if (found_legend)
			break;
	}


	if (!found_legend) {
		std::cerr << "ERROR: Legend position 'L' not found in file " << filename << std::endl;
		return false;
	}

	return true;
}