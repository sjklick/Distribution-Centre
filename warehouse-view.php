<html>
	<head>
		<title>Warehouse-View</title>
		<link rel="stylesheet" href="warehouse-view.css"/>
	</head>
	<body>
		<?php echo "<p>Unable to reach warehouse.</p>"; ?>
		<?php echo "<div class='warehouse'>"; ?>
		<?php
			for ($row=0; $row<10; $row++) {
				for ($column=0; $column<10; $column++) {
					echo "<div class='tile floor'></div>";
				}
			}
		?>
		<?php echo "</div>"; ?>
	</body>
</html>
