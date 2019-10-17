; author:     Carlo Zoccoli           ;
; creation:   16 October 2019                   ;
; last update:  16 October 2019                   ;

.data
VectorA:  	.byte  	1,1,1,1,1,1,1,1,1,1
			.byte   2,2,2,2,2,2,2,2,2,2
			.byte   3,3,3,3,3,3,3,3,3,3
			.byte  	4,4,4,4,4,4,4,4,4,4
			.byte	5,5,5,5,5,5,5,5,5,5


VectorB:  	.byte  	10,10,10,10,10,10,10,10,10,10
			.byte   20,20,20,20,20,20,20,20,20,20
			.byte   30,30,30,30,30,30,30,30,30,30
			.byte  	40,40,40,40,40,40,40,40,40,40
			.byte	50,50,50,50,50,50,50,50,50,50

Result:		.space 50	 

Max:	.byte 0
Min:	.byte 255

.text         

start:  	
			daddui r2, r0,	0 ; indice del vettore a 0
			daddui r3, r0,  49

cycle:
			lb r4, VectorA(r2)
			lb r5, VectorB(r2)
			dadd r6, r4, r5
			sb r6, Result(r2)
			daddui r2, r2, 1
			bne r2, r3, cycle

			daddui r2, r0, 0
			daddui r3, r0, 49

			lb r6, Max(r0)
			lb r5, Min(r0)

cycle2:	
			lb r4, Result(r2)
			slt r1, r6, r4	; if(max<Result[r2]) r1 = 1 else r1 = 0
			bne r1, r0, max; condizione verificata r1 != r0
			slt r1, r4, r5	; if(Result[r2]<min) r1 = 1 else r1 = 0 
			bne r1, r0, min ;condizione verificata r1 != r0

continue:	
			daddui r2, r2, 1		
			bne r2, r3, cycle2
			sb r6, Max(r0)
			sb r5, Max(r0)
			j block


max:		daddui r6, r0, 0
			dadd r6, r0, r4
  			j continue

min:		daddui r5, r0, 0
			dadd r5, r0, r4
			j continue

block: j block