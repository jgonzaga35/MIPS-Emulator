// COMP1521 2020 T2 Assignment 2
// Written by Justin Gonzaga July 2020
// "smips.c" 
// An emulator for a small simple subset of the MIPS

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include "smips.h"
#include <stdbool.h>

int main (int argc, char *argv[]) 
{
    int registers[NUM_OF_REGISTERS] = {0};    // Array of registers.
    int instructions[MAX_INSTRUCTIONS] = {0}; // Array of instructions in decimal form.               
    FILE *input_stream;                       // The currently open file.                         
    int offset;                               // Number of instructions to go back
    char string[MAX_INSTRUCTIONS];            // String containing output of a single instruction.
    char output[MAX_INSTRUCTIONS] = {0};      // String containing program output.  
    uint32_t components[4] = {0};             // Array holding the components of a MIPS instruction.
    int sign;                                 // The sign of the immediate value.
    int bit_pattern;                          // The bit pattern corresponding to a MIPS instruction.
    int count = 0;                            // Number of instructions.
    char *fileName = argv[1];                 // The name of the file.
 
    // Open the file for reading.
    input_stream = fopen(fileName, "rb");
    if (input_stream == NULL) {
        fprintf(stderr, "No such file or directory: '%s'\n", argv[1]);
        exit(1);
    }

    // Scan in hexadecimal instructions into instructions array.
    uint32_t number;
    while (1) {
        int scannedNum = fscanf(input_stream, "%x", &number);
        if (scannedNum == EOF) break;

        // Error handling: invalid input (no valid number scanned)
        if (scannedNum == 0) {
            fprintf(stderr, "%s:%d: invalid instruction code: %08X\n", fileName, count + 1, 0);
            exit(1);
        }

        // Error handling: invalid input (number scanned but does not correspond to instruction code)
        if (!isValid(number)) {
            fprintf(stderr, "%s:%d: invalid instruction code: %08X\n", fileName, count + 1, number);
            exit(1);
        }

        instructions[count] = number;
        count++;
    }

    // Print hexadecimal numbers as MIPS instructions.
    printf("Program\n");    
    for (int i = 0; i < count; i++) {
        printf(" %2d: ", i);

        if (instructions[i] == SYSCALL) {
            printf("syscall\n");
            continue;
        }

        // Get the bit pattern for the last 6 bits.
        bit_pattern = instructions[i] >> 26;
 
        // Compute s, t, d register and immediate value i.
        components[S] = (instructions[i] >> 21) & 0x1F;
        components[T] = (instructions[i] >> 16) & 0x1F;
        components[I] = instructions[i] & 0xFFFF;
        components[D] = (instructions[i] >> 11) & 0x1F;

        // Adjust immediate value if negative.
        sign = components[I] >> 15;
        if (sign == NEGATIVE) {
            components[I] = (~components[I] & 0xFF) + 1;
            components[I] *= -1;
        }

        // Print the MIPS instruction corresponding to the bit pattern.
        switch (bit_pattern) {
            case 0: 
                bit_pattern = instructions[i] & 0x7FF;
                if (bit_pattern == ADD) printf("add  ");  
                else if (bit_pattern == SUB) printf("sub  ");
                else if (bit_pattern == AND) printf("and  ");  
                else if (bit_pattern == OR) printf("or   ");
                else if (bit_pattern == SLT) printf("slt  ");
                printf("$%d, $%d, $%d\n", components[D], components[S], components[T]);
                break;
            case MUL: 
                printf("mul  $%d, $%d, $%d\n", components[D], components[S], components[T]);
                break;
            case BEQ: 
                printf("beq  $%d, $%d, %d\n", components[S], components[T], components[I]);
                break;
            case BNE: 
                printf("bne  $%d, $%d, %d\n", components[S], components[T], components[I]);
                break;
            case ADDI: 
                printf("addi $%d, $%d, %d\n", components[T], components[S], components[I]);
                break;
            case SLTI: 
                printf("slti $%d, $%d, %d\n", components[T], components[S], components[I]);
                break;
            case ANDI: 
                printf("andi $%d, $%d, %d\n", components[T], components[S], components[I]);
                break;
            case ORI: 
                printf("ori  $%d, $%d, %d\n", components[T], components[S], components[I]);
                break;
            case LUI: 
                printf("lui  $%d, %d\n", components[T], components[I]);
                break;
        }
    }

    // Execute program and print output.
    for (int i = 0; i < count; i++) {
        // Register $0 is always 0.
        if (registers[0] != 0) registers[0] = 0;

        // Process syscalls.
        if (instructions[i] == SYSCALL) {
            syscall_mips(output, string, registers);
            continue;
        } 
        // Process corresponding MIPS instruction.
        bit_pattern = instructions[i] >> 26;

        // Compute s, t, d register and immediate value i.
        components[S] = (instructions[i] >> 21) & 0x1F;
        components[T] = (instructions[i] >> 16) & 0x1F;
        components[I] = instructions[i] & 0xFFFF;
        components[D] = (instructions[i] >> 11) & 0x1F;

        // Adjust immediate value if negative.
        sign = components[I] >> 15;
        if (sign == NEGATIVE) {
            components[I] = (~components[I] & 0xFF) + 1;
            components[I] *= -1;
        }

        switch (bit_pattern) {
            case 0: bit_pattern = instructions[i] & 0x7FF;
                    if (bit_pattern == ADD) add(components, registers);
                    else if (bit_pattern == SUB) sub(components, registers);
                    else if (bit_pattern == AND) and_mips(components, registers);
                    else if (bit_pattern == OR) or_mips(components, registers);
                    else if (bit_pattern == SLT) slt(components, registers);
                    break;
            case MUL: mul(components, registers);
                    break;
            case BEQ: offset = beq(components, registers);
                    if (offset < 0) i += (offset - 1);
                    if (i < 0) print_output(output, registers);
                    break;
            case BNE: offset = bne(components, registers);
                    if (offset < 0) i += (offset - 1);
                    if (i < 0) print_output(output, registers);
                    break;
            case ADDI: addi(components, registers);
                    break;
            case SLTI: slti(components, registers);
                    break;
            case ANDI: andi(components, registers);
                    break;
            case ORI: ori(components, registers); 
                    break;
            case LUI: lui(components, registers);
                    break;
        }
    }
    fclose(input_stream);

    print_output(output, registers);
    return 0;
}

bool isValid (uint32_t number) {

    uint32_t d_component = (number >> 11) & 0x1F;
    int bit_pattern = 0;

    // Does the bit pattern correspon to a MIPS instruction?
    // Are the values of the registers and immediate values valid?
    if ((number >> 26) == 0) {
        bit_pattern = number & 0x7FF;
        switch (bit_pattern) {
            case ADD: break;
            case SUB: break;
            case AND: break;
            case OR: break;
            case SLT: break;
            case SYSCALL: break;
            default: return false;
        }
        if (d_component < 0 || d_component > 31) return false;
    } else {
        bit_pattern = number >> 26;
        switch(bit_pattern) {
            case MUL: break;
            case BEQ: break;
            case BNE: break;
            case ADDI: break;
            case SLTI: break;
            case ANDI: break;
            case ORI: break;
            case LUI: break;
            default: return false;
        }
    }

    return true;
}

void syscall_mips(char output[], char string[], int registers[]) {
    if (registers[2] == PRINT) {
        sprintf(string, "%d", registers[4]);
        strcat(output, string);
    } 
    else if (registers[2] == PRINT_CHAR) {
        sprintf(string, "%c", registers[4]);
        strcat(output, string);
    }
    else {
        if (registers[2] != EXIT) {
            sprintf(string, "Unknown system call: %d\n", registers[2]);
            strcat(output, string);
        }
        print_output(output, registers);
        exit(0);
    }
}

void print_output(char output[], int registers[]) {
    printf("Output\n");
    printf("%s", output);
    printf("Registers After Execution\n");
    for (int j = 0; j < NUM_OF_REGISTERS; j++) {
        if (registers[j] != 0) printf("$%-2d = %d \n", j, registers[j]);
    }
    exit(0);
}

void add(uint32_t components[], int registers[]) {
    registers[components[D]] = registers[components[S]] + registers[components[T]];
    return;
}

void sub(uint32_t components[], int registers[]) {
    registers[components[D]] = registers[components[S]] - registers[components[T]];
    return;
}

void and_mips(uint32_t components[], int registers[]) {
    registers[components[D]] = registers[components[S]] & registers[components[T]];
    return;
}

void or_mips(uint32_t components[], int registers[]) {
    registers[components[D]] = registers[components[S]] | registers[components[T]];
    return;
}

void slt(uint32_t components[], int registers[]) {
    if (registers[components[S]] < registers[components[T]]) {
        registers[components[D]] = 1;
    } else {
        registers[components[D]] = 0;
    }
    return;
}

void mul(uint32_t components[], int registers[]) {
    registers[components[D]] = registers[components[S]] * registers[components[T]];
    return;
}

int beq(uint32_t components[], int registers[]) {
    if (registers[components[S]] == registers[components[T]]) {
        return components[I];
    } else {
        return 0;
    }
}

int bne(uint32_t components[], int registers[]) {
    if (registers[components[S]] != registers[components[T]]) {
        return components[I];
    } else {
        return 0;
    }
}

void addi(uint32_t components[], int registers[]) {
    registers[components[T]] = registers[components[S]] + components[I];
    return;
}

void slti(uint32_t components[], int registers[]) {
    registers[components[T]] = (registers[components[S]] < components[I]);
    return;
}

void andi(uint32_t components[], int registers[]) {
    registers[components[T]] = registers[components[S]] & components[I];
    return;
}

void ori(uint32_t components[], int registers[]) {
    registers[components[T]] = registers[components[S]] | components[I];
    return;
}

void lui(uint32_t components[], int registers[]) {
    registers[components[T]] = components[I] << 16;          
    return;
}
