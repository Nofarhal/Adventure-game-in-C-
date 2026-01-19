#pragma once
#include "utils.h"
#include "Object.h"	

class Player {
private:
	int id;
	Position pos;
	int type; // 0 - WDXAS, 1 - ILMJK
	Direction selected_direction;
	Item held_items = Item::NONE;
	int room_id;
	int speed = 25; // How many pixel moves per second
	bool left = false;
	int score = 0;
	int health = game_consts::MAX_HEALTH;

	// spring 
	int spring_counter = 0;
	int speed_spring = 0;
	int spring_cycles_left = 0;
	Direction flight_dir = Direction::STAY;
	Direction spring_dir = Direction::STAY;

public:
	Player(int _id, int _type) : id(_id), type(_type), selected_direction(Direction::STAY), held_items(Item::NONE), room_id(-1), score(0), health(game_consts::MAX_HEALTH) {}

	void set_position(int x, int y) { pos.x = x; pos.y = y; }

	void set_position(Position i_pos) { pos = i_pos; }

	void set_room(int r_id) { room_id = r_id; }

	int get_room() const { return room_id; }

	void set_selected_direction(Direction dir) { selected_direction = dir; }

	Direction get_selected_direction() const { return selected_direction; }

	void next_position(int& x, int& y, long tick, bool update = false);

	void update_position(long tick);

	void set_left(bool _left) { left = _left; }

	bool get_left() const { return left; }

	int get_force()	const;

	Position getPosition() const { return pos; }
	int getX() const { return pos.x; }
	int getY() const { return pos.y; }

	Item get_item() const { return held_items; }
	Item use_item();
	void pick_item(Item item);
	void item_used() { held_items = Item::NONE; }
	bool has_item() const { return held_items != Item::NONE; }

	int get_score() const { return score; }
	void add_score(int points) { score += points; }
	void reset_score() { score = 0; }

	int get_id() const { return id; }
	int get_health() const { return health; }
	void add_health(int amount) { health += amount; if (health > game_consts::MAX_HEALTH) health = game_consts::MAX_HEALTH; }
	void remove_health(int amount) { health -= amount; if (health < 0) health = 0; }
	bool is_alive() const { return health; }


	// spring func
	bool is_on_spring() const { return spring_counter > 0; }
	bool is_under_spring_effect() const { return spring_cycles_left > 0; }
	void reset_spring();
	int get_spring_counter() const { return spring_counter; }
	void release_spring(std::vector<Artifact>& artifacts_in_room);
	Direction get_spring_direction() const { return spring_dir; }
	Direction get_flight_direction() const { return flight_dir; }

	void step_on_spring(Direction _spring_dir) { spring_counter++; spring_dir = _spring_dir; }
	void transfer_spring_energy(Player& other);

	void save_player(std::ostream& file) const;
	void load_player(std::istream& file);

};
