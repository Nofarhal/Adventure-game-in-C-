#pragma once
#include "utils.h"
#include "Object.h"
#include "Enemy.h"
#include "Player.h"
#include "Obstacle.h"
#include "DynamicWall.h"
#include "Bomb.h"
#include <map>



// Order of the importance, if they are on the same tile the more important gets drawn (enemy most important)
enum class ICON {
	I_ACTIVE_BOMB,
	I_ENEMY,
	I_PLAYER1,
	I_PLAYER2,
	I_TORCH,
	I_DARK,
	I_BOMB,
	I_KEY,
	I_HEART,
	I_DOOR_SWITCH_ON,
	I_DOOR_SWITCH_OFF,
	I_LIGHT_SWITCH,
	I_SUMMON_ENEMY,
	I_RIDDLE,
	I_OBSTACLE,
	I_SPRING,
	I_SPIKES,
	I_PRESSURE_PLATE,
	I_DYNAMIC_WALL,
	I_SPRING_COMPRESSED,
	I_BREAKABLE_WALL,
	I_ROOM,
	I_NONE
};

class Drawer;

struct Tile {
	bool up_to_date = true;
	std::vector<std::pair<ICON, int>> objects_in_tile;
	Position pos;
	char printed_char = ' ';

	bool is_identical_data(const Tile& other, const Drawer& dr) const;
};

class Drawer {
private:
	std::vector<std::vector<Tile>> room_map;
	const std::vector<std::string>* mapData;
	const std::vector<std::pair<Item, Position>>* items;
	const std::vector<Artifact>* artifacts;
	const std::vector <Obstacle>* obstacles;
	const std::vector<DynamicWall>* dynamic_walls;
	Enemy const* enemy;
	std::pair<const Player const*, const Player const*> players;
	bool* dark;
	Position pos;
	std::vector <Bomb>* activeBombs;
	const std::map<Position, int>* breakable_walls;

	std::string icon_to_colorable(const ICON icon) const;

	char get_tile_char(int x, int y) const;

public:
	Drawer() {}

	Drawer(const std::vector<std::string>* _mapData,
		const std::vector<std::pair<Item, Position>>* _items,
		const std::vector<Artifact>* _artifacts,
		const std::vector <Obstacle>* _obstacles,
		const std::vector<DynamicWall>* _dynamic_walls,
		Enemy const* _enemy,
		std::pair<const Player const*, const Player const*> _players,
		bool* _dark,
		std::vector <Bomb>* bombs,
		const std::map<Position, int>* breakable);

	static bool COLOR_MODE;

	void draw_obj(Tile& tl, const int id_to_draw) const;
	void draw_room(bool force_redraw = false);

	char IconToChar(const ICON icon, const Position pos = Position(-1, -1)) const;
	char IconToChar(const ICON icon, const int id) const;
	ICON item_to_icon(Item it) const;
	ICON artif_to_icon(Artifact ar) const;

	void build_tiles();
	void draw_inventory(Position start_pos = Position(29,23)) const;
};