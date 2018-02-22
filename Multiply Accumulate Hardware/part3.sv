// Aishwarya Gandhi(111424452)
//Kewal Raul(111688087)


module part2_mac(clk, reset, a, b, valid_in, f, overflow,
valid_out);
input clk, reset, valid_in;
input signed [7:0] a, b;
output logic signed [15:0] f;
output logic valid_out, overflow;
logic signed [7:0] q1,q2;
logic signed [15:0]q3,qf;
logic enable,enable_f;
logic signed [15:0] temp;


always_ff @(posedge clk, posedge reset) 
	begin
		if((reset==1))
		begin
			enable<=0;
			q1<=0;
			q2<=0;
		end
		else
		begin
			
			if(valid_in==1)
			begin
				enable<=1;
				q1<=a;
				q2<=b;
			end
			else	
			begin
				enable<=0;
				q1<=0;
				q2<=0;
			end
			
		end
	end

assign q3=q1*q2;
assign temp=f+qf;

always_ff @(posedge clk,posedge reset)
	begin
		if(reset==1)
			begin
			qf<=0;
			enable_f<=0;
			end
		else
			if(enable==1)
				begin		
				qf<=q3;	
				enable_f<=1;	
				end		
			else
				begin		
				qf<=0;	
				enable_f<=0;	
				end			
	end

always_ff @(posedge clk,posedge reset)
	begin
	if(reset==1)
		begin
		f<=0;
		valid_out<=0;
		end	
		
	else 
		begin
			if(enable_f==1)
			begin	
				f<=f+qf;
				valid_out<=1;
			end
			else
			begin
				
				valid_out<=0;
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
		
		if((f!=0)&&(temp[15]!=f[15])&&(qf[15]==f[15]))
		overflow<=1;
		else
		begin
			if(overflow==1)
			overflow<=1;
			else
			overflow<=0;
		end 	
			
	end 	

endmodule		


module tb_part2_mac();

   logic clk, reset, valid_in, valid_out, overflow;
   logic signed [7:0] a, b;
   logic signed [15:0] f;


   part2_mac dut(.clk(clk), .reset(reset), .a(a), .b(b), .valid_in(valid_in), .f(f), .overflow(overflow), .valid_out(valid_out));

   initial clk = 0;
   always #5 clk = ~clk;

   initial begin
       
      reset = 1;
      {a, b} = {8'b0,8'b0};
      valid_in = 0;

      @(posedge clk);
      #1; // After 1 posedge
      reset = 0; a = 2; b = 2; valid_in = 1;
      @(posedge clk);
      #1; // After 2 posedges
      a = 3; b = -3; valid_in = 1;
      @(posedge clk);
      #1; // After 3 posedges
      a = 5; b = 5; valid_in = 0;
      @(posedge clk);
      #1; // After 4 posedges
      a = 50; b = 15; valid_in = 1;
      @(posedge clk);
      #1; // After 5 posedges
      a = 100; b = 2; valid_in = 1;
      @(posedge clk);
      @(posedge clk);
      @(posedge clk);
      #1; // After 6 posedges  
      reset=1;	
      @(posedge clk);
      #1; // After 7 posedges
      reset=0;a = 125; b =100; valid_in = 1;
      @(posedge clk);
      #1; // After 8 posedges
      a = 125; b = 100; valid_in = 1;
      @(posedge clk);
      #1; // After 9 posedges
      a = 125; b = 100; valid_in = 1;
      @(posedge clk);
      #1; // After 10 posedges
      a = 100; b = 2; valid_in = 1;
      @(posedge clk);
      @(posedge clk);
      @(posedge clk);
      #1; // After 12 posedges  
      reset=1;
      @(posedge clk);
      #1; // After 13 posedges
      reset=0; a = 2;b = 2; valid_in = 1;
      @(posedge clk);
      #1; // After 14 posedges
      a = -3; b = 3; valid_in = 1;
      @(posedge clk);
      #1; // After 15 posedges
      a = -20; b =30; valid_in = 1;
      @(posedge clk);
      #1; // After 16 posedges
      a = -125; b =120; valid_in = 1;
      @(posedge clk);
      #1; // After 17 posedges
      a = -125; b =120; valid_in = 1;
      @(posedge clk);
      #1; // After 18 posedges
      a = -125; b =120; valid_in = 1;
      @(posedge clk);
      #1; // After 19 posedges
      a = 25; b =120; valid_in = 1;


end


   initial begin
      @(posedge clk);
      #1; // After 1 posedge
      $display("valid_out = %b. Expected value is 0.", valid_out);
      $display("f = %d. Expected value is 0.", f);
      $display("overflow = %b Expected value is 0", overflow);
 
      @(posedge clk);
      #1; // After 2 posedges
      $display("valid_out = %b. Expected value is 0.", valid_out);
      $display("f = %d. Expected value is 0.", f);
      $display("overflow = %b Expected value is 0", overflow);

      @(posedge clk);
      #1; // After 3 posedges
      $display("valid_out = %b. Expected value is 0.", valid_out);
      $display("f = %d. Expected value is 0.", f);
      $display("overflow = %b Expected value is 0", overflow);

      @(posedge clk);
      #1; // After 4 posedges
      $display("valid_out = %b. Expected value is 1.", valid_out);
      $display("f = %d. Expected value is 4.", f);
      $display("overflow = %b Expected value is 0", overflow);

      @(posedge clk);
      #1; // After 5 posedges
      $display("valid_out = %b. Expected value is 1.", valid_out);
      $display("f = %d. Expected value is -5.", f);
      $display("overflow = %b Expected value is 0",overflow );

      @(posedge clk);
      #1; // After 6 posedges
      $display("valid_out = %b. Expected value is 0.", valid_out);
      $display("f = %d. Expected value is -5.", f);
      $display("overflow = %b Expected value is 0", overflow);

      @(posedge clk);
      #1; // After 7 posedges
      $display("valid_out = %b. Expected value is 1.", valid_out);
      $display("f = %d. Expected value is 745).", f);
      $display("overflow = %b Expected value is 0", overflow);

      @(posedge clk);
      #1; // After 8 posedges
      $display("valid_out = %b. Expected value is 1.", valid_out);
      $display("f = %d. Expected value is 945.", f);
	$display("overflow = %b Expected value is 0", overflow);

      @(posedge clk);
      #1; // After 9 posedges
      $display("valid_out = %b. Expected value is 0.", valid_out);
      $display("f = %d. Expected value is 0.", f);
      $display("overflow = %b Expected value is 0", overflow);

      @(posedge clk);
      #1; // After 10 posedges
      $display("valid_out = %b. Expected value is 0.", valid_out);
      $display("f = %d. Expected value is 0", f);
      $display("overflow = %b Expected value is 0", overflow);

      @(posedge clk);
      #1; // After 11 posedges
      $display("valid_out = %b. Expected value is 0.", valid_out);
      $display("f = %d. Expected value is 0", f);
      $display("overflow = %b Expected value is 0", overflow);


      @(posedge clk);
      #1; // After 12 posedges
      $display("valid_out = %b. Expected value is 1.", valid_out);
      $display("f = %d. Expected value is 12500.", f);
      $display("overflow = %b Expected value is 0", overflow);

      @(posedge clk);
      #1; // After 13 posedges
      $display("valid_out = %b. Expected value is 1.", valid_out);
      $display("f = %d. Expected value is 25000.", f);
      $display("overflow = %b Expected value is 0", overflow);

      @(posedge clk);
      #1; // After 14 posedges
      $display("valid_out = %b. Expected value is 1.", valid_out);
      $display("f = %d. Expected value is -28036.", f);
      $display("overflow = %b Expected value is 1", overflow);

      @(posedge clk);
      #1; // After 15 posedges
      $display("valid_out = %b. Expected value is 1.", valid_out);
      $display("f = %d. Expected value is -27836.", f);
      $display("overflow = %b Expected value is 1", overflow);

      @(posedge clk);
      #1; // After 16 posedges
      $display("valid_out = %b. Expected value is 0.", valid_out);
      $display("f = %d. Expected value is 0.", f);
      $display("overflow = %b Expected value is 0", overflow);

      @(posedge clk);
      #1; // After 17 posedges
      $display("valid_out = %b. Expected value is 0.", valid_out);
      $display("f = %d. Expected value is 0.", f);
      $display("overflow = %b Expected value is 0", overflow);

      @(posedge clk);
      #1; // After 18 posedges
      $display("valid_out = %b. Expected value is 0.", valid_out);
      $display("f = %d. Expected value is 0.", f);
      $display("overflow = %b Expected value is 0", overflow);

      @(posedge clk);
      #1; // After 19 posedges
      $display("valid_out = %b. Expected value is 1.", valid_out);
      $display("f = %d. Expected value is 4.", f);
      $display("overflow = %b Expected value is 0", overflow);

      @(posedge clk);
      #1; // After 20 posedges
      $display("valid_out = %b. Expected value is 1.", valid_out);
      $display("f = %d. Expected value is -5.", f);
      $display("overflow = %b Expected value is 0", overflow);

      @(posedge clk);
      #1; // After 21 posedges
      $display("valid_out = %b. Expected value is 1.", valid_out);
      $display("f = %d. Expected value is -605.", f);
      $display("overflow = %b Expected value is 0", overflow);

      @(posedge clk);
      #1; // After 22 posedges
      $display("valid_out = %b. Expected value is 1.", valid_out);
      $display("f = %d. Expected value is -15605.", f);
      $display("overflow = %b Expected value is 0", overflow);

      @(posedge clk);
      #1; // After 23 posedges
      $display("valid_out = %b. Expected value is 1.", valid_out);
      $display("f = %d. Expected value is -30605", f);
      $display("overflow = %b Expected value is 1", overflow);

      @(posedge clk);
      #1; // After 24 posedges
      $display("valid_out = %b. Expected value is 1.", valid_out);
      $display("f = %d. Expected value is 19931.", f);
      $display("overflow = %b Expected value is 1", overflow);

      @(posedge clk);
      #1; // After 25 posedges
      $display("valid_out = %b. Expected value is 1.", valid_out);
      $display("f = %d. Expected value is 22931.", f);
      $display("overflow = %b Expected value is 1", overflow); 
  
      #20;
      $finish;

   end

endmodule // tb_part2_mac

