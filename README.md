================
CDAP ODBC Driver
================

Introduction
============

CDAP ODBC Driver provides access to CDAP Datasets via ODBC interface.

CDAP ODBC Driver provides the following functionality:
 * Support for asynchronous mode;
 * Connection pool;
 * Support for CDAP Datasets schema data types: INT, LONG, FLOAT, DOUBLE, STRING, BOOLEAN, BYTES 
and MAP\<STRING, STRING\>; the latter is converted to STRING type;
 * ODBC escaped sequences for function calls, date and timestamps.


Compatibility
=============

Operating systems:
 * Windows 7
 * Windows 8/8.1
 * Windows 10

CDAP ODBC Driver was tested with the following applications:
 * Microsoft Excel 2013
 * Microsoft Excel 2016
 * Tableau 9.2 and higher
Compatibility with other applications is not guaranteed.


Build instructions
==================

This solution requires:
 * MS Visual Studio 2015 Update 1
 * WiX Toolset v3.10 or higher

Solution contains 5 projects:
 * CdapOdbc - core and startup project, implements ODBC interface; should be used for debugging;
 * CdapOdbc.Core - driver's functionality
 * CdapOdbc.setup - WiX installer;
 * CdapOdbc.Tests - functional tests;
 * CdapOdbc.UnitTests - unit tests;

Output path:
 * .DDL file: \<path-to-solution>\\\<architecture>\\\<configuration>
 * .MSI installer: \<path-to-solution\>\CdapOdbc.setup\\bin\\\<architecture>\\<solution>


Installation
============

Install CDAP ODBC Driver as you would normally install any driver. System reload after installation is required.


Configuration
=============

## Microsoft Excel
MS Excel requires no additional configuration.

## Tableau
Tableau requires no additional configuration.


Troubleshooting
===============

## "The specified DSN contains an architecture mismatch between the Driver and Application" error
This error occurs when a 32-bit application attempts to use a 64-bit driver. 64-bit and 32-bit ODBC drivers are managed by 64-bit and 32-bit ODBC Administrators respectively, but ODBC data sources are architecture-independent and can be accessed by any application. Some versions of MS Excel 64-bit use MS Query 32-bit, which causes an architecture mismatch.

Solution: install 32-bit version of CDAP ODBC Driver.
