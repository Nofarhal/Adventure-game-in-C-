#pragma once
#include <string>
#include "utils.h"
#include <assert.h>

class Room;
class Player;
class Game;

enum class Item {
	NONE,
	KEY,
	TORCH,
	BOMB,
	HEART
};

enum class ARTIF_TYPE {
	LIGHT_SWITCH,
	SUMMON_ENEMY,
	SPIKES,
	RIDDLE,
	SPRING,
	DOOR_SWITCH,
	PRESSURE_PLATE
};

class Artifact {
	ARTIF_TYPE type;
	Position pos;
	Direction dir = Direction::STAY;
	int room_id;
	int target_door_id;
	bool status = false;
	long* tick = nullptr;
	std::ofstream* game_result = nullptr;
	Game* owner;

public:
	Artifact() {}
	Artifact(ARTIF_TYPE _type, Position _pos, Game* i_owner) : type(_type), pos(_pos), owner(i_owner) {}
	Artifact(ARTIF_TYPE _type, Position _pos, int _target_id) : type(_type), pos(_pos), target_door_id(_target_id) { assert(type == ARTIF_TYPE::DOOR_SWITCH || type == ARTIF_TYPE::PRESSURE_PLATE); }
	Artifact(ARTIF_TYPE _type, Position _pos, Direction _dir) : type(_type), pos(_pos), dir(_dir) { 
		assert(type == ARTIF_TYPE::SPRING);
		status = true;
	};
	bool activate(Room& r, Player& p);

	int get_target_door_id() const { return target_door_id; }
	bool is_active() const { return status; }
	void set_active() { status = !status; }

	Position get_pos() const { return pos; }
	bool is_type(ARTIF_TYPE ref_tp) const { return ref_tp == type; }
	ARTIF_TYPE get_type() const { return type; }
	Direction get_direction() const { return dir; }

	bool showRiddleOnScreen(Room& r, Player& p);

	void set_files(long* _tick, std::ofstream* _game_result) {
		tick = _tick;
		game_result = _game_result;
	}
};