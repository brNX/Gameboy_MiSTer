//
// timer.v
//
// Gameboy for the MIST board https://github.com/mist-devel
// Gameboy for MiSTer https://github.com/MiSTer-devel/Gameboy_MiSTer
// 
// Copyright (c) 2015 Till Harbaum <till@harbaum.org> 
// Copyright (c) 2020 Bruno Duarte Gouveia <bgouveia@gmail.com>
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
// August 2020 changes: rewrite to add obscure behaviour from https://gbdev.io/pandocs/#timer-and-divider-registers
//                      IRQ and reload handling modeled after https://github.com/furrtek/DMG-CPU-Inside/blob/master/Schematics/3_TIMER.png
// 
// 
//  
//

module timer (
    input  reset,
    input  clk,    // 4 Mhz cpu clock
    input  isGBC,
    output reg irq,
    output reg sound_clk,
    
    // cpu register interface
    input  cpu_sel,
    input [1:0] cpu_addr,
    input  cpu_wr,
    input [7:0] cpu_di,
    output [7:0] cpu_do
);

wire resetdiv = cpu_sel && cpu_wr && ~cpu_addr[1] && ~cpu_addr[0]; // cpu_addr == 2'b00 resetdiv also resets internal counter
wire tma_write = cpu_sel && cpu_wr && cpu_addr[1] && ~cpu_addr[0]; // cpu_addr == 2'b10
wire tima_write = cpu_sel && cpu_wr && cpu_addr[1] && cpu_addr[0]; // cpu_addr == 2'b11

//falling edge detector for sound_clk
reg sound_pin_r = 0;  
reg sound_pin_r2 = 0;
always @(posedge clk) begin
    if (reset) begin
        sound_pin_r <= 0;
        sound_pin_r2 <= 0;
        sound_clk <= 0;
    end else begin
        sound_pin_r2 <= sound_pin_r;
        sound_pin_r <= isGBC?clk_counter[14]:clk_counter[13];
        if (~sound_pin_r && sound_pin_r2) sound_clk <= ~sound_clk;
    end
end

wire [7:0] div = clk_counter [15:8];
reg [15:0] clk_counter;
always @(posedge clk or posedge resetdiv)
    if(resetdiv) begin
      clk_counter <= 16'd3;
    end else
        if (reset) begin
             clk_counter <= 16'd7;
        end else begin
            clk_counter <= clk_counter + 16'd1;
        end

reg [7:0] tma=0;
reg [7:0] tima=0;
reg [2:0] tac=0;

//TMA is a latch TODO: change this to a synchronous set and add condition to the TIMA reload instead
always @* begin
  if (reset)
    tma = 8'h00;
  else begin
        if (tma_write) tma = cpu_di;
  end
end


//falling edge detector
reg inc_r = 0;
reg inc_r2 = 0;

//https://github.com/furrtek/DMG-CPU-Inside/blob/master/Schematics/3_TIMER.png  irq == INT_TIMER
reg [1:0] BOGA1MHZ;
reg NYDU = 0;
always @(posedge clk) begin
    if (reset) begin
        BOGA1MHZ <= 2'd0;
        irq <= 0;
        NYDU <= 0;
    end else begin
        BOGA1MHZ <= BOGA1MHZ + 2'd1;

        if (BOGA1MHZ == 2'b00) begin
            NYDU <= irq?1'b0:(&tima);
            irq <= ~(~NYDU | (&tima));   
        end

    end
end

always @(posedge clk) begin
    if(reset) begin
        tima <= 0;
        tac <= 0;
        inc_r <= 0;
        inc_r2 <=0;
    end else begin
        inc_r <=   (tac[1:0] == 2'b00)? (clk_counter[9] & tac[2]):
                   (tac[1:0] == 2'b01)? (clk_counter[3] & tac[2]):
                   (tac[1:0] == 2'b10)? (clk_counter[5] & tac[2]):
                   (clk_counter[7] & tac[2]);
        inc_r2 <= inc_r;
        
        if (~inc_r && inc_r2) begin
            tima <= tima + 8'd1;
        end

        if (irq && !tima_write) tima <= tma;    // reload timer

        if(cpu_sel && cpu_wr) begin
            case(cpu_addr)
                2'b00:  ;    // writing clears counter
                2'b01:  tima <= cpu_di;
                2'b10:  ; //tma2 <= cpu_di;
                2'b11:  tac <= cpu_di[2:0];
            endcase
        end       

    end
end

assign cpu_do = 
    (cpu_addr == 2'b00)?div:
    (cpu_addr == 2'b01)?tima:
    (cpu_addr == 2'b10)?tma:
    {5'b11111, tac};

endmodule
