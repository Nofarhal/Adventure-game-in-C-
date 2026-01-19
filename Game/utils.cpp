#include <windows.h>
#include "utils.h"
#include <iostream>

void gotoxy(int x, int y) {
    std::cout.flush();
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = FALSE; // Hide the cursor
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void showCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = TRUE; // Show the cursor
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void cls() { system("cls"); }

bool Position::is_in_radius(const Position& center, int radius) const
{
    int dx = abs(x - center.x);
    int dy = abs(y - center.y);
    return dx <= radius && dy <= radius;
}

Direction opposite_dir(Direction dir) {
    switch (dir) {
    case Direction::UP: return Direction::DOWN;
    case Direction::DOWN: return Direction::UP;
    case Direction::RIGHT: return Direction::LEFT;
    case Direction::LEFT: return Direction::RIGHT;
    default: return Direction::STAY;
    }
}

// Make it so that an action is performed 'speed' times per second
bool should_act(long tick, int speed) {
    float second = 1000.0f / GAME_SPEED;
    float step_every = second / speed;
    float stage_in_cycle = tick - std::floor(tick / step_every) * step_every;
    return (stage_in_cycle < 1);
}

bool toInt(const std::string& s, int& result) {
    try {
        size_t pos;
        result = std::stoi(s, &pos);

        return pos == s.size();
    }
    catch (...) {
        return false;
    }
}

bool is_legal_char(char ch) {
    std::vector<char> legal_chars = {
        keys::P1_UP, keys::P1_DOWN, keys::P1_LEFT, keys::P1_RIGHT, keys::P1_STAY, keys::DROP_P1_ITEM,
        keys::P2_UP, keys::P2_DOWN, keys::P2_LEFT, keys::P2_RIGHT, keys::P2_STAY, keys::DROP_P2_ITEM,
        keys::OPTION_A, keys::OPTION_B, keys::OPTION_C, keys::OPTION_D,
    };

    return std::find(legal_chars.begin(), legal_chars.end(), ch) != legal_chars.end();
}