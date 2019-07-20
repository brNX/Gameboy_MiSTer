
module boot_rom(
	input clk,
	input [7:0] addr,
	output [7:0] data
);

reg [7:0] rom_data [255:0];

initial begin
    $readmemh("boot_rom-sameboy.mem", rom_data);
end

always @(posedge clk)
begin
    data <= rom_data[addr];
end

endmodule
