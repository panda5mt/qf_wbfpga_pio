module r1024x16_1024x16 (WA,RA,WD,WClk,RClk,WClk_En,RClk_En,WEN,RD);

input [9:0] WA;
input [9:0] RA;
input WClk,RClk;
input WClk_En,RClk_En;
input [1:0] WEN;
input [15:0] WD;
output [15:0] RD;

parameter [16383:0] INIT = 16384'b0;
parameter INIT_FILE="init_1024x16.hex";	

parameter addr_int = 10 ;
parameter data_depth_int = 1024;
parameter data_width_int = 16;
parameter wr_enable_int = 2;
parameter reg_rd_int = 0;


RAM_16K_BLK #(.addr_int(addr_int),.data_depth_int(data_depth_int),.data_width_int(data_width_int),.wr_enable_int(wr_enable_int),.reg_rd_int(reg_rd_int),
			  .INIT(INIT),.INIT_FILE(INIT_FILE)
			  )
RAM_INST (	.WA(WA),
			.RA(RA),
			.WD(WD),
			.WClk(WClk),
			.RClk(RClk),
			.WClk_En(WClk_En),
			.RClk_En(RClk_En),
			.WEN(WEN),
			.RD(RD)
			);
endmodule

