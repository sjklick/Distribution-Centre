<?php

class Success {
	public $success = true;
	public $limit;
	public $pages;
	public $category;
}

class Failure {
	public $success = false;
	public $error;
}

const errorRequestFormat = "invalid_page_count_request_format";
const errorLimit = "invalid_limit";
const errorCategory = "invalid_category";

// Get POST body.
$details = json_decode(file_get_contents("php://input"));

// Validate POST body format.
if (!isset($details) || !array_key_exists("category", $details) || !array_key_exists("limit", $details)) {
	$connection = null;
	$result = new Failure;
	$result->error = errorRequestFormat;
	$response = json_encode($result);
	die($response);
}

include "../../connection/connect.php";
$connection = connect();
if (isset($connection)) {
	// Validate category.
	if ($details->category != "all") {
		$query = "SELECT EXISTS(SELECT * FROM categories WHERE category=\"".$details->category."\")";
		foreach ($connection->query($query) as $row) {
			$exists = boolval($row[0]);
		}
		if (! $exists) {
			$connection = null;
			$result = new Failure;
			$result->error = errorCategory;
			$response = json_encode($result);
			die($response);
		}
	}

	// Validate limit.
	if (!ctype_digit((string) $details->limit) || ($details->limit < 1)) {
		$connection = null;
		$result = new Failure;
		$result->error = errorLimit;
		$response = json_encode($result);
		die($response);
	}

	// Get how many pages are needed.
	if ($details->category != "all") {
		$query = "SELECT COUNT(*) FROM products WHERE category=\"".$details->category."\"";
	} else {
		$query = "SELECT COUNT(*) FROM products";
	}
	foreach ($connection->query($query) as $row) {
		$productCount = intval($row[0]);
	}
	if ($productCount == 0) {
		$pageCount = 0;
	} else {
		$pageCount = ceil($productCount/$details->limit);
	}
	// Return response.
	$result = new Success;
	$result->limit = $details->limit;
	$result->pages = $pageCount;
	$result->category = $details->category;
	$response = json_encode($result);
	echo $response;
}
$connection = null;

?>