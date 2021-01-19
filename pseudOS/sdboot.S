.section .text.boot

sd_start:
	la a0, 0x92000000
	li a1, 0
	sw a1, 24(a0)
	sw a1, 28(a0)
	sw a1, 32(a0)
	sw a1, 36(a0)

	la a0, 0x94000000
	li a1, 0x61616161
	sw a1, (a0)
	sw a1, 4(a0)
	sw a1, 8(a0)
	sw a1, 12(a0)
	sw a1, 364(a0)
	sw a1, 368(a0)
	sw a1, 372(a0)
	sw a1, 724(a0)
	sw a1, 728(a0)
	sw a1, 732(a0)

#	# ----
#	li a2, 5580
#	add a0, a0, a2
#	li a1, 0x62626262
#	sw a1, (a0)
#	li a1, 0x63636363
#	sw a1, 4(a0)
#	li a1, 0x64646464
#	sw a1, 8(a0)
#
#	la a0, 0x94000000
#	li a2, 540
#	add a0, a0, a2
#	li a1, 0x02626262
#	sw a1, (a0)
#	li a1, 0x03636363
#	sw a1, 4(a0)
#	li a1, 0x04646464
#	sw a1, 8(a0)
#
#	la a0, 0x94000000
#	li a2, 1980
#	add a0, a0, a2
#	li a1, 0x69696262
#	sw a1, (a0)
#	li a1, 0x6a6a6363
#	sw a1, 4(a0)
#	li a1, 0x6b6b6464
#	sw a1, 8(a0)
#
#	# works
#	la a0, 0x94000000
#	li a2, 1260
#	add a0, a0, a2
#	li a1, 0x79796262
#	sw a1, (a0)
#	li a1, 0x7a7a6363
#	sw a1, 4(a0)
#	li a1, 0x7b7b6464
#	sw a1, 8(a0)
#
#	la a0, 0x94000000
#	li a2, 7236
#	add a0, a0, a2
#	li a1, 0x79306262
#	sw a1, (a0)
#	li a1, 0x7a316363
#	sw a1, 4(a0)
#	li a1, 0x7b326464
#	sw a1, 8(a0)
#	li a1, 0x7b336464
#	sw a1, 12(a0)
#	li a1, 0x7b346464
#	sw a1, 16(a0)
#	li a1, 0x7b356464
#	sw a1, 20(a0)
#
#	la a0, 0x94000000
#	li a2, 9396
#	add a0, a0, a2
#	li a1, 0x79306262
#	sw a1, (a0)
#	li a1, 0x7a316363
#	sw a1, 4(a0)
#	li a1, 0x7b326464
#	sw a1, 8(a0)
#	li a1, 0x7b336464
#	sw a1, 12(a0)
#	li a1, 0x7b346464
#	sw a1, 16(a0)
#	li a1, 0x7b356464
#	sw a1, 20(a0)

	la a0, 0x94000000
	li a1, 0x30300000


	# works
	la a0, 0x94000000
	li a2, 1096
	add a0, a0, a2
	li a1, 0x30306262
	sw a1, (a0)
	li a1, 0x31306363
	sw a1, 4(a0)
	li a1, 0x32306464
	sw a1, 8(a0)
	li a1, 0x33306464
	sw a1, 12(a0)
	li a1, 0x34306464
	sw a1, 16(a0)
	li a1, 0x35306464
	sw a1, 380(a0)
	j sd_start
