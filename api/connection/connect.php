<?php

function connect() {
	try {
		$configFile = fopen("../../config.txt", "r");
		fgets($configFile);
		$username = trim(fgets($configFile));
		$password = trim(fgets($configFile));
		fclose($configFile);
	} catch (Exception $exception) {
		$connection = null;
		return $connection;
	}
	try {
		$dsn = "mysql:host=localhost;dbname=model";
		$connection = new PDO($dsn, $username, $password);
		$connection->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
	} catch (PDOexception $exception) {
		$connection = null;
		return $connection;
	}
	return $connection;
}

?>
