var previous_tile_id;
var bin_tile_div;
var selected_bin_id = 1;
var shipping_tile_div;
var selected_shipping = false;
var receiving_tile_div;
var selected_receiving = false;
var table_item = [];
var table_quantity = [];

// Create warehouse map.
var warehouse = [];
warehouse[0] = ['X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'R', 'X'];
warehouse[1] = ['X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'];
warehouse[2] = ['X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'];
warehouse[3] = ['X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'];
warehouse[4] = ['X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'];
warehouse[5] = ['X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'];
warehouse[6] = ['X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'];
warehouse[7] = ['X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'];
warehouse[8] = ['X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'];
warehouse[9] = ['X', 'S', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'];

function onTileClick(tile_div) {
	for (let b=0; b<22; b++) {
		if (bin_tile_div[b].id == tile_div.id) {
			selected_bin_id = b+1;
			selected_shipping = false;
			selected_receiving = false;
			return;
		}
	}
	if (shipping_tile_div.id == tile_div.id) {
		selected_bin_id = -1;
		selected_shipping = true;
		selected_receiving = false;
		return;
	}
	if (receiving_tile_div.id == tile_div.id) {
		selected_bin_id = -1;
		selected_shipping = false;
		selected_receiving = true;
	}
}

// Add tile divs to the DOM.
let row, column;
let warehouse_element = document.getElementById("warehouse-div");
for (row=0; row<10; row++) {
	for (column=0; column<10; column++) {
		let tile_div = document.createElement("div");
		tile_div.classList.add("tile");
		tile_div.id = row.toString()+","+column.toString();
		warehouse_element.appendChild(tile_div);
		tile_div.onclick = function(){onTileClick(this)};
	}
}
if (typeof shipping_tile_div === 'undefined') {
	let id = "9,1";
	shipping_tile_div = document.getElementById(id);
	shipping_tile_div.classList.add("shipping-bin");
}
if (typeof receiving_tile_div === 'undefined') {
	let id = "0,8";
	receiving_tile_div = document.getElementById(id);
	receiving_tile_div.classList.add("receiving-bin");
}

// Add rows to bin table in the DOM.
let table_element = document.getElementById("bin-table");
for (row=0; row<12; row++) {
	let table_row = document.createElement("tr");
	table_item[row] = document.createElement("td");
	table_quantity[row] = document.createElement("td");
	table_item[row].id = "table-item-"+row.toString();
	table_quantity[row].id = "table-quantity-"+row.toString();
	table_item[row].innerText = " ";
	table_quantity[row].innerText = " ";
	table_row.appendChild(table_item[row]);
	table_row.appendChild(table_quantity[row]);
	table_element.appendChild(table_row);
}

// Update tile classes.
for (row=0; row<10; row++) {
	for (column=0; column<10; column++) {
		let id = row.toString()+","+column.toString();
		let tile_div = document.getElementById(id);
		if (warehouse[row][column] == '.') {
			tile_div.classList.add("floor");
		} else if (warehouse[row][column] == 'X') {
			tile_div.classList.add("wall");
		} else {
			tile_div.classList.add("unknown");
		}
	}
}

function updateStockBinPositions() {
	var xhttpStockBinPositions = new XMLHttpRequest();
	xhttpStockBinPositions.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			let data = JSON.parse(this.responseText);
			if (typeof bin_tile_div === 'undefined') {
				bin_tile_div = [];
				for (let b=0; b<data.length; b++) {
					let id = data[b].row.toString()+","+data[b].column.toString();
					bin_tile_div[b] = document.getElementById(id);
					bin_tile_div[b].classList.remove("floor");
					switch (data[b].facing) {
						case 'u':
							bin_tile_div[b].classList.add("stock-bin-up");
							break;
						case 'd':
							bin_tile_div[b].classList.add("stock-bin-down");
							break;
						case 'l':
							bin_tile_div[b].classList.add("stock-bin-left");
							break;
						case 'r':
							bin_tile_div[b].classList.add("stock-bin-right");
							break;
						default:
							bin_tile_div[b].classList.add("stock-bin-error");
					}
				}
			}
		} else if (this.readyState == 4 && this.status != 200) {
			setTimeout(updateStockBinPositions, 100);
		}
	}
	xhttpStockBinPositions.open("GET", "api/bin_positions/read.php", true);
	xhttpStockBinPositions.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
	xhttpStockBinPositions.send();
}

function updateBinItemCount() {
	var xhttpBinItemCount = new XMLHttpRequest();
	xhttpBinItemCount.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			let data = JSON.parse(this.responseText);
			if (typeof bin_tile_div != 'undefined') {
				for (let b=0; b<data.stock.length; b++) {
					bin_tile_div[b].style.backgroundImage = "url(./graphics/item-"+data.stock[b].toString()+".svg)";
					bin_tile_div[b].style.backgroundSize = "80% 80%";
					bin_tile_div[b].style.backgroundRepeat = "no-repeat";
					bin_tile_div[b].style.backgroundPosition = "center";
				}
			}
			if (typeof receiving_tile_div != 'undefined') {
				receiving_tile_div.style.backgroundImage = "url(./graphics/item-"+data.receiving.toString()+".svg)";
				receiving_tile_div.style.backgroundSize = "80% 80%";
				receiving_tile_div.style.backgroundRepeat = "no-repeat";
				receiving_tile_div.style.backgroundPosition = "center";
			}
			if (typeof shipping_tile_div != 'undefined') {
				shipping_tile_div.style.backgroundImage = "url(./graphics/item-"+data.shipping.toString()+".svg)";
				shipping_tile_div.style.backgroundSize = "80% 80%";
				shipping_tile_div.style.backgroundRepeat = "no-repeat";
				shipping_tile_div.style.backgroundPosition = "center";
			}
			setTimeout(updateBinItemCount, 1000);
		} else if (this.readyState == 4 && this.status != 200) {
			setTimeout(updateBinItemCount, 1000);
		}
	}
	xhttpBinItemCount.open("GET", "api/bin_item_counts/read.php", true);
	xhttpBinItemCount.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
	xhttpBinItemCount.send();
}

function updateOrders() {
	var xhttpOrders = new XMLHttpRequest();
	xhttpOrders.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			let data = JSON.parse(this.responseText);
			if (data.length == 0) {
				document.getElementById("order").innerText = "No orders";
			} else {
				document.getElementById("order").innerText = "Processing order #"+data[0].toString()+".";
			}
			setTimeout(updateOrders, 1000);
		} else if (this.readyState == 4 && this.status != 200) {
			setTimeout(updateOrders, 1000);
		}
	}
	xhttpOrders.open("GET", "api/orders/read.php", true);
	xhttpOrders.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
	xhttpOrders.send();
}

function updatePickers() {
	var xhttpPickers = new XMLHttpRequest();
	xhttpPickers.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			let data = JSON.parse(this.responseText);
			// Remove old picker positions from display.
			if (typeof previous_tile_id !== "undefined") {
				for (let p=0; p<4; p++) {
					let tile_div = document.getElementById(previous_tile_id[p]);
					tile_div.classList.remove("picker");
					tile_div.innerText = "";
				}
			}
			// Update old positions for next call.
			// Update picker positions on display.
			previous_tile_id = [];
			for (let p=0; p<4; p++) {
				let id = data[p].row.toString()+","+data[p].column.toString();				
				previous_tile_id[p] = id;
				let tile_div = document.getElementById(id);
				tile_div.classList.add("picker");
				tile_div.innerText = p.toString() + " " +data[p].facing + "\n" + data[p].state;
			}
			setTimeout(updatePickers, 500);
		} else if (this.readyState == 4 && this.status != 200) {
			setTimeout(updatePickers, 500);
		}
	}
	xhttpPickers.open("GET", "api/pickers/read.php", true);
	xhttpPickers.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
	xhttpPickers.send();
}

function updateBinTable() {
	if (selected_bin_id != -1) {
		var xhttpTable = new XMLHttpRequest();
		xhttpTable.onreadystatechange = function() {
			if (this.readyState == 4 && this.status == 200) {
				let data = JSON.parse(this.responseText);
				document.getElementById("table-bin").innerText = "Bin: " + selected_bin_id.toString();
				for (let i=0; i<12; i++) {
					if (i<data.length) {
						table_item[i].innerText = data[i].name;
						table_quantity[i].innerText = data[i].quantity.toString();
					} else {
						table_item[i].innerText = " ";
						table_quantity[i].innerText = " ";
					}
				}
				setTimeout(updateBinTable, 500);	
			} else if (this.readyState == 4 && this.status != 200) {
				setTimeout(updateBinTable, 500);
			}
		}
		xhttpTable.open("GET", "api/bin/read.php?id="+selected_bin_id.toString(), true);
		xhttpTable.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
		xhttpTable.send();
	} else if (selected_shipping) {
		var xhttpTable = new XMLHttpRequest();
		xhttpTable.onreadystatechange = function() {
			if (this.readyState == 4 && this.status == 200) {
				let data = JSON.parse(this.responseText);
				document.getElementById("table-bin").innerText = "Bin: Shipping";
				for (let i=0; i<12; i++) {
					if (i<data.shippingItem.length) {
						table_item[i].innerText = data.shippingItem[i].name;
						table_quantity[i].innerText = data.shippingItem[i].quantity.toString()+"/"+data.shippingItem[i].needed.toString();
					} else {
						table_item[i].innerText = " ";
						table_quantity[i].innerText = " ";
					}
				}
				setTimeout(updateBinTable, 500);	
			} else if (this.readyState == 4 && this.status != 200) {
				setTimeout(updateBinTable, 500);
			}
		}
		xhttpTable.overrideMimeType("application/json");
		xhttpTable.open("GET", "shipping.json", true);
		xhttpTable.send();
	} else if (selected_receiving) {
		var xhttpTable = new XMLHttpRequest();
		xhttpTable.onreadystatechange = function() {
			if (this.readyState == 4 && this.status == 200) {
				let data = JSON.parse(this.responseText);
				document.getElementById("table-bin").innerText = "Bin: Receiving";
				for (let i=0; i<12; i++) {
					if (i<data.name.length) {
						table_item[i].innerText = data.name[i];
						table_quantity[i].innerText = 1;
					} else {
						table_item[i].innerText = " ";
						table_quantity[i].innerText = " ";
					}
				}
				setTimeout(updateBinTable, 500);	
			} else if (this.readyState == 4 && this.status != 200) {
				setTimeout(updateBinTable, 500);
			}
		}
		xhttpTable.overrideMimeType("application/json");
		xhttpTable.open("GET", "receiving.json", true);
		xhttpTable.send();
	} else setTimeout(updateBinTable, 500);
}

// Kick-start updates.
updateStockBinPositions();
updateBinItemCount();
updateOrders();
updatePickers();
updateBinTable();
