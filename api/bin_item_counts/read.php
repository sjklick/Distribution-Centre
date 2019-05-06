<?php

include "../connection/connect.php";
$connection = connect();
if (isset($connection)) {
	class ItemCount {
		public $stock;
		public $receiving;
		public $shipping;
	}
	$count = new ItemCount;
	$count->stock = array();
	for ($bin=1; $bin<23; $bin++) {
		$query = "SELECT SUM(quantity) FROM stock_items WHERE bin_id=".$bin;
		foreach ($connection->query($query) as $row) {
			$currentCount = intval($row[0]);
			array_push($count->stock, $currentCount);
		}
	}
	$query = "SELECT COUNT(name) FROM receiving_items";
	foreach ($connection->query($query) as $row) {
		$count->receiving = intval($row[0]);
	}
	$query = "SELECT SUM(quantity) FROM shipping_items";
	foreach ($connection->query($query) as $row) {
		$count->shipping = intval($row[0]);
	}
	echo json_encode($count);
}
$connection = null;

?>
