library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity user_logic is
   Port ( clk : in STD_LOGIC;
          rstN : in STD_LOGIC;
          wren : in STD_LOGIC;
          waddr : in STD_LOGIC_VECTOR (1 downto 0);
          wdata : in STD_LOGIC_VECTOR (31 downto 0);
          rden : in STD_LOGIC;
          raddr : in STD_LOGIC_VECTOR (1 downto 0);
          rdata : out STD_LOGIC_VECTOR (31 downto 0);
          busy : out STD_LOGIC);
end user_logic;

architecture slow_arch of user_logic is
   type state_type is (idle, swap, sub, start);
   signal state_reg, state_next: state_type;
   signal a_reg, a_next, b_reg, b_next: unsigned(31 downto 0);
   --signal wren_current : STD_LOGIC := '0';
   signal gcd_current : unsigned(31 downto 0);
begin
    -- state & data registers
   process(clk,rstN, wdata,a_next,b_next,wren)
   begin
      if rstN='0' then
         state_reg <= idle;
         b_reg <= "00000000000000000000000000000000";
         a_reg <= "00000000000000000000000000000000";
      end if;
   elsif (clk'event and clk='1') then
      state_reg <= state_next;
      a_reg <= a_next;
      b_reg <= b_next;
   end if;
end process;

   -- next-state logic & data path functional units/routing
process(state_reg)
begin
   busy <= '1';
   case state_reg is
      when start =>
         a_next <= unsigned(wdata);
         b_next <= b_reg;
         state_next <= sub;
      when idle =>
         if wren='1' then
            state_next <= start;
         else
            state_next <= idle;
            busy <= '0';
         end if;
      when swap =>
         if (a_reg < b_reg) then
            a_next <= b_reg;
            b_next <= a_reg;
         end if;
         state_next <= sub;
      when sub =>
         --if (a_reg = b_reg) then
         --   state_next <= idle;
         --   a_next <= a_reg;
         --   b_next <= b_reg;
         --else
         --   a_next <= a_reg - b_reg;
         --   state_next <= swap;
         --end if;
		 state_next <= idle;
   end case;
end process;
   -- output
busy <= '0' when state_reg=idle else '1';
rdata <= std_logic_vector(a_reg);
end slow_arch;

