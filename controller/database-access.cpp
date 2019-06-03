#include <string>
#include <fstream>
#include <mysql/mysql.h>
#include "database-access.hpp"
#include "database-exception.hpp"

static MYSQL* connection;

static MYSQL* connect () {
	MYSQL* conn;
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
	conn = mysql_init(NULL);
	if (conn != NULL) {
		if (!mysql_real_connect(conn, "localhost", username.c_str(), password.c_str(), "stock", 0, socket.c_str(), 0)) {
			error = "Failed to connect to database - ";
			error += mysql_error(conn);
			error += ".";
			throw DatabaseException(error);
		}
	} else throw DatabaseException(error);
	return conn;
}

static void disconnect (MYSQL* conn) {
	mysql_close(conn);
}

static void disable_auto_commit (MYSQL* conn) {
	std::string error;
	if (mysql_autocommit(conn, 0) != 0) {
		error = "Failed to disable autocommit.";
		throw DatabaseException(error);
	}
}

static void enable_auto_commit (MYSQL* conn) {
	std::string error;
	if (mysql_autocommit(conn, 1) != 0) {
		error = "Failed to enable autocommit.";
		throw DatabaseException(error);
	}
}

static void commit (MYSQL* conn) {
	std::string error;
	if (mysql_commit(conn) != 0) {
		error = "Failed to commit.";
		throw DatabaseException(error);
	}
}

static void make_query (MYSQL* conn, std::string query) {
	std::string error;
	if (mysql_query(conn, query.c_str()) != 0) {
		error = "Query failed - ";
		error += mysql_error(conn);
		error += ".";
		throw DatabaseException(error);
	}
}

static MYSQL_RES* get_result (MYSQL* conn) {
	MYSQL_RES* result;
	std::string error;
	result = mysql_use_result(conn);
	if (result) return result;
	else {
		error = "Failed to get result from query - ";
		error += mysql_error(conn);
		error += ".";
		throw DatabaseException(error);
	}
}

namespace Database {
	// Connection related functions.

	void connection_create () {
		try {
			connection = connect();	
		} catch (DatabaseException& e) {
			throw DatabaseException("connection_create - "+e.message());
		}
	}

	void connection_destroy () {
		disconnect(connection);
	}

	// Stock bin related functions.

	std::vector<int> stock_get_id_list () {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		std::vector<int> id;
		try {
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
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		try {
			query = "SELECT * FROM stock_bins WHERE bin_id="+std::to_string(binId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				Position bin;
				bin.row = std::stoi(row[1]);
				bin.column = std::stoi(row[2]);
				bin.facing = CharToDirection(row[3][0]);
				mysql_free_result(result);
				return bin;
			}
			mysql_free_result(result);
			std::string error;
			error = "Invalid stock bin.";
			throw DatabaseException(error);
		} catch (DatabaseException& e) {
			throw DatabaseException("stock_get_position - "+e.message());
		}
	}

	int stock_get_item_count (int binId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		int nItems;
		try {
			query = "SELECT SUM(quantity) FROM stock_items WHERE bin_id="+std::to_string(binId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				if (row[0] == NULL) nItems = 0;
				else nItems = std::stoi(row[0]);
			} else nItems = -1;
			mysql_free_result(result);
			return nItems;
		} catch (DatabaseException& e) {
			throw DatabaseException("stock_get_item_count - "+e.message());
		}
	}

	std::vector<Item> stock_get_contents (int binId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		std::vector<Item> items;
		Item temp;
		try {
			query = "SELECT * FROM stock_items WHERE bin_id="+std::to_string(binId)+";";
			make_query(connection, query);
			result = get_result(connection);
			while (row = mysql_fetch_row(result)) {
				temp.name = std::string(row[1]);
				temp.quantity = std::stoi(row[2]);
				items.push_back(temp);
			}
			mysql_free_result(result);
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

	int stock_where_to_take_item (std::string item) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		try {
			query = "SELECT * FROM stock_items WHERE name=\""+item+"\" AND ";
			query += "quantity>(SELECT COUNT(*) FROM picker_tasks WHERE item_name=";
			query += "\""+item+"\" AND task_type=\"ship\") LIMIT 1;";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				if (row[0] != NULL) {
					int binId = std::stoi(row[0]);
					mysql_free_result(result);
					return binId;
				}
			}
			mysql_free_result(result);
			std::string error;
			error = "Insufficient stock.";
			throw DatabaseException(error);
		} catch (DatabaseException& e) {
			throw DatabaseException("stock_where_to_take_item - "+e.message());
		}
	}

	int stock_where_to_place_item () {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		try {
			query = "SELECT bin_id FROM stock_bins WHERE ";
			query += "(SELECT COALESCE(SUM(quantity), 0) FROM stock_items WHERE stock_items.bin_id=stock_bins.bin_id)+";
			query += "(SELECT COUNT(*) FROM picker_tasks WHERE picker_tasks.bin_id=stock_bins.bin_id ";
			query += "AND task_type=\"receive\")<12 ORDER BY RAND() LIMIT 1;";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				if (row[0] != NULL) {
					int binId = std::stoi(row[0]);
					mysql_free_result(result);
					return binId;
				}
			}
			mysql_free_result(result);
			std::string error;
			error = "Insufficient room.";
			throw DatabaseException(error);
		} catch (DatabaseException& e) {
			throw DatabaseException("stock_where_to_place_item - "+e.message());
		}
	}

	// Receiving bin related functions.

	std::string receiving_get_next_item_to_stock () {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		std::vector<std::string> receivingItems;
		try {
			receivingItems = Database::receiving_get_items();
			for (std::vector<std::string>::iterator it = receivingItems.begin(); it != receivingItems.end(); it++) {
				query = "SELECT COUNT(*) FROM picker_tasks WHERE item_name=";
				query += "\""+(*it)+"\" AND task_type=\"receive\";";
				make_query(connection, query);
				result = get_result(connection);
				if (row = mysql_fetch_row(result)) {
					if (std::stoi(row[0]) == 0) {
						mysql_free_result(result);
						return (*it);
					}
				}
				mysql_free_result(result);
			}
			return "";
		} catch (DatabaseException& e) {
			throw DatabaseException("receiving_get_next_item_to_stock - "+e.message());
		}
	}

	std::vector<std::string> receiving_get_items () {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		std::vector<std::string> items;
		try {
			query = "SELECT * FROM receiving_items;";
			make_query(connection, query);
			result = get_result(connection);
			while (row = mysql_fetch_row(result)) {
				items.push_back(std::string(row[0]));
			}
			mysql_free_result(result);
			return items;
		} catch (DatabaseException& e) {
			throw DatabaseException("receiving_get_items - "+e.message());
		}
	}

	void receiving_replenish () {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		std::vector<std::string> items;
		try {		
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
			enable_auto_commit(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("receiving_replenish - "+e.message());
		}
	}

	// Shipping bin related functions.
	void shipping_clear () {
		std::string query;
		try {
			query = "TRUNCATE shipping_items;";
			make_query(connection, query);
		} catch (DatabaseException& e) {
			throw DatabaseException("shipping_clear - "+e.message());
		}
	}

	std::vector<Item> shipping_get_items () {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		std::vector<Item> shippingItems;
		try {
			query = "SELECT * FROM shipping_items;";
			make_query(connection, query);
			result = get_result(connection);
			while (row = mysql_fetch_row(result)) {
				Item temp;
				temp.name = std::string(row[0]);
				temp.quantity = std::stoi(row[1]);
				shippingItems.push_back(temp);
			}
			mysql_free_result(result);
			return shippingItems;
		} catch (DatabaseException& e) {
			throw DatabaseException("shipping_get_items - "+e.message());
		}
	}

	// Customer order related functions.

	int order_get_current () {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		int orderId;
		try {
			query = "SELECT * FROM orders LIMIT 1;";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) orderId = std::stoi(row[0]);
			else orderId = -1;
			mysql_free_result(result);
			return orderId;
		} catch (DatabaseException& e) {
			throw DatabaseException("order_get_current - "+e.message());
		}
	}

	std::string order_get_next_item_to_ship (int orderId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		std::vector<Item> orderItems, shippingItems;
		try {
			orderItems = Database::order_get_items(orderId);
			shippingItems = Database::shipping_get_items();
			for (std::vector<Item>::iterator it = orderItems.begin(); it != orderItems.end(); it++) {
				for (std::vector<Item>::iterator it2 = shippingItems.begin(); it2 != shippingItems.end(); it2++) {
					if ((*it).name == (*it2).name) {
						(*it).quantity -= (*it2).quantity;
					}
				}
			}
			for (std::vector<Item>::iterator it = orderItems.begin(); it != orderItems.end(); it++) {
				query = "SELECT COUNT(*) FROM picker_tasks WHERE item_name=";
				query += "\""+(*it).name+"\" AND task_type=\"ship\";";
				make_query(connection, query);
				result = get_result(connection);
				if (row = mysql_fetch_row(result)) {
					(*it).quantity -= std::stoi(row[0]);
				}
				mysql_free_result(result);
			}
			for (std::vector<Item>::iterator it = orderItems.begin(); it != orderItems.end(); it++) {
				if ((*it).quantity > 0) {
					return (*it).name;
				}
			}
			return "";
		} catch (DatabaseException& e) {
			throw DatabaseException("order_get_next_item_to_ship - "+e.message());
		}
	}

	bool order_check_if_ready (int orderId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		bool ready;
		try {
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
			return ready;
		} catch (DatabaseException& e) {
			throw DatabaseException("order_check_if_ready - "+e.message());
		}
	}

	std::vector<Item> order_get_items (int orderId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		std::vector<Item> items;
		Item temp;
		try {
			query = "SELECT * FROM order_items WHERE order_id="+std::to_string(orderId)+";";
			make_query(connection, query);
			result = get_result(connection);
			while (row = mysql_fetch_row(result)) {
				temp.name = std::string(row[1]);
				temp.quantity = std::stoi(row[2]);
				items.push_back(temp);
			}
			mysql_free_result(result);
		} catch (DatabaseException& e) {
			throw DatabaseException("order_get_items - "+e.message());
		}
		return items;
	}

	std::string order_get_customer_name (int orderId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query, customer;
		try {
			customer = "";
			query = "SELECT customer FROM orders WHERE order_id="+std::to_string(orderId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				customer = std::string(row[0]);
			}
			mysql_free_result(result);
			return customer;
		} catch (DatabaseException& e) {
			throw DatabaseException("order_get_customer_name - "+e.message());
		}
	}

	std::string order_get_customer_email (int orderId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query, email;
		try {
			email = "";
			query = "SELECT email FROM orders WHERE order_id="+std::to_string(orderId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				if (row[0] != NULL) email = std::string(row[0]);
			}
			mysql_free_result(result);
			return email;
		} catch (DatabaseException& e) {
			throw DatabaseException("order_get_customer_email - "+e.message());
		}
	}

	bool order_confirmation_needed (int orderId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		bool needed;
		try {
			needed = false;
			query = "SELECT confirmation FROM orders WHERE order_id="+std::to_string(orderId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				needed = bool(std::stoi(row[0]));
			}
			mysql_free_result(result);
			return needed;
		} catch (DatabaseException& e) {
			throw DatabaseException("order_confirmation_needed - "+e.message());
		}
	}

	void order_remove_items (int orderId) {
		std::string query;
		try {
			query = "DELETE FROM order_items WHERE order_id="+std::to_string(orderId)+";";
			make_query(connection, query);
		} catch (DatabaseException& e) {
			throw DatabaseException("order_remove_items - "+e.message());
		}
	}

	void order_remove (int orderId) {
		std::string query;
		try {
			query = "DELETE FROM orders WHERE order_id="+std::to_string(orderId)+";";
			make_query(connection, query);
		} catch (DatabaseException& e) {
			throw DatabaseException("order_remove - "+e.message());
		}
	}

	// Picker related functions.

	std::vector<int> picker_get_id_list () {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		std::vector<int> id;
		try {
			query = "SELECT picker_id FROM pickers ORDER BY picker_id;";
			make_query(connection, query);
			result = get_result(connection);
			while (row = mysql_fetch_row(result)) {
				id.push_back(std::stoi(row[0]));
			}
			mysql_free_result(result);
			return id;
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_get_id_list - "+e.message());
		}
	}

	State picker_get_state (int pickerId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		try {
			query = "SELECT state FROM pickers WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				std::string state = std::string(row[0]);
				mysql_free_result(result);
				return StringToState(state);
			}
			mysql_free_result(result);
			std::string error;
			error = "Failed to get a state.";
			throw DatabaseException(error);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_get_state - "+e.message());
		}
	}

	void picker_set_state (int pickerId, State state) {
		std::string query;
		query = "UPDATE pickers Set state=\""+StateToString(state);
		query += "\" WHERE picker_id="+std::to_string(pickerId)+";";
		try {
			make_query(connection, query);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_set_state - "+e.message());
		}
	}

	Position picker_get_home (int pickerId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		Position home;
		std::string query;
		try {
			query = "SELECT * FROM pickers WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				home.row = std::stoi(row[1]);
				home.column = std::stoi(row[2]);
				home.facing = CharToDirection(row[3][0]);			
			}
			mysql_free_result(result);
			return home;
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_get_home - "+e.message());
		}
	}

	Position picker_get_current (int pickerId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		Position current;
		std::string query;
		try {
			query = "SELECT * FROM pickers WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				current.row = std::stoi(row[4]);
				current.column = std::stoi(row[5]);
				current.facing = CharToDirection(row[6][0]);
			}
			mysql_free_result(result);
			return current;
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_get_current - "+e.message());
		}
	}

	Position picker_get_target (int pickerId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		Position target;
		std::string query;
		try {
			query = "SELECT * FROM pickers WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				target.row = std::stoi(row[7]);
				target.column = std::stoi(row[8]);
				target.facing = CharToDirection(row[9][0]);
			}
			mysql_free_result(result);
			return target;
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_get_target - "+e.message());
		}
	}

	void picker_set_current (int pickerId, Position current) {
		std::string query;
		try {
			query = "UPDATE pickers SET curr_row=";
			query += std::to_string(current.row);
			query += ", curr_col=";
			query += std::to_string(current.column);
			query += ", curr_dir=\"";
			query += DirectionToChar(current.facing);
			query += "\" WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_set_current - "+e.message());
		}
	}

	void picker_set_target (int pickerId, Position target) {
		std::string query;
		try {
			query = "UPDATE pickers SET trgt_row=";
			query += std::to_string(target.row);
			query += ", trgt_col=";
			query += std::to_string(target.column);
			query += ", trgt_dir=\"";
			query += DirectionToChar(target.facing);
			query += "\" WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_set_target - "+e.message());
		}
	}

	void picker_take_item_from_receiving (int pickerId) {
		std::string query;
		try {
			disable_auto_commit(connection);
			query = "DELETE FROM receiving_items WHERE name=";
			query += "(SELECT item_name FROM picker_tasks WHERE task_id=";
			query += "(SELECT task_id FROM pickers WHERE picker_id=";
			query += std::to_string(pickerId)+"));";
			make_query(connection, query);
			query = "UPDATE pickers SET has_item=TRUE WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			commit(connection);
			enable_auto_commit(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_take_item_from_receiving - "+e.message());
		}
	}

	void picker_take_item_from_stock (int pickerId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query, itemName;
		int taskId, binId, quantity;
		try {
			disable_auto_commit(connection);
			query = "SELECT task_id FROM pickers WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				taskId = std::stoi(row[0]);
			}
			mysql_free_result(result);
			query = "SELECT bin_id FROM picker_tasks WHERE task_id="+std::to_string(taskId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				binId = std::stoi(row[0]);
			}
			mysql_free_result(result);
			query = "SELECT item_name FROM picker_tasks WHERE task_id="+std::to_string(taskId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				itemName = std::string(row[0]);
			}
			mysql_free_result(result);
			query = "SELECT quantity FROM stock_items WHERE bin_id="+std::to_string(binId)+" AND name=\""+itemName+"\";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				if (row[0] == NULL) quantity = -1;
				else quantity = std::stoi(row[0]);
			}
			mysql_free_result(result);
			if (quantity > 1) {
				query = "UPDATE stock_items SET quantity=quantity-1 WHERE bin_id=";
				query += std::to_string(binId)+" AND name=\""+itemName+"\";";
			} else {
				query = "DELETE FROM stock_items WHERE bin_id="+std::to_string(binId);
				query += " AND name=\""+itemName+"\";";
			}
			make_query(connection, query);
			query = "UPDATE pickers SET has_item=TRUE WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			commit(connection);
			enable_auto_commit(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_take_item_from_stock - "+e.message());
		}
	}

	void picker_place_item_into_stock (int pickerId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query, itemName;
		int taskId, binId, quantity;
		try {
			disable_auto_commit(connection);
			query = "SELECT task_id FROM pickers WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				taskId = std::stoi(row[0]);
			}
			mysql_free_result(result);
			query = "SELECT bin_id FROM picker_tasks WHERE task_id="+std::to_string(taskId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				binId = std::stoi(row[0]);
			}
			mysql_free_result(result);
			query = "SELECT item_name FROM picker_tasks WHERE task_id="+std::to_string(taskId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				itemName = std::string(row[0]);
			}
			mysql_free_result(result);
			query = "SELECT quantity FROM stock_items WHERE bin_id="+std::to_string(binId)+" AND name=\""+itemName+"\";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				quantity = std::stoi(row[0]);
			} else {
				quantity = 0;
			}
			mysql_free_result(result);
			if (quantity > 0) {
				query = "UPDATE stock_items SET quantity=quantity+1 WHERE bin_id=";
				query += std::to_string(binId)+" AND name=\""+itemName+"\";";
			} else {
				query = "INSERT INTO stock_items (bin_id, name, quantity) VALUES (";
				query += std::to_string(binId)+", \""+itemName+"\", 1) ";
			}
			make_query(connection, query);
			query = "UPDATE pickers SET has_item=FALSE WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			query = "UPDATE pickers SET task_id=NULL WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			query = "DELETE FROM picker_tasks WHERE task_id="+std::to_string(taskId)+";";
			make_query(connection, query);
			commit(connection);
			enable_auto_commit(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_place_item_into_stock - "+e.message());
		}
	}

	void picker_place_item_into_shipping (int pickerId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query, itemName;
		int taskId, quantity;
		try {
			disable_auto_commit(connection);
			query = "SELECT task_id FROM pickers WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				taskId = std::stoi(row[0]);
			}
			mysql_free_result(result);
			query = "SELECT item_name FROM picker_tasks WHERE task_id="+std::to_string(taskId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				itemName = std::string(row[0]);
			}
			mysql_free_result(result);
			query = "SELECT quantity FROM shipping_items WHERE name=\""+itemName+"\";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				quantity = std::stoi(row[0]);
			} else {
				quantity = 0;
			}
			mysql_free_result(result);
			if (quantity > 0) {
				query = "UPDATE shipping_items SET quantity=quantity+1 WHERE name=";
				query += "\""+itemName+"\";";
			} else {
				query = "INSERT INTO shipping_items (name, quantity) VALUES (";
				query += "\""+itemName+"\", 1);";
			}
			make_query(connection, query);
			query = "UPDATE pickers SET has_item=FALSE WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			query = "UPDATE pickers SET task_id=NULL WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			query = "DELETE FROM picker_tasks WHERE task_id="+std::to_string(taskId)+";";
			make_query(connection, query);
			commit(connection);
			enable_auto_commit(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_place_item_into_shipping - "+e.message());
		}
	}

	bool picker_check_if_assigned (int pickerId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		try {
			query = "SELECT task_id FROM pickers WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				bool assigned;
				if (row[0] == NULL) assigned = false;
				else assigned = true;
				mysql_free_result(result);
				return assigned;
			} else {
				mysql_free_result(result);
				std::string error;
				error = "Failed to determine if picker has an assignment.";
				throw DatabaseException(error);
			}
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_check_if_assigned - "+e.message());
		}
	}

	bool picker_is_task_ship (int pickerId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		try {
			query = "SELECT task_type FROM picker_tasks WHERE task_id=";
			query += "(SELECT task_id FROM pickers WHERE picker_id="+std::to_string(pickerId)+");";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				std::string taskType = row[0];
				mysql_free_result(result);
				if (taskType == "ship") return true;
				return false;
			} else {
				mysql_free_result(result);
				std::string error;
				error = "Failed to get task type.";
				throw DatabaseException(error);
			}
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_is_task_ship - "+e.message());
		}
	}

	bool picker_is_task_receive (int pickerId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		try {
			query = "SELECT task_type FROM picker_tasks WHERE task_id=";
			query += "(SELECT task_id FROM pickers WHERE picker_id="+std::to_string(pickerId)+");";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				std::string taskType = row[0];
				mysql_free_result(result);
				if (taskType == "receive") return true;
				return false;
			} else {
				mysql_free_result(result);
				std::string error;
				error = "Failed to get task type.";
				throw DatabaseException(error);
			}
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_is_task_receive - "+e.message());
		}
	}

	bool picker_has_item (int pickerId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		try {
			query = "SELECT has_item FROM pickers WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				bool hasItem = bool(std::stoi(row[0]));
				mysql_free_result(result);
				return hasItem;
			} else {
				mysql_free_result(result);
				std::string error;
				error = "Failed to determine if picker has item.";
				throw DatabaseException(error);
			}
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_has_item - "+e.message());
		}
	}

	int picker_get_assigned_bin (int pickerId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		try {
			query = "SELECT bin_id FROM picker_tasks WHERE task_id=(SELECT task_id FROM pickers ";
			query += "WHERE picker_id="+std::to_string(pickerId)+");";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				int binId = std::stoi(row[0]);
				mysql_free_result(result);
				return binId;
			}
			mysql_free_result(result);
			std::string error;
			error = "Failed to get bin ID.";
			throw DatabaseException(error);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_get_assigned_bin - "+e.message());
		}
	}

	void picker_assign_shipping_task (int pickerId, std::string item, int binId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		int taskId;
		try {
			disable_auto_commit(connection);
			query = "INSERT INTO picker_tasks (task_type, item_name, bin_id) VALUES (";
			query += "\"ship\", ";
			query += "\""+item+"\", ";
			query += std::to_string(binId);
			query += ");";
			make_query(connection, query);
			query = "SELECT LAST_INSERT_ID();";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				taskId = std::stoi(row[0]);
			}
			mysql_free_result(result);
			query = "UPDATE pickers SET task_id="+std::to_string(taskId);
			query += " WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			commit(connection);
			enable_auto_commit(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_assign_shipping_task - "+e.message());
		}
	}

	void picker_assign_receiving_task (int pickerId, std::string item, int binId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		int taskId;
		try {
			disable_auto_commit(connection);
			query = "INSERT INTO picker_tasks (task_type, item_name, bin_id) VALUES (";
			query += "\"receive\", ";
			query += "\""+item+"\", ";
			query += std::to_string(binId);
			query += ");";
			make_query(connection, query);
			query = "SELECT LAST_INSERT_ID();";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				taskId = std::stoi(row[0]);
			}
			mysql_free_result(result);
			query = "UPDATE pickers SET task_id="+std::to_string(taskId);
			query += " WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			commit(connection);
			enable_auto_commit(connection);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_assign_receiving_task - "+e.message());
		}
	}

	int picker_get_yield_count (int pickerId) {
		MYSQL_RES* result;
		MYSQL_ROW row;
		std::string query;
		try {
			query = "SELECT yield_count FROM pickers WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
			result = get_result(connection);
			if (row = mysql_fetch_row(result)) {
				int count = std::stoi(row[0]);
				mysql_free_result(result);
				return count;
			} else {
				mysql_free_result(result);
				std::string error;
				error = "Failed to get yield count.";
				throw DatabaseException(error);
			}
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_get_yield_count - "+e.message());
		}
	}

	void picker_increment_yield_count (int pickerId) {
		std::string query;
		try {
			query = "UPDATE pickers SET yield_count=yield_count+1 WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_increment_yield_count - "+e.message());
		}
	}

	void picker_reset_yield_count (int pickerId) {
		std::string query;
		try {
			query = "UPDATE pickers SET yield_count=0 WHERE picker_id="+std::to_string(pickerId)+";";
			make_query(connection, query);
		} catch (DatabaseException& e) {
			throw DatabaseException("picker_reset_yield_count - "+e.message());
		}
	}
}