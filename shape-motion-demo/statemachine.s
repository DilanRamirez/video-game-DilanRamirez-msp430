	.file "shapemotion.c"
	.file "myShape.c"

	.extern lostScreen
	.extern welcomeScreen

	.data
x:	.word 0
	
	.text
jt:	.word case0 		;jt[0]
	.word case1
	.word case2
	.word case3
	.word default		;jt[1]
	
	.global sounds
sounds:	cmp #5, state
	jnc default

	mov r12,r13
	add r13, r13
	mov jt(r13), r0

case0:	add #1, &x
	jmp end

case1:	add #1, &x
	jmp end

case2:	add #1, &x
	jmp end
	
case3:	call #lostScreen
	jmp end
	
default:add #1, &x
	
end:	pop r0
