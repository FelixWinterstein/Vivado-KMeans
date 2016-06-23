    ----------------------------------------------------------------------------------
    -- Felix Winterstein, Imperial College London
    -- 
    -- README.txt
    -- 
    -- Revision 1.01
    -- Additional Comments: distributed under a 3-clause BSD license, see LICENSE.txt
    -- 
    ----------------------------------------------------------------------------------

The code provided in this folder is a C-based HLS implementation of 'Lloyd`s Algorithm' for K-Means Clustering and is part of a case study described in the paper:
_F. Winterstein, S. Bayliss, and G. Constantinides, “High-level synthesis of dynamic data structures: a case study using Vivado HLS,”
in Proc. Int. Conf. on Field Programmable Technology (FPT), 2013, pp. 362-365_.

The C source files are provided here without project files, but they contain HLS directives specific to Xilinx Vivado HLS.
If you want to create a Vivado HLS project using these sources you may find the following instructions helpful:

1. Launch Vivado HLS and create a new project.
2. Add the C sources in 'source/' and set 'lloyds_algorithm_top' as top-level function.
3. Add all files in 'simulation/' (*.cpp and *.mat) as test bench files.
4. Select a device and clock period constaint.
5. All design parameters are set in the file 'source/lloyds_algorithm_top.h'.
6. Run the C test bench 'lloyds_algorithm_tb.cpp' to check whether everything is set up properly.

Synthesizing RTL code:
A VHDL testbench is provided in 'rtl/simulation/testbench.vhd' which can be used to run an RTL simulation of the generated VHDL code:

1. Run synthesis in Vivado HLS
2. Run 'Export RTL' and select 'IP-XACT' as format. This creates a folder '{HLS project name}/{solution name}/impl'.
3. Change to the folder 'rtl/source' and run the linux shell script 'reload_source_files.sh'.
   The script copies all *.vhd files from 'lloyds_algorithm/solution1/impl/vhdl' into the folder 'rtl/source' (where 'lloyds_algorithm/solution1' is '{HLS project name}/{solution name}').
   It also generates a TCL-script 'update_fileset.tcl' in the same folder in order to load the *.vhd files into a Vivado RTL project.
4. Launch Vivado (RTL flow) and create a new project.
5. Add the HDL sources in 'rtl/source/'.
6. Add the constraint file in 'rtl/constraints/'.
7. Select the same device as for HLS.
8. Add 'simulation/testbench.vhd' as simulation source and set 'testbench' as top level entity for simulation.

I used Modelsim for RTL simulation, I haven't tested the testbench with other simulators.
The testbench in 'rtl/simulation' uses the same *.mat-input files as the C simulation.
Before running the simulation, modify line 27 and 18 of 'testbench.vhd' according to the input files you want to use.
The file name indicates the clustering parameters N (data point set), K (clusters), D (dimensionality), and s (standard deviation sigma). 
Please refer to the paper above for information about the meaning of these parameters. The constants 'MY_N' (line 16) and 'MY_K' (line 17) must be adapted according to the input files.
Run the *.do-file 'rtl/simulation/rerun.do' to set up the basic parameters and waveforms for a Modelsim simulation.

