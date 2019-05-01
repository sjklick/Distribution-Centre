#include <cstdlib>
#include <chrono>
#include <thread>
#include <iostream>
#include "controller.hpp"

int main() {
	Controller controller;
	if (!controller.init()) return 1;

	while (true) {
		if (!controller.readState()) return 1;
		controller.updateState();
		if (!controller.writeState()) return 1;

		// Sleep for 1 second.
		std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(1));
	}

	controller.~Controller();
}
