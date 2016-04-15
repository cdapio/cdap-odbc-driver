# CDAP ODBC Driver build instructions


## Prerequisites

 * MS Visual Studio 2015 Update 1
 * WiX Toolset v3.10 or higher

## Solution description

Solution contains 5 projects:
 * **CdapOdbc** - core and startup project, implements ODBC interface; should be used for debugging;
    * Note: this project contains VERSION resource that populates .dll file and product installer with version number  
 * **CdapOdbc.Core** - driver's functionality
 * **CdapOdbc.setup** - WiX installer;
 * **CdapOdbc.Tests** - functional tests;
 * **CdapOdbc.UnitTests** - unit tests;

Additional project **CdapOdbc.TestsManual** is not included into the solution. It contains test data files,
CLI scripts and Excel sheet for manually testing Driver with Excel and Tableau. 

Build output path for .DLL file:
```
<path-to-solution>\<architecture>\<configuration>
```
Build output path for .MSI installer:
```
<path-to-solution>\CdapOdbc.setup\bin\<architecture>\<configuration>\en-us
```
Localization is used to redefine standard WiX messages by adding custom text style.

## Build instructions
There are two ways you can build on Windows, from the command line or from Visual Studio.

### Build using Visual Studio 
The easiest way to build is by using the Visual Studio solution file. Open CdapOdbc140.sln file 
located in the root of repository and select Build --> Build Solution from the toolbar.

### Build using the command line
Alternatively, solution can be built from the command line:
* Go to Start menu and open one of Visual Studio 2015 command lines depending on platform x86/x64.
* Navigate to project root folder.
* Run the following command:
```
> MSBuild
```

## Running tests 
To run tests execute the following:
```
>VSTest.Console.exe Debug/CdapOdbc.Tests.dll
```
or for x64:
```
>VSTest.Console.exe x64/Debug/CdapOdbc.Tests.dll /platform:x64
```
