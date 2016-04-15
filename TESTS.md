#CDAP ODBC Driver Tests Guide
##Introduction
Driver has automated functional tests. It also has manual test sheet to test integration with Excel.
##Functional Tests Configuration Guide
Functional tests are located in CdapOdbc.Tests project. They use Microsoft Unit Testing Framework for C++.
###Configuration
Testing required two pre-configured instances of CDAP with specific schemas as described below. 
Connection strings to the respective CDAP Explore services are configured in file **stdafx.h** by macro definitions:
```
#define TEST_CONNECTION_STRING L"Driver=Cask CDAP ODBC Driver;Host=104.154.22.63"
#define TEST_CONNECTION_STRING2 L"Driver=Cask CDAP ODBC Driver;Host=localhost"
```
The first Explore service must have specific database schema. The second one doesn't require any schema - only a valid connection. Both services must be with anonymous authentication.
###Database schema
Table **stream_contacts** must have at least 4 columns:

| Column # | Type         |
|---------:|--------------|
| 1        | VARCHAR(128) |
| 3        | DOUBLE       |
| 4        | VARCHAR(128) |

Data may be any. Other columns may be any. Column names don't matter.

###Running tests
**From Visual Studio UI:**
 - Open solution
 - Select target architecture: **Test**-->**Test settings**-->**Default processor architecture**
 - Run or debug tests: **Test**-->**Run** or **Test**-->**Debug**

**From command line:**

To run tests execute the following command from Visual Studio target-specific Command Prompt (see BUILD.md):
```
>VSTest.Console.exe Debug/CdapOdbc.Tests.dll
```
or for x64:
```
>VSTest.Console.exe x64/Debug/CdapOdbc.Tests.dll /platform:x64
```

##Manual tests - Integration with Excel

Manual tests are located in folder CdapOdbc.TestsManual. These tests are intended to verify all CDAP Datasets data types compatibility with MS Excel.

###Configuration

Initially all tests were based on CDAP cluster located at 104.154.22.63. Connection files are set up for that cluster. However you can use any other cluster. Only pre-configure the testing environment as follows:
 - Copy all .csv and .sh files from CdapOdbc.TestsManual into local folder.
 - Copy test-connections folder to the same local folder.
 - Run **test-setup.sh** script from CDAP CLI to create streams and datasets and load data.
 - Put **TestExcel.xslx** file and **test-connections** folder into one folder.
 - Open TestExcel.xlsx and select **Refresh All**.
 - Confirm connection parameters and make sure all tests pass. 
