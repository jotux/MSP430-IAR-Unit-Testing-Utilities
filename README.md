## Introduction

This repo contains a few simple macros, scripts, and other utilities to help setup a complete unit testing framework for your MSP430 project using the IAR compiler. Note: This **ONLY** works for IAR as we are running the unit tests against the build-in IAR simulator.

To open the example project open **/example/project/demo_workspace.eww** with IAR.

## Dependencies

 - IAR 5.40.x (compiler and simulator)
 - Python 2.7.x (build scripts and parsing)

## Currently supported devices

Currently this project runs against the MSP430FR5739 but the concept can be used to test code for any MSP430 device (or possibly any device with an IAR compiler and simulator).

## Setting up your test project

### No Path in __FILE__ macros

In **project->options->C/C++ compiler->Extra Options** select **Use command line options** and paste **--no_path\_in\_file\_macros**. This will prevent the entire path from being printed in the unit tests later.

### Custom Linker Config File

By default the linker config file for the simulator will configure the simulator to have the same amount of RAM and FLASH memory as the real device. If we were using the simulator to run the real code this would be useful but for unit tests there is no reason to have an arbitrary memory limitation. To get around this we're going to copy the linker config file, edit it, and tell the linker to use that file instead of the default for the test project.

Copy the **lnk430fr5739.xcl** file from the IAR directory (/IAR Systems/Embedded Workbench 6.0/430/config) to your test project and rename it **test_lnk430fr5739.xcl**. Find this line in the file:

    -P(CODE)CODE=C200-FF7F

and add a range to it (giving yourself more code space):

    -P(CODE)CODE=C200-FF7F,10000-FFFFF

Now in **project->options->Linker->Config** select **Override default** and browse for your file. If your file is in your test project directory you can use a relative path to it:

    $PROJ_DIR$\test_lnk430fr5739.xcl

### Custom Device Descriptor File

In **project->options->Debugger** make sure the **Driver** is set to **Simulator**.

Since we've added memory in the linker config we're going to have to edit the device descriptor file too, which basically maps the memory and peripherals for the debugger so it knows where everything is.

Just like the linker file, copy the **msp430fr5739.ddf** file from the IAR directory (/IAR Systems/Embedded Workbench 6.0/430/config) to your test project and rename it **test_msp430fr5739.ddf**. Find this line in the file:

    Memory2  = FRAM             Memory      0xC200      0xFFFF    R

and add memory to the end

    Memory2  = FRAM             Memory      0xC200      0xFFFFF    R

Now in **project->options->Debugger** under **Device description file** select **Override default** and browse for your file. If your file is in your test project directory you can use a relative path to it:

    $PROJ_DIR$\test_msp430fr5739.ddf

Now that your device descriptor is in your project directory if you try to debug it will complain the sfr config file is missing. To get rid of these errors copy the **msp430fr5739.sfr** file from the IAR directory (/IAR Systems/Embedded Workbench 6.0/430/config) to your test project directory.

### Add the Cycle Counter Macro

In the IAR simulator there are *psuedo*-registers that allow you to do a few different things. One of them is **CYCLECOUNTER** which continuously counts the number of cycles executing in the simulator. These registers are not normally accessible in user-space (your simulator code), but there are workarounds to read the cycle count in a simulator project.

The **cycle_counter.mac** file is an IAR simulator macro file that pulls the **CYCLECOUNTER** value out of the simulator and writes it to a global variable. You can read more about IAR macros in the IAR compiler reference if you're interested.

To use this file load it via **project->options->Debugger** in the **Setup macros** section select **Use macro file** and browse for your file. If your file is in your test project directory you can use a relative path to it:

    $PROJ_DIR$\cycle_counter.mac

Now that the macro is loaded the approximate run time of every test can be output and viewed in the final test results.

## Building your test project from the command line

To build your project make sure the IAR tools directory (/IAR Systems/Embedded Workbench 6.0/common/bin) is in your PATH. First we clean the project with:

    IarBuild.exe .\test\canary.ewp -clean Debug

(Note: my test project is named canary)

To build your project invoke:

    IarBuild.exe .\test\canary.ewp -make Debug

To make this easier I made a simple batch file, **_build.bat**, which cleans and builds your test project. You'll notice the script **_parse_build.py** called in **_build.bat** -- because I'm using a continuous integration server (jenkins) to check build status and because IARbuild.exe doesn't return non-zero on build failure, this script was added. In **_build.bat** the output of the build is spit out to _build.txt, and the python script checks the output to make sure the build was successful. After checking the output the python script exits with a useful code to tell the CI builder if it was good or not.

## Running unit tests

(Note: I'm not going to go into writing unit tests or the **test.h** file, which you should be able to do on your own. See the example project in this repo for a sample of writing unit tests against code)

To invoke the debugger from the command line we need to call the **canary.cspy.bat** file in the test project's **settings** directory and pass it the hex file generated from the build:

    .\settings\canary.cspy.bat .\Debug\Exe\canary.d43

If your project is working you'll see the debug output in your console.

This works fine on your local machine but it will cause a problem if you try to do it on a CI server.  The **canary.cspy.bat** file is autogenerated when the debugger is launched from the IAR IDE, so the server will not see the file without launching the IDE. Also, within the **canary.cspy.bat** file it expands the relative paths from **$PROJ_DIR$** to the absolute path -- so when your CI server clones the code to build it the paths will be wrong. To get around this we're going to copy the generated **canary.cspy.bat** file up to the root of the test project directory and edit it. Once the file has been copied to the root of your /test directory open it and find the text:

    --macro "/path/to/your\cycle_counter.mac"

replace the path with a relative path:

    --macro .\test\cycle_counter.mac

Now you can run the debugger from any machine where you have the compiler installed.

(Note: If you change debug parameters you'll need to repeat this process, as the new settings in debug will be in the autogenerated cspy file instead of the one you copied.)

To simplify this process you'll see a batch file in the project called **_test.bat** along with a python script **_parse_test.py**. The **_test.bat** script will invoke the debugger and output the results to **_test.txt**. It then called **_parse_test.py** which converts the custom output of the debugger to junit-compatible results (this could be done in the test project code itself, I just wanted to keep the test console output human-readable) and outputs them to **_test.xml**.
