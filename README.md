# IEM_Mesytec_Unpacker
Daniel Fernandez RUiz

## Introduction 

The IEM_Mesytec_Unpacker is a code developed to translate data stored in the Mesytec Data format to the RootCERN data format.
If the code is working correctly the result should be a TTree organized by module number where each branch preserves the name of the model given in the MVME
The code is meant to minimize the input from the user and be as self-contained as possible.
The current version still can be upgraded for efficiency and flexibility. I did this unpacker on my free time in the future I would like
to upgrade the code.

## For users

#### Recommended but not necessary 
mvme (mesytec VME data acquisition: https://www.mesytec.com/downloads/mvme.html ). Control software of the VME module used for data acquisition y can be used to replay the individual runs and check the generated histograms are correct.

### Mandatory 
You need **RootCERN (https://root.cern/install/) and >=Python 3.11.5 version**. 

### Code Structure
The code is fully self-contained and aside from the requirements should be able to run on its own, the folders are organized the following way:
* Include-> ModuleFather.h (Virtual class gives structure to daughter classes used to read).
* Src-> Daughter classes, the modules that are currently implemented are Caen785, Caen785N, Caen1190a, MADC32, MDPP16, MDPP32, VMMR8.
* Zip-> Files to analyze.
* Root-> Store for outputs.

### How to execute the code
If the user just wants to unpack the data follow these steps:

**NOTE: All files are stored in the Zip folder and the code looks in this folder, this is to make the code self-contained, if the user desires to modify it just change the path in Run_unpack.sh**
1. Open FileList.txt with your preferred editor.
2. Copy the name with no extension of the file (or files) you desire to Unpack (avoid empty lines at the end).
3. Chmod +x Run_unpack.sh
4. ./Run_unpack.sh

And that is it! The code will search for the file in the folder unzip it and extract the Mesytec file with the data to transcribe. In the end, the root file contains a TTree with the data and a series of histograms to compare with the Mesytec replay to ensure that all is ok
All root files are stored in the Root folder and a root session will be opened automatically

## For developers/curious people
For those interested in the internal workings:
For every run, MVME produces a .zip containing all relevant files for the run. The .mvlclst contains the data stored as a hex list.
In the hex, each line starts with a header summarizing the info in that line.

     Header: Type[7:0] Continue[0:0] ErrorFlags[2:0] StackNum[3:0] CtrlId[2:0] Length[12:0]
      TTTT TTTT CEEE SSSS IIIL LLLL LLLL LLLL

"StackNum" is the 4-bit field used to describe detectors that send info to the same place. meme reserves stack0 for interactive commands, so readout command stacks
start with stack1 for event0, stack2 for event1, etc.

Constants dealing with frame types here: https://github.com/flueke/mesytec-mvlc/blob/22d88cdd9cea341d6bdb5b455c5681ea2a14ad19/src/mesytec-mvlc/mvlc_constants.h#L140

Only relevant frames are F3 F5 and F9 (data frames)
Each F3 is an event in the electronics (i.e. a trigger) each F5 is the data sent by a module. If we have N modules F3 will be followed by N F5. F9 is a continuation frame used to break up large data strings.

The order in which the modules send the info is always the same (module number set by user) this is reflected in the F5s that will have the same order.

### Objectives for the unpacker

The code needs to locate all F3 and transcribe the information in each F5. The information in the F5 is unique to the module type-> The code needs to know the module structure to unpack. 
Also, each model needs unique unpacking functions. The code must be flexible enough to take many different configurations, the code is already tested with multiple files and it runs correctly. 

### How it works
The code uses sequentially three programs to Unpack
1. Run_unpack.sh: A simple bash script that searches for the desired file unzips and calls the other two programs.
2. Interpreter.py: Python scrip used to obtain the module configuration. Information on the module order (and many other things) is stored in the FA modules. This script does a "quick" read of the hex and locates the hex patterns associated with the event declaration and the module starts using a dictionary. The info is transcribed to a configuration script for the unpacker. 
(More info in the script)
3. Unpack_mvme: Unpacker does a slow read of the file and transcribes to the root. Each module has a custom class with the appropriate transcription methods (src folder) folder.
All classes inherit from the ModuleFather.h virtual class (include folder)
For each F3 the Unpacker will call the reading methods from each class and save that information to the branches of a TTree (More info in the script).

### Example video
https://github.com/user-attachments/assets/16e9e31b-b9a0-4099-aa6f-c2601b58dbff


