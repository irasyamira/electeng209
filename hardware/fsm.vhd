library ieee; 
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

	port (clk: in std_logic;
			rx: in std_logic;
			reset : in std_logic;
			CMP7_N : in std_logic;
			CMP15_S : in std_logic;
			CMP7_S : in std_logic;
			EN_N : out std_logic;
			EN_S : out std_logic;
			RESET_S : out std_logic;
			RESET_N : out std_logic;
			EN_SHIFT : out std_logic
);
			
end group27;
			
Architecture mix of group27 is

type state is (idle, start, data, stop);
signal CS, NS : state; 


begin

Synchronous_process: process (reset, clk)
	begin
		if clk'event and clk='1' then
			if reset  = '1' then			--must check this again bcs not sure?????
				CS <= idle;
			else
				CS <= NS;
			end if;
		end if;
	end process;
	

NextState_logic: process (CS, rx, CMP7_N, CMP7_S, CMP15_S)
	begin	
case CS is 
	when idle =>							--When in idle state
		if rx = '0' then
			NS <= start;
		elsif rx = '1' then				--If rx = 1, back to idle state regardless of RESET_S
			NS <= idle;
		else
			NS <= idle;
		end if;
	when start =>							--When in start state
		if CMP7_S = '1' then
			NS <= data;
		elsif CMP7_S = '0' then
			NS <= start;
		else
			NS <= idle;
		end if;
	when data =>							--When in data state
		if (CMP15_S = '1' and CMP7_N = '1') then
			NS <= stop;
		elsif (CMP15_S = '1' and CMP7_N = '0') then
			NS <= data;
		elsif CMP15_S = '0' then
			NS <= data;
		else
			NS <= idle;
		end if;
	when stop =>							--When in stop state
		if (CMP15_S = '0') then
			NS <= stop;
		else
			NS <= idle;
		end if;
end case;
end process;
		
Output_logic: process (CS, rx, CMP7_N, CMP7_S, CMP15_S)
	begin
		case CS is
				when idle => 
					if rx = '0' then
						RESET_S <= '1'; --RESET_S = 1 means resetting S=0 *not S=1
					else
						RESET_S <= '0';
					end if;
				when start =>
					if CMP7_S = '1' then
						RESET_S <= '1';
						RESET_N <= '0';
					elsif CMP7_S = '0' then
						EN_S <= '1';
						EN_Shift <= '0';
					else 
						RESET_S <= '0';
					end if;
				when data =>
					if CMP15_S = '1' and CMP7_N = '1' then
						RESET_S <= '1';
						EN_Shift <= '0';
					elsif CMP15_S = '0' then
						EN_S <= '1';
					elsif CMP15_S = '1' and CMP7_N = '0' then
						RESET_S <= '1';
						EN_SHIFT <= '1';
						EN_N <= '1';
					end if;
				when stop =>
					if (CMP15_S = '1') then
						EN_Shift <= '0';
					end if;
			end case;
		end process;
end architecture;