<html>
	<head>
		<title>Warehouse-View</title>
		<link rel="stylesheet" href="warehouse-view.css"/>
	</head>
	<body>
		<?php echo "<p>Unable to reach warehouse.</p>"; ?>
		<?php echo "<div class='warehouse'>"; ?>
		<?php
			$warehouse =	array
							(
							array('X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'),
							array('X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'),
							array('X', '.', '.', 'B', 'B', '.', 'B', 'B', '.', 'X'),
							array('X', '.', '.', 'B', 'B', '.', 'B', 'B', '.', 'X'),
							array('X', '.', '.', 'B', 'B', '.', 'B', 'B', '.', 'X'),
							array('X', '.', '.', '.', '.', '.', 'B', 'B', '.', 'X'),
							array('X', '.', '.', 'B', 'B', '.', 'B', 'B', '.', 'X'),
							array('X', '.', '.', 'B', 'B', '.', 'B', 'B', '.', 'X'),
							array('X', '.', '.', '.', '.', '.', '.', '.', '.', 'X'),
							array('X', 'S', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X')
							);
			array_push($warehouse); 
			for ($row=0; $row<10; $row++) {
				for ($column=0; $column<10; $column++) {
					if ($warehouse[$row][$column] == '.') {
						echo "<div class='tile floor'></div>";
					} elseif ($warehouse[$row][$column] == 'X') {
						echo "<div class='tile wall'></div>";
					} elseif ($warehouse[$row][$column] == 'B') {
						echo "<div class='tile bin'></div>";
					} elseif ($warehouse[$row][$column] == 'S') {
						echo "<div class='tile shipping'></div>";
					} else {
						echo "<div class='tile unknown'>?</div>";
					}
				}
			}
		?>
		<?php echo "</div>"; ?>
	</body>
</html>
