<?php

class Success {
	public $success = true;
	public $orderId;
}

class Failure {
	public $success = false;
	public $error;
}

const errorRequestFormat = "invalid_order_request_format";
const errorServer = "server_side";
const errorCustomerInfo = "customer_info_refused";
const errorStock = "insufficient_stock";
const errorMaxItems = "maximum_items_exceeded";
const errorMaxOrders = "maximum_orders_exceeded";

// Get order in JSON format.
$order = json_decode(file_get_contents("php://input"));

// Verify that the contents are in a valid order format.
$success = true;
if (!isset($order)) $success = false;
if (!array_key_exists("customer", $order)) $success = false;
if (!array_key_exists("email", $order)) $success = false;
if (!array_key_exists("confirmReceived", $order)) $success = false;
if (!array_key_exists("confirmShipped", $order)) $success = false;
if (!array_key_exists("items", $order)) $success = false;
for ($i=0; $i<count($order->items); $i++) {
	if (!array_key_exists("name", $order->items[$i])) $success = false;
	if (!array_key_exists("quantity", $order->items[$i])) $success = false;
}
if (! $success) {
	$result = new Failure;
	$result->error = errorRequestFormat;
	$response = json_encode($result);
	die($response);
}

// Verify that the order does not exceed 12 items total.
$totalItems = 0;
for ($i=0; $i<count($order->items); $i++) {
	$totalItems += $order->items[$i]->quantity;
}
if ($totalItems > 12) {
	$result = new Failure;
	$result->error = errorMaxItems;
	$response = json_encode($result);
	die($response);
}

// Place order in SQL database.
include "../connection/connect.php";
$connection = connect();
if (isset($connection)) {
	if (! $connection->beginTransaction()) {
		$connection = null;
		$result = new Failure;
		$result->error = errorServer;
		$response = json_encode($result);
		die($response);
	}
	// Verify that another order can be accepted.
	$query = "SELECT COUNT(*) FROM orders";
	foreach ($connection->query($query) as $row) {
		$orderCount = intval($row[0]);
	}
	if ($orderCount > 4) {
		$connection = null;
		$result = new Failure;
		$result->error = errorMaxOrders;
		$response = json_encode($result);
		die($response);
	}
	// Create customer entry.
	$query = "INSERT INTO orders (customer, email, confirmation) VALUES (\"";
	$query .= $order->customer;
	$query .= "\", \"";
	$query .= $order->email;
	$query .= "\", ";
	if ($order->confirmShipped) $query .= "true";
	else $query .= "false";
	$query .= ")";
	try {
		$connection->query($query);
	} catch (PDOexception $exception) {
		$connection = null;
		$result = new Failure;
		$result->error = errorCustomerInfo;
		$response = json_encode($result);
		die($response);
	}
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
		try {
			$connection->query($query);
		} catch (PDOexception $exception) {
			$connection = null;
			$result = new Failure;
			$result->error = errorStock;
			$response = json_encode($result);
			die($response);
		}
	}
	if (! $connection->commit()) {
		$connection = null;
		$result = new Failure;
		$result->error = errorServer;
		$response = json_encode($result);
		die($response);
	}
	$connection = null;
} else {
	$connection = null;
	$result = new Failure;
	$result->error = errorServer;
	$response = json_encode($result);
	die($response);
}

// Order was placed without error.
$result = new Success;
$result->orderId = intval($id);
$response = json_encode($result);
echo $response;

if ($order->confirmReceived) {
	$to = $order->email;
	$subject = "Confirmation: Fictitious Order Received";
	$message = "Hi ".$order->customer.",\n\n";
	$message .= "This email is to confirm that your fictitious order has been received.";
	$message .= " Your order ID is ".$id.".\n\n";
	for ($i=0; $i<count($order->items); $i++) {
		$message .= $order->items[$i]->name;
		$message .= " (";
		$message .= $order->items[$i]->quantity;
		$message .= ")\n";
	}
	$message .= "\n";
	if ($order->confirmShipped) {
		$message .= "A confirmation email will be sent when your order ships. ";
	}
	$message .= "Thank you for choosing our fictitous service.";
	$message = wordwrap($message, 70);
	$additional_headers = "From: sjk.dev.test@gmail.com";
	mail($to, $subject, $message, $additional_headers);
}

?>
