<?php

class Item {
	public $name;
	public $quantity;
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
	"ERROR_invalid_page"
);

include "../../connection/connect.php";
$connection = connect();
if (isset($connection)) {
	// Validate category.
	if (isset($_GET["category"])) {
		$selectedCategory = $_GET["category"];
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
	} else {
		$selectedCategory = "all";
	}
	// Validate limit.
	if (isset($_GET["limit"])) {
		if (ctype_digit($_GET["limit"])) {
			$selectedLimit = intval($_GET["limit"]);
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
	} else {
		$selectedLimit = 10;
	}
	// Validate page.
	if (isset($_GET["page"])) {
		if (ctype_digit($_GET["page"])) {
			$selectedPage = intval($_GET["page"]);
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
	} else {
		$selectedPage = 1;
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
		$item->quantity = intval($row['quantity']);
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