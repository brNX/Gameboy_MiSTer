//
// gb.v
//
// Gameboy for the MIST board https://github.com/mist-devel
// 
// Copyright (c) 2015 Till Harbaum <till@harbaum.org> 
// 
// This source file is free software: you can redistribute it and/or modify 
// it under the terms of the GNU General Public License as published 
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version. 
// 
// This source file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License 
// along with this program.  If not, see <http://www.gnu.org/licenses/>. 
//

module gb (
	input reset,
	input clk_sys,
	input ce,
	input ce_2x,
	input cpu_speed,
	input isGBC,
	
	input [12:0] vram_addr,
	input vram_rd,
	input vram1_rd,
	input vram_wren,
	input vram1_wren,
	input [7:0] vram_di,
	
    // cpu register adn oam interface
	input  cpu_sel_oam,
	input  cpu_sel_reg,
	input [7:0] cpu_addr,
	input  cpu_wr,
	input [7:0] cpu_di,
	output [7:0] cpu_do,

	// lcd interface
	output [1:0] lcd_mode

);

reg [7:0] vram_do;
reg [7:0] vram1_do;

wire clk = clk_sys & ce;
//wire clk2x = clk_sys & ce_2x;

wire current_cpu_ce = cpu_speed ? ce_2x:ce;
wire clk_cpu = clk_sys & current_cpu_ce;

//wire cpu_clken = current_cpu_ce;  //when hdma is enabled stop CPU (GBC)

// --------------------------------------------------------------------
// ------------------------------ video -------------------------------
// --------------------------------------------------------------------
wire [12:0] video_addr;

wire [12:0] curr_vram_addr = (vram_rd | vram1_rd | vram_wren | vram1_wren) ? vram_addr:video_addr;

// lcd interface
wire lcd_clkena;
wire [14:0] lcd_data;
wire [1:0] lcd_mode_wire;

assign lcd_mode = lcd_mode_wire;
wire lcd_on;


video video (
	.reset		 ( reset         ),
	.clk		 ( clk           ),
	.clk_reg	 ( clk_cpu       ),   //can be 2x in cgb double speed mode
	.isGBC		 ( isGBC         ),
	

/* verilator lint_off PINCONNECTEMPTY */
	.irq         (),
	.vblank_irq  (),


	.cpu_sel_reg ( cpu_sel_reg   ),
	.cpu_sel_oam ( cpu_sel_oam   ),
	.cpu_addr    ( cpu_addr      ),
	.cpu_wr      ( cpu_wr        ),
	.cpu_di      ( cpu_di        ),
	.cpu_do      ( cpu_do        ),
	
	.lcd_on      ( lcd_on        ),
	.lcd_clkena  ( lcd_clkena    ),
	.lcd_data    ( lcd_data      ),
	.mode        ( lcd_mode_wire ),
	
	.vram_rd     (    ),
	.vram_addr   ( video_addr    ),
	.vram_data   ( vram_do       ),
	
	// vram connection bank1 (GBC)
	.vram1_data  ( vram1_do      ),
	
	.dma_rd      (       ),
	.dma_addr    (       ),
	.dma_data    (       )
/* verilator lint_on PINCONNECTEMPTY */
);


lcd lcd (
	 .clk    ( clk_cpu    ),

	 // serial interface
	 .clkena ( lcd_clkena ),
	 .data   ( lcd_data   ),
	 .mode   ( lcd_mode_wire ),  // used to detect begin of new lines and frames
	 .on     ( lcd_on     ),
	 .isGBC	 ( isGBC      )
);

// total 8k/16k (CGB) vram from $8000 to $9fff

reg [7:0] vram0_array [0:8191] /*verilator public*/; 
reg [7:0] vram1_array [0:8191] /*verilator public*/; 

always @ (posedge clk_cpu)
    begin
        if(vram_wren) begin
            vram0_array[curr_vram_addr] <= vram_di;
        end
        else begin
            vram_do <= vram0_array[curr_vram_addr];
        end     
    end
	
always @ (posedge clk_cpu)
    begin
        if(vram1_wren) begin
            vram1_array[curr_vram_addr] <= vram_di;
        end
        else begin
            vram1_do <= vram1_array[curr_vram_addr];
        end     
    end

endmodule
