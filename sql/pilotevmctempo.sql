-- phpMyAdmin SQL Dump
-- version 4.8.4
-- https://www.phpmyadmin.net/
--
-- Hôte : 127.0.0.1:3306
-- Généré le :  Dim 18 août 2019 à 07:37
-- Version du serveur :  5.7.24
-- Version de PHP :  7.2.14

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Base de données :  `id3758222_pilotevmc`
--

-- --------------------------------------------------------

--
-- Structure de la table `pilotevmctempo`
--

DROP TABLE IF EXISTS `pilotevmctempo`;
CREATE TABLE IF NOT EXISTS `pilotevmctempo` (
  `DATE` int(10) UNSIGNED DEFAULT NULL,
  `PTEC` varchar(4) NOT NULL,
  `BBRHCJB` decimal(9,0) UNSIGNED DEFAULT NULL,
  `BBRHPJB` decimal(9,0) UNSIGNED DEFAULT '0',
  `BBRHCJW` decimal(9,0) UNSIGNED DEFAULT '0',
  `BBRHPJW` decimal(9,0) UNSIGNED DEFAULT '0',
  `BBRHCJR` decimal(9,0) UNSIGNED DEFAULT '0',
  `BBRHPJR` decimal(9,0) UNSIGNED DEFAULT '0',
  `IINST1` decimal(3,0) UNSIGNED DEFAULT NULL,
  `dureeMax` smallint(5) UNSIGNED DEFAULT NULL,
  `etatWifi` tinyint(1) DEFAULT NULL,
  `tempExt` decimal(3,0) DEFAULT NULL,
  `tempCuis` decimal(3,0) DEFAULT NULL,
  `tempSdb` decimal(3,0) DEFAULT NULL,
  `humCuis` tinyint(4) UNSIGNED DEFAULT NULL,
  `humSdb` tinyint(4) UNSIGNED DEFAULT NULL,
  `mode` tinyint(1) DEFAULT NULL,
  `marche` tinyint(1) DEFAULT NULL,
  `vitesse` tinyint(1) DEFAULT NULL,
  `annotations` varchar(16) DEFAULT NULL,
  `ISOUSC` decimal(2,0) UNSIGNED NOT NULL DEFAULT '45',
  `OPTARIF` varchar(4) NOT NULL DEFAULT 'BBR',
  `DEMAIN` varchar(4) NOT NULL DEFAULT '----',
  `PAPP` decimal(5,0) UNSIGNED DEFAULT NULL,
  `HCHP` tinyint(1) NOT NULL,
  `HCHC` tinyint(1) NOT NULL,
  UNIQUE KEY `DATE` (`DATE`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
