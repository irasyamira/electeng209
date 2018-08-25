library IEEE;
use IEEE.STD_LOGIC_1164.all;

entity N_CMP7 is
     port(
         a : in STD_LOGIC_VECTOR(3 downto 0);
         --b : in STD_LOGIC_VECTOR(3 downto 0);
         equal : out STD_LOGIC
         );
end N_CMP7;


architecture mix of N_CMP7 is
begin

    comparator : process (a) is
    begin
			if (a= "0111") then  --to determine if counter has reached 7
            equal <= '1';
			else
            equal <= '0';
        end if;
    end process comparator;

end mix;