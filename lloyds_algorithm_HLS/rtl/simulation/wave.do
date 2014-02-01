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
add wave -noupdate /testbench/uut/data_value_v_read
add wave -noupdate /testbench/uut/data_value_v_empty_n
add wave -noupdate -radix decimal /testbench/data_type_clusters_out_value_v_din
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
add wave -noupdate -divider {centre positions}
add wave -noupdate -divider {data memory}
add wave -noupdate /testbench/uut/data_int_memory_value_v_u/ce1
add wave -noupdate /testbench/uut/data_int_memory_value_v_u/we1
add wave -noupdate -radix unsigned /testbench/uut/data_int_memory_value_v_u/address1
add wave -noupdate /testbench/uut/data_int_memory_value_v_u/ce0
add wave -noupdate -radix unsigned /testbench/uut/data_int_memory_value_v_u/address0
add wave -noupdate /testbench/uut/centre_positions_3_value_v_u/ce0
add wave -noupdate -radix unsigned /testbench/uut/centre_positions_3_value_v_u/address0
add wave -noupdate /testbench/uut/centre_positions_0_value_v_u/ce0
add wave -noupdate -radix unsigned /testbench/uut/centre_positions_0_value_v_u/address0
add wave -noupdate /testbench/uut/centre_positions_1_value_v_u/ce0
add wave -noupdate -radix unsigned /testbench/uut/centre_positions_1_value_v_u/address0
add wave -noupdate /testbench/uut/centre_positions_2_value_v_u/ce0
add wave -noupdate -radix unsigned /testbench/uut/centre_positions_2_value_v_u/address0
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {19650417 ps} 0}
configure wave -namecolwidth 324
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
WaveRestoreZoom {19610973 ps} {19709027 ps}
