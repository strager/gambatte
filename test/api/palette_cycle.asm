.size 8000

.text@40
	jp lvblank

.text@100
	jp lbegin

.text@150
lbegin:
	ld a, 01
	ldff(ff), a
	ld a, 80
	ldff(40), a
	ei
lloop:
	halt
	jr lloop

.text@200
lvblank:
	ldff a, (47)
	inc a
	and a, 03
	ldff(47), a
	reti

