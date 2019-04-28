<?php

// Get order in JSON format.
$order = json_decode(file_get_contents("php://input"));

// Verify that the contents are in a valid order format.
$error = "Invalid order request format.\n";
if (!isset($order)) die($error);
if (!array_key_exists("customer", $order)) die($error);
if (!array_key_exists("email", $order)) die($error);
if (!array_key_exists("items", $order)) die($error);
for ($i=0; $i<count($order->items); $i++) {
	if (!array_key_exists("name", $order->items[$i])) die($error);
	if (!array_key_exists("quantity", $order->items[$i])) die($error);
}

// Place order in SQL database.
$error = "Unable to connect to warehouse.\n";
include "../connection/connect.php";
$connection = connect();
if (isset($connection)) {
	// Create customer entry.
	$query = "INSERT INTO orders (customer, email) VALUES (\"";
	$query .= $order->customer;
	$query .= "\", \"";
	$query .= $order->email;
	$query .= "\")";
	$connection->query($query);
	// Get order number.
	$id = $connection->lastInsertId();
	// Insert order items.
	for ($i=0; $i<count($order->items); $i++) {
		$query = "INSERT INTO order_items (order_id, name, quantity) VALUES (";
		$query .= $id;
		$query .= ", \"";
		$query .= $order->items[$i]->name;
		$query .= "\", ";
		$query .= $order->items[$i]->quantity;
		$query .= ")";
		$connection->query($query);
	}
} else die($error);
$connection = null;

echo "Order received.\n";

?>
