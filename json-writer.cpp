#include <fstream>
#include "json-writer.hpp"

void writeStateJSON(int currentOrderId, int numPickers, OrderPicker* picker[], int numBins, Position bin[], int nItems[]) {
	Position current;
	std::ofstream stateFile;
	stateFile.open("state.json", std::ios::out | std::ios::trunc);
	stateFile << "{";

	// order
	stateFile << "\"order\":";
	stateFile << currentOrderId;

	// pickers
	stateFile << ",\"picker\":[";
	for (int i=0; i<numPickers; i++) {
		current = picker[i]->getPosition();
		stateFile << "{\"position\":{";
		stateFile << "\"row\":";
		stateFile << current.row;
		stateFile << ",\"column\":";
		stateFile << current.column;
		stateFile << ",\"facing\":";
		stateFile << "\"" << DirectionToChar(current.facing) << "\"}";
		stateFile << ",\"hasItem\":";
		if (picker[i]->hasItem()) stateFile << "true";
		else stateFile << "false";
		stateFile << ",\"state\":";
		stateFile << "\"" << StateToString(picker[i]->getState()) << "\"";
		stateFile << "}";
		if (i != (numPickers-1)) stateFile << ",";
	}
	stateFile << "]";

	// bins
	stateFile << ",\"bin\":[";
	for (int i=0; i<numBins; i++) {
		current = bin[i];
		stateFile << "{\"position\":{";
		stateFile << "\"row\":";
		stateFile << current.row;
		stateFile << ",\"column\":";
		stateFile << current.column;
		stateFile << ",\"facing\":";
		stateFile << "\"" << DirectionToChar(current.facing) << "\"}";
		stateFile << ",\"nItems\":";
		stateFile << nItems[i];
		stateFile << "}";
		if (i != (numBins-1)) stateFile << ",";
	}
	stateFile << "]";

	stateFile << "}";
	stateFile.close();
}

void writeBinJSON(int binId, std::vector<Item> binContents) {
	std::ofstream binFile;
	binFile.open("bin_"+std::to_string(binId)+".json", std::ios::out | std::ios::trunc);
	binFile << "{\"item\":[";

	for (std::vector<Item>::iterator it = binContents.begin(); it != binContents.end(); it++) {
		binFile << "{\"name\":";
		binFile << "\"" << (*it).name << "\",";
		binFile << "\"quantity\":";
		binFile << std::to_string((*it).quantity) << "}";
		if (binContents.end() - it != 1) binFile << ",";
	}

	binFile << "]}";
	binFile.close();
}

void writeShippingJSON(std::vector<ShippingItem> shippingContents) {
	std::ofstream shippingFile;
	shippingFile.open("shipping.json", std::ios::out | std::ios::trunc);
	shippingFile << "{\"shippingItem\":[";

	for (std::vector<ShippingItem>::iterator it = shippingContents.begin(); it != shippingContents.end(); it++) {
		shippingFile << "{\"name\":";
		shippingFile << "\"" << (*it).name << "\",";
		shippingFile << "\"quantity\":";
		shippingFile << std::to_string((*it).quantity) << ",";
		shippingFile << "\"needed\":";
		shippingFile << std::to_string((*it).needed) << "}";
		if (shippingContents.end() - it != 1) shippingFile << ",";
	}

	shippingFile << "]}";
	shippingFile.close();
}

void writeReceivingJSON(std::vector<std::string> receivingContents) {
	std::ofstream receivingFile;
	receivingFile.open("receiving.json", std::ios::out | std::ios::trunc);
	receivingFile << "{\"name\":[";

	for (std::vector<std::string>::iterator it = receivingContents.begin(); it != receivingContents.end(); it++) {
		receivingFile << "\"" << (*it) << "\"";
		if (receivingContents.end() - it != 1) receivingFile << ",";
	}

	receivingFile << "]}";
	receivingFile.close();
}
