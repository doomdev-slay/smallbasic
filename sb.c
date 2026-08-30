#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Assembly templates (Intel syntax)


// Updated header template for modern GCC/Gentoo systems
#define ASM_START      ".intel_syntax noprefix\n" \
                       ".global main\n" \
                       ".text\n" \
                       "main:\n" \
                       "    .type main, @function\n"



#define ASM_EXIT       ".section .note.GNU-stack,\"\",@progbits\n"

int main(int argc, char *argv[]) {

    if (argv[1] == NULL) {

      printf("Usage: smallbasic <input_file>\n");
      return 1;
      
    }
  
    FILE *source_code = fopen(argv[1], "r");
    FILE *output_asm = fopen("output.s", "w");
    
    // Buffer to dynamicly store the .data section strings at the end of the file
    char data_section[4096] = ".data\n";
    int text_count = 0;

    // Add a format string for printing integers via printf
    strcat(data_section, "fmt_int:\n    .ascii \"%d\\n\\0\"\n");

    if (!source_code || !output_asm) {
        printf("Error: Could not open source or output files!\n");
        return 1;
    }

    // Write the assembly header
    fprintf(output_asm, ASM_START);

    char line[256];
    while (fgets(line, sizeof(line), source_code) != NULL) {
        line[strcspn(line, "\n")] = 0; // Remove new line character

        // --- 1. INSTRUCTION: PRINT STRING (print "text") ---
        if (strncmp(line, "print \"", 7) == 0) {
            char *end_quote = strrchr(line, '"');
            if (end_quote) {
                *end_quote = '\0';
                char *text = line + 7;
                int length = strlen(text) + 1; // +1 for the \n character

                // Add string to data section with a unique label (txt0, txt1, etc.)
                char temp_buf[256];
                sprintf(temp_buf, "txt%d:\n    .ascii \"%s\\n\"\n", text_count, text);
                strcat(data_section, temp_buf);

                // Generate assembly for the write syscall
                fprintf(output_asm, "    # --- print \"%s\" ---\n", text);
                fprintf(output_asm, "    mov rax, 1\n");                  // sys_write
                fprintf(output_asm, "    mov rdi, 1\n");                  // stdout
                fprintf(output_asm, "    lea rsi, [txt%d]\n", text_count); // text address
                fprintf(output_asm, "    mov rdx, %d\n", length);         // text length
                fprintf(output_asm, "    syscall\n");

                fprintf(output_asm, "    # --- Exit Program Gracefully ---\n");
                fprintf(output_asm, "    mov rax, 60\n");
                fprintf(output_asm, "    mov rdi, 0\n");
                fprintf(output_asm, "    syscall\n\n");


                text_count++;
            }
        }
        // --- 2. INSTRUCTION: SET VARIABLE (a = 5 or b = 5) ---
        else if (strncmp(line, "a = ", 4) == 0) {
            int value = atoi(line + 4);
            fprintf(output_asm, "    # --- variable a = %d ---\n", value);
            fprintf(output_asm, "    mov r8, %d\n", value); // variable 'a' lives in register R8
        }
        else if (strncmp(line, "b = ", 4) == 0) {
            int value = atoi(line + 4);
            fprintf(output_asm, "    # --- variable b = %d ---\n", value);
            fprintf(output_asm, "    mov r9, %d\n", value); // variable 'b' lives in register R9
        }
        // --- 3. INSTRUCTION: ADD (a += b) ---
        else if (strcmp(line, "a += b") == 0) {
            fprintf(output_asm, "    # --- a += b ---\n");
            fprintf(output_asm, "    add r8, r9\n"); // Add R9 into R8
        }
        // --- 4. INSTRUCTION: PRINT VARIABLE (print_var a) ---
        else if (strcmp(line, "print_var a") == 0) {
            fprintf(output_asm, "    # --- print_var a ---\n");
            fprintf(output_asm, "    lea rdi, [fmt_int]\n"); // 1st argument for printf: format string
            fprintf(output_asm, "    mov rsi, r8\n");        // 2nd argument for printf: value of 'a'
            fprintf(output_asm, "    mov al, 0\n");          // Required for functions with floating points (set to 0)
            fprintf(output_asm, "    call printf\n");        // Call standard C printf function
        }
    }

    // End of program and append the data section
    fprintf(output_asm, ASM_EXIT);
    fprintf(output_asm, "%s", data_section);
    
    
   
    fclose(source_code);
    fclose(output_asm);
    system("gcc -no-pie -Wl,-z,noexecstack -Wl,--entry=main -o program.elf output.s");
    printf("Assembled! Generated file: output.s\n");
    printf("Compiled! Generated file: program.elf\n");
    
    return 0;
}

