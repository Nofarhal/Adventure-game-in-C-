#pragma once
#include "utils.h"
#include "Player.h"
#include <queue>

class Obstacle {
private:
	Position pos;
	std::vector<Position> parts;

public:
	Obstacle() {}
	Obstacle(int x, int y ) : pos(x,y) {}

	int getX() const { return pos.x; }
	int getY() const { return pos.y; }

	Position getPosition() const { return pos; }

	bool is_at(int x, int y) const;
	void add_part(Position pos) { parts.emplace_back(pos); }
	int get_size() const { return parts.size(); }
	const std::vector<Position>& get_parts() const { return parts; }
	bool try_push(Direction dir, const std::vector<std::string>& mapData, const std::vector <Obstacle>& obstacles);
	void build_obs_parts(std::vector<Obstacle>& obs, std::vector<std::vector<bool>>& visited);
};