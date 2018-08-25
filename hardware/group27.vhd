library ieee; 
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity group27 is
	port (clk: in std_logic;
			rx: in std_logic;
			CMP7_N : in std_logic;
			CMP15_S : in std_logic;
			CMP7_S : in std_logic;
			EN_N : out std_logic;
			EN_S : out std_logic;
			RESET_S : out std_logic;
			RESET_N : out std_logic;
			EN_SHIFT : out std_logic
			--states : out std_logic
);
			
end group27;
			
Architecture mix of group27 is

type state is (idle, start, data, stop);
signal CS, NS : state; 


begin

Synchronous_process: process (CS,clk)
	begin
		if clk'event and clk = '1' then
			CS <= NS;
		else
			CS <= CS;
		end if;	
	end process;
	

NextState_logic: process (CS, rx, CMP7_N, CMP7_S, CMP15_S)
	begin
		NS<=idle;
case CS is 
	when idle =>							--When in idle state
		if rx = '0' then              -- If rx is 0 then go into start state
			NS <= start;
		else
			NS <= idle;
		end if;
	--states <= "00" ;
	
	when start =>			            --When in start state
		if CMP7_S = '1' then          --If S counter equals to 7 then CMP7_S = 1 indicating is at middle of start state
			NS <= data;
		else
			NS <= start;
		end if;
	--states <= "01" ;
	
	when data =>							--When in data state
		if (CMP15_S = '1' and CMP7_N = '1') then --Goes into stop state because have stored 7 bits 
			NS <= stop;
		elsif (CMP15_S = '1' and CMP7_N = '0') then --Still stores data because have not reached 7 (CMP7_N)
			NS <= data;
		else
			NS <= data;
		end if;
	--states <= "10" ;
	
	when stop =>							--When in stop state
		if (CMP15_S = '1') then
			NS <= idle;
		else
			NS <= stop;
		end if;
		
	--states <= "11" ;
	
end case;
end process;
		
Output_logic: process (CS, rx, CMP7_N, CMP7_S, CMP15_S)
	begin
					RESET_S<='0';      --initialize all to 0
					RESET_N<='0';
					EN_S<='0';
					EN_N<='0';
					EN_SHIFT<='0';
					
		case CS is
				when idle => 
					if rx = '0' then   --if goes into start state, then S counter starts counting
						RESET_S <= '1'; --RESET_S = 1 means resetting S=0 *not S=1
					else
						RESET_S <= '0';
					end if;
					
				when start =>
					if CMP7_S = '1' then --start counting from 0 again when counter reaches 7
						RESET_S <= '1';
						RESET_N <= '1';
					else 
						EN_S <= '1';
					end if;
					
				when data =>        
					if CMP15_S = '1' and CMP7_N = '1' then  --starts counting from 0 again when at the middle of the bit
						RESET_S <= '1';
						EN_SHIFT <= '1';
						RESET_N <= '1';
					elsif CMP15_S = '1' and CMP7_N = '0' then
						RESET_S <= '1';
						EN_SHIFT <= '1';
						EN_N <= '1';
						
					else
						EN_S <= '1';
						
					end if;
					
				when stop =>
					if (CMP15_S = '1') then
						RESET_S <= '1';
						else 
						  EN_S <= '1';
					end if;
					
			end case;
		end process;
end architecture;