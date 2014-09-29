library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.math_real.all;

entity clk_div is
	
	port (
		clk_in : in  std_logic;
		clk_out : out std_logic; 
		rst      : in  std_logic);
	end clk_div;

architecture BHV of clk_div is
begin
	process(clk_in, rst)
		variable CYCLES : integer := 150;
		variable counter: integer := 0;
		variable clk 	: std_logic;
	begin
		if(rising_edge(clk_in)) then 
			counter := counter + 1;
			if(counter = 1) then 
				clk := '0';
			elsif(counter = CYCLES) then 
				clk := '1'; 
				counter := 0;
			end if;
		end if;
		clk_out <= clk;
	end process;
	
end BHV;