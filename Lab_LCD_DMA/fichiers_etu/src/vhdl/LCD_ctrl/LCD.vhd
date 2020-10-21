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
		 Waitrequest : OUT std_logic;
		 
		 
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

	type Etat is (idle, write1, write2, write3 );
	signal Etat_present, Etat_futur : Etat := idle;
	signal start_send_s 			: std_logic := '0';

	signal reset_LCD_s 			: std_logic := '0'; -- LCD_RESETn	(par défaut pas de reset)
	signal mode_LCD_s 			: std_logic := '0'; -- IM0			(par défaut mode 16 bits)
	signal cs_n_LCD_s 			: std_logic := '0'; -- LCD_CSn 		(par défaut actif)
	
	-- Data or Cmd ?
	signal DCn_s	 			: std_logic; -- LCD_D_Cn 
	
	signal dataSend_s			: std_logic_vector (15 DOWNTO 0);


  BEGIN
  
  LCD_RESETn <= not reset_LCD_s;
  IM0 <= mode_LCD_s;
  LCD_CSn <= cs_n_LCD_s;
  LCD_D_Cn <= DCn_s;
  LCD_DATA <= dataSend_s;
  LCD_RDn <= '1';

-- PROCESSUS D'ECRITURE --
pRegWr:
	process(Clk, nReset)
	begin
		if nReset = '0' then
			reset_LCD_s <= '0'; -- Input by default
			cs_n_LCD_s <= '0';
			DCn_s <= '0';
			mode_LCD_s <= '0';
			start_send_s <= '0';
			dataSend_s <= (others => '0');

		elsif rising_edge(Clk) then
		
			start_send_s <= '0';
		
			if ChipSelect = '1' and Write = '1' then -- Write cycle
				case Address(1 downto 0) is
				
					-- CONTROLE REGISTRE
					when "00" => 
						cs_n_LCD_s 	<= WriteData(0);
						mode_LCD_s  <= WriteData(1);
					
					-- SEND COMMAND
					when "01" => 
						DCn_s <= '0';
						dataSend_s <= WriteData;
						start_send_s <= '1';
						
					-- SEND DATA
					when "10" => 
						DCn_s <= '1';
						dataSend_s <= WriteData;
						start_send_s <= '1';
						
					-- CONTROLE RESET
					when "11" => 
						reset_LCD_s <= WriteData(0);
					
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
						ReadData(0) <= cs_n_LCD_s;
						ReadData(1) <= mode_LCD_s;
					
					when "01" => 
					when "10" =>
					when "11" => 
						ReadData(0) <= reset_LCD_s;
					when others => null;
				end case;
			end if;
		end if;
	end process pRegRd;


-- Reset ou met à jour l'état présent
Sequentiel_maj_etat: 
	process (Clk, nReset)
	begin
		if (nReset = '0') then
			Etat_Present <= idle;
		elsif rising_edge(Clk) then
			Etat_Present <= Etat_Futur;
		end if;
	end process Sequentiel_maj_etat;
	
-- Logique de la machine d'état
-- OUT : LCD_WRn / Etat_Futur / Waitrequest <= '0';
Combinatoire_machine :
	process (Etat_present, start_send_s )
	begin
		Etat_Futur <= idle;
		LCD_WRn <= '1';
		Waitrequest <= '0';
		
		case Etat_Present is
		
			when idle =>
				LCD_WRn <= '1';
				Waitrequest <= '0';
				if (start_send_s = '1') then
					Etat_Futur <= write1;
				else 
					Etat_Futur <= idle;
				end if;
				
			when write1 =>
				LCD_WRn <= '0';
				Waitrequest <= '1';
				Etat_Futur <= write2;
				
			when write2 =>
				LCD_WRn <= '0';
				Waitrequest <= '1';
				Etat_Futur <= write3;
				
			when write3 =>
				LCD_WRn <= '0';
				Waitrequest <= '1'; -- ou à 0... !
				if (start_send_s = '1') then
					Etat_Futur <= write1;
				else 
					Etat_Futur <= idle;
				end if;
				
			when others =>
				Etat_Futur <= idle;
				LCD_WRn <= '1';
			end case ;
	
	
	end process Combinatoire_machine;

end comp;
