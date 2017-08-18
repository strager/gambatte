.size 8000

.text@100
	jp lbegin

.data@147
	13

.data@149
	03

.text@150
lbegin:
	ld a, 0a
	ld(0000), a
	ld a, (a000)
	add a, 05
	ld(a000), a
	ld a, 00
	ld(0000), a
	ld a, 03
	ld(4000), a
	ld a, 0a
	ld(0000), a
	ld a, (bfff)
	sub a, 09
	ld(bfff), a
	ld a, 00
	ld(0000), a
	ld a, 00
	ld(4000), a
lloop:
	halt
	jr lloop

