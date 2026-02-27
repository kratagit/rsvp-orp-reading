    .intel_syntax noprefix
    .global GetORPIndex_Asm
    .global FastStrLen
    .global CountUtf8CodepointsASM
    .global SplitWordASM

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

.L_st_loop:
    mov cl, byte ptr [rdi + rax]    # Load byte
    test cl, cl             # Check for null terminator
    jz .L_done
    
    inc rax                 # Increment length
    jmp .L_st_loop

.L_done:
    ret

/*
    Function: CountUtf8CodepointsASM
    Purpose: Counts real UTF-8 characters (codepoints) incredibly fast.
    Input:  RDI (const char* str)
    Output: RAX (count)
*/
CountUtf8CodepointsASM:
    xor rax, rax            # RAX = count = 0
    test rdi, rdi
    jz .L_utf8_done

.L_utf8_loop:
    mov cl, byte ptr [rdi]  # Load current byte
    test cl, cl             # Check null terminator
    jz .L_utf8_done

    # UTF-8 logic: count only bytes that are NOT continuation bytes (10xxxxxx)
    # Continuation bytes trigger: (byte & 0xC0) == 0x80
    
    mov ch, cl
    and ch, 0xC0            # Mask top 2 bits
    cmp ch, 0x80            # Check if it matches 10xxxxxx
    je .L_skip_inc          # If continuation byte, don't increment count

    inc rax                 # It's a start of a new char, increment count

.L_skip_inc:
    inc rdi                 # Move to next byte
    jmp .L_utf8_loop

.L_utf8_done:
    ret


/*
    Function: SplitWordASM
    Purpose: Splits word into Left, Center, Right parts based on ORP index.
    Arguments:
      RDI: const char* word
      RSI: int orpIndex
      RDX: char* leftBuf
      RCX: char* centerBuf
      R8:  char* rightBuf
*/
SplitWordASM:
    push rbx
    push r12
    push r13
    
    # RDI = current source pointer
    xor r9, r9              # r9 = currentCodepoint = 0

    # --- PHASE 1: FILL LEFT BUFFER ---
.L_fill_left:
    cmp r9, rsi             # if currentCodepoint == orpIndex
    je .L_finish_left       # stop filling left

    mov al, byte ptr [rdi]  # Load first byte of char
    test al, al
    jz .L_terminate_all     # End of string prematurely?

    # Calculate char length based on first byte
    # 0xxxxxxx -> 1 byte
    # 110xxxxx -> 2 bytes
    # 1110xxxx -> 3 bytes
    # 11110xxx -> 4 bytes
    
    mov bl, 1               # Default length = 1
    test al, 0x80
    jz .L_do_copy_left      # 0xxxxxxx (ASCII) -> len 1
    
    mov bl, 2
    mov ah, al              # Save AL
    and ah, 0xE0
    cmp ah, 0xC0
    je .L_do_copy_left      # 110xxxxx -> len 2

    # Reload AL (actually just use original AL if available, but here reloading from mem is fine)
    mov al, byte ptr [rdi]
    mov bl, 3
    mov ah, al
    and ah, 0xF0
    cmp ah, 0xE0
    je .L_do_copy_left      # 1110xxxx -> len 3

    mov bl, 4               # Assume 4 if not others (11110xxx)

.L_do_copy_left:
    # Copy BL bytes from [RDI] to [RDX]
    movzx r12, bl           # r12 becomes loop counter for bytes
.L_copy_bytes_left:
    mov al, byte ptr [rdi]
    mov byte ptr [rdx], al
    inc rdi
    inc rdx
    dec r12
    jnz .L_copy_bytes_left

    inc r9                  # currentCodepoint++
    jmp .L_fill_left

.L_finish_left:
    mov byte ptr [rdx], 0   # Terminate Left


    # --- PHASE 2: FILL CENTER BUFFER (Single Character) ---
    # We are at ORP index.
    mov al, byte ptr [rdi]
    test al, al
    jz .L_terminate_center_right  # End of string?

    # Calculate length again
    mov bl, 1
    test al, 0x80
    jz .L_do_copy_center
    
    mov bl, 2
    mov ah, al
    and ah, 0xE0
    cmp ah, 0xC0
    je .L_do_copy_center

    mov al, byte ptr [rdi]  # Reload
    mov bl, 3
    mov ah, al
    and ah, 0xF0
    cmp ah, 0xE0
    je .L_do_copy_center

    mov bl, 4

.L_do_copy_center:
    movzx r12, bl
.L_copy_bytes_center:
    mov al, byte ptr [rdi]
    mov byte ptr [rcx], al
    inc rdi
    inc rcx
    dec r12
    jnz .L_copy_bytes_center
    
    mov byte ptr [rcx], 0   # Terminate Center


    # --- PHASE 3: FILL RIGHT BUFFER (Rest of string) ---
    # Just strcpy remaining bytes
.L_fill_right_loop:
    mov al, byte ptr [rdi]
    test al, al
    jz .L_terminate_right_final
    
    mov byte ptr [r8], al
    inc rdi
    inc r8
    jmp .L_fill_right_loop

.L_terminate_right_final:
    mov byte ptr [r8], 0
    pop r13
    pop r12
    pop rbx
    ret

.L_terminate_all:
    mov byte ptr [rdx], 0
.L_terminate_center_right:
    mov byte ptr [rcx], 0
    mov byte ptr [r8], 0
    pop r13
    pop r12
    pop rbx
    ret
