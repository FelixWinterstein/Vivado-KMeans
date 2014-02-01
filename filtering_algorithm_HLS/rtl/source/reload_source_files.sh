#----------------------------------------------------------------------------------
#-- Felix Winterstein, Imperial College London
#--
#-- reload_source_files.sh
#--
#-- Revision 1.01
#-- Additional Comments: distributed under a BSD license, see LICENSE.txt
#--
#----------------------------------------------------------------------------------
mv filtering_algorithm_wrapper.vhd filtering_algorithm_wrapper.bak
rm *.vhd
cp ../../filtering_algorithm/solution1/impl/vhdl/* .
# restore wrapper file
mv filtering_algorithm_wrapper.bak filtering_algorithm_wrapper.vhd
# loop over all files in this folder
echo -e "remove_files *.vhd\n"add_files -norecurse {`ls $PWD/*.vhd`} > update_fileset.tcl
