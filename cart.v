
module cart(
	input clk,
	input [23:0] addr,
	output [7:0] data
);

reg [7:0] rom_data [8388608:0] /*verilator public*/;

/*initial begin
    $readmemh("boot_rom.mem", rom_data);
end*/

always @(posedge clk)
begin
    data <= rom_data[addr];
end

endmodule
