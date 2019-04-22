<?php

function connect() {
	$connection = null;
	try {
		$configFile = fopen("../../config.txt", "r");
		fgets($configFile);
		$username = trim(fgets($configFile));
		$password = trim(fgets($configFile));
		fclose($configFile);
	} catch (Exception $exception) {
		return $connection;
	}
	try {
		$dsn = "mysql:host=localhost;dbname=stock";
		$connection = new PDO($dsn, $username, $password);
		$connection->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
	} catch (PDOexception $exception) {
		return $connection;
	}
	return $connection;
}

?>
