Vivado-KMeans
=============

Hand-written HDL code and C-based HLS designs for K-means clustering implementations on FPGAs

High-level synthesis promises a significant shortening of the FPGA design cycle when compared with design entry using register transfer level (RTL) languages.
Recent evaluations report that C-to-RTL flows can produce results with a quality close to hand-crafted designs.
Algorithms which use dynamic, pointer-based data structures, which are common in software, remain difficult to implement well.
This repository contains the source code of a comparative case study using Xilinx Vivado HLS as an exemplary state-of-the-art high-level synthesis (HLS) tool.
Our test cases are two alternative algorithms for the same compute-intensive machine learning technique (K-means clustering) with significantly different computational properties.
We compare a data-flow centric implementation (Lloyd's Algorithm) to a recursive tree traversal implementation (the Filtering Algorithm) which incorporates complex data-dependent control flow and makes use of pointer-linked data structures and dynamic memory allocation.
In addition to C-based HLS designs, we include hand-written and -optimised VHDL code for both implementations for comparison.

Probably the best way to understand what this is all about is to read my papers included in this repository.

(c) Felix Winterstein, Imperial College London. The source code is distributed under a 3-clause BSD license (see LICENSE.txt).

[![githalytics.com alpha](https://cruel-carlota.pagodabox.com/fdd004cf9c26b71e20a5809a77718c8c "githalytics.com")](http://githalytics.com/FelixWinterstein/Vivado-KMeans)
