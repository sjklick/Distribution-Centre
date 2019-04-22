<?php
$configFile = fopen("../../config.txt", "r") or die();
fgets($configFile);
$username = trim(fgets($configFile));
$password = trim(fgets($configFile));
fclose($configFile);
try {
	$dsn = "mysql:host=localhost;dbname=stock";
	$connection = new PDO($dsn, $username, $password);
	$connection->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOexception $exception) {
	echo $exception->getMessage() . "\n";
	die();
}
$categories = array();
$query = "SELECT * FROM categories";
foreach ($connection->query($query) as $row) {
	array_push($categories, $row['category']);
}
$connection = null;
echo json_encode($categories);
?>
