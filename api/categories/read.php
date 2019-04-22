<?php

include "../connection/connect.php";
$connection = connect();
if ($connection != null) {
	$categories = array();
	$query = "SELECT * FROM categories";
	foreach ($connection->query($query) as $row) {
		array_push($categories, $row['category']);
	}
	echo json_encode($categories);
}
$connection = null;

?>
