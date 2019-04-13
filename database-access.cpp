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

int Database::whichBinHasItem(std::string item) {
	int binId;
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query("SELECT * FROM stock_items WHERE name=\""+item+"\" AND QUANTITY>0 LIMIT 1;");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			if(row = mysql_fetch_row(result)) {
				binId = std::stoi(row[0]);
				mysql_free_result(result);
				return binId;
			}
		}
	}
	return -1;
}

void Database::prepareShippingBin(int orderId) {
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::vector<Item> items;
	Item temp;
	std::string query("SELECT * FROM order_items WHERE order_id="+std::to_string(orderId));
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			while (row = mysql_fetch_row(result)) {
				temp.name = std::string(row[1]);
				temp.quantity = std::stoi(row[2]);
				items.push_back(temp);
			}
		}
		mysql_free_result(result);
	}
	for (std::vector<Item>::iterator it = items.begin(); it != items.end(); it++) {
		query = "INSERT INTO shipping_items (name, quantity, needed) VALUES (";
		query += "\""+(*it).name + "\", 0, " + std::to_string((*it).quantity) + ");";
		mysql_query(connection, query.c_str());
	}
}

void Database::emptyShippingBin() {
	std::string query("TRUNCATE shipping_items;");
	mysql_query(connection, query.c_str());
}

void Database::putItemInShipping(std::string itemName) {
	std::string query("UPDATE shipping_items SET quantity=quantity+1 WHERE name=\""+itemName+"\";");
	mysql_query(connection, query.c_str());
}

bool Database::orderFulfilled(int orderId) {
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query("SELECT * FROM shipping_items WHERE quantity<>needed;");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			if(row = mysql_fetch_row(result)) {
				mysql_free_result(result);
				return false;
			}
		}
		mysql_free_result(result);
	}
	return true;
}

void Database::removeOrder(int orderId) {
	std::string query("DELETE FROM orders WHERE order_id="+std::to_string(orderId)+";");
	mysql_query(connection, query.c_str());
}

void Database::removeItemFromStockBin(int binId, std::string itemName) {
	MYSQL_RES* result;
	MYSQL_ROW row;
	int quantity;
	std::string query("SELECT * FROM stock_items WHERE bin_id="+std::to_string(binId)+" AND name=\""+itemName+"\";");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			if (row = mysql_fetch_row(result)) {
				// Get current quantity, decrement.
				quantity = std::stoi(row[2]);
				mysql_free_result(result);
				quantity--;
				query = "UPDATE stock_items SET quantity="+std::to_string(quantity);
				query += " WHERE bin_id="+std::to_string(binId)+" AND name=\""+itemName+"\";";
				mysql_query(connection, query.c_str());
			} else mysql_free_result(result);
		}
	}
}

void Database::removeItemFromOrderItems(int orderId, std::string itemName) {
	MYSQL_RES* result;
	MYSQL_ROW row;
	int quantity;
	std::string query("SELECT * FROM order_items WHERE order_id="+std::to_string(orderId)+" AND name=\""+itemName+"\";");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			if (row = mysql_fetch_row(result)) {
				// Get current quantity, decrement.
				quantity = std::stoi(row[2]);
				mysql_free_result(result);
				quantity--;
				if (quantity > 0) {
					query = "UPDATE order_items SET quantity="+std::to_string(quantity);
					query += " WHERE order_id="+std::to_string(orderId)+" AND name=\""+itemName+"\";";
				} else {
					query = "DELETE FROM order_items";
					query += " WHERE order_id="+std::to_string(orderId)+" AND name=\""+itemName+"\";";
				}
				mysql_query(connection, query.c_str());
			} else mysql_free_result(result);
		}
	}
}

void Database::removeOrderItems(int orderId) {
	std::string query("DELETE FROM order_items WHERE order_id="+std::to_string(orderId)+";");
	mysql_query(connection, query.c_str());
}
