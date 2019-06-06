<?php

include "../../connection/connect.php";
$connection = connect();
if (isset($connection)) {
	$categories = array();
	array_push($categories, "all");
	$query = "SELECT * FROM categories";
	foreach ($connection->query($query) as $row) {
		array_push($categories, $row['category']);
	}
	echo json_encode($categories);
}
$connection = null;

?>
