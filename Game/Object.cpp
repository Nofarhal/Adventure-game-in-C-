#include "Object.h"
#include "Room.h"
#include <windows.h>
#include "Game.h"

bool Artifact::activate(Room& r, Player& p) {
	switch (type) {
		case ARTIF_TYPE::LIGHT_SWITCH:
			r.switch_dark();
			break;
		case ARTIF_TYPE::SUMMON_ENEMY:
			r.wake_enemy();
			break;
		case ARTIF_TYPE::SPIKES:
			p.add_health(-1);
			if (game_result)
				*game_result << "Player " << p.get_id() << " hit by spike at tick " << (int)*tick << ". Health is now " << p.get_health() << std::endl;
			break;
		case ARTIF_TYPE::RIDDLE:
			return showRiddleOnScreen(r, p);
			break;
		case ARTIF_TYPE::DOOR_SWITCH:
			r.activate_door_switch(*this);
			break;
		case ARTIF_TYPE::PRESSURE_PLATE:
			break;
		case ARTIF_TYPE::SPRING:
			if (is_active()) {
				if (p.get_selected_direction() != dir && p.get_flight_direction() != dir)
					return false;

				p.step_on_spring(dir);
			}

			set_active();
			break;
	}

	return true;
}

bool Artifact::showRiddleOnScreen(Room& r, Player& p) {
	Riddle riddle = Riddle::getRandomRiddle();

	int question_start_y = 6;
	if (!owner->is_silent_mode()) {
		cls();

		for (int x = 10; x <= 70; x++) {
			gotoxy(x, 2); std::cout << (Drawer::COLOR_MODE ? colors::CYAN : "") << "=" << (Drawer::COLOR_MODE ? colors::WHITE : "");
			gotoxy(x, 20); std::cout << (Drawer::COLOR_MODE ? colors::CYAN : "") << "=" << (Drawer::COLOR_MODE ? colors::WHITE : "");
		}
		for (int y = 2; y <= 20; y++) {
			gotoxy(10, y); std::cout << (Drawer::COLOR_MODE ? colors::CYAN : "") << "|" << (Drawer::COLOR_MODE ? colors::WHITE : "");
			gotoxy(70, y); std::cout << (Drawer::COLOR_MODE ? colors::CYAN : "") << "|" << (Drawer::COLOR_MODE ? colors::WHITE : "");
		}

		gotoxy(23, 4);
		std::cout << (Drawer::COLOR_MODE ? colors::YELLOW : "") << " I T ' S   R I D D L E   T I M E  " << (Drawer::COLOR_MODE ? colors::WHITE : "");


		std::string question = riddle.getQuestion();
		size_t pos = 0, prev = 0;
		while ((pos = question.find('\n', prev)) != std::string::npos) {
			gotoxy(13, question_start_y++);
			std::cout << question.substr(prev, pos - prev);
			prev = pos + 1;
		}
		gotoxy(13, question_start_y++);
		std::cout << question.substr(prev);

		gotoxy(13, question_start_y + 1); std::cout << (Drawer::COLOR_MODE ? colors::BLUE : "") << "A) " << (Drawer::COLOR_MODE ? colors::WHITE : "") << riddle.getOptionA();
		gotoxy(13, question_start_y + 2); std::cout << (Drawer::COLOR_MODE ? colors::BLUE : "") << "B) " << (Drawer::COLOR_MODE ? colors::WHITE : "") << riddle.getOptionB();
		gotoxy(13, question_start_y + 3); std::cout << (Drawer::COLOR_MODE ? colors::BLUE : "") << "C) " << (Drawer::COLOR_MODE ? colors::WHITE : "") << riddle.getOptionC();
		gotoxy(13, question_start_y + 4); std::cout << (Drawer::COLOR_MODE ? colors::BLUE : "") << "D) " << (Drawer::COLOR_MODE ? colors::WHITE : "") << riddle.getOptionD();

		gotoxy(13, question_start_y + 7);
		std::cout << (Drawer::COLOR_MODE ? colors::YELLOW : "") << "Choose answer [A/B/C/D]:" << (Drawer::COLOR_MODE ? colors::WHITE : "");
	}

	char answer;
	while (true) {
		answer = owner->my_getch(*tick);

		if (owner->is_load_mode() && answer == 0) {
			answer = keys::DUMMY_OPTION; // dummy answer for load mode
			break;
		}

		if (answer == keys::OPTION_A || answer == keys::OPTION_B || answer == keys::OPTION_C || answer == keys::OPTION_D) 
			break;
	}
	if (game_result)
		*game_result << "Riddle asked at tick " << (int)*tick << ". The riddle: " << riddle.getQuestion() << ". Player answered: " << answer << ". Correct answer: " << riddle.getCorrectAnswer() << std::endl;

	if (answer == riddle.getCorrectAnswer()) {
		if (!status)
			p.add_score(game_consts::RIDDLE_SCORE_REWARD);

		if (!owner->is_silent_mode()) {
			gotoxy(13, question_start_y + 9);
			if (Drawer::COLOR_MODE) 
				std::cout << colors::GREEN << "You are correct!" << colors::WHITE;
			else 
				std::cout << "You are correct!";

			Sleep(1200);
			r.get_drawer()->draw_room(true);
		}

		return true;
	}
	else {
		status = true; // Points only awarded if riddle not solved before 
		p.set_selected_direction(Direction::STAY);
		if (!owner->is_silent_mode()) {
			gotoxy(13, question_start_y + 9);
			if (Drawer::COLOR_MODE) 
				std::cout << colors::RED << "Wrong!Maybe next time" << colors::WHITE;
			else 
				std::cout << "Wrong! Maybe next time";

			Sleep(1500);
			r.get_drawer()->draw_room(true);
		}

		return false;
	}
}

