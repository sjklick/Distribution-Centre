#include <string>
#include <fstream>
#include "database-access.hpp"

bool Database::connect() {
	// Load credentials.
	// Save username and password on separate lines of "credentials.txt".
	std::ifstream configFile;
	std::string socket, username, password;
	configFile.open("config.txt", std::ios::in);
	configFile >> socket;
	configFile >> username;
	configFile >> password;
	configFile.close();
	// Make SQL server connection.
	connection = mysql_init(NULL);
	if (connection != NULL) {
		if (!mysql_real_connect(connection, "localhost", username.c_str(), password.c_str(), "stock", 0, socket.c_str(), 0)) {
			std::string error;
			error = mysql_error(connection);
			std::ofstream logFile;
			logFile.open("log.txt", std::ios::out | std::ios::app);
			logFile << error << std::endl;
			logFile.close();
		} else return true;
	}
	return false;
}

void Database::disconnect() {
	// Cleanly close SQL server connection.
	if (connection != NULL) mysql_close(connection);
}

Position Database::getPickerHome(int pickerId) {
	connect();
	Position home;
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query("SELECT * FROM pickers WHERE picker_id="+std::to_string(pickerId)+";");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			if(row = mysql_fetch_row(result)) {
				home.row = std::stoi(row[1]);
				home.column = std::stoi(row[2]);
				home.facing = CharToDirection(row[3][0]);
				mysql_free_result(result);
				disconnect();
				return home;
			}
		}
	}
	home.row = -1;
	home.column = -1;
	home.facing = invalid;
	disconnect();
	return home;
}

State Database::getPickerState(int pickerId) {
	connect();
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query("SELECT * FROM pickers WHERE picker_id="+std::to_string(pickerId)+";");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			if(row = mysql_fetch_row(result)) {
				std::string state = row[7];
				disconnect();
				return StringToState(state);
			}
		}
	}
	disconnect();
	return State::invalid;
}

void Database::setPickerState(int pickerId, State state) {
	connect();
	std::string query("UPDATE pickers Set state=\""+StateToString(state)+"\" WHERE picker_id="+std::to_string(pickerId)+";");
	mysql_query(connection, query.c_str());
	disconnect();
}

Position Database::getBinPosition(int binId) {
	connect();
	Position bin;
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query("SELECT * FROM stock_bins WHERE bin_id="+std::to_string(binId)+";");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			if(row = mysql_fetch_row(result)) {
				bin.row = std::stoi(row[1]);
				bin.column = std::stoi(row[2]);
				bin.facing = CharToDirection(row[3][0]);
				mysql_free_result(result);
				disconnect();
				return bin;
			}
		}
	}
	bin.row = -1;
	bin.column = -1;
	bin.facing = invalid;
	disconnect();
	return bin;
}

int Database::getBinItemCount(int binId) {
	connect();
	int nItems;
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query("SELECT SUM(quantity) FROM stock_items WHERE bin_id="+std::to_string(binId)+";");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			if (row = mysql_fetch_row(result)) {
				if (row[0] == NULL) {
					mysql_free_result(result);
					disconnect();
					return 0;
				} else {
					nItems = std::stoi(row[0]);
					mysql_free_result(result);
					disconnect();
					return nItems;
				}
			}
		}
		mysql_free_result(result);
	}
	disconnect();
	return -1;
}

int Database::getNextOrderId() {
	connect();
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
				disconnect();
				return nextId;
			}
		}
	}
	disconnect();
	return -1;
}

std::vector<Item> Database::getBinContents(int binId) {
	connect();
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
	disconnect();
	return items;
}

std::vector<Item> Database::getOrderItems(int orderId) {
	connect();
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
	disconnect();
	return items;
}

int Database::whichBinHasItem(std::string item) {
	connect();
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
				disconnect();
				return binId;
			}
		}
	}
	disconnect();
	return -1;
}

void Database::prepareShippingBin(int orderId) {
	connect();
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
	disconnect();
}

void Database::emptyShippingBin() {
	connect();
	std::string query("TRUNCATE shipping_items;");
	mysql_query(connection, query.c_str());
	disconnect();
}

void Database::putItemInShipping(std::string itemName) {
	connect();
	std::string query("UPDATE shipping_items SET quantity=quantity+1 WHERE name=\""+itemName+"\";");
	mysql_query(connection, query.c_str());
	disconnect();
}

std::vector<ShippingItem> Database::getShippingContents() {
	connect();
	std::vector<ShippingItem> items;
	ShippingItem temp;
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query("SELECT * FROM shipping_items;");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			while (row = mysql_fetch_row(result)) {
				temp.name = std::string(row[0]);
				temp.quantity = std::stoi(row[1]);
				temp.needed = std::stoi(row[2]);
				items.push_back(temp);
			}
		}
		mysql_free_result(result);
	}
	disconnect();
	return items;
}

bool Database::orderFulfilled(int orderId) {
	connect();
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query("SELECT * FROM shipping_items WHERE quantity<>needed;");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			if(row = mysql_fetch_row(result)) {
				mysql_free_result(result);
				disconnect();
				return false;
			}
		}
		mysql_free_result(result);
	}
	disconnect();
	return true;
}

void Database::removeOrder(int orderId) {
	connect();
	std::string query("DELETE FROM orders WHERE order_id="+std::to_string(orderId)+";");
	mysql_query(connection, query.c_str());
	disconnect();
}

void Database::removeItemFromStockBin(int binId, std::string itemName) {
	connect();
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query("SELECT * FROM stock_items WHERE bin_id="+std::to_string(binId)+" AND name=\""+itemName+"\";");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			if (row = mysql_fetch_row(result)) {
				// Get current quantity, decrement.
				int quantity = std::stoi(row[2]);
				mysql_free_result(result);
				quantity--;
				if (quantity > 0) {
					query = "UPDATE stock_items SET quantity="+std::to_string(quantity);
					query += " WHERE bin_id="+std::to_string(binId)+" AND name=\""+itemName+"\";";
				} else {
					query = "DELETE FROM stock_items WHERE bin_id="+std::to_string(binId)+" AND name=\""+itemName+"\";";
				}
				mysql_query(connection, query.c_str());
			}
		} else mysql_free_result(result);
	}
	disconnect();
}

void Database::placeItemIntoStockBin(int binId, std::string itemName) {
	connect();
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string query;
	// Check if item is already in bin.
	query = "SELECT * FROM stock_items WHERE bin_id="+std::to_string(binId)+" AND name=\""+itemName+"\";";
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			if (row = mysql_fetch_row(result)) {
				// If it is in bin, update quantity.
				int quantity = std::stoi(row[2]);
				mysql_free_result(result);
				quantity++;	
				query = "UPDATE stock_items SET quantity="+std::to_string(quantity);;
				query += " WHERE bin_id="+std::to_string(binId)+" AND name=\""+itemName+"\";";
				mysql_query(connection, query.c_str());
				disconnect();
				return;
			} else {
				// Otherwise, add a new entry for item.
				mysql_free_result(result);
				query = "INSERT INTO stock_items (bin_id, name, quantity) VALUES (";
				query += std::to_string(binId)+", \""+itemName+"\", 1);";
				mysql_query(connection, query.c_str());
				disconnect();
				return;
			}
		} else mysql_free_result(result);
	}
	disconnect();
}

void Database::removeItemFromOrderItems(int orderId, std::string itemName) {
	connect();
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
	disconnect();
}

void Database::removeOrderItems(int orderId) {
	connect();
	std::string query("DELETE FROM order_items WHERE order_id="+std::to_string(orderId)+";");
	mysql_query(connection, query.c_str());
	disconnect();
}

std::vector<std::string> Database::getLowInventory() {
	connect();
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::vector<std::string> items;
	std::string query("SELECT * FROM products ORDER BY quantity LIMIT 5;");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			while (row = mysql_fetch_row(result)) {
				items.push_back(std::string(row[0]));
			}
		}
		mysql_free_result(result);
	}
	disconnect();
	return items;
}

std::vector<std::string> Database::getReceivingItems() {
	connect();
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::vector<std::string> items;
	std::string query("SELECT * FROM receiving_items;");
	if (mysql_query(connection, query.c_str()) == 0) {
		result = mysql_use_result(connection);
		if (result) {
			while (row = mysql_fetch_row(result)) {
				items.push_back(std::string(row[0]));
			}
		}
		mysql_free_result(result);
	}
	disconnect();
	return items;
}

void Database::placeNewStock(std::vector<std::string> itemNames) {
	connect();
	std::string query;
	for (std::vector<std::string>::iterator it = itemNames.begin(); it != itemNames.end(); it++) {
		query = "INSERT INTO receiving_items (name) VALUES (\"";
		query += (*it) + "\");";
		mysql_query(connection, query.c_str());
	}
	disconnect();
}

std::vector<int> Database::whichBinsHaveRoom() {
	std::vector<int> bins;
	for (int i=0; i<22; i++) {
		int itemCount = Database::getBinItemCount(i+1);
		if (itemCount < 12) bins.push_back(i+1);
	}
	return bins;
}

void Database::removeItemFromReceiving (std::string itemName) {
	connect();
	std::string query;
	query = "DELETE FROM receiving_items WHERE name=\""+itemName+"\";";
	mysql_query(connection, query.c_str());
	disconnect();
}
