onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /testbench/ap_clk
add wave -noupdate /testbench/state
add wave -noupdate /testbench/reset_counter
add wave -noupdate /testbench/reset_counter_done
add wave -noupdate /testbench/ap_rst
add wave -noupdate /testbench/ap_start
add wave -noupdate -radix unsigned /testbench/uut/n_v
add wave -noupdate -radix unsigned /testbench/uut/k_v
add wave -noupdate /testbench/uut/root_v_read
add wave -noupdate -radix unsigned /testbench/uut/root_v_dout
add wave -noupdate /testbench/uut/node_data_read
add wave -noupdate -radix decimal /testbench/node_type_node_data_dout
add wave -noupdate -radix hexadecimal /testbench/uut/node_data_dout
add wave -noupdate /testbench/node_address_v_read
add wave -noupdate -radix unsigned /testbench/node_address_v_dout
add wave -noupdate /testbench/uut/cntr_pos_init_value_v_read
add wave -noupdate /testbench/uut/cntr_pos_init_value_v_empty_n
add wave -noupdate -radix decimal /testbench/data_type_cntr_pos_init_value_v_dout
add wave -noupdate -radix hexadecimal /testbench/uut/cntr_pos_init_value_v_dout
add wave -noupdate /testbench/uut/clusters_out_value_v_full_n
add wave -noupdate /testbench/uut/clusters_out_value_v_write
add wave -noupdate -radix decimal -expand -subitemconfig {/testbench/data_type_clusters_out_value_v_din(0) {-height 15 -radix decimal} /testbench/data_type_clusters_out_value_v_din(1) {-height 15 -radix decimal} /testbench/data_type_clusters_out_value_v_din(2) {-height 15 -radix decimal}} /testbench/data_type_clusters_out_value_v_din
add wave -noupdate /testbench/uut/ap_idle
add wave -noupdate /testbench/uut/ap_done
add wave -noupdate -radix decimal /testbench/cycle_counter
add wave -noupdate -divider Filter
add wave -noupdate -divider {centre heap}
add wave -noupdate -divider Freelist
add wave -noupdate -divider {centre positions}
add wave -noupdate /testbench/uut/centre_positions_0_value_v_u/ce0
add wave -noupdate /testbench/uut/centre_positions_1_value_v_u/ce0
add wave -noupdate -divider {tree node memory}
add wave -noupdate /testbench/uut/tree_node_int_memory_0_u/ce0
add wave -noupdate /testbench/uut/tree_node_leaf_memory_0_u/ce0
add wave -noupdate /testbench/uut/tree_node_int_memory_0_u/we0
add wave -noupdate -radix unsigned /testbench/uut/tree_node_int_memory_0_u/address0
add wave -noupdate -radix unsigned /testbench/uut/tree_node_leaf_memory_0_u/address0
add wave -noupdate /testbench/uut/tree_node_int_memory_1_u/ce0
add wave -noupdate /testbench/uut/tree_node_leaf_memory_1_u/ce0
add wave -noupdate /testbench/uut/tree_node_int_memory_1_u/we0
add wave -noupdate -radix unsigned /testbench/uut/tree_node_int_memory_1_u/address0
add wave -noupdate -radix unsigned /testbench/uut/tree_node_leaf_memory_1_u/address0
add wave -noupdate /testbench/uut/tree_node_int_memory_2_u/ce0
add wave -noupdate /testbench/uut/tree_node_int_memory_2_u/we0
add wave -noupdate /testbench/uut/tree_node_leaf_memory_2_u/ce0
add wave -noupdate /testbench/uut/tree_node_leaf_memory_2_u/we0
add wave -noupdate /testbench/uut/tree_node_int_memory_3_u/ce0
add wave -noupdate /testbench/uut/tree_node_int_memory_3_u/we0
add wave -noupdate /testbench/uut/tree_node_leaf_memory_3_u/ce0
add wave -noupdate /testbench/uut/tree_node_leaf_memory_3_u/we0
add wave -noupdate -divider {node stack}
add wave -noupdate -divider filt_centres_out
add wave -noupdate /testbench/uut/filt_centres_out_0_u/we0
add wave -noupdate /testbench/uut/filt_centres_out_0_u/ce0
add wave -noupdate /testbench/uut/filt_centres_out_1_u/we0
add wave -noupdate /testbench/uut/filt_centres_out_1_u/ce0
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {88230000 ps} 0}
configure wave -namecolwidth 258
configure wave -valuecolwidth 100
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
WaveRestoreZoom {0 ps} {104262 ps}
