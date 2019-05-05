<?php

include "../connection/connect.php";
$connection = connect();
if (isset($connection)) {
	for ($bin=0; $bin<22; $bin++) {
		$query = "SELECT * FROM orders LIMIT 1";
		foreach ($connection->query($query) as $row) {
			$current = intval($row[0]);
		}
	}
	echo json_encode($current);
}
$connection = null;

?>
