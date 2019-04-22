<?php

include "../connection/connect.php";
$connection = connect();
if (isset($connection)) {
	class Item {
		public $name;
		public $quantity;
	}
	$products = array();
	if (isset($_GET["category"])) {
		$category = $_GET["category"];
		$query = "SELECT * FROM products WHERE category=\"".$category."\"";
	} else {
		$query = "SELECT * FROM products";
	}
	foreach ($connection->query($query) as $row) {
		$item = new Item;
		$item->name = $row['name'];
		$item->quantity = $row['quantity'];
		array_push($products, $item);
	}
	echo json_encode($products);
}
$connection = null;

?>
