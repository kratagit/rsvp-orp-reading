    .intel_syntax noprefix
    .global GetORPIndex_Asm
    .global FastStrLen

    .text

/*
    Function: GetORPIndex_Asm
    Purpose: Calculates the Optimal Recognition Point (index of red letter)
    Input:  RDI (int wordLength)
    Output: RAX (int index)
*/
GetORPIndex_Asm:
    xor rax, rax            # Result = 0

    cmp rdi, 1
    jle .L_return           # If len <= 1, return 0

    mov rax, 1
    cmp rdi, 5
    jle .L_return           # If len <= 5, return 1

    mov rax, 2
    cmp rdi, 9
    jle .L_return           # If len <= 9, return 2

    mov rax, 3
    cmp rdi, 13
    jle .L_return           # If len <= 13, return 3

    mov rax, 4              # Default for len > 13 is 4
    
.L_return:
    ret


/*
    Function: FastStrLen
    Purpose: Calculates string length (optimized loop)
    Input:  RDI (const char* str)
    Output: RAX (length)
*/
FastStrLen:
    xor rax, rax            # RAX = 0 (length counter)
    
    test rdi, rdi           # Check if ptr is NULL
    jz .L_done

.L_loop:
    mov cl, byte ptr [rdi + rax]    # Load byte
    test cl, cl             # Check for null terminator
    jz .L_done
    
    inc rax                 # Increment length
    jmp .L_loop

.L_done:
    ret
