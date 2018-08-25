library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity display is
port (
       clk : in std_logic;
         seg : in std_logic_vector(6 downto 5);  --BCD input
         segment4 : out std_logic_vector(3 downto 0)  -- 7 bit decoded output.
     );
end entity;

architecture Behavioral of display is

begin
process (seg)
BEGIN

case  seg is
when "00"=> segment4 <="0001";  -- 'En0' this determines which seven segment the data is displayed on
when "01"=> segment4 <="0010";  -- 'En1'
when "10"=> segment4 <="0100";  -- 'En2'
when "11"=> segment4 <="1000";  -- 'En3'

 
when others=> segment4 <="0000"; 
end case;


end process;

end Behavioral;