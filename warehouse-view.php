<?php
	$picker =	array
				(
				['row' => 1, 'column' => 1, 'facing' => 'r'],
				['row' => 2, 'column' => 1, 'facing' => 'r'],
				['row' => 3, 'column' => 1, 'facing' => 'u'],
				['row' => 4, 'column' => 1, 'facing' => 'r']
				);
	$data = json_encode($picker);
	/*$picker = json_decode($data);
	echo $picker[0]->row;
	echo $picker[0]->column;
	echo $picker[0]->facing;
	echo "\n";
	echo $picker[2]->row;
	echo $picker[2]->column;
	echo $picker[2]->facing;
	echo "\n";*/
?>
