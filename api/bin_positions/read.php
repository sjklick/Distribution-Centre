<?php

include "../connection/connect.php";
$connection = connect();
if (isset($connection)) {
	class Position {
		public $row;
		public $column;
		public $facing;
	}
	$positions = array();
	$query = "SELECT * FROM stock_bins";
	foreach ($connection->query($query) as $row) {
		$position = new Position;
		$position->row = $row['row'];
		$position->column = intval($row['col']);
		$position->facing = intval($row['dir']);
		array_push($positions, $position);
	}
	echo json_encode($positions);
}
$connection = null;

?>
