#pragma once
#include "utils.h"

class DynamicWall {
private:
	Position pos;
	int controlled_by_switch_id;
	bool visible;

public:
	DynamicWall(int x, int y, int switch_id) : pos(x, y), controlled_by_switch_id(switch_id), visible(true) {}

	DynamicWall(Position _pos, int switch_id) : pos(_pos), controlled_by_switch_id(switch_id), visible(true) {}

	Position getPosition() const { return pos; }
	int get_switch_id() const { return controlled_by_switch_id; }
	bool is_visible() const { return visible; }

	void show() { visible = true; }
	void hide() { visible = false; }
};