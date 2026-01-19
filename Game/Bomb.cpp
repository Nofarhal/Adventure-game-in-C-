#include "Bomb.h"

Bomb::Bomb(Position position) : pos(position), countdown(EXPLOSTION_COUNTDOWN) {}

bool Bomb::updateBooming(long tick) {
	if (should_act(tick, BOMB_SPEED))
		countdown--;

	if (countdown <= 0) 
		return true;

	return false;
}

std::vector<Position> Bomb::getExplosionArea() const {
	std::vector<Position> area;
	for (int demX = -DAMAGE_RADIUS; demX <= DAMAGE_RADIUS; demX++) {
		for (int demY = -DAMAGE_RADIUS; demY <= DAMAGE_RADIUS; demY++) {
			area.push_back(Position(pos.x + demX, pos.y + demY));
		}
	}
	return area;
}
