-----------------------------------------------------------------------
--
-- File         : LCD.vhd
-- Author       : Spinelli Isaia
-- Description  : Controller LCD 16 bits (LCD.vhd),
-- used for     : T-EmbHardw lab
-- avalon master > Szmbol / 8 / 1 wait / o lat / words
--| Modifications |-----------------------------------------------------------
-- Ver  Date       Auteur  Description
------------------------------------------------------------------------------

library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;
    use ieee.std_logic_unsigned.all; -- addition std_logic_vector

ENTITY ParallelPort IS
	 PORT(
		-- Avalon interfaces signals
		 Clk 		: IN std_logic;
		 nReset 	: IN std_logic;
		 Address 	: IN std_logic_vector (2 DOWNTO 0);
		 ChipSelect : IN std_logic;
		 Read 		: IN std_logic;
		 Write 		: IN std_logic;
		 ReadData	: OUT std_logic_vector (31 DOWNTO 0);
		 WriteData 	: IN std_logic_vector (31 DOWNTO 0);
		 Waitrequest : OUT std_logic;

		-- master interface
 		master_address	  : out  std_logic_vector(31 downto 0);
		master_read	  : out std_logic;
		master_readdata	  : in std_logic_vector(15 downto 0) ;
		master_waitrequest : in std_logic;
		-- IRQ generation
		end_of_transaction_irq : out std_logic;	
		 
		 
		-- LCD external interface
		LCD_RESETn 	: OUT std_logic;
		LCD_CSn 	: OUT std_logic;
		LCD_D_Cn 	: OUT std_logic;
		LCD_WRn 	: OUT std_logic;
		LCD_RDn 	: OUT std_logic;
		IM0 		: OUT std_logic;
		
		LCD_DATA 	: OUT std_logic_vector (15 DOWNTO 0)
	 );
End ParallelPort;
-------- register model (a proposal) -----------
-- 000 controle LCD (cs & mode)
-- 001 write command to LCD
-- 010 write data to LCD
-- 011 reset LCD
-- 100 write pointer of the image to copy
-- 101 write size of the image to copy
-- 110 control register 
--	bit 0 => start transfer 
--	bit 1 => reserved 
--	bit 2 => IRQ ack 

ARCHITECTURE comp OF ParallelPort IS

	type Etat is (idle, write1, write2, write3 );
	signal Etat_present, Etat_futur : Etat := idle;
	signal start_send_s 			: std_logic := '0';

	type EtatDMA is (idle1, rd1, wrLCD, incAddr, wait1, irq );
	signal Etat_present_dma, Etat_futur_dma : EtatDMA := idle1;

	signal Etat_present_dma_s		: std_logic_vector (2 DOWNTO 0) := "000";

	signal reset_LCD_s 			: std_logic := '0'; -- LCD_RESETn	(par défaut pas de reset)
	signal mode_LCD_s 			: std_logic := '0'; -- IM0			(par défaut mode 16 bits)
	signal cs_n_LCD_s 			: std_logic := '0'; -- LCD_CSn 		(par défaut actif)
	
	-- Data or Cmd ?
	signal DCn_s	 			: std_logic; -- LCD_D_Cn 
	signal Waitrequest_s	 		: std_logic := '0';
	
	signal dataSend_s			: std_logic_vector (15 DOWNTO 0);

	signal dma_ptr_img			: std_logic_vector (31 DOWNTO 0);
	signal dma_size_img			: std_logic_vector (31 DOWNTO 0);
	


	signal dma_start_transfer 		: std_logic := '0';
	signal dma_irq_ack 			: std_logic := '0';
	signal dma_irq	 			: std_logic := '0';

	signal start_MSS_LCD_s 			: std_logic := '0';
	signal MUX_data_LCD_s 			: std_logic := '0';
	signal dataSend_dma_s			: std_logic_vector (15 DOWNTO 0);


	-- Gestion du compteur
	signal en_cpt_s 			: std_logic := '0'; 
	signal load_cpt_s 			: std_logic := '0'; 
	signal count_s      			: unsigned(31 downto 0) := (others => '0');
	signal count_next_s 			: unsigned(31 downto 0) := (others => '0');
	signal cpt_s				: std_logic_vector (31 DOWNTO 0) := (others => '0');

	-- regsitres
	signal save_info 			: std_logic := '0'; 
	signal read_data_dma 			: std_logic := '0';
	signal dma_ptr_img_mem			: std_logic_vector (31 DOWNTO 0);
	signal dma_size_img_mem			: std_logic_vector (31 DOWNTO 0);
	

  BEGIN
  
  LCD_RESETn <= not reset_LCD_s;
  IM0 <= mode_LCD_s;
  LCD_CSn <= cs_n_LCD_s;
  LCD_D_Cn <= DCn_s;
  LCD_DATA <= 	dataSend_dma_s  when (MUX_data_LCD_s = '1') else
		dataSend_s;
  LCD_RDn <= '1';

  Waitrequest <= Waitrequest_s;

  end_of_transaction_irq <= dma_irq; 


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

			dma_ptr_img <= (others => '0');
			dma_size_img <= (others => '0');
			dma_start_transfer <= '0';
			dma_irq_ack <= '0';

		elsif rising_edge(Clk) then
		
			start_send_s <= '0';
			dma_irq_ack <= '0';
			dma_start_transfer <= '0';
		
			if ChipSelect = '1' and Write = '1' then -- Write cycle
				case Address(2 downto 0) is
				
					-- CONTROLE REGISTRE
					when "000" => 
						cs_n_LCD_s 	<= WriteData(0);
						mode_LCD_s  <= WriteData(1);
					
					-- SEND COMMAND
					when "001" => 
						DCn_s <= '0';
						dataSend_s <= WriteData(15 DOWNTO 0);
						start_send_s <= '1';
						
					-- SEND DATA
					when "010" => 
						DCn_s <= '1';
						dataSend_s <= WriteData(15 DOWNTO 0);
						start_send_s <= '1';
						
					-- CONTROLE RESET
					when "011" => 
						reset_LCD_s <= WriteData(0);

					-- POINTER IMG
					when "100" => 
						dma_ptr_img <= WriteData;
						DCn_s <= '1';

					-- SIZE IMG
					when "101" => 
						dma_size_img <= WriteData;
						DCn_s <= '1';

					-- CONTROLE DMA
					when "110" => 
						dma_start_transfer <= WriteData(0);
						dma_irq_ack <= WriteData(1);
					
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
				case Address(2 downto 0) is
					when "000" => 
						ReadData <= x"12345678";
						ReadData(0) <= cs_n_LCD_s;
						ReadData(1) <= mode_LCD_s;
					
					when "001" => 
					when "010" =>
					when "011" => 
						ReadData(0) <= reset_LCD_s;

					-- POINTER IMG
					when "100" => 
						ReadData <= dma_ptr_img_mem;

					-- SIZE IMG
					when "101" => 
						ReadData <= dma_size_img_mem;

					-- etat dma
					when "110" => 
						ReadData(2 DOWNTO 0) <= Etat_present_dma_s;

					-- ext adr
					when "111" => 

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
	process (Etat_present, start_send_s, start_MSS_LCD_s )
	begin
		Etat_Futur <= idle;
		LCD_WRn <= '1';
		Waitrequest_s <= '0';
		
		case Etat_Present is
		
			when idle =>
				LCD_WRn <= '1';
				Waitrequest_s <= '0';
				if (start_send_s = '1' or start_MSS_LCD_s = '1') then
					Etat_Futur <= write1;
				else 
					Etat_Futur <= idle;
				end if;
				
			when write1 =>
				LCD_WRn <= '0';
				Waitrequest_s <= '1';
				Etat_Futur <= write2;
				
			when write2 =>
				LCD_WRn <= '0';
				Waitrequest_s <= '1';
				Etat_Futur <= write3;
				
			when write3 =>
				LCD_WRn <= '0';
				Waitrequest_s <= '1'; -- ou à 0... !
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


-- MSS GESTION DMA
-- Reset ou met à jour l'état présent
Sequentiel_maj_etat_dma: 
	process (Clk, nReset)
	begin
		if (nReset = '0') then
			Etat_Present_dma <= idle1;
		elsif rising_edge(Clk) then
			Etat_Present_dma <= Etat_Futur_dma;
		end if;
	end process Sequentiel_maj_etat_dma;
	
-- Logique de la machine d'état
-- OUT : master_read / master_address / dma_irq / start_MSS_LCD_s / Etat_Futur / MUX_data_LCD_s
Combinatoire_machine_dma :
	process (Etat_present_dma, dma_start_transfer, master_waitrequest)
	begin
		-- par defaut 
		Etat_present_dma_s <= "111";
		Etat_Futur_dma <= idle1;
		master_read <= '0';
		master_address <= dma_ptr_img_mem + cpt_s;
		load_cpt_s <= '0';
		en_cpt_s <= '0';

		MUX_data_LCD_s <= '0';
		start_MSS_LCD_s <= '0'; 

		dma_irq <= '0';
		save_info <= '0';

		read_data_dma <= '0';


		case Etat_Present_dma is
		
			when idle1 =>
				Etat_present_dma_s <= "000"; 
				-- enregistre les informations 
				save_info <= '1';

				-- met a 0 le compteur 
				load_cpt_s <= '1';


				if (dma_start_transfer = '1') then
					Etat_Futur_dma <= rd1;
				else 
					Etat_Futur_dma <= idle1;
				end if;
				
			when rd1 =>
				Etat_present_dma_s <= "001"; 
				-- Lecture SRAM
				master_read <= '1';
				-- mode DMA
				MUX_data_LCD_s <= '1';

				if (master_waitrequest = '1' and Etat_Futur_dma /= wrLCD ) then -- and init_rd = '0' 
					Etat_Futur_dma <= rd1;
				else
					Etat_Futur_dma <= wrLCD;
					master_read <= '0'; 
					-- demande de lire les data 
					read_data_dma <= '1';
					
				end if;

				
			when wrLCD =>
				Etat_present_dma_s <= "010";  
				Etat_Futur_dma <= incAddr;
				MUX_data_LCD_s <= '1';
				-- Start WR LCD
				start_MSS_LCD_s <= '1';
				-- incremente l addresse
				en_cpt_s <= '1';


			when incAddr =>
				Etat_present_dma_s <= "011"; 
				Etat_Futur_dma <= wait1;
				MUX_data_LCD_s <= '1';

			when wait1 =>
				Etat_present_dma_s <= "100"; 
				MUX_data_LCD_s <= '1';


				if (Waitrequest_s = '1') then -- (Si la machine d etat LCD envoie encore les data)
					Etat_Futur_dma <= wait1;
				else
					if cpt_s = (dma_size_img_mem(30 DOWNTO 0)  & '0')  then  -- 2x Nb de pixel (16 bytes)
						Etat_Futur_dma <= irq;
					else 
						Etat_Futur_dma <= rd1;
					end if;
				end if;


				
			when irq =>
				Etat_present_dma_s <= "101";
				dma_irq <= '1';

				if (dma_irq_ack = '1') then
					Etat_Futur_dma <= idle1;
				else 
					Etat_Futur_dma <= irq;
				end if;

				
			when others =>
				Etat_Futur_dma <= idle1;
				Etat_present_dma_s <= "110"; 


			end case ;
	
	
	end process Combinatoire_machine_dma;


-- Registre pour la lecture du ptr de l image et la taille
process(Clk, nReset)
begin
	if(nReset = '0') then
		dma_size_img_mem <= (others => '0');
	elsif(rising_edge(Clk)) then
		if(save_info = '1') then
			dma_size_img_mem <= dma_size_img;
			dma_ptr_img_mem <= dma_ptr_img;
		end if;
	end if;
end process;


-- Registre pour la lecture des data lu dans la SRAM
process(Clk, nReset)
begin
	if(nReset = '0') then
		dataSend_dma_s <= (others => '0');
	elsif(rising_edge(Clk)) then
		if(read_data_dma = '1') then
			dataSend_dma_s <= master_readdata;
		end if;
	end if;
end process;



-- Compteur (+2 / 16 bztes)
count_next_s <= count_s + 2;

process(Clk, nReset)
begin
	if(nReset = '0') then
		count_s <= (others => '0');
	elsif(rising_edge(Clk)) then
		if(load_cpt_s = '1') then
			count_s <= (others => '0');
		elsif (en_cpt_s = '1') then
			count_s <= count_next_s;
		end if;
	end if;
end process;

cpt_s <= std_logic_vector(count_s(31 downto 0));

end comp;
