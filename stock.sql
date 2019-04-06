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
-- Table structure for table `bins`
--

DROP TABLE IF EXISTS `bins`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bins` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `row` int(11) DEFAULT NULL,
  `col` int(11) DEFAULT NULL,
  `dir` char(1) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=23 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `bins`
--

LOCK TABLES `bins` WRITE;
/*!40000 ALTER TABLE `bins` DISABLE KEYS */;
INSERT INTO `bins` VALUES (1,2,3,'l'),(2,3,3,'l'),(3,4,3,'l'),(4,2,4,'r'),(5,3,4,'r'),(6,4,4,'r'),(7,6,3,'l'),(8,7,3,'l'),(9,6,4,'r'),(10,7,4,'r'),(11,2,6,'u'),(12,3,6,'l'),(13,4,6,'l'),(14,5,6,'l'),(15,6,6,'l'),(16,7,6,'d'),(17,2,7,'u'),(18,3,7,'r'),(19,4,7,'r'),(20,5,7,'r'),(21,6,7,'r'),(22,7,7,'d');
/*!40000 ALTER TABLE `bins` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `items`
--

DROP TABLE IF EXISTS `items`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `items` (
  `bin` int(11) NOT NULL,
  `name` varchar(50) NOT NULL,
  `quantity` int(11) DEFAULT NULL,
  PRIMARY KEY (`bin`,`name`),
  KEY `fk_name` (`name`),
  CONSTRAINT `fk_bin` FOREIGN KEY (`bin`) REFERENCES `bins` (`id`),
  CONSTRAINT `fk_name` FOREIGN KEY (`name`) REFERENCES `products` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `items`
--

LOCK TABLES `items` WRITE;
/*!40000 ALTER TABLE `items` DISABLE KEYS */;
INSERT INTO `items` VALUES (1,'apple',2),(1,'milk',2),(2,'apple',1),(2,'plate',2),(3,'apple',4),(3,'milk',2),(4,'banana',5),(4,'cup',6),(5,'banana',2),(5,'plate',2),(6,'banana',1),(6,'fork',3),(7,'chair',4),(7,'fork',5),(8,'chair',6),(8,'milk',3),(9,'plate',3),(9,'table cloth',3),(10,'apple',4),(10,'banana',3),(10,'bread',3),(11,'beef',2),(11,'carrot',4),(12,'carrot',2),(12,'chicken',5),(13,'beef',5),(13,'milk',2),(14,'cup',5),(14,'table cloth',3),(15,'bread',2),(15,'grapes',5),(16,'carrot',3),(16,'table',3),(17,'carrot',1),(17,'table cloth',1),(18,'grapes',5),(19,'cup',2),(19,'table',2),(20,'bread',5),(20,'cup',3),(20,'knife',2),(21,'chair',1),(21,'knife',3),(22,'apple',1),(22,'knife',3),(22,'milk',2);
/*!40000 ALTER TABLE `items` ENABLE KEYS */;
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
  PRIMARY KEY (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `products`
--

LOCK TABLES `products` WRITE;
/*!40000 ALTER TABLE `products` DISABLE KEYS */;
INSERT INTO `products` VALUES ('apple',NULL,'food'),('banana',NULL,'food'),('beef',NULL,'food'),('bread',NULL,'food'),('carrot',NULL,'food'),('chair',NULL,'furniture'),('chicken',NULL,'food'),('cup',NULL,'symbel'),('fork',NULL,'symbel'),('grapes',NULL,'food'),('knife',NULL,'symbel'),('milk',NULL,'food'),('plate',NULL,'symbel'),('table',NULL,'furniture'),('table cloth',NULL,'symbel');
/*!40000 ALTER TABLE `products` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2019-04-06 11:00:39
