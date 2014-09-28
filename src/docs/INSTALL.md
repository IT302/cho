## Structure
Each application in the benchmark suite has three directories. For example, gsm has gsm (the original CHStone application), gsm\_opencl (the OpenCl port targeting CPUs) and gsm\_opencl\_fpga (the OpenCL port targeting Altera OpenCl). The CHStone applications are covered by their original licenses while OpenCL applications are released under GPL v3.. Please read cho/LICENSE.


## Depencdencies 
+ Scons build system
+ OpenCL SDK  for CPUs (Intel OpenCL SDK or AMD APP SDK)  and FPGA (Altera SDK for OpenCL). 

##Building and Running

The build system is based on  Scons. cho/SConstruct is  main script SCons script.  To build the OpenCL and C applications run 
```shell
$ scons -Q cpu=1
```
from the top directory. For now the FPGA application has to built manually from each individual directory using Makefile. The present setup targets Altera SDK for OpenCL. Please, read Altera SDK for OpenCL: Getting Started Guide to see how to setup  the SDK.


## Issues, Questions and Contributing Code
Please contact me via geoffrey.ndu@manchester.ac.uk