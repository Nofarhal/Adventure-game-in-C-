#include "Drawer.h"
#include <iostream>

Drawer::Drawer(const std::vector<std::string>* _mapData,
	const std::vector<std::pair<Item, Position>>* _items,
	const std::vector<Artifact>* _artifacts,
	const std::vector <Obstacle>* _obstacles,
	const std::vector<DynamicWall>* _dynamic_walls,
	Enemy const* _enemy,
	std::pair<const Player const*, const Player const*> _players,
	bool* _dark,
	std::vector <Bomb>* bombs,
	const std::map<Position, int>* breakable) :
	mapData(_mapData), items(_items), artifacts(_artifacts), obstacles(_obstacles),
	dynamic_walls(_dynamic_walls), enemy(_enemy), players(_players), dark(_dark), activeBombs(bombs),
	breakable_walls(breakable), 
	room_map(MAX_Y + 1, std::vector<Tile>(MAX_X + 1)) {
	for (int x = 0; x <= MAX_X; x++) {
		for (int y = 0; y <= MAX_Y; y++) 
			room_map[y][x].pos = Position(x, y);
	}
}


bool Drawer::COLOR_MODE = false;

ICON Drawer::item_to_icon(Item it) const {
	switch (it) {
	case Item::BOMB: return ICON::I_BOMB;
	case Item::KEY: return ICON::I_KEY;
	case Item::TORCH: return ICON::I_TORCH;
	case Item::HEART: return ICON::I_HEART;
	case Item::NONE: return ICON::I_NONE;
	}
}

ICON Drawer::artif_to_icon(Artifact ar) const {
	switch (ar.get_type()) {
	case ARTIF_TYPE::LIGHT_SWITCH: return ICON::I_LIGHT_SWITCH;
	case ARTIF_TYPE::SUMMON_ENEMY: return ICON::I_SUMMON_ENEMY;
	case ARTIF_TYPE::SPIKES: return ICON::I_SPIKES;
	case ARTIF_TYPE::RIDDLE: return ICON::I_RIDDLE;
	case ARTIF_TYPE::SPRING:
		if (!ar.is_active())
			return ICON::I_SPRING_COMPRESSED;
		else
			return ICON::I_SPRING;

	case ARTIF_TYPE::DOOR_SWITCH:
		if (ar.is_active())
			return ICON::I_DOOR_SWITCH_ON;
		else
			return ICON::I_DOOR_SWITCH_OFF;
	case ARTIF_TYPE::PRESSURE_PLATE: return ICON::I_PRESSURE_PLATE;
	}
}

char Drawer::IconToChar(const ICON icon, const Position pos) const {
	switch (icon) {

	case ICON::I_ENEMY: return 'E';
	case ICON::I_PLAYER1: return '&';
	case ICON::I_PLAYER2: return '$';
	case ICON::I_DARK: return ' ';
	case ICON::I_KEY: return 'K';
	case ICON::I_TORCH: return '!';
	case ICON::I_BOMB: return '@';
	case ICON::I_HEART: return 'H';
	case ICON::I_DOOR_SWITCH_ON: return '/';
	case ICON::I_DOOR_SWITCH_OFF: return '\\';
	case ICON::I_LIGHT_SWITCH: return 'S';
	case ICON::I_SUMMON_ENEMY: return 'X';
	case ICON::I_RIDDLE: return '?';
	case ICON::I_OBSTACLE: return '*';
	case ICON::I_SPRING: return '#';
	case ICON::I_SPRING_COMPRESSED: return ' ';
	case ICON::I_SPIKES: return '<';
	case ICON::I_PRESSURE_PLATE: return 'L';
	case ICON::I_DYNAMIC_WALL: return '[';
	case ICON::I_NONE: return ' ';
	case ICON::I_ROOM: return (*mapData)[pos.y][pos.x];
	}
}

char Drawer::IconToChar(const ICON icon, const int id) const {
	if (icon != ICON::I_ACTIVE_BOMB && icon != ICON::I_BREAKABLE_WALL)
		return ' ';

	if (icon == ICON::I_ACTIVE_BOMB)
		return id + '0';

	if (icon == ICON::I_BREAKABLE_WALL) {
		if (id == 0)
			return '|';
		else if (id == 1)
			return '-';
	}
}

std::string Drawer::icon_to_colorable(const ICON icon) const {
	switch (icon) {
	case ICON::I_ENEMY: return colors::RED;
	case ICON::I_PLAYER1: return colors::YELLOW;
	case ICON::I_PLAYER2: return colors::GREEN;
	case ICON::I_DARK: return "";
	case ICON::I_KEY: return colors::PINK;
	case ICON::I_TORCH: return colors::PINK;
	case ICON::I_BOMB: return colors::PINK;
	case ICON::I_HEART: return colors::PINK;
	case ICON::I_DOOR_SWITCH_OFF: return colors::PINK;
	case ICON::I_DOOR_SWITCH_ON: return colors::GREEN;
	case ICON::I_LIGHT_SWITCH: return colors::PINK;
	case ICON::I_OBSTACLE: return "";
	case ICON::I_SPRING: return colors::PINK;
	case ICON::I_SPRING_COMPRESSED: return "";
	case ICON::I_SUMMON_ENEMY: return colors::PINK;
	case ICON::I_SPIKES: return colors::RED;
	case ICON::I_RIDDLE: return colors::BLUE;
	case ICON::I_PRESSURE_PLATE: return colors::GREEN;
	case ICON::I_DYNAMIC_WALL: return colors::TURQUOISE;
	case ICON::I_BREAKABLE_WALL: return colors::CYAN;
	case ICON::I_ACTIVE_BOMB: return colors::RED;
	case ICON::I_ROOM: return "";
	case ICON::I_NONE: return "";
	}
}

bool Tile::is_identical_data(const Tile& other, const Drawer& dr) const {
	ICON other_priority_element = ICON::I_NONE;
	int other_priority_id = -1;

	for(int id = 0; id < other.objects_in_tile.size(); id++) {
		const auto& ic = other.objects_in_tile[id];
		if (ic.first < other_priority_element) {
			other_priority_element = ic.first;
			other_priority_id = id;
		}
	}
	
	char charToPrint = dr.IconToChar(other_priority_element, other.pos);
	if (other_priority_element == ICON::I_ACTIVE_BOMB || other_priority_element == ICON::I_BREAKABLE_WALL)
		charToPrint = dr.IconToChar(other_priority_element, other.objects_in_tile[other_priority_id].second);

	return printed_char == charToPrint;
}

void Drawer::draw_obj(Tile& tl, const int id_to_draw) const {
	ICON icon = ICON::I_NONE;
	if(id_to_draw != -1)
		icon = tl.objects_in_tile[id_to_draw].first;

	gotoxy(tl.pos.x, tl.pos.y);

	char charToPrint = IconToChar(icon, tl.pos);
	if (icon == ICON::I_ACTIVE_BOMB || icon == ICON::I_BREAKABLE_WALL)
		charToPrint = IconToChar(icon, tl.objects_in_tile[id_to_draw].second);

	tl.printed_char = charToPrint;
	if (COLOR_MODE)
		std::cout << icon_to_colorable(icon) << charToPrint << colors::WHITE;
	else
		std::cout << charToPrint;
}



void Drawer::build_tiles() {
	std::vector<std::vector<Tile>> current_room_state(MAX_Y + 1, std::vector<Tile>(MAX_X + 1, Tile()));
	// Go over room map and add room tiles
	for (int x = 0; x <= MAX_X; x++) {
		for (int y = 0; y <= MAX_Y; y++) {

			current_room_state[y][x].pos = Position(x, y);
			if ((*mapData)[y][x] != ' ')
				current_room_state[y][x].objects_in_tile.push_back(std::make_pair(ICON::I_ROOM, -1));
		}
	}

	// Get all items and artifacts
	for (const auto& item : *items)
		current_room_state[item.second.y][item.second.x].objects_in_tile.push_back(std::make_pair(item_to_icon(item.first), -1));

	for (const auto& art : *artifacts)
		current_room_state[art.get_pos().y][art.get_pos().x].objects_in_tile.push_back(std::make_pair(artif_to_icon(art), -1));

	// Obstacle
	for (const auto& obs : *obstacles) {
		for (const auto& part_pos : obs.get_parts())
			current_room_state[part_pos.y][part_pos.x].objects_in_tile.push_back(std::make_pair(ICON::I_OBSTACLE, -1));
	}
	for (const auto& dw : *dynamic_walls) {
		if (dw.is_visible()) {
			Position dw_pos = dw.getPosition();
			current_room_state[dw_pos.y][dw_pos.x].objects_in_tile.push_back(std::make_pair(ICON::I_DYNAMIC_WALL, -1));
		}
	}

	// Breakable walls
	for (const auto& bw : *breakable_walls) {
			current_room_state[bw.first.y][bw.first.x].objects_in_tile.push_back(std::make_pair(ICON::I_BREAKABLE_WALL, bw.second));
	}

	// enemy
	if (!enemy->is_sleep())
		current_room_state[enemy->getPosition().y][enemy->getPosition().x].objects_in_tile.push_back(std::make_pair(ICON::I_ENEMY, -1));

	// Players
	const auto [player1, player2] = players;
	if (!player1->get_left())
		current_room_state[player1->getPosition().y][player1->getPosition().x].objects_in_tile.push_back(std::make_pair(ICON::I_PLAYER1, -1));
	if (!player2->get_left())
		current_room_state[player2->getPosition().y][player2->getPosition().x].objects_in_tile.push_back(std::make_pair(ICON::I_PLAYER2, -1));

	// Active bomb
	for (const auto& bomb : *activeBombs) {
		Position bombPos = bomb.getPos();
		current_room_state[bombPos.y][bombPos.x].objects_in_tile.push_back(std::make_pair(ICON::I_ACTIVE_BOMB, bomb.getCountdown()));
	}
	

	// Darkness
	if (*dark) {
		for (int x = 0; x <= MAX_X; x++) {
			for (int y = 0; y <= MAX_Y; y++) {
				if ((abs(player1->getPosition().x - x) < TORCH_RADIUS && abs(player1->getPosition().y - y) < TORCH_RADIUS && player1->get_item() == Item::TORCH) || 
					(abs(player2->getPosition().x - x) < TORCH_RADIUS && abs(player2->getPosition().y - y) < TORCH_RADIUS && player2->get_item() == Item::TORCH))
					continue;

				current_room_state[y][x].objects_in_tile.push_back(std::make_pair(ICON::I_DARK, -1));
			}
		}
	}

	for (int x = 0; x <= MAX_X; x++) {
		for (int y = 0; y <= MAX_Y; y++) {

			auto& old_tile = room_map[y][x];
			auto& new_tile = current_room_state[y][x];
			new_tile.up_to_date = old_tile.is_identical_data(new_tile, *this);

			if (!new_tile.up_to_date)
				old_tile = new_tile;
		}
	}
}


void Drawer::draw_room(bool force_redraw) {
	build_tiles();

	for (int x = 0; x <= MAX_X; x++) {
		for (int y = 0; y <= MAX_Y; y++) {
			ICON priority_element = ICON::I_NONE;
			int priority_id = -1;
			if (room_map[y][x].up_to_date && !force_redraw)
				continue;

			for(int id = 0; id < room_map[y][x].objects_in_tile.size(); id++) {
				const auto& ic = room_map[y][x].objects_in_tile[id];
				if (ic.first < priority_element) {
					priority_element = ic.first;
					priority_id = id;
				}
			}

			draw_obj(room_map[y][x], priority_id);
			room_map[y][x].up_to_date = true;
		}
	}
}

void Drawer::draw_inventory(Position start_pos) const{
	const auto [player1, player2] = players;
	// Player 1 inventory
	gotoxy(start_pos.x, start_pos.y);
	std::cout << "Player1 ";
	for (int i = 0; i < player1->get_health(); i++) {
		std::cout << "H ";
	}
	for (int i = player1->get_health(); i < game_consts::MAX_HEALTH; i++) {
		std::cout << "  ";
	}
	std::cout << "| Item:";
	if (player1->has_item())
		std::cout << IconToChar(item_to_icon(player1->get_item())) << "   ";
	else
		std::cout << "None";

	std::cout << " | score: " << player1->get_score() << "  ";


	// Player 2 inventory
	gotoxy(start_pos.x, start_pos.y + 1);
	std::cout << "Player2 ";
	for (int i = 0; i < player2->get_health(); i++) {
		std::cout << "H ";
	}
	for (int i = player2->get_health(); i < game_consts::MAX_HEALTH; i++) {
		std::cout << "  ";
	}
	std::cout << "| Item:";
	if (player2->has_item())
		std::cout << IconToChar(item_to_icon(player2->get_item())) << "   ";
	else
		std::cout << "None";

	std::cout << " | score: " << player2->get_score() << "  ";
}