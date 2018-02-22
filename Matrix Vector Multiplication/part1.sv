// Aishwarya Gandhi(111424452) & Kewal Raul(111688087)

module mvm3_part1(clk, reset, s_valid, m_ready, data_in, m_valid, s_ready, data_out, overflow);
input clk, reset, s_valid, m_ready;
input signed [7:0] data_in;
output logic signed [15:0] data_out;
output logic m_valid, s_ready, overflow;
logic [3:0]addr_m;
logic [1:0]addr_x;
logic [3:0] count_m,count_x;
logic signed [7:0] matrix_v, vector_v;
logic enable;

datapath d(clk,reset,data_out,s_valid,m_ready,m_valid,s_ready,overflow,addr_x, addr_m,data_in,enable);
endmodule

module memory(clk, data_in, data_out, addr, wr_en);
parameter WIDTH=16, SIZE=64, LOGSIZE=6;
input [WIDTH-1:0] data_in;
output logic [WIDTH-1:0] data_out;
input [LOGSIZE-1:0] addr;
input clk, wr_en;
logic [SIZE-1:0][WIDTH-1:0] mem;
always_ff @(posedge clk) begin
data_out <= mem[addr];
if (wr_en)
mem[addr] <= data_in;
end
endmodule



module ctrlpath(clk,reset,s_valid,m_ready,s_ready,wr_en_x, wr_en_m,count_m,count_x,addr_m,addr_x,enable);
input logic clk, reset, s_valid, m_ready;
output logic wr_en_x, wr_en_m;
output logic [3:0] count_m,count_x;
output logic [3:0]addr_m;
output logic [1:0]addr_x;
input enable;
input logic s_ready;

always_comb
begin
	if(s_valid==1 && s_ready==1)
	begin
		if(count_m<9)
		begin			
		wr_en_m = 1;
		wr_en_x = 0;
		end

		else if(count_m>=9 && count_x<3)
		begin			
		wr_en_m = 0;
		wr_en_x = 1;
		end
		
		else
		begin
		wr_en_m = 0;
		wr_en_x = 0;

		end
	end

	else 
	begin
		wr_en_m = 0;
		wr_en_x = 0;

	end	
end


//Writing
always_ff @(posedge clk)
	begin	

		if(reset==1)
		begin	
			addr_m <= 0;
			addr_x <=0;
			count_m <= 0;
			count_x <= 0;
		end
		else
		begin
		
			if(wr_en_x==0 && wr_en_m==1 && count_m<9 && s_ready==1)	
			begin
				addr_m <= addr_m+ 1;
				count_m <= count_m+ 1;
			end

			if(wr_en_m==0 && wr_en_x==1 && count_x<3 &&  s_ready==1)	
			begin
				addr_x <= addr_x+ 1;
				count_x <= count_x+ 1;
			end

//Reading
			if(count_m>=9 && count_x>=3 && s_ready==0)
			begin
				count_m <= 0;
				count_x <= 0;
				addr_m <= 0;
				addr_x <= 0;
			end

			if(s_ready==0 )
			begin
			if(wr_en_m==0 && count_m<9 && enable==1)	
				begin
					if(wr_en_x==0 && count_x<3 && enable==1)
					begin
					addr_m <= addr_m+ 1;
					count_m <= count_m+ 1;
					addr_x <= addr_x+ 1;
					count_x <= count_x+ 1;
					end
				end
			if(count_x==3)
				begin
				count_x<=0;
				addr_x<=0;
				end
			end
		end
	end
	

endmodule


module datapath(clk,reset,data_out,s_valid,m_ready,m_valid,s_ready,overflow,addr_x, addr_m,data_in,enable);
input clk, reset, s_valid, m_ready;
input signed [7:0] data_in;
output logic signed [15:0] data_out;
output logic m_valid, s_ready, overflow;
logic signed [7:0] matrix_v, vector_v;
logic signed [15:0]q,temp;
logic [3:0] count_m,count_x;
logic signed [15:0] f,acc;
output [3:0] addr_m;
output [1:0] addr_x;
logic wr_en_x, wr_en_m;
output logic enable;
logic [3:0] count_dm,count_dx;
logic enable_f;


ctrlpath c(clk,reset,s_valid,m_ready,s_ready,wr_en_x, wr_en_m,count_m,count_x,addr_m,addr_x,enable);
memory #(8,9,4) m(clk, data_in, matrix_v, addr_m, wr_en_m);
memory #(8,3,2) x(clk, data_in, vector_v, addr_x, wr_en_x);

always_ff @(posedge clk)
	begin
		if(reset==1)
		enable<=0;
		else if (reset==0 && s_ready==0 && m_valid==0 && count_dx==0)        
		enable<=1;
		else
		enable<=0;

	end

always_comb begin
	if(reset==1)
		if(count_m==0)		
		s_ready = 1;
		else
		s_ready = 0;		
	else 
	begin
		if (count_m>=9 && count_x>=3)
		s_ready = 0;
		else if(count_dm==9)
		s_ready = 1;
	end

end

assign q=matrix_v*vector_v;
assign temp=f+q;

always_ff @(posedge clk)
	begin
	if(enable==1)
	enable_f<=1;
	else
	enable_f<=0;
	end


always_ff @(posedge clk)
	begin
		if(reset==1)
		begin	
		f<=0;
		m_valid<=0;
		count_dx<=0;
		count_dm<=0;
		end

		else 
			begin
			if(enable_f==1 && count_dx<3)
			begin	
				f<=f+q;
				count_dx<=count_dx+1;
				count_dm<=count_dm+1;

				
				if(count_dx==2)
				begin
				m_valid<=1;
				count_dx<=0;
				end
				

				
			end
			if(count_dm==9)
			count_dm <=0;
			
			if(m_ready==1 && m_valid==1)
			begin
			f <= 0;
			m_valid <= 0;
			end
		end

	end

always_ff @(posedge clk,posedge reset)
	begin
	if(reset==1)
	begin	
		overflow<=0;
		
	end

	else
		
		if((f!=0)&&(temp[15]!=f[15])&&(q[15]==f[15]))
		overflow<=1;

		else if(m_ready==1 && m_valid==1)
		overflow<=0;
	 
		else
		begin

			if(overflow==1)
			overflow<=1;
			else
			overflow<=0;
		end 	
			
	end 

always_comb
	begin
	if(m_ready==1 && m_valid==1)
		begin
		data_out = f;
		end
	else
	data_out=0;
	end

	

endmodule 

// Testbench using c file

/*module check_timing();

   logic clk, s_valid, s_ready, m_valid, m_ready, reset, overflow;
   logic signed [7:0] data_in;
   logic signed [15:0] data_out;
   
   initial clk=0;
   always #5 clk = ~clk;
   

   mvm3_part1 dut (.clk(clk), .reset(reset), .s_valid(s_valid), .m_ready(m_ready), 
		   .data_in(data_in), .m_valid(m_valid), .s_ready(s_ready), .data_out(data_out),
		   .overflow(overflow));


   //////////////////////////////////////////////////////////////////////////////////////////////////
   // code to feed some test inputs

   // rb and rb2 represent random bits. Each clock cycle, we will randomize the value of these bits.
   // When rb is 0, we will not let our testbench send new data to the DUT.
   // When rb is 1, we can send data.
   logic rb, rb2;
   always begin
      @(posedge clk);
      #1;
      std::randomize(rb, rb2); // randomize rb
   end

   logic [7:0] invals[120:0];
   initial $readmemh("inputData1", invals);
   
   logic [15:0] j;

   // If s_valid is set to 1, we will put data on data_in.
   // If s_valid is 0, we will put an X on the data_in to test that your system does not 
   // process the invalid input.
   always @* begin
      if (s_valid == 1)
         data_in = invals[j];
      else
         data_in = 'x;
   end

   // If our random bit rb is set to 1, and if j is within the range of our test vector (invals),
   // we will set s_valid to 1.
   always @* begin
      if ((j>=0) && (j<120) && (rb==1'b1)) begin
         s_valid=1;
      end
      else
         s_valid=0;
   end

   // If we set s_valid and s_ready on this clock edge, we will increment j just after
   // this clock edge.
   always @(posedge clk) begin
      if (s_valid && s_ready)
         j <= #1 j+1;
   end
   ////////////////////////////////////////////////////////////////////////////////////////
   // code to receive the output values

   // we will use another random bit (rb2) to determine if we can assert m_ready.
   logic [15:0] i;
   always @* begin
      if ((i>=0) && (i<120) && (rb2==1'b1))
         m_ready = 1;
      else
         m_ready = 0;
   end
	 
integer filehandle=$fopen("outValues");
   always @(posedge clk) begin
      if (m_ready && m_valid) begin
	$fdisplay(filehandle, "%d", data_out);	
	i=i+1;
      end 
   end

   ////////////////////////////////////////////////////////////////////////////////

   initial begin
      j=0; i=0;

      // Before first clock edge, initialize
      m_ready = 0; 
      reset = 0;
   
      // reset
      @(posedge clk); #1; reset = 1; 
      @(posedge clk); #1; reset = 0; 

      // wait until 30 outputs have come out, then finish.
      wait(i==30);
      $fclose(filehandle);
      $finish;
   end

endmodule*/

//Simple testbench to check overflow

/*module check_timing();

   logic clk, s_valid, s_ready, m_valid, m_ready, reset, overflow;
   logic signed [7:0] data_in;
   logic signed [15:0] data_out;
   
   initial clk=0;
   always #5 clk = ~clk;
   

   mvm3_part1 dut (.clk(clk), .reset(reset), .s_valid(s_valid), .m_ready(m_ready), 
		   .data_in(data_in), .m_valid(m_valid), .s_ready(s_ready), .data_out(data_out),
		   .overflow(overflow));


   //////////////////////////////////////////////////////////////////////////////////////////////////
   // code to feed some test inputs

   // rb and rb2 represent random bits. Each clock cycle, we will randomize the value of these bits.
   // When rb is 0, we will not let our testbench send new data to the DUT.
   // When rb is 1, we can send data.
   logic rb, rb2;
   always begin
      @(posedge clk);
      #1;
      std::randomize(rb, rb2); // randomize rb
   end

   // Put our test data into this array. These are the values we will feed as input into the system.
   logic [7:0] invals[0:35] = '{1, -8, 3, 9, -5, 11, -7, 8, -9, 1,  -22, 3, 
				10, 11, 12, 127, 127, 127, 1,  2, 3, 127, 127, 127,
				19, 18, -17,  16,  -15,  14, 13, -12, 11, 19, -22, 27 
				};

   
   
   logic signed [15:0] expVals[0:8]  = {186, 152, -210, 4191, -17149, 762, -494, 1012, 808};
   logic 	expOverflow[0:8] = {0, 0, 0, 0, 1, 0, 0, 0, 0};
   
   logic [15:0] j;

   // If s_valid is set to 1, we will put data on data_in.
   // If s_valid is 0, we will put an X on the data_in to test that your system does not 
   // process the invalid input.
   always @* begin
      if (s_valid == 1)
         data_in = invals[j];
      else
         data_in = 'x;
   end

   // If our random bit rb is set to 1, and if j is within the range of our test vector (invals),
   // we will set s_valid to 1.
   always @* begin
      if ((j>=0) && (j<36) && (rb==1'b1)) begin
         s_valid=1;
      end
      else
         s_valid=0;
   end

   // If we set s_valid and s_ready on this clock edge, we will increment j just after
   // this clock edge.
   always @(posedge clk) begin
      if (s_valid && s_ready)
         j <= #1 j+1;
   end
   ////////////////////////////////////////////////////////////////////////////////////////
   // code to receive the output values

   // we will use another random bit (rb2) to determine if we can assert m_ready.
   logic [15:0] i;
   always @* begin
      if ((i>=0) && (i<36) && (rb2==1'b1))
         m_ready = 1;
      else
         m_ready = 0;
   end

   always @(posedge clk) begin
      if (m_ready && m_valid) begin
	 if ((data_out == expVals[i]) && (overflow == expOverflow[i]))
           $display("SUCCESS:          y[%d] = %d; overflow = %b" , i, data_out, overflow);
	 else
	   $display("ERROR:   Expected y[%d] = %d; overflow = %b.   Instead your system produced: y[%d] = %d; overflow = %b" , i, expVals[i], expOverflow[i], i, data_out, overflow);
         i=i+1; 
      end 
   end

   ////////////////////////////////////////////////////////////////////////////////

   initial begin
      j=0; i=0;

      // Before first clock edge, initialize
      m_ready = 0; 
      reset = 0;
   
      // reset
      @(posedge clk); #1; reset = 1; 
      @(posedge clk); #1; reset = 0; 

      // wait until 3 outputs have come out, then finish.
      wait(i==9);
      $finish;
   end


   // This is just here to keep the testbench from running forever in case of error.
   // In other words, if your system never produces three outputs, this code will stop 
   // the simulation after 1000 clock cycles.
   initial begin
      repeat(1000) begin
         @(posedge clk);
      end
      $display("Warning: Output not produced within 1000 clock cycles; stopping simulation so it doens't run forever");
      $stop;
   end

endmodule
/*
