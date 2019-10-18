; author:     Carlo Zoccoli           ;
; creation:   16 October 2019                   ;
; last update:  16 October 2019                   ;

.data
VectorA:  	.byte  	1,2,3,4,5,6,7,8,9,10
			.byte   21,22,23,24,25,26,27,28,29,30	
			.byte   31,32,33,34,35,36,37,38,39,40
			.byte  	41,42,43,44,45,46,47,48,49,50
			.byte	51,52,53,54,55,56,57,58,59,60


VectorB:  	.byte  	11,12,13,14,15,16,17,18,19,20
			.byte   20,20,20,20,20,20,20,20,20,20
			.byte   30,30,30,30,30,30,30,30,30,30
			.byte  	40,40,40,40,40,40,40,40,40,40
			.byte	50,50,50,50,50,50,50,50,50,50

Result:		.space 50	 

Max:	.space 1
Min:	.space 1

.text         

start:  	
			daddui r2, r0,	0 ; indice del vettore a 0
			daddui r3,r0, 50

cycle:
			lb r4, VectorA(r2)
			lb r5, VectorB(r2)
			dadd r6, r4, r5
			sb r6, Result(r2)
			daddui r2, r2, 1
			bne r2, r3, cycle

			daddui r2, r0, 0
			daddui r3,r0, 50

			daddui r6, r0, 0 	; max
			daddui r5, r0, 255	; min
			
cycle2:	
			lb r4, Result(r2)
			slt r1, r4, r6	; if(Result[r2]<max) r1 = 1 else r1 = 0
			beq r1, r0, max ; condizione verificata r1==r0
continue1:
			slt r1, r4, r5	; if(Result[r2]<min) r1 = 1 else r1 = 0 
			bne r1, r0, min ; condizione verificata r1 != r0
continue2:

			daddui r2, r2, 1		
			bne r2, r3, cycle2
			sb r6, Max(r0)
			sb r5, Max(r0)
			j block


max:		daddui r6, r0, 0
			dadd r6, r0, r4
  			j continue1

min:		daddui r5, r0, 0
			dadd r5, r0, r4
			j continue2

			nop

block: j block