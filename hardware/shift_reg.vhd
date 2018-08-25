library ieee; 
use ieee.std_logic_1164.all; 
 
entity shift_reg is 
port(C, SI, en_shift : in std_logic; 
      PO : out std_logic_vector(7 downto 0)); 
end shift_reg;
 
architecture mix of shift_reg is 
  signal tmp: std_logic_vector(7 downto 0); 
  begin 
    process (C) 
      begin 
        if (C'event and C='1') then --to store the bit 
				if en_shift = '1' then 
				  tmp <= SI & tmp(7 downto 1); 
            end if;
			 end if;
    end process; 
    PO <= tmp; 
end mix; 