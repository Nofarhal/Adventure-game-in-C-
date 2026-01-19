#pragma once
#include "utils.h"
#include <vector>

class Enemy {
private:
	Position pos;
	std::vector<Position> path;
	int speed = 35;
	bool sleep = true;


public:
	Enemy() {}
	Enemy(Position _pos, bool _sleep) : pos(_pos), sleep(_sleep) {}

	void find_path(std::vector<std::string>& screen, Position player1, Position player2, long tick);

	void move(long tick);

	Position getPosition() const { return pos; }
	int getX() const { return pos.x; }
	int getY() const { return pos.y; }

	bool empty_path() const { return path.empty(); }

	void wake_up() { sleep = false; }
	bool is_sleep() const { return sleep; }
};
