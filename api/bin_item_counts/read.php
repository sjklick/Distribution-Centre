<?php

include "../connection/connect.php";
$connection = connect();
if (isset($connection)) {
	$nItems = array();
	for ($bin=0; $bin<22; $bin++) {
		$query = "SELECT SUM(quantity) FROM stock_items WHERE bin_id=".$bin;
		foreach ($connection->query($query) as $row) {
			$currentCount = intval($row[0]);
			array_push($nItems, $currentCount);
		}
	}
	echo json_encode($nItems);
}
$connection = null;

?>
