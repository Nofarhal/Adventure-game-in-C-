#pragma once
#include "utils.h"
#include <vector>

class Bomb {
private:
	Position pos;
	int countdown;
	static const int BOMB_SPEED = 10;

public:
	static const int EXPLOSTION_COUNTDOWN = 5;
	static const int DAMAGE_RADIUS = 3; // 7*7 

	Bomb(Position position);

	Position getPos() const { return pos; }
	int getCountdown() const { return countdown; }

	bool updateBooming(long tick);
	std::vector<Position> getExplosionArea() const;
	void setCountdown(int c) { countdown = c; }
};
