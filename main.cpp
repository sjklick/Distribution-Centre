#include <cstdlib>
#include <chrono>
#include <ctime>
#include <thread>
#include <iostream>
#include <fstream>
#include "controller.hpp"
#include "database-exception.hpp"

int main() {
	try {
		Controller controller;
		controller.init();
		while (true) {
			controller.updateState();
			// Sleep for 1 second.
			std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(1));
		}
		controller.~Controller();
	} catch (DatabaseException& e) {
		std::ofstream logFile;
		std::time_t errorTime;
		std::string timeString;
		errorTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		timeString = std::string(std::ctime(&errorTime));
		timeString.pop_back();
		logFile.open("log.txt", std::ios::out | std::ios::app);
		logFile << timeString << ": " << e.message() << std::endl;
		logFile.close();
		return 0;
	}
}
