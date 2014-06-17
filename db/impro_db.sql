-- phpMyAdmin SQL Dump
-- version 3.4.10.1deb1
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Jun 17, 2014 at 03:38 AM
-- Server version: 5.5.37
-- PHP Version: 5.3.10-1ubuntu3.11

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `impro_db`
--

-- --------------------------------------------------------

--
-- Table structure for table `quarter`
--

CREATE TABLE IF NOT EXISTS `quarter` (
  `ID` int(11) NOT NULL AUTO_INCREMENT COMMENT 'Unique ID',
  `pos` int(11) NOT NULL COMMENT 'Position inside the measure (1:first, 2:middle, 3:last)',
  `instrument` varchar(255) NOT NULL,
  `mode` enum('mj','min') NOT NULL COMMENT 'The mode of the scale used (mj or min)',
  `chord_grade` int(11) NOT NULL COMMENT 'Goes from 1 to 12 (1: tonal, 5: modal, 8: dominant)',
  `chord_mode` enum('mj','min','7','7+') NOT NULL COMMENT 'Mode of the chord (mj, min,7,7+)',
  `tag_dyna` varchar(255) NOT NULL,
  `tag_mood` varchar(255) NOT NULL,
  `tag_genre` varchar(255) NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 COMMENT='This table contains a quarter, which is storing 4 semiquivers.' AUTO_INCREMENT=10 ;

--
-- Dumping data for table `quarter`
--

INSERT INTO `quarter` (`ID`, `pos`, `instrument`, `mode`, `chord_grade`, `chord_mode`, `tag_dyna`, `tag_mood`, `tag_genre`) VALUES
(1, 1, 'prova', 'mj', 1, 'mj', 'groove', 'moderate', 'blues'),
(2, 2, 'prova', 'mj', 1, 'mj', 'groove', 'moderate', 'blues'),
(3, 2, 'prova', 'mj', 10, 'min', 'groove', 'moderate', 'blues'),
(4, 3, 'prova', 'mj', 10, 'min', 'groove', 'moderate', 'blues'),
(5, 1, 'prova', 'mj', 3, 'min', 'groove', 'moderate', 'blues'),
(6, 2, 'prova', 'mj', 3, 'min', 'groove', 'moderate', 'blues'),
(7, 2, 'prova', 'mj', 8, '7', 'groove', 'moderate', 'blues'),
(8, 3, 'prova', 'mj', 8, '7', 'groove', 'moderate', 'blues');

-- --------------------------------------------------------

--
-- Table structure for table `semiquiver`
--

CREATE TABLE IF NOT EXISTS `semiquiver` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `pos` int(11) NOT NULL COMMENT 'Goes from 1 to 4 (position in the quarter''s subdivision)',
  `quarter` int(11) NOT NULL COMMENT 'Reference to the quarter',
  `velocity_min` int(11) NOT NULL,
  `velocity_max` int(11) NOT NULL,
  `pC` float NOT NULL DEFAULT '0' COMMENT 'Probability to change note',
  `pC3qrt` float NOT NULL DEFAULT '0' COMMENT 'Probability for the beginning of a quarter triplet',
  `pC3qvr` float NOT NULL DEFAULT '0' COMMENT ' Probability for the beginning of a quiver triplet',
  `pC3smq` float NOT NULL DEFAULT '0' COMMENT 'Probability for the beginning of a semiquiver triplet',
  `pg1` float NOT NULL DEFAULT '0',
  `pg2` float NOT NULL DEFAULT '0',
  `pg3` float NOT NULL DEFAULT '0',
  `pg4` float NOT NULL DEFAULT '0',
  `pg5` float NOT NULL DEFAULT '0',
  `pg6` float NOT NULL DEFAULT '0',
  `pg7` float NOT NULL DEFAULT '0',
  `pg8` float NOT NULL DEFAULT '0',
  `pg9` float NOT NULL DEFAULT '0',
  `pg10` float NOT NULL DEFAULT '0',
  `pg11` float NOT NULL DEFAULT '0',
  `pg12` float NOT NULL DEFAULT '0',
  `pg13` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`),
  KEY `quarter` (`quarter`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 COMMENT='A semiquiver is the atom we use. Every 4/4 measure is composed by 16 semiquivers' AUTO_INCREMENT=35 ;

--
-- Dumping data for table `semiquiver`
--

INSERT INTO `semiquiver` (`ID`, `pos`, `quarter`, `velocity_min`, `velocity_max`, `pC`, `pC3qrt`, `pC3qvr`, `pC3smq`, `pg1`, `pg2`, `pg3`, `pg4`, `pg5`, `pg6`, `pg7`, `pg8`, `pg9`, `pg10`, `pg11`, `pg12`, `pg13`) VALUES
(1, 1, 1, 100, 200, 0.5, 0, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0.25, 0, 0, 0, 0, 0.25),
(2, 2, 1, 100, 200, 0.5, 0, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0.25, 0, 0, 0, 0, 0.25),
(3, 3, 1, 100, 200, 0.5, 0, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0.25, 0, 0, 0, 0, 0.25),
(4, 4, 1, 100, 200, 0.5, 0, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0.25, 0, 0, 0, 0, 0.25),
(7, 1, 2, 100, 200, 0.5, 0, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0.25, 0, 0, 0, 0, 0.25),
(8, 2, 2, 100, 200, 0.5, 0, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0.25, 0, 0, 0, 0, 0.25),
(9, 3, 2, 100, 200, 0.5, 0, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0.25, 0, 0, 0, 0, 0.25),
(10, 4, 2, 100, 200, 0.5, 0, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0.25, 0, 0, 0, 0, 0.25),
(11, 1, 3, 100, 200, 0.5, 0, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0, 0, 0.25, 0, 0, 0.25),
(12, 2, 3, 100, 200, 0.5, 0, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0, 0, 0.25, 0, 0, 0.25),
(13, 3, 3, 100, 200, 0.5, 0, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0, 0, 0.25, 0, 0, 0.25),
(14, 4, 3, 100, 200, 0.5, 0, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0, 0, 0.25, 0, 0, 0.25),
(15, 1, 4, 100, 200, 0.5, 0, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0, 0, 0.25, 0, 0, 0.25),
(16, 2, 4, 100, 200, 0.5, 0, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0, 0, 0.25, 0, 0, 0.25),
(17, 3, 4, 100, 200, 0.5, 0, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0, 0, 0.25, 0, 0, 0.25),
(18, 4, 4, 100, 200, 0.5, 0, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0, 0, 0.25, 0, 0, 0.25),
(19, 1, 5, 100, 200, 0.5, 0, 0, 0, 0, 0, 0.25, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0.25),
(20, 2, 5, 100, 200, 0.5, 0, 0, 0, 0, 0, 0.25, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0.25),
(21, 3, 5, 100, 200, 0.5, 0, 0, 0, 0, 0, 0.25, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0.25),
(22, 4, 5, 100, 200, 0.5, 0, 0, 0, 0, 0, 0.25, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0.25),
(23, 1, 6, 100, 200, 0.5, 0, 0, 0, 0, 0, 0.25, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0.25),
(24, 2, 6, 100, 200, 0.5, 0, 0, 0, 0, 0, 0.25, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0.25),
(25, 3, 6, 100, 200, 0.5, 0, 0, 0, 0, 0, 0.25, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0.25),
(26, 4, 6, 100, 200, 0.5, 0, 0, 0, 0, 0, 0.25, 0, 0, 0.25, 0, 0, 0, 0.25, 0, 0, 0.25),
(27, 1, 7, 100, 200, 0.5, 0, 0.1, 0, 0, 0, 0.25, 0, 0, 0.15, 0, 0.25, 0, 0, 0, 0.25, 0.1),
(28, 2, 7, 100, 200, 0.5, 0, 0.1, 0, 0, 0, 0.25, 0, 0, 0.15, 0, 0.25, 0, 0, 0, 0.25, 0.1),
(29, 3, 7, 100, 200, 0.5, 0, 0.1, 0, 0, 0, 0.25, 0, 0, 0.15, 0, 0.25, 0, 0, 0, 0.25, 0.1),
(30, 4, 7, 100, 200, 0.5, 0, 0.1, 0, 0, 0, 0.25, 0, 0, 0.15, 0, 0.25, 0, 0, 0, 0.25, 0.1),
(31, 1, 8, 100, 200, 0.5, 0, 0.1, 0, 0, 0, 0.25, 0, 0, 0.15, 0, 0.25, 0, 0, 0, 0.25, 0.1),
(32, 2, 8, 100, 200, 0.5, 0, 0.1, 0, 0, 0, 0.25, 0, 0, 0.15, 0, 0.25, 0, 0, 0, 0.25, 0.1),
(33, 3, 8, 100, 200, 0.5, 0, 0.1, 0, 0, 0, 0.25, 0, 0, 0.15, 0, 0.25, 0, 0, 0, 0.25, 0.1),
(34, 4, 8, 100, 200, 0.5, 0, 0.1, 0, 0, 0, 0.25, 0, 0, 0.15, 0, 0.25, 0, 0, 0, 0.25, 0.1);

--
-- Constraints for dumped tables
--

--
-- Constraints for table `semiquiver`
--
ALTER TABLE `semiquiver`
  ADD CONSTRAINT `semiquiver_ibfk_1` FOREIGN KEY (`quarter`) REFERENCES `quarter` (`ID`);

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
