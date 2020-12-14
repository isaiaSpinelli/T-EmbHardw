LIBRARY ieee;
USE ieee.std_logic_1164.all;
  use ieee.std_logic_unsigned.all;
  use ieee.numeric_std.all;

--LIBRARY __library_name;
-- USE __library_name.__package_name.ALL;

ENTITY CI_test IS
 PORT(

	 dataa: IN STD_LOGIC_VECTOR (31 DOWNTO 0); -- operand A <always required>

	 result : OUT STD_LOGIC_VECTOR (31 DOWNTO 0) -- result <always required>
);
END CI_test;

ARCHITECTURE test OF CI_test IS

constant mask1: Std_Logic_Vector(31 downto 0) := x"0000001f";
constant mask2: Std_Logic_Vector(31 downto 0) := x"0000003f";

 BEGIN


process(dataa)
    	variable green : unsigned(result'range);
	variable red : unsigned(result'range);
	variable blue : unsigned(result'range);

	variable sum : unsigned(result'range);

  begin
    
	red :=  (shift_right(unsigned(dataa), 11)) and unsigned(mask1);
	red := resize(((shift_left(red, 3)) * 21), 32) ; -- to_unsigned(21, 6)

	green :=  shift_right(unsigned(dataa), 5) and unsigned(mask2) ;
	green := resize(((shift_left(green, 2)) * 72), 32);

	blue :=  shift_right(unsigned(dataa), 0) and unsigned(mask1) ;
	blue := resize(((shift_left(blue, 3)) * 7), 32);

	sum := red+green+blue;
	-- resize( red+green+blue , 32);
	sum := (sum/100);
	


	result <= std_logic_vector(sum);
  end process;


END test;
