<?php

include "../connection/connect.php";
$connection = connect();
if (isset($connection)) {
	$orders = array();
	$query = "SELECT * FROM orders";
	foreach ($connection->query($query) as $row) {
		$order = intval($row[0]);
		array_push($orders, $order);
	}
	echo json_encode($orders);
}
$connection = null;

?>
