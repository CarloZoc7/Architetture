; Architetture dei Sistemi di Elaborazione - 02LSEOV 02LSEOQ   ;
; author:	Carlo Zoccoli            ;
; creation:   23 October 2019                   ;
; last update:  23 October 2019                   ;

.data
	X:  ;.byte 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
		;.byte 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
		;.byte 255, 255, 255, 255, 255, 255, 255, 255, 255, 255

	   	.byte 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
	   	.byte 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
	   	.byte 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
	evencounter: .space 1
	oddcounter:	.space 1

.text         

; registri utilizzati
; r1 indice del vettore
; r2 massima grandezza del vettore
; r3 operando precedente
; r4 operando antecedente
; r5 risultato XOR

; r9 contatore di numero di 1 all'interno del valore stabilito
; r10, r6 risultati di shift
; r11 counter odd_counter
; r12 counter even_counter


start:
	dadd r1, r0, r0 	; indice del vettore	(5)			
	daddui r2, r0, 29	; grandezza del vettore massima		(1)

cycle:
	lb r3, X(r1)		; carico l'elemento precedente del vettore	(1)
	lb r4, 1(r1)		; carico l'elemtno antecedente del vettore	(1)
	daddui r1, r1, 1	; (1)
	
	dadd r9, r0, r0 	; azzero r9 per usarlo come contatore di 1	(1)

	xor r5, r3, r4		; CLK_end fino a questo punto(tot = 11)

counter_ones:

	andi r6, r5, 1		; svolgo and bit a bit per prendere l'ultimo bit	(1)
	dsrl r5, r5, 1		; divido per 2 tramite utilizzo di shift right		(1)

	dadd r9, r9, r6		; (1)
	bnez r5, counter_ones	;(1) peggiore dei casi 8 volte per la grandezza in byte

	dsrl r10, r9, 1		; prendo nuovamente l'ultimo bit per controllare se il numero di bit a 1 sia pari o dispari
	beqz r10, even      ;(4)
	j odd       ;(2)

continue:
	
	bne r1, r2, cycle	;(1)

	sb r12, evencounter(r0)
	sb r11, oddcounter(r0)

HALT

even:
	daddui r12, r12, 1 ;(2)
	j continue	;(1)

odd:
	daddui r11, r11, 1	;(2)
	j continue	;(1)