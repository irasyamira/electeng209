library ieee; 
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity S_counter is
  
  port(clk, rst_s: in std_logic;
				rst_n , EN_S, EN_N : in std_logic;
				s : out std_logic_vector(3 downto 0);
				n : out std_logic_vector(3 downto 0));	
	
end S_counter; 

architecture archi of S_counter is 
  signal tmpS,tmpN: std_logic_vector(3 downto 0); 
  begin  
    process (clk) 
      begin 
			if (clk'event and clk='1') then 
				if (rst_s='1')  then
					tmpS <= "0000";
					elsif EN_S = '1' then
						tmpS <= tmpS + 1;
				end if;
			end if;
		  
		 if (clk'event and clk='1') then 
			if (rst_n='1')  then
				tmpN <= "0000";
			elsif EN_N = '1' then	
            tmpN <= tmpN + 1; 
        end if;
		 end if; 
		  
    end process; 
    s <= tmpS;
	 n <= tmpN; 
end archi; 