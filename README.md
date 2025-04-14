# NINO
A simple, extendable, simulated CPU architecture based on the Manchester Baby - for educational purposes.

![image](https://github.com/user-attachments/assets/d1b11eae-a15b-48b8-b2c7-5d4763270421)

Nino  is a simulated CPU - loosely based on the Manchester Baby of 1948.

I have written it in C++ on the Arduino IDE. This makes it accessible to anyone who has an Arduino compatible board.

I use the Teensy 4, because it is stupidly fast! 

But you can run it on anything from an ATmega328 upwards.

It deviates from the Manchester Baby in that it allows up to 16 basic instructions in its Instruction Set. 

It also uses a 24-bit word in memory.  

This includes a 4-bit field for the instruction, a 4-bit field for register select select, and a 16-bit address/data field.

The op-code or function is controlled by bits 23:20 of the instruction word. This allows for the 16 primary instructions. However, there are a further secondary 16 operations which do not involve the memory or registers.

The address/data field is on the right of the instruction word and is little endian.  It occupies bits 15:0 of the instruction word.

If Nino were implemented as a bit-serial processor, this would be the natural way to serialise the data - LSB first.

### Registers

Building upon the experience gained from the Baby and the Manchester MK 1, where certain locations in memory were used as index registers, Nino uses this approach too. These registers were included to allow the program counter to be modified, using the data from the register. This allows relative jumps, indexed addressing and incrementing/decrementing of the PC. 

The Manchester Baby was the first machine to make use of index registers, or "modifiers" as they were called in 1948.

There provision for 16 such 16-bit registers, labelled R0 to R15, where R0 is the accumulator.

These registers occupy a contiguous block of 16-words at the base of memory. They are general purpose, so they can be used for various tasks. 

They are addressed by the "Register Field" which is bits 19:16 of the instruction word.

These registers can be manipulated from the Accumulator, and may also be used in their own right as (multiple) program counters, allowing rapid context change, and as stack pointers and temporary stores for frequently required data, without having to branch further into memory.

Experience has shown that programs consist of small modular routines, seldom exceeding 20 or 30 words, so relative branching and the ability to move locally both forwards and backwards within a limited range of addresses greatly enhances versatility. It also allows the use of relocatable code, if very few absolute jumps are specified.

### Instructions

The instruction incorporates the op-code field and the register select field as an orthogonal means of performing a wide range of 16-bit operations on the Accumulator R0 and the selected register R1 to R15. 

The orthogonal nature of the instruction set allows any op-code to be used with any register, with one exception, opcode 0, which is reserved for options that do not reference the registers.

This leads to a very easy to remember instruction set as it can be expressed as a pair of hex digits the upper or rightmost digit specifies the op-code and the lower digit selects the register number. 

Opcodes 0x do not operate on a register, they are used for conditional branches, Call and Return, Input output and other operations that do not reference the memory. This technique gives a total of 241 instructions out of a possible 256.

Registers are coded to reside in the first 16-bit words of memory, followed by a series of RST (restart) operations.

The instruction field looks like a pair of hex nibbles:

ADD A, R1
0x05 0x01

This would appear in the instruction field as $51

### Registers

The Accumulator A  is R0,  R1-R7 are general purpose registers, R8 to R15 might also be used for specific purposes such as return stack pointer, Program Counter etc.

Whilst the cpu is coded up as a switch statement and very simple, it took some time getting a text screen interface to work, to show the contents of the memory after each instruction has executed.

In this simulation memory to just 32 words, so that it will fit on a terminal screen, and can be implemented on the smaller Arduinos.

This github repo contains the first draft of this simulated cpu. It has been purposefully been kept simple so that it is relatively easy to understand for the newcomer.

The aim is that it can easily be extended to suit the application requirements.

It was just a quick weekend hack, and currently only set up to perform a continuous addition into the Accumulator.

As an exercise in programming to illustrate how a simple cpu may be simulated in about 225 lines of code, approximately 30 are comments, the instruction array is 35, the cpu switch sttements are 40, Debug 1 (commented out) is 40 and Debug 2 is 80 lines.

It is called NINO - which is Spanish for (boy) child, but also stands for Nonsense In Nonsense Out.

## Instruction Set

The Manchester Baby first ran with only 6 useful instructions, but it could only do subtraction, addition came later. In the first draft I have added addition, and the means to load the accumulator with a non-inverted version of the data in memory.

The current line up is:

0    JMP    - a 16-bit absolute jump, by reloading the PC with a target address

1    JPR    - a relative jump, by adding a "modifier" to the PC

2    LDN    - load an inverted data word into the Accumulator

3    LDP    - load a non-inverted data word into the Accumulator

4    SUB    - Subtract a data word from the Accumulator

5    ADD    - Add a data word to the Accumulator

6    SKZ    - Skip the next instruction if the Accumulator is zero.

7    STO    - Store the contents of the Accumulator into memory

This is a minimal subset to get the CPU to do anything. The following instructions are provisional and expected to change:

8    AND    - Bitwise AND the Accumulator with a memory location

9    OR     - Bitwise OR the Accumulator with a memory location

10   XOR    - Bitwise XOR the Accumulator with a memory location

11   INC    - Increment the value of the Accumulator

12   DEC    - Decrement the value of the Accumulator

13   IN     - Input data from the bus into the Acccumulator

14   OUT    - Output data to the databus

15   NOP    - No Operation

This adds a few more functions, but is less than perfect. To simplify the instructon decoding it would be better to put the memory referencing instructions in the lower 8, and those that do not reference memory in the upper 8.

However, the intention is to have 16 general purpose registers R0 to R15 held in the base of memory.

This allows a certain amount of abstraction from the raw machine level, and gives a much richer instruction set. 

R0 is the Accumulator and is always the source of one of the operands. A 4-bit field allows us to select any other register, memory, or even the Accumulator as the other operand source.

This opens up a lot more flexibility, and gives a machine that is easier to program.

Back in 2019 I proposed an architecture called Suite-16, which used a similar technique:

### Suite-16 Instructions

Register OPS-
     0n        ---       --     Non-Register Ops (see below)
     1n        SET       Rn     Constant  (Set)         Rn = @(PC)
     2n        LD        Rn     (Load)                  AC = Rn
     3n        ST        Rn     (Store)                 Rn = AC
     4n        LD        @Rn    (Load Indirect)         AC = @Rn
     5n        ST        @Rn    (Store Indirect)        @Rn = AC
     6n        POP       @Rn    Pop  AC                 AC = @Rn  Rn = Rn + 1
     7n        PUSH      @Rn    Push AC                 Rn = Rn - 1  @Rn = AC  
     8n        AND       Rn     (AND)                   AC = AC & Rn 
     9n        OR        Rn     (OR)                    AC = AC | Rn 
     An        ADD       Rn     (Add)                   AC = AC + Rn
     Bn        SUB       Rn     (Sub)                   AC = AC - Rn
     Cn        INV       Rn     (Invert)                Rn = ~Rn
     Dn        DCR       Rn     (Decrement)             Rn = Rn - 1
     En        INR       Rn     (Increment)             Rn = Rn + 1
     Fn        XOR       Rn     (XOR)                   AC = AC ^ Rn
     
Non-register OPS- always start with 0x

     00        BRA    Always Branch                 Target = @(PC)
     01        BGT    AC>0                          Target = @(PC)
     02        BLT    AC<0                          Target = @(PC)
     03        BGE    AC>=0                         Target = @(PC)
     04        BLE    AC<=0                         Target = @(PC)
     05        BNE    AC!=0                         Target = @(PC)
     06        BEQ    AC=0                          Target = @(PC)   
     07        JMP    16-bit                        Target = @(PC)
     08        CALL   16-bit                        Target = @(PC)
     09        RET    Return
     0A        ADI    Add 8-bit Immediate           Immediate = IR7:0
     0B        SBI    Subtract 8-bit Immediate      Immediate = IR7:0
     0C        OUT                                  putchar(AC), port = IR7:0
     0D        IN                                   AC = getchar(), port = IR7:0
     0E        JP@                                  BRA (R0)
     0F        OPR                                  Allows microcoded instructions and NOP AC &= AC

### Influences

The basis of the Nino cpu is the Manchester Baby of 1948 and it's successor the Manchester MK 1, but there are two other major influences:- 

The instruction set of the RCA 1802 which gave the idea of 16 general purpose registers, and Steve Wozniak's "SWEET 16" virtual machine for the 6502, which was also highly influenced by the RCA 1802 instruction set.




