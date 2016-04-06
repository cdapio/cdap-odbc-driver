=====================
Cask CDAP ODBC Driver
=====================

Introduction
============

ODBC driver for CDAP Datasets allows to access datasets via ODBC interface. Driver is compatible and tested
with the following applications:
 * Microsoft Excel (including MS Query)
 * Tableau.

Driver provides the following functionality:
 * Support for asynchronous mode
 * Connection pool
 * Support for CDAP Datasets schema data types: INT, LONG, FLOAT, DOUBLE, STRING, BOOLEAN, BYTES and MAP<STRING, STRING>; the latter is converted to STRING type
 * ODBC escaped sequences for function calls, date and timestamps.
Driver supports the following HSQL features see:
TODO: add link.

Known limitations
=================

 * ODBC Driver does not support interval, guid, datetime and procedure call escape sequences as they are not supported by Hive in CDAP.
 * 

Compatibility
============

Cask CDAP ODBC Driver is compatible with Windows 7, Windows 8/8.1 and Windows 10.


Installation
============

Install Cask CDAP ODBC Driver as you would normally install any driver. System reload after install in required.

Configuration
=============

For MS Excel: no additional configuration is required.
For Tableau:
TODO: add configuration file and provide instructions on where to put it

Troobleshooting
===============



