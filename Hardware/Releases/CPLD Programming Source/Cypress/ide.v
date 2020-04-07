// Darwin Jukebox Control PLD, Cypress CY37032VP44-100AC
// PIO Mode 4 Timings with EP7312-CV-90 CPU
//
// 8/16/2002 Modified to correct timing violations, and
// increased performance to PIO Mode 4 per ATA spec. -BPW
// 8/23/2002 Modified to add a slowed CS and OE and WE for 
// the ethernet controller for rev 100-0018-03 and beyond.
// -BPW 
module ATA(reset, cs5, moe, mwe, clk, intrq, 
           exprdy, cs0, cs1, eint, dior, diow, rw, oe,
		   a0, a1, a2, a3, da0, da1, da2, iordy,
		   cs2, ethernetsel, mweslow, moeslow);
		   
           // Input declarations
		   input reset;
		   input cs5;
		   input moe;
		   input mwe;
		   input a3, a2, a1, a0;
		   input clk;
		   input intrq;
		   input iordy;
		   input cs2;

		   // Output declarations
		   output rw;
		   output oe;
		   output exprdy;
		   output cs0;
		   output cs1;
		   output eint;
		   output da2, da1, da0;
		   output dior;
		   output diow;
		   output ethernetsel;
		   output mweslow;
		   output moeslow;

           // Internal Variables
		   reg tmp_exprdy;
		   reg tmp_cs0;
		   reg tmp_cs1;
		   reg tmp_dior;
		   reg tmp_diow;
		   reg tmp_rw;
		   reg [5:0] count;
		   reg [2:0] state;
		   reg tmp_cs2;
		   reg tmp_moe;
		   reg tmp_mwe;

		   parameter[2:0]
		            IDLE = 0,
		            CS_ASSERTED = 1,
		            READ = 2,
					NORMAL_READ = 3,
					IORDY_READ = 4,
					WRITE = 5,
					NORMAL_WRITE = 6,
					IORDY_WRITE = 7;

           // Update the state of all the external ouput pins.
		   assign da2 = a2;
		   assign da1 = a1;
		   assign da0 = a0;
		   assign rw = tmp_rw;
		   assign oe = tmp_cs0 & tmp_cs1;
		   assign eint = !intrq;
		   assign exprdy = tmp_exprdy;
		   assign cs0 = tmp_cs0;
		   assign cs1 = tmp_cs1;
		   assign dior = tmp_dior;
		   assign diow = tmp_diow;
		   assign ethernetsel = tmp_cs2;
		   assign mweslow = tmp_mwe | mwe;
		   assign moeslow = tmp_moe | moe;
			
		   always @(posedge clk or negedge reset)
		   begin

		    // Handle a reset by initializing the state machine.
		    if(reset == 1'b 0)
	         begin
			  tmp_exprdy <= 1'b 1;
		      tmp_cs0 <= 1'b 1;
		      tmp_cs1 <= 1'b 1;
			  tmp_cs2 <= 1'b 1;
		      tmp_dior <= 1'b 1;
		      tmp_diow <= 1'b 1;
			  tmp_rw <= 1'b 1;
			  tmp_moe <= 1'b 1;
			  tmp_mwe <= 1'b 1;
		      count <= 6'b 000000;
			  state <= IDLE;
			 end
            else 
		     begin
			  // Delay Ethernet chip select by one clock to guarantee
			  // proper hold times during Ethernet reads and writes.
			  // Do not deassert tmp_cs2 until MOE and MWE deassert also. 
			  tmp_cs2 <= cs2 & moeslow & mweslow;
			  tmp_moe <= moe | tmp_cs2;
			  tmp_mwe <= mwe | tmp_cs2;

	        // Implementation of the state machine.
		    case(state) // warp parallel_case
		     IDLE: begin
			 
			 // Check for the assertion of CS5. Assert the appropriate
			 // ATA chip select, and move to the next state. 
		     if(cs5 == 1'b 0) 
			  begin
		       if(a3 == 1'b 0)
				begin
			     tmp_cs0 <= 1'b 0;
				 tmp_exprdy <= 1'b 0;
				 state <= CS_ASSERTED;
			    end
			   else 
				begin
			     tmp_cs1 <= 1'b 0;
				 tmp_exprdy <= 1'b 0;
				 state <= CS_ASSERTED;
			    end
              end
			 end
             
			 // Wait for the assertion of /MOE or /MWE to determine if a
			 // read or write access is to take place.
			 CS_ASSERTED: begin
             if(moe == 1'b 0)
			  begin
			   state <= READ;
			   tmp_rw <= 1'b 0;
			  end
			 if(mwe == 1'b 0)
 			  begin
			   state <= WRITE;
			   tmp_rw <= 1'b 1;
			  end
			 end
			 
			 // If a read access is taking place, assert /DIOR and wait to 
			 // determine if IORDY will be asserted for the cycle.
			 READ: begin
			 count <= count + 1;

			 // Activate dior 2 clocks after CS to comply with the 
			 // chip select to /DIOR timing spec.
			 if(count == 6'b 000000) 
			  begin
 			   tmp_dior <= 1'b 0;
			  end

			 // Check for the negation of IORDY three clocks after the
			 // assertion of /DIOR per the ATA spec (35 ns min). 
             if(count == 6'b 000011)
			  begin
			  
 			   // If IORDY is deasserted, perform an IORDY controlled read
			   // access, otherwise perform a normal read.
			   if(iordy == 1'b 0)
			     state <= IORDY_READ;
			   else
			    begin
                 tmp_exprdy <= 1'b 1;
			     state <= NORMAL_READ;
				end
              end
             end

			 // Performs a normal ATA read access.
			 NORMAL_READ: begin
			  count <= count + 1;
			  if(count == 6'b 000101)
			   begin
			    tmp_dior <= 1'b 1;
			   end
			  if(count == 6'b 000110) 
               begin
			    tmp_cs0 <= 1'b 1;
			    tmp_cs1 <= 1'b 1;
				tmp_rw <= 1'b 1;
			    count <= 6'b 000000;
			    state <= IDLE; 
			   end
 			 end

			 // Perform an IORDY read access per the ATA timing spec.
			 IORDY_READ: begin
			  count <= count + 1; 
              
			  // Wait for the assertion of IORDY or an IORDY timeout.
			  if((iordy == 1'b 1) || (tmp_exprdy == 1'b 1) || (count >= 6'b 111001))
			   begin
			    if(tmp_exprdy == 1'b 0)
				 begin
				  tmp_exprdy <= 1'b 1;
				  count <= 6'b 000000;
			     end

			    // Deassert /DIOR a little late to allow the EP72xx 
				// to read data.
                else
				 begin
			      if(count == 6'b 000001)
			       begin
			        tmp_dior <= 1'b 1;
			       end
				  if(count == 6'b 000010)
				   begin
				    tmp_cs0 <= 1'b 1;
			        tmp_cs1 <= 1'b 1;
					tmp_rw <= 1'b 1;
			        count <= 6'b 000000;
			        state <= IDLE; 
                   end
				end
             end
			end

			 // If a write access is taking place, assert /DIOW and wait to 
			 // determine if IORDY will be asserted for the cycle.
			 WRITE: begin
			 count <= count + 1;

			 // Activate /DIOW on a count of two to comply with the 
			 // chip select to /DIOW timing spec.
			 if(count == 6'b 000000) 
			  begin
 			   tmp_diow <= 1'b 0;
			  end

			 // Check for the negation of IORDY three clocks after the
			 // assertion of /DIOW per the ATA spec (35 ns min). 
             if(count == 6'b 000011)
			  begin
			  
 			   // If IORDY is deasserted, perform an IORDY controlled write
			   // access, otherwise perform a normal write.
			   if(iordy == 1'b 0)
			     state <= IORDY_WRITE;
			   else
			    begin
			     tmp_exprdy <= 1'b 1;
				 state <= NORMAL_WRITE;
				end
              end
             end

             // Perform a normal write access.
             NORMAL_WRITE: begin
              count <= count + 1;
			  if(count == 6'b 000100) 
			   begin
			    tmp_diow <= 1'b1;
			   end
			  if(count == 6'b 000101) 
               begin
			    tmp_cs0 <= 1'b 1;
			    tmp_cs1 <= 1'b 1;
 		  		count <= 6'b 000000;
			    state <= IDLE; 
			   end
 			 end

			 // Perform an IORDY controlled write access per the ATA timing spec.
			 IORDY_WRITE: begin
			  count <= count + 1;

			  // Wait for the assertion of IORDY or an IORDY timeout to terminate
 			  // the cycle.
			  if((iordy == 1'b 1) || (tmp_diow == 1'b 1)||(count >= 6'b 111001))
			   begin
                
  			    // Terminate the IORDY controlled write access.
				if(tmp_diow == 1'b 0)
				 begin
				  // Deassert /DIOW at this time to meet ATA write data 
				  // hold spec.
				  tmp_diow <= 1'b 1;
				  tmp_exprdy <= 1'b 1;
     			  count <= 6'b 000000;
				 end
				
			    // Complete the write access.
			    else
			     begin
				  if(count == 6'b 000001)
			       begin
				    tmp_cs0 <= 1'b 1;
			        tmp_cs1 <= 1'b 1;
	    		    count <= 6'b 000000;
			        state <= IDLE; 
                   end
			     end
                end
			  end
		   endcase
		  end
		 end
	         		  
endmodule


