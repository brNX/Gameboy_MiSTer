// Gameboy for the MiST
// (c) 2015 Till Harbaum

// The gameboy lcd runs from a shift register which is filled at 4194304 pixels/sec

module lcd (
	input   clk,
	input   clkena,
	input [14:0] data,
	input [1:0] mode,
    input  on
);


reg [14:0] vbuffer_inptr;
reg vbuffer_write;

reg [14:0] vbuffer_outptr;
reg [14:0] vbuffer_lineptr;

reg [14:0] lcd_buffer [0:23039] /*verilator public*/;

always @ (posedge clk)
    begin
        if(clkena) begin
            lcd_buffer[vbuffer_inptr] <= data;
        end 
    end

always @(posedge clk) begin
	if(!on || (mode==2'd01)) begin  //lcd disabled of vsync restart pointer
	   vbuffer_inptr <= 15'h0;
	end else begin
		
		// end of vsync
		if(clkena) begin
			vbuffer_inptr <= vbuffer_inptr + 15'd1;
		end
		
	end;
end


endmodule
