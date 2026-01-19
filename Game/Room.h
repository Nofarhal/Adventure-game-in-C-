#pragma once
#include "Obstacle.h"
#include "Bomb.h"
#include "Riddle.h"
#include "Drawer.h"
#include "Screen_manager.h"
#include <map>

class Game;

namespace MapChars{
	static constexpr char WALL = 'W';
	static constexpr char FIRST_DOOR = '0';
	static constexpr char LAST_DOOR = '9';
}

namespace LoserScreen {
	static const int LOSER_NUM_OF_SCREENS = 6;
	static const int LOOP_SCREENS = 3;
}

enum class SpecialPositions {
	REGULAR,
	WALL,
	DOOR,
	ITEM,
	EDGE,
	OBSTACLE,
	ATRIFACT
};

class Room {
private:
	long* tick;
	int id;
	std::vector<std::string> mapData;
	std::map<Position, int> breakable_walls;
	bool player1_active, player2_active;
	Position starting_position1;
	Position starting_position2;
	std::vector<std::pair<Item, Position>> items_in_room;
	std::vector<Artifact> artifacts_in_room;
	Player* player1 = nullptr, *player2 = nullptr;
	std::vector <Obstacle> obstacles;
	std::vector<DynamicWall> dynamic_walls;
	std::map<int, int> door_destinations;
	std::map<int, int> door_switch_count;
	std::map<int, int> door_switch_needed;
	std::map<int, int> door_keys_count;
	std::map<int, int> door_keys_needed;
	std::pair<int, int> is_item_at(Position pos) const;
	Enemy enemy;
	bool dark = false;
	bool locked = false;
	Position legend_pos;
	Drawer dr;
	std::ofstream* game_result;
	bool silent_mode = false;
	Game* owner;

	std::vector<Bomb> activeBombs;

public:
	Room() {};
	Room(int map_id, long* _tick, Game* i_owner);

	void initialize(Player* p1, Player* p2);
	int run(std::ofstream* game_prog, std::ofstream* game_res, bool i_silent_mode, int sleep_time);

	const std::vector<std::pair<Item, Position>>* get_items_in_room() const { return &items_in_room; }
	const std::vector<Artifact>* get_artifacts_in_room() const { return &artifacts_in_room; }
	const std::vector <Obstacle>* get_obstacle_in_room() const { return &obstacles; }
	const std::vector<DynamicWall>* get_dynamic_walls_in_room() const { return &dynamic_walls; }
	std::pair<const Player const*, const Player const*> get_players() const { return std::make_pair(player1, player2); }
	Enemy const* get_enemy() const { return &enemy; }
	Drawer* get_drawer() { return &dr;  }
	std::vector<Bomb>* get_active_booms() { return &activeBombs; }
	const std::map<Position, int>* get_breakable_walls() const { return &breakable_walls; }
	bool handle_keypress();
	int handlePause(std::string& saveName);
	int handle_player_action(const std::pair<SpecialPositions, int> next_pos, const int player_id, const long tick);
	int process_player_move(Player* player, int player_id, int next_x, int next_y, long tick);
	void process_spring_hit(Player* giver, Player* reciver, int& next_x, int& next_y);
	int update_room_state(long tick);
	void update_enemy(long tick);
	std::pair<SpecialPositions, int> nextPosType(int x, int y, int id) const;
	void set_door_destination(int door_id, int destination_room) { door_destinations[door_id] = destination_room; }
	int get_door_destination(int door_id) const;

	// Items and artifacts
	void scan_and_build_obstacles();
	void add_obstacle(int x, int y) { obstacles.emplace_back(x, y); }
	void drop_item(int player_id);

	
	//Bombs
	void updateBomb(long tick);
	void explodeBomb(const Bomb& bomb);
	char get_tile_char(int x, int y) const;

	void switch_dark() { dark = !dark; }
	void wake_enemy() { enemy.wake_up(); }
	void activate_door_switch(Artifact& art);

	void update_pressure_plates();
	void update_dynamic_walls();

	// files
	void Loser();

	void setMapData(const std::vector<std::string>& newMapData) { mapData = newMapData; }
	void setPlayerStartPos(int id, Position pos);
	void addItem(Item item, Position pos) { items_in_room.push_back(std::make_pair(item, pos)); }
	void addArtifact(Artifact art) { artifacts_in_room.push_back(art); }
	void addDynamicWall(int x, int y, int id) { dynamic_walls.emplace_back(x, y, id); }
	void addBreakableWall(Position pos, int hit_count) { breakable_walls[pos] = hit_count; }
	void setEnemy(Position pos) { enemy = Enemy(pos, true); }
	void set_required_switches(int door_id, int amount) { door_switch_needed[door_id] = amount; }
	void set_required_keys(int door_id, int amount) { door_keys_needed[door_id] = amount; }
	void setDark(bool is_dark) { dark = is_dark; }
	void setLocked(bool is_locked) { locked = is_locked; }
	void set_legend_pos(int x, int y) { legend_pos = Position(y, x); }
	Game* get_owner() const { return owner; }

	void check_spring_release(Player* p);
	bool should_block_input(Player* p) const;

	void save_room(std::ostream& file) const;
	void load_room(std::istream& file);
};


