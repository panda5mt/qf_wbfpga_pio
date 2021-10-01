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
wire                     WBs_CLK_i     ;  // Wishbone FPGA Clock
wire                     WBs_RST_i     ;  // Wishbone FPGA Reset

// Wishbone Bus Signals
//
wire    [10:0]  WBs_ADR_i     ;  // Wishbone Address Bus
wire                     WBs_RAM0_CYC_i;  
wire                     WBs_RAM1_CYC_i;
wire                     WBs_RAM2_CYC_i;
wire                     WBs_RAM3_CYC_i;
wire                     WBs_STATUS_CYC_i;
wire              [3:0]  WBs_BYTE_STB_i;  // Wishbone Byte   Enables
wire                     WBs_WE_i      ;  // Wishbone Write  Enable Strobe
wire                     WBs_STB_i     ;  // Wishbone Transfer      Strobe
wire    [DATAWIDTH-1:0]  WBs_DAT_i     ;  // Wishbone Write  Data Bus
 
reg                      WBs_ACK_o     ;  // Wishbone Client Acknowledge

// SPI SRAMS
//
input	[3:0]	QUAD_In_i		;
output			QUAD_oe_o		;
input	[3:0]	QUAD_Out_o		;

wire	[3:0]	QUAD_In_i		;
wire			QUAD_oe_o		;
wire	[3:0]	QUAD_Out_o		;
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
				cam_reg_out	<= {cam_reg1[23:0],CAM_DAT[7:0]}; //cam_freerun[31:0];
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
assign select_ram2		= (cam_ram_cnt[10:9] == 2'b10); // 1024 =< cam_ram_cnt < 1536 
assign select_ram3		= (cam_ram_cnt[10:9] == 2'b11); // 1536 =< cam_ram_cnt < 2048
assign cam_push_clk		= cam_reg_rdy & ~(PCLKI) ;

assign cam_push_sig0	= cam_reg_rdy & select_ram0;
assign cam_push_sig1	= cam_reg_rdy & select_ram1;
assign cam_push_sig2	= cam_reg_rdy & select_ram2;
assign cam_push_sig3	= cam_reg_rdy & select_ram3;
/* CAMERA - RAM interface: end */


/* QSPI SRAM - RAM interface: begin */
/* FSM */
reg		[7:0]	qsram_status	;
wire	[7:0]	qsram_status	;
reg		[7:0]	qsram_command	;
wire	[7:0]	qsram_command	;
reg		[23:0]	qsram_addr		;
wire	[23:0]	qsram_addr		;
reg		[23:0]	qsram_addr_next	;
wire	[23:0]	qsram_addr_next	;

wire 	[10:0]	read_fbram_addr	;
reg		[10:0]	read_fbram_addr	;

wire 			read_fbram_ch	;
reg				read_fbram_ch	;
wire 			read_fbram_sig	; // read signal
reg				read_fbram_sig	;
wire 	[31:0]	read_fbram_data	;
reg		[31:0]	read_fbram_data	;
wire 			read_fbram_clk	;

assign read_fbram_clk = PCLKI & read_fbram_sig ;

localparam QRSET 	= 8'hFF;	// RESET
localparam QWR00 	= 8'd0;		// Write Command bit[7] send 
// ........
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
localparam EXEC9 	= 8'd23;	// check FB_RAM address whether end of RAM0 or RAM1

// QSPI SRAM's parameter 
localparam QPIWR 	= 8'b0011_1000;	// Quad Write Command (8'h38)
localparam QPIRD	= 8'b1110_1011;	// Quad Read Command  (8'hEB)
localparam STADR	= 24'h04;		// Quad Start Address (24bit)

// select state-machine parameter 
wire qsram_write_mode;	// RAM0,1 -> QSPI SRAM
wire qsram_sram_mode;	// QSPI SRAM -> RAM2,3

assign qsram_write_mode	= (WBs_RAM_STATUS_i[1:0] == 2'b10);
assign qsram_read_mode	= (WBs_RAM_STATUS_i[1:0] == 2'b01);


always @( negedge PCLKI or posedge WBs_RST_i) begin // todo: change pclki
	if(WBs_RST_i)begin
		qsram_status		<= QRSET;
		qsram_command		<= QPIRD;	// Read Command
		QUAD_oe_o			<= 1'b1;	// OE = 1 Output, OE=0 input
		qsram_addr			<= STADR;
		read_fbram_addr		<= 11'b0;
		qsram_addr_next		<= 22'b0;
	end // reset
	else 
	begin
		if (qsram_write_mode) begin
		casez(qsram_status)
		QRSET :begin
			if (select_ram0 == 1'b0) begin // camera writes ram1 memory not ram0
				// QPI_nCE <= 1'b0;
				// do not forget CE = 0
				qsram_command	<= QPIWR		;	// Write Command
				qsram_status	<= QWR00		;
				read_fbram_ch	<= 1'b0			;
			end
			else begin
				//QPI_nCE <= 1'b1;
				qsram_status	<= qsram_status	; 	// stop until select_ram0 == 1'b1
			end
		end

		// send qsram_command[7]~[0]
		8'b0000_0??? :begin		
			QUAD_Out_o[0]		<= qsram_command[7]				;
			qsram_command		<= {qsram_command[6:0],1'b0}	;
			qsram_status		<= qsram_status + 8'b1			;
		end

		// QWADR0-5 : Send 24-bit Address
		QWADR0,
		QWADR1,
		QWADR2,
		QWADR3 :begin									
			QUAD_Out_o[3:0]		<= qsram_addr[23:20]			;
			qsram_addr			<= {qsram_addr[19:0],4'h0}		; // 4bit shift
			qsram_status		<= qsram_status + 8'b1			; 
		end
		
		QWADR4 :begin									
			QUAD_Out_o[3:0]		<= qsram_addr[23:20]			;
			qsram_addr			<= {qsram_addr[19:0],4'h0}		; // 4bit shift
			read_fbram_sig		<= 1'b1							; // FB_RAM read signal
			qsram_status		<= QWADR5						; 
		end

		// get data from FB_RAM
		QWADR5 :begin									
			QUAD_Out_o[3:0]		<= qsram_addr[23:20]			;
			read_fbram_sig		<= 1'b0 						;
			read_fbram_data 	<= (read_fbram_addr[10:9]==2'b00)? RAM0_Dat_out : RAM1_Dat_out;
			qsram_status		<= EXEC0						; 
		end

		// EXEC0~7:32bit data -> QSPI SRAM
		EXEC0,EXEC1,EXEC2,
		EXEC3,EXEC4,EXEC5 :begin										
			QUAD_Out_o[3:0]		<= read_fbram_data[31:28]		;
			read_fbram_data		<= {read_fbram_data[27:0],4'b0}	;	// 4bit shift
			qsram_status		<= qsram_status + 8'b1			;			
		end
		
		EXEC6 :begin										
			QUAD_Out_o[3:0]		<= read_fbram_data[31:28]		;
			read_fbram_data		<= {read_fbram_data[27:0],4'b0}	;	// 4bit shift
			read_fbram_sig		<= 1'b1							;	// FB_RAM read signal
			qsram_status		<= EXEC7						;			
		end

		EXEC7 :begin										
			QUAD_Out_o[3:0]		<= read_fbram_data[31:28]		;
			read_fbram_addr 	<= (read_fbram_addr + 11'h01) % 11'd1024  ;
			read_fbram_sig		<= 1'b0 						;
			read_fbram_data 	<= (read_fbram_addr[10:9]==2'b00)? RAM0_Dat_out : RAM1_Dat_out;
			qsram_addr_next 	<= qsram_addr_next + 22'd4 		;					// 4-byte countup
			qsram_status		<= (qsram_addr_next[8:0]==9'h1ff)? EXEC8 : EXEC0;	// 512byte burst finished?		
		end

		EXEC8 :begin
			// QPI_nCE <=1 // deactivate nCE
			qsram_addr			<= qsram_addr_next				;
			qsram_status 		<= EXEC9						;
			
		end

		// now, We are on end of address of RAM0 or RAM1 ?
		EXEC9 :begin 
			if (read_fbram_addr[8:0] == 9'h00)	
			begin 
				if((read_fbram_ch == 1'b0) && (read_fbram_addr[10:9]==2'b01))	// selected RAM0 but next address is RAM1
				begin 
					if(select_ram1 == 1'b0)				// CAMERA module writting RAM0 ?
					begin								// Yes!
						// QPI_nCE <= 1'b0; 
						read_fbram_ch 	<= 1'b1;		// change to RAM1
						qsram_command	<= QPIWR;		// QSPI SRAM Write Command
						qsram_status 	<= QWR00;		// back to state "QWR00" 
					end
					else
					begin // RAM1 is busy!
						qsram_status 	<= qsram_status; // stay this state
					end
				end
				else 
				if((read_fbram_ch == 1'b1) && (read_fbram_addr[10:9]==2'b00))	// selected RAM1 but next address is RAM0
				begin
					if(select_ram0 == 1'b0)				// CAMERA module writting RAM1 ?
					begin								// Yes!
						// QPI_nCE <= 1'b0; 
						read_fbram_ch 	<= 1'b0;		// change to RAM0
						qsram_command	<= QPIWR;		// QSPI SRAM Write Command
						qsram_status 	<= QWR00;		// back to state "QWR00" 
					end
					else 
					begin	// RAM0 is busy!
						qsram_status 	<= qsram_status; // stay this state					
					end
				end
			end
			else
			begin 	// now, We are NOT on end of address of RAM0 nor RAM1.
				// QPI_nCE <= 1'b0; 
				qsram_command	<= QPIWR;		// QSPI SRAM Write Command
				qsram_status 	<= QWR00;		// back to state "QWR00" 
			end			
		end // EXEC9

		default :begin
			qsram_status		<= QRSET ; // reset
		end
		endcase
		end // qsram_write_mode
	end
end


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
			.RA(read_fbram_addr[8:0]),	//.RA(WBs_ADR_i[8:0]),
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
			.RA(read_fbram_addr[8:0]),	//.RA(WBs_ADR_i[8:0]),
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

assign WBs_RAM_STATUS_o = {24'h0, 7'h0, VSYNCI, 6'h0, cam_ram_cnt[10:9]}; // status written by FPGA
			
endmodule