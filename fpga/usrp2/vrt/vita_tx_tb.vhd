--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   00:08:18 11/12/2013
-- Design Name:   
-- Module Name:   C:/TEST/Test_firmware/UHD-Fairwaves_last_dual/fpga/usrp2/top/N2x0/build_UmTRXv2/vita_tx_tb.vhd
-- Project Name:  u2plus_umtrx
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: vita_tx_chain
-- 
-- Dependencies:
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--
-- Notes: 
-- This testbench has been automatically generated using types std_logic and
-- std_logic_vector for the ports of the unit under test.  Xilinx recommends
-- that these types always be used for the top-level I/O of a design in order
-- to guarantee that the testbench will bind correctly to the post-implementation 
-- simulation model.
--------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;
 
ENTITY vita_tx_tb IS
END vita_tx_tb;
 
ARCHITECTURE behavior OF vita_tx_tb IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT vita_tx_chain
GENERIC(
      BASE_CTRL : INTEGER;
      BASE_DSP : INTEGER;
      REPORT_ERROR : INTEGER;
      DO_FLOW_CONTROL : INTEGER;
      PROT_ENG_FLAGS : INTEGER;
      USE_TRANS_HEADER : INTEGER;
      DSP_NUMBER : INTEGER
      );
    PORT(
         clk : IN  std_logic;
         reset : IN  std_logic;
         dac_clk : IN  std_logic;
         set_stb : IN  std_logic;
         set_addr : IN  std_logic_vector(7 downto 0);
         set_data : IN  std_logic_vector(31 downto 0);
         vita_time : IN  std_logic_vector(63 downto 0);
         tx_data_i : IN  std_logic_vector(35 downto 0);
         tx_src_rdy_i : IN  std_logic;
         tx_dst_rdy_o : OUT  std_logic;
         err_data_o : OUT  std_logic_vector(35 downto 0);
         err_src_rdy_o : OUT  std_logic;
         err_dst_rdy_i : IN  std_logic;
         tx_i : OUT  std_logic_vector(23 downto 0);
         tx_q : OUT  std_logic_vector(23 downto 0);
         underrun : OUT  std_logic;
         run : OUT  std_logic;
         debug : OUT  std_logic_vector(31 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal clk : std_logic := '0';
   signal reset : std_logic := '0';
   signal dac_clk : std_logic := '0';
   signal set_stb : std_logic := '0';
   signal set_addr : std_logic_vector(7 downto 0) := (others => '0');
   signal set_data : std_logic_vector(31 downto 0) := (others => '0');
   signal vita_time : std_logic_vector(63 downto 0) := (others => '0');
   signal tx_data_i : std_logic_vector(35 downto 0) := (others => '0');
   signal tx_src_rdy_i : std_logic := '0';
   signal err_dst_rdy_i : std_logic := '0';

 	--Outputs
   signal tx_dst_rdy_o : std_logic;
   signal err_data_o : std_logic_vector(35 downto 0);
   signal err_src_rdy_o : std_logic;
   signal tx_i : std_logic_vector(23 downto 0);
   signal tx_q : std_logic_vector(23 downto 0);
   signal underrun : std_logic;
   signal run : std_logic;
   signal debug : std_logic_vector(31 downto 0);

   -- Clock period definitions
   constant clk_period : time := 10 ns;
   constant dac_clk_period : time := 80 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: vita_tx_chain 
   GENERIC MAP (BASE_CTRL=>126,
     BASE_DSP=>135,
     REPORT_ERROR=>1,
     DO_FLOW_CONTROL=>1,
     PROT_ENG_FLAGS=>1,
     USE_TRANS_HEADER=>1,
     DSP_NUMBER=>0
   )
   
   PORT MAP (
          clk => clk,
          reset => reset,
          dac_clk => dac_clk,
          set_stb => set_stb,
          set_addr => set_addr,
          set_data => set_data,
          vita_time => vita_time,
          tx_data_i => tx_data_i,
          tx_src_rdy_i => tx_src_rdy_i,
          tx_dst_rdy_o => tx_dst_rdy_o,
          err_data_o => err_data_o,
          err_src_rdy_o => err_src_rdy_o,
          err_dst_rdy_i => err_dst_rdy_i,
          tx_i => tx_i,
          tx_q => tx_q,
          underrun => underrun,
          run => run,
          debug => debug
        );

   -- Clock process definitions
   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
   end process;
 
   dac_clk_process :process
   begin
		dac_clk <= '0';
		wait for dac_clk_period/2;
		dac_clk <= '1';
		wait for dac_clk_period/2;
   end process;
 

   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
      reset <= '1';
      wait for 100 ns;	

      wait for dac_clk_period*10;

reset <= '0';


tx_src_rdy_i <= '1';
set_addr <= x"88"; --BASE_DSP+1
set_data <=  x"0001" & x"0001"; --scale_i,scale_q
set_stb <= '0'; wait for dac_clk_period;
      -- insert stimulus here 
set_stb <= '1'; wait for dac_clk_period;
set_stb <= '0';

set_addr <=x"89"; ----BASE_DSP+2
set_data <= "0000000000000000000000" & "00" & x"01"; --enable_hb1, enable_hb2, interp_rate
set_stb <= '0'; wait for dac_clk_period;
      -- insert stimulus here 
set_stb <= '1'; wait for dac_clk_period;
set_stb <= '0';
      wait;
   end process;
tx_data_i <= x"000000000",  x"000000001" after (dac_clk_period*12+dac_clk_period/2);
END;
