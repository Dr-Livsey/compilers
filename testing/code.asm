.scope0
    movi eax, 5
    movi ebx, 1
    add eax, ebx
    movi ebx, 22
    lsh eax, ebx
    str a
    lea ebx, a
    mov [ebx], eax
while0:
    ld eax, a
    test eax, eax
    jz while_0_exit
.scope1
    str b
    lea eax, b
    movi ebx, 5
    mov [eax], ebx
    movi eax, 5
    movi ebx, 255
    or eax, ebx
    ld ebx, a
    lsh ebx, eax
    movi eax, 25
    and ebx, eax
    lea eax, a
    mov [eax], ebx
    ld eax, a
    movi ebx, 1
    add eax, ebx
    ld ebx, b
    movi ecx, 0
    cmp ebx, ecx
    jne jne_true0
    movi ebx, 0
    jmp jne_false0
jne_true0:
    movi ebx, 1
jne_false0:
    land eax, ebx
    test eax, eax
    jz if_body_exit0
.scope2
    movi eax, 1
    movi ebx, 2
    mul eax, ebx
    movi ebx, 1
    add eax, ebx
    movi ebx, 25
    xor eax, ebx
    pnt eax
.scope2_end
    jmp if_end0
if_body_exit0:
    movi eax, 1
    test eax, eax
    jz if_body_exit1
.scope2
    str c
    lea eax, c
    movi ebx, 25
    mov [eax], ebx
while1:
    ld eax, c
    movi ebx, 1000
    cmp eax, ebx
    jle jle_true0
    movi eax, 0
    jmp jle_false0
jle_true0:
    movi eax, 1
jle_false0:
    test eax, eax
    jz while_1_exit
    movi eax, 25
    movi ebx, 1
    add eax, ebx
    lea ebx, c
    mul [ebx], eax
    jmp while1
while_1_exit:
    ld eax, a
    movi ebx, 25
    cmp eax, ebx
    jne jne_true1
    movi eax, 0
    jmp jne_false1
jne_true1:
    movi eax, 1
jne_false1:
    ld ebx, a
    movi ecx, 34
    cmp ebx, ecx
    jg jg_true0
    movi ebx, 0
    jmp jg_false0
jg_true0:
    movi ebx, 1
jg_false0:
    land eax, ebx
    test eax, eax
    jz if_body_exit2
    lea eax, a
    movi ebx, 100
    mov [eax], ebx
    jmp if_end1
if_body_exit2:
.scope2_end
if_end1:
    jmp if_end0
if_body_exit1:
.scope2
    str i
    lea eax, i
    movi ebx, 0
    mov [eax], ebx
while2:
    ld eax, i
    movi ebx, 100
    cmp eax, ebx
    jl jl_true0
    movi eax, 0
    jmp jl_false0
jl_true0:
    movi eax, 1
jl_false0:
    test eax, eax
    jz while_2_exit
.scope3
    ld eax, i
    pnt eax
.scope3_end
    jmp while2
while_2_exit:
    ld eax, i
    ret eax
.scope2_end
if_end0:
.scope1_end
    jmp while0
while_0_exit:
.scope0_end
