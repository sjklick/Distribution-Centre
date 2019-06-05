<?php

class Success {
	public $status;
	public $limit;
	public $pages;
	public $category;
}

class Failure {
	public $status;
}

$statusCode = array(
	"SUCCESS",
	"ERROR_invalid_limit",
	"ERROR_invalid_category"
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
	// Return response.
	$result = new Success;
	$result->status = $statusCode[0];
	$result->limit = $selectedLimit;
	$result->pages = $pageCount;
	$result->category = $selectedCategory;
	$response = json_encode($result);
	echo $response;
}
$connection = null;

?>