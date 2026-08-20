/*!40101 SET NAMES utf8 */;

CREATE USER openphysio IDENTIFIED BY 'op-pass';
CREATE DATABASE openphysio CHARACTER SET 'utf8';
USE openphysio;
GRANT ALL ON *.* TO openphysio;
