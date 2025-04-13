# NINO
A simple, extendable, simulated CPU architecture based on the Manchester Baby - for educational purposes.

![image](https://github.com/user-attachments/assets/d1b11eae-a15b-48b8-b2c7-5d4763270421)

Nino  is a simulated CPU - loosely based on the Manchester Baby of 1948.

I have written it in C++ on the Arduino IDE. This makes it accessible to anyone who has an Arduino compatible board.


I use the Teensy 4, because it is stupidly fast! 

But you can run it on anything from an ATmega328 upwards.


It deviates from the Manchester Baby in that it allows up to 16 instructions in its Instruction Set.

 This makes it a lot more versatile.

It also uses a 24-bit instruction word.  

This allows an 8-bit field for the instruction and register select, and a 16-bit address/data field.
I have moved the address/data field to the right of the instruction word and made it little endian.  It occupies bits 15:0 of the instruction word.

If it were implemented as a bit-serial processor, this would be the natural way to serialise the data - LSB first.

The Manchester Baby was the first machine to make use of index registers, or "modifiers" as they were called in 1948.

I have made provision for 16 such 16-bit registers, labelled R0 to R15.

These registers will occupy a contiguous block of 16-words in memory. They are general purpose, so they can be used for various tasks. They are addressed by the "Register Field" which is bits 19:16 of the instruction word.

The op-code or function is controlled by bits 23:20 of the instruction word.


So the instruction field looks something like a pair of hex nibbles:
ADD A, R1
0x05 0x01
The Accumulator A  is R0,  R1-R7 are general purpose, R8 to R15 might be used for specific purposes such as return stack pointer, Program counter etc.


Whilst the cpu is coded up as a switch statement and very simple, I spent a while getting a text screen interface to work, to show the contents of the memory with after each instruction.
I have limited the memory to just 32 words, so that it will fit on a terminal screen.


I have created this github repo that contains the first draft of this simulated cpu.

It was just a quick weekend hack, and currently only set up to perform a continuous addition into the Accumulator.

It was more an exercise in programming to see how a simple cpu could be simulated in code.
The code us 225 lines approximately of which comments are 30, the array of instructions is 35, the cpu is 40, Debug 1 (commented out) is 40 and Debug 2 is 80 lines.


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

This adds a few more functions, but is less than perfect. To simplify the instructon decoding it would be better to put the memory referencing instructions in the first 8, and those that do not reference memory in the upper 8.


