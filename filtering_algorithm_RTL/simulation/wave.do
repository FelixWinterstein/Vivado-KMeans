onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /testbench/clk
add wave -noupdate /testbench/read_file_done
add wave -noupdate /testbench/state
add wave -noupdate /testbench/reset_counter
add wave -noupdate /testbench/reset_counter_done
add wave -noupdate /testbench/init_counter
add wave -noupdate /testbench/init_counter_done
add wave -noupdate -radix unsigned /testbench/wr_node_data_init.count
add wave -noupdate -radix unsigned /testbench/wr_node_data_init.left
add wave -noupdate /testbench/sclr
add wave -noupdate /testbench/start
add wave -noupdate -divider {Filtering Algorithm Top}
add wave -noupdate /testbench/uut/state
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/sclr
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/wr_init_node
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/wr_init_cent
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/wr_init_pos
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/start
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/processing_done
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/state
add wave -noupdate /testbench/uut/comb_valid
add wave -noupdate /testbench/uut/init_counter_done
add wave -noupdate -radix unsigned /testbench/uut/iterations_counter
add wave -noupdate /testbench/uut/iterations_counter_done
add wave -noupdate -divider {memory_mgmt init}
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_state
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_init_cent
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_centre_list_address_init
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_counter
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/tmp_wr_centre_list_address
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_cent_reg
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/tmp_centre_in
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_init_node
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_node_reg
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/tmp_wr_node_address
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_node_data_reg
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_node_data_reg.count
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_init_pos
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_centre_list_pos_address_init
add wave -noupdate -expand -subitemconfig {/testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_centre_list_pos_data_init(0) {-height 15 -radix decimal} /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_centre_list_pos_data_init(1) {-height 15 -radix decimal} /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_centre_list_pos_data_init(2) {-height 15 -radix decimal}} /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_centre_list_pos_data_init
add wave -noupdate -divider Scheduler
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/state
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_push
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_push_reg
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_pop
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_empty
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_valid
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/cntr_stack_k_out
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/schedule_k_reg
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/schedule_state
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/schedule_counter
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/schedule_counter_done
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/schedule_next
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/processing_done_counter
add wave -noupdate -divider memory_mgmt
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/rd
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/rd_node_addr
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/rd_k
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/rd_centre_list_address
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/rd_state
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/rd_centre_list_address_reg
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/rd_k_reg
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/rd_counter
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/tmp_rd_centre_list_address
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/reading_centres
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/tmp_centre_out
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/rd_counter_done
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/tmp_rd_node_address
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_cent_nd
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_cent
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_centre_list_address
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_centre_list_data
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_state
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_counter
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_cent_reg
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/centre_index_memory_top_inst/wea(0)
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_centre_list_address_reg
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/tmp_wr_centre_list_address
add wave -noupdate -divider {alloc info}
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/reading_centres
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/centre_index_memory_top_inst/rd_first_cl
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/centre_index_memory_top_inst/wr_first_cl
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/centre_index_memory_top_inst/trace_mem_wr_addr
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/centre_index_memory_top_inst/trace_mem_we
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/centre_index_memory_top_inst/trace_mem_d
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/last_centre
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/item_read_twice
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/rd_centre_list_address_out
add wave -noupdate -divider retrieve_centres_n_data
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/valid
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/item_read_twice_delay_line(1)
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/rd_centre_list_data
add wave -noupdate -radix decimal -expand -subitemconfig {/testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/rd_centre_list_pos_data(0) {-height 15 -radix decimal} /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/rd_centre_list_pos_data(1) {-height 15 -radix decimal} /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/rd_centre_list_pos_data(2) {-height 15 -radix decimal}} /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/rd_centre_list_pos_data
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/rd_node_data
add wave -noupdate -divider clostest_to_point
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/nd
add wave -noupdate -radix decimal -expand -subitemconfig {/testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/u_in.bnd_lo(0) {-height 15 -radix decimal} /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/u_in.bnd_lo(1) {-height 15 -radix decimal} /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/u_in.bnd_lo(2) {-height 15 -radix decimal}} /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/u_in.bnd_lo
add wave -noupdate -radix decimal -expand -subitemconfig {/testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/u_in.bnd_hi(0) {-height 15 -radix decimal} /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/u_in.bnd_hi(1) {-height 15 -radix decimal} /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/u_in.bnd_hi(2) {-height 15 -radix decimal}} /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/u_in.bnd_hi
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/closest_to_point_inst/nd
add wave -noupdate -radix decimal -expand -subitemconfig {/testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/u_midpoint(0) {-height 15 -radix decimal} /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/u_midpoint(1) {-height 15 -radix decimal} /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/u_midpoint(2) {-height 15 -radix decimal}} /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/u_midpoint
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/closest_to_point_inst/u_in
add wave -noupdate -divider resync
add wave -noupdate -divider {prune centres}
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/nd
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/point_list_d
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/point_list_idx
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/point
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/bnd_lo
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/bnd_hi
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/state
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/pruning_test_nd
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/pruning_test_rdy
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/too_far
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/idle_delay_line(8)
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/index_delay_line(8)
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/counter
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/valid
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/result
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/point_list_idx_out
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/rdy
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/min_num_centres
add wave -noupdate -divider pruning_test
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/pruning_test_inst/nd
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/pruning_test_inst/cand
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/pruning_test_inst/closest_cand
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/pruning_test_inst/tmp_diff_1_rdy
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/pruning_test_inst/tmp_diff_1
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/pruning_test_inst/tmp_diff_2_rdy
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/pruning_test_inst/tmp_diff_2
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/pruning_test_inst/tmp_mul_1_rdy
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/pruning_test_inst/tmp_mul_1
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/pruning_test_inst/tmp_mul_2_rdy
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/pruning_test_inst/tmp_mul_2
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/pruning_test_inst/rdy
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/pruning_test_inst/result
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/pruning_test_inst/tmp_tree_adder_res_1_ext
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/prune_centres_inst/pruning_test_inst/tmp_tree_adder_res_2_clean
add wave -noupdate -divider {dot products}
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/dot_product_inst_1_2/point_1
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/dot_product_inst_1_2/point_2
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/dot_product_inst_1_2/result
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/dot_product_inst_2_2/result
add wave -noupdate -divider {compute squared sums}
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/compute_squared_sums_inst/nd
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/compute_squared_sums_inst/u_sum_sq
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/compute_squared_sums_inst/u_count
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/compute_squared_sums_inst/op1
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/compute_squared_sums_inst/op2
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/compute_squared_sums_inst/rdy
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/sum_sq_rdy_delay(0)
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/compute_squared_sums_inst/squared_sums
add wave -noupdate -divider {dead end}
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/tmp_u_left
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/tmp_u_right
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/new_k
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/tmp_dead_end
add wave -noupdate -divider {process tree node output}
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/rdy
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/update_centre_buffer
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/dead_end
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/final_index_out
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/sum_sq_out
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/debug_u_left
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/debug_u_right
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/k_out
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/centre_index_rdy
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/centre_index_wr
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/process_tree_node_inst/centre_indices_out
add wave -noupdate -divider {stack mgmt}
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/memory_mgmt_inst/wr_centre_list_address
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/push
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/pop
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/node_stack_mgmt_inst/push
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/node_addr_in_1
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/node_addr_in_2
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/k_in_1
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/k_in_2
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/cntr_addr_in_1
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/cntr_addr_in_2
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/node_stack_mgmt_inst/pop
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/node_stack_mgmt_inst/stack_pointer
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/node_stack_mgmt_inst/node_stack_memory_inst/wea(0)
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/node_stack_mgmt_inst/node_addr_in
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/centre_stack_mgmt_inst/push
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/centre_stack_mgmt_inst/cntr_addr_in
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/centre_stack_mgmt_inst/k_in
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/centre_stack_mgmt_inst/stack_pointer
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/node_stack_mgmt_inst/valid
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/node_stack_mgmt_inst/empty
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/node_stack_mgmt_inst/node_addr_out
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/centre_stack_mgmt_inst/valid
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/k_out
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/stack_top_inst/cntr_addr_out
add wave -noupdate -divider allocator
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/ptn_rdy
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/ptn_dead_end
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/ptn_centre_index_rdy
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/allocator_alloc
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/allocator_free_1
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/allocator_free_2
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/allocator_free
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/g_dyn_alloc/allocator_inst/tmp_free
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/allocator_free_address
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/g_dyn_alloc/allocator_inst/address_in
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/g_dyn_alloc/allocator_inst/free_list_mem_we
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/g_dyn_alloc/allocator_inst/free_list_mem_din
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/rd_centre_list_address_out
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/rd_centre_list_address_out_reg
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/g_dyn_alloc/allocator_inst/current_free_location
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/g_dyn_alloc/allocator_inst/rdy
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/g_dyn_alloc/allocator_inst/free_list_mem_dout
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/g_dyn_alloc/allocator_inst/address_out
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/cntr_stack_addr_in
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/g_dyn_alloc/allocator_inst/alloc_count
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/allocator_heap_full
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/g_dyn_alloc/allocator_inst/debug_max_alloc_count
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/g_dyn_alloc/allocator_inst/debug_fl_last_item_reached
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/g_dyn_alloc/allocator_inst/debug_invalid_location
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/g_dyn_alloc/allocator_inst/debug_invalid_malloc
add wave -noupdate -divider centre_buffer_mgmt
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_mgmt_inst/nd
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_mgmt_inst/request_rdo
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_mgmt_inst/init
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_mgmt_inst/state
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_mgmt_inst/addr_in
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_mgmt_inst/sum_sq_in
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_mgmt_inst/count_in
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_mgmt_inst/wgtcent_reg
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_mgmt_inst/sum_sq_reg
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_mgmt_inst/count_reg
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_mgmt_inst/tmp_we
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_mgmt_inst/we_reg
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_mgmt_inst/tmp_wgtcent_int
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_mgmt_inst/tmp_sum_sq_int
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_mgmt_inst/tmp_count_int
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_mgmt_inst/centre_buffer_dist_inst/a
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_mgmt_inst/centre_buffer_dist_inst/dpra
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_mgmt_inst/centre_buffer_dist_inst/d
add wave -noupdate -divider {final output single}
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/debug_max_stack_counter
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/g_dyn_alloc/allocator_inst/debug_max_alloc_count
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/g_dyn_alloc/allocator_inst/debug_fl_last_item_reached
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/g_dyn_alloc/allocator_inst/debug_invalid_malloc
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/visited_nodes
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/cycle_count
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/rdy
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/rdo_centre_buffer
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/centre_buffer_addr
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/valid
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/count_out
add wave -noupdate -radix decimal /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/sum_sq_out
add wave -noupdate -radix decimal -expand -subitemconfig {/testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/wgtcent_out(0) {-radix decimal} /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/wgtcent_out(1) {-radix decimal} /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/wgtcent_out(2) {-radix decimal}} /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/wgtcent_out
add wave -noupdate -divider Divider
add wave -noupdate /testbench/uut/divider_nd
add wave -noupdate -radix decimal /testbench/uut/divider_wgtcent_in
add wave -noupdate -radix decimal /testbench/uut/divider_count_in
add wave -noupdate /testbench/uut/divider_top_inst/divider_valid(0)
add wave -noupdate /testbench/uut/divider_top_inst/divide_by_zero
add wave -noupdate -radix decimal -expand -subitemconfig {/testbench/uut/divider_top_inst/tmp_quotient(0) {-height 15 -radix decimal} /testbench/uut/divider_top_inst/tmp_quotient(1) {-height 15 -radix decimal} /testbench/uut/divider_top_inst/tmp_quotient(2) {-height 15 -radix decimal}} /testbench/uut/divider_top_inst/tmp_quotient
add wave -noupdate /testbench/uut/divider_top_inst/rdy
add wave -noupdate -radix decimal -expand -subitemconfig {/testbench/uut/comb_new_position(0) {-height 15 -radix decimal} /testbench/uut/comb_new_position(1) {-height 15 -radix decimal} /testbench/uut/comb_new_position(2) {-height 15 -radix decimal}} /testbench/uut/comb_new_position
add wave -noupdate -divider {Final Output}
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/rdy
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/visited_nodes
add wave -noupdate /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/first_start
add wave -noupdate -radix unsigned /testbench/uut/g_par_2(0)/filtering_alogrithm_single_inst/cycle_count
add wave -noupdate /testbench/uut/rdy
add wave -noupdate /testbench/uut/valid
add wave -noupdate -radix decimal -expand -subitemconfig {/testbench/uut/clusters_out(0) {-height 15 -radix decimal} /testbench/uut/clusters_out(1) {-height 15 -radix decimal} /testbench/uut/clusters_out(2) {-height 15 -radix decimal}} /testbench/uut/clusters_out
add wave -noupdate -radix decimal /testbench/uut/distortion_out
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {1463870000 ps} 0}
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
WaveRestoreZoom {1463818352 ps} {1464001648 ps}
