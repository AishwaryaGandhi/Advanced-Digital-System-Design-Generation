// ESE 507 Project 3 Handout Code
// Fall 2017
// Peter Milder

// Getting started:
// The main() function contains the code to read the parameters. 
// For Parts 1 and 2, your code should be in the genLayer() function. Please
// also look at this function to see an example for how to create the ROMs.
//p1
// For Part 3, your code should be in the genAllLayers() function.


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <assert.h>
#include <math.h>
#include <map>
#include <algorithm>
using namespace std;

void printUsage();
void genLayer(int M, int N, int P, int bits, vector<int>& constvector, string modName, ofstream &os);
void genAllLayers(int N, int M1, int M2, int M3, int mult_budget, int bits, vector<int>& constVector, string modName, ofstream &os);
void readConstants(ifstream &constStream, vector<int>& constvector);
void genROM(vector<int>& constVector, int bits, string modName, ofstream &os);
int optimize_P(int M, int P,int L, int P_sum);

int done = 0;

int main(int argc, char* argv[]) {

   // If the user runs the program without enough parameters, print a helpful message
   // and quit.
   if (argc < 7) {
      printUsage();
      return 1;
   }

   int mode = atoi(argv[1]);

   ifstream const_file;
   ofstream os;
   vector<int> constVector;

   //----------------------------------------------------------------------
   // Look here for Part 1 and 2
   if ((mode == 1) && (argc == 7)) {
      // Mode 1: Generate one layer with given dimensions and one testbench

      // --------------- read parameters, etc. ---------------
      int M = atoi(argv[2]);
      int N = atoi(argv[3]);
      int P = atoi(argv[4]);
      int bits = atoi(argv[5]);
      const_file.open(argv[6]);
      if (const_file.is_open() != true) {
         cout << "ERROR reading constant file " << argv[6] << endl;
         return 1;
      }

      // Read the constants out of the provided file and place them in the constVector vector
      readConstants(const_file, constVector);

      string out_file = "layer_" + to_string(M) + "_" + to_string(N) + "_" + to_string(P) + "_" + to_string(bits) + ".sv";

      os.open(out_file);
      if (os.is_open() != true) {
         cout << "ERROR opening " << out_file << " for write." << endl;
         return 1;
      }
      // -------------------------------------------------------------

      // call the genLayer function you will write to generate this layer
      string modName = "layer_" + to_string(M) + "_" + to_string(N) + "_" + to_string(P) + "_" + to_string(bits);
      genLayer(M, N, P, bits, constVector, modName, os); 

   }
   //--------------------------------------------------------------------


   // ----------------------------------------------------------------
   // Look here for Part 3
   else if ((mode == 2) && (argc == 9)) {
      // Mode 2: Generate three layer with given dimensions and interconnect them

      // --------------- read parameters, etc. ---------------
      int N  = atoi(argv[2]);
      int M1 = atoi(argv[3]);
      int M2 = atoi(argv[4]);
      int M3 = atoi(argv[5]);
      int mult_budget = atoi(argv[6]);
      int bits = atoi(argv[7]);
      const_file.open(argv[8]);
      if (const_file.is_open() != true) {
         cout << "ERROR reading constant file " << argv[8] << endl;
         return 1;
      }
      readConstants(const_file, constVector);

      string out_file = "network_" + to_string(N) + "_" + to_string(M1) + "_" + to_string(M2) + "_" + to_string(M3) + "_" + to_string(mult_budget) + "_" + to_string(bits) + ".sv";


      os.open(out_file);
      if (os.is_open() != true) {
         cout << "ERROR opening " << out_file << " for write." << endl;
         return 1;
      }
      // -------------------------------------------------------------

      string mod_name = "network_" + to_string(N) + "_" + to_string(M1) + "_" + to_string(M2) + "_" + to_string(M3) + "_" + to_string(mult_budget) + "_" + to_string(bits);

      // call the genAllLayers function
      genAllLayers(N, M1, M2, M3, mult_budget, bits, constVector, mod_name, os);

   }
   //-------------------------------------------------------

   else {
      printUsage();
      return 1;
   }

   // close the output stream
   os.close();

}

// Read values from the constant file into the vector
void readConstants(ifstream &constStream, vector<int>& constvector) {
   string constLineString;
   while(getline(constStream, constLineString)) {
      int val = atoi(constLineString.c_str());
      constvector.push_back(val);
   }
}

// Generate a ROM based on values constVector.
// Values should each be "bits" number of bits.
void genROM(vector<int>& constVector, int bits, string modName, ofstream &os) {

      int numWords = constVector.size();
      int addrBits = ceil(log2(numWords));

      os << "module " << modName << "(clk, addr, z);" << endl;
      os << "   input clk;" << endl;
      os << "   input [" << addrBits-1 << ":0] addr;" << endl;
      os << "   output logic signed [" << bits-1 << ":0] z;" << endl;
      os << "   always_ff @(posedge clk) begin" << endl;
      os << "      case(addr)" << endl;
      int i=0;
      for (vector<int>::iterator it = constVector.begin(); it < constVector.end(); it++, i++) {
         if (*it < 0)
            os << "        " << i << ": z <= -" << bits << "'d" << abs(*it) << ";" << endl;
         else
            os << "        " << i << ": z <= "  << bits << "'d" << *it      << ";" << endl;
      }
      os << "      endcase" << endl << "   end" << endl << "endmodule" << endl << endl;
}

// Parts 1 and 2
// Here is where you add your code to produce a neural network layer.
void genLayer(int M, int N, int P, int bits, vector<int>& constVector, string modName, ofstream &os) {
	
	int i ,j;
	int F= M/P;
	std::string addr_m,addr_b ;
	
   // Make your module name: layer_M_N_P_bits, where these parameters are replaced with the
   // actual numbers
   //----------------------Top level Module ----------------------------------------
    os << "module " << modName << "(clk, reset, s_valid, m_ready, data_in, m_valid, s_ready, data_out);" << endl;
    os << "input clk, reset, s_valid, m_ready; "<< endl;
    os << "input signed ["<<bits-1<<":0] data_in; "<< endl;
    os << "output signed ["<<bits-1<<":0] data_out; "<< endl;
    os << "output m_valid, s_ready;"<< endl;
	
	for(i=1;i<=P;i++){
		 os << "logic ["<<ceil(log2(M*N))-1<<":0] addr_m"<<i<<"; "<< endl;
	}

    os << "logic ["<<ceil(log2(N))-1<<":0] addr_x;"<< endl;

	for(i=1;i<=P;i++){
		 os << "logic ["<<ceil(log2(M))-1<<":0] addr_b"<<i<<"; "<< endl;
	}
	
    os << "logic wr_en_x,accum_src, enable_accum,enable_src;"<< endl;
    os << "logic ["<<ceil(log2(N+1))<<":0] output_counter;"<< endl;
	os << "logic ["<<ceil(log2(P))<<":0] dataOutCounts;"<< endl;
	
	
	for(i=1;i<=P;i++){
		
	addr_m += "addr_m" + std::to_string(i) + ",";
	addr_b += "addr_b" + std::to_string(i) + ",";
		 
	}
	
	// Control Module Instantiation
    os << "control_" << modName << " ctrl(clk, reset, s_valid, m_ready, addr_x, wr_en_x, "<<addr_m;
	os << "enable_src, m_valid, s_ready, enable_accum, "<<addr_b;
	os << "output_counter, dataOutCounts);"<< endl;
		
	// Data Path Instantiation
    os << "datapath_" << modName << " dpth(clk, reset, data_in, addr_x, wr_en_x, "<<addr_m;
	os << "enable_src, data_out, enable_accum, "<<addr_b;
	os << "output_counter,dataOutCounts,m_valid,m_ready);"<< endl;
	
    os << "endmodule" << endl << endl;
	
   //----------------------Control Module ----------------------------------------   
  
   	// Control Module Definition
   os << "module control_" << modName << "(clk, reset, s_valid, m_ready, addr_x, wr_en_x, "<<addr_m;
   os << "enable_src, m_valid, s_ready, accum_en, "<<addr_b;
   os << "output_counter, dataOutCounts);"<< endl;
	
   os <<"input clk, reset, s_valid, m_ready;"<< endl;

   for(i=1;i<=P;i++){
		 os << "output logic ["<<ceil(log2(M*N))-1<<":0] addr_m"<<i<<"; "<< endl;
	}
   
   os << "output logic ["<<ceil(log2(N))-1<<":0] addr_x;"<< endl;
	  
   for(i=1;i<=P;i++){
		 os << "output logic ["<<ceil(log2(M))-1<<":0] addr_b"<<i<<"; "<< endl;
	}  
  
   os <<"output logic wr_en_x,m_valid, s_ready, accum_en, enable_src;"<< endl;
   os <<"logic accum_src, computing;"<< endl;
   os <<"logic ["<<ceil(log2(N))-1<<":0] input_counter;"<< endl;
   os <<"output logic ["<<ceil(log2(N+1))<<":0] output_counter;"<< endl;
   os << "output logic ["<<ceil(log2(P))<<":0] dataOutCounts;"<< endl;
   
   os <<"parameter M="<<M<<";"<< endl;
   os <<"parameter N="<<N<<";"<< endl << endl;

   os <<"always_comb begin"<< endl;
   os <<"   if ((s_valid == 1) && (s_ready == 1) && (input_counter < N))"<< endl;
   os <<" 	wr_en_x = 1;"<< endl;
   os <<"   else"<< endl;
   os <<" 	wr_en_x = 0;"<< endl;
   os <<"end"<< endl;
   
   os <<"always_ff @(posedge clk) begin"<< endl;
   os <<"   if (reset == 1) begin"<< endl;
   os <<"   	 s_ready <= 1;"<< endl;
   os <<"   	 addr_x <= 0;"<< endl;
   os <<"accum_src <= 1;"<< endl;  
   os <<"computing <= 0;"<< endl;
   os <<"output_counter <= 0;"<< endl;
   os <<"input_counter <= 0;"<< endl;
   os <<"accum_en <= 1;"<< endl;
   os <<"m_valid <= 0;"<< endl;
   os <<"dataOutCounts <= 0;"<< endl;
   
   	for (i=0;i<P;i++){
	os << "addr_m"<<i+1<<"<="<<i<<"*N;"<<endl;
	os << "addr_b"<<i+1<<"<="<<i<<";"<<endl;
	}
 
	os <<"end"<<endl;
   
    os<<"	else if (computing == 0) begin"<< endl;
									
	os<<"				if (s_valid == 1) begin"<< endl;
										
	os<<"					if (input_counter < N) begin"<< endl;
	os<<"					   input_counter <= input_counter + 1;"<< endl;
	os<<"					   if (addr_x == N-1)"<< endl;
	os<<"						 addr_x <= 0;			 "<< endl;
	os<<"					   else"<< endl;
	os<<"						addr_x <= addr_x + 1;"<< endl;
										
	os<<"					if(input_counter == N-1) begin"<< endl;
	os<<"					   input_counter <= 0;"<< endl;
	os<<"					   s_ready <= 0;"<< endl;
	os<<"				      	   computing <= 1;"<< endl;
	os<<"					end	"<< endl;
										
										
	os<<"					end "<< endl;
	os<<"				end // if (s_valid == 1)"<< endl;
	os<<"			end // if (computing == 0)"<< endl;
	
	os<<"else if (computing == 1) begin"<< endl;

	os<<"	 if ((m_valid == 1) && (m_ready == 1) && dataOutCounts == "<<P-1<<")"<< endl;                    
	os<<"	   output_counter <= 0;"<< endl;  
	os<<"	 else if (output_counter != N+1)"<< endl;
	os<<"	  output_counter <= output_counter + 1;"<< endl;
	 
	os<<"	 if ((m_valid == 1) && (m_ready == 1))"<< endl;
	os<<"	   accum_src <= 1;"<< endl;           
	os<<"	 else if (output_counter == 0)"<< endl; 
	os<<"	   accum_src <= 0;"<< endl;

	os<<"	if ( accum_src == 1)"<< endl;
	os<<"	   enable_src <= 1;"<< endl;           
	os<<"	 else "<< endl;
	os<<"	   enable_src <= 0;"<< endl;
	
	os<<"if(output_counter != N-1 && output_counter != N+1 && accum_en==1) begin"<< endl;
	
// Control addr_m	
	for (i=0;i<P;i++){
		os<<"if (addr_m"<<i+1<<"=="<<i+1<<"*N-1)"<< endl;	
		if (F==1)
		os <<"addr_m"<<i+1<<"<="<<i<<"*N;"<< endl;
		else
		os <<"addr_m"<<i+1<<"<="<<i+P<<"*N;"<< endl;
		
		for(j=1;j<F;j++){
			
			if(j==F-1){
				os <<"else if (addr_m"<<i+1<<"=="<<M-P+i+1<<"*N-1)"<< endl;
				os <<"addr_m"<<i+1<<"<="<<i<<"*N;"<< endl;
			}
			else{						
			os <<"else if (addr_m"<<i+1<<"=="<<i+1+(j*P)<<"*N-1)"<< endl;
			os <<"addr_m"<<i+1<<"<="<< i+(j*P)+P<<"*N;"<<endl;	
			}		
		}	
			
		os <<"else"<< endl;
		os <<"addr_m"<<i+1<<" <= addr_m"<<i+1<<" +1 ;"<< endl; 
		
	}
	
// Control addr_x

        os <<"    if (addr_x == N-1)"<< endl;
        os <<"      addr_x <= 0;"<< endl;
        os <<"    else"<< endl;
        os <<"      addr_x <= addr_x + 1;"<< endl;
        os <<" 	  end	"<< endl;
	
// Control addr_b
	os<<"if(output_counter == N) begin"<< endl;
	
	for (i=0;i<P;i++){
		os <<"if (addr_b"<<i+1<<"=="<<i<<")"<< endl;	
		if (F==1)
		os <<"addr_b"<<i+1<<"<="<<i<<";"<<endl;
		else
		os <<"addr_b"<<i+1<<"<="<<i+P<< ";"<<endl;

	for(j=1;j<F;j++){
			
			if(j==F-1){
				os <<"else if (addr_b"<<i+1<<"=="<<M-P+i<<")"<< endl;
				os <<"addr_b"<<i+1<<"<="<<i<< ";"<<endl;
			}
		
			else{						
			os <<"else if (addr_b"<<i+1<<"=="<<i+(j*P)<<")"<< endl;
			os <<"addr_b"<<i+1<<"<="<< i+(j*P)+P<<";"<<endl;	
			}
	}
}

		os <<" 	  end	"<< endl;
		
		os <<" if ((m_valid == 1) && (m_ready == 1) && dataOutCounts =="<< P-1<<")"<< endl;                                    
		os <<"   accum_en <= 1;"<< endl;             
		os <<" else if(output_counter == N+1)"<< endl;
		os <<"   accum_en <= 0;"<< endl;
		
		os <<" if ((m_valid == 1) && (m_ready == 1) && dataOutCounts =="<< P-1<<")"<< endl;                                        //add flag
		os <<"   m_valid <= 0;"<< endl;
		os <<" else if (output_counter == N+1) "<< endl;
		os <<"   m_valid <= 1;"<< endl;
		   
		os <<" if (dataOutCounts !="<< P-1<<" && m_ready ==1 && m_valid == 1)"<< endl;
		os <<"      dataOutCounts <= dataOutCounts +1;"<< endl;
		os <<" else if (dataOutCounts == "<< P-1<<" && (m_valid == 1) && (m_ready == 1))"<< endl;
		os <<"      dataOutCounts <= 0 ;"<< endl;
			
		os <<"if(m_valid == 1 && m_ready == 1 && dataOutCounts =="<< P-1<<") begin"<< endl;
		os <<"	if(addr_m1 == 0) begin"<< endl;
		os <<"	computing <= 0;"<< endl;
        os <<"      		s_ready <= 1;"<< endl;
		os <<"	end "<< endl; 
		os <<" end"<< endl;	 
		os <<"end // if (computing == 1)"<< endl;      
		os <<"end"<< endl;
   
		os << "endmodule" << endl << endl;

   //----------------------Memory Module -----------------------------------------

   os << "module memory_" << modName << "(clk, data_in, data_out, addr, wr_en);" << endl;
   os << "parameter WIDTH=16, SIZE=64, LOGSIZE=6;" << endl;
   os << "input [WIDTH-1:0] data_in;" << endl;
   os << "output logic [WIDTH-1:0] data_out;" << endl;
   os << "input [LOGSIZE-1:0] 	    addr;" << endl;
   os << "input 		    clk, wr_en;" << endl;
   os << "logic [SIZE-1:0][WIDTH-1:0] mem;" << endl;
   os << "always_ff @(posedge clk) begin" << endl;
   os << "   data_out <= mem[addr];" << endl;
   os << "   if (wr_en)" << endl;
   os << "	mem[addr] <= data_in;" << endl;
   os << "end" << endl;
   os << "endmodule" << endl << endl;

   
   //----------------------Datapath Module ---------------------------------------- 
   
   // Data Path Definition
   os << "module datapath_" << modName << "(clk, reset, data_in, addr_x, wr_en_x, "<<addr_m;
   os << "enable_src, data_out, enable_accum, "<<addr_b;
   os << "output_counter,dataOutCounts,m_valid,m_ready);"<< endl;
	
   os << "input clk, reset, wr_en_x, enable_src, enable_accum;"<< endl;
   os << "input signed ["<< bits-1<<":0] data_in;"<< endl; 
   
   for(i=1;i<=P;i++){
		 os << "input ["<<ceil(log2(M*N))-1<<":0] addr_m"<<i<<"; "<< endl;
	}
   
   os << "input ["<<ceil(log2(N))-1<<":0] addr_x;"<< endl;
	  
   for(i=1;i<=P;i++){
		 os << "input ["<<ceil(log2(M))-1<<":0] addr_b"<<i<<"; "<< endl;
	}
   os << "input m_valid,m_ready;"<< endl;
   os << "output logic signed ["<<bits-1<<":0] data_out; "<< endl;
   os << "logic signed ["<< bits-1<<":0]data_out_x;"<< endl;
   
    for (i=0;i<P;i++){
		os  << "logic signed ["<<bits-1<<":0] data_out"<<i+1<<",data_out_m"<<i+1<<", data_out_b"<<i+1<<",mult_out"<<i+1<<", add_out"<<i+1<<", mux_out"<<i+1<<",mult2_out"<<i+1<<"; "<< endl;
   	}
	
   os << "input ["<<ceil(log2(N+1))<<":0] output_counter;"<< endl;
   os << "input ["<<ceil(log2(P))<<":0] dataOutCounts;"<< endl;
   
   os << "parameter T="<<bits<<";"<< endl << endl;
   os << "parameter N="<<N<<";"<< endl << endl << endl;
   
   os << "memory_" << modName << " # ("<<bits<<"," << N <<","<<ceil(log2(N))<<") mem_x(clk, data_in, data_out_x, addr_x, wr_en_x); "<< endl;
   
    for (i=0;i<P;i++){
		os  << modName<<"_W_rom "<<"mem_m"<<i+1<<"(clk, addr_m"<<i+1<<", data_out_m"<<i+1<<");"<<endl;
        os  << modName<<"_B_rom "<<"mem_b"<<i+1<<"(clk, addr_b"<<i+1<<", data_out_b"<<i+1<<");"<<endl<<endl;
	}
	for (i=0;i<P;i++){
		os <<"assign mult_out"<<i+1<<" = data_out_x * data_out_m"<<i+1<<";"<<endl<<endl;
	}

	os <<"always_ff @(posedge clk) begin"<<endl;
	for (i=0;i<P;i++){		
		os <<"mult2_out"<<i+1<<"<=mult_out"<<i+1<<";"<<endl;
		}
	os <<"end"<<endl;
	
	for (i=0;i<P;i++){	
	os <<"assign add_out"<<i+1<<" = mult2_out"<<i+1<<" + data_out"<<i+1<<";"<<endl;
	os <<"assign mux_out"<<i+1<<" = (enable_src == 1) ? {8'h00, data_out_b"<<i+1<<"} : add_out"<<i+1<<";"<<endl;
    }
    
    for (i=0;i<P;i++){
    os <<"always_ff @(posedge clk) begin"<<endl;
    os <<"   if (reset == 1)"<<endl;
  	os <<"   data_out"<<i+1<<" <= 0;"<<endl;
    os <<"   else if (enable_accum == 1) begin"<<endl;
	os <<"  if(mux_out"<<i+1<<"[T-1]==1 && output_counter==N+1)"<<endl;
	os <<"	data_out"<<i+1<<" <= 0;"<<endl;
	os <<"	else"<<endl;
	os <<"	data_out"<<i+1<<" <= mux_out"<<i+1<<";"<<endl;
    os <<"   end  "<<endl;     
    os <<"end"<<endl;
}


	os <<"always_comb begin"<<endl;
	os <<"if (reset ==1)"<<endl;
	os <<"	data_out = 0;"<<endl;
	
	for (i=0;i<P;i++){
	os <<"else if ((m_valid == 1) && (m_ready == 1) && dataOutCounts=="<<i<<")"<<endl;
	os <<"	data_out = data_out"<<i+1<<";"<<endl;
}
	os <<"else"<<endl;
 	os <<"	data_out = 0;"<<endl;
    os <<"end"<<endl;
	
    os << "endmodule" << endl << endl;
   

   // At some point you will want to generate a ROM with values from the pre-stored constant values.
   // Here is code that demonstrates how to do this for the simple case where you want to put all of
   // the matrix values W in one ROM, and all of the bias values B into another ROM. (This is probably)
   // what you will need for P=1, but you will want to change this for P>1.


   // Check there are enough values in the constant file.
   if (M*N+M > constVector.size()) {
      cout << "ERROR: constVector does not contain enough data for the requested design" << endl;
      cout << "The design parameters requested require " << M*N+M << " numbers, but the provided data only have " << constVector.size() << " constants" << endl;
      assert(false);
   }

   // Generate a ROM (for W) with constants 0 through M*N-1, with "bits" number of bits
   string romModName = modName + "_W_rom";
   vector<int> wVector(&constVector[0], &constVector[M*N]);
   genROM(wVector, bits, romModName, os);

   // Generate a ROM (for B) with constants M*N through M*N+M-1 wits "bits" number of bits 
   romModName = modName + "_B_rom";
   vector<int> bVector(&constVector[M*N], &constVector[M*N+M]);

   genROM(bVector, bits, romModName, os);

}

// Part 3: Generate a hardware system with three layers interconnected.
// Layer 1: Input length: N, output length: M1
// Layer 2: Input length: M1, output length: M2
// Layer 3: Input length: M2, output length: M3
// mult_budget is the number of multipliers your overall design may use.
// Your goal is to build the fastest design that uses mult_budget or fewer multipliers
// constVector holds all the constants for your system (all three layers, in order)
void genAllLayers(int N, int M1, int M2, int M3, int mult_budget, int bits, vector<int>& constVector, string modName, ofstream &os) {

   // Here you will write code to figure out the best values to use for P1, P2, and P3, given
   // mult_budget. 
   
	bool P1_visited,P2_visited,P3_visited;
	int val1,val2,val3;
	int P1=1,P2=1,P3=1;
	int i=0;
	int P_sum;
	int L=mult_budget;

	std::map<string,int> m_map;
	m_map["P1"]=M1;
	m_map["P2"]=M2;
	m_map["P3"]=M3;
			
	val1=M1*N;
	val2=M2*M1;
	val3=M3*M2;

// Iterating Optimization Start

	P_sum=P1+P2+P3;
	
	while(P_sum<L && done<3){
		
		std::map<int,string> p_map;
		p_map[val1]="P1";
		p_map[val2]="P2";
		p_map[val3]="P3";	
		
		vector <int> vals;
		vals.push_back(val1);
		vals.push_back(val2);
		vals.push_back(val3);		
				
		sort(vals.begin(), vals.end(), greater<int>());
		
		P1_visited=false;
		P2_visited=false;
		P3_visited=false;

	for(i=0;i<vals.size();i++){
		if(p_map[vals[i]]=="P1" && P1_visited==false){
			P1=optimize_P(m_map[p_map[vals[i]]],P1,L,P_sum);
			P_sum=P1+P2+P3;
			val1=val1/P1;
			P1_visited=true;
		}
		else if (p_map[vals[i]]=="P2" && P2_visited==false){
			P2=optimize_P(m_map[p_map[vals[i]]],P2,L,P_sum);
			P_sum=P1+P2+P3;
			val2=val2/P2;
			P2_visited=true;
		}
		else if (p_map[vals[i]]=="P3" && P3_visited==false){
			P3=optimize_P(m_map[p_map[vals[i]]],P3,L,P_sum);
			P_sum=P1+P2+P3;
			val3=val3/P3;
			P3_visited=true;
		}		
	}
			
}
  
   // output top-level module
   // set your top-level name to "network_top"
   os << "module " << modName << "(clk, reset, s_valid, m_ready, data_in, m_valid, s_ready, data_out);" << endl;
   os << "input clk, reset, s_valid, m_ready; "<< endl;
   os << "input signed ["<<bits-1<<":0] data_in; "<< endl;
   os << "output signed ["<<bits-1<<":0] data_out; "<< endl;
   os << "output m_valid, s_ready;"<< endl;
   os << "logic m_valid_layer1,m_valid_layer2,s_ready_layer2,s_ready_layer3;"<< endl;
   os << "logic signed [15:0] data_out_layer1,data_out_layer2;"<< endl;

   string subModName = "layer1_" + to_string(M1) + "_" + to_string(N) + "_" + to_string(P1) + "_" + to_string(bits);
   os <<subModName<<" layer1(clk, reset, s_valid, s_ready_layer2, data_in, m_valid_layer1, s_ready, data_out_layer1);"<< endl;
   
   subModName = "layer2_" + to_string(M2) + "_" + to_string(M1) + "_" + to_string(P2) + "_" + to_string(bits);
   os <<subModName<<" layer2(clk, reset, m_valid_layer1, s_ready_layer3, data_out_layer1, m_valid_layer2, s_ready_layer2, data_out_layer2);"<< endl;
	
   subModName = "layer3_" + to_string(M3) + "_" + to_string(M2) + "_" + to_string(P3) + "_" + to_string(bits);
   os <<subModName<<" layer3(clk, reset, m_valid_layer2, m_ready, data_out_layer2, m_valid, s_ready_layer3, data_out); "<< endl; 
   
   os << "endmodule" << endl;
   
   // -------------------------------------------------------------------------
   // Split up constVector for the three layers
   // layer 1's W matrix is M1 x N and its B vector has size M1
   int start = 0;
   int stop = M1*N+M1;
   vector<int> constVector1(&constVector[start], &constVector[stop]);

   // layer 2's W matrix is M2 x M1 and its B vector has size M2
   start = stop;
   stop = start+M2*M1+M2;
   vector<int> constVector2(&constVector[start], &constVector[stop]);

   // layer 3's W matrix is M3 x M2 and its B vector has size M3
   start = stop;
   stop = start+M3*M2+M3;
   vector<int> constVector3(&constVector[start], &constVector[stop]);

   if (stop > constVector.size()) {
      cout << "ERROR: constVector does not contain enough data for the requested design" << endl;
      cout << "The design parameters requested require " << stop << " numbers, but the provided data only have " << constVector.size() << " constants" << endl;
      assert(false);
   }
   // --------------------------------------------------------------------------


   // generate the three layer modules
   subModName = "layer1_" + to_string(M1) + "_" + to_string(N) + "_" + to_string(P1) + "_" + to_string(bits);
   genLayer(M1, N, P1, bits, constVector1, subModName, os);

   subModName = "layer2_" + to_string(M2) + "_" + to_string(M1) + "_" + to_string(P2) + "_" + to_string(bits);
   genLayer(M2, M1, P2, bits, constVector2, subModName, os);

   subModName = "layer3_" + to_string(M3) + "_" + to_string(M2) + "_" + to_string(P3) + "_" + to_string(bits);
   genLayer(M3, M2, P3, bits, constVector3, subModName, os);

   // You will need to add code in the module at the top of this function to stitch together insantiations of these three modules

}

int optimize_P(int M, int P,int L,int P_sum){
	int temp=P;
	while(P_sum<L){
		
		P++;
		P_sum++;
		
		if(M%P==0){

			return P;

		}

	}
	done++;
	return temp;
}

void printUsage() {
  cout << "Usage: ./gen MODE ARGS" << endl << endl;

  cout << "   Mode 1: Produce one neural network layer and testbench (Part 1 and Part 2)" << endl;
  cout << "      ./gen 1 M N P bits const_file" << endl;
  cout << "      Example: produce a neural network layer with a 4 by 5 matrix, with parallelism 1" << endl;
  cout << "               and 16 bit words, with constants stored in file const.txt" << endl;
  cout << "                   ./gen 1 4 5 1 16 const.txt" << endl << endl;

  cout << "   Mode 2: Produce a system with three interconnected layers with four testbenches (Part 3)" << endl;
  cout << "      Arguments: N, M1, M2, M3, mult_budget, bits, const_file" << endl;
  cout << "         Layer 1: M1 x N matrix" << endl;
  cout << "         Layer 2: M2 x M1 matrix" << endl;
  cout << "         Layer 3: M3 x M2 matrix" << endl;
  cout << "              e.g.: ./gen 2 4 5 6 7 15 16 const.txt" << endl << endl;
}
