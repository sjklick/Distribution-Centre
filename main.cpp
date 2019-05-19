#include <cstdlib>
#include <chrono>
#include <ctime>
#include <thread>
#include <iostream>
#include <fstream>
#include "floor-map.hpp"
#include "order-picker.hpp"
#include "database-access.hpp"
#include "database-exception.hpp"

int main() {
	int timeUntilRestock;
	try {
		while (true) {
			if (Database::receiving_get_items().empty() && (timeUntilRestock == 0)) {
				Database::receiving_replenish();
				timeUntilRestock = 30;
			} else if (Database::receiving_get_items().empty()){
				timeUntilRestock--;
			}
			Map::reset();
			for (std::vector<int>::iterator it; it != Database::picker_get_id_list().end(); it++) {
				Map::set_obstructed(Picker::get_position(*it));
				if (!Picker::is_assigned(*it)) {
					if (Database::order_get_next_item_to_ship(Database::order_get_current()) != "") {
						int orderId = Database::order_get_current();
						std::string nextItem = Database::order_get_next_item_to_ship(orderId);
						int bin = Database::stock_where_to_take_item(nextItem);
						Picker::assign_shipping_task(*it, nextItem, bin);
					} else if (Database::receiving_get_next_item_to_stock() != "") {
						std::string nextItem = Database::receiving_get_next_item_to_stock();
						int bin = Database::stock_where_to_place_item();
						Picker::assign_shipping_task(*it, nextItem, bin);
					}
				}
			}
			for (std::vector<int>::iterator it; it != Database::picker_get_id_list().end(); it++) {
				Picker::update(*it);
			}
			std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(1));
		}
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
