set_property SRC_FILE_INFO {cfile:c:/Users/dat14amr/Program/HannesAdamMachine/HannesAdamMachine.srcs/sources_1/bd/design_1/ip/design_1_microblaze_0_0/design_1_microblaze_0_0.xdc rfile:../../../HannesAdamMachine.srcs/sources_1/bd/design_1/ip/design_1_microblaze_0_0/design_1_microblaze_0_0.xdc id:1 order:EARLY scoped_inst:design_1_i/microblaze_0/U0} [current_design]
set_property SRC_FILE_INFO {cfile:c:/Users/dat14amr/Program/HannesAdamMachine/HannesAdamMachine.srcs/sources_1/bd/design_1/ip/design_1_clk_wiz_0_0/design_1_clk_wiz_0_0.xdc rfile:../../../HannesAdamMachine.srcs/sources_1/bd/design_1/ip/design_1_clk_wiz_0_0/design_1_clk_wiz_0_0.xdc id:2 order:EARLY scoped_inst:design_1_i/clk_wiz_0/inst} [current_design]
set_property src_info {type:SCOPED_XDC file:1 line:2 export:INPUT save:INPUT read:READ} [current_design]
set_ip_msg_config -idlist { DPOP-3 PDCN-1569 }
set_property src_info {type:SCOPED_XDC file:2 line:57 export:INPUT save:INPUT read:READ} [current_design]
set_input_jitter [get_clocks -of_objects [get_ports clk_in1]] 0.1
