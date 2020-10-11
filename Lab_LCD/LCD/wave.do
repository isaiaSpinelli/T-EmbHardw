onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /parallelport/Clk
add wave -noupdate /parallelport/nReset
add wave -noupdate /parallelport/Address
add wave -noupdate /parallelport/ChipSelect
add wave -noupdate /parallelport/Read
add wave -noupdate /parallelport/Write
add wave -noupdate /parallelport/ReadData
add wave -noupdate /parallelport/WriteData
add wave -noupdate -color Magenta /parallelport/Waitrequest
add wave -noupdate /parallelport/LCD_RESETn
add wave -noupdate /parallelport/LCD_CSn
add wave -noupdate /parallelport/LCD_D_Cn
add wave -noupdate -color {Medium Blue} /parallelport/LCD_WRn
add wave -noupdate /parallelport/LCD_RDn
add wave -noupdate /parallelport/IM0
add wave -noupdate /parallelport/LCD_DATA
add wave -noupdate /parallelport/Etat_present
add wave -noupdate /parallelport/Etat_futur
add wave -noupdate /parallelport/start_send_s
add wave -noupdate /parallelport/reset_LCD_s
add wave -noupdate /parallelport/mode_LCD_s
add wave -noupdate /parallelport/cs_n_LCD_s
add wave -noupdate /parallelport/DCn_s
add wave -noupdate /parallelport/dataSend_s
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {start_wr {430237 ps} 0} {end_wr {490237 ps} 0}
quietly wave cursor active 2
configure wave -namecolwidth 150
configure wave -valuecolwidth 100
configure wave -justifyvalue left
configure wave -signalnamewidth 1
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1000
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ps
update
WaveRestoreZoom {381926 ps} {510348 ps}
