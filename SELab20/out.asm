.586
.model flat, stdcall
includelib libucrt.lib
includelib kernel32.lib
includelib DNP_Lib.lib

ExitProcess PROTO :DWORD
outnum PROTO :SDWORD
outchar PROTO :DWORD
outstr PROTO :DWORD
innum PROTO
inchar PROTO
in_str PROTO :DWORD
myabs PROTO :SDWORD
mystrcmp PROTO :DWORD, :DWORD
copystr PROTO :DWORD, :DWORD

.stack 4096

.const
	L0 BYTE "Hello, DNP-2026!", 0
	L1 BYTE 13, 10, 0
	L2 BYTE "1. Ввод и вывод данных:", 13, 10, 0
	L3 BYTE "Введите первое число (short): ", 0
	L4 BYTE "Введите второе число (short): ", 0
	L5 BYTE "Вы ввели: ", 0
	L6 BYTE " и ", 0
	L7 BYTE 13, 10, 13, 10, 0
	L8 BYTE "2. Типы данных и литералы:", 13, 10, 0
	L12 BYTE "Example", 0
	L13 BYTE "decVal = ", 0
	L14 BYTE 13, 10, "hexVal (0x1A) = ", 0
	L15 BYTE 13, 10, "sym = ", 0
	L16 BYTE 13, 10, "text = ", 0
	L17 BYTE 13, 10, 13, 10, 0
	L18 BYTE "3. Арифметические операции и приоритет:", 13, 10, 0
	L22 BYTE "a + b = ", 0
	L23 BYTE 13, 10, 0
	L24 BYTE "a - b = ", 0
	L25 BYTE 13, 10, 0
	L26 BYTE "a * b = ", 0
	L27 BYTE 13, 10, 0
	L28 BYTE "a / b = ", 0
	L29 BYTE 13, 10, 0
	L30 BYTE "a % b = ", 0
	L31 BYTE 13, 10, 0
	L32 BYTE "(2 + 2) * 2 = ", 0
	L33 BYTE 13, 10, 13, 10, 0
	L34 BYTE "4. Цикл с постусловием (do while):", 13, 10, 0
	L36 BYTE " ", 0
	L39 BYTE 13, 10, 13, 10, 0
	L40 BYTE "5. Условная конструкция:", 13, 10, 0
	L41 BYTE "a меньше b", 13, 10, 13, 10, 0
	L42 BYTE "a больше или равно b", 13, 10, 13, 10, 0
	L43 BYTE "6. Пользовательские функции:", 13, 10, 0
	L45 BYTE "square(4) = ", 0
	L46 BYTE 13, 10, 0
	L47 BYTE 13, 10, 0
	L48 BYTE "7. Стандартная библиотека:", 13, 10, 0
	L49 BYTE "abs(-15) = ", 0
	L50 BYTE 13, 10, 0
	L51 BYTE "apple", 0
	L52 BYTE "banana", 0
	L53 BYTE "strcmp(", 34, "apple", 34, ", ", 34, "banana", 34, ") = ", 0
	L54 BYTE 13, 10, 0

.data
	square_result SWORD 0
	main_a SWORD 0
	main_b SWORD 0
	main_decVal SWORD 0
	main_hexVal SWORD 0
	main_sym BYTE 0
	main_text BYTE 256 DUP(0)
	main_sum SWORD 0
	main_diff SWORD 0
	main_prod SWORD 0
	main_div SWORD 0
	main_mod SWORD 0
	main_expr SWORD 0
	main_i SWORD 0
	main_sq SWORD 0
	main_modVal SWORD 0
	main_s1 BYTE 256 DUP(0)
	main_s2 BYTE 256 DUP(0)
	main_cmp SWORD 0

.code

square PROC square_x:SDWORD
	push square_x
	push square_x
	pop ebx
	pop eax
	imul eax, ebx
	push eax
	pop eax
	mov square_result, ax
	movsx eax, square_result
	push eax
	pop eax
	ret
	ret
square ENDP

greeting PROC
	push offset L0
	call outstr
	push offset L1
	call outstr
	ret
greeting ENDP

main PROC
	push offset L2
	call outstr
	push offset L3
	call outstr
	call innum
	mov main_a, ax
	push offset L4
	call outstr
	call innum
	mov main_b, ax
	push offset L5
	call outstr
	movsx eax, main_a
	push eax
	call outnum
	push offset L6
	call outstr
	movsx eax, main_b
	push eax
	call outnum
	push offset L7
	call outstr
	push offset L8
	call outstr
	push -15
	pop eax
	mov main_decVal, ax
	push 26
	pop eax
	mov main_hexVal, ax
	push 65
	pop eax
	mov main_sym, al
	push offset L12
	push offset main_text
	call copystr
	push offset L13
	call outstr
	movsx eax, main_decVal
	push eax
	call outnum
	push offset L14
	call outstr
	movsx eax, main_hexVal
	push eax
	call outnum
	push offset L15
	call outstr
	movzx eax, main_sym
	push eax
	call outchar
	push offset L16
	call outstr
	push offset main_text
	call outstr
	push offset L17
	call outstr
	push offset L18
	call outstr
	movsx eax, main_a
	push eax
	movsx eax, main_b
	push eax
	pop ebx
	pop eax
	add eax, ebx
	push eax
	pop eax
	mov main_sum, ax
	movsx eax, main_a
	push eax
	movsx eax, main_b
	push eax
	pop ebx
	pop eax
	sub eax, ebx
	push eax
	pop eax
	mov main_diff, ax
	movsx eax, main_a
	push eax
	movsx eax, main_b
	push eax
	pop ebx
	pop eax
	imul eax, ebx
	push eax
	pop eax
	mov main_prod, ax
	movsx eax, main_a
	push eax
	movsx eax, main_b
	push eax
	pop ebx
	pop eax
	cdq
	idiv ebx
	push eax
	pop eax
	mov main_div, ax
	movsx eax, main_a
	push eax
	movsx eax, main_b
	push eax
	pop ebx
	pop eax
	cdq
	idiv ebx
	mov eax, edx
	push eax
	pop eax
	mov main_mod, ax
	push 2
	push 2
	pop ebx
	pop eax
	add eax, ebx
	push eax
	push 2
	pop ebx
	pop eax
	imul eax, ebx
	push eax
	pop eax
	mov main_expr, ax
	push offset L22
	call outstr
	movsx eax, main_sum
	push eax
	call outnum
	push offset L23
	call outstr
	push offset L24
	call outstr
	movsx eax, main_diff
	push eax
	call outnum
	push offset L25
	call outstr
	push offset L26
	call outstr
	movsx eax, main_prod
	push eax
	call outnum
	push offset L27
	call outstr
	push offset L28
	call outstr
	movsx eax, main_div
	push eax
	call outnum
	push offset L29
	call outstr
	push offset L30
	call outstr
	movsx eax, main_mod
	push eax
	call outnum
	push offset L31
	call outstr
	push offset L32
	call outstr
	movsx eax, main_expr
	push eax
	call outnum
	push offset L33
	call outstr
	push offset L34
	call outstr
	push 1
	pop eax
	mov main_i, ax
DO_START_1:
	movsx eax, main_i
	push eax
	call outnum
	push offset L36
	call outstr
	movsx eax, main_i
	push eax
	push 1
	pop ebx
	pop eax
	add eax, ebx
	push eax
	pop eax
	mov main_i, ax
	movsx eax, main_i
	push eax
	push 5
	pop ebx
	pop eax
	cmp eax, ebx
	jle DO_START_1
	push offset L39
	call outstr
	push offset L40
	call outstr
	movsx eax, main_a
	push eax
	movsx eax, main_b
	push eax
	pop ebx
	pop eax
	cmp eax, ebx
	jge IF_ELSE_2
	push offset L41
	call outstr
	jmp IF_END_2
IF_ELSE_2:
	push offset L42
	call outstr
IF_END_2:
	push offset L43
	call outstr
	push 4
	call square
	movsx eax, ax
	push eax
	pop eax
	mov main_sq, ax
	push offset L45
	call outstr
	movsx eax, main_sq
	push eax
	call outnum
	push offset L46
	call outstr
	call greeting
	push offset L47
	call outstr
	push offset L48
	call outstr
	movsx eax, main_decVal
	push eax
	call myabs
	movsx eax, ax
	push eax
	pop eax
	mov main_modVal, ax
	push offset L49
	call outstr
	movsx eax, main_modVal
	push eax
	call outnum
	push offset L50
	call outstr
	push offset L51
	push offset main_s1
	call copystr
	push offset L52
	push offset main_s2
	call copystr
	push offset main_s1
	push offset main_s2
	call mystrcmp
	movsx eax, ax
	push eax
	pop eax
	mov main_cmp, ax
	push offset L53
	call outstr
	movsx eax, main_cmp
	push eax
	call outnum
	push offset L54
	call outstr
	push 0
	call ExitProcess
main ENDP
end main
