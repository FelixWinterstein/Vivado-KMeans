create_clock -name CLK -period 5.000000 -waveform "0.000000 2.5" ap_clk

#set_input_delay 0 -clock CLK  [all_inputs]
#set_output_delay 0 -clock CLK [all_outputs]


