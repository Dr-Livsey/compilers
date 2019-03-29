.scope0
    str a
    lea eax, a
    movi ebx, 1
    mov [eax], ebx
while0:
    movi eax, 1
    movi ebx, 4
    lsh eax, ebx
    ld ebx, a
    cmp ebx, eax
    jl jl_true0
    movi ebx, 0
    jmp jl_false0
jl_true0:
    movi ebx, 1
jl_false0:
    test ebx, ebx
    jz while_0_exit
.scope1
    ld eax, a
    movi ebx, 2
    mod eax, ebx
    movi ebx, 0
    cmp eax, ebx
    je je_true0
    movi eax, 0
    jmp je_false0
je_true0:
    movi eax, 1
je_false0:
    test eax, eax
    jz if_body_exit0
.scope2
    ld eax, a
    pnt eax
.scope2_end
    jmp if_end0
if_body_exit0:
    ld eax, a
    movi ebx, 2
    mod eax, ebx
    test eax, eax
    jz if_body_exit1
.scope2
    ld eax, a
    movi ebx, 10
    mul eax, ebx
    pnt eax
.scope2_end
    jmp if_end0
if_body_exit1:
if_end0:
    lea eax, a
    inc [eax]
.scope1_end
    jmp while0
while_0_exit:
    str i
    lea eax, i
    movi ebx, 0
    mov [eax], ebx
while1:
    ld eax, i
    movi ebx, 10
    cmp eax, ebx
    jl jl_true1
    movi eax, 0
    jmp jl_false1
jl_true1:
    movi eax, 1
jl_false1:
    test eax, eax
    jz while_1_exit
.scope1
    ld eax, i
    pnt eax
    lea eax, i
    inc [eax]
.scope1_end
    jmp while1
while_1_exit:
.scope0_end
