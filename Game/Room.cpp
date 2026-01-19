#include "Room.h"
#include <windows.h>
#include "Object.h"
#include "Game.h"

#define LOSE_NUM -2
#define NO_ACTION_NUM -1

// Room management funcs

Room::Room(int map_id, long* _tick, Game* i_owner) :id(map_id) {
	tick = _tick;
	owner = i_owner;

	// Initialize room based on map_id
	items_in_room.clear();
	artifacts_in_room.clear();
	obstacles.clear();
	door_destinations.clear();
	door_switch_needed.clear();
	door_switch_count.clear();
	door_keys_count.clear();
	door_keys_needed.clear();
}

void Room::initialize(Player* p1, Player* p2) {
	if (!p1 || !p2) {
		std::cout << "ERROR NULL POINTER PLAYERS AT ROOM::DRAWPLAYERS()" << std::endl;
		return;
	}

	player1 = p1;
	player2 = p2;

	if (player1->get_room() == id)
		player1->set_left(false);
	if (player2->get_room() == id)
		player2->set_left(false);

	player1->set_position(starting_position1);
	player2->set_position(starting_position2);
	player1->set_selected_direction(Direction::STAY);
	player2->set_selected_direction(Direction::STAY);
	
	scan_and_build_obstacles();
	dr = Drawer(&mapData, get_items_in_room(), get_artifacts_in_room(), get_obstacle_in_room(), get_dynamic_walls_in_room(), get_enemy(), get_players(), &dark, get_active_booms(), get_breakable_walls());
}

int Room::run(std::ofstream* game_prog, std::ofstream* game_res, bool i_silent_mode, int sleep_time) {
	silent_mode = i_silent_mode;
	game_result = game_res;

	for (auto & art : artifacts_in_room) 
		art.set_files(tick, game_result);

	if (!silent_mode)
		dr.draw_room(true);

	while (true) {
		int door_hit = update_room_state(*tick);

		if (door_hit != NO_ACTION_NUM && door_hit != LOSE_NUM && player1->get_left() && player2->get_left())
			return door_hit;

		if (door_hit == LOSE_NUM) {
			Loser();
			return NO_ACTION_NUM;
		}

		if (!handle_keypress()) // return false if players wish to exit game
			return NO_ACTION_NUM;

		if (!silent_mode) {
			dr.draw_room();
			dr.draw_inventory(legend_pos);
			Sleep(sleep_time);
		}

		(*tick)++;
	}

	return 0;
}

// Event handlers

bool Room::handle_keypress() {
	char ch = owner->my_getch(*tick);
	if (ch == FINISH_GAME)
		return false;

	bool legal_ch = true;

	switch (ch) {
	case keys::P1_RIGHT:
		if (!should_block_input(player1))
			player1->set_selected_direction(Direction::RIGHT);
		break;
	case keys::P1_LEFT:
		if (!should_block_input(player1))
			player1->set_selected_direction(Direction::LEFT);
		break;
	case keys::P1_DOWN:
		if (!should_block_input(player1))
			player1->set_selected_direction(Direction::DOWN);
		break;
	case keys::P1_UP:
		if (!should_block_input(player1))
			player1->set_selected_direction(Direction::UP);
		break;
	case keys::P1_STAY:
		if (!should_block_input(player1))
			player1->set_selected_direction(Direction::STAY);
		break;

	case keys::P2_RIGHT:
		if (!should_block_input(player2))
			player2->set_selected_direction(Direction::RIGHT);
		break;
	case keys::P2_LEFT:
		if (!should_block_input(player2))
			player2->set_selected_direction(Direction::LEFT);
		break;
	case keys::P2_DOWN:
		if (!should_block_input(player2))
			player2->set_selected_direction(Direction::DOWN);
		break;
	case keys::P2_UP:
		if (!should_block_input(player2))
			player2->set_selected_direction(Direction::UP);
		break;
	case keys::P2_STAY:
		if (!should_block_input(player2))
			player2->set_selected_direction(Direction::STAY);
		break;
	case keys::DROP_P1_ITEM:
		drop_item(1);
		break;
	case keys::DROP_P2_ITEM:
		drop_item(2);
		break;
	case keys::ESC: {
		legal_ch = false;
		std::string saveName;
		int pause_result = handlePause(saveName);
		if (pause_result == 0)
			return false;
		else if (pause_result == 1)
			return true;
		else if (pause_result == 2) {
				owner->saveGame(saveName + ".save");
			return false;
		}
		break;
	}

	default:
		break;
	}

	return true;
}

int Room::handle_player_action(const std::pair<SpecialPositions, int> next_pos, const int player_id, const long tick) {
	auto& active_player = player_id == 1 ? player1 : player2;

	switch (next_pos.first)
	{
	case SpecialPositions::REGULAR:
		active_player->update_position(tick);
		return NO_ACTION_NUM;

	case SpecialPositions::WALL:
		if (active_player->is_under_spring_effect()) {
			active_player->reset_spring();
			return NO_ACTION_NUM;
		}

		if (active_player->is_on_spring()) 
			active_player->release_spring(artifacts_in_room);
		
		return NO_ACTION_NUM;

	case SpecialPositions::OBSTACLE:
	{
		int next_x, next_y;
		active_player->next_position(next_x, next_y, tick, false);

		Obstacle* target_obs = nullptr;
		for (auto& obs : obstacles) {
			if (obs.is_at(next_x, next_y)) {
				target_obs = &obs;
				break;
			}
		}

		if (!target_obs)
			return NO_ACTION_NUM;

		int total_force = active_player->get_force();

		Player* other_player = (player_id == 1) ? player2 : player1;
		int other_next_x, other_next_y;
		other_player->next_position(other_next_x, other_next_y, tick, false);

		if (target_obs->is_at(other_next_x, other_next_y)) {
			if (other_player->get_selected_direction() == active_player->get_selected_direction())
				total_force += other_player->get_force();
		}

		if (total_force >= target_obs->get_size()) {
			Direction push_dir;
			if (active_player->is_under_spring_effect())
				push_dir = active_player->get_flight_direction();
			else
				push_dir = active_player->get_selected_direction();

			if (target_obs->try_push(push_dir, mapData, obstacles)) {
				active_player->update_position(tick);
				scan_and_build_obstacles();
			}
		}
		return NO_ACTION_NUM;
	}


	case SpecialPositions::DOOR:
		if (next_pos.second == 2 && id == 2) { // winning door
			if (player1->get_score() < game_consts::WINNING_SCORE || player2->get_score() < game_consts::WINNING_SCORE) {
				gotoxy(20, 22);
				if (Drawer::COLOR_MODE)
					std::cout << colors::RED << "Both players need at least 50 points to win!" << colors::WHITE << std::flush;
				else
					std::cout << "Both players need at least 50 points to win!" << std::flush;
				Sleep(900);
				gotoxy(20, 22);
				std::cout << "                                                    " << std::flush;
				return NO_ACTION_NUM;
			}
		}
		
		if (door_keys_needed.count(next_pos.second)) {
			if (active_player->get_item() == Item::KEY) {
				door_keys_count[next_pos.second]++;
				active_player->use_item();
			}
			if (door_keys_count[next_pos.second] < door_keys_needed[next_pos.second])
				return NO_ACTION_NUM;
			}

		if (door_switch_needed.count(next_pos.second)) {
			if (door_switch_count[next_pos.second] < door_switch_needed[next_pos.second])
				return NO_ACTION_NUM;
		}

		if (locked && next_pos.second != 1) {
			locked = false;
			active_player->use_item();
		}
		return next_pos.second;

	case SpecialPositions::ITEM:
	{
		int next_x, next_y;
		active_player->next_position(next_x, next_y, tick);
		Item found_item = Item::NONE;
		int item_index = -1;

		for (int i = 0; i < items_in_room.size(); i++) {
			if (items_in_room[i].second.x == next_x && items_in_room[i].second.y == next_y) {
				found_item = items_in_room[i].first;
				item_index = i;
				break;
			}
		}
		if (found_item == Item::NONE) {
			active_player->update_position(tick);
			return NO_ACTION_NUM;
		}

		if (found_item == Item::HEART) {
			if (active_player->get_health() < game_consts::MAX_HEALTH) {
				active_player->add_health(1);
				*game_result << "Player " << player_id << " picked up a heart, at tick " << (int)tick << std::endl;
				items_in_room.erase(items_in_room.begin() + item_index);
			}

			active_player->update_position(tick);
			return NO_ACTION_NUM;
		}

		if (!active_player->has_item()) {
			active_player->pick_item(found_item);
			items_in_room.erase(items_in_room.begin() + item_index);
		}
	}

		active_player->update_position(tick);
		return NO_ACTION_NUM;

	case SpecialPositions::ATRIFACT:
		int next_x_a, next_y_a;
		active_player->next_position(next_x_a, next_y_a, tick);
		bool move_allowed;

		for (auto it = artifacts_in_room.begin(); it != artifacts_in_room.end(); ++it) {
			if (it->get_pos().x == next_x_a && it->get_pos().y == next_y_a) {
				
				move_allowed = it->activate(*this, *active_player);

				if (move_allowed) {
					if (it->is_type(ARTIF_TYPE::SUMMON_ENEMY) || it->is_type(ARTIF_TYPE::RIDDLE))
						artifacts_in_room.erase(it);
				}

				break;
			}
		}

		if (!active_player->is_alive())
			return LOSE_NUM;

		if(move_allowed)
			active_player->update_position(tick);

		return NO_ACTION_NUM;


	default:
		return NO_ACTION_NUM;
	}

}

int Room::handlePause(std::string& saveName) {
	const int MENU_WIDTH = 40;
	const int MENU_HEIGHT = 8;

	const int START_X = (MAX_X - MENU_WIDTH) / 2;
	const int START_Y = (MAX_Y - MENU_HEIGHT) / 2;

	for (int y = START_Y; y < START_Y + MENU_HEIGHT; y++) {
		gotoxy(START_X, y);
		for (int x = 0; x < MENU_WIDTH; x++)
			std::cout << " ";
	}

	for (int x = START_X; x < START_X + MENU_WIDTH; x++) {
		gotoxy(x, START_Y); std::cout << "-";
		gotoxy(x, START_Y + MENU_HEIGHT - 1); std::cout << "-";
	}

	for (int y = START_Y; y < START_Y + MENU_HEIGHT; y++) {
		gotoxy(START_X, y); std::cout << "|";
		gotoxy(START_X + MENU_WIDTH - 1, y); std::cout << "|";
	}

	gotoxy(START_X, START_Y); std::cout << "+";
	gotoxy(START_X + MENU_WIDTH - 1, START_Y); std::cout << "+";
	gotoxy(START_X, START_Y + MENU_HEIGHT - 1); std::cout << "+";
	gotoxy(START_X + MENU_WIDTH - 1, START_Y + MENU_HEIGHT - 1); std::cout << "+";

	const char* titleColor = Drawer::COLOR_MODE ? colors::YELLOW : "";
	const char* textColor = Drawer::COLOR_MODE ? colors::WHITE : "";

	gotoxy(START_X + (MENU_WIDTH - 11) / 2, START_Y + 2);
	std::cout << titleColor << "GAME PAUSED" << textColor;

	gotoxy(START_X + 2, START_Y + 4);
	std::cout << "Press " << titleColor << "ESC" << textColor << " to Resume";

	gotoxy(START_X + 2, START_Y + 5);
	std::cout << "Press " << titleColor << "H" << textColor << " to Quit";

	gotoxy(START_X + 2, START_Y + 6);
	std::cout << "Press " << titleColor << "S" << textColor << " to save and quit";

	while (true) {
		if (_kbhit()) {
			char ch = std::tolower(_getch());


			if (ch == keys::ESC) {
				dr.draw_room(true);
				return 1;
			}
			else if (ch == keys::QUIT_GAME)
				return 0;
			else if (ch == keys::SAVE_AND_QUIT) {
				gotoxy(START_X + 2, START_Y + 6);
				std::cout << "Name:                      ";
				gotoxy(START_X + 8, START_Y + 6);
				showCursor();
				std::cin >> saveName;
				hideCursor();
				return 2;
			}

		}
	}
}

void Room::update_enemy(long tick) {
	if (enemy.is_sleep())
		return;

	enemy.find_path(mapData, Position(player1->getX(), player1->getY()), Position(player2->getX(), player2->getY()), tick);
	if (enemy.empty_path())
		return;

	enemy.move(tick);

	if (enemy.getPosition() == player1->getPosition()) {
		player1->remove_health(game_consts::MAX_HEALTH);
		*game_result << "Player 1 killed by the enemy in room " << id << " at tick " << (int)tick << std::endl;
	}

	if (enemy.getPosition() == player2->getPosition()) {
		player2->remove_health(game_consts::MAX_HEALTH);
		*game_result << "Player 2 killed by the enemy in room " << id << " at tick " << (int)tick << std::endl;
	}
}

int Room::process_player_move(Player* player, int player_id, int next_x, int next_y, long tick) {
	bool player_move = next_x != player->getX() || next_y != player->getY();

	if (player_move && !player->get_left()) {

		auto next_pos = nextPosType(next_x, next_y, player_id);
		auto next_room = handle_player_action(next_pos, player_id, tick);
		update_pressure_plates();

		if (next_room > 0) {
			int target_room = get_door_destination(next_room);
			player->set_left(true);
			*game_result << "Player " << player_id << " exited room " << id << " to room " << (next_room == 1 ? id - 1 : id + 1) << " at tick " << (int)tick << std::endl;
			player->set_room(target_room);
			player->set_position(-1, -1);
			return target_room;
		}
	}
	return NO_ACTION_NUM;
}

void Room::process_spring_hit(Player* giver, Player* reciver, int& next_x, int& next_y) {
	if (giver->is_under_spring_effect()) {
		if (next_x == reciver->getX() && next_y == reciver->getY()) {
			giver->transfer_spring_energy(*reciver);
			next_x = giver->getX();
			next_y = giver->getY();
		}
	}
}

int Room::update_room_state(const long tick)
{
	updateBomb(tick);

	check_spring_release(player1);
	check_spring_release(player2);

	int next_x1, next_y1, next_x2, next_y2;
	player1->next_position(next_x1, next_y1, tick);
	player2->next_position(next_x2, next_y2, tick);

	// spring energy transfer
	process_spring_hit(player1, player2, next_x1, next_y1);
	process_spring_hit(player2, player1, next_x2, next_y2);

	int next_room1 = process_player_move(player1, 1, next_x1, next_y1, tick);
	int next_room2 = process_player_move(player2, 2, next_x2, next_y2, tick);

	update_enemy(tick);

	if(player1->is_alive() == false || player2->is_alive() == false)
		return LOSE_NUM;

	if (next_room1 != NO_ACTION_NUM)
		return next_room1;
	else
		return next_room2;
}

std::pair<SpecialPositions, int> Room::nextPosType(int x, int y, int id) const{
	if (x < 0 || x > MAX_X || y < 0 || y > MAX_Y)
		return std::make_pair(SpecialPositions::EDGE, 0);

	char tileChar = get_tile_char(x, y);

	if (tileChar == MapChars::WALL)
	char tileChar = mapData[y][x];

	
	if (mapData[y][x] == MapChars::WALL)
		return std::make_pair(SpecialPositions::WALL, 0);

	auto it = breakable_walls.find(Position(x, y));
	if (it != breakable_walls.end()) {
		if (it->second  >= 2)
			return { SpecialPositions::REGULAR,0 };
		return { SpecialPositions::WALL,0 };
	}

	for (auto& dw : dynamic_walls) {
		if (dw.getPosition() == Position(x, y) && dw.is_visible()) {
			return  std::make_pair(SpecialPositions::WALL, -1);
		}
	}

	if (std::isdigit(tileChar))
		return std::make_pair(SpecialPositions::DOOR, tileChar - '0');

	for(auto& item : items_in_room) {
		if (item.second.x == x && item.second.y == y)
			return std::make_pair(SpecialPositions::ITEM, 0);
	}

	for (auto& art : artifacts_in_room) {
		if (art.get_pos().x == x && art.get_pos().y == y)
			return std::make_pair(SpecialPositions::ATRIFACT, 0);
	}

	for (auto& obs : obstacles) {
		if (obs.is_at(x, y))
			return  std::make_pair(SpecialPositions::OBSTACLE, 0);
	}

	return  std::make_pair(SpecialPositions::REGULAR, 0);
}

int Room::get_door_destination(int door_id) const
{
	auto it = door_destinations.find(door_id);
	if (it != door_destinations.end())
		return it->second;
	return (door_id == 1) ? id - 1 : id + 1;
}


// Manage items and artifacts

// First value is artifact id and second is item id
std::pair<int, int> Room::is_item_at(Position pos) const {

	for (int a_id = 0; a_id < artifacts_in_room.size(); a_id++) {
		const auto& ar = artifacts_in_room[a_id];
		if (ar.get_pos() == pos)
			return std::make_pair(a_id, -1);
	}

	for (int it_id = 0; it_id < items_in_room.size(); it_id++) {
		const auto& it = items_in_room[it_id];
		if (it.second == pos)
			return std::make_pair(-1, it_id);
	}

	return std::make_pair(-1, -1);
}

void Room::scan_and_build_obstacles(){

	std::vector<Obstacle> all_potential_parts;
	for (const auto& obs : obstacles) {
		if (obs.get_size() > 0) {
			for (const auto& part : obs.get_parts())
				all_potential_parts.emplace_back(part.x, part.y);
		}
		else 
			all_potential_parts.emplace_back(obs.getX(), obs.getY());

	}

	obstacles.clear();

	std::vector<std::vector<bool>> visited(MAX_X, std::vector<bool>(MAX_Y, false));

	for (const auto& obs_part : all_potential_parts) {
		int x = obs_part.getX();
		int y = obs_part.getY();

		if (!visited[x][y]) {
			Obstacle new_obs(x, y);
			new_obs.build_obs_parts(all_potential_parts, visited);
			obstacles.push_back(new_obs);
		}
	}
}

void Room::drop_item(int player_id) {
	Player* p = player_id == 1 ? player1 : player2;
	if (!p->has_item())
		return;
	Item dropped_item = p->use_item();
	Position drop_pos = p->getPosition();

	if (dropped_item == Item::BOMB) 
		activeBombs.emplace_back(drop_pos);
	
	else 
		items_in_room.push_back(std::make_pair(dropped_item, drop_pos));
	

}

void Room::activate_door_switch(Artifact& art) {
	art.set_active();
	int val = 2 * art.is_active() - 1; // if art.is_active() is true, val = 1 else -1
	door_switch_count[art.get_target_door_id()] += val;
}

void Room::update_pressure_plates()
{
	for (auto& art : artifacts_in_room) {
		if (!art.is_type(ARTIF_TYPE::PRESSURE_PLATE))
			continue;
		Position plate_pos = art.get_pos();

		bool player_on_plate = false;

		if (!player1->get_left() && player1->getPosition() == plate_pos)
			player_on_plate = true;

		if (!player2->get_left() && player2->getPosition() == plate_pos)
			player_on_plate = true;

		bool was_active = art.is_active();

		if (player_on_plate && !was_active) {
			art.set_active();
		}
		else if (!player_on_plate && was_active) {
			art.set_active();
		}
	}
	update_dynamic_walls();
}

void Room::update_dynamic_walls()
{
	for (auto& dw : dynamic_walls) {
		int switch_id = dw.get_switch_id();

		bool should_hide = false;

		for (auto& art : artifacts_in_room) {
			if (art.is_type(ARTIF_TYPE::PRESSURE_PLATE) && art.get_target_door_id() == switch_id && art.is_active()) {
					should_hide = true;
					break;
				}
			}
		if (should_hide) {
			dw.hide();
		}
		else {
			dw.show();
		}
	}
}

char Room::get_tile_char(int x, int y) const {
	Position pos(x, y);
	auto it = breakable_walls.find(pos);
	if (it != breakable_walls.end()) {
		if (it->second < 2) return '|';
		else
		return ' ';
	}
	return mapData[y][x];
}

void Room::updateBomb(long tick) {
	for (auto it = activeBombs.begin(); it != activeBombs.end();) {
		if (it->updateBooming(tick)) {
			explodeBomb(*it);
			it = activeBombs.erase(it);
		}
		else {
			++it;
		}
	}
}

void Room::explodeBomb(const Bomb& bomb)
{
	const Position& exploding_pos = bomb.getPos();
	const int explosion_radius = Bomb::DAMAGE_RADIUS;
	
	if (player1->getPosition().is_in_radius(exploding_pos, explosion_radius) && !player1->get_left()) {
		player1->remove_health(1);
		*game_result << "Player 1 hit by a bomb explosion in room " << id << " at tick " << (int)*tick << std::endl;
		if (!player1->is_alive()) {
			Loser();
			return;
		}
	}
	if (player2->getPosition().is_in_radius(exploding_pos, explosion_radius) && !player2->get_left()) {
		player2->remove_health(1);
		*game_result << "Player 2 hit by a bomb explosion in room " << id << " at tick " << (int)*tick << std::endl;
		if (!player2->is_alive()) {
			Loser();
			return;
		}
	}

	if (!enemy.is_sleep() && enemy.getPosition().is_in_radius(exploding_pos, explosion_radius)) {
		enemy = Enemy(); // remove enemy
	}


	for (auto it = items_in_room.begin(); it != items_in_room.end();) {
		if (it->second.is_in_radius(exploding_pos, explosion_radius)) {
			it = items_in_room.erase(it); // clear the item
		}
		else {
			++it;
		}
	}

	for (auto it = obstacles.begin(); it != obstacles.end(); ) {
		if (it->getPosition().is_in_radius(exploding_pos, explosion_radius)) {
			it = obstacles.erase(it); // clear the obstacle
		}
		else {
			++it;
		}
	}

	for (auto it = artifacts_in_room.begin(); it != artifacts_in_room.end();) {
		if (it->get_pos().is_in_radius(exploding_pos, explosion_radius)) {
			it = artifacts_in_room.erase(it); // clear the artifact
		}
		else {
			++it;
		}
	}

	for (auto it = breakable_walls.begin(); it != breakable_walls.end(); ) {
		if (it->first.is_in_radius(exploding_pos, explosion_radius)) {
			it->second++;
			if (it->second >= 2) {
				it = breakable_walls.erase(it);
				continue;
			}
		}

		++it;
	}
}

void Room::Loser() {
	ScreenManager sm;
	*game_result << "Game over! A player has lost in room " << id << " at tick " << (int)*tick << std::endl;

	if (owner->is_silent_mode())
		return;

	for (int i = 0; i < LoserScreen::LOOP_SCREENS; i++) {
		for (int j = 1; j <= LoserScreen::LOSER_NUM_OF_SCREENS; j++) {
			cls();
			std::string filename = "lose" + std::to_string(j) + ".txt";
			if (!sm.print_extra_files(filename)) {
				std::cout << "YOU LOSE!!!!\nError loading " << filename << std::endl;
				Sleep(1000);
				return;
			}

			if (j< LoserScreen::LOSER_NUM_OF_SCREENS)
				Sleep(200);
			else
				Sleep(600);
		}
	}
}

void Room::setPlayerStartPos(int id, Position pos) {
	if (id == 1)
		starting_position1 = pos;
	else
		starting_position2 = pos;
}

void Room::check_spring_release(Player* p) {
	if (p->is_on_spring()) {
		Direction dir = p->get_selected_direction();
		Direction spring_dir = p->get_spring_direction();

		if (dir == Direction::STAY || dir != spring_dir)
			p->release_spring(artifacts_in_room);
	}
}

bool Room::should_block_input(Player* p) const{
	if (!p->is_under_spring_effect())
		return false;

	for (auto& art : artifacts_in_room) {
		if (art.is_type(ARTIF_TYPE::SPRING) && art.get_pos() == p->getPosition())
			return true;
	}

	return false;
}

void Room::save_room(std::ostream& file) const {
	file << id << ' ' << dark << ' ' << locked << '\n';

	file << items_in_room.size() << '\n';
	for (const auto& item : items_in_room) 
		file << static_cast<int>(item.first) << ' ' << item.second.x << ' ' << item.second.y << '\n';

	file << artifacts_in_room.size() << '\n';
	for (const auto& art : artifacts_in_room) 
		file << static_cast<int>(art.get_type()) << ' ' << art.get_pos().x << ' ' << art.get_pos().y << ' ' << art.is_active() << ' ' << art.get_target_door_id() << ' ' << static_cast<int>(art.get_direction()) << '\n';

	file << obstacles.size() << '\n';
	for (const auto& obs : obstacles) {
		file << obs.getPosition().x << ' ' << obs.getPosition().y << ' ' << obs.get_parts().size() << '\n';
		for (const auto& part : obs.get_parts())
			file << part.x << ' ' << part.y << ' ';
		file << '\n';
	}

	file << breakable_walls.size() << '\n';
	for (const auto& bw : breakable_walls)
		file << bw.first.x << ' ' << bw.first.y << ' ' << bw.second << '\n';

	file << dynamic_walls.size() << '\n';
	for (const auto& dw : dynamic_walls)
		file << dw.getPosition().x << ' ' << dw.getPosition().y << ' ' << dw.get_switch_id() << ' ' << dw.is_visible() << '\n';

	file << door_switch_count.size() << '\n';
	for (const auto& dsc : door_switch_count)
		file << dsc.first << ' ' << dsc.second << '\n';

	file << door_keys_count.size() << '\n';
	for (const auto& dkc : door_keys_count)
		file << dkc.first << ' ' << dkc.second << '\n';

	file << activeBombs.size() << '\n';
	for (const auto& bomb : activeBombs)
		file << bomb.getPos().x << ' ' << bomb.getPos().y << ' ' << bomb.getCountdown() << '\n';

	file << enemy.getX() << ' ' << enemy.getY() << ' ' << enemy.is_sleep() << '\n';
}

void Room::load_room(std::istream& file) {
	int size, type, x, y, active, target_id, hits, key, val, dir;
	file >> id >> dark >> locked;

	items_in_room.clear();
	file >> size;
	for (int i = 0; i < size; i++) {
		file >> type >> x >> y;
		items_in_room.push_back(std::make_pair(static_cast<Item>(type), Position(x, y)));
	}

	artifacts_in_room.clear();
	file >> size;
	for (int i = 0; i < size; i++) {
		file >> type >> x >> y >> active >> target_id >> dir;
		ARTIF_TYPE art_type = static_cast<ARTIF_TYPE>(type);
		if (art_type == ARTIF_TYPE::DOOR_SWITCH || art_type == ARTIF_TYPE::PRESSURE_PLATE) {
			Artifact art(art_type, Position(x, y), target_id);
			if (active)
				art.set_active();
			artifacts_in_room.push_back(art);
		}
		else if (art_type == ARTIF_TYPE::SPRING) {
			Artifact art(art_type, Position(x, y), static_cast<Direction>(dir));
			if (!active)
				art.set_active();
			artifacts_in_room.push_back(art);
		}
		else {
			Artifact art(art_type, Position(x, y), owner);
			if (active)
				art.set_active();
			artifacts_in_room.push_back(art);
		}
	}

	obstacles.clear();
	file >> size;
	for (int i = 0; i < size; i++) {
		int parts_count;
		file >> x >> y >> parts_count;
		Obstacle obs(x, y);
		for (int j = 0; j < parts_count; j++) {
			int part_x, part_y;
			file >> part_x >> part_y;
			obs.add_part(Position(part_x, part_y));
		}
		obstacles.push_back(obs);
	}

	breakable_walls.clear();
	file >> size;
	for (int i = 0; i < size; i++) {
		file >> x >> y >> hits;
		breakable_walls[Position(x, y)] = hits;
	}

	dynamic_walls.clear();
	file >> size;
	for (int i = 0; i < size; i++) {
		bool visible;
		file >> x >> y >> target_id >> visible;
		DynamicWall dw(x, y, target_id);
		if (!visible)
			dw.hide();
		dynamic_walls.push_back(dw);
	}

	door_switch_count.clear();
	file >> size;
	for (int i = 0; i < size; i++) {
		file >> key >> val;
		door_switch_count[key] = val;
	}

	door_keys_count.clear();
	file >> size;
	for (int i = 0; i < size; i++) {
		file >> key >> val;
		door_keys_count[key] = val;
	}
	activeBombs.clear();
	int bomb_count, b_x, b_y, countdown;
	file >> bomb_count;
	for (int i = 0; i < bomb_count; i++) {
		file >> b_x >> b_y >> countdown;
		Bomb bomb(Position(b_x, b_y));
		bomb.setCountdown(countdown);
		activeBombs.push_back(bomb);
	}

	file >> x >> y >> active;
	enemy = Enemy(Position(x, y), active);

	dr = Drawer(&mapData, get_items_in_room(), get_artifacts_in_room(), get_obstacle_in_room(), get_dynamic_walls_in_room(), get_enemy(), get_players(), &dark, get_active_booms(), get_breakable_walls());
}

