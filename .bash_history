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
screen -x
screen
git add .
echo $JAVA_HMOE
echo $JAVA_HOME
ssh -XC dat14amr@power.cs.lth.se
ls -lart
cd git/
ls
cd edaf15/
ls
git pull
git pull
ls -alrt
vim .bashrc 
ssh dat14amr@power.cs.lth.se
ssh dat14amr@power.cs.lth.se
ssh dat14amr@power.cs.lth.se
ssh dat14amr@power.cs.lth.se -XC
ssh dat14amr@power.cs.lth.se -XC
ssh dat14amr@power.cs.lth.se
ls
cd Desktop/
ls /alrt
ls -alrt
cd ..
less lab6.txt 
git pull --rebase
git status
git add hwAccel1.zip 
git add lab6.txt 
git add Pictures/(
git add Pictures/*
git add Knowledge\ Base/*
git commit -m "Firing Petri and hwAccel1 bad communication"
git push origin master
git pull --rebase
git status
git add .
git commit -a
git status
git push
git pull
git commit -m "All we wrote"
git push
git pull
git push
ssh -XC dat14amr@power.cs.lth.se
ssh -XC dat14amr@power.cs.lth.se
ssh dat14amr@power.cs.lth.se
ssh dat14amr@power.cs.lth.se
vim test
cat test 
vim test
vim arrlen.c
gcc arrlen.c -o arrlen
./arrlen 
gcc arrlen.c -o arrlen
vim arrlen.c
gcc arrlen.c -o arrlen
./arrlen 
vim arrlen.c
gcc arrlen.c -o arrlen
./arrlen 
vim arrlen.c
gcc arrlen.c -o arrlen
./arrlen 
vim arrlen.c
scp -r EDAN15\ Design\ of\ Embedded\ Circuits/ amr@84.55.119.97:public_html/has/
cd git/
ls -alrt
cd edaf15/
git pull
git diff
rm -rf *
git pull
ls -lart
cd small/
l
ls
chmod -x small.c 
vim small.c 
cd ..
git pull
git stash
git pull 
cd small/
ls
vim small.c 
make
cd ..
cd small/
ls
cd ..
cd small/
ls
vim main.c 
vim small.c 
atom small.c 
make
make
make
git status
git add *.c
git commit -m "removed aPos, aNeg, etc etc"
git push
rm fm note  stats.csv 
git push
cp small.c ~
git pull --rebase
git status
git rm fm note stats.csv
git status
git pull --rebase
git reset
git reset --hard
cp ~/small.c .
git add small.c 
git commit -m "removed aPos, aNeg, etc etc"
git push
git pull --rebase
make
git log
ls -lart
diff small.c  ~/small.c 
vim small.c 
make
vim small.c 
make
ddd fm
vim small.c 
make
vim small.c 
make
vim small.c 
make
vim small.c 
git reset --hard
git pull
ls -lart
cd ..
cd small/
ls
vim small.c 
make
vim small.c 
cd ..
mc
cd fast/
cp fast.c fast.old
cd ..
cp small/small.c fast/fast.c 
cd fast/
make
 vim fast.c 
make
 vim fast.c 
make
 vim fast.c 
make
 vim fast.c 
make
vim fast.c 
make
vim fast.c 
vim main.c 
vim fast.c 
maek
make
vim fast.c 
make
cd ..
cd small/
ls
make
ls -lart
vim small.c 
git status
git add small.c 
git commit -m"upload of small.c"
git push 
git pull
cp small.c ~/small.c.old2
git pull
git push
vim small.c 
ls -lart
cd
cd edan15_PrintMe/
cd ..
ls -alrt
make
make
ssh a3
make
make
make
make
make
make
make
make
oprofile
ssh dat14amr@power.cs.lth.se
cd git/
ls -alrt
cd edaf15/
ls -alrt
git pull --rebase
git status
ls -alrt
cd ..
tar -cjvf edaf15.tar.bz2
tar -cjvf edaf15.tar.bz2 edaf15/
cd fa
cd edaf15
git stash; git pull 
cd fast/
ls -alrt
vim fast.c 
cd ..
git pull
cd fast/
make
ls -alrt
cd ..
git pull
cd fast/
ls
vim fast.
vim fast.c 
cd ..
git pull
ls -alrt
cd fast/
ls
vim fast.c 
cd ..
cd small/
vim small.c 
cp small.c ../fast/fast.c 
cd ../fast/
ls
vim fast.c
vim .ssh/authorized_keys 
youtube-dl
pip
easy_install
ls -lart
cd mnt
ls
mkdir mnt
cd mnt
ls
mkdir power
sshfs dat14amr@power.cs.lth.se: power/
cd
vim fast.c
cat fast.c 
ssh power.cs.lth.se
ssh pi@84.55.119.115
ssh pi@84.55.119.115
alsamixer
ls -lart
vim small.c 
vim small.c 
cd git/edaf15
ls -lart
cd small/
ls
ssh dat14amr@power.cs.lth.se
ssh dat14amr@power.cs.lth.se
power-de-e-najs 
vim .bashrc
source .bashrc 
power-de-e-najs 
power-de-e-najs 
power-de-e-najs 
power-de-e-najs 
power-de-e-najs 
power-de-e-najs 
uptime
power
ssh power
ls -alrt
ssh dat14amr@power.cs.lth.se
ssh power
ssh power.cs.lth.se
ssh dat14amr@power
ssh dat14amr@power.cs.lth.se
ls -lart
ssh dat14amr@power.cs.lth.se
ls -alr
file costrig2 
vim costrig2 
ssh dat14amr@cs.lth.se
ssh dat14amr@power.cs.lth.se
ssh dat14amr@power.cs.lth.se
ssh dat14amr@power.cs.lth.se
ssh dat14amr@power.cs.lth.se
ssh dat14amr@power.cs.lth.se
ssh dat14amr@power.cs.lth.se
ssh power
vim .ssh/config n
vim .ssh/config 
cd
ssh power
ssh dat14amr@power.cs.lth.se
ssh dat14amr@power.cs.lth.se
ssh dat14amr@power.cs.lth.se
ssh dat14amr@power.cs.lth.se
