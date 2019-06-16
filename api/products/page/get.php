<?php

class Item {
	public $name;
}

class Success {
	public $status;
	public $limit;
	public $page;
	public $category;
	public $products;
}

class Failure {
	public $status;
}

$statusCode = array(
	"SUCCESS",
	"ERROR_invalid_limit",
	"ERROR_invalid_category",
	"ERROR_invalid_page",
	"ERROR_invalid_page_request_format"
);

// Get page details in JSON format.
$details = json_decode(file_get_contents("php://input"));

// Verify that the contents are in a valid order format.
$success = true;
if (!isset($details)) $success = false;
if (!array_key_exists("page", $details)) $success = false;
if (!array_key_exists("limit", $details)) $success = false;
if (!array_key_exists("category", $details)) $success = false;
if (!array_key_exists("quantity", $details)) $success = false;
if (!is_bool($details->quantity)) $success = false;
if (!array_key_exists("description", $details)) $success = false;
if (!is_bool($details->description)) $success = false;
if (!array_key_exists("image_url", $details)) $success = false;
if (!array_key_exists("price", $details)) $success = false;
if (!is_bool($details->image_url)) $success = false;
if (! $success) {
	$result = new Failure;
	$result->status = $statusCode[4];
	$response = json_encode($result);
	echo $response;
	die();
}

include "../../connection/connect.php";
$connection = connect();
if (isset($connection)) {
	// Validate category.
	$selectedCategory = $details->category;
	if ($selectedCategory != "all") {
		$query = "SELECT EXISTS(SELECT * FROM categories WHERE category=\"".$selectedCategory."\")";
		foreach ($connection->query($query) as $row) {
			$exists = boolval($row[0]);
		}
		if (! $exists) {
			$connection = null;
			$result = new Failure;
			$result->status = $statusCode[2];
			$response = json_encode($result);
			echo $response;
			die();
		}
	}
	// Validate limit.
	if (is_int($details->limit)) {
		$selectedLimit = $details->limit;
		if ($selectedLimit < 1) {
			$connection = null;
			$result = new Failure;
			$result->status = $statusCode[1];
			$response = json_encode($result);
			echo $response;
			die();
		}
	} else {
		$connection = null;
		$result = new Failure;
		$result->status = $statusCode[1];
		$response = json_encode($result);
		echo $response;
		die();
	}
	// Validate page.
	if (is_int($details->page)) {
		$selectedPage = $details->page;
		if ($selectedPage < 1) {
			$connection = null;
			$result = new Failure;
			$result->status = $statusCode[3];
			$response = json_encode($result);
			echo $response;
			die();
		}
	} else {
		$connection = null;
		$result = new Failure;
		$result->status = $statusCode[3];
		$response = json_encode($result);
		echo $response;
		die();
	}
	// Get how many pages are needed.
	if ($selectedCategory != "all") {
		$query = "SELECT COUNT(*) FROM products WHERE category=\"".$selectedCategory."\"";
	} else {
		$query = "SELECT COUNT(*) FROM products";
	}
	foreach ($connection->query($query) as $row) {
		$productCount = intval($row[0]);
	}
	if ($productCount == 0) {
		$pageCount = 0;
	} else {
		$pageCount = ceil($productCount/$selectedLimit);
	}
	if ($pageCount < $selectedPage) {
		$connection = null;
		$result = new Failure;
		$result->status = $statusCode[3];
		$response = json_encode($result);
		echo $response;
		die();
	}
	// Get the items.
	$offset = ($selectedPage-1)*$selectedLimit;
	if ($selectedCategory == "all") {
		$query = "SELECT * FROM products LIMIT ".$selectedLimit." OFFSET ".$offset;
	} else {
		$query = "SELECT * FROM products WHERE category=\"".$selectedCategory."\" LIMIT ".$selectedLimit." OFFSET ".$offset;
	}
	$selectedProducts = array();
	foreach ($connection->query($query) as $row) {
		$item = new Item;
		$item->name = $row['name'];
		if ($details->quantity) $item->quantity = intval($row['quantity']);
		if ($details->description) {
			$item->description = $row['description'];
			if (is_null($item->description)) $item->description = "";
		}
		if ($details->image_url) {
			$item->image_url = $row['image_url'];
			if (is_null($item->image_url)) $item->image_url = "";
		}
		if ($details->price) {
			$item->price = number_format($row['price'], 2, ".", "");
		}
		array_push($selectedProducts, $item);
	}
	// Return response.
	$result = new Success;
	$result->status = $statusCode[0];
	$result->limit = $selectedLimit;
	$result->page = $selectedPage;
	$result->category = $selectedCategory;
	$result->products = $selectedProducts;
	$response = json_encode($result);
	echo $response;
}
$connection = null;

?>