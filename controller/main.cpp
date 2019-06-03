#include <cstdlib>
#include <chrono>
#include <ctime>
#include <thread>
#include <fstream>
#include "floor-map.hpp"
#include "order-picker.hpp"
#include "database-access.hpp"
#include "database-exception.hpp"

int main() {
	int timeUntilRestock = 30;
	int timeUntilStartNextOrder = 5;
	try {
		Database::connection_create();
		while (true) {
			if (Database::receiving_get_items().empty()) {
				if (timeUntilRestock == 0) {
					Database::receiving_replenish();
					timeUntilRestock = 30;
				} else {
					timeUntilRestock--;
				}
			}
			int orderId = Database::order_get_current();
			if (Database::order_check_if_ready(orderId)) {
				if (timeUntilStartNextOrder == 0) {
					if (Database::order_confirmation_needed(orderId)) {
						std::string customer, email, systemCall;
						customer = Database::order_get_customer_name(orderId);
						email = Database::order_get_customer_email(orderId);
						systemCall = "php ./confirmation.php ";
						systemCall += std::to_string(orderId);
						systemCall += " ";
						systemCall += customer;
						systemCall += " ";
						systemCall += email;
						std::system(systemCall.c_str());
					}
					Database::order_remove_items(orderId);
					Database::order_remove(orderId);
					Database::shipping_clear();
					orderId = Database::order_get_current();
					timeUntilStartNextOrder = 5;
				} else {
					timeUntilStartNextOrder--;
				}
			}
			Map::reset();
			std::vector<int> pickerList = Database::picker_get_id_list();
			for (std::vector<int>::iterator it = pickerList.begin(); it != pickerList.end(); it++) {
				Map::set_obstructed(Picker::get_position(*it));
				if (!Picker::is_assigned(*it)) {
					std::string next;
					if ((next = Database::order_get_next_item_to_ship(orderId)) != "") {
						int bin = Database::stock_where_to_take_item(next);
						Picker::assign_shipping_task(*it, next, bin);
					} else if ((next = Database::receiving_get_next_item_to_stock()) != "") {
						int bin = Database::stock_where_to_place_item();
						Picker::assign_receiving_task(*it, next, bin);
					}
				}
			}
			for (std::vector<int>::iterator it = pickerList.begin(); it != pickerList.end(); it++) {
				Picker::update(*it);
			}
			std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(100));
		}
	} catch (DatabaseException& e) {
		Database::connection_destroy();
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
