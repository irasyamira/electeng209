library IEEE;
use IEEE.STD_LOGIC_1164.all;

entity S_CMP7 is
     port(
         a : in STD_LOGIC_VECTOR(3 downto 0);
         --b : in STD_LOGIC_VECTOR(3 downto 0);
         equal : out STD_LOGIC
         );
end S_CMP7;


architecture mix of S_CMP7 is
begin

    comparator : process (a) is
    begin
			if (a= "0111") then --to determine if counter S has reached 7
            equal <= '1';
			else
            equal <= '0';
        end if;
    end process comparator;

end mix;