
.code

GetInit PROC
    mov rax,gs:[60h]
    mov rax,[rax + 18h]
    mov rax,[rax + 30h]
    ret
GetInit ENDP

GetExportTable PROC
    ; 输入参数: rcx = baseAddress (PE 文件的基地址)
    ; 返回值:  rax = 导出表的地址 (IMAGE_EXPORT_DIRECTORY*)

    mov     rax, rcx                    ; rax = baseAddress
    movzx   rdx, word ptr [rax+3ch]           ; rdx = dosHeader.e_lfanew (DOS Header的 e_lfanew 字段)
    add     rax, rdx                    ; rax = ntheader

    lea     rax, [rax + 88h]            ;rax = optionHeader->数据目录表
    
    mov     edx, dword ptr [rax]        ;数据目录表数据目录表的RVA
    lea     rax, [rcx + rdx]

    ret

GetExportTable ENDP




 END
