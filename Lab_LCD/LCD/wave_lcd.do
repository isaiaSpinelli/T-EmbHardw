restart

force clk 0 0, 1 10ns -repeat 20ns
force nReset 0 0, 1 100ns

force Address xx 0, 00 205ns, xx 225ns, 10 405ns, xx 425ns
force WriteData 16#XX 0, 16#2A 205ns, 16#XX 225ns, 16#CC 405ns, 16#XX 425ns
force Write 0 0, 1 205ns, 0 225ns, 1 405ns, 0 425ns
force ChipSelect 0 0, 1 208ns, 0 228ns, 1 405ns, 0 425ns
run 600ns
