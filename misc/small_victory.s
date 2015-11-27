.section .rodata
string0: .string "There is no preprocessor yet."
string1: .string "There are many things the parser doesn't support yet."
string2: .string "The code generator is ultra simple at this point."
string3: .string "But this program represents a major milestone."
string4: .string "So without further ado...\n"
string5: .string "Hello, World!"

.text  #beginning of the code

.global main

main:
	# **** FUNCTION PREAMBLE ****
	pushq %rbp          #set up stack frame
	movq %rsp, %rbp     #set up stack frame

	# **** FUNCTION BODY ****

	movl $string0, %edi
	call puts
	movl $string1, %edi
	call puts
	movl $string2, %edi
	call puts
	movl $string3, %edi
	call puts
	movl $string4, %edi
	call puts
	movl $string5, %edi
	call puts
	movl $0, %eax

	# **** FUNCTION POSTAMBLE ****
	leave   #restore stack frame
	ret     #return from function call
