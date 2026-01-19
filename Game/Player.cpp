#include "Player.h"

void Player::next_position(int& x, int& y, long tick, bool update) {
	x = pos.x;
	y = pos.y;

	if (should_act(tick, speed_spring * speed) && spring_cycles_left > 0) {

		if (update)
			spring_cycles_left--;

		if (spring_cycles_left == 0) // keep going in the same direction but in the normal speed after the energy of the spring is over 
			selected_direction = flight_dir;

		switch (flight_dir) {
			case Direction::UP: y--; break;
			case Direction::DOWN: y++; break;
			case Direction::LEFT: x--; break;
			case Direction::RIGHT: x++; break;
			default: break;
		}
	}
	
	// Regular movement
	if (should_act(tick, speed)) {

		switch (selected_direction) {
			case Direction::UP: y--; break;
			case Direction::DOWN: y++; break;
			case Direction::LEFT: x--; break;
			case Direction::RIGHT: x++; break;
			default: break;
		}
	}
}

void Player::pick_item(Item item)
{
	if (held_items==Item::NONE) {
		held_items = item;
	}
}

Item Player::use_item() {
	Item temp = held_items;
	held_items = Item::NONE; // Remove the item after use
	return temp;
}

void Player::update_position(long tick) {
	next_position(pos.x, pos.y, tick, true);
}



// spring
void Player::release_spring(std::vector<Artifact>& artifacts_in_room) {
	if (!spring_counter) 
		return;

	speed_spring = spring_counter;
	spring_cycles_left = spring_counter * spring_counter;

	flight_dir = opposite_dir(spring_dir);
	spring_counter = 0;

	selected_direction = Direction::STAY;

	for (auto& ar : artifacts_in_room) {
		if (ar.get_pos() == pos) {
			ar.set_active();
			break;
		}
	}
 }
 
void Player::reset_spring() {
	spring_counter = 0;
	spring_cycles_left = 0;
	speed_spring = 0;
	flight_dir = Direction::STAY;
}
 
void Player::transfer_spring_energy(Player& other) {
	other.speed_spring = this->speed_spring;
	other.spring_cycles_left = this->spring_cycles_left;
	other.flight_dir = this->flight_dir;

	other.set_selected_direction(Direction::STAY);
}

int Player::get_force()	const {
	if (is_under_spring_effect())
		return (speed_spring > 1) ? speed_spring : 1;
	return 1;
}

void Player::save_player(std::ostream& file) const
{
	file << id << ' ' << type << ' ' << pos.x << ' ' << pos.y << ' ' << room_id << std::endl;
	file << static_cast<int>(selected_direction) << ' ' << static_cast<int>(held_items) << ' ' << score << ' ' << health << ' ' << left << std::endl;
	file << spring_counter << ' ' << speed_spring << ' ' << spring_cycles_left << ' ' << static_cast<int>(flight_dir) << ' ' << static_cast<int>(spring_dir) << std::endl;
}

void Player::load_player(std::istream& file)
{
	int dir_int, item_int, flight_dir_int, spring_dir_int;
	file >> id >> type >> pos.x >> pos.y >> room_id;
	file >> dir_int >> item_int >> score >> health >> left;

	selected_direction = static_cast<Direction>(dir_int);
	held_items = static_cast<Item>(item_int);
	file >> spring_counter >> speed_spring >> spring_cycles_left >> flight_dir_int >> spring_dir_int;
	flight_dir = static_cast<Direction>(flight_dir_int);
	spring_dir = static_cast<Direction>(spring_dir_int);


}
