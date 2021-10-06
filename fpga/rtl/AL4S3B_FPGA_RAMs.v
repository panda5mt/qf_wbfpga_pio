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
				WBs_STATUS_CYC_i,
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
				CAM_DAT,
				QUAD_In_i,
				QUAD_oe_o,
				QUAD_Out_o, 
				QUAD_nCE_o,
				QUAD_CLK_i
			);


//------Port Parameters----------------
//

parameter                ADDRWIDTH                   =   7  ;   // Allow for up to 128 registers in the FPGA
parameter                DATAWIDTH                   =  32  ;   // Allow for up to 128 registers in the FPGA

parameter                AL4S3B_DEF_REG_VALUE        = 32'hFAB_DEF_AC;


//------Port Signals-------------------
//
// HighSpeed CLK(36~72MHz)
input					QUAD_CLK_i		;
// AHB-To_FPGA Bridge I/F
//
input   [10:0]           WBs_ADR_i     ;  // Address Bus                to   FPGA
input                    WBs_RAM0_CYC_i;  
input                    WBs_RAM1_CYC_i;
input                    WBs_RAM2_CYC_i;
input                    WBs_RAM3_CYC_i;
input                    WBs_STATUS_CYC_i;
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
wire                    WBs_CLK_i   ;	// Wishbone FPGA Clock
wire                    WBs_RST_i   ;	// Wishbone FPGA Reset
wire 					QUAD_CLK_i	;

// Wishbone Bus Signals
//
wire    [10:0]			WBs_ADR_i     ;  // Wishbone Address Bus
wire					WBs_RAM0_CYC_i;  
wire					WBs_RAM1_CYC_i;
wire					WBs_RAM2_CYC_i;
wire					WBs_RAM3_CYC_i;
wire					WBs_STATUS_CYC_i;
wire	[3:0]			WBs_BYTE_STB_i;  // Wishbone Byte   Enables
wire					WBs_WE_i      ;  // Wishbone Write  Enable Strobe
wire					WBs_STB_i     ;  // Wishbone Transfer      Strobe
wire	[DATAWIDTH-1:0]	WBs_DAT_i     ;  // Wishbone Write  Data Bus
 
reg						WBs_ACK_o     ;  // Wishbone Client Acknowledge

// SPI SRAMS
//

input	[3:0]	QUAD_In_i		;
output			QUAD_oe_o		;
input	[3:0]	QUAD_Out_o		;

wire	[3:0]	QUAD_In_i		;
wire			QUAD_oe_o		;
wire	[3:0]	QUAD_Out_o		;
output			QUAD_nCE_o		;
wire 			QUAD_nCE_o		;
// CAMERA 
//
input           PCLKI			;
input           VSYNCI			;
input           HREFI			;
input	[7:0]	CAM_DAT			;

wire 			PCLKI			;
wire 			VSYNCI			;
wire 			HREFI			;
wire	[7:0]	CAM_DAT			;

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
wire			FB_STATUS_Wr_Dcd  ;



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
assign FB_STATUS_Wr_Dcd  = WBs_STATUS_CYC_i & WBs_STB_i & WBs_WE_i  & (~WBs_ACK_o);

// Define the Acknowledge back to the host for registers
//
assign WBs_ACK_o_nxt     = (WBs_RAM0_CYC_i | WBs_RAM1_CYC_i | WBs_RAM2_CYC_i | WBs_RAM3_CYC_i | WBs_STATUS_CYC_i) & WBs_STB_i & (~WBs_ACK_o);

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
localparam  	RAM_COUNT_FULL = 11'd1024;
reg	 [1:0]		cam_status;
wire [1:0]		cam_status;

localparam		CRSET = 2'd0;  // RESET
localparam		CB08F = 2'd1;  // Camera buffer 8bit Full 
localparam		CB16F = 2'd2;  // Camera buffer 16bit full
localparam		CB24F = 2'd3;  // Camera buffer 24bit full
//localparam CB32F =3'd4;
assign cam_data_valid	= HREFI & VSYNCI ;
always @( posedge PCLKI or posedge WBs_RST_i)
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
				cam_reg_out	<= 32'hAABBCCDD;//{cam_reg1[23:0],CAM_DAT[7:0]}; //cam_freerun[31:0];
				cam_reg1	<= 32'h0;
				cam_reg_rdy	<= 1'b1;
				cam_ram_cnt	<= (cam_ram_cnt + 11'h01) % RAM_COUNT_FULL; // modulo-N counter
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

assign cam_push_clk		= cam_reg_rdy & ~(PCLKI) ;

assign cam_push_sig0	= cam_reg_rdy & select_ram0;
assign cam_push_sig1	= cam_reg_rdy & select_ram1;

/* CAMERA - RAM interface: end */


/* QSPI SRAM - RAM interface: begin */
/* FSM */
reg		[7:0]	qspi_status		;
wire	[7:0]	qspi_status		;
reg		[7:0]	qspi_command	;
wire	[7:0]	qspi_command	;
reg		[23:0]	qspi_addr		;
wire	[23:0]	qspi_addr		;
reg		[23:0]	qspi_addr_next	;
wire	[23:0]	qspi_addr_next	;

wire 	[10:0]	txrx_fbram_addr	;
reg		[10:0]	txrx_fbram_addr	;

wire 			txrx_fbram_ch	;
reg				txrx_fbram_ch	;
wire 			read_fbram_sig	; // read signal
reg				read_fbram_sig	;
wire 			write_fbram_sig	; // write signal
reg				write_fbram_sig	;
wire 	[31:0]	txrx_fbram_data	;
reg		[31:0]	txrx_fbram_data	;
wire 			read_fbram_clk	;
wire 			write_fbram_clk	;


assign read_fbram_clk 	= QUAD_CLK_i & read_fbram_sig 	;
assign write_fbram_clk	= QUAD_CLK_i & write_fbram_sig	;

localparam QRSET 	= 8'hFF;	// RESET
localparam QWR00 	= 8'd0;		// Write Command bit[7] send 
localparam QWR01 	= 8'd1;		// Write Command bit[6] send
localparam QWR02 	= 8'd2;		// Write Command bit[5] send
localparam QWR03 	= 8'd3;		// Write Command bit[4] send
localparam QWR04 	= 8'd4;		// Write Command bit[3] send
localparam QWR05 	= 8'd5;		// Write Command bit[2] send
localparam QWR06 	= 8'd6;		// Write Command bit[1] send
localparam QWR07	= 8'd7;		// Write Command bit[0] send
localparam QWADR0	= 8'd8;		// Send Address [23:20]
localparam QWADR1	= 8'd9;		// Send Address [19:16]
localparam QWADR2	= 8'd10;	// Send Address [15:12]
localparam QWADR3	= 8'd11;	// Send Address [11:8]
localparam QWADR4	= 8'd12;	// Send Address [7:4] & get data(32bit length) from FB_RAM
localparam QWADR5	= 8'd13;	// Send Address [3:0]
localparam EXEC0 	= 8'd14;	// Send data[31:28]
localparam EXEC1 	= 8'd15;	// Send data[27:24]
localparam EXEC2 	= 8'd16;	// Send data[23:20]
localparam EXEC3 	= 8'd17;	// Send data[19:16]
localparam EXEC4 	= 8'd18;	// Send data[15:12]
localparam EXEC5 	= 8'd19;	// Send data[11:8]
localparam EXEC6 	= 8'd20;	// Send data[7:4] & get data(32bit length) from FB_RAM
localparam EXEC7 	= 8'd21;	// Send data[3:0] & check QSPI RAM address whether end of 512byte burst length
localparam EXEC8 	= 8'd22;	// update QSPI RAM Address(if we need one more 512byte-burst)
localparam EXEC9 	= 8'd23;	// dummy wait
localparam EXEC10 	= 8'd24;	// check FB_RAM address whether end of RAM0 or RAM1

localparam QRD00 	= 8'd32;	// Read Command bit[7] send 
localparam QRD01 	= 8'd33;	// Read Command bit[6] send
localparam QRD02 	= 8'd34;	// Read Command bit[5] send
localparam QRD03 	= 8'd35;	// Read Command bit[4] send
localparam QRD04 	= 8'd36;	// Read Command bit[3] send
localparam QRD05 	= 8'd37;	// Read Command bit[2] send
localparam QRD06 	= 8'd38;	// Read Command bit[1] send
localparam QRD07	= 8'd39;	// Read Command bit[0] send
localparam QRADR0	= 8'd40;	// Send Address [23:20]
localparam QRADR1	= 8'd41;	// Send Address [19:16]
localparam QRADR2	= 8'd42;	// Send Address [15:12]
localparam QRADR3	= 8'd43;	// Send Address [11:8]
localparam QRADR4	= 8'd44;	// Send Address [7:4] & get data(32bit length) from FB_RAM
localparam QRADR5	= 8'd45;	// Send Address [3:0]
// wait cycles(6cycle + tACLK(=max 5.5nsec))
localparam WCYC00 	= 8'd46;
localparam WCYC01 	= 8'd47;
localparam WCYC02 	= 8'd48;
localparam WCYC03 	= 8'd49;
localparam WCYC04 	= 8'd50;
localparam WCYC05 	= 8'd51;

localparam EXEC20 	= 8'd52;	// Send data[31:28]
localparam EXEC21 	= 8'd53;	// Send data[27:24]
localparam EXEC22 	= 8'd54;	// Send data[23:20]
localparam EXEC23 	= 8'd55;	// Send data[19:16]
localparam EXEC24 	= 8'd56;	// Send data[15:12]
localparam EXEC25 	= 8'd57;	// Send data[11:8]
localparam EXEC26 	= 8'd58;	// Send data[7:4] & get data(32bit length) from FB_RAM
localparam EXEC27 	= 8'd59;	// Send data[3:0] & check QSPI RAM address whether end of 512byte burst length
localparam EXEC28 	= 8'd60;	// update QSPI RAM Address(if we need one more 512byte-burst)
localparam EXEC29 	= 8'd61;	// dummy wait
localparam EXEC30 	= 8'd62;	// check FB_RAM address whether end of RAM0 or RAM1

// QSPI SRAM's parameter
localparam QPIRSE	= 8'b0110_0110; // Reset enable Command (8'h66)
localparam QPIRST	= 8'b1001_1001; // Reset Command 		(8'h99)
localparam QPIWR 	= 8'b0011_1000;	// Quad Write Command 	(8'h38)
localparam QPIRD	= 8'b1110_1011;	// Quad Read Command  	(8'hEB)
localparam STADR	= 24'h00;		// Quad Start Address 	(24bit)
localparam nCE_SEL	= 1'b0;			// SELECT
localparam nCE_DES	= 1'b1;			// DESELECT


// select state-machine parameter 
wire qspi_write_mode;		// RAM0,1 -> QSPI SRAM
wire qspi_read_mode;		// QSPI SRAM -> RAM2,3
wire qspi_reset_mode;		// QSPI reset 
wire qspi_tx_go_flag;		// Send 4kB GO-FLAG.

// Status(Cortex-M4F -> FPGA)
assign qspi_write_mode	= (WBs_RAM_STATUS_i[1:0] == 2'b10);
assign qspi_read_mode	= (WBs_RAM_STATUS_i[1:0] == 2'b01);
assign qspi_reset_mode	= (WBs_RAM_STATUS_i[1:0] == 2'b11);

assign qspi_tx_go_flag	= (WBs_RAM_STATUS_i[2] == 1'b1);
// Status(FPGA -> Cortex-M4F) We come to end of address of RAM3 or RAM4
wire 	qspi_tx_fin		;
assign qspi_tx_fin = (txrx_fbram_addr[8:0] == 9'h00);

always @( negedge /*PCLKI*/ QUAD_CLK_i or posedge WBs_RST_i) begin
	if(WBs_RST_i)begin
		qspi_status			<= QRSET	;
		write_fbram_sig		<= 1'b0		;
		read_fbram_sig		<= 1'b0		;
		qspi_command		<= QPIRD	;	// Read Command
		QUAD_oe_o			<= 1'b1		;	// OE = 1 Output, OE=0 input
		qspi_addr			<= STADR	;
		txrx_fbram_addr		<= 11'b0	;	// for reason of QSPI's page boundary (512byte), STADR / 4
		qspi_addr_next		<= 22'b0	;
		QUAD_nCE_o			<= nCE_DES	;	// deactivate nCE
	end // reset
	else 
	if (qspi_write_mode) begin
		case(qspi_status)
		QRSET :begin
			if (select_ram0 == 1'b0) begin // camera writes ram1 memory not ram0
				write_fbram_sig	<= 1'b0			;
				read_fbram_sig	<= 1'b0			;
				QUAD_oe_o		<= 1'b1			;	// OSIO[3:0] Output
				qspi_command	<= QPIWR		;	// Write Command
				qspi_status		<= QWR00		;
				txrx_fbram_ch	<= 1'b0			;
			end
			else begin
				write_fbram_sig	<= 1'b0			;
				read_fbram_sig	<= 1'b0			;
				QUAD_nCE_o 		<= nCE_DES		;
				qspi_status		<= qspi_status	; 	// stop until select_ram0 == 1'b1
			end
		end

		// send qspi_command[7]~[0]
		QWR00,QWR01,QWR02,QWR03,
		QWR04,QWR05,QWR06,QWR07:begin		
			QUAD_nCE_o 			<= nCE_SEL						;	// select nCE
			QUAD_Out_o[0]		<= qspi_command[7]				;
			qspi_command		<= {qspi_command[6:0],1'b0}		;
			qspi_status			<= qspi_status + 8'b1			;
		end

		// QWADR0-5 : Send 24-bit Address
		QWADR0,QWADR1,QWADR2,
		QWADR3 :begin									
			QUAD_Out_o[3:0]		<= qspi_addr[23:20]				;
			qspi_addr			<= {qspi_addr[19:0],4'h0}		; // 4bit shift
			qspi_status			<= qspi_status + 8'b1			; 
		end
		
		QWADR4 :begin									
			QUAD_Out_o[3:0]		<= qspi_addr[23:20]			;
			qspi_addr			<= {qspi_addr[19:0],4'h0}		; // 4bit shift
			read_fbram_sig		<= 1'b1							; // FB_RAM read signal
			qspi_status			<= QWADR5						; 
		end

		// get data from FB_RAM
		QWADR5 :begin									
			QUAD_Out_o[3:0]			<= qspi_addr[23:20]			;
			read_fbram_sig			<= 1'b0 						;
			txrx_fbram_data[31:0] 	<= (txrx_fbram_addr[10:9]==2'b00)? RAM0_Dat_out : RAM1_Dat_out;
			qspi_status				<= EXEC0						; 
		end

		// EXEC0~7:32bit data -> QSPI SRAM
		EXEC0,EXEC1,EXEC2,
		EXEC3,EXEC4,EXEC5 :begin										
			QUAD_Out_o[3:0]			<= txrx_fbram_data[31:28]		;
			txrx_fbram_data[31:0]	<= {txrx_fbram_data[27:0],4'b0}	;	// 4bit shift
			qspi_status				<= qspi_status + 8'b1			;			
		end
		
		EXEC6 :begin										
			QUAD_Out_o[3:0]			<= txrx_fbram_data[31:28]		;
			txrx_fbram_data[31:0]	<= {txrx_fbram_data[27:0],4'b0}	;	// 4bit shift
			read_fbram_sig			<= 1'b1							;	// FB_RAM read signal
			qspi_status				<= EXEC7						;			
		end

		EXEC7 :begin										
			QUAD_Out_o[3:0]			<= txrx_fbram_data[31:28]		;
			txrx_fbram_addr 		<= (txrx_fbram_addr + 11'h01) % 11'd1024  ;
			read_fbram_sig			<= 1'b0 						;
			txrx_fbram_data[31:0] 	<= (txrx_fbram_addr[10:9]==2'b00)? RAM0_Dat_out : RAM1_Dat_out;
			qspi_addr_next 			<= qspi_addr_next + 22'd4 		;					// 4-byte countup
			qspi_status				<= (qspi_addr_next[8:0]==9'h1fC)? EXEC8 : EXEC0;	// 512byte burst finished? (h'1FC = d'512 - d'4)		
		end

		EXEC8 :begin
			QUAD_Out_o[3:0]		<= 4'h00						;
			QUAD_nCE_o 			<= nCE_DES 						;	// deactivate nCE
			qspi_addr			<= qspi_addr_next				;
			qspi_status 		<= EXEC9						;
			
		end

		// dummy wait 
		EXEC9 :begin
			qspi_status 		<= EXEC10						;
		end
		
		// now, We are on end of address of RAM0 or RAM1 ?
		EXEC10 :begin 
			if (txrx_fbram_addr[8:0] == 9'h00)	
			begin 
				if((txrx_fbram_ch == 1'b0)/* && (txrx_fbram_addr[10:9]==2'b01)*/)	// selected RAM0 but next address is RAM1
				begin 
					if(select_ram0)							// CAMERA module writting RAM0 ?
					begin									// Yes!
						//QUAD_nCE_o 		<= nCE_SEL	;		// activate nCE
						txrx_fbram_ch 	<= 1'b1		;		// change to RAM1
						qspi_command	<= QPIWR	;		// QSPI SRAM Write Command
						qspi_status 	<= QWR00	;		// back to state "QWR00" 
					end
					else
					begin 									// RAM1 is busy!
						qspi_status 	<= qspi_status; 	// stay this state
					end
				end
				else 
				if((txrx_fbram_ch == 1'b1)/* && (txrx_fbram_addr[10:9]==2'b00)*/)	// selected RAM1 but next address is RAM0
				begin
					if(select_ram1)							// CAMERA module writting RAM1 ?
					begin									// Yes!
						//QUAD_nCE_o 		<= nCE_SEL	;		// activate nCE
						txrx_fbram_ch 	<= 1'b0		;		// change to RAM0
						qspi_command	<= QPIWR	;		// QSPI SRAM Write Command
						qspi_status 	<= QWR00	;		// back to state "QWR00" 
					end
					else 
					begin									// RAM0 is busy!
						qspi_status 	<= qspi_status;	// stay this state					
					end
				end
			end
			else
			begin 								// now, We are NOT on end of address of RAM0 nor RAM1.
				//QUAD_nCE_o 		<= nCE_SEL	;	// activate nCE
				qspi_command	<= QPIWR	;	// QSPI SRAM Write Command
				qspi_status 	<= QWR00	;	// back to state "QWR00" 
			end			
		end // EXEC10

		default :begin
			QUAD_oe_o			<=	1'b1	;	// output mode
			QUAD_nCE_o 			<= nCE_DES	;	// deactivate nCE
			qspi_status			<= QRSET	;	// reset
		end
		endcase
	end // qspi_write_mode
	else
	if (qspi_read_mode) begin
		case(qspi_status)
		QRSET :begin 

			//if (qspi_tx_go_flag == 1'b1) begin // GO-FLAG says GO.
				write_fbram_sig	<= 1'b0			;
				read_fbram_sig	<= 1'b0			;
				QUAD_oe_o		<= 1'b1			;	// QSPI output mode
				QUAD_nCE_o 		<= nCE_DES		;	// deselect nCE
				qspi_command	<= QPIRD		;	// read Command
				txrx_fbram_ch	<= 1'b0			;	// 0:RAM2, 1: RAM3
				qspi_status		<= QRD00		;	// next status
			// end

			// else begin							// GO-FLAG says STAY.
			// 	write_fbram_sig	<= 1'b0			;
			// 	read_fbram_sig	<= 1'b0			;
			// 	QUAD_nCE_o 		<= nCE_DES		;
			// 	qspi_status		<= qspi_status	; 	// stop until qspi_tx_go_flag == 1'b1
			// end
		end
		// send qspi_command[7]~[0]
		QRD00,QRD01,QRD02,QRD03,
		QRD04,QRD05,QRD06,QRD07:begin	
			if (qspi_tx_go_flag == 1'b1) begin // GO-FLAG says GO.	
				QUAD_Out_o[0]		<= qspi_command[7]				;
				QUAD_nCE_o 			<= nCE_SEL						;
				qspi_command		<= {qspi_command[6:0],1'b0}		;
				qspi_status			<= qspi_status + 8'b1			;
			end
			else 
			begin
				qspi_status			<= qspi_status;
			end
		end

		// QRADR0-5 : Send 24-bit Address
		QRADR0,QRADR1,QRADR2,
		QRADR3,QRADR4,QRADR5 :begin									
			QUAD_Out_o[3:0]		<= qspi_addr[23:20]				;
			qspi_addr			<= {qspi_addr[19:0],4'h0}		; // 4bit shift
			qspi_status			<= qspi_status + 8'b1			; 
		end
		// wait cycle (6 cycle)
		WCYC00,WCYC01,WCYC02,
		WCYC03,WCYC04,WCYC05 :begin
			QUAD_oe_o			<=	1'b0						; // QSPI input mode
			qspi_status			<= qspi_status + 8'b1			; 
		end

		// read from QSPI SRAM (total 32bit)
		EXEC20,EXEC21,EXEC22,
		EXEC23,EXEC24,EXEC25,EXEC26 :begin
			write_fbram_sig			<= 1'b0 				;
			txrx_fbram_data[31:0]	<= {txrx_fbram_data[27:0], QUAD_Out_o[3:0]}	;	// 4bit shift
			qspi_status				<= qspi_status + 8'b1	;	
		end

		EXEC27 :begin										
			txrx_fbram_data			<= {txrx_fbram_data[27:0],QUAD_Out_o[3:0]}	;
			txrx_fbram_addr 		<= (txrx_fbram_addr + 11'h01) % 11'd1024  ;
			write_fbram_sig			<= 1'b1 						;
			qspi_addr_next 			<= qspi_addr_next + 22'd4 		;					// 4-byte countup
			qspi_status				<= (qspi_addr_next[8:0]==9'h1fC)? EXEC28 : EXEC20;	// 512byte burst finished? (h'1FC = d'512 - d'4)		
		end

		EXEC28: begin
			QUAD_Out_o[3:0]			<= 4'h00			;
			write_fbram_sig			<= 1'b0 			;
			QUAD_nCE_o 				<= nCE_DES			;	// deactivate nCE
			qspi_addr				<= qspi_addr_next	;
			qspi_status				<= EXEC29			; 
		end

		// flag = 0
		EXEC29 :begin
				qspi_status 		<= EXEC30			;
				QUAD_oe_o			<= 1'b1				;	// QSPI output mode
					
		end

		// now, We are on end of address of RAM2 or RAM3 ?
		EXEC30 :begin 
			if (txrx_fbram_addr[8:0] == 9'h00)	
			begin 
				if(qspi_tx_go_flag == 1'b0)					// GO-flag = 0 ?
				begin										// Yes!
					//QUAD_nCE_o 		<= nCE_SEL			;	// activate nCE
					txrx_fbram_ch 	<= ~txrx_fbram_ch	;	// change to RAM3
					qspi_command	<= QPIRD			;	// QSPI SRAM Read Command
					qspi_status 	<= QRD00			;	// back to state "QRD00" 
				end
				else
				begin 									// wait until GO-Flag = 0
					
					qspi_status 	<= qspi_status ; 	// stay this state
				end
			end
			else
			begin 								// now, We are NOT on end of address of RAM0 nor RAM1.
				//QUAD_nCE_o 		<= nCE_SEL	;	// activate nCE
				qspi_command	<= QPIRD	;	// QSPI SRAM Read Command
				qspi_status 	<= QRD00	;	// back to state "QRD00" 
			end			
		end // EXEC30


		default :begin
			QUAD_oe_o			<=	1'b1			;	// output mode
			QUAD_nCE_o 			<= nCE_DES			;	// deactivate nCE
			qspi_status			<= QRSET			;	// reset
		end
		endcase
	end //qspi_read_mode
	// else if(qspi_reset_mode) begin // todo: add this
	/*
	
	
	*/	
	// end
	else begin
		QUAD_oe_o			<=	1'b1	;	// output mode
		QUAD_nCE_o 			<= nCE_DES	;	// deactivate nCE
		qspi_status			<= QRSET	;	// reset
	end
end

assign select_ram2		= (txrx_fbram_addr[10:9] == 2'b00); // 
assign select_ram3		= (txrx_fbram_addr[10:9] == 2'b01); // 
assign cam_push_sig2	= write_fbram_sig & select_ram2;
assign cam_push_sig3	= write_fbram_sig & select_ram3;


/* QSPI SRAM - RAM interface: end */


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


assign WBs_RAM0_DAT_o = 32'h0;//RAM0_Dat_out;

r512x32_512x32 RAM0_INST (	
			.WA(cam_ram_cnt[8:0]), 		//.WA(WBs_ADR_i[8:0]),
			.RA(txrx_fbram_addr[8:0]),	//.RA(WBs_ADR_i[8:0]),
			.WD(cam_reg_out),			//.WD(WBs_DAT_i[31:0]),
			.WClk(cam_push_clk),		//.WClk(WBs_CLK_i),
			.RClk(read_fbram_clk),		//.RClk(WBs_CLK_i),
			.WClk_En(1'b1),//.WClk_En(1'b1),
			.RClk_En(1'b1),
			.WEN({cam_push_sig0,cam_push_sig0,cam_push_sig0,cam_push_sig0}),
			//.WEN({FB_RAM0_Wr_Dcd,FB_RAM0_Wr_Dcd,FB_RAM0_Wr_Dcd,FB_RAM0_Wr_Dcd}),
			.RD(RAM0_Dat_out)
			);

assign WBs_RAM1_DAT_o = 32'h0;//{RAM1_Dat_out};

r512x32_512x32 RAM1_INST (	
			.WA(cam_ram_cnt[8:0]), 		//.WA(WBs_ADR_i[8:0]),
			.RA(txrx_fbram_addr[8:0]),	//.RA(WBs_ADR_i[8:0]),
			.WD(cam_reg_out),			//.WD(WBs_DAT_i[31:0]),
			.WClk(cam_push_clk),		//.WClk(WBs_CLK_i),
			.RClk(read_fbram_clk),		//.RClk(WBs_CLK_i),
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
			.WD(txrx_fbram_data),			//.WD(WBs_DAT_i[31:0]),
			.WClk(write_fbram_clk),		//.WClk(WBs_CLK_i),
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
			.WD(txrx_fbram_data),			//.WD(WBs_DAT_i[31:0]),
			.WClk(write_fbram_clk),		//.WClk(WBs_CLK_i),
			.RClk(WBs_CLK_i),
			.WClk_En(1'b1),
			.RClk_En(1'b1),
			.WEN({cam_push_sig3,cam_push_sig3,cam_push_sig3,cam_push_sig3}),
			//.WEN({FB_RAM3_Wr_Dcd,FB_RAM3_Wr_Dcd,FB_RAM3_Wr_Dcd,FB_RAM3_Wr_Dcd}),
			.RD(RAM3_Dat_out)
			);

// status register
reg [31:0] WBs_RAM_STATUS_i;	//status written by Cortex-M4F
wire [31:0] WBs_RAM_STATUS_i;


always @( posedge WBs_CLK_i or posedge WBs_RST_i)begin
	if(WBs_RST_i) begin
		WBs_RAM_STATUS_i <= 32'h0;		
	end
	else if(FB_STATUS_Wr_Dcd) begin
		WBs_RAM_STATUS_i <= WBs_DAT_i[31:0];
	end

end

assign WBs_RAM_STATUS_o = {24'h0, 7'h0, VSYNCI, 4'h0,qspi_tx_fin,1'b0, cam_ram_cnt[10:9]}; // status written by FPGA
			
endmodule