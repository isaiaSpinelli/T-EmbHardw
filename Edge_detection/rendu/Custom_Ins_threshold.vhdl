LIBRARY ieee;
USE ieee.std_logic_1164.all;
  use ieee.std_logic_unsigned.all;
  use ieee.numeric_std.all;

--LIBRARY __library_name;
-- USE __library_name.__package_name.ALL;

ENTITY CI_threshold IS
    generic (
        -- Threshold level
        THRESHOLD  : integer   := 128
    );
 PORT(

	dataa: IN STD_LOGIC_VECTOR (31 DOWNTO 0); -- operand A <always required>
	datab: IN STD_LOGIC_VECTOR (31 DOWNTO 0); -- operand A <always required>

	result : OUT STD_LOGIC_VECTOR (31 DOWNTO 0) -- result <always required>
);
END CI_threshold;

ARCHITECTURE comb OF CI_threshold IS

--constant threshold: unsigned(31 downto 0) := x"00000080";
constant threshold_2: integer  := 128;


 BEGIN


process(dataa, datab)
    	variable dataa_abs : unsigned(result'range);
	variable datab_abs : unsigned(result'range);
	variable sum : unsigned(result'range);


  begin
    
	dataa_abs := unsigned(abs(signed(dataa)));
	datab_abs := unsigned(abs(signed(datab)));

	sum := dataa_abs + datab_abs;

	if sum > THRESHOLD then -- threshold
	  result <= x"ffffffff"; 
	else
	  result <= x"00000000";
	end if;


  end process;


END comb;
