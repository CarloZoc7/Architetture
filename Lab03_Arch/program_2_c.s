; Architetture dei Sistemi di Elaborazione - 02LSEOV 02LSEOQ   ;
; author:	Carlo Zoccoli            ;
; creation:   23 October 2019                   ;
; last update:  23 October 2019                   ;

.data
	v1:	.double		40.53,11.38,2.29,94.38,6.49,46.36,55.71,81.8,49.75,51.35,79.9,2.12,32.34,48.21,46.11,65.57,23.49,41.03,79.57,89.27,44.44,72.57,18.77,61.88,89.61,75.62,28.36,99.2,54.66,70.35
	v2:	.double		5.62,23.8,32.29,8.41,23.07,33.62,13.51,56.15,44.71,58.32,96.59,10.49,75.65,98.9,31.58,54.62,45.78,18.57,21.81,74.66,64.68,2.37,57.09,88.4,14.92,80.45,16.99,64.01,19.09,81.99
	v3:	.double		32.51,24.38,23.79,26.67,43.77,81.93,20.07,19.4,93.04,80.01,19.66,23.93,1.74,57.46,50.49,60.06,20.67,17.26,53.78,55.85,54.54,55.22,96.31,1.18,7.31,92.72,89.07,24.12,98.13,41.33
	v4:	.double		58.43,71.98,48.52,63.35,14.17,54.41,37.51,41.25,56.39,64.09,90.08,57.17,51.33,65.47,30.82,15.62,1.51,38.11,77.22,64.7,6.84,82.65,51.54,30.1,56.1,52.53,42.78,50,2.84,59.65
	v5:	.space		240
	v6:	.space		240


.text         
	;	for(i=0; i<30; i++)
	;		v5[i] = (v1[i]*v2[i]+v3[i])
	;		v6[i] = (v3[i]*v4[i])/v5[i]

	;	CAMBIAMENTO CON UNROLL
	;	for(i=0; i<30; i=i+3)
	;		v5[i] = (v1[i]*v2[i]+v3[i])
	;		v6[i] = (v3[i]*v4[i])/v5[i]
	;
	;		v15[i+1] = (v11[i+1]*v12[i+1]+v13[i+1])
	;		v16[i+1] = (v13[i+1]*v14[i+1])/v15[i+1]
	;
	;		v25[i+2] = (v21[i+2]*v22[i+2]+v23[i+2])
	;		v26[i+2] = (v23[i+2]*v24[i+2])/v25[i+2]
start:
	
		dadd r1, r1, r0 ; r1 indice del vettore
		daddui r2, r2, 240
cycle:
			
		daddui r11, r1, 8
		daddui r21, r1, 16

		l.d f1, v1(r1)
		l.d f2, v2(r1)
		l.d f3, v3(r1)
		l.d f4, v4(r1)



		l.d f11, v1(r11)
		l.d f12, v2(r11)
		l.d	f13, v3(r11)
		l.d f14, v4(r11)

		l.d f21, v1(r21)
		l.d f22, v2(r21)
		l.d	f23, v3(r21)
		l.d f24, v4(r21)

		mul.d f5, f1, f2	; f5 = f1*f2
		mul.d f6, f3, f4	; f6 = f3*f4

		mul.d f15, f11, f12
		mul.d f16, f13, f14

		mul.d f25, f21, f22
		mul.d f26, f23, f24
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		add.d f5, f5, f3	; f5 = f5*f3
		add.d f15, f15, f13	; f15= f15*f13
		add.d f25, f25, f23 ; f25 = f25*f23
		nop
		nop
		nop					; fase di execute
		div.d f6, f6, f5	; f6 = f6/f5
		div.d f16, f16, f15
		div.d f26, f26, f25
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		s.d f5, v5(r1)
		s.d f6, v6(r1)
		
		s.d f15, v5(r11)
		s.d f16, v6(r11)

		s.d f25, v5(r21)
		s.d f26, v6(r21)
		daddui r1, r1, 24 ; per i=i+3 sará pari a i = i+24
		nop
		nop
		nop
		bne r1, r2, cycle
		nop

HALT