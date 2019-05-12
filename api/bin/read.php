<?php

include "../connection/connect.php";
$connection = connect();
if (isset($connection)) {
	class Item {
		public $name;
		public $quantity;
	}
	class Data {
		public $id;
		public $item = array();
	}
	$data = new Data;
	if (isset($_GET["id"])) {
		$id = $_GET["id"];
		$data->id = $id;
		if ($id == "shipping") $query = "SELECT * FROM shipping_items";
		else if ($id == "receiving") $query = "SELECT * FROM receiving_items";
		else $query = "SELECT * FROM stock_items WHERE bin_id=".$id;
	}
	foreach ($connection->query($query) as $row) {
		$temp = new Item;
		$temp->name = $row['name'];
		if ($id == "receiving") $temp->quantity = 1;
		else $temp->quantity = intval($row['quantity']);
		array_push($data->item, $temp);
	}
	echo json_encode($data);
}
$connection = null;

?>
