#pragma once
#include <string>	
#include <random>
#include <ctime>
#include <vector>
#include <fstream>
# include <iostream>

class Riddle {
private:
	std::string question;
	std::string a, b, c, d;
	std::mt19937 rng;
	char correct_ans;

	static std::vector<Riddle> riddle_pool;

public:
	Riddle() : correct_ans(' ') {}

	Riddle(const std::string& q, const std::string& A, const std::string& B, const std::string& C, const std::string& D, char correctAns)
		: question(q), a(A), b(B), c(C), d(D), correct_ans(correctAns) {}

	const std::string& getQuestion() const { return question; }
	const std::string& getOptionA() const { return a; }
	const std::string& getOptionB() const { return b; }
	const std::string& getOptionC() const { return c; }
	const std::string& getOptionD() const { return d; }
	char getCorrectAnswer() const { return correct_ans; }

	bool loadRiddles(const std::string& filename);
	static Riddle getRandomRiddle();
};