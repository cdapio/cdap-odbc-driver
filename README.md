# CDAP ODBC Driver

## Introduction

CDAP ODBC Driver provides access to CDAP Datasets via ODBC interface.

CDAP ODBC Driver provides the following functionality:
 * Support for asynchronous mode;
 * Connection pool;
 * Support for CDAP Datasets schema data types: *INT*, *LONG*, *FLOAT*, *DOUBLE*, *STRING*, *BOOLEAN*, *BYTES*, 
and *MAP\<STRING, STRING\>*; the latter is converted to *STRING* type;
 * ODBC escaped sequences for function calls, date and timestamps.


## Compatibility

Operating systems:
 * Windows 7
 * Windows 8/8.1
 * Windows 10

CDAP ODBC Driver was tested with the following applications:
 * Microsoft Excel 2013
 * Microsoft Excel 2016
 * Tableau 9.2 and higher

Compatibility with other applications is not guaranteed.

## Installation

Install CDAP ODBC Driver as you would normally install any driver. System reload after installation is required.
ODBC data source with name "CDAP Datasets 32-bit" or "CDAP Datasets 64-bit" - depending on driver's version - will be created during the installation.

## Configuration

CDAP ODBC Driver requires no specific configuration.

## Troubleshooting

### "The specified DSN contains an architecture mismatch between the Driver and Application" error
This error occurs when a 32-bit application installed in 64-bit OS attempts to use a 64-bit driver. In 64-bit Windows 64-bit and 32-bit ODBC drivers are managed by 64-bit and 32-bit ODBC Administrators respectively, but ODBC data sources are architecture-independent and can be accessed by any application. Some versions of MS Excel 64-bit use MS Query 32-bit, which causes an architecture mismatch.

**Solution:** install 32-bit version of CDAP ODBC Driver.
