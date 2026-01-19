#include "Enemy.h"
#include <queue>

void Enemy::move(long tick) {
	if (this->sleep)
		return;

	if (this->path.empty())
		return;

	if (should_act(tick, speed)) {
		this->pos = this->path.back();
		this->path.pop_back();
	}
}

void Enemy::find_path(std::vector<std::string>& screen, Position player1, Position player2, long tick) {

	if (!should_act(tick, speed))
		return;

	// Initialize all possible positions with false visited value
	std::vector<std::vector<std::pair<bool, Position>>> visited(MAX_X);
	for (auto& v : visited)
		v = std::vector<std::pair<bool, Position>>(MAX_Y, std::make_pair(false, Position(-1, -1)));

	visited[this->pos.x][this->pos.y].first = true;

	// Start at the enemys position
	std::queue<Position> to_visit;	
	to_visit.push(this->pos);
	int found_player = -1;

	// Find shortest path using bfs, each node adds to the queue its unvisited neighbors and the node from which you came added to visited
	while (!to_visit.empty())
	{
		Position current = to_visit.front();
		to_visit.pop();
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

			if (screen[next_element.y][next_element.x] == 'W')
				continue;

			if (std::isdigit(screen[next_element.y][next_element.x]))
				continue;

			if (visited[next_element.x][next_element.y].first == true)
				continue;

			visited[next_element.x][next_element.y].first = true;
			visited[next_element.x][next_element.y].second = Position(current.x, current.y);
			to_visit.push(next_element);

			if (next_element == player1) {
				found_player = 1;
				break;
			}

			if (next_element == player2) {
				found_player = 2;
				break;
			}
		}

		if (found_player != -1)
			break;
	}

	if (found_player == -1)
		return;

	// Reconstruct path - NOTE! the path is in reverse order (from player to enemy)
	Position trace_pos = found_player == 1 ? player1 : player2;
	while (trace_pos != this->pos) {
		this->path.push_back(trace_pos);
		trace_pos = visited[trace_pos.x][trace_pos.y].second;
		
	}
}
 