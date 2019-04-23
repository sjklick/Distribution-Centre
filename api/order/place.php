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

// For testing, just echo that this seems like a valid order format.
echo "Valid order request.\n";

?>
