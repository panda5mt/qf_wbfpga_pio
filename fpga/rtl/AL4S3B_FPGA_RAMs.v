// -----------------------------------------------------------------------------
// title          : AL4S3B Example FPGA Register Module
// project        : Tamar2 Device
// -----------------------------------------------------------------------------
// file           : AL4S3B_FPGA_RAMs.v
// author         : SSG
// company        : QuickLogic Corp
// created        : 2020/05/27	
// last update    : 2020/05/27
// platform       : ArcticLink 4 S3B
// standard       : Verilog 2001
// -----------------------------------------------------------------------------
// description: The FPGA example IP design contains the essential logic for
//              interfacing the ASSP of the AL4S3B to registers and memory 
//              located in the programmable FPGA.
// -----------------------------------------------------------------------------
// copyright (c) 2016
// -----------------------------------------------------------------------------
// revisions  :
// date            version    author                  description
// 2020/05/27      1.0        Rakesh moolacheri     Initial Release
//
// -----------------------------------------------------------------------------
// Comments: This solution is specifically for use with the QuickLogic
//           AL4S3B device. 
// -----------------------------------------------------------------------------
//
`timescale 1ns / 10ps

module AL4S3B_FPGA_RAMs ( 

				// AHB-To_FPGA Bridge I/F
				//
				WBs_ADR_i,
				WBs_RAM0_CYC_i,
				WBs_RAM1_CYC_i,
				WBs_RAM2_CYC_i,
				WBs_RAM3_CYC_i,
				//WBs_RAM4_CYC_i,
				WBs_BYTE_STB_i,
				WBs_WE_i,
				WBs_STB_i,
				WBs_DAT_i,
				WBs_CLK_i,
				WBs_RST_i,
				WBs_RAM0_DAT_o,
				WBs_RAM1_DAT_o,
				WBs_RAM2_DAT_o,
				WBs_RAM3_DAT_o,
				WBs_RAM_STATUS_o,
				//WBs_RAM4_DAT_o,
				WBs_ACK_o,
				
				PCLKI,
				VSYNCI,
				HREFI,
				CAM_DAT
			);


//------Port Parameters----------------
//

parameter                ADDRWIDTH                   =   7  ;   // Allow for up to 128 registers in the FPGA
parameter                DATAWIDTH                   =  32  ;   // Allow for up to 128 registers in the FPGA

parameter                AL4S3B_DEF_REG_VALUE        = 32'hFAB_DEF_AC;


//------Port Signals-------------------
//

// AHB-To_FPGA Bridge I/F
//
input   [10:0]           WBs_ADR_i     ;  // Address Bus                to   FPGA
input                    WBs_RAM0_CYC_i;  
input                    WBs_RAM1_CYC_i;
input                    WBs_RAM2_CYC_i;
input                    WBs_RAM3_CYC_i;
//input                    WBs_RAM4_CYC_i;
input             [3:0]  WBs_BYTE_STB_i;  // Byte Select                to   FPGA
input                    WBs_WE_i      ;  // Write Enable               to   FPGA
input                    WBs_STB_i     ;  // Strobe Signal              to   FPGA
input   [DATAWIDTH-1:0]  WBs_DAT_i     ;  // Write Data Bus             to   FPGA
input                    WBs_CLK_i     ;  // FPGA Clock               from FPGA
input                    WBs_RST_i     ;  // FPGA Reset               to   FPGA
output  [DATAWIDTH-1:0]  WBs_RAM0_DAT_o; 
output  [DATAWIDTH-1:0]  WBs_RAM1_DAT_o;  
output  [DATAWIDTH-1:0]  WBs_RAM2_DAT_o;  
output  [DATAWIDTH-1:0]  WBs_RAM3_DAT_o;
output  [DATAWIDTH-1:0]  WBs_RAM_STATUS_o;  
//output  [DATAWIDTH-1:0]  WBs_RAM4_DAT_o;
output                   WBs_ACK_o     ;  // Transfer Cycle Acknowledge from FPGA



// FPGA Global Signals
//
wire                     WBs_CLK_i     ;  // Wishbone FPGA Clock
wire                     WBs_RST_i     ;  // Wishbone FPGA Reset

// Wishbone Bus Signals
//
wire    [10:0]  WBs_ADR_i     ;  // Wishbone Address Bus
wire                     WBs_RAM0_CYC_i;  
wire                     WBs_RAM1_CYC_i;
wire                     WBs_RAM2_CYC_i;
wire                     WBs_RAM3_CYC_i;
//wire                     WBs_RAM4_CYC_i;
wire              [3:0]  WBs_BYTE_STB_i;  // Wishbone Byte   Enables
wire                     WBs_WE_i      ;  // Wishbone Write  Enable Strobe
wire                     WBs_STB_i     ;  // Wishbone Transfer      Strobe
wire    [DATAWIDTH-1:0]  WBs_DAT_i     ;  // Wishbone Write  Data Bus
 
reg                      WBs_ACK_o     ;  // Wishbone Client Acknowledge


//
// CAMERA 
//
input           PCLKI;
input           VSYNCI;
input           HREFI;
input [7:0]		CAM_DAT;

wire 			PCLKI;
wire 			VSYNCI;
wire 			HREFI;
wire [7:0]		CAM_DAT;

//------Define Parameters--------------
//

//
// None at this time
//

//------Internal Signals---------------
//
wire			FB_RAM0_Wr_Dcd    ;
wire			FB_RAM1_Wr_Dcd    ;
wire			FB_RAM2_Wr_Dcd    ;
wire			FB_RAM3_Wr_Dcd    ;


wire			WBs_ACK_o_nxt;

wire [31:0]  	RAM0_Dat_out;
wire [31:0]		RAM1_Dat_out;
wire [31:0]		RAM2_Dat_out;
wire [31:0]		RAM3_Dat_out;
wire [31:0]		RAM_Status_out;
//wire [7:0]   RAM4_Dat_out;


//------Logic Operations---------------
//

// Define the FPGA's local register write enables
//
assign FB_RAM0_Wr_Dcd    = WBs_RAM0_CYC_i & WBs_STB_i & WBs_WE_i  & (~WBs_ACK_o);
assign FB_RAM1_Wr_Dcd    = WBs_RAM1_CYC_i & WBs_STB_i & WBs_WE_i  & (~WBs_ACK_o);
assign FB_RAM2_Wr_Dcd    = WBs_RAM2_CYC_i & WBs_STB_i & WBs_WE_i  & (~WBs_ACK_o);
assign FB_RAM3_Wr_Dcd    = WBs_RAM3_CYC_i & WBs_STB_i & WBs_WE_i  & (~WBs_ACK_o);
//assign FB_RAM4_Wr_Dcd    = WBs_RAM4_CYC_i & WBs_STB_i & WBs_WE_i  & (~WBs_ACK_o);

// Define the Acknowledge back to the host for registers
//
assign WBs_ACK_o_nxt     = (WBs_RAM0_CYC_i | WBs_RAM1_CYC_i | WBs_RAM2_CYC_i | WBs_RAM3_CYC_i/* | WBs_RAM4_CYC_i*/) & WBs_STB_i & (~WBs_ACK_o);

/* CAMERA - RAM interface: begin */
reg         cam_reg_rdy;
wire        cam_reg_rdy;
wire		cam_data_valid;

wire        select_ram0;   
wire        select_ram1;
wire        select_ram2;
wire        select_ram3;
wire        cam_push_sig0;
wire        cam_push_sig1;
wire        cam_push_sig2;
wire        cam_push_sig3;
wire        cam_push_clk;

reg  [10:0]   	cam_ram_cnt; // 11bit, up to 2048
reg  [31:0]     cam_reg1;
reg  [31:0]		cam_reg_out;     
reg  [31:0]		cam_freerun;

wire [31:0]     cam_reg1;
wire [31:0] 	cam_reg_out;
wire [31:0]		cam_freerun;

/* FSM */
localparam  RAM_COUNT_FULL = 11'd2048;
reg	 [1:0]  	 cam_status;
wire [1:0]  	 cam_status;

localparam CRSET =2'd0;  // RESET
localparam CB08F =2'd1;  // Camera buffer 8bit Full 
localparam CB16F =2'd2;  // Camera buffer 16bit full
localparam CB24F =2'd3;  // Camera buffer 24bit full
//localparam CB32F =3'd4;
assign cam_data_valid	= HREFI & VSYNCI ;
always @( posedge PCLKI or posedge WBs_RST_i )
begin
    if(WBs_RST_i)
    begin
        cam_reg1		<= 32'h0;
        cam_reg_out		<= 32'h00;
        cam_status		<= CRSET;
        cam_reg_rdy		<= 1'b0;
        cam_ram_cnt		<= 11'h00;
        cam_freerun		<= 32'h00;
    end
    else begin // PCLK
		if(cam_data_valid)begin
			case(cam_status)
			CRSET: begin
				cam_reg1	<= {24'h00, CAM_DAT[7:0]};
				//cam_reg_out <= 32'h0;
				cam_reg_rdy <= 1'b0;
				cam_status	<= CB08F;
			end
			CB08F: begin	
				cam_reg1	<= {16'h00,cam_reg1[7:0],CAM_DAT[7:0]};
				cam_reg_rdy <= 1'b0;
				cam_status	<= CB16F;
			end
			CB16F: begin	
				cam_reg1	<= {8'h00,cam_reg1[15:0],CAM_DAT[7:0]};
				cam_reg_rdy <= 1'b0;
				cam_status	<= CB24F;
			end
			CB24F: begin
				cam_reg_out	<= {cam_reg1[23:0],CAM_DAT[7:0]}; //cam_freerun[31:0];
				cam_reg1	<= 32'h0;
				cam_reg_rdy	<= 1'b1;
				cam_ram_cnt	<= cam_ram_cnt + 11'h01;// % RAM_COUNT_FULL; // modulo-N counter
				cam_freerun	<= cam_freerun + 32'h01;
				cam_status	<= CRSET;
			end
			/*
			default: begin
				cam_reg_out	<= cam_reg_out;
				cam_reg_rdy <= 1'b0;
			end
			*/
			endcase
		end
		else begin //!cam_data_valid
			cam_reg_rdy	<= 1'b0;
			cam_status	<= cam_status;
			cam_reg_out <= cam_reg_out;
		end
	end	// PCLK
end
assign select_ram0		= (cam_ram_cnt[10:9] == 2'b00); // 0 =< cam_ram_cnt < 512
assign select_ram1		= (cam_ram_cnt[10:9] == 2'b01); // 512 =< cam_ram_cnt < 1024 
assign select_ram2		= (cam_ram_cnt[10:9] == 2'b10); // 1024 =< cam_ram_cnt < 1536 
assign select_ram3		= (cam_ram_cnt[10:9] == 2'b11); // 1536 =< cam_ram_cnt < 2048
assign cam_push_clk		= cam_reg_rdy & ~(PCLKI) ;

assign cam_push_sig0	= cam_reg_rdy & select_ram0;
assign cam_push_sig1	= cam_reg_rdy & select_ram1;
assign cam_push_sig2	= cam_reg_rdy & select_ram2;
assign cam_push_sig3	= cam_reg_rdy & select_ram3;
/* CAMERA - RAM interface: end */

// Define the FPGA's Local Registers
//
always @( posedge WBs_CLK_i or posedge WBs_RST_i)
begin
    if (WBs_RST_i)
    begin
        WBs_ACK_o         <=  1'b0           ;
    end  
    else
    begin
        WBs_ACK_o         <=  WBs_ACK_o_nxt  ;
    end  
end

assign WBs_RAM0_DAT_o = RAM0_Dat_out;

r512x32_512x32 RAM0_INST (	
			.WA(cam_ram_cnt[8:0]), 	//.WA(WBs_ADR_i[8:0]),
			.RA(WBs_ADR_i[8:0]),
			.WD(cam_reg_out),			//.WD(WBs_DAT_i[31:0]),
			.WClk(cam_push_clk),		//.WClk(WBs_CLK_i),
			.RClk(WBs_CLK_i),
			.WClk_En(1'b1),//.WClk_En(1'b1),
			.RClk_En(1'b1),
			.WEN({cam_push_sig0,cam_push_sig0,cam_push_sig0,cam_push_sig0}),
			//.WEN({FB_RAM0_Wr_Dcd,FB_RAM0_Wr_Dcd,FB_RAM0_Wr_Dcd,FB_RAM0_Wr_Dcd}),
			.RD(RAM0_Dat_out)
			);

assign WBs_RAM1_DAT_o = {RAM1_Dat_out};

r512x32_512x32 RAM1_INST (	
			.WA(cam_ram_cnt[8:0]), 	//.WA(WBs_ADR_i[8:0]),
			.RA(WBs_ADR_i[8:0]),
			.WD(cam_reg_out),			//.WD(WBs_DAT_i[31:0]),
			.WClk(cam_push_clk),		//.WClk(WBs_CLK_i),
			.RClk(WBs_CLK_i),
			.WClk_En(1'b1),
			.RClk_En(1'b1),
			.WEN({cam_push_sig1,cam_push_sig1,cam_push_sig1,cam_push_sig1}),
			//.WEN({FB_RAM1_Wr_Dcd,FB_RAM1_Wr_Dcd,FB_RAM1_Wr_Dcd,FB_RAM1_Wr_Dcd}),
			.RD(RAM1_Dat_out)
			);
		

assign WBs_RAM2_DAT_o = {RAM2_Dat_out};

r512x32_512x32 RAM2_INST (	
			.WA(cam_ram_cnt[8:0]), 	//.WA(WBs_ADR_i[8:0]),
			.RA(WBs_ADR_i[8:0]),
			.WD(cam_reg_out),			//.WD(WBs_DAT_i[31:0]),
			.WClk(cam_push_clk),		//.WClk(WBs_CLK_i),
			.RClk(WBs_CLK_i),
			.WClk_En(1'b1),
			.RClk_En(1'b1),
			.WEN({cam_push_sig2,cam_push_sig2,cam_push_sig2,cam_push_sig2}),
			//.WEN({FB_RAM2_Wr_Dcd,FB_RAM2_Wr_Dcd,FB_RAM2_Wr_Dcd,FB_RAM2_Wr_Dcd}),
			.RD(RAM2_Dat_out)
			);

assign WBs_RAM3_DAT_o = RAM3_Dat_out;

r512x32_512x32 RAM3_INST (	
			.WA(cam_ram_cnt[8:0]), 	//.WA(WBs_ADR_i[8:0]),
			.RA(WBs_ADR_i[8:0]),
			.WD(cam_reg_out),			//.WD(WBs_DAT_i[31:0]),
			.WClk(cam_push_clk),		//.WClk(WBs_CLK_i),
			.RClk(WBs_CLK_i),
			.WClk_En(1'b1),
			.RClk_En(1'b1),
			.WEN({cam_push_sig3,cam_push_sig3,cam_push_sig3,cam_push_sig3}),
			//.WEN({FB_RAM3_Wr_Dcd,FB_RAM3_Wr_Dcd,FB_RAM3_Wr_Dcd,FB_RAM3_Wr_Dcd}),
			.RD(RAM3_Dat_out)
			);



assign WBs_RAM_STATUS_o = {30'h0,cam_ram_cnt[10:9]}; //RAM_Status_out;

			
endmodule