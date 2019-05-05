<?php

include "../connection/connect.php";
$connection = connect();
if (isset($connection)) {
	class Item {
		public $name;
		public $quantity;
	}
	$stock = array();
	if (isset($_GET["id"])) {
		$id = $_GET["id"];
		$query = "SELECT * FROM stock_items WHERE bin_id=".$id;
	}
	foreach ($connection->query($query) as $row) {
		$item = new Item;
		$item->name = $row['name'];
		$item->quantity = intval($row['quantity']);
		array_push($stock, $item);
	}
	echo json_encode($stock);
}
$connection = null;

?>
