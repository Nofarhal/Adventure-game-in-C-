#include "Riddle.h"
#include "Game.h"

std::vector<Riddle> Riddle::riddle_pool;

bool Riddle::loadRiddles(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error opening riddle file: " << filename << std::endl;
		return false;
	}

	std::string line;
	std::string q, A, B, C, D;
	char ans = ' ';

	while (std::getline(file, line)) {
		if (line.empty()) 
			continue;

		if (line.rfind("Q:", 0) == 0) {
			q = line.substr(3);
			size_t pos = 0;
			while ((pos = q.find("\\n", pos)) != std::string::npos) {
				q.replace(pos, 2, "\n");
				pos += 1; // Move past the replaced character
			}
		}
		else if (line.rfind("A:", 0) == 0) {
			A = line.substr(3);
		}
		else if (line.rfind("B:", 0) == 0) {
			B = line.substr(3);
		}
		else if (line.rfind("C:", 0) == 0) {
			C = line.substr(3);
		}
		else if (line.rfind("D:", 0) == 0) {
			D = line.substr(3);
		}
		else if (line.rfind("ANS:", 0) == 0) {
			ans = line[5];
			
			if (!q.empty())
				riddle_pool.emplace_back(q, A, B, C, D, ans);

			q = "";
		}
	}

	file.close();
	return true;
}

Riddle Riddle::getRandomRiddle()
{
	if (riddle_pool.empty()) {
		std::cerr << "Riddle pool is empty. Returning default riddle." << std::endl;
		return Riddle();
	}

	static std::mt19937 rng(static_cast<unsigned>(Game::random_seed));
	std::uniform_int_distribution<std::size_t> dist(0, riddle_pool.size() - 1);
	return riddle_pool[dist(rng)];
}


