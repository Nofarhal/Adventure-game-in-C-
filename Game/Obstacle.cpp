#include "Obstacle.h"

bool Obstacle::is_at(int x, int y) const {
	for (const auto& part : parts) {
		if (part.x == x && part.y == y) 
			return true;
	}
	return false;
}

bool Obstacle::try_push(Direction dir, const std::vector<std::string>& mapData, const std::vector<Obstacle>& obstacles)
{
	int dx = 0, dy = 0;

	switch (dir) {
	case Direction::UP: dy = -1; break;
	case Direction::DOWN: dy = 1; break;
	case Direction::LEFT: dx = -1; break;
	case Direction::RIGHT: dx = 1; break;
	default: return false;
	}

	for (const auto& part : parts) {
		int new_x = part.x + dx;
		int new_y = part.y + dy;

		if (new_y >= mapData.size() || new_y < 0 || new_x >= mapData[0].size() || new_x < 0)
			return false;

		if (mapData[new_y][new_x] == 'W')
			return false;

		for (const auto& obs : obstacles) {
			if (&obs == this)
				continue;

			if (obs.is_at(new_x, new_y))
				return false;
		}
	}

	for (auto& part : parts) {
		part.x += dx;
		part.y += dy;
	}

	pos.x += dx;
	pos.y += dy;

	return true;

}

void Obstacle::build_obs_parts(std::vector<Obstacle>& obstacles, std::vector<std::vector<bool>>& visited) {

	std::vector<std::vector<bool>> is_obs_board(MAX_Y, std::vector<bool>(MAX_X, false));

	for (const auto& obs : obstacles)
		is_obs_board[obs.getY()][obs.getX()] = true;

	visited[this->pos.x][this->pos.y] = true;

	std::queue<Position> to_visit;
	to_visit.push(this->pos);

	while (!to_visit.empty())
	{
		Position current = to_visit.front();
		to_visit.pop();

		this->add_part(current);

		for (int dir_id = 0; dir_id < 4; dir_id++)
		{
			Direction dir = static_cast<Direction>(dir_id);
			Position next_element(-1, -1);

			switch (dir)
			{
			case Direction::UP:
				next_element = Position(current.x, current.y - 1);
				break;

			case Direction::DOWN:
				next_element = Position(current.x, current.y + 1);
				break;

			case Direction::LEFT:
				next_element = Position(current.x - 1, current.y);
				break;

			case Direction::RIGHT:
				next_element = Position(current.x + 1, current.y);
				break;
			}

			if (next_element.x < 0 || next_element.x >= MAX_X || next_element.y < 0 || next_element.y >= MAX_Y)
				continue;

			if (visited[next_element.x][next_element.y])
				continue;

			if (is_obs_board[next_element.y][next_element.x] == false)
				continue;

			visited[next_element.x][next_element.y] = true;
			to_visit.push(next_element);
		}
	}
}
