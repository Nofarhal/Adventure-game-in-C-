#pragma once
#include "utils.h"
#include "Screen_manager.h"
#include <filesystem>


enum class Game_Status {
	Exit,
	Color,
	No_color,
	Load_game
};

class OpenScreen {
private:
	int filename_to_load;
	std::vector<std::string> saved_files;

	enum class screen_ids {
		MAIN_SCREEN,
		INSTRUCTIONS_SCREEN
	};

public:
	OpenScreen() {
		for (const auto& entry : std::filesystem::directory_iterator("./")) {
			if (entry.is_regular_file() && entry.path().string().size() > 5 && entry.path().string().substr(entry.path().string().size() - 5) == ".save") {
				saved_files.push_back(entry.path().string().substr(2));
			}
		}
	};

	Game_Status runGame();

	void draw(screen_ids screen_id) const;

	std::string get_selected_saved_file() const {
		if (filename_to_load < 0 || filename_to_load >= saved_files.size())
			return "";
		return saved_files[filename_to_load];
	}
};