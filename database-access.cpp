#include <string>
#include <fstream>
#include <mysql/mysql.h>
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
	if (mysql_autocommit(connection, 0) != 0) {
		error = "Failed to disable autocommit.";
		throw DatabaseException(error);
	}
}

static void commit (MYSQL* connection) {
	std::string error;
	if (mysql_commit(connection) != 0) {
		error = "Failed to commit.";
		throw DatabaseException(error);
	}
}

static void make_query (MYSQL* connection, std::string query) {
	std::string error;
	if (mysql_query(connection, query.c_str()) != 0) {
		error = "Query failed - ";
		error += mysql_error(connection);
		error += ".";
		throw DatabaseException(error);
	}
}

static MYSQL_RES* get_result (MYSQL* connection) {
	MYSQL_RES* result;
	std::string error;
	result = mysql_use_result(connection);
	if (result) return result;
	else {
		error = "Failed to get result from query - ";
		error += mysql_error(connection);
		error += ".";
		throw DatabaseException(error);
	}
}

namespace Database {
	// Stock bin related functions.

	std::vector<int> stock_get_id_list () {
		MYSQL* connection;
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		std::vector<int> id;
		try {
			connection = connect();	
			query = "SELECT bin_id FROM stock_bins;";
			make_query(connection, query);
			result = get_result(connection);
			while (row = mysql_fetch_row(result)) {
				id.push_back(std::stoi(row[0]));
			}
			mysql_free_result(result);
			return id;
		} catch (DatabaseException& e) {
			throw DatabaseException("stock_get_id_list - "+e.message());
		}
	}

	Position stock_get_position (int binId) {
		MYSQL* connection;
		MYSQL_RES* result;
		MYSQL_ROW row;
		Position bin;
		std::string query;
		try {
			connection = connect();	
			query = "SELECT * FROM stock_bins WHERE bin_id="+std::to_string(binId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if(row = mysql_fetch_row(result)) {
				bin.row = std::stoi(row[1]);
				bin.column = std::stoi(row[2]);
				bin.facing = CharToDirection(row[3][0]);
			} else {
				bin.row = -1;
				bin.column = -1;
				bin.facing = invalid;
			}
			mysql_free_result(result);
			disconnect(connection);
			return bin;
		} catch (DatabaseException& e) {
			throw DatabaseException("stock_get_position - "+e.message());
		}
	}

	int stock_get_item_count (int binId) {
		MYSQL* connection;
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		int nItems;
		try {
			connection = connect();
			query = "SELECT SUM(quantity) FROM stock_items WHERE bin_id="+std::to_string(binId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				if (row[0] == NULL) nItems = 0;
				else nItems = std::stoi(row[0]);
			} else nItems = -1;
			mysql_free_result(result);
			disconnect(connection);
			return nItems;
		} catch (DatabaseException& e) {
			throw DatabaseException("stock_get_item_count - "+e.message());
		}
	}

	std::vector<Item> stock_get_contents (int binId) {
		MYSQL* connection;
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		std::vector<Item> items;
		Item temp;
		try {
			connection = connect();
			query = "SELECT * FROM stock_items WHERE bin_id="+std::to_string(binId)+";";
			make_query(connection, query);
			result = get_result(connection);
			while (row = mysql_fetch_row(result)) {
				temp.name = std::string(row[1]);
				temp.quantity = std::stoi(row[2]);
				items.push_back(temp);
			}
			mysql_free_result(result);
			disconnect(connection);
			return items;
		} catch (DatabaseException& e) {
			throw DatabaseException("stock_get_contents - "+e.message());
		}
	}

	std::vector<int> stock_find_bins_with_room () {
		std::vector<int> bins;
		try {
			for (int i=0; i<22; i++) {
				int itemCount = Database::stock_get_item_count(i+1);
				if (itemCount < 12) bins.push_back(i+1);
			}
			return bins;
		} catch (DatabaseException& e) {
			throw DatabaseException("stock_find_bins_with_room - "+e.message());
		}
	}

	int stock_find_first_item_location (std::string item) {
		MYSQL* connection;
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		int binId;
		try {
			connection = connect();
			query = "SELECT * FROM stock_items WHERE name=\""+item+"\" AND QUANTITY>0 LIMIT 1;";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) binId = std::stoi(row[0]);
			else binId = -1;
			mysql_free_result(result);
			disconnect(connection);
			return binId;
		} catch (DatabaseException& e) {
			throw DatabaseException("stock_find_first_item_location - "+e.message());
		}
	}

	int stock_where_to_take_item (std::string item) {
		return 0;
	}

	int stock_where_to_place_item () {
		return 0;
	}

	// Receiving bin related functions.

	std::string receiving_get_next_item_to_stock () {
		std::string nextItem = "";
		return nextItem;
	}

	std::vector<std::string> receiving_get_items () {
		MYSQL* connection;
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		std::vector<std::string> items;
		try {
			connection = connect();
			query = "SELECT * FROM receiving_items;";
			make_query(connection, query);
			result = get_result(connection);
			while (row = mysql_fetch_row(result)) {
				items.push_back(std::string(row[0]));
			}
			mysql_free_result(result);
			disconnect(connection);
			return items;
		} catch (DatabaseException& e) {
			throw DatabaseException("receiving_get_items - "+e.message());
		}
	}

	void receiving_replenish () {
		MYSQL* connection;
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		std::vector<std::string> items;
		try {		
			connection = connect();
			query = "SELECT * FROM products ORDER BY quantity LIMIT 5;";
			make_query(connection, query);
			result = get_result(connection);
			while (row = mysql_fetch_row(result)) {
				items.push_back(std::string(row[0]));
			}
			mysql_free_result(result);
			disable_auto_commit(connection);
			for (std::vector<std::string>::iterator it = items.begin(); it != items.end(); it++) {
				query = "INSERT INTO receiving_items (name) VALUES (\""+(*it)+"\");";
				make_query(connection, query);
			}
			commit(connection);
			disconnect(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("receiving_replenish - "+e.message());
		}
	}

	// Shipping bin related functions.
	void shipping_clear () {
		MYSQL* connection;
		std::string query;
		try {
			connection = connect();
			query = "TRUNCATE shipping_items;";
			make_query(connection, query);
			disconnect(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("shipping_clear - "+e.message());
		}
	}

	// Customer order related functions.

	int order_get_current () {
		MYSQL* connection;
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		int orderId;
		try {
			connection = connect();
			query = "SELECT * FROM orders LIMIT 1;";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) orderId = std::stoi(row[0]);
			else orderId = -1;
			mysql_free_result(result);
			disconnect(connection);
			return orderId;
		} catch (DatabaseException& e) {
			throw DatabaseException("order_get_current - "+e.message());
		}
	}

	std::string order_get_next_item_to_ship (int orderId) {
		std::string nextItem = "";
		return nextItem;
	}

	bool order_check_if_ready (int orderId) {
		MYSQL* connection;
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		bool ready;
		try {
			connection = connect();
			query = "SELECT name, quantity FROM (";
			query += " SELECT name, quantity FROM order_items WHERE order_id="+std::to_string(orderId);
			query += " UNION ALL";
			query += " SELECT name, quantity FROM shipping_items";
			query += " ) temp";
			query += " GROUP BY name, quantity";
			query += " HAVING COUNT(*) = 1";
			query += " ORDER BY name;";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) ready = false;
			else ready = true;
			mysql_free_result(result);
			disconnect(connection);
			return ready;
		} catch (DatabaseException& e) {
			throw DatabaseException("order_check_if_ready - "+e.message());
		}
	}

	std::vector<Item> order_get_items (int orderId) {
		MYSQL* connection;
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		std::vector<Item> items;
		Item temp;
		try {
			connection = connect();
			query = "SELECT * FROM order_items WHERE order_id="+std::to_string(orderId)+";";
			make_query(connection, query);
			result = get_result(connection);
			while (row = mysql_fetch_row(result)) {
				temp.name = std::string(row[1]);
				temp.quantity = std::stoi(row[2]);
				items.push_back(temp);
			}
			mysql_free_result(result);
			disconnect(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("order_get_items - "+e.message());
		}
		return items;
	}

	void order_remove_items (int orderId) {
		MYSQL* connection;
		std::string query;
		try {
			connection = connect();
			query = "DELETE FROM order_items WHERE order_id="+std::to_string(orderId)+";";
			make_query(connection, query);
			disconnect(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("order_remove_items - "+e.message());
		}
	}

	void order_remove (int orderId) {
		MYSQL* connection;
		std::string query;
		try {
			connection = connect();
			query = "DELETE FROM orders WHERE order_id="+std::to_string(orderId)+";";
			make_query(connection, query);
			disconnect(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("order_remove - "+e.message());
		}
	}

	// Picker related functions.

	std::vector<int> picker_get_id_list () {
		std::vector<int> id;
		return id;
	}

	State picker_get_state (int pickerId) {
		MYSQL* connection;
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		try {
			query = "SELECT * FROM pickers WHERE picker_id="+std::to_string(pickerId)+";";
			connection = connect();
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				std::string state = row[7];
				disconnect(connection);
				return StringToState(state);
			}
			disconnect(connection);
			return State::invalid;
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_get_state - "+e.message());
		}
	}

	void picker_set_state (int pickerId, State state) {
		MYSQL* connection;
		std::string query;
		query = "UPDATE pickers Set state=\""+StateToString(state);
		query += "\" WHERE picker_id="+std::to_string(pickerId)+";";
		try {
			connection = connect();
			make_query(connection, query);
			disconnect(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_set_state - "+e.message());
		}
	}

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
			if (row = mysql_fetch_row(result)) {
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
			if (row = mysql_fetch_row(result)) {
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

	Position picker_get_target (int pickerId) {
		Position target;
		return target;
	}

	void picker_set_current (int pickerId, Position current) {
		MYSQL* connection;
		std::string query;
		try {
			connection = connect();
			query = "UPDATE pickers SET curr_row=";
			query += std::to_string(current.row);
			query += ", curr_col=";
			query += std::to_string(current.column);
			query += ", curr_dir=\"";
			query += DirectionToChar(current.facing);
			query += "\" WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			disconnect(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_set_current - "+e.message());
		}
	}

	void picker_set_target (int pickerId, Position current) {
	}

	void picker_take_item_from_receiving (int pickerId, std::string itemName) {
		MYSQL* connection;
		std::string query;
		try {
			connection = connect();
			disable_auto_commit(connection);
			query = "DELETE FROM receiving_items WHERE name=\""+itemName+"\";";
			make_query(connection, query);
			query = "UPDATE pickers SET name=\""+itemName+"\" WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			commit(connection);
			disconnect(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_take_item_from_receiving - "+e.message());
		}
	}

	void picker_take_item_from_stock (int pickerId, std::string itemName, int binId) {
		MYSQL* connection;
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		try {
			connection = connect();
			disable_auto_commit(connection);
			query = "SELECT * FROM stock_items WHERE bin_id="+std::to_string(binId)+" AND name=\""+itemName+"\";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				int quantity = std::stoi(row[2]);
				mysql_free_result(result);
				quantity--;
				if (quantity > 0) {
					query = "UPDATE stock_items SET quantity=quantity-1 WHERE bin_id=";
					query += std::to_string(binId)+" AND name=\""+itemName+"\";";
				} else {
					query = "DELETE FROM stock_items WHERE bin_id="+std::to_string(binId);
					query += " AND name=\""+itemName+"\";";
				}
			}
			make_query(connection, query);
			query = "UPDATE pickers SET name=\""+itemName+"\" WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			commit(connection);
			disconnect(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_take_item_from_stock - "+e.message());
		}
	}

	void picker_take_item_from_receiving (int pickerId) {
	}

	void picker_take_item_from_stock (int pickerId) {
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
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				// If it is in bin, update quantity.
				mysql_free_result(result);
				query = "UPDATE stock_items SET quantity=quantity+1";
				query += " WHERE bin_id="+std::to_string(binId)+" AND name=\""+itemName+"\";";
				make_query(connection, query);
				query = "UPDATE pickers SET name=NULL WHERE picker_id="+std::to_string(pickerId)+";";
				make_query(connection, query);
				commit(connection);
			} else {
				// Otherwise, add a new entry for item.
				mysql_free_result(result);
				query = "INSERT INTO stock_items (bin_id, name, quantity) VALUES (";
				query += std::to_string(binId)+", \""+itemName+"\", 1);";
				make_query(connection, query);
				query = "UPDATE pickers SET name=NULL WHERE picker_id="+std::to_string(pickerId)+";";
				make_query(connection, query);
				commit(connection);
			}
			disconnect(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_place_item_into_stock - "+e.message());
		}
	}

	void picker_place_item_into_shipping (int pickerId, std::string itemName) {
		MYSQL* connection;
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		try {
			connection = connect();
			disable_auto_commit(connection);
			// Check if item is already in bin.
			query = "SELECT * FROM shipping_items WHERE name=\""+itemName+"\";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				// If it is in bin, update quantity.
				mysql_free_result(result);
				query = "UPDATE shipping_items SET quantity=quantity+1 WHERE name=\""+itemName+"\";";
				make_query(connection, query);
				query = "UPDATE pickers SET name=NULL WHERE picker_id="+std::to_string(pickerId)+";";
				make_query(connection, query);
				commit(connection);
			} else {
				// Otherwise, add a new entry for item.
				mysql_free_result(result);
				query = "INSERT INTO shipping_items (name, quantity) VALUES (";
				query += "\""+itemName+"\", 1);";
				make_query(connection, query);
				query = "UPDATE pickers SET name=NULL WHERE picker_id="+std::to_string(pickerId)+";";
				make_query(connection, query);
				commit(connection);
			}
			disconnect(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_place_item_into_shipping - "+e.message());
		}
	}

	void picker_place_item_into_stock (int pickerId) {
	}

	void picker_place_item_into_shipping (int pickerId) {
	}

	bool picker_check_if_assigned (int pickerId) {
		return false;
	}

	bool picker_is_task_complete (int pickerId) {
		return false;
	}

	bool picker_is_task_ship (int pickerId) {
		return false;
	}

	bool picker_is_task_receive (int pickerId) {
		return false;
	}

	bool picker_has_item (int pickerId) {
		return false;
	}

	void picker_assign_shipping_task (int pickerId, std::string item, int binId) {
	}

	void picker_assign_receiving_task (int pickerId, std::string item, int binId) {
	}

	int picker_get_yield_count (int pickerId) {
		return 0;
	}

	void picker_increment_yield_count (int pickerId) {
	}

	void picker_reset_yield_count (int pickerId) {
	}
}