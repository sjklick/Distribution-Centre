<?php

include "../connection/connect.php";
$connection = connect();
if (isset($connection)) {
	class Picker {
		public $row;
		public $column;
		public $facing;
		public $state;
	}
	$pickers = array();
	$query = "SELECT * FROM stock_bins";
	foreach ($connection->query($query) as $row) {
		$picker = new Picker;
		$picker->row = intval($row['curr_row']);
		$picker->column = intval($row['curr_col']);
		$picker->facing = $row['curr_dir'];
		$picker->state = $row['state'];
		array_push($pickers, $picker);
	}
	echo json_encode($pickers);
}
$connection = null;

?>
