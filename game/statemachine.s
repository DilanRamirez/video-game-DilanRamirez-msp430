	.file "shapemotion.c"
	.file "myShape.c"

	.extern lostScreen	;uses function from myShape.c 
	.extern state		;state is a global variable used by the state machine
	
	.data
x:	.word 0
	
	.text
jt:	.word case0 		;jump table for the state machine
	.word case1		;jt[1]
	.word case2		;...	
	.word case3		;...
	.word default		;jt[4]

	.global sounds
sounds:	cmp #5, state		;compares the number of states with the state variable
	jnc default		;if state > #cases, it jumps to default

	mov state,r13
	add r13, r13
	mov jt(r13), r0

case0:	add #1, &x
	jmp end

case1:	add #1, &x
	jmp end

case2:	add #1, &x
	jmp end
	
case3:	call #lostScreen	;it calls the function once the lives are over 
	jmp end
	
default:add #1, &x
	
end:	pop r0
