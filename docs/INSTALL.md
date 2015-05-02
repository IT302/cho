
  / ___| | | |/ _ \ 
 | |   | |_| | | | |
 | |___|  _  | |_| |
  \____|_| |_|\___/ 
 
## Overview 

 The benchmarks were tested  using Intel's and Altera's OpenCL SDKs only. Note that CHO is still at the early stage of development. 


## Structure
Each application in the benchmark suite has three directories. For example, gsm has gsm (the original CHStone application), gsm\_opencl (the OpenCl port targeting CPUs) and gsm\_opencl\_fpga (the OpenCL port targeting Altera OpenCl). The CHStone applications are covered by their original licenses while OpenCL applications are released under GPL v3.. Please read cho/LICENSE.


## Depencdencies 
+ gcc
+ Scons build system
+ OpenCL SDK  for CPUs (Intel OpenCL SDK or AMD APP SDK)  and FPGA (Altera SDK for OpenCL). 

##Building and Running

The build system is based on  Scons. src/SConstruct is  main  SCons script.  To build the OpenCL and C applications run 
```shell
$ scons
```
from src directory. For now the FPGA application has to built manually from each individual directory using make. The present setup targets Altera SDK for OpenCL. Please, read Altera SDK for OpenCL: Getting Started Guide to see how to set up  the SDK.


## Issues, Questions and Contributing Code
Please contact me via geoffrey.nduATmanchester.ac.uk