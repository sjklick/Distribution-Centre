#include <cstdlib>
#include <chrono>
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
			controller.readState();
			controller.updateState();
			controller.writeState();
			// Sleep for 1 second.
			std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(1));
		}
		controller.~Controller();
	} catch (DatabaseException& e) {
		std::ofstream logFile;
		logFile.open("log.txt", std::ios::out | std::ios::app);
		logFile << e.message() << std::endl;
		logFile.close();
	}
}
