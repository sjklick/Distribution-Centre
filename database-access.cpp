#include <string>
#include <fstream>
#include "database-access.hpp"
#include "database-exception.hpp"

static MYSQL* connect () {
	MYSQL* connection;
	std::string error;
	// Load credentials.
	// Save username and password on separate lines of "credentials.txt".
	std::ifstream configFile;
	std::string socket, username, password;
	error = "Failed to load database credentials.";
	configFile.open("config.txt", std::ios::in);
	if (!configFile.good()) throw DatabaseException(error);
	configFile >> socket;
	if (!configFile.good()) throw DatabaseException(error);
	configFile >> username;
	if (!configFile.good()) throw DatabaseException(error);
	configFile >> password;
	if (!configFile.good()) throw DatabaseException(error);
	configFile.close();
	if (!configFile.good()) throw DatabaseException(error);
	// Make SQL server connection.
	error = "Failed to connect to database.";
	connection = mysql_init(NULL);
	if (connection != NULL) {
		if (!mysql_real_connect(connection, "localhost", username.c_str(), password.c_str(), "stock", 0, socket.c_str(), 0)) {
			error = "Failed to connect to database - ";
			error += mysql_error(connection);
			error += ".";
			throw DatabaseException(error);
		}
	} else throw DatabaseException(error);
	return connection;
}

static void disconnect (MYSQL* connection) {
	if (connection != NULL) mysql_close(connection);
}

static void disable_auto_commit (MYSQL* connection) {
	std::string error;
	error = "Failed to disable autocommit.";
	if (mysql_autocommit(connection, 0) != 0) throw DatabaseException(error);
}

static void commit (MYSQL* connection) {
	std::string error;
	error = "Failed to commit.";
	if (mysql_commit(connection) != 0) throw DatabaseException(error);
}

static void make_query (MYSQL* connection, std::string query) {
	std::string error;
	error = "Query failed.";
	if (mysql_query(connection, query.c_str()) != 0) throw DatabaseException(error);
}

static MYSQL_RES* get_result (MYSQL* connection) {
	MYSQL_RES* result;
	std::string error;
	error = "Failed to get result from query.";
	result = mysql_use_result(connection);
	if (result) return result;
	else throw DatabaseException(error);
}

namespace Database {
	Position picker_get_home (int pickerId) {
		MYSQL* connection;
		MYSQL_RES* result;
		MYSQL_ROW row;
		Position home;
		std::string query;
		try {
			connection = connect();
			query = "SELECT * FROM pickers WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if(row = mysql_fetch_row(result)) {
				home.row = std::stoi(row[1]);
				home.column = std::stoi(row[2]);
				home.facing = CharToDirection(row[3][0]);			
			}
			mysql_free_result(result);
			disconnect(connection);
			return home;
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_get_home - "+e.message());
		}
	}

	Position picker_get_current (int pickerId) {
		MYSQL* connection;
		MYSQL_RES* result;
		MYSQL_ROW row;
		Position current;
		std::string query;
		try {
			connection = connect();
			query = "SELECT * FROM pickers WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if(row = mysql_fetch_row(result)) {
				current.row = std::stoi(row[4]);
				current.column = std::stoi(row[5]);
				current.facing = CharToDirection(row[6][0]);
			}
			mysql_free_result(result);
			disconnect(connection);
			return current;
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_get_current - "+e.message());
		}
	}

	void setPickerCurrent(int pickerId, Position current) {
		MYSQL* connection;
		connection = connect();
		std::string query = "";
		query += "UPDATE pickers SET curr_row=";
		query += std::to_string(current.row);
		query += ", curr_col=";
		query += std::to_string(current.column);
		query += ", curr_dir=\"";
		query += DirectionToChar(current.facing);
		query += "\" WHERE picker_id="+std::to_string(pickerId)+";";
		mysql_query(connection, query.c_str());
		disconnect(connection);
	}

	State getPickerState(int pickerId) {
		MYSQL* connection;
		connection = connect();
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query("SELECT * FROM pickers WHERE picker_id="+std::to_string(pickerId)+";");
		if (mysql_query(connection, query.c_str()) == 0) {
			result = mysql_use_result(connection);
			if (result) {
				if(row = mysql_fetch_row(result)) {
					std::string state = row[7];
					disconnect(connection);
					return StringToState(state);
				}
			}
		}
		disconnect(connection);
		return State::invalid;
	}

	void setPickerState(int pickerId, State state) {
		MYSQL* connection;
		connection = connect();
		std::string query("UPDATE pickers Set state=\""+StateToString(state)+"\" WHERE picker_id="+std::to_string(pickerId)+";");
		mysql_query(connection, query.c_str());
		disconnect(connection);
	}

	Position getBinPosition(int binId) {
		MYSQL* connection;
		connection = connect();
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
					disconnect(connection);
					return bin;
				}
			}
		}
		bin.row = -1;
		bin.column = -1;
		bin.facing = invalid;
		disconnect(connection);
		return bin;
	}

	int getBinItemCount(int binId) {
		MYSQL* connection;
		connection = connect();
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
						disconnect(connection);
						return 0;
					} else {
						nItems = std::stoi(row[0]);
						mysql_free_result(result);
						disconnect(connection);
						return nItems;
					}
				}
			}
			mysql_free_result(result);
		}
		disconnect(connection);
		return -1;
	}

	int getNextOrderId() {
		MYSQL* connection;
		connection = connect();
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
					disconnect(connection);
					return nextId;
				}
			}
		}
		disconnect(connection);
		return -1;
	}

	std::vector<Item> getBinContents(int binId) {
		MYSQL* connection;
		connection = connect();
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
		disconnect(connection);
		return items;
	}

	std::vector<Item> getOrderItems(int orderId) {
		MYSQL* connection;
		connection = connect();
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
		disconnect(connection);
		return items;
	}

	int whichBinHasItem(std::string item) {
		MYSQL* connection;
		connection = connect();
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
					disconnect(connection);
					return binId;
				}
			}
		}
		disconnect(connection);
		return -1;
	}

	void prepareShippingBin(int orderId) {
		MYSQL* connection;
		connection = connect();
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
		disconnect(connection);
	}

	void emptyShippingBin() {
		MYSQL* connection;
		connection = connect();
		std::string query("TRUNCATE shipping_items;");
		mysql_query(connection, query.c_str());
		disconnect(connection);
	}

	void putItemInShipping(std::string itemName) {
		MYSQL* connection;
		connection = connect();
		std::string query("UPDATE shipping_items SET quantity=quantity+1 WHERE name=\""+itemName+"\";");
		mysql_query(connection, query.c_str());
		disconnect(connection);
	}

	std::vector<ShippingItem> getShippingContents() {
		MYSQL* connection;
		connection = connect();
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
		disconnect(connection);
		return items;
	}

	bool orderFulfilled(int orderId) {
		MYSQL* connection;
		connection = connect();
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query("SELECT * FROM shipping_items WHERE quantity<>needed;");
		if (mysql_query(connection, query.c_str()) == 0) {
			result = mysql_use_result(connection);
			if (result) {
				if(row = mysql_fetch_row(result)) {
					mysql_free_result(result);
					disconnect(connection);
					return false;
				}
			}
			mysql_free_result(result);
		}
		disconnect(connection);
		return true;
	}

	void removeOrder(int orderId) {
		MYSQL* connection;
		connection = connect();
		std::string query("DELETE FROM orders WHERE order_id="+std::to_string(orderId)+";");
		mysql_query(connection, query.c_str());
		disconnect(connection);
	}

	void removeItemFromStockBin(int binId, std::string itemName) {
		MYSQL* connection;
		connection = connect();
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
		disconnect(connection);
	}

	void placeItemIntoStockBin(int binId, std::string itemName) {
		MYSQL* connection;
		connection = connect();
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
					disconnect(connection);
					return;
				} else {
					// Otherwise, add a new entry for item.
					mysql_free_result(result);
					query = "INSERT INTO stock_items (bin_id, name, quantity) VALUES (";
					query += std::to_string(binId)+", \""+itemName+"\", 1);";
					mysql_query(connection, query.c_str());
					disconnect(connection);
					return;
				}
			} else mysql_free_result(result);
		}
		disconnect(connection);
	}

	void removeItemFromOrderItems(int orderId, std::string itemName) {
		MYSQL* connection;
		connection = connect();
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
		disconnect(connection);
	}

	void removeOrderItems(int orderId) {
		MYSQL* connection;
		connection = connect();
		std::string query("DELETE FROM order_items WHERE order_id="+std::to_string(orderId)+";");
		mysql_query(connection, query.c_str());
		disconnect(connection);
	}

	std::vector<std::string> getLowInventory() {
		MYSQL* connection;
		connection = connect();
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
		disconnect(connection);
		return items;
	}

	std::vector<std::string> getReceivingItems() {
		MYSQL* connection;
		connection = connect();
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
		disconnect(connection);
		return items;
	}

	void placeNewStock(std::vector<std::string> itemNames) {
		MYSQL* connection;
		connection = connect();
		std::string query;
		for (std::vector<std::string>::iterator it = itemNames.begin(); it != itemNames.end(); it++) {
			query = "INSERT INTO receiving_items (name) VALUES (\"";
			query += (*it) + "\");";
			mysql_query(connection, query.c_str());
		}
		disconnect(connection);
	}

	std::vector<int> whichBinsHaveRoom() {
		std::vector<int> bins;
		for (int i=0; i<22; i++) {
			int itemCount = Database::getBinItemCount(i+1);
			if (itemCount < 12) bins.push_back(i+1);
		}
		return bins;
	}

	void removeItemFromReceiving (std::string itemName) {
		MYSQL* connection;
		connection = connect();
		std::string query;
		query = "DELETE FROM receiving_items WHERE name=\""+itemName+"\";";
		mysql_query(connection, query.c_str());
		disconnect(connection);
	}

	void picker_take_item_from_receiving (int pickerId, std::string itemName) {
		MYSQL* connection;
		std::string query;
		try {
			connection = connect();
			disable_auto_commit(connection);
			query = "DELETE FROM receiving_items WHERE name=\""+itemName+"\";";
			mysql_query(connection, query.c_str());
			query = "UPDATE pickers SET name=\""+itemName+"\" WHERE picker_id="+std::to_string(pickerId)+";";
			mysql_query(connection, query.c_str());
			commit(connection);
			disconnect(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_take_item_from_receiving - "+e.message());
		}
	}

	void picker_place_item_into_stock (int pickerId, std::string itemName, int binId) {
		MYSQL* connection;
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		try {
			connection = connect();
			disable_auto_commit(connection);
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
						query = "UPDATE pickers SET name=NULL WHERE picker_id="+std::to_string(pickerId)+";";
						mysql_query(connection, query.c_str());
						commit(connection);
						disconnect(connection);
						return;
					} else {
						// Otherwise, add a new entry for item.
						mysql_free_result(result);
						query = "INSERT INTO stock_items (bin_id, name, quantity) VALUES (";
						query += std::to_string(binId)+", \""+itemName+"\", 1);";
						mysql_query(connection, query.c_str());
						query = "UPDATE pickers SET name=NULL WHERE picker_id="+std::to_string(pickerId)+";";
						mysql_query(connection, query.c_str());
						// Assume this works for now, add error checking later.
						commit(connection);
						disconnect(connection);
						return;
					}
				} else mysql_free_result(result);
			}
			disconnect(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_place_item_into_stock - "+e.message());
		}
	}
}