# Building CDAP ODBC Driver
## Prerequisites
- Visual Studio 2015
## Build from source
There are two ways you can build on Windows, from the command line or from Visual Studio.
### Visual Studio solution files
The easiest way to build is by using the Visual Studio solution file. 
It contains the driver code and tests.  
The Visual Studio solution files are located in the top of the repository. To build simply open the solution file you need and away you go!
### Command line
The more complex and complete way to build can be performed from the command line. 
#### Build solution
First go to Start menu and open one of Visual Studio 2015 command lines depending on platform x86/x64. After that navigate to project root folder. Run the following:
```
>MSBuild
```
#### Run the tests 
To run the test execute the following:
```
>VSTest.Console.exe Debug/CdapOdbc.Tests.dll
```
or for x64:
```
>VSTest.Console.exe x64/Debug/CdapOdbc.Tests.dll /platform:x64
```