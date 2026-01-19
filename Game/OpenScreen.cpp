#include <iostream>
#include <windows.h>
#include "OpenScreen.h"

Game_Status OpenScreen::runGame() {
	cls();

	ScreenManager screenManager;
	if (!screenManager.print_extra_files("main_menu.txt")) {
		std::cout << "Error loading main menu screen file\nPress any key to exit..." << std::endl;
		_getch();
		return Game_Status::Exit;
	}

	bool instructionsPresented = false;
	while (true) {
		char ch = _getch();
		cls();

		if(instructionsPresented) {
			screenManager.print_extra_files("main_menu.txt");
			instructionsPresented = false;
			continue;
		}

		switch (ch)
		{
			case '1':
				return Game_Status::Color;

			case '8': 
				if (screenManager.print_extra_files("instructions.txt")) 
					instructionsPresented = true;
				else {
					gotoxy(20, 10);
					std::cout << "Error loading instructions screen file" << std::endl;
					Sleep(2000);

					cls();
					screenManager.print_extra_files("main_menu.txt");
					instructionsPresented = false;
				}
				break;

			case '9':
				return Game_Status::Exit;

			case '3':
				return Game_Status::No_color;

			case '2': {
				int start = 5;
				gotoxy(20, start);
				for (int i = 0; i < saved_files.size(); i++) {
					const auto& file = saved_files[i];
					if (file.size() > 5 && file.substr(file.size() - 5) == ".save")
						std::cout << (i + 1) << ". " << file.substr(0, file.size() - 5);
					gotoxy(20, ++start);
				}
				gotoxy(20, ++start);
				start++;

				std::cout << "Enter save file number: ";
				showCursor();
				std::string filename_to_load_str;
				std::cin >> filename_to_load_str;
				while (!toInt(filename_to_load_str, filename_to_load) || filename_to_load <= 0 || filename_to_load > saved_files.size()) {
					gotoxy(20, start);
					std::cout << "Please enter a valid file number";
					gotoxy(44, (start - 1));
					std::cout << "         ";
					gotoxy(44, (start - 1));
					std::cin >> filename_to_load_str;
				}
				filename_to_load--;
				hideCursor();
				return Game_Status::Load_game;
			}

			default:
				screenManager.print_extra_files("main_menu.txt");
				gotoxy(29, 23);
				std::cout << "wrong key try again :( \n";
				break;
		}
	}
}

