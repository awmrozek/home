git status
git add Documents/ Pictures/ kol*
git add Documents/ Pictures/ ko1
git add Documents/ Pictures/ ko1.m
git status
git pull --rebase
git commit -m "Creted ko1 direct"
git pull --rebase
vim .gitignore 
git statis
git status
git add .
git commit -m "added hidden directories as git wishes"
git pull --rebase
git status
ls *7z
df -h
mkdir tmp
man unzip
unzip project_4.zip -d tmp/
cd tmp/
ls
find . -iname "
find . -iname "*.C"
find . -iname "*.C" |less
find . -iname "*.C" |head -n 1
vim `find . -iname "*.C" |head -n 1`
find . | grep -i gcd
cd ..
rm -rf tmp/*
git log
git diff fe308c7ed
git checkout fe308c7ed
vim .gitignore 
vim .git/info/exclude
cd
git add .gitignore 
gnore
git add .gitignore 
ls -lart
ls -alrt
man 7z
7z
p7zip 
man p7zip
p7zip -d project_4.7z tmp/
p7zip -d project_4.7z 
rm -rf tmp
cd project_4/
ls -lart
grep -rin putfsl *
cd project_4.sdk/
ls
grep -rin putfsl * |less
cd ..
find . -iname core1
cd project_4.sdk/core
ls
cd project_4.sdk/core1
ls
cd src/
ls
grep pop *
ls
vim helloworld.c 
ls -lart
cd ..
cd ..
cd ..
grep -rin putfsl * |less
vf ..
cd ..
rm -rf project_4
rm -rf project_4
ls -lart
mcedit
nano allwewrote.txt
git add allwewrote.txt w
git add allwewrote.txt 
git commit -m "Created allwewrote.txt where we keep all we produced ourselvesss"
git status
git checkout project_4.7z
git add Pictures/panther.PNG 
git status
git ignore .bash_history
git push origin master
git add allwewrote.txt 
git commit -m "core communication not yet working"
git push origin master
ls -alr
ls *m
cd Downloads/
ls
ls *zip
git add .
git commit -m "Polskie piosenki z chomika dodano"
cd Windows/
ls
cd Desktop/
ls
cd ..
cd ..
find . -iname "*.MP3"
cd Downloads/
ls
pwd
move TestBench1.txt TestBench1.vhd
mv TestBench1.txt TestBench1.vhd
exit
cd
ls -lart *c
cd git/
ls -alrt
cd edaf15/
ls -alrt
git pull --rebase
git pull
ls- lart
ls
cd ..
ls- lart
ls -alrt
cd edaf15/
ls
sl
ls
cd /usr/local/cs/
ls
cd EDAF15/
ls
xeyes
xeyes
git pull
alsamixer
matlab
cd Downloads/
ls -lart
ls -lart
cat allwewrote.txt 
cat allwewrote.txt 
;
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
   type state_type is (idle, swap, sub);
   signal state_reg, state_next: state_type;
   signal a_reg, a_next, b_reg, b_next: unsigned(31 downto 0);
   --signal wren_current : STD_LOGIC := '0';
   signal gcd_current : unsigned(31 downto 0);
begin
   -- state & data registers
   process(clk,rstN, wdata,a_next,b_next)
   begin
      if rstN='0' then;          state_reg <= idle;                    b_reg <= unsigned(wdata);
         a_reg <= unsigned(wdata);
      elsif (clk'event and clk='1') then
         state_reg <= state_next;
         a_reg <= a_next;
         b_reg <= b_next;
      end if;
   end process;
   
   -- next-state logic & data path functional units/routing
   process(state_reg,a_reg,b_reg,wren,wdata)
   begin
      busy <= '1';
      --a_next <= a_reg;
      --b_next <= b_reg;
      case state_reg is
         when idle =>
            if wren='1' then -- and wren_current='0' then
               -- if user enters a == b
               busy <= '0';
               a_next <= unsigned(wdata);
               
               state_next <= swap;
               if (a_next = b_next) then
                 state_next <= idle;
               end if;
            else
               state_next <= idle;
               busy <= '0';
            end if;
         when swap =>
            if (a_reg=b_reg) then
               state_next <= idle;
               --wren_current <= '0';
            else
               if (a_reg < b_reg) then
                  a_next <= b_reg;
                  b_next <= a_reg;
               end if;
               state_next <= sub;
            end if;
         when sub =>
            if (a_reg = b_reg) then
                state_next <= idle;
            else
                a_next <= a_reg - b_reg;
                state_next <= swap;
            end if;
         end case;
   end process;
   -- output
   busy <= '0' when state_reg=idle else '1';
   rdata <= std_logic_vector(a_reg);
end slow_arch;

--architecture Behavioral of user_logic is
--    type STATE_TYPE is (S0, S1, S2, S3);
--    --attribute ENUM_ENCODING: STRING;
--    --attribute ENUM_ENCODING of STATE_TYPE: type is "0001 0010 0100 1000";
    
--    signal CS, NS: STATE_TYPE;
--    signal reset : STD_LOGIC;
--begin
--    -- insert the following after 'begin'
--    reset <= not rstN;
--    busy <= '0';
    
--    SYNC_PROC: process(clk, reset)
--    begin
--        if (reset = '1') then
--            CS <= S0;
--            -- other state variables reset
--        elsif rising_edge(clk) then
--            CS <= NS;
--            -- other state variable assignment
--        end if;
--    end process;
    
--    COMB_PROC: process(CS, wdata)
--    begin
--        -- assign default signals here to avoid latches
--    case CS is
--        when S0 =>
--            NS <= S1;
--            rdata <= wdata;
--        when S1 =>
--            NS <= S2;
--            rdata <= wren & rden  & "0000000000000000000000000000000";
--        when S2 =>
--            NS <= S3;
--            rdata <= wdata;
--        when S3 =>
--            NS <= S0;
--            rdata <= wdata;
--    end case;
--    end process;

--end Behavioral;
;
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
   type state_type is (idle, swap, sub);
   signal state_reg, state_next: state_type;
   signal a_reg, a_next, b_reg, b_next: unsigned(31 downto 0);
   --signal wren_current : STD_LOGIC := '0';
   signal gcd_current : unsigned(31 downto 0);
begin
   -- state & data registers
   process(clk,rstN, wdata,a_next,b_next)
   begin
      if rstN='0' then
         state_reg <= idle;
         
         b_reg <= unsigned(wdata);
         a_reg <= unsigned(wdata);
      elsif (clk'event and clk='1') then
         state_reg <= state_next;
         a_reg <= a_next;
         b_reg <= b_next;
      end if;
   end process;
   
   -- next-state logic & data path functional units/routing
   process(state_reg,a_reg,b_reg,wren,wdata)
   begin
      busy <= '1';
      --a_next <= a_reg;
      --b_next <= b_reg;
      case state_reg is
         when idle =>
            if wren='1' then -- and wren_current='0' then;                -- if user enters a == b;                busy <= '0';                a_next <= unsigned(wdata);
               
               state_next <= swap;
               if (a_next = b_next) then                  state_next <= idle;                end if;             else                state_next <= idle;                busy <= '0';             end if;          when swap =>
            if (a_reg=b_reg) then                state_next <= idle;                --wren_current <= '0';             else                if (a_reg < b_reg) then                   a_next <= b_reg;                   b_next <= a_reg;                end if;                state_next <= sub;             end if;          when sub =>
            if (a_reg = b_reg) then                 state_next <= idle;             else                 a_next <= a_reg - b_reg;                 state_next <= swap;             end if;          end case;    end process;    -- output;    busy <= '0' when state_reg=idle else '1';    rdata <= std_logic_vector(a_reg);
end slow_arch;
--architecture Behavioral of user_logic is
--    type STATE_TYPE is (S0, S1, S2, S3);
--    --attribute ENUM_ENCODING: STRING;
--    --attribute ENUM_ENCODING of STATE_TYPE: type is "0001 0010 0100 1000";
    
--    signal CS, NS: STATE_TYPE;
--    signal reset : STD_LOGIC;
--begin
--    -- insert the following after 'begin'
--    reset <= not rstN;
--    busy <= '0';
    
--    SYNC_PROC: process(clk, reset)
--    begin
--        if (reset = '1') then
--            CS <= S0;
--            -- other state variables reset
--        elsif rising_edge(clk) then
--            CS <= NS;
--            -- other state variable assignment
--        end if;
--    end process;
    
--    COMB_PROC: process(CS, wdata)
--    begin
--        -- assign default signals here to avoid latches
--    case CS is
--        when S0 =>
--            NS <= S1;
--            rdata <= wdata;
--        when S1 =>
--            NS <= S2;
--            rdata <= wren & rden  & "0000000000000000000000000000000";
--        when S2 =>
--            NS <= S3;
--            rdata <= wdata;
--        when S3 =>
--            NS <= S0;
--            rdata <= wdata;
--    end case;
--    end process;
--end Behavioral;
;
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
   type state_type is (idle, swap, sub);
   signal state_reg, state_next: state_type;
   signal a_reg, a_next, b_reg, b_next: unsigned(31 downto 0);
   --signal wren_current : STD_LOGIC := '0';
   signal gcd_current : unsigned(31 downto 0);
begin
   -- state & data registers
   process(clk,rstN, wdata,a_next,b_next)
   begin
      if rstN='0' then;          state_reg <= idle;                    b_reg <= unsigned(wdata);
         a_reg <= unsigned(wdata);
      elsif (clk'event and clk='1') then
         state_reg <= state_next;
         a_reg <= a_next;
         b_reg <= b_next;
      end if;
   end process;
   
   -- next-state logic & data path functional units/routing
   process(state_reg,a_reg,b_reg,wren,wdata)
   begin
      busy <= '1';
      --a_next <= a_reg;
      --b_next <= b_reg;
      case state_reg is
         when idle =>
            if wren='1' then -- and wren_current='0' then
               -- if user enters a == b
               busy <= '0';
               a_next <= unsigned(wdata);
               
               state_next <= swap;
               if (a_next = b_next) then
                 state_next <= idle;
               end if;
            else
               state_next <= idle;
               busy <= '0';
            end if;
         when swap =>
            if (a_reg=b_reg) then
               state_next <= idle;
               --wren_current <= '0';
            else
               if (a_reg < b_reg) then
                  a_next <= b_reg;
                  b_next <= a_reg;
               end if;
               state_next <= sub;
            end if;
         when sub =>
            if (a_reg = b_reg) then
                state_next <= idle;
            else
                a_next <= a_reg - b_reg;
                state_next <= swap;
            end if;
         end case;
   end process;
   -- output
   busy <= '0' when state_reg=idle else '1';
   rdata <= std_logic_vector(a_reg);
end slow_arch;

--architecture Behavioral of user_logic is
--    type STATE_TYPE is (S0, S1, S2, S3);
--    --attribute ENUM_ENCODING: STRING;
--    --attribute ENUM_ENCODING of STATE_TYPE: type is "0001 0010 0100 1000";
    
--    signal CS, NS: STATE_TYPE;
--    signal reset : STD_LOGIC;
--begin
--    -- insert the following after 'begin'
--    reset <= not rstN;
--    busy <= '0';
    
--    SYNC_PROC: process(clk, reset)
--    begin
--        if (reset = '1') then
--            CS <= S0;
--            -- other state variables reset
--        elsif rising_edge(clk) then
--            CS <= NS;
--            -- other state variable assignment
--        end if;
--    end process;
    
--    COMB_PROC: process(CS, wdata)
--    begin
--        -- assign default signals here to avoid latches
--    case CS is
--        when S0 =>
--            NS <= S1;
--            rdata <= wdata;
--        when S1 =>
--            NS <= S2;
--            rdata <= wren & rden  & "0000000000000000000000000000000";
--        when S2 =>
--            NS <= S3;
--            rdata <= wdata;
--        when S3 =>
--            NS <= S0;
--            rdata <= wdata;
--    end case;
--    end process;

--end Behavioral;

grep -rin power .bashrc 
ssh dat14amr@power.cs.lth.se
ssh dat14amr@power.cs.lth.se
cd .ssh/
ls
vim authorized_keys 
cd .ssh/
ls -lart
cat authorized_keys 
cat id_rsa.pub 
ls -lart
ssh dat14amr@power.cs.lth.se
ssh dat14amr@power.cs.lth.se
ssh dat14amr@power.cs.lth.se
ssh -XC dat14amr@power.cs.lth.se
ssh dat14amr@power.cs.lth.se -XC
matlab
cat > source.vhd
vim source.vhd 
texmaker
