#include <string>
#include <fstream>
#include <mysql.h>
#include "database-access.hpp"

static MYSQL* connection;

bool Database::connect() {
	// Load credentials.
	// Save username and password on separate lines of "credentials.txt".
	std::ifstream credFile;
	std::string username, password;
	credFile.open("credentials.txt", std::ios::in);
	credFile >> username;
	credFile >> password;
	credFile.close();
	// Make SQL server connection.
	connection = mysql_init(NULL);
	if (connection != NULL) {
		if (!mysql_real_connect(connection, "localhost", username.c_str(), password.c_str(), "stock", 0, NULL, 0)) {
			return false;
		} else return true;
	}
	return false;
}

void Database::disconnect() {
	// Cleanly close SQL server connection.
	if (connection != NULL) mysql_close(connection);
}

Position* Database::getBinPosition(int binId) {
	Position* bin;
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query("SELECT * FROM stock_bins WHERE bin_id="+std::to_string(binId)+";");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			bin = new Position;
			if(row = mysql_fetch_row(result)) {
				bin->row = std::stoi(row[1]);
				bin->column = std::stoi(row[2]);
				bin->facing = CharToDirection(row[3][0]);
				mysql_free_result(result);
				return bin;
			}
		}
	}
	return NULL;
}

int Database::getBinItemCount(int binId) {
	int nItems;
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query("SELECT SUM(quantity) FROM stock_items WHERE bin_id="+std::to_string(binId)+";");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			if(row = mysql_fetch_row(result)) {
				nItems = std::stoi(row[0]);
				mysql_free_result(result);
				return nItems;
			}
		}
	}
	return -1;
}

int Database::getNextOrderId() {
	int nextId;
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query("SELECT * FROM orders LIMIT 1;");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			if(row = mysql_fetch_row(result)) {
				nextId = std::stoi(row[0]);
				mysql_free_result(result);
				return nextId;
			}
		}
	}
	return -1;
}

std::vector<Item> Database::getBinContents(int binId) {
	std::vector<Item> items;
	Item temp;
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query("SELECT * FROM stock_items WHERE bin_id="+std::to_string(binId)+";");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			while (row = mysql_fetch_row(result)) {
				temp.name = std::string(row[1]);
				temp.quantity = std::stoi(row[2]);
				items.push_back(temp);
			}
			mysql_free_result(result);
		}
	}
	return items;
}

std::vector<Item> Database::getOrderItems(int orderId) {
	std::vector<Item> items;
	Item temp;
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query("SELECT * FROM order_items WHERE order_id="+std::to_string(orderId)+";");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			while (row = mysql_fetch_row(result)) {
				temp.name = std::string(row[1]);
				temp.quantity = std::stoi(row[2]);
				items.push_back(temp);
			}
			mysql_free_result(result);
		}
	}
	return items;
}
