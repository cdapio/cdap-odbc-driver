#Functional Tests Configuration Guide
##Introduction
Functional tests are in CdapOdbc.Tests project. They use Microsoft Unit Testing Framework for C++.
Tests use 2 CDAP Explore services with specific schemas described below. 
##Configuration
Connection strings to two test CDAP Explore services configured in file stdafx.h by macro definitions:
```
#define TEST_CONNECTION_STRING L"Driver=Cask CDAP ODBC Driver;Host=104.154.22.63"
#define TEST_CONNECTION_STRING2 L"Driver=Cask CDAP ODBC Driver;Host=localhost"
```
The first Explore service must have specific database schema. The second one doesn't require any schema only valid connection. Both services must be with anonymous authentication.
##Database Schema
Table stream_contacts
| Column # | Type         |
| --------:| ------------ |
| 1        | VARCHAR(128) |
| 3        | DOUBLE       |
| 4        | VARCHAR(128) |

Data may be any. Other columns may be any. Column names don't matter.