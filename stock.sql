-- MySQL dump 10.16  Distrib 10.1.38-MariaDB, for Linux (x86_64)
--
-- Host: localhost    Database: stock
-- ------------------------------------------------------
-- Server version	10.1.38-MariaDB

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `categories`
--

DROP TABLE IF EXISTS `categories`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `categories` (
  `category` varchar(15) NOT NULL,
  PRIMARY KEY (`category`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `categories`
--

LOCK TABLES `categories` WRITE;
/*!40000 ALTER TABLE `categories` DISABLE KEYS */;
INSERT INTO `categories` VALUES ('food'),('furniture'),('symbel');
/*!40000 ALTER TABLE `categories` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `order_items`
--

DROP TABLE IF EXISTS `order_items`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `order_items` (
  `order_id` int(11) NOT NULL,
  `name` varchar(20) NOT NULL,
  `quantity` int(11) DEFAULT NULL,
  PRIMARY KEY (`order_id`,`name`),
  KEY `name` (`name`),
  CONSTRAINT `order_items_ibfk_1` FOREIGN KEY (`order_id`) REFERENCES `orders` (`order_id`),
  CONSTRAINT `order_items_ibfk_2` FOREIGN KEY (`name`) REFERENCES `products` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `order_items`
--

LOCK TABLES `order_items` WRITE;
/*!40000 ALTER TABLE `order_items` DISABLE KEYS */;
INSERT INTO `order_items` VALUES (3,'banana',2),(3,'beef',1),(4,'chair',1),(4,'cup',1),(4,'fork',1),(4,'knife',1),(4,'plate',1),(4,'table',1),(4,'table cloth',1),(5,'carrot',2),(5,'grapes',2),(5,'milk',1);
/*!40000 ALTER TABLE `order_items` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER order_item_insert AFTER INSERT ON order_items FOR EACH ROW UPDATE products SET quantity=quantity-NEW.quantity WHERE name=NEW.name */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `orders`
--

DROP TABLE IF EXISTS `orders`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `orders` (
  `order_id` int(11) NOT NULL AUTO_INCREMENT,
  `customer` varchar(50) DEFAULT NULL,
  `email` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`order_id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `orders`
--

LOCK TABLES `orders` WRITE;
/*!40000 ALTER TABLE `orders` DISABLE KEYS */;
INSERT INTO `orders` VALUES (3,'Test_1',NULL),(4,'Test_2',NULL),(5,'Test_3',NULL);
/*!40000 ALTER TABLE `orders` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `pickers`
--

DROP TABLE IF EXISTS `pickers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pickers` (
  `picker_id` int(11) NOT NULL AUTO_INCREMENT,
  `home_row` int(11) NOT NULL,
  `home_col` int(11) NOT NULL,
  `home_dir` char(1) NOT NULL,
  `curr_row` int(11) NOT NULL,
  `curr_col` int(11) NOT NULL,
  `curr_dir` char(1) NOT NULL,
  PRIMARY KEY (`picker_id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `pickers`
--

LOCK TABLES `pickers` WRITE;
/*!40000 ALTER TABLE `pickers` DISABLE KEYS */;
INSERT INTO `pickers` VALUES (1,1,1,'r',1,1,'r'),(2,2,1,'r',2,1,'r'),(3,3,1,'r',3,1,'r'),(4,4,1,'r',4,1,'r');
/*!40000 ALTER TABLE `pickers` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `products`
--

DROP TABLE IF EXISTS `products`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `products` (
  `name` varchar(50) NOT NULL,
  `description` varchar(255) DEFAULT NULL,
  `category` varchar(15) DEFAULT NULL,
  `quantity` int(11) DEFAULT NULL,
  PRIMARY KEY (`name`),
  KEY `category` (`category`),
  CONSTRAINT `products_ibfk_1` FOREIGN KEY (`category`) REFERENCES `categories` (`category`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `products`
--

LOCK TABLES `products` WRITE;
/*!40000 ALTER TABLE `products` DISABLE KEYS */;
INSERT INTO `products` VALUES ('apple',NULL,'food',2),('banana',NULL,'food',1),('beef',NULL,'food',2),('bread',NULL,'food',4),('carrot',NULL,'food',0),('chair',NULL,'furniture',1),('chicken',NULL,'food',5),('cup',NULL,'symbel',1),('fork',NULL,'symbel',5),('grapes',NULL,'food',2),('knife',NULL,'symbel',1),('milk',NULL,'food',2),('plate',NULL,'symbel',3),('table',NULL,'furniture',0),('table cloth',NULL,'symbel',3);
/*!40000 ALTER TABLE `products` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `receiving_items`
--

DROP TABLE IF EXISTS `receiving_items`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `receiving_items` (
  `name` varchar(50) NOT NULL,
  PRIMARY KEY (`name`),
  CONSTRAINT `receiving_items_ibfk_1` FOREIGN KEY (`name`) REFERENCES `products` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `receiving_items`
--

LOCK TABLES `receiving_items` WRITE;
/*!40000 ALTER TABLE `receiving_items` DISABLE KEYS */;
/*!40000 ALTER TABLE `receiving_items` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `shipping_items`
--

DROP TABLE IF EXISTS `shipping_items`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `shipping_items` (
  `name` varchar(50) NOT NULL,
  `quantity` int(11) DEFAULT NULL,
  `needed` int(11) DEFAULT NULL,
  PRIMARY KEY (`name`),
  CONSTRAINT `shipping_items_ibfk_1` FOREIGN KEY (`name`) REFERENCES `products` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `shipping_items`
--

LOCK TABLES `shipping_items` WRITE;
/*!40000 ALTER TABLE `shipping_items` DISABLE KEYS */;
/*!40000 ALTER TABLE `shipping_items` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `stock_bins`
--

DROP TABLE IF EXISTS `stock_bins`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `stock_bins` (
  `bin_id` int(11) NOT NULL AUTO_INCREMENT,
  `row` int(11) DEFAULT NULL,
  `col` int(11) DEFAULT NULL,
  `dir` char(1) DEFAULT NULL,
  PRIMARY KEY (`bin_id`)
) ENGINE=InnoDB AUTO_INCREMENT=23 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `stock_bins`
--

LOCK TABLES `stock_bins` WRITE;
/*!40000 ALTER TABLE `stock_bins` DISABLE KEYS */;
INSERT INTO `stock_bins` VALUES (1,2,3,'l'),(2,3,3,'l'),(3,4,3,'l'),(4,2,4,'r'),(5,3,4,'r'),(6,4,4,'r'),(7,6,3,'l'),(8,7,3,'l'),(9,6,4,'r'),(10,7,4,'r'),(11,2,6,'u'),(12,3,6,'l'),(13,4,6,'l'),(14,5,6,'l'),(15,6,6,'l'),(16,7,6,'d'),(17,2,7,'u'),(18,3,7,'r'),(19,4,7,'r'),(20,5,7,'r'),(21,6,7,'r'),(22,7,7,'d');
/*!40000 ALTER TABLE `stock_bins` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `stock_items`
--

DROP TABLE IF EXISTS `stock_items`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `stock_items` (
  `bin_id` int(11) NOT NULL,
  `name` varchar(50) NOT NULL,
  `quantity` int(11) DEFAULT NULL,
  PRIMARY KEY (`bin_id`,`name`),
  KEY `fk_name` (`name`),
  CONSTRAINT `fk_bin` FOREIGN KEY (`bin_id`) REFERENCES `stock_bins` (`bin_id`),
  CONSTRAINT `fk_name` FOREIGN KEY (`name`) REFERENCES `products` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `stock_items`
--

LOCK TABLES `stock_items` WRITE;
/*!40000 ALTER TABLE `stock_items` DISABLE KEYS */;
INSERT INTO `stock_items` VALUES (1,'apple',2),(2,'banana',3),(3,'beef',3),(4,'bread',4),(5,'carrot',2),(6,'chair',2),(7,'chicken',5),(8,'cup',2),(9,'fork',6),(10,'grapes',4),(11,'knife',2),(12,'milk',3),(13,'plate',4),(14,'table',1),(15,'table cloth',4);
/*!40000 ALTER TABLE `stock_items` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER stock_item_insert
AFTER INSERT
ON stock_items
FOR EACH ROW
UPDATE products SET quantity=quantity+NEW.quantity WHERE name=NEW.name */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER stock_item_update
AFTER UPDATE
ON stock_items
FOR EACH ROW
UPDATE products SET quantity=IF(NEW.quantity>OLD.quantity, quantity+(NEW.quantity-OLD.quantity), quantity) WHERE name=OLD.name */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2019-05-01 11:28:21
