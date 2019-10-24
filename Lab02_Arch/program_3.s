; Architetture dei Sistemi di Elaborazione - 02LSEOV 02LSEOQ   ;
; author:	Carlo Zoccoli            ;
; creation:   23 October 2019                   ;
; last update:  23 October 2019                   ;

.data
	X: .byte 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
	even_counter:   .space 1
	odd_counter:	.space 1

.text         

start:
	daddu r1, r0, r0 
	daddui r2, r0, 9

cycle:
	lb r3, X(r1)
	daddui r5, r0, 1
	lb r4, X(r1)

	daddu r6, r0, r0

power:	
	mul r5, r1, r3
	daddui r6, r6, 1
	bne r6, r4, power



	daddui r1, r1, 1
	bne r1, r2, cycle
		
HALT