

Source Code Examples from "System Design with SystemC"
Release Date: 27 June 2002


This source code is from the book "System Design with SystemC".
For detailed discussion on these examples, see the relevant sections
within the "System Design with SystemC" book.

To obtain the book and find additional source code downloads, etc., visit

   www.systemc.org 

Look in the "Products & Solutions" -> "SystemC Books". Then look at the
entry for "System Design with SystemC".

If you are using Microsoft Visual C++, see the Visual C++ section
at the end of this document.

These examples only work with SystemC 2.0 or later releases. See
www.systemc.org to download the latest SystemC releases. 


UNIX/Linux Instructions
-----------------------

These examples are designed to be extracted and installed on top
of an existing SystemC installation. (This is the easiest way,
though it is not a hard requirement.) To do this, locate the top level
directory of your SystemC installation. It should contain the INSTALL
file and the "examples" and "msvc60" directories. Extract the the
contents of this examples archive so that the "examples" and "msvc60"
directories within the archive overlay the corresponding directories
in your SystemC installation.

Build your SystemC installation according to the instructions in
the INSTALL file if you haven't already done so.

Now identify the name of your SystemC library in one of the "lib-*" directories
at the top of the SystemC installation. (e.g. lib-gccsparcOS5/libsystemc.a).

Next, change directory to the "examples/system_design_with_systemc" directory.

Locate the "mk_*" file that corresponds to your SystemC library
(e.g. mk_gccsparcOS5). If the appropriate file does not exist, 
create one by copying and modifying one of the existing files.

To build all of the examples, simply enter the name of your "mk_*"
script on the command line with no arguments. To build a specific
set of examples, specify the names of the example directories
as command line arguments to the appropriate "mk_*" script.

To run a specific example, type "dirname/run.x" where "dirname"
is the example name. You can also run under control of a debugger
for example by typing "gdb dirname/run.x".


Microsoft Visual C++ Instructions
---------------------------------

These examples have been tested with Microsoft Visual C++ 6.0 SP5
and the SystemC 2.0.1 release.

These examples are designed to be extracted and installed on top
of an existing SystemC installation. (This is by far the easiest way,
though it is not a hard requirement.) To do this, locate the top level
directory of your SystemC installation. It should contain the INSTALL
file and the "examples" and "msvc60" directories. Extract the the
contents of this examples archive so that the "examples" and "msvc60"
directories within the archive overlay the corresponding directories
in your SystemC installation.

The VisualC++ "project" files for these examples are designed to work with
the DEBUG version of the systemc library. You must build (or rebuild)
the DEBUG version of your SystemC library before building the examples.
To do so, double click on the "msvc60/systemc/systemc.dsw" file to
launch Visual C++. Click on the "Build->Set Active Configuration" menu.
Click on the "SystemC Win32 Debug" configuration and then click OK. Then
click on the "Build->Rebuild All" menu.

Now you are ready to launch VisualC++ on one of the examples. Choose
one of the examples in the "msvc60/system_design_with_systemc" directory
and double click on the ".dsw" workspace file within it to launch VisualC++.
Then click on the "Build->Rebuild All" menu to build the example.

Before running the example, we recommend setting a breakpoint at
the last line within the sc_main() function. This will allow you to
see the program output within the popup console window without having
the popup console window disappear immediately when the program completes.


