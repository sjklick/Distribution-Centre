var previous_tile_id;
var bin_tile_div;

// Create warehouse map.
var warehouse = [];
warehouse[0] = ['X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'];
warehouse[1] = ['X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'];
warehouse[2] = ['X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'];
warehouse[3] = ['X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'];
warehouse[4] = ['X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'];
warehouse[5] = ['X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'];
warehouse[6] = ['X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'];
warehouse[7] = ['X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'];
warehouse[8] = ['X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'];
warehouse[9] = ['X', 'S', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'];

// Add tile divs to the DOM.
let row, column;
let warehouse_element = document.getElementById("warehouse-div");
for (row=0; row<10; row++) {
	for (column=0; column<10; column++) {
		let tile_div = document.createElement("div");
		tile_div.classList.add("tile");
		tile_div.id = row.toString()+","+column.toString();
		warehouse_element.appendChild(tile_div);
	}
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
		} else if (warehouse[row][column] == 'S') {
			tile_div.classList.add("shipping");
		} else {
			tile_div.classList.add("unknown");
		}
	}
}

function updateState() {
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			var data = JSON.parse(this.responseText);
			// Update current order.
			if (data.order == -1) document.getElementById("order").innerText = "Processing no order.";
			else document.getElementById("order").innerText = "Processing order #"+data.order.toString()+".";
			// Remove old picker positions from display.
			if (typeof previous_tile_id !== "undefined") {
				for (let p=0; p<4; p++) {
					let tile_div = document.getElementById(previous_tile_id[p]);
					tile_div.classList.remove("picker");
					tile_div.innerText = "";
				}
			}
			// Initialize bin tiles.
			if (typeof bin_tile_div === 'undefined') {
				bin_tile_div = [];
				for (let b=0; b<22; b++) {
					let id = data.bin[b].position.row.toString()+","+data.bin[b].position.column.toString();
					bin_tile_div[b] = document.getElementById(id);
					bin_tile_div[b].classList.remove("floor");
					bin_tile_div[b].classList.add("bin");
				}
			}
			// Update old positions for next call.
			// Update picker positions on display.
			previous_tile_id = [];
			for (let p=0; p<4; p++) {
				let id = data.picker[p].position.row.toString()+","+data.picker[p].position.column.toString();				
				previous_tile_id[p] = id;
				let tile_div = document.getElementById(id);
				tile_div.classList.add("picker");
				tile_div.innerText = p.toString() + " " + data.picker[p].position.facing + "\n" + data.picker[p].state;
			}
			// Update bin text.
			for (let b=0; b<22; b++) {
					let id = data.bin[b].position.row.toString()+","+data.bin[b].position.column.toString();
					bin_tile_div[b].innerText = data.bin[b].position.facing + "\n" + data.bin[b].nItems.toString();
				}
			// Set an update request for half a second from now.
			setTimeout(updateState, 500);
		} else if (this.readyState == 4 && this.status != 200) {
			setTimeout(updateState, 500);
		}
	}
	xhttp.overrideMimeType("application/json");
	xhttp.open("GET", "state.json", true);
	xhttp.send();
}

// Kick-start updates.
updateState();
