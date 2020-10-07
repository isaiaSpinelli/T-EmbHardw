-----------------------------------------------------------------------
--
-- File         : LCD.vhd
-- Author       : Spinelli Isaia
-- Description  : Controller LCD 16 bits (LCD.vhd),
-- used for     : T-EmbHardw lab
--| Modifications |-----------------------------------------------------------
-- Ver  Date       Auteur  Description
------------------------------------------------------------------------------

library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;

ENTITY ParallelPort IS
	 PORT(
		-- Avalon interfaces signals
		 Clk 		: IN std_logic;
		 nReset 	: IN std_logic;
		 Address 	: IN std_logic_vector (1 DOWNTO 0);
		 ChipSelect : IN std_logic;
		 Read 		: IN std_logic;
		 Write 		: IN std_logic;
		 ReadData	: OUT std_logic_vector (15 DOWNTO 0);
		 WriteData 	: IN std_logic_vector (15 DOWNTO 0);
		 
		 
		-- Parallel Port external interface
		LCD_RESETn 	: OUT std_logic;
		LCD_CSn 	: OUT std_logic;
		LCD_D_Cn 	: OUT std_logic;
		LCD_WRn 	: OUT std_logic;
		LCD_RDn 	: OUT std_logic;
		IM0 		: OUT std_logic;
		
		LCD_DATA 	: OUT std_logic_vector (15 DOWNTO 0)
	 );
End ParallelPort;


ARCHITECTURE comp OF ParallelPort IS

	signal reset_LCD_s 			: std_logic; -- LCD_RESETn
	signal mode_LCD_s 			: std_logic; -- IM0
	signal cs_LCD_s 			: std_logic; -- LCD_CSn 
	
	-- Data or Cmd ?
	signal DCn_s	 			: std_logic; -- LCD_D_Cn 
	
	signal dataSend_s			: std_logic_vector (15 DOWNTO 0);


  BEGIN
  
  LCD_RESETn <= not reset_LCD_s;
  IM0 <= mode_LCD_s;
  LCD_CSn <= cs_LCD_s;
  DCn_s <= LCD_D_Cn;

pRegWr:
	process(Clk, nReset)
	begin
		if nReset = '0' then
			iRegDir <= (others => '0'); -- Input by default

		elsif rising_edge(Clk) then
			if ChipSelect = '1' and Write = '1' then -- Write cycle
				case Address(1 downto 0) is
				
					-- CONTROLE REGISTRE
					when "00" => 
						reset_LCD_s <= WriteData(0);
						cs_LCD_s 	<= WriteData(1);
						LCD_D_Cn 	<= WriteData(2);
					
					-- SEND COMMAND
					when "01" => 
						LCD_D_Cn <= '0';
						dataSend_s <= WriteData;
						
					-- SEND DATA
					when "10" => 
						LCD_D_Cn <= '1';
						dataSend_s <= WriteData;
						
					when "11" => 
					
					when others => null;
				end case;
			end if;
		end if;
	end process pRegWr;

-- Read Process from registers with wait 1
pRegRd:
	process(Clk)
	begin
		if rising_edge(Clk) then
			ReadData <= (others => '0'); -- default value
			if ChipSelect = '1' and Read = '1' then -- Read cycle
				case Address(1 downto 0) is
					when "00" => 
					
						ReadData(0) <= reset_LCD_s;
						ReadData(1) <= mode_LCD_s;
						ReadData(2) <= cs_LCD_s;
					
					when "01" => 
					when "10" =>
					when "11" => 
					when others => null;
				end case;
			end if;
		end if;
	end process pRegRd;



end comp;
