<?php
	$picker =	array
				(
				['row' => 1, 'column' => 1, 'facing' => 'r'],
				['row' => 2, 'column' => 1, 'facing' => 'r'],
				['row' => 3, 'column' => 1, 'facing' => 'u'],
				['row' => 4, 'column' => 1, 'facing' => 'r']
				);
	$data = json_encode($picker);
	echo $data;
?>
