#ifndef __ORDER_PICKER__
#define __ORDER_PICKER__

#include <vector>
#include <string>
#include "position.hpp"
#include "state.hpp"
#include "database-access.hpp"

class OrderPicker {
private:
	Database db;
	Position home, current, target, extricate;
	std::vector<Position> path;
	bool item, delivered, holdingStockItem, stockDelivered, stock;
	std::string itemName, stockItemName;
	int binId, stockBinId;
	int yieldCount, id;
	void updateStateIdle(char map[10][10]);
	void updateStateYield(char map[10][10]);
	void updateStateHome(char map[10][10]);
	void updateStateRetrieve(char map[10][10]);
	void updateStateStock(char map[10][10]);
	void updateStateReceive(char map[10][10]);
	void updateStateShip(char map[10][10]);
	void updateStateExtend(char map[10][10]);
	void updateStateRetract(char map[10][10]);
	void updateStatePick(char map[10][10]);
	void updateStatePlace(char map[10][10]);
	void updateStateExtricate(char map[10][10]);
public:
	OrderPicker(int id, Position home);
	Position getPosition();
	State getState();
	bool hasItem();
	int getTargetBinId();
	std::string getItemName();
	bool processItem(int binId, Position bin, std::string itemName);
	void stockItem(int stockBinId, Position bin, std::string stockItemName);
	int getStockBin();
	std::string getStockItemName();
	void update(char map[10][10]);
	~OrderPicker();
};

#endif
