/**************************************************************************************************************************************
A simple simulator, based on Manchester Baby for Arduino compatible boards

Ken Boak April 13th 2025

Nino Instructions
_________________________________________________________________________________________________
|23 |22 |21 |20 |19 |18 |17 |16 |15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |  24-bit word
-------------------------------------------------------------------------------------------------
|I3 |I2 |I1 |I0 |R3 |R2 |R1 |R0 |n15|n14|n13|n12|n11|n10|n9 |n8 |n7 |n6 |n5 |n4 |n3 |n2 |n1 |n0 |  24-bit word in memory n = (N)
-------------------------------------------------------------------------------------------------
|___|___|___|___|___|___|___|___|C15|C14|C13|C12|C11|C10|C09|C08|C07|C06|C05|C04|C03|C02|C01|C00|  16-bit Program Counter (PC)  C PC
-------------------------------------------------------------------------------------------------
|___|___|___|___|___|___|___|___|A15|A14|A13|A12|A11|A10|A09|A08|A07|A06|A05|A04|A03|A02|A01|A00|  16-bit Accumulator
-------------------------------------------------------------------------------------------------
 reg = (M[PC] & 0x0F0000)>>16 ;
 n = (M[PC] & 0x00FFFF) ;
 N = (M[s]) = A;
 op = (M[PC] & 0xf00000)>>20 ;

0	000	        C = s	 	    JMP	      Copy content of Store line to CI (Program Counter C)
1	001	        C = c+s,	 	JRP	      Add content of Store line to CI (Program Counter C)
2	010 	      A = 0 - n   LDN	      Copy content of Store line, negated, to accumulator.
3 011	        A = 0 + n	 	LDP	      Copy content of Store line, un-negated, to accumulator.
4	100	        A = A - s	  SUB	      Subtract content of Store line from Accumulator
5	101	        A = A - s	  ADD       Add content of Store line to Accumulator
6	110	        Test		    CMP	      Skip next instruction if content  of Accumulator is negative
7	111	   		  N = A       STO	      Copy content of acc. to Store line.
************************************************************************************************************************************/

#define MEMSIZE  32
int M[MEMSIZE] = {   
        (0x03 * 1048576)+(0x00 * 65536 ) + 00000,   // LDP A
		    (0x03 * 1048576)+(0x01 * 65536 ) + 00000,   // LDP A
        (0x05 * 1048576)+(0x01 * 65536 ) + 00001,   // ADD n
        (0x07 * 1048576)+(0x01 * 65536 ) + 0,       // STO n
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0
        (0x00 * 1048576)+(0x00 * 65536 ) + 00000,   // JMP 0       
        } ;
  int PC = 0 ;  int n = 0 ; int s = 0 ; int ir = 0 ; int a = 0 ; int A = 0 ; int op = 0 ; int x = 0 ; int fn = 0 ; int reg = 0; int data = 0 ;
  long millis_old = 0 ; long millis_new = 0 ; long count = 0 ;
  char hi = 160 ;
  
  void setup() 
{  
 Serial.begin(2000000); 
 millis_old = millis() ;
 delay(1000);
 Serial.print("Manchester Baby") ;
 Serial.println() ; Serial.println() ;
 }

void execute()
{
// Serial.print("   Count=") ; Serial.print(count); count++ ;

/* Instruction Decoder */
  op = (M[PC] & 0xF00000)/ 1048576 ;
  s =  (M[PC] & 0x0F0000)/65536 ;
  n =  (M[PC] & 0x00FFFF) ;
  
 
/* Opcode Execute */ 
    switch (op) {
    case 0x0:  PC = s                             ; break ;       // 16-bit JMP
    case 0x1:  PC = PC + s                        ; break ;       // JPR Relative Jump 
    case 0x2:  A = 0 - n ; PC++                   ; break ;       // LDN
    case 0x3:  A = 0 + n ; PC++                   ; break ;       // LDP
    case 0x4:  A = A - n ; PC++                   ; break ;       // SUB
    case 0x5:  A = A + n ; PC++                   ; break ;       // ADD
    case 0x6:  if(A == 0) {PC++;}                 ; break ;       // SKZ 
    case 0x7:  M[s] = (M[s] & 0xFF0000) + A; PC++ ; break ;       // STO
    case 0x8:     ; break ;  
    case 0x9:     ; break ;  
    case 0xA:     ; break ;  
    case 0xB:     ; break ;  
    case 0xC:     ; break ;  
    case 0xD:     ; break ;  
    case 0xE:     ; break ;  
    case 0xF:     ; break ; 
    }

/*   Serial.print (" PC=") ;
     Serial.print (PC, HEX); 
     Serial.print (" Line  ") ;
     Serial.print (s, HEX);

     Serial.print (" Acc=") ;
     if (A < 10000) {Serial.print("0");}
     if (A < 1000) {Serial.print("0");}
     if (A < 100) {Serial.print("0");}
     if (A < 10) {Serial.print("0");}
     if (A <= 1) {Serial.print("0");} 
     Serial.println(A);
    
      // if(op=7) M[s]= A;   
     Serial.print ("   Opr= ") ;
     if (op<16) { Serial.print("0");}
     Serial.print(op, HEX);

    Serial.print ("  n= ") ;
    if (n < 10000) {Serial.print("0");}
    if (n < 1000) {Serial.print("0");}
    if (n < 100) {Serial.print("0");}
    if (n < 10) {Serial.print("0");}
    if (n <= 1) {Serial.print("0");}
    //if (n == 0) {Serial.print("0"); }
    Serial.print(n);

    Serial.print ("  A= ") ;
    if (A < 4096) { Serial.print("0");}
    if (A < 256) { Serial.print("00");}
    if (A < 16) { Serial.print("000");}   
    Serial.print(A, HEX);
    A = (A & 0x00FFFF); 

    int mem = M[0];
    Serial.print ("  Mem= ") ;
    if (mem < 4096) { Serial.print("0");}
    if (mem < 256) { Serial.print("00");}
    if (mem < 16) { Serial.print("000");}   
    Serial.println(mem, HEX); 
  */

     for (int i = 0; i <= 31; i++)
     {ir = ((M[i] & 0xFF0000)/65536); fn = ((M[i] & 0xF00000)/1048576); reg = ((M[i] & 0x0F0000)/65536); data =(M[i]& 0x00FFFF);
     print_inst(ir,i) ;
     print_row(data,i); 
     print_mem(M[i]) ; 
     print_op(fn , reg) ;  
     }

 // 16-bit word as a bit pattern

      if(x>=32768)  {Serial.print("1"); x =  x- 32768;} 
    delay(50) ;
   }  
  void loop() { execute() ;  }

// ****************************************************************************************************************

void print_row(int x, int y) {                  // Output a 16-bit number in row of 1's and 0's
      if(x>=32768)  {Serial.print("1"); x =  x- 32768;}  else {Serial.print("0");}
      if(x>=16384)  {Serial.print("1"); x =  x- 16384;}  else {Serial.print("0");}
      if(x>=8192)   {Serial.print("1"); x =  x- 8192;}   else {Serial.print("0");}
      if(x>=4096)   {Serial.print("1  "); x =  x- 4096;} else {Serial.print("0  ");}
      if(x>=2048)   {Serial.print("1"); x =  x- 2048;}   else {Serial.print("0");}
      if(x>=1024)   {Serial.print("1"); x =  x- 1024;}   else {Serial.print("0");}
      if(x>=512)    {Serial.print("1"); x =  x- 512;}    else {Serial.print("0");}
      if(x>=256)    {Serial.print("1  "); x =  x- 256;}  else {Serial.print("0  ");}
      if(x>=128)    {Serial.print("1"); x =  x- 128;}    else {Serial.print("0");}
      if(x>=64)     {Serial.print("1"); x =  x- 64;}     else {Serial.print("0");}
      if(x>=32)     {Serial.print("1"); x =  x- 32;}     else {Serial.print("0");}
      if(x>=16)     {Serial.print("1  "); x =  x- 16;}   else {Serial.print("0  ");}
      if(x>=8)      {Serial.print("1"); x =  x- 8;}      else {Serial.print("0");}
      if(x>=4)      {Serial.print("1"); x =  x- 4;}      else {Serial.print("0");}
      if(x>=2)      {Serial.print("1"); x =  x- 2;}      else {Serial.print("0");}
      if(x>=1)      {Serial.print("1  "); x =  x- 1;}    else {Serial.print("0  ");}
      }

      void print_inst(int x, int y) {  // Output a 8-bit instruction as a bit pattern
      if (y<16){Serial.print("0");} Serial.print(y, HEX); Serial.print("   ");
      int inst = x ;

      if(x>=128)    {Serial.print("1"); x =  x- 128;}    else {Serial.print("0");}
      if(x>=64)     {Serial.print("1"); x =  x- 64;}     else {Serial.print("0");}
      if(x>=32)     {Serial.print("1"); x =  x- 32;}     else {Serial.print("0");}
      if(x>=16)     {Serial.print("1"); x =  x- 16;}     else {Serial.print("0");}
      if(x>=8)      {Serial.print("1"); x =  x- 8;}      else {Serial.print("0");}
      if(x>=4)      {Serial.print("1"); x =  x- 4;}      else {Serial.print("0");}
      if(x>=2)      {Serial.print("1"); x =  x- 2;}      else {Serial.print("0");}
      if(x>=1)      {Serial.print("1 "); x =  x- 1;}     else {Serial.print("0 ");}
      if (inst<16){Serial.print("0");} Serial.print(inst, HEX); Serial.print("   "); }

    void print_op(int x, int z){  // Print the opcode at location x and the operand address z
    switch (x) {
    case 0x0:  Serial.print(" JMP  "); if (z<16){Serial.print("0");} Serial.print(z, HEX); Serial.println("   "); break ;       // JMP 16-bit
    case 0x1:  Serial.print(" JPR  "); if (z<16){Serial.print("0");} Serial.print(z, HEX); Serial.println("   "); break ;       // JPR Relative Jump 
    case 0x2:  Serial.print(" LDN  "); if (z<16){Serial.print("0");} Serial.print(z, HEX); Serial.println("   "); break ;       // LDN
    case 0x3:  Serial.print(" LDP  "); if (z<16){Serial.print("0");} Serial.print(z, HEX); Serial.println("   "); break ;       // LDP
    case 0x4:  Serial.print(" SUB  "); if (z<16){Serial.print("0");} Serial.print(z, HEX); Serial.println("   "); break ;       // SUB
    case 0x5:  Serial.print(" ADD  "); if (z<16){Serial.print("0");} Serial.print(z, HEX); Serial.println("   "); break ;       // ADD
    case 0x6:  Serial.print(" SKZ  "); if (z<16){Serial.print("0");} Serial.print(z, HEX); Serial.println("   "); break ;       // SKZ 
    case 0x7:  Serial.print(" STO  "); if (z<16){Serial.print("0");} Serial.print(z, HEX); Serial.println("   "); break ;       // STO
    }
    }

    void print_mem(int x) {   // Print the 3-byte word stored at location x
    if (x < 1048576) { Serial.print("0");}
    if (x < 65536) { Serial.print("0");} 
    if (x < 4096) { Serial.print("0");}
    if (x < 256) { Serial.print("0");}
    if (x < 16) { Serial.print("0");} 
    Serial.print(x, HEX); Serial.print("  "); 
    }

