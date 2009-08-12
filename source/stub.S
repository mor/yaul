	.rodata

	.globl bgImg
	.balign 32
bgImg:
	.incbin "../data/background"


	.globl coverImg
	.balign 32
coverImg:
	.incbin "../data/nocover"

	.global titleImg
	.balign 32
titleImg:
	.incbin "../data/yaul_ver"
