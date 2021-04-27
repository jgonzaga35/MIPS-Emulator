// COMP1521 2020 T2 Assignment 2
// Written by Justin Gonzaga July 2020
// "smips.h" 
// A header file for "smips.c", an emulator for 
// a small subset of MIPS instructions.

#include <stdint.h>
#include <stdbool.h>

#define SYSCALL 12
#define NUM_OF_REGISTERS 32
#define MAX_INSTRUCTIONS 10000
#define S 0
#define T 1
#define I 2
#define D 3
#define NEGATIVE 1
#define ADD 0x20
#define SUB 0x22
#define AND 0x24
#define OR 0x25
#define SLT 0x2A
#define MUL 0x1C
#define BEQ 0x4
#define BNE 0x5
#define ADDI 0x8
#define SLTI 0xA
#define ANDI 0xC
#define ORI 0xD
#define LUI 0xF
#define PRINT 1
#define PRINT_CHAR 11
#define EXIT 10

// Is the number a valid hexadecimal instruction?
bool isValid(uint32_t number); 

// Execute syscall corresponding to register value.
void syscall_mips(char output[], char string[], int registers[]);

// Prints program output and value of registers (if non-zero).
void print_output(char output[], int registers[]);

// Stores the sum of $s and $t into $d.
void add(uint32_t components[], int registers[]);

// Stores the difference between $s and $t into $d.
void sub(uint32_t components[], int registers[]);

// Takes bitwise AND of $s and $t and stores value into $d.
void and_mips(uint32_t components[], int registers[]);

// Takes bitwise OR of $s and $t and stores value into $d.
void or_mips(uint32_t components[], int registers[]);

// Stores 1 into $d if $t is greater than $s, 0 otherwise.
void slt(uint32_t components[], int registers[]);

// Multiplies value of $s and $t and stores answer into $d.
void mul(uint32_t components[], int registers[]);

// Stores immediate value into $s if $s == $t and returns the immediate value.
int beq(uint32_t components[], int registers[]);

// Returns the immediate value if $s != $t, 0 otherwise.
int bne(uint32_t components[], int registers[]);

// Adds the immediate value I with $s and stores the value into $t.
void addi(uint32_t components[], int registers[]);

// Stores 1 into $t if i is greater than $s, 0 otherwise.
void slti(uint32_t components[], int registers[]);

// Stores the bitwise AND of $s and i and stores value into $t.
void andi(uint32_t components[], int registers[]);

// Takes the bitwise OR of $s and i and stores into $t.
void ori(uint32_t components[], int registers[]);

// Left shifts i by 16 bits and stores value into $t.
void lui(uint32_t components[], int registers[]);
