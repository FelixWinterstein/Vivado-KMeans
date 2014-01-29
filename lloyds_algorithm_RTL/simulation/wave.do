onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /testbench/clk
add wave -noupdate /testbench/read_file_done
add wave -noupdate /testbench/state
add wave -noupdate /testbench/reset_counter
add wave -noupdate /testbench/reset_counter_done
add wave -noupdate /testbench/init_counter
add wave -noupdate /testbench/init_counter_done
add wave -noupdate /testbench/sclr
add wave -noupdate /testbench/start
add wave -noupdate -divider {Filtering Algorithm Top}
add wave -noupdate /testbench/uut/state
add wave -noupdate /testbench/uut/comb_valid
add wave -noupdate /testbench/uut/init_counter_done
add wave -noupdate -radix unsigned /testbench/uut/iterations_counter
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/sclr
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/start
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/wr_init_node
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/processing_done
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/state
add wave -noupdate -divider {memory_mgmt init}
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/wr_init_node
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/wr_init_pos
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/wr_node_reg
add wave -noupdate -radix unsigned /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/tmp_wr_node_address
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/wr_pos_reg
add wave -noupdate -radix unsigned /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/wr_pos_address_reg
add wave -noupdate -radix decimal -expand -subitemconfig {/testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/wr_pos_data_reg(0) {-height 15 -radix decimal} /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/wr_pos_data_reg(1) {-height 15 -radix decimal} /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/wr_pos_data_reg(2) {-height 15 -radix decimal}} /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/wr_pos_data_reg
add wave -noupdate -divider Scheduler
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/state
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/schedule_state
add wave -noupdate -radix unsigned /testbench/uut/lloyds_algorithm_core_inst/schedule_counter
add wave -noupdate -radix unsigned /testbench/uut/lloyds_algorithm_core_inst/schedule_node_counter
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/schedule_counter_done
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/schedule_first
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/schedule_next
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/processing_done
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/processing_done_reg
add wave -noupdate -divider memory_mgmt
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/rd
add wave -noupdate -radix unsigned /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/rd_node_addr
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/rd_state
add wave -noupdate -radix unsigned /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/rd_k_reg
add wave -noupdate -radix unsigned /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/rd_counter
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/rd_counter_done
add wave -noupdate -radix unsigned /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/tmp_rd_node_address
add wave -noupdate -divider retrieve_centres_n_data
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/valid
add wave -noupdate -radix decimal /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/rd_centre_list_pos_data
add wave -noupdate -radix decimal /testbench/uut/lloyds_algorithm_core_inst/memory_mgmt_inst/rd_node_data
add wave -noupdate -divider {Closest point search}
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/g_par_1(0)/process_node_inst/closest_to_point_inst/nd
add wave -noupdate -radix decimal /testbench/uut/lloyds_algorithm_core_inst/g_par_1(0)/process_node_inst/closest_to_point_inst/reg_u_in
add wave -noupdate -radix decimal /testbench/uut/lloyds_algorithm_core_inst/g_par_1(0)/process_node_inst/closest_to_point_inst/reg_point
add wave -noupdate -divider {process tree node output}
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/g_par_1(0)/process_node_inst/nd
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/g_par_1(0)/process_node_inst/rdy
add wave -noupdate -radix unsigned /testbench/uut/lloyds_algorithm_core_inst/g_par_1(0)/process_node_inst/final_index_out
add wave -noupdate -radix unsigned /testbench/uut/lloyds_algorithm_core_inst/processing_done_counter
add wave -noupdate -divider centre_buffer_mgmt
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/g_par_2(0)/centre_buffer_mgmt_inst/init
add wave -noupdate -radix unsigned /testbench/uut/lloyds_algorithm_core_inst/g_par_2(0)/centre_buffer_mgmt_inst/addr_in_init
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/g_par_2(0)/centre_buffer_mgmt_inst/nd
add wave -noupdate -radix unsigned /testbench/uut/lloyds_algorithm_core_inst/g_par_2(0)/centre_buffer_mgmt_inst/addr_in
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/g_par_2(0)/centre_buffer_mgmt_inst/state
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/g_par_2(0)/centre_buffer_mgmt_inst/we_reg
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/g_par_2(0)/centre_buffer_mgmt_inst/valid
add wave -noupdate -radix decimal /testbench/uut/lloyds_algorithm_core_inst/g_par_2(0)/centre_buffer_mgmt_inst/wgtcent_out
add wave -noupdate -radix decimal /testbench/uut/lloyds_algorithm_core_inst/g_par_2(0)/centre_buffer_mgmt_inst/sum_sq_out
add wave -noupdate -radix decimal /testbench/uut/lloyds_algorithm_core_inst/g_par_2(0)/centre_buffer_mgmt_inst/count_out
add wave -noupdate -divider {adder tree}
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/centre_buffer_valid(0)
add wave -noupdate -radix unsigned /testbench/uut/lloyds_algorithm_core_inst/centre_buffer_count(0)
add wave -noupdate -divider {final output single}
add wave -noupdate -radix unsigned /testbench/uut/lloyds_algorithm_core_inst/cycle_count
add wave -noupdate -radix unsigned /testbench/uut/lloyds_algorithm_core_inst/processing_done_counter
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/rdy
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/rdo_centre_buffer
add wave -noupdate -radix unsigned /testbench/uut/lloyds_algorithm_core_inst/centre_buffer_addr
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/valid
add wave -noupdate -radix unsigned /testbench/uut/lloyds_algorithm_core_inst/count_out
add wave -noupdate -radix decimal /testbench/uut/lloyds_algorithm_core_inst/sum_sq_out
add wave -noupdate -radix decimal -expand -subitemconfig {/testbench/uut/lloyds_algorithm_core_inst/wgtcent_out(0) {-height 15 -radix decimal} /testbench/uut/lloyds_algorithm_core_inst/wgtcent_out(1) {-height 15 -radix decimal} /testbench/uut/lloyds_algorithm_core_inst/wgtcent_out(2) {-height 15 -radix decimal}} /testbench/uut/lloyds_algorithm_core_inst/wgtcent_out
add wave -noupdate -divider Divider
add wave -noupdate /testbench/uut/divider_nd
add wave -noupdate -radix decimal /testbench/uut/divider_wgtcent_in
add wave -noupdate -radix decimal /testbench/uut/divider_count_in
add wave -noupdate /testbench/uut/comb_valid
add wave -noupdate /testbench/uut/divider_top_inst/divide_by_zero
add wave -noupdate -radix decimal /testbench/uut/comb_new_position
add wave -noupdate -divider {Final Output}
add wave -noupdate /testbench/uut/lloyds_algorithm_core_inst/first_start
add wave -noupdate -radix unsigned /testbench/uut/lloyds_algorithm_core_inst/cycle_count
add wave -noupdate /testbench/uut/valid
add wave -noupdate -radix decimal -expand -subitemconfig {/testbench/uut/clusters_out(0) {-height 15 -radix decimal} /testbench/uut/clusters_out(1) {-height 15 -radix decimal} /testbench/uut/clusters_out(2) {-height 15 -radix decimal}} /testbench/uut/clusters_out
add wave -noupdate -radix decimal /testbench/uut/distortion_out
add wave -noupdate /testbench/uut/rdy
add wave -noupdate -radix unsigned /testbench/uut/iterations_counter
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {119917578 ps} 0}
configure wave -namecolwidth 434
configure wave -valuecolwidth 56
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ps
update
WaveRestoreZoom {119912935 ps} {120004583 ps}
