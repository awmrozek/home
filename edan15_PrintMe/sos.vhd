entity sos is

port (
	clk : in std_logic;
	reset : in std_logic;
	token : in std_logic_vector (0 to 7);
	match : out std_logic;
	position : out std_logic_vecotr (0 to 31);
);

end sos;

architecture behavioral of sos is

begin

end;