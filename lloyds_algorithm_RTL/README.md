    ----------------------------------------------------------------------------------
    -- Felix Winterstein, Imperial College London
    -- 
    -- README.txt
    -- 
    -- Revision 1.01
    -- Additional Comments: distributed under a 3-clause BSD license, see LICENSE.txt
    -- 
    ----------------------------------------------------------------------------------

The HDL code provided in this folder is an implementation of 'Lloyd`s Algorithm' for K-Means Clustering and is part of a case study described in the paper:
_F. Winterstein, S. Bayliss, and G. Constantinides, “Fpga-based k-means clustering using tree-based data structures,”
in Proc. Int. Conf. on Field Programmable Logic and Applications (FPL), 2013, pp. 1–6_.

The VHDL source files and Xilinx IP cores (*.xci files) are provided here without project files.
I used these sources in a Xilinx Vivado project (Vivado 2012.2, RTL flow).
If you want to create a Vivado project using these sources you may find the following instructions helpful:

1. Launch Vivado and create a new project.
2. Add the HDL sources in 'source/vhdl'.
3. Skip adding existing IP here (this will be done later).
4. Add the constraint file in 'constraints/'.
5. Select a device (the IP cores were generated for a Virtex 7 xc7vx485tffg1157-1 FPGA).
6. In Vivado, run the TCL-script 'import_ip_cores.tcl' to include all IP cores in the design.
7. You can regenerate all cores using the TCL-script 'regenerate_ip_cores.tcl' (it may be necessary to upgrade the IP cores first if the coregen version has changed).
8. Add 'simulation/testbench.vhd' as simulation source and set 'testbench' as top level entity for simulation.
9. All design parameters are set in the file 'source/vhdl/lloyds_algorithm_pkg.vhd'.

Simulating the design:
I used Modelsim, I haven't tried the testbench with other simulators.
The 'simulation' folder contains several files with input data (*.mat) that are read by the testbench.
Before running the simulation, modify line 27 and 28 of 'testbench.vhd' according to the input files you want to use.
The file name indicates the clustering parameters N (data point set), K (clusters), D (dimensionality), and s (standard deviation sigma). 
Please refer to the paper above for information about the meaning of these parameters. The constants 'MY_N' (line 24) and 'MY_K' (line 25) must be adapted according to the input files.
Run the *.do-file 'simulation/rerun.do' to set up the basic parameters and waveforms for a Modelsim simulation.
