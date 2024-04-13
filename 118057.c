static int thumb_assemble(ArmOpcode *ao, ut64 off, const char *str) {
	ut64 m;
	ao->o = UT32_MAX;
	if (!strcmpnull (ao->op, "udf")) {
		ao->o = 0xde;
		ao->o |= getnum (ao->a[0]) << 8;
		return 2;
	} else
	if ((m = opmask (ao->op, "add", S_BIT | W_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_CONST: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut32 num = getnum (ao->a[2]);

			if ((reg1 > 15) || (reg2 > 15)) {
				return -1;
			}
			
			if (reg2 == 13) {
				if ((reg1 < 8) && (num < 1024) && (num % 4 == 0) && (!(m & DOTW_BIT)) && (!(m & W_BIT))) {
					ao->o = 0x00a8;
					ao->o |= reg1;
					ao->o |= (num >> 2) << 8;
					return 2;
				}

				if ((reg1 == 13) && (num < 512) && (num % 4 == 0) && (!(m & DOTW_BIT)) && (!(m & W_BIT))) {
					ao->o = 0x00b0;
					ao->o |= (num >> 2) << 8;
					return 2;
				}

				err = false;
				ut32 thnum = getthimmed12 (ao->a[2]);
				if (!err && (!(m & W_BIT))) {
					ao->o = 0x0df10000;
					ao->o |= reg1;
					ao->o |= thnum;
					if (m & S_BIT) {
						ao->o |= 1 << 28;
					}
					return 4;
				}

				if (num > 4095) {
					return -1;
				}

				ao->o = 0x0df20000;
				ao->o |= reg1;
				ao->o |= getthzeroimmed12 (num);
				return 4;
			}

			if (num < 8) {
				ao->o = 0x001c;
				ao->o |= (num & 0x3) << 14;
				ao->o |= (num >> 2);
				if (std_16bit_2reg (ao, m)) {
					return 2;
				}
			}

			if ((reg1 < 8) && (reg1 == reg2) && (num < 256)) {
				ao->o = 0x0030;
				ao->o |= reg1;
				ao->o |= num << 8;
				return 2;
			}

			err = false;
			ut32 thnum = getthimmed12 (ao->a[2]);
			if (!err && (!(m & W_BIT))) {
				ao->o = 0x00f10000;
				ao->o |= thnum;
				return std_32bit_2reg (ao, m, false);
			}

			if (num > 4095) {
				return -1;
			}

			ao->o = 0x00f20000;
			ao->o |= getthzeroimmed12 (num);
			return std_32bit_2reg (ao, m, false);
		        }
			break;
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->a[3] = "lsl 0";
		        }
			// intentional fallthrough
		case THUMB_REG_REG_SHIFT: {
			if (ao->a[3] == NULL) { // double fallthrough
				std_opt_3 (ao);
			}
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG_SHIFT: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getreg (ao->a[2]);
			ut32 shift = thumb_getshift (ao->a[3]);

			if ((reg1 > 15) || (reg2 > 15) || (reg3 > 15)) {
				return -1;
			}
				
			if (reg2 == 13) {
				if ((reg1 == reg3) && (!(m & DOTW_BIT)) && (shift == 0)) {
					ao->o = 0x6844;
					ao->o |= (reg1 & 0x7) << 8;
					ao->o |= (reg1 >> 3) << 15;
					return 2;
				}

				if ((reg1 == 13) && (!(m & DOTW_BIT)) && (shift == 0)) {
					ao->o = 0x8544;
					ao->o |= reg3 << 11;
					return 2;
				}

				ao->o = 0x0deb0000;
				ao->o |= reg1;
				ao->o |= reg3 << 8;
				ao->o |= shift;
				if (m & S_BIT) {
					ao->o |= 1 << 28;
				}
				return 4;
			}

			if ((reg3 < 8) && (!(m & DOTW_BIT)) && (shift == 0)) {
				ao->o = 0x0018;
				ao->o |= (reg3 >> 2);
				ao->o |= (reg3 & 0x3) << 14;
				if (std_16bit_2reg (ao, m)) {
					return 2;
				}
			}

			if ((reg1 == reg2) && (!(m & DOTW_BIT)) && (shift == 0)) {
				ao->o = 0x0044;
				ao->o |= (reg1 & 0x7) << 8;
				ao->o |= (reg1 >> 3) << 15;
				ao->o |= reg3 << 11;
				return 2;
			}

			ao->o = 0x00eb0000;
			return std_32bit_3reg (ao, m, true);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "adc", S_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_CONST: {
			ao->o = 0x40f10000;
			ao->o |= getthimmed12 (ao->a[2]);
			return std_32bit_2reg (ao, m, false);
		        }
			break;
		case THUMB_REG_REG: {
			ao->o = 0x4041;
			if (std_16bit_2reg (ao, m)) {
				return 2;
			}
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0x40eb0000;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		case THUMB_REG_REG_SHIFT: {
			std_opt_3 (ao);
		        }
			// intentional fallthrough
			// a bit naughty, perhaps?
		case THUMB_REG_REG_REG_SHIFT: {
			ao->o = 0x40eb0000;
			return std_32bit_3reg(ao, m, true);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "adr", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST: {
			ut8 reg = getreg (ao->a[0]);
			st32 label = getnum (ao->a[1]);
			if ( !(m & DOTW_BIT) && (reg < 8) && (label < 1024) && (label >= 0) && (label % 4 == 0)) {
				ao->o = 0x00a0;
				ao->o |= reg;
				ao->o |= (label / 4) << 8;
				return 2;
			} else if ((label < 0) && (label > -4096)) {
				if (m & DOTN_BIT) {
					// this is explicitly an error
					return -1;
				}
				ao->o = 0xaff20000;
				ao->o |= reg;
				ao->o |= getthzeroimmed12 (-label);
				return 4;
			} else if ((label > 0) && (label < 4096)) {
				if (m & DOTN_BIT) {
					// this is explicitly an error
					return -1;
				}
				ao->o = 0x0ff20000;
				ao->o |= reg;
				ao->o |= getthzeroimmed12 (label);
				return 4;
			}
			return -1;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "and", S_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			ao->o = 0x0040;
			if (std_16bit_2reg (ao, m)) {
				return 2;
			}
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0x00ea0000;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		case THUMB_REG_CONST: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_CONST: {
			ut32 imm = getthimmed12 (ao->a[2]);
			ao->o = 0x00f00000;
			ao->o |= imm;
			return std_32bit_2reg (ao, m, false);
		        }
			break;
		case THUMB_REG_REG_SHIFT: {
			std_opt_3 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG_SHIFT: {
			ao->o = 0x00ea0000;
			return std_32bit_3reg (ao, m, true);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "asr", S_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG_CONST: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut32 imm = getnum (ao->a[2]);
			if (((int)imm < 1) && ((int)imm > 32)) {
				return -1;
			}
			ao->o = 0x0010;
			ao->o |= (imm & 0x3) << 14;
			ao->o |= (imm & 0x1c) >> 2;
			if (std_16bit_2reg (ao, m)) {
				return 2;
			}
			ao->o = 0x4fea2000;
			ao->o |= reg1;
			ao->o |= reg2 << 8;
			ao->o |= (imm & 0x3) << 14;
			ao->o |= (imm & 0x1c) << 2;
			if (m & S_BIT) {
				ao->o |= 1 << 28;
			}
			return 4;
		        }
			break;
		case THUMB_REG_REG: {
			ao->o = 0x0041;
			if (std_16bit_2reg (ao, m)) {
				return 2;
			}
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0x40fa00f0;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if (( m = opmask (ao->op, "b", 0) )) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_CONST: {
			st32 offset = thumb_getoffset (ao->a[0], off);
			if (offset % 2 != 0) {
				return -1;
			}

			if ((m & C_BITS) == C_BITS) {
				if ((offset >= -2048) && (offset <= 2046) && (!(m & DOTW_BIT))) {
					ao->o = 0x00e0;
					ao->o |= ((offset/2 & 0xff) << 8);
					ao->o |= ((offset/2 & 0x700) >> 8);
					return 2;
				}
				if ((offset < -16777216) || (offset > 16777214) || (offset % 2 != 0)) {
					return -1;
				}
				ao->o = 0x00f00090;
				ao->o |= getthbimmed(offset);
				return 4;
			} else {
				if ((offset >= -256) && (offset <= 254) && (!(m & DOTW_BIT))) {
					ao->o = 0x00d0;
					ao->o |= (ut16) ((offset/2) << 8);
					ao->o |= ((m & C_BITS) >> 2);
					return 2;
				}
				if ((offset < -1048576) || (offset > 1048574) || (offset % 2 != 0)) {
					return -1;
				}

				ao->o = 0x00f00080;
				ao->o |= (ut32)(offset & 0x80000) >> 16;
				ao->o |= (ut32)(offset & 0x40000) >> 13;
				ao->o |= (ut32)(offset & 0x3f000) << 12;
				ao->o |= (ut32)(offset & 0xe00) >> 9;
				ao->o |= (ut32)(offset & 0x1fe) << 7;
				if (offset < 0) {
					ao->o |= 1 << 18;
				}
				ao->o |= (((m & C_BITS) & 0xc) << 28);
				ao->o |= (((m & C_BITS) & 0x30) << 12);
				return 4;
			}
		        }
			break;
		default:
			return -1;
		}
	} else
	if (( m = opmask (ao->op, "bl", 0) )) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_CONST: {
			st32 offset = thumb_getoffset (ao->a[0], off);
			ao->o = 0x00f000d0;
			if ((offset > 16777214) || (offset < -16777216) || (offset % 2 != 0)) {
				return -1;
			}
			ao->o |= getthbimmed(offset);
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if (( m = opmask (ao->op, "bx", 0) )) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG: {
			ut32 reg1 = getreg (ao->a[0]);
			ao->o = 0x0047;
			ao->o |= reg1 << 11;
			return 2;
		        }
			break;
		default:
			return -1;
		}
	} else 
	if (( m = opmask (ao->op, "blx", 0) )) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG: {
			ut32 reg1 = getreg (ao->a[0]);
			ao->o = 0x8047;
			ao->o |= reg1 << 11;
			return 2;
		        }
			break;
		case THUMB_CONST: {
			st32 offset = thumb_getoffset (ao->a[0], off);
			ao->o = 0x00f000c0;
			if ((offset > 16777214) || (offset < -16777216) || (offset % 2 != 0)) {
				return -1;
			}
			offset += off & 0x2; // (Align(PC,4)
			ao->o |= getthbimmed (offset);
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if (( m = opmask (ao->op, "bfc", 0) )) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST_CONST: {
			if (m & DOTN_BIT) {
				// this is explicitly an error
				return -1;
			}
			ut8 reg1 = getreg (ao->a[0]);
			ut32 lsb = getnum (ao->a[1]);
			ut32 width = getnum (ao->a[2]);
			ut32 msb = lsb + width - 1;
			if ((lsb > 31) || (msb > 31)) {
				return -1;
			}
			ao->o = 0x6ff30000;
			ao->o |= reg1;
			ao->o |= ((lsb & 0x1c) << 2);
			ao->o |= ((lsb & 0x3) << 14);
			ao->o |= (msb << 8);
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if (( m = opmask (ao->op, "bfi", 0) )) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG_CONST_CONST: {
			ut32 lsb = getnum (ao->a[2]);
			ut32 width = getnum (ao->a[3]);
			ut32 msb = lsb + width - 1;
			if ((lsb > 31) || (msb > 31)) {
				return -1;
			}
			ao->o = 0x60f30000;
			ao->o |= ((lsb & 0x1c) << 2);
			ao->o |= ((lsb & 0x3) << 14);
			ao->o |= (msb << 8);
			return std_32bit_2reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if (( m = opmask (ao->op, "bic", S_BIT) )) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			ao->o = 0x8043;
			if (std_16bit_2reg (ao, m)) {
				return 2;
			}
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0x20ea0000;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		case THUMB_REG_CONST: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_CONST: {
			ao->o = 0x20f00000;
			ao->o |= getthimmed12 (ao->a[2]);
			return std_32bit_2reg (ao, m, false);
		        }
			break;
		case THUMB_REG_REG_SHIFT: {
			std_opt_3 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG_SHIFT: {
			ao->o = 0x20ea0000;
			return std_32bit_3reg (ao, m, true);
		        }
			break;
		default:
			return -1;
		}
	} else
	if (( m = opmask (ao->op, "bkpt", 0) )) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_CONST: {
			ut32 num = getnum (ao->a[0]);
			if (num > 255) {
				return -1;
			}
			ao->o = 0x00be;
			ao->o |= num << 8;
			return 2;
		        }
			break;
		default:
			return -1;
		}
	} else
	if (( m = opmask (ao->op, "cbnz", 0) )) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST: {
			ut8 reg1 = getreg (ao->a[0]);
			st32 offset = thumb_getoffset (ao->a[1], off);
			if ((reg1 > 7) || (offset > 127) || (offset % 2 != 0)) {
				return -1;
			}
			ao->o = 0x00b9;
			ao->o |= reg1 << 8;
			ao->o |= (offset & 0x3e) << 10;
			ao->o |= (offset & 0x40) >> 5;
			return 2;
		        }
			break;
		default:
			return -1;
		}
	} else
	if (( m = opmask (ao->op, "cbz", 0) )) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST: {
			ut8 reg1 = getreg (ao->a[0]);
			st32 offset = thumb_getoffset (ao->a[1], off);
			if ((reg1 > 7) || (offset > 127) || (offset % 2 != 0)) {
				return -1;
			}
			ao->o = 0x00b1;
			ao->o |= reg1 << 8;
			ao->o |= (offset & 0x3e) << 10;
			ao->o |= (offset & 0x40) >> 5;
			return 2;
		        }
			break;
		default:
			return -1;
		}
	} else
	if (( m = opmask (ao->op, "cdp", TWO_BIT) )) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_COPROC_CONST_COREG_COREG_COREG: {
			ao->a[5] = "0";
		        }
			//intentional fallthrough
		case THUMB_COPROC_CONST_COREG_COREG_COREG_CONST: {
			ut32 coproc = getcoproc (ao->a[0]);
			ut32 opc1 = getnum (ao->a[1]);
			ut8 reg1 = getcoprocreg (ao->a[2]);
			ut8 reg2 = getcoprocreg (ao->a[3]);
			ut8 reg3 = getcoprocreg (ao->a[4]);
			ut32 opc2 = getnum (ao->a[5]);
		        if ((coproc > 15) || (opc1 > 15) || (opc2 > 7)) {
				return -1;
			}
			ao->o = 0x00ee0000;
			if (m & TWO_BIT) {
				ao->o |= 1 << 20;
			}
			ao->o |= coproc;
			ao->o |= opc1 << 28;
			ao->o |= reg1 << 4;
			ao->o |= reg2 << 24;
			ao->o |= reg3 << 8;
			ao->o |= opc2 << 13;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else 	
	if (( m = opmask (ao->op, "clrex", 0) )) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_NONE: {
			ao->o = 0xbff32f8f;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if (( m = opmask (ao->op, "clz", 0) )) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			ao->o = 0xb0fa80f0;
			ao->a[2] = ao->a[1];
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if (( m = opmask (ao->op, "cmn", 0) )) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST: {
			ut8 reg1 = getreg (ao->a[0]);
			ut32 num = getthimmed12 (ao->a[1]);
			ao->o = 0x10f1000f;
			ao->o |= reg1 << 24;
			ao->o |= num;
			return 4;
		        }
			break;
		case THUMB_REG_REG: {
			ao->o = 0xc042;
			if (std_16bit_2reg (ao, m)) {
				return 2;
			}
			ao->a[2] = "lsl 0";
		        }
			// intentional fallthrough
		case THUMB_REG_REG_SHIFT: {
			ao->o = 0x10eb000f;
			return std_32bit_2reg (ao, m, true);
		        }
			break;
		default:
			return -1;
		}
	} else
	if (( m = opmask (ao->op, "cmp", 0) )) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST: {
			ut8 reg1 = getreg (ao->a[0]);
			ut32 num = getnum (ao->a[1]);
			if ((num < 256) && (!(m & DOTW_BIT))) {
				ao->o = 0x0028;
				ao->o |= reg1;
				ao->o |= num << 8;
				return 2;
			}
			num = getthimmed12 (ao->a[1]);
			ao->o = 0xb0f1000f;
			ao->o |= reg1 << 24;
			ao->o |= num;
			return 4;
		        }
			break;
		case THUMB_REG_REG: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ao->o = 0x8042;
			if (std_16bit_2reg (ao, m)) {
				return 2;
			}
			if (!(m & DOTW_BIT)) {
				ao->o = 0x0045;
				ao->o |= ((reg1 & 0x7) << 8);
				ao->o |= ((reg1 & 0x8) << 12);
				ao->o |= reg2 << 11;
				return 2;
			}
			ao->a[2] = "lsl 0";
		        }
			// intentional fallthrough
		case THUMB_REG_REG_SHIFT: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut32 shift = thumb_getshift (ao->a[2]);
			ao->o = 0xb0eb000f;
			ao->o |= reg1 << 24;
			ao->o |= reg2 << 8;
			ao->o |= shift;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if (( m = opmask (ao->op, "cps", ID_BIT | IE_BIT) )) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_OTHER: {
			st8 aif = iflag(ao->a[0]);
			if (aif == -1) {
				return -1;
			}
			if (!(m & DOTW_BIT)) {
				ao->o = 0x60b6;
				ao->o |= aif << 8;
				if (m & ID_BIT) {
					ao->o |= 1 << 12;
				}
				return 2;
			}
			ao->a[1] = "0";
		        }
			// intentional fallthrough
		case THUMB_OTHER_CONST: {
			st8 aif = iflag(ao->a[0]);
			ut8 mode = getnum (ao->a[1]);
			if ((mode > 31) || (aif == -1)) {
				return -1;
			}
			ao->o = 0xaff30085;
			ao->o |= mode << 8;
			ao->o |= aif << 13;
			if (m & ID_BIT) {
				ao->o |= 1 << 1;
			}
			return 4;
		        }
			break;
		case THUMB_CONST: {
			ut8 mode = getnum (ao->a[0]);
			if ((m & ID_BIT) || (m & IE_BIT) || (mode > 31)) {
				return -1;
			}
			ao->o = 0xaff30081;
			ao->o |= mode << 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "dbg", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_CONST: {
			ut32 option = getnum (ao->a[0]);
			if (option > 15) {
				return -1;
			}
			ao->o = 0xaff3f080;
			ao->o |= option << 8;
			return 4;
		        }
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "dmb", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_NONE: {
			ao->o = 0xbff35f8f;
			return 4;
		        }
			break;
		case THUMB_OTHER: {
			r_str_case (ao->a[0], false);
			if (strcmpnull (ao->a[0], "sy")) {
				return -1;
			}
			ao->a[0] = "15";
		        }
			// intentional fallthrough
		case THUMB_CONST: {
			ut32 option = getnum (ao->a[0]);
			if (option != 15) {
				return -1;
			}
			ao->o = 0xbff3508f;
			ao->o |= option << 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "dsb", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_NONE: {
			ao->o = 0xbff34f8f;
			return 4;
		        }
			// intentional fallthrough
		case THUMB_OTHER: {
			r_str_case (ao->a[0], false);
			if (!strcmpnull ((ao->a[0] = parse_hints(ao->a[0])), "-1")) {
				return -1;
			}
		        }
			// intentional fallthrough
		case THUMB_CONST: {
			ut32 option = getnum (ao->a[0]);
			if ((option != 6) && (option != 7) && (option != 14) && (option != 15)) {
				return -1;
			}
			ao->o = 0xbff3408f;
			ao->o |= option << 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "eor", S_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST:
			std_opt_2 (ao);
			// intentional fallthrough
		case THUMB_REG_REG_CONST: {
			err = false;
			ut32 imm = getthimmed12 (ao->a[2]);
			if (err) {
				return -1;
			}
			ao->o = 0x80f00000;
			ao->o |= imm;
			return std_32bit_2reg (ao, m, false);
		        }
			break;
		case THUMB_REG_REG: {
			ao->o = 0x4040;
			if (std_16bit_2reg (ao, m)) {
				return 2;
			}
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG:
			ao->a[3] = "lsl 0";
			// intentional fallthrough
		case THUMB_REG_REG_REG_SHIFT: {
			ao->o = 0x80ea0000;
			return std_32bit_3reg (ao, m, true);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "isb", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_NONE: {
			ao->o = 0xbff36f8f;
			return 4;
		        }
			// intentional fallthrough
		case THUMB_OTHER: {
			r_str_case (ao->a[0], false);
			if (strcmpnull (ao->a[0], "sy")) {
				return -1;
			}
			ao->a[0] = "15";
		        }
			// intentional fallthrough
		case THUMB_CONST: {
			ut32 option = getnum (ao->a[0]);
			if (option != 15) {
				return -1;
			}
			ao->o = 0xbff3608f;
			ao->o |= option << 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = itmask (ao->op))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_OTHER: {
			ut16 cond = 0;
			ut16 i;
			
			const char *conds[] = {
				"eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
				"hi", "ls", "ge", "lt", "gt", "le", "al", "nv", 0
			};
			r_str_case (ao->a[0], false);
			for (i = 0; conds[i]; i++) {
				if (!(strcmpnull(ao->a[0], conds[i]))) {
					cond = i;
					break;
				}
			}

			if (i == 16) {
				return -1;
			}
			ao->o = 0x00bf;
			ao->o |= cond << 12;

			ut8 nrcs = (m & 0x30) >> 4;
			ut8 thiset = 0;

			for (i = 0; i < nrcs; i++) {
				thiset = ((m & (1 << (3 - i))) >> (3 - i));
				ao->o |= ((cond & 0x1) ^ thiset) << (11 - i);
			}
			ao->o |= 1 << (11 - i);
			return 2;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "ldc", TWO_BIT | L_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_COPROC_COREG_BRACKREG_CONSTBRACK: {
			ut8 proc = getcoproc (ao->a[0]);
			ut8 reg1 = getcoprocreg (ao->a[1]);
			ut8 reg2 = getregmemstart (ao->a[2]);
			st32 imm = getnummemend (ao->a[3]);
			ao->o = 0x10ed0000;
			if (m & L_BIT) {
				ao->o |= 1 << 30;
			}
			if (m & TWO_BIT) {
				ao->o |= 1 << 20;
			}
			if (imm < 0) {
				imm = -imm;
			} else {
				ao->o |= 1 << 31;
			}				
			if ((proc > 15) || (reg1 > 15) || (reg2 > 15) || (imm > 1024) || (imm % 4 != 0)) {
				return -1;
			}
			ao->o |= proc;
			ao->o |= reg1 << 4;
			ao->o |= (imm >> 2) << 8;
			ao->o |= reg2 << 24;
			return 4;
		        }
			break;
		case THUMB_COPROC_COREG_BRACKREGBRACK:
			ao->a[3] = "0";
			// intentional fallthrough
		case THUMB_COPROC_COREG_BRACKREGBRACK_CONST: {
			ut8 proc = getcoproc (ao->a[0]);
			ut8 reg1 = getcoprocreg (ao->a[1]);
			ut8 reg2 = getregmemstartend (ao->a[2]);
			st32 imm = getnum (ao->a[3]);
			ao->o = 0x30ec0000;
			if (m & L_BIT) {
				ao->o |= 1 << 30;
			}
			if (m & TWO_BIT) {
				ao->o |= 1 << 20;
			}
			if (imm < 0) {
				imm = -imm;
			} else {
				ao->o |= 1 << 31;
			}				
			if ((proc > 15) || (reg1 > 15) || (reg2 > 15) || (imm > 1024) || (imm % 4 != 0)) {
				return -1;
			}
			ao->o |= proc;
			ao->o |= reg1 << 4;
			ao->o |= (imm >> 2) << 8;
			ao->o |= reg2 << 24;
			return 4;
		        }
			break;
		case THUMB_COPROC_COREG_BRACKREG_CONSTBRACKBANG: {
			ut8 proc = getcoproc (ao->a[0]);
			ut8 reg1 = getcoprocreg (ao->a[1]);
			ut8 reg2 = getregmemstart (ao->a[2]);
			st32 imm = getnummemendbang (ao->a[3]);
			ao->o = 0x30ed0000;
			if (m & L_BIT) {
				ao->o |= 1 << 30;
			}
			if (m & TWO_BIT) {
				ao->o |= 1 << 20;
			}
			if (imm < 0) {
				imm = -imm;
			} else {
				ao->o |= 1 << 31;
			}				
			if ((proc > 15) || (reg1 > 15) || (reg2 > 15) || (imm > 1024) || (imm % 4 != 0)) {
				return -1;
			}
			ao->o |= proc;
			ao->o |= reg1 << 4;
			ao->o |= (imm >> 2) << 8;
			ao->o |= reg2 << 24;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "ldm", DB_BIT | EA_BIT | IA_BIT | FD_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REGBANG_LIST: {
			ut8 reg1 = getregmembang (ao->a[0]);
			ut32 list = getreglist (ao->a[1]);
			if (!((m & DB_BIT) || (m & EA_BIT)) && !(list & 0xff00) && (reg1 < 8) && !(m & DOTW_BIT)) {
				ao->o = 0x00c8;
				ao->o |= reg1;
				if (list & (1 << reg1)) {
					list ^= 1 << (reg1);
				}
				ao->o |= (list & 0xff) << 8;
					
				return 2;
			}
			if (list & 0x2000) {
				return -1;
			}
			if ((m & DB_BIT) || (m & EA_BIT)) {
				ao->o = 0x30e90000;
			} else {
				// ldmia is the default!
				ao->o = 0xb0e80000;
			}
				
			ao->o |= reg1 << 24;
			ao->o |= (list & 0xff) << 8;
			ao->o |= (list & 0xff00) >> 8;
			return 4;
		        }
			break;
		case THUMB_REG_LIST: {
			ut8 reg1 = getreg (ao->a[0]);
			ut32 list = getreglist (ao->a[1]);
			if (!((m & DB_BIT) || (m & EA_BIT)) && !(list & 0xff00) && (reg1 < 8) && !(m & DOTW_BIT)) {
				ao->o = 0x00c8;
				ao->o |= reg1;
				ao->o |= 1 << (reg1 + 8);
				ao->o |= (list & 0xff) << 8;
				return 2;
			}
			if (list & 0x2000) {
				return -1;
			}
			
			if ((m & DB_BIT) || (m & EA_BIT)) {
				ao->o = 0x10e90000;
			} else {
				ao->o = 0x90e80000;
			}
				
			ao->o |= reg1 << 24;
			ao->o |= (list & 0xff) << 8;
			ao->o |= (list & 0xff00) >> 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "ldr", B_BIT | H_BIT | D_BIT | T_BIT | S_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		ut32 ldrsel = m & (B_BIT | H_BIT | D_BIT);
		if ((m & S_BIT) && !(m & (B_BIT | H_BIT))) {
			return -1;
		}
		switch (argt) {
		case THUMB_REG_CONST:
			ao->a[2] = ao->a[1];
			strcat (ao->a[2],"]");
			ao->a[1] = "[r15";
			// intentional fallthrough
		case THUMB_REG_BRACKREGBRACK:
			if (ao->a[2] == NULL) { // double fallthrough
				ao->a[1][strlen (ao->a[1]) -1] = '\0';
				ao->a[2] = "0]";
			}
			// intentional fallthrough
		case THUMB_REG_BRACKREG_CONSTBRACK: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getregmemstart (ao->a[1]);
			st32 num = getnummemend (ao->a[2]);
			if (ldrsel == 0) {
				if (m & T_BIT) {
					if ((num < 0) || (num > 255)) {
						return -1;
					}
					ao->o = 0x50f8000e;
					ao->o |= num << 8;
					return mem_32bit_2reg (ao, m);
				}
				if (reg2 == 15) {
					if ((num > 4095) || (num < -4095)) {
						return -1;
					}
					if ((reg1 < 8) && (num < 1024) && (num % 4 == 0)) {
						ao->o = 0x0048;
						ao->o |= reg1;
						ao->o |= (num >> 2) << 8;
						return 2;
					}
					ao->o = 0x5ff80000;
					if (num < 0) {
						num = -num;
					} else {
						ao->o |= 1 << 31;
					}
					ao->o |= reg1 << 4;
					ao->o |= (num & 0xff) << 8;
					ao->o |= (num & 0x0f00) >> 8;
					return 4;
				}
				if ((reg2 == 13) && (reg1 < 8) && (num >= 0) && (num < 1024) && (num % 4 == 0) && (!(m & DOTW_BIT))) {
					ao->o = 0x0098;
					ao->o |= reg1;
					ao->o |= (num >> 2) << 8;
					return 2;
				}
				if ((num >= 0) && (num < 128) && (num % 4 == 0)) {
					ao->o = 0x0068;
					ao->o |= (num >> 4);
					ao->o |= ((num >> 2) & 0x3) << 14;
					if (mem_16bit_2reg (ao, m)) {
						return 2;
					}
				}
				if ((num > 4095) || (num < -1023)) {
					return -1;
				}
				if (num >= 0) {
					ao->o = 0xd0f80000;
					ao->o |= (num & 0xff) << 8;
					ao->o |= (num & 0xf00) >> 8;
					return mem_32bit_2reg (ao, m);
				}
				ao->o = 0x50f8000c;
				ao->o |= (-num & 0xff) << 8;
				return mem_32bit_2reg (ao, m);
			} else
			if (ldrsel == B_BIT) {
				if (m & T_BIT) {
					if ((num < 0) || (num > 255)) {
						return -1;
					}
					ao->o = 0x10f8000e;
					if (m & S_BIT) {
						ao->o |= 1 << 16;
					}
					ao->o |= num << 8;
					return mem_32bit_2reg (ao, m);
				}
				if (reg2 == 15) {
					if ((num > 4095) || (num < -4095)) {
						return -1;
					}
					ao->o = 0x1ff80000;
					if (m & S_BIT) {
						ao->o |= 1 << 16;
					}
					if (num < 0) {
						num = -num;
					} else {
						ao->o |= 1 << 31;
					}
					ao->o |= reg1 << 4;
					ao->o |= (num & 0xff) << 8;
					ao->o |= (num & 0x0f00) >> 8;
					return 4;
				}
				if ((num >= 0) && (num < 32) && (!(m & S_BIT))) {
					ao->o = 0x0078;
					ao->o |= (num >> 2);
					ao->o |= (num & 0x3) << 14;
					if (mem_16bit_2reg (ao, m)) {
						return 2;
					}
				}
				if ((num > 4095) || (num < -255)) {
					return -1;
				}
				if (num >= 0) {
					ao->o = 0x90f80000;
					if (m & S_BIT) {
						ao->o |= 1 << 16;
					}
					ao->o |= (num & 0xff) << 8;
					ao->o |= (num & 0xf00) >> 8;
					return mem_32bit_2reg (ao, m);
				}
				ao->o = 0x10f8000c;
				if (m & S_BIT) {
					ao->o |= 1 << 16;
				}
				ao->o |= -num << 8;
				return mem_32bit_2reg (ao, m);
			} else
			if (ldrsel == H_BIT) {
				if (m & T_BIT) {
					if ((num < 0) || (num > 255)) {
						return -1;
					}
					ao->o = 0x30f8000e;
					if (m & S_BIT) {
						ao->o |= 1 << 16;
					}
					ao->o |= num << 8;
					return mem_32bit_2reg (ao, m);
				}
				if (reg2 == 15) {
					if ((num > 4095) || (num < -4095)) {
						return -1;
					}
					ao->o = 0x3ff80000;
					if (m & S_BIT) {
						ao->o |= 1 << 16;
					}
					if (num < 0) {
						num = -num;
					} else {
						ao->o |= 1 << 31;
					}
					ao->o |= reg1 << 4;
					ao->o |= (num & 0xff) << 8;
					ao->o |= (num & 0x0f00) >> 8;
					return 4;
				}
				if ((num >= 0) && (num < 64) && (num % 2 == 0) && (!(m & S_BIT))) {
					ao->o = 0x0088;
					ao->o |= (num >> 3);
					ao->o |= ((num >> 1) & 0x3) << 14;
					if (mem_16bit_2reg (ao, m)) {
						return 2;
					}
				}
				if ((num > 4095) || (num < -255)) {
					return -1;
				}
				if (num >= 0) {
					ao->o = 0xb0f80000;
					if (m & S_BIT) {
						ao->o |= 1 << 16;
					}
					ao->o |= (num & 0xff) << 8;
					ao->o |= (num & 0xf00) >> 8;
					return mem_32bit_2reg (ao, m);
				}
				ao->o = 0x30f8000c;
				if (m & S_BIT) {
					ao->o |= 1 << 16;
				}
				ao->o |= -num << 8;
				return mem_32bit_2reg (ao, m);
			} else {
				return -1;
			}			
		        }
			break;
		case THUMB_REG_BRACKREGBRACK_CONST: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getregmemstartend (ao->a[1]);
			st32 num = getnum (ao->a[2]);
			if ((num < -255) || (num > 255)) {
				return -1;
			}
			if (ldrsel == 0) {
				ao->o = 0x50f80009;
			} else 
			if (ldrsel == B_BIT) {
				ao->o = 0x10f80009;
			} else 
			if (ldrsel == H_BIT) {
				ao->o = 0x30f80009;
			} else {
				return -1;
			}
			if (m & S_BIT) {
				ao->o |= 1 << 16;
			}
			if (num < 0) {
				num = -num;
			} else {
				ao->o |= 1 << 1;
			}
			ao->o |= num << 8;
			ao->o |= reg1 << 4;
			ao->o |= reg2 << 24;
			return 4;
		        }
			break;
		case THUMB_REG_BRACKREG_CONSTBRACKBANG: {
			st32 num = getnummemendbang (ao->a[2]);
			if ((num < -255) || (num > 255)) {
				return -1;
			}
			if (ldrsel == 0) {
				ao->o = 0x50f8000d;
			} else 
			if (ldrsel == B_BIT) {
				ao->o = 0x10f8000d;
			} else 
			if (ldrsel == H_BIT) {
				ao->o = 0x30f8000d;
			} else {
				return -1;
			}
			if (m & S_BIT) {
				ao->o |= 1 << 16;
			}
			if (num < 0) {
				num = -num;
			} else {
				ao->o |= 1 << 1;
			}
			ao->o |= num << 8;
			return mem_32bit_2reg (ao, m);
		        }
			break;
		case THUMB_REG_BRACKREG_REGBRACK: {
			ut8 reg3 = getregmemend (ao->a[2]);
			if (reg3 < 8) {
				if (ldrsel == 0) {
					ao->o = 0x0058;
				} else
				if (ldrsel == B_BIT) {
					if (m & S_BIT) {
						ao->o = 0x0056;
					} else {
						ao->o = 0x005c;
					}
				} else
				if (ldrsel == H_BIT) {
					if (m & S_BIT) {
						ao->o = 0x005e;
					} else {
						ao->o = 0x005a;
					}
				} else
				{
					return -1;
				}
				ao->o |= (reg3 & 0x3) << 14;
				ao->o |= (reg3 & 0x4) >> 2;
				if (mem_16bit_2reg (ao, m)) {
					return 2;
				}
			}
			ao->a[2][strlen (ao->a[2]) -1] = '\0';
			ao->a[3] = "lsl 0]";
		        }
			// intentional fallthrough
		case THUMB_REG_BRACKREG_REG_SHIFTBRACK: {
			ut8 reg3 = getreg (ao->a[2]);
			ut32 shift = getshiftmemend (ao->a[3]);

			shift >>= 2;
			if (shift & 0xffffcfff) {
				return -1;
			}

			if (ldrsel == 0) {
				ao->o = 0x50f80000;
			} else
			if (ldrsel == B_BIT) {
				ao->o = 0x10f80000;
			} else
			if (ldrsel == H_BIT) {
				ao->o = 0x30f80000;
			} else
			{
				return -1;
			}
			if (m & S_BIT) {
				ao->o |= 1 << 16;
			}
			ao->o |= reg3 << 8;
			ao->o |= shift;
			return mem_32bit_2reg (ao, m);
		        }
			break;
		case THUMB_REG_REG_BRACKREGBRACK: {
			ao->a[2][strlen (ao->a[2]) -1] = '\0';
			ao->a[3] = "0]";
		        }
			// intentional fallthrough
		case THUMB_REG_REG_BRACKREG_CONSTBRACK: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getregmemstart (ao->a[2]);
			st32 num = getnummemend (ao->a[3]);

			if ((num > 1020) || (num < -1020) || (num % 4 != 0) || (ldrsel != D_BIT)) {
				return -1;
			}
			ao->o = 0x50e90000;
			if (num < 0) {
				num = -num;
			} else {
				ao->o |= 1 << 31;
			}
			ao->o |= reg1 << 4;
			ao->o |= reg2;
			ao->o |= reg3 << 24;
			ao->o |= (num >> 2) << 8;
			return 4;
		        }
			break;
		case THUMB_REG_REG_BRACKREGBRACK_CONST: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getregmemstartend (ao->a[2]);
			st32 num = getnum (ao->a[3]);
			if ((num > 1020) || (num < -1020) || (num % 4 != 0) || (ldrsel != D_BIT)) {
				return -1;
			}
			ao->o = 0x70e80000;
			if (num < 0) {
				num = -num;
			} else {
				ao->o |= 1 << 31;
			}
			ao->o |= reg1 << 4;
			ao->o |= reg2;
			ao->o |= reg3 << 24;
			ao->o |= (num >> 2) << 8;
			return 4;
		        }
			break;
		case THUMB_REG_REG_BRACKREG_CONSTBRACKBANG: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getregmemstart (ao->a[2]);
			st32 num = getnummemendbang (ao->a[3]);
			if ((num > 1020) || (num < -1020) || (num % 4 != 0) || (ldrsel != D_BIT)) {
				return -1;
			}
			ao->o = 0x70e90000;
			if (num < 0) {
				num = -num;
			} else {
				ao->o |= 1 << 31;
			}
			ao->o |= reg1 << 4;
			ao->o |= reg2;
			ao->o |= reg3 << 24;
			ao->o |= (num >> 2) << 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "ldrex", B_BIT | H_BIT | D_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		ut32 ldrsel = m & (B_BIT | H_BIT | D_BIT);
		switch (argt) {
		case THUMB_REG_BRACKREGBRACK: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getregmemstartend (ao->a[1]);
			
			if (ldrsel == B_BIT) {
				ao->o = 0xd0e84f0f;
				ao->o |= reg1 << 4;
				ao->o |= reg2 << 24;
				return 4;
			} else
			if (ldrsel == H_BIT) {
				ao->o = 0xd0e85f0f;
				ao->o |= reg1 << 4;
				ao->o |= reg2 << 24;
				return 4;
			} else
			if (ldrsel == 0) {
				ao->a[1][strlen (ao->a[1]) - 1] = '\0';
				ao->a[2] = "0]";
			} else {
				return -1;
			}
			}
			// intentional fallthrough
		case THUMB_REG_BRACKREG_CONSTBRACK: {
			st32 num = getnummemend (ao->a[2]);
			if ((ldrsel != 0) || (num < 0) || (num > 1020) || (num % 4 != 0)) {
				return -1;
			}
			ao->o = 0x50e8000f;
			ao->o |= (num >> 2) << 8;
			return mem_32bit_2reg (ao, m);
		        }
			break;
		case THUMB_REG_REG_BRACKREGBRACK: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getregmemstartend (ao->a[2]);
			if (!(ldrsel & D_BIT)) {
				return -1;
			}
			ao->o = 0xd0e87f00;
			ao->o |= reg1 << 4;
			ao->o |= reg2;
			ao->o |= reg3 << 24;
			return 4;
		}
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "lsl", S_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG_CONST: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut32 num = getnum (ao->a[2]);
			if (num > 32) {
				return -1;
			}
			ao->o = 0x0000;			
			if (std_16bit_2reg (ao, m)) {
				ao->o |= (num & 0x03) << 14;
				ao->o |= num >> 2;
				return 2;
			}
			ao->o = 0x4fea0000;
			ao->o |= reg1;
			ao->o |= reg2 << 8;
			ao->o |= (num >> 2) << 4;
			ao->o |= (num & 0x3) << 14;
			if (m & S_BIT) {
				ao->o |= 1 << 28;
			}
			return 4;
		        }
			break;
		case THUMB_REG_REG: {
			ao->o = 0x8040;
			if (std_16bit_2reg (ao, m)) {
				return 2;
			}
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0x00fa00f0;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else		
	if ((m = opmask (ao->op, "lsr", S_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG_CONST: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut32 num = getnum (ao->a[2]);
			if (num > 32) {
				return -1;
			}
			ao->o = 0x0008;
			if (std_16bit_2reg (ao, m)) {
				ao->o |= (num & 0x03) << 14;
				ao->o |= num >> 2;
				return 2;
			}
			ao->o = 0x4fea1000;
			ao->o |= reg1;
			ao->o |= reg2 << 8;
			ao->o |= (num >> 2) << 4;
			ao->o |= (num & 0x3) << 14;
			if (m & S_BIT) {
				ao->o |= 1 << 28;
			}
			return 4;
		        }
			break;
		case THUMB_REG_REG: {
			ao->o = 0xc040;
			if (std_16bit_2reg (ao, m)) {
				return 2;
			}
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0x20fa00f0;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "mcr", R_BIT | TWO_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_COPROC_CONST_REG_COREG_COREG: {
			ao->a[5] = "0";
		        }
			// intentional fallthrough
		case THUMB_COPROC_CONST_REG_COREG_COREG_CONST: {
			ut32 coproc = getcoproc (ao->a[0]);
			ut32 opc1 = getnum (ao->a[1]);
			ut32 reg1 = getreg (ao->a[2]);
			ut32 coreg1 = getcoprocreg (ao->a[3]);
			ut32 coreg2 = getcoprocreg (ao->a[4]);
			ut32 opc2 = getnum (ao->a[5]);

			if ((coproc > 15) || (opc1 > 7) || (reg1 > 15) || (coreg1 > 15) || (coreg2 > 15) || (opc2 > 7) || (m & R_BIT)) {
				return -1;
			}

			ao->o = 0x00ee1000;
			if (m & TWO_BIT) {
				ao->o |= 1 << 20;
			}
			ao->o |= coproc;
			ao->o |= opc1 << 29;
			ao->o |= reg1 << 4;
			ao->o |= coreg1 << 24;
			ao->o |= coreg2 << 8;
			ao->o |= opc2 << 13;
			return 4;
		        }
			break;
		case THUMB_COPROC_CONST_REG_REG_COREG: {
			ut32 coproc = getcoproc (ao->a[0]);
			ut32 opc = getnum (ao->a[1]);
			ut32 reg1 = getreg (ao->a[2]);
			ut32 reg2 = getreg (ao->a[3]);
			ut32 coreg = getcoprocreg (ao->a[4]);

			if ((coproc > 15) || (opc > 15) || (reg1 > 15) || (reg2 > 15) || (coreg > 15) || (!(m & R_BIT))) {
				return -1;
			}

			ao->o = 0x40ec0000;
			if (m & TWO_BIT) {
				ao->o |= 1 << 20;
			}
			ao->o |= coproc;
			ao->o |= opc << 12;
			ao->o |= reg1 << 4;
			ao->o |= reg2 << 24;
			ao->o |= coreg << 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "mla", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG_REG_REG: {
			ut32 reg4 = getreg (ao->a[3]);
			if (reg4 > 15) {
				return -1;
			}
			ao->o = 0x00fb0000;
			ao->o |= reg4 << 4;

			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "mls", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG_REG_REG: {
			ut32 reg4 = getreg (ao->a[3]);
			if (reg4 > 15) {
				return -1;
			}
			ao->o = 0x00fb1000;
			ao->o |= reg4 << 4;

			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "mov", S_BIT | W_BIT | T_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST: {
			ut32 reg1 = getreg (ao->a[0]);
			err = false;
			ut32 num = getnum (ao->a[1]);

			if (reg1 > 15) {
				return -1;
			}
			
			if ((m & W_BIT) || (m & T_BIT)) {
				ut32 wnum = getnum (ao->a[1]);
				if (wnum > 65535) {
					return -1;
				}
				ao->o = 0x40f20000;
				if (m & T_BIT) {
					ao->o |= 1 << 31;
				}
				ao->o |= reg1;
				ao->o |= getthzeroimmed16 (wnum);
				return 4;
			}
			
			if (err) {
				return -1;
			}
			
			if ((num < 256) && (reg1 < 8) && (!(m & DOTW_BIT))) {
				ao->o = 0x0020;
				ao->o |= reg1;
				ao->o |= num << 8;
				return 2;
			}
				
			ao->o = 0x4ff00000;
			ao->o |= reg1;
			ao->o |= getthimmed12 (ao->a[1]);
			if (m & S_BIT) {
				ao->o |= 1 << 28;
			}
			return 4;
		        }
			break;
		case THUMB_REG_REG: {
			ut32 reg1 = getreg (ao->a[0]);
			ut32 reg2 = getreg (ao->a[1]);
			
			if ((reg1 > 15) || (reg2 > 15)) {
				return -1;
			}

			if ((!(m & S_BIT)) && (!(m & DOTW_BIT))) {
				ao->o = 0x0046;
				ao->o |= (reg1 & 0x7) << 8;
				ao->o |= (reg1 & 0x8) << 12;
				ao->o |= reg2 << 11;
				return 2;
			}
			
			if ((reg1 < 8) && (reg2 < 8) && (!(m & DOTW_BIT))) {
				ao->o = 0;
				ao->o |= reg1 << 8;
				ao->o |= reg2 << 11;
				return 2;
			}
			
			ao->o = 0x4fea0000;
			ao->o |= reg1;
			ao->o |= reg2 << 8;
			if (m & S_BIT) {
				ao->o |= 1 << 28;
			}
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else 
	if ((m = opmask (ao->op, "mrc", TWO_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_COPROC_CONST_REG_COREG_COREG: {
			ao->a[5] = "0";
		        }
			// intentional fallthrough
		case THUMB_COPROC_CONST_REG_COREG_COREG_CONST: {
			ut32 coproc = getcoproc (ao->a[0]);
			ut32 opc1 = getnum (ao->a[1]);
			ut32 reg1 = getreg (ao->a[2]);
			ut32 coreg1 = getcoprocreg (ao->a[3]);
			ut32 coreg2 = getcoprocreg (ao->a[4]);
			ut32 opc2 = getnum (ao->a[5]);

			if ((coproc > 15) || (opc1 > 7) || (reg1 > 15) || (coreg1 > 15) || (coreg2 > 15) || (opc2 > 7)) {
				return -1;
			}

			ao->o = 0x10ee1000;
			if (m & TWO_BIT) {
				ao->o |= 1 << 20;
			}
			ao->o |= coproc;
			ao->o |= opc1 << 29;
			ao->o |= reg1 << 4;
			ao->o |= coreg1 << 24;
			ao->o |= coreg2 << 8;
			ao->o |= opc2 << 13;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "mrrc", TWO_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_COPROC_CONST_REG_REG_COREG: {
			ut32 coproc = getcoproc (ao->a[0]);
			ut32 opc = getnum (ao->a[1]);
			ut32 reg1 = getreg (ao->a[2]);
			ut32 reg2 = getreg (ao->a[3]);
			ut32 coreg = getcoprocreg (ao->a[4]);

			if ((coproc > 15) || (opc > 15) || (reg1 > 15) || (reg2 > 15) || (coreg > 15)) {
				return -1;
			}

			ao->o = 0x50ec0000;
			if (m & TWO_BIT) {
				ao->o |= 1 << 20;
			}
			ao->o |= coproc;
			ao->o |= opc << 12;
			ao->o |= reg1 << 4;
			ao->o |= reg2 << 24;
			ao->o |= coreg << 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "mrs", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_OTHER: {
			ut32 reg1 = getreg (ao->a[0]);
			r_str_case (ao->a[1], false);

			if (reg1 > 15) {
				return -1;
			}

			if ((!strcmp(ao->a[1], "cpsr")) || (!strcmp(ao->a[1], "apsr"))) {
				ao->o = 0xeff30080;
				ao->o |= reg1;
				return 4;
			}

			if (!strcmp(ao->a[1], "spsr")) {
				ao->o = 0xfff30080;
				ao->o |= reg1;
				return 4;
			}
			
			return -1;
		        }
			break;
		default:
			return -1;
		}
	} else 
	if ((m = opmask (ao->op, "msr", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_OTHER_REG: {
			r_str_case (ao->a[0], false);
			ut8 spsr = 0;
			ut8 bank = interpret_msrbank (ao->a[0], &spsr);
			ut32 reg1 = getreg (ao->a[1]);

			if ((bank == 0) || (reg1 > 15)) {
				return -1;
			}
			
			ao->o = 0x80f30080;
			ao->o |= reg1 << 24;
			ao->o |= bank;
			if (spsr != 0) {
				ao->o |= 1 << 28;
			}
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "mul", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg3 = getreg (ao->a[2]);

			ao->o = 0x4043;
			if ((reg1 == reg3) && (std_16bit_2reg (ao, m))) {
				return 2;
			}

			ao->o = 0x00fb00f0;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "mvn", S_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST: {
			ut8 reg1 = getreg (ao->a[0]);
			err = false;
			ut32 num = getthimmed12 (ao->a[1]);
			
			if ((reg1 > 15) || err) {
				return -1;
			}

			ao->o = 0x6ff00000;
			ao->o |= reg1;
			ao->o |= num;
			if (m & S_BIT) {
				ao->o |= 1 << 28;
			}
			return 4;
		        }
			break;
		case THUMB_REG_REG: {
			ao->a[2] = "lsl 0";
		        }
			// intentional fallthrough
		case THUMB_REG_REG_SHIFT: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut32 shift = thumb_getshift (ao->a[2]);

			if ((reg1 > 15) || (reg2 > 15)) {
				return -1;
			}
			
			ao->o = 0xc043;
			if ((shift == 0) && (std_16bit_2reg (ao, m))) {
				return 2;
			}

			ao->o = 0x6fea0000;
			ao->o |= reg1;
			ao->o |= reg2 << 8;
			ao->o |= shift;
			if (m & S_BIT) {
				ao->o |= 1 << 28;
			}
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "nop", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_NONE: {
			if (m & DOTW_BIT) {
				ao->o = 0xaff30080;
				return 4;
			}
			ao->o = 0x00bf;
			return 2;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "orn", S_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_CONST: {
			err = false;
			ut32 num = getthimmed12 (ao->a[2]);

			if (err) {
				return -1;
			}

			ao->o = 0x60f00000;
			ao->o |= num;
			return (std_32bit_2reg (ao, m, false));
		        }
			break;
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->a[3] = "lsl 0";
		        }
			// intentional fallthrough
		case THUMB_REG_REG_SHIFT: {
			if (ao->a[3] == NULL) { // double fallthrough
				std_opt_3 (ao);
			}
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG_SHIFT: {
			ao->o = 0x60ea0000;
			return std_32bit_3reg (ao, m, true);
		        }
			break;
		default:
			return -1;
		}
	} else 
	if ((m = opmask (ao->op, "orr", S_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_CONST: {
			err = false;
			ut32 num = getthimmed12 (ao->a[2]);

			if (err) {
				return -1;
			}

			ao->o = 0x40f00000;
			ao->o |= num;
			return std_32bit_2reg (ao, m, false);
		        }
			break;
		case THUMB_REG_REG: {
			ao->o = 0x0043;
			if (std_16bit_2reg (ao, m)) {
				return 2;
			}
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->a[3] = "lsl 0";
		        }
			// intentional fallthrough
		case THUMB_REG_REG_SHIFT: {
			if (ao->a[3] == NULL) { // double fallthrough
				std_opt_3 (ao);
			}
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG_SHIFT: {
			ao->o = 0x40ea0000;
			return (std_32bit_3reg (ao, m, true));
		        }
			break;
		default:
			return -1;
		}
	} else 
	if ((m = opmask (ao->op, "pkh", BT_BIT | TB_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			if (m & TB_BIT) {
				ao->a[3] = "asr 0";
			} else
			if (m & BT_BIT) {
				ao->a[3] = "lsl 0";
			} else {
				return -1;
			}
		        }
			// intentional fallthrough
		case THUMB_REG_REG_SHIFT: {
			if (ao->a[3] == NULL) { // double fallthrough
				std_opt_3 (ao);
			}
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG_SHIFT: {
			ut32 shift = thumb_getshift (ao->a[3]);

			if (((m & TB_BIT) && ((shift & 0x00003000) != 0x00002000)) || ((m & BT_BIT) && ((shift & 0x00003000) != 0)) || ((m & (TB_BIT | BT_BIT)) == 0)) {	
				return -1;
			}

			ao->o = 0xc0ea0000;
			return (std_32bit_3reg (ao, m, true));
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "pld", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_BRACKREG_CONSTBRACK: {
			ut8 reg1 = getregmemstart (ao->a[0]);
			st32 num = getnummemend (ao->a[1]);

			if (reg1 == 15) {
				if ((num < -4095) || (num > 4095)) {
					return -1;
				}
				ao->o = 0x1ff800f0;
				if (num > 0) {
					ao->o |= 1 << 31;
				} else {
					num = -num;
				}
				ao->o |= (num & 0x0ff) << 8;
				ao->o |= (num & 0xf00) >> 8;
				return 4;
			}

			if ((reg1 > 15) || (num < -255) || (num > 4095)) {
				return -1;
			}

			if (num > 0) {
				ao->o = 0x90f800f0;
				ao->o |= (num & 0x0ff) << 8;
				ao->o |= (num & 0xf00) >> 8;
				ao->o |= reg1 << 24;
				return 4;
			}
			num = -num;
			ao->o = 0x10f800fc;
			ao->o |= num << 8;
			ao->o |= reg1 << 24;
			return 4;
		        }
			break;
		case THUMB_BRACKREG_REGBRACK: {
			ao->a[1][strlen (ao->a[1]) - 1] = '\0';
			ao->a[2] = "lsl 0]";
		        }
			// intentional fallthrough
		case THUMB_BRACKREG_REG_SHIFTBRACK: {
			ut8 reg1 = getregmemstart (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut32 shift = getshiftmemend (ao->a[2]) >> 2;

			if ((reg1 > 15) || (reg2 > 15) || ((shift & 0xffffcfff) != 0)) {
				return -1;
			}

			ao->o = 0x10f800f0;
			ao->o |= reg1 << 24;
			ao->o |= reg2 << 8;
			ao->o |= shift;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "pli", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_BRACKREG_CONSTBRACK: {
			ut8 reg1 = getregmemstart (ao->a[0]);
			st32 num = getnummemend (ao->a[1]);

			if (reg1 == 15) {
				if ((num < -4095) || (num > 4095)) {
					return -1;
				}
				ao->o = 0x1ff900f0;
				if (num > 0) {
					ao->o |= 1 << 31;
				} else {
					num = -num;
				}
				ao->o |= (num & 0x0ff) << 8;
				ao->o |= (num & 0xf00) >> 8;
				return 4;
			}

			if ((reg1 > 15) || (num < -255) || (num > 4095)) {
				return -1;
			}

			if (num > 0) {
				ao->o = 0x90f900f0;
				ao->o |= (num & 0x0ff) << 8;
				ao->o |= (num & 0xf00) >> 8;
				ao->o |= reg1 << 24;
				return 4;
			}
			num = -num;
			ao->o = 0x10f900fc;
			ao->o |= num << 8;
			ao->o |= reg1 << 24;
			return 4;
		        }
			break;
		case THUMB_BRACKREG_REGBRACK: {
			ao->a[1][strlen (ao->a[1]) -1] = '\0';
			ao->a[2] = "lsl 0]";
		        }
			// intentional fallthrough
		case THUMB_BRACKREG_REG_SHIFTBRACK: {
			ut8 reg1 = getregmemstart (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut32 shift = getshiftmemend (ao->a[2]) >> 2;

			if ((reg1 > 15) || (reg2 > 15) || ((shift & 0xffffcfff) != 0)) {
				return -1;
			}

			ao->o = 0x10f900f0;
			ao->o |= reg1 << 24;
			ao->o |= reg2 << 8;
			ao->o |= shift;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "pop", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_LIST: {
			st32 list = getreglist (ao->a[0]);
			if ((list <= 0) || ((list & (1 << 13)) != 0)) {
				return -1;
			}
			if ((!(m & DOTW_BIT)) && ((list & 0x00007f00) == 0)) {
				ao->o = 0x00bc;
				ao->o |= (list & 0x8000) >> 15;
				ao->o |= (list & 0xff) << 8;
				return 2;
			}
			ao->o = 0xbde80000;
			ao->o |= (list & 0xff00) >> 8;
			ao->o |= (list & 0xff) << 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "push", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_LIST: {
			st32 list = getreglist (ao->a[0]);
			if ((list <= 0) || ((list & 0x0000a000) != 0)) {
				return -1;
			}
			if ((!(m & DOTW_BIT)) && ((list & 0x00001f00) == 0)) {
				ao->o = 0x00b4;
				ao->o |= (list & 0x4000) >> 14;
				ao->o |= (list & 0xff) << 8;
				return 2;
			}
			ao->o = 0x2de90000;
			ao->o |= (list & 0xff00) >> 8;
			ao->o |= (list & 0xff) << 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "qadd", EIGHT_BIT | SIXTEEN_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			if (m & SIXTEEN_BIT) {
				ao->o = 0x90fa10f0;
			} else
			if (m & EIGHT_BIT) {
				ao->o = 0x80fa10f0;
			} else {
				ao->o = 0x80fa80f0;
			}

			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "qasx", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0xa0fa10f0;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "qdadd", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0x80fa90f0;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "qdsub", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0x80fab0f0;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "qsax", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0xe0fa10f0;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "qsub", EIGHT_BIT | SIXTEEN_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			if (m & SIXTEEN_BIT) {
				ao->o = 0xd0fa10f0;
			} else
			if (m & EIGHT_BIT) {
				ao->o = 0xc0fa10f0;
			} else {
				ao->o = 0x80faa0f0;
			}
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "rbit", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			ao->a[2] = ao->a[1];
			ao->o = 0x90faa0f0;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "rev", SIXTEEN_BIT | SH_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			if (m & SIXTEEN_BIT) {
				ao->o = 0x40ba;
			} else
			if (m & SH_BIT) {
				ao->o = 0xc0ba;
			} else {
				ao->o = 0x00ba;
			}

			if (std_16bit_2reg (ao, m)) {
				return 2;
			}
			
			if (m & SIXTEEN_BIT) {
				ao->o = 0x90fa90f0;
			} else
			if (m & SH_BIT) {
				ao->o = 0x90fab0f0;
			} else {
				ao->o = 0x90fa80f0;
			}
			ao->a[2] = ao->a[1];
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "rfe", IA_BIT | FD_BIT | DB_BIT | EA_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		ut32 wb = 0;
		switch (argt) {
		case THUMB_REGBANG: {
			ao->a[0][strlen (ao->a[0]) - 1] = '\0';
			wb = 0x20000000;
		        }
			// intentional fallthrough
		case THUMB_REG: {
			ut8 reg1 = getreg (ao->a[0]);

			if (reg1 > 15) {
				return -1;
			}
			
			if ((m & DB_BIT) || (m & EA_BIT)) {
				ao->o = 0x10e800c0;
			} else {
				ao->o = 0x90e900c0;
			}

			ao->o |= reg1 << 24;
			ao->o |= wb;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "ror", S_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG_CONST: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut32 num = getnum (ao->a[2]);

			if ((reg1 > 15) || (reg2 > 15) || (num > 31) || (num < 1)) {
				return -1;
			}

			ao->o = 0x4fea3000;
			ao->o |= reg1;
			ao->o |= reg2 << 8;
			ao->o |= (num & 0x3) << 14;
			ao->o |= (num & 0x1c) << 2;
			if (m & S_BIT) {
				ao->o |= 1 << 28;
			}
			return 4;
		        }
			break;
		case THUMB_REG_REG: {
			ao->o = 0xc041;
			if (std_16bit_2reg (ao, m)) {
				return 2;
			}
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0x60fa00f0;
			return (std_32bit_3reg (ao, m, false));
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "rrx", S_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			
			if ((reg1 > 15) || (reg2 > 15)) {
				return -1;
			}
			
			ao->o = 0x4fea3000;
			ao->o |= reg1;
			ao->o |= reg2 << 8;
			if (m & S_BIT) {
				ao->o |= 1 << 28;
			}
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "rsb", S_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_CONST: {
			err = false;
			ut32 num = getthimmed12 (ao->a[2]);

			if (err) {
				return -1;
			}

			ao->o = 0x4042;
			if ((num == 0) && std_16bit_2reg (ao, m)) {
				return 2;
			}

			ao->o = 0xc0f10000;
			ao->o |= num;
			return (std_32bit_2reg (ao, m, false));
		        }
			break;
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->a[3] = "lsl 0";
		        }
			// intentional fallthrough
		case THUMB_REG_REG_SHIFT: {
			if (ao->a[3] == NULL) { // double fallthrough
				std_opt_3 (ao);
			}
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG_SHIFT: {
			ao->o = 0xc0eb0000;
			return (std_32bit_3reg (ao, m, true));
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "sadd", EIGHT_BIT | SIXTEEN_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			if (m & SIXTEEN_BIT) {
				ao->o = 0x90fa00f0;
			} else
			if (m & EIGHT_BIT) {
				ao->o = 0x80fa00f0;
			} else {
				return -1;
			}
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "sasx", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0xa0fa00f0;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "sbc", S_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			ao->o = 0x8041;
			if (std_16bit_2reg (ao, m)) {
				return 2;
			}
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->a[3] = "lsl 0";
		        }
			// intentional fallthrough
		case THUMB_REG_REG_SHIFT: {
			if (ao->a[3] == NULL) { // double fallthrough
				std_opt_3 (ao);
			}
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG_SHIFT: {
			ao->o = 0x60eb0000;
			return std_32bit_3reg (ao, m, true);
		        }
			break;
		case THUMB_REG_CONST: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_CONST: {
			ao->o = 0x60f10000;
			err = false;
			ut32 num = getthimmed12 (ao->a[2]);

			if (err) {
				return -1;
			}
			ao->o |= num;
			
			return std_32bit_2reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if (( m = opmask (ao->op, "sbfx", 0) )) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG_CONST_CONST: {
			ut32 lsb = getnum (ao->a[2]);
			ut32 width = getnum (ao->a[3]);
			ut32 msb = lsb + width - 1;
			if ((lsb > 31) || (msb > 31)) {
				return -1;
			}
			ao->o = 0x40f30000;
			ao->o |= ((lsb & 0x1c) << 2);
			ao->o |= ((lsb & 0x3) << 14);
			ao->o |= ((width - 1) << 8);
			return std_32bit_2reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "sdiv", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0x90fbf0f0;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "sel", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0xa0fa80f0;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "setend", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_OTHER: {
			r_str_case (ao->a[0], false);
			ao->o = 0x50b6;
			if (!(strcmpnull (ao->a[0], "be"))) {
				ao->o |= 1 << 11;
				return 2;
			} else
			if (!(strcmpnull (ao->a[0], "le"))) {
				return 2;
			} else {
				return -1;
			}
			break;
		        }
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "sev", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_NONE:
			if (m & DOTW_BIT) {
				ao->o = 0xaff30480;
				return 4;
			} else {
				ao->o = 0x40bf;
				return 2;
			}
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "shadd", EIGHT_BIT | SIXTEEN_BIT ))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			if (m & SIXTEEN_BIT) {
				ao->o = 0x90fa20f0;
			} else
			if (m & EIGHT_BIT) {
				ao->o = 0x80fa20f0;
			} else {
				return -1;
			}
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "shasx", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0xa0fa20f0;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "shsax", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0xe0fa20f0;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "shsub", EIGHT_BIT | SIXTEEN_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			if (m & SIXTEEN_BIT) {
				ao->o = 0xd0fa20f0;
			} else
			if (m & EIGHT_BIT) {
				ao->o = 0xc0fa20f0;
			} else {
				return -1;
			}
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "smc", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_CONST: {
			err = false;
			ut32 num = getnum (ao->a[0]);
			
			if (err || (num > 15)) {
				return -1;
			}

			ao->o = 0xf0f70080;
			ao->o |= num << 24;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "smla", BB_BIT | BT_BIT | TB_BIT | TT_BIT | WB_BIT | WT_BIT | L_BIT | D_BIT | X_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG_REG_REG: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getreg (ao->a[2]);
			ut8 reg4 = getreg (ao->a[3]);
			
			if ((reg1 > 15) || (reg2 > 15) || (reg3 > 15) || (reg4 > 15) || (m & DOTN_BIT)) {
				return -1;
			}
			if (m & L_BIT) {
				if (m & BB_BIT) {
					ao->o = 0xc0fb8000;
				} else
				if (m & BT_BIT) {
					ao->o = 0xc0fb9000;
				} else
				if (m & TB_BIT) {
					ao->o = 0xc0fba000;
				} else
				if (m & TT_BIT) {
					ao->o = 0xc0fbb000;
				} else
				if (m & D_BIT) {
					ao->o = 0xc0fbc000;
					if (m & X_BIT) {
						ao->o |= 1 << 12;
					}
				} else {
					ao->o = 0xc0fb0000;
				}
				ao->o |= reg1 << 4;
				ao->o |= reg2;
				ao->o |= reg3 << 24;
				ao->o |= reg4 << 8;
				return 4;
			}
			if (m & BB_BIT) {
				ao->o = 0x10fb0000;
				ao->o |= reg4 << 4;
				return std_32bit_3reg (ao, m, false);
			}
			if (m & BT_BIT) {
				ao->o = 0x10fb1000;
				ao->o |= reg4 << 4;
				return std_32bit_3reg (ao, m, false);
			}
			if (m & TB_BIT) {
				ao->o = 0x10fb2000;
				ao->o |= reg4 << 4;
				return std_32bit_3reg (ao, m, false);
			}
			if (m & TT_BIT) {
				ao->o = 0x10fb3000;
				ao->o |= reg4 << 4;
				return std_32bit_3reg (ao, m, false);
			}
			if (m & D_BIT) {
				ao->o = 0x20fb0000;
				if (m & X_BIT) {
					ao->o |= 1 << 12;
				}
				ao->o |= reg4 << 4;
				return std_32bit_3reg (ao, m, false);
			}
			if (m & WB_BIT) {
				ao->o = 0x30fb0000;
				ao->o |= reg4 << 4;
				return std_32bit_3reg (ao, m, false);
			}
			if (m & WT_BIT) {
				ao->o = 0x30fb1000;
				ao->o |= reg4 << 4;
				return std_32bit_3reg (ao, m, false);
			}
			return -1;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "smlsd", X_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG_REG_REG: {
			ut8 reg4 = getreg (ao->a[3]);

			if (reg4 > 15) {
				return -1;
			}
			ao->o = 0x40fb0000;
			if (m & X_BIT) {
				ao->o |= 1 << 12;
			}
			ao->o |= reg4 << 4;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "smlsld", X_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG_REG_REG: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getreg (ao->a[2]);
			ut8 reg4 = getreg (ao->a[3]);

			if ((reg1 > 15) || (reg2 > 15) || (reg3 > 15) || (reg4 > 15) || (m & DOTN_BIT)) {
				return -1;
			}
			ao->o = 0xd0fbc000;

			if (m & X_BIT) {
				ao->o |= 1 << 12;
			}

			ao->o |= reg1 << 4;
			ao->o |= reg2;
			ao->o |= reg3 << 24;
			ao->o |= reg4 << 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "smmla", R_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG_REG_REG: {
			ut8 reg4 = getreg (ao->a[3]);

			if (reg4 > 15) {
				return -1;
			}
			ao->o = 0x50fb0000;
			if (m & R_BIT) {
				ao->o |= 1 << 12;
			}
			ao->o |= reg4 << 4;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "smmls", R_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG_REG_REG: {
			ut8 reg4 = getreg (ao->a[3]);

			if (reg4 > 15) {
				return -1;
			}
			ao->o = 0x60fb0000;
			if (m & R_BIT) {
				ao->o |= 1 << 12;
			}
			ao->o |= reg4 << 4;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "smmul", R_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0x50fb00f0;
			if (m & R_BIT) {
				ao->o |= 1 << 12;
			}
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "smuad", X_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0x20fb00f0;
			if (m & X_BIT) {
				ao->o |= 1 << 12;
			}
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "smul", BB_BIT | BT_BIT | TB_BIT | TT_BIT | WB_BIT | WT_BIT | L_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			if (m & BB_BIT) {
				ao->o = 0x10fb00f0;
			} else
			if (m & BT_BIT) {
				ao->o = 0x10fb10f0;
			} else
			if (m & TB_BIT) {
				ao->o = 0x10fb20f0;
			} else
			if (m & TT_BIT) {
				ao->o = 0x10fb30f0;
			} else
			if (m & WB_BIT) {
				ao->o = 0x30fb00f0;
			} else
			if (m & WT_BIT) {
				ao->o = 0x30fb10f0;
			} else {
				return -1;
			}
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		case THUMB_REG_REG_REG_REG: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getreg (ao->a[2]);
			ut8 reg4 = getreg (ao->a[3]);

			if ((reg1 > 15) || (reg2 > 15) || (reg3 > 15) || (reg4 > 15) || (m & DOTN_BIT) || (!(m & L_BIT))) {
				return -1;
			}

			ao->o = 0x80fb0000;
			ao->o |= reg1 << 4;
			ao->o |= reg2;
			ao->o |= reg3 << 24;
			ao->o |= reg4 << 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "smusd", X_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0x40fb00f0;
			if (m & X_BIT) {
				ao->o |= 1 << 12;
			}
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "srs", DB_BIT | FD_BIT | IA_BIT | EA_BIT))) {
		ut32 w = 0;
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_CONSTBANG: {
			ao->a[0][strlen (ao->a[0]) - 1] = '\0';
			w = 1;
		        }
			// intentional fallthrough
		case THUMB_CONST: {
			ut32 num = getnum (ao->a[0]);
			if (num > 31) {
				return -1;
			}
			if ((m & DB_BIT) || (m & FD_BIT)) {
				ao->o = 0x0de800c0;
			} else {
				ao->o = 0x8de900c0;
			}				
			ao->o |= num << 8;
			ao->o |= w << 29;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "ssat", SIXTEEN_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST_REG: {
			ao->a[3] = "lsl 0";
		        }
			// intentional fallthrough
		case THUMB_REG_CONST_REG_SHIFT: {
			ut8 reg1 = getreg (ao->a[0]);
			ut32 num = getnum (ao->a[1]) - 1;
			ut8 reg2 = getreg (ao->a[2]);
			ut32 shift = thumb_getshift (ao->a[3]);

			if (err || (reg1 > 15) || (reg2 > 15) || (num > 31) || (shift & 0x00001000) || ((m & SIXTEEN_BIT) && shift)) {
				return -1;
			}

			if (shift & 0x00002000) {
				shift |= 0x20000000;
				shift &= 0xffffdfff;
			}

			if (m & SIXTEEN_BIT) {
				ao->o = 0x20f30000;
			} else {
				ao->o = 0x00f30000;
			}

			ao->o |= reg1;
			ao->o |= reg2 << 24;
			ao->o |= num << 8;
			ao->o |= shift;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "ssax", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0xe0fa00f0;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "ssub", EIGHT_BIT | SIXTEEN_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			if (m & EIGHT_BIT) {
				ao->o = 0xc0fa00f0;
			} else
			if (m & SIXTEEN_BIT) {
				ao->o = 0xd0fa00f0;
			} else {
				return -1;
			}
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else if ((m = opmask (ao->op, "stc", L_BIT | TWO_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_COPROC_COREG_BRACKREGBRACK: {
			ao->a[2][strlen (ao->a[2]) - 1] = '\0';
			ao->a[3] = "0]";
		        }
			// intentional fallthrough
		case THUMB_COPROC_COREG_BRACKREG_CONSTBRACK: {
			ut8 coproc = getcoproc (ao->a[0]);
			ut8 coreg = getcoprocreg (ao->a[1]);
			ut8 reg = getregmemstart (ao->a[2]);
			st32 num = getnummemend (ao->a[3]);

			if ((coproc > 15) || (coreg > 15) || (reg > 15) || (num > 4092) || (num < -4092) || (num % 4 != 0)) {
				return -1;
			}
		
			ao->o = 0x00ed0000;
			if (m & L_BIT) {
				ao->o |= 1 << 30;
			}
			if (m & TWO_BIT) {
				ao->o |= 1 << 20;
			}
			if (num < 0) {
				num = -num;
			} else {
				ao->o |= 1 << 31;
			}				
			ao->o |= coproc;
			ao->o |= coreg << 4;
			ao->o |= reg << 24;
			ao->o |= (num >> 2) << 8;
			return 4;
		        }
			break;
		case THUMB_COPROC_COREG_BRACKREGBRACK_CONST: {
			ut8 coproc = getcoproc (ao->a[0]);
			ut8 coreg = getcoprocreg (ao->a[1]);
			ut8 reg = getregmemstartend (ao->a[2]);
			st32 num = getnum (ao->a[3]);

			if ((coproc > 15) || (coreg > 15) || (reg > 15) || (num > 4092) || (num < -4092) || (num % 4 != 0)) {
				return -1;
			}
		
			ao->o = 0x20ec0000;
			if (m & L_BIT) {
				ao->o |= 1 << 30;
			}
			if (m & TWO_BIT) {
				ao->o |= 1 << 20;
			}
			if (num < 0) {
				num = -num;
			} else {
				ao->o |= 1 << 31;
			}
			ao->o |= coproc;
			ao->o |= coreg << 4;
			ao->o |= reg << 24;
			ao->o |= (num >> 2) << 8;
			return 4;
		        }
			break;
		case THUMB_COPROC_COREG_BRACKREG_CONSTBRACKBANG: {
			ut8 coproc = getcoproc (ao->a[0]);
			ut8 coreg = getcoprocreg (ao->a[1]);
			ut8 reg = getregmemstart (ao->a[2]);
			st32 num = getnummemendbang (ao->a[3]);

			if ((coproc > 15) || (coreg > 15) || (reg > 15) || (num > 4092) || (num < -4092) || (num % 4 != 0)) {
				return -1;
			}
		
			ao->o = 0x20ed0000;
			if (m & L_BIT) {
				ao->o |= 1 << 30;
			}
			if (m & TWO_BIT) {
				ao->o |= 1 << 20;
			}
			if (num < 0) {
				num = -num;
			} else {
				ao->o |= 1 << 31;
			}
			ao->o |= coproc;
			ao->o |= coreg << 4;
			ao->o |= reg << 24;
			ao->o |= (num >> 2) << 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else if ((m = opmask (ao->op, "stm", FD_BIT | DB_BIT | IA_BIT | EA_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		bool wb = false;
		switch (argt) {
		case THUMB_REGBANG_LIST: {
			wb = true;
			ao->a[0][strlen (ao->a[0]) - 1] = '\0';
		        }
			// intentional fallthrough
		case THUMB_REG_LIST: {
			ut8 reg = getreg (ao->a[0]);
			st32 list = getreglist (ao->a[1]);
			if ((list <= 0) || ((list & 0x0000a000) != 0)) {
				return -1;
			}

			if ((!(m & DOTW_BIT)) && ((list & 0x0000ff00) == 0) && (!(m & (FD_BIT | DB_BIT))) && wb) {
				ao->o = 0x00c0;
				ao->o |= (list & 0x000000ff) << 8;
				ao->o |= reg;
				return 2;
			}

			if ((m & (FD_BIT | DB_BIT | IA_BIT | EA_BIT)) == 0) {
				return -1;
			}
			
			if (m & (FD_BIT | DB_BIT)) {
				ao->o = 0x00e90000;
			} else {
				ao->o = 0x80e80000;
			}

			if (wb) {
				ao->o |= 1 << 29;
			}

			ao->o |= reg << 24;
			ao->o |= (list & 0x000000ff) << 8;
			ao->o |= (list & 0x0000ff00) >> 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else if ((m = opmask (ao->op, "str", B_BIT | T_BIT | D_BIT | H_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		ut32 strsel = m & (B_BIT | H_BIT | D_BIT);
		switch (argt) {
		case THUMB_REG_BRACKREGBRACK:
			if (ao->a[2] == NULL) { // double fallthrough
				ao->a[1][strlen (ao->a[1]) -1] = '\0';
				ao->a[2] = "0]";
			}
			// intentional fallthrough
		case THUMB_REG_BRACKREG_CONSTBRACK: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getregmemstart (ao->a[1]);
			st32 num = getnummemend (ao->a[2]);
			if (m & T_BIT) {
				if ((num < 0) || (num > 255)) {
					return -1;
				}
				if (strsel == 0) {
					ao->o = 0x40f8000e;
				} else
			        if (strsel == H_BIT) {
					ao->o = 0x20f8000e;
				} else
			        if (strsel == B_BIT) {
					ao->o = 0x00f8000e;
				} else {
					return -1;
				}
				ao->o |= num << 8;
				return mem_32bit_2reg (ao, m);
			}
			
			if ((strsel == 0) && (reg2 == 13) && (num >= 0) && (num < 1024) && ((num % 4) == 0) && (reg1 < 8) & (!(m & DOTW_BIT))) {
				ao->o = 0x0090;
				ao->o |= reg1;
				ao->o |= (num >> 2) << 8;
				return 2;
			}

			bool t1form = false;
			if ((strsel == 0) && (num < 128) && (num >= 0) && (num % 4 == 0)) {
				ao->o = 0x0060;
				ao->o |= (num >> 4);
				ao->o |= ((num >> 2) & 0x3) << 14;
				t1form = true;
			}
			if ((strsel == B_BIT) && (num < 32) && (num >= 0)) {
				ao->o = 0x0070;
				ao->o |= (num >> 2);
				ao->o |= (num & 0x3) << 14;
				t1form = true;
			}
			if ((strsel == H_BIT) && (num < 64) && (num >= 0) && (num % 2 == 0)) {
				ao->o = 0x0080;
				ao->o |= (num >> 3);
				ao->o |= ((num >> 1) & 0x3) << 14;
				t1form = true;
			}
			if (t1form) {
				if (mem_16bit_2reg (ao, m)) {
					return 2;
				}
			}
			
			if ((num > 4095) || (num < -255)) {
				return -1;
			}
			if ((num >= 0) && (num < 4096)) {
				if (strsel == 0) {
					ao->o = 0xc0f80000;
				} else
				if (strsel == B_BIT) {
					ao->o = 0x80f80000;
				} else
				if (strsel == H_BIT) {
					ao->o = 0xa0f80000;
				} else {
					return -1;
				}
				ao->o |= (num >> 8);
				ao->o |= (num & 0x000000ff) << 8;
				return mem_32bit_2reg (ao, m);
			}
			if (strsel == 0) {
				ao->o = 0x40f8000c;
			} else
			if (strsel == B_BIT) {
				ao->o = 0x00f8000c;
			} else
			if (strsel == H_BIT) {
				ao->o = 0x20f8000c;
			} else {
				return -1;
			}
			ao->o |= -num << 8;
			return mem_32bit_2reg (ao, m);
		        }
			break;
		case THUMB_REG_BRACKREGBRACK_CONST: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getregmemstartend (ao->a[1]);
			st32 num = getnum (ao->a[2]);

			if ((num > 255) || (num < -255)) {
				return -1;
			}

			if (strsel == 0) {
				ao->o = 0x40f80009;
			} else 
			if (strsel == B_BIT) {
				ao->o = 0x00f80009;
			} else 
			if (strsel == H_BIT) {
				ao->o = 0x20f80009;
			} else {
				return -1;
			}

			if (num < 0) {
				num = -num;
			} else {
				ao->o |= 1 << 1;
			}
			ao->o |= num << 8;
			ao->o |= reg1 << 4;
			ao->o |= reg2 << 24;
			return 4;
		        }
			break;
		case THUMB_REG_BRACKREG_CONSTBRACKBANG: {
			st32 num = getnummemendbang (ao->a[2]);

			if ((num > 255) || (num < -255)) {
				return -1;
			}

			if (strsel == 0) {
				ao->o = 0x40f8000d;
			} else 
			if (strsel == B_BIT) {
				ao->o = 0x00f8000d;
			} else 
			if (strsel == H_BIT) {
				ao->o = 0x20f8000d;
			} else {
				return -1;
			}

			if (num < 0) {
				num = -num;
			} else {
				ao->o |= 1 << 1;
			}
			ao->o |= num << 8;
			return mem_32bit_2reg (ao, m);
		        }
			break;
		case THUMB_REG_BRACKREG_REGBRACK: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getregmemstart (ao->a[1]);
			ut8 reg3 = getregmemend (ao->a[2]);
			if ((reg1 < 8) && (reg2 < 8) && (reg3 < 8) && (!(m & DOTW_BIT))) {
				if (strsel == 0) {
					ao->o = 0x0050;
				} else
				if (strsel == B_BIT) {
					ao->o = 0x0054;
				} else
				if (strsel == H_BIT) {
					ao->o = 0x0052;
				} else {
					return -1;
				}
				ao->o |= reg1 << 8;
				ao->o |= reg2 << 11;
				ao->o |= (reg3 & 0x3) << 14;
				ao->o |= (reg3 >> 2);
				return 2;
			}
			ao->a[2][strlen (ao->a[2]) - 1] = '\0';
			ao->a[3] = "lsl 0]";
		        }
			// intentional fallthrough
		case THUMB_REG_BRACKREG_REG_SHIFTBRACK: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getregmemstart (ao->a[1]);
			ut8 reg3 = getreg (ao->a[2]);
			ut32 shift = getshiftmemend (ao->a[3]) >> 2;
			
			if (((shift & 0xffffcfff) != 0) || (reg1 > 15) || (reg2 > 15) || (reg3 > 15)) {
				return -1;
			}

			if (strsel == 0) {
				ao->o = 0x40f80000;
			} else 
			if (strsel == B_BIT) {
				ao->o = 0x00f80000;
			} else 
			if (strsel == H_BIT) {
				ao->o = 0x20f80000;
			} else {
				return -1;
			}
			
			ao->o |= reg1 << 4;
			ao->o |= reg2 << 24;
			ao->o |= reg3 << 8;
			ao->o |= shift;
			return 4;
		        }
			break;
		case THUMB_REG_REG_BRACKREGBRACK: {
			ao->a[2][strlen (ao->a[2]) - 1] = '\0';
			ao->a[3] = "0]";
		        }
			// intentional fallthrough
		case THUMB_REG_REG_BRACKREG_CONSTBRACK: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getregmemstart (ao->a[2]);
			st32 num = getnummemend (ao->a[3]);

			if ((reg1 > 15) || (reg2 > 15) || (reg3 > 15) || (strsel != D_BIT) || (num > 1023) || (num < -1023) || ((num % 4) != 0)) {
				return -1;
			}

			ao->o = 0x40e90000;

			if (num < 0) {
				num = -num;
			} else {
				ao->o |= 1 << 31;
			}
			ao->o |= reg1 << 4;
			ao->o |= reg2;
			ao->o |= reg3 << 24;
			ao->o |= (num >> 2) << 8;
			return 4;
		        }
			break;
		case THUMB_REG_REG_BRACKREG_CONSTBRACKBANG: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getregmemstart (ao->a[2]);
			st32 num = getnummemendbang (ao->a[3]);

			if ((reg1 > 15) || (reg2 > 15) || (reg3 > 15) || (strsel != D_BIT) || (num > 1023) || (num < -1023) || ((num % 4) != 0)) {
				return -1;
			}

			ao->o = 0x60e90000;

			if (num < 0) {
				num = -num;
			} else {
				ao->o |= 1 << 31;
			}
			ao->o |= reg1 << 4;
			ao->o |= reg2;
			ao->o |= reg3 << 24;
			ao->o |= (num >> 2) << 8;
			return 4;
		        }
			break;
		case THUMB_REG_REG_BRACKREGBRACK_CONST: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getregmemstartend (ao->a[2]);
			st32 num = getnum (ao->a[3]);

			if ((reg1 > 15) || (reg2 > 15) || (reg3 > 15) || (strsel != D_BIT) || (num > 1023) || (num < -1023) || ((num % 4) != 0)) {
				return -1;
			}

			ao->o = 0x60e80000;

			if (num < 0) {
				num = -num;
			} else {
				ao->o |= 1 << 31;
			}
			ao->o |= reg1 << 4;
			ao->o |= reg2;
			ao->o |= reg3 << 24;
			ao->o |= (num >> 2) << 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "strex", B_BIT | D_BIT | H_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		ut32 strsel = m & (B_BIT | H_BIT | D_BIT);
		switch (argt) {
		case THUMB_REG_REG_BRACKREGBRACK: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getregmemstartend (ao->a[2]);
			
			if ((strsel == D_BIT) || (reg1 > 15) || (reg2 > 15) || (reg3 > 15)) {
				return -1;
			}
			if (strsel == B_BIT) {
				ao->o = 0xc0e8400f;
				ao->o |= reg1 << 8;
				ao->o |= reg2 << 4;
				ao->o |= reg3 << 24;
				return 4;
			} else
			if (strsel == H_BIT) {
				ao->o = 0xc0e8500f;
				ao->o |= reg1 << 8;
				ao->o |= reg2 << 4;
				ao->o |= reg3 << 24;
				return 4;
			}

			ao->a[2][strlen (ao->a[2]) - 1] = '\0';
			ao->a[3] = "0]";
		        }
			// intentional fallthrough
		case THUMB_REG_REG_BRACKREG_CONSTBRACK: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getregmemstart (ao->a[2]);
			st32 num = getnummemend (ao->a[3]);

			if ((strsel != 0) || (reg1 > 15) || (reg2 > 15) || (reg3 > 15) || (num < 0) || (num > 1023) || ((num % 4) !=0)) {
				return -1;
			}

			ao->o = 0x40e80000;
			ao->o |= reg1;
			ao->o |= reg2 << 4;
			ao->o |= reg3 << 24;
			ao->o |= (num >> 2) << 8;
			return 4;
		        }
			break;
		case THUMB_REG_REG_REG_BRACKREGBRACK: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getreg (ao->a[2]);
			ut8 reg4 = getregmemstartend (ao->a[3]);

			if ((strsel != D_BIT) || (reg1 > 15) || (reg2 > 15) || (reg3 > 15) || (reg4 > 15)) {
				return -1;
			}

			ao->o = 0xc0e87000;
			ao->o |= reg1 << 8;
			ao->o |= reg2 << 4;
			ao->o |= reg3;
			ao->o |= reg4 << 24;
			return 4;
		        }
			break;
		}
	} else 
	if ((m = opmask (ao->op, "sub", S_BIT | W_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_CONST: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut32 num = getnum (ao->a[2]);

			if ((reg1 > 15) || (reg2 > 15)) {
				return -1;
			}

			if ((reg1 == 15) && (reg2 == 14) && (num < 256)) {
				ao->o = 0xdef3008f;
				ao->o |= num << 8;
				return 4;
			}
			if (reg2 == 13) {
				if ((reg1 == 13) && (!(m & DOTW_BIT)) && (!(m & W_BIT)) && (num <= 4096) && (num % 4 == 0)) {
					ao->o = 0x80b0;
					ao->o |= (num >> 2) << 8;
					return 2;
				}
				err = false;
				ut32 thnum = getthimmed12 (ao->a[2]);
				
				if (!err && (!(m & W_BIT))) {
					ao->o = 0xadf10000;
					ao->o |= thnum;
					ao->o |= reg1;
					if (m & S_BIT) {
						ao->o |= 1 << 28;
					}
					return 4;
				}

				if (num > 4096) {
					return -1;
				}

				ao->o = 0xadf20000;
				ao->o |= getthzeroimmed12 (num);
				ao->o |= reg1;
				return 4;
			}

			if ((reg1 < 8) && (reg2 < 8) && (!(m & DOTW_BIT)) && (!(m & W_BIT)) && (num < 8)) {
				ao->o = 0x001e;
				ao->o |= reg1 << 8;
				ao->o |= reg2 << 11;
				ao->o |= (num & 0x3) << 14;
				ao->o |= (num >> 2);
				return 2;
			}

			if ((reg1 < 8) && (reg1 == reg2) && (!(m & DOTW_BIT)) && (!(m & W_BIT)) && (num < 256)) {
				ao->o = 0x0038;
				ao->o |= reg1;
				ao->o |= num << 8;
				return 2;
			}

			err = false;
			ut32 thnum = getthimmed12 (ao->a[2]);
			
			if (!err && (!(m & W_BIT))) {
				ao->o = 0xa0f10000;
				ao->o |= thnum;
				return std_32bit_2reg (ao, m, false);
			}

			if (num > 4096) {
				return -1;
			}

			ao->o = 0xa0f20000;
			ao->o |= reg1;
			ao->o |= reg2 << 24;
			ao->o |= getthzeroimmed12 (num);
			return 4;
		        }
			break;
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->a[3] = "lsl 0";
		        }
			// intentional fallthrough
		case THUMB_REG_REG_SHIFT:
			if (ao->a[3] == NULL) { // double fallthrough
				std_opt_3 (ao);
			}
			// intentional fallthrough
		case THUMB_REG_REG_REG_SHIFT: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getreg (ao->a[2]);
			ut32 shift = thumb_getshift (ao->a[3]);

			if (reg2 == 13) {
				ao->o = 0xadeb0000;
				ao->o |= reg1;
				ao->o |= reg3 << 8;
				ao->o |= shift;
				if (m & S_BIT) {
					ao->o |= 1 << 28;
				}
				return 4;
			}

			if ((shift == 0) && (reg1 < 8) && (reg2 < 8) && (reg3 < 8) && (!(m & DOTW_BIT))) {
				ao->o = 0x001a;
				ao->o |= reg1 << 8;
				ao->o |= reg2 << 11;
				ao->o |= (reg3 & 0x3) << 14;
				ao->o |= (reg3 >> 2);
				return 2;
			}
			
			ao->o = 0xa0eb0000;
			return std_32bit_3reg (ao, m, true);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "svc", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_CONST: {
			ut32 num = getnum (ao->a[0]);
			if (num > 255) {
				return -1;
			}
			ao->o = 0x00df;
			ao->o |= num << 8;
			return 2;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "sxta", B_BIT | H_BIT | SIXTEEN_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->a[3] = "lsl 0";
		        }
			// intentional fallthrough
		case THUMB_REG_REG_SHIFT: {
			if (ao->a[3] == NULL) { // double fallthrough
				std_opt_3 (ao);
			}
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG_SHIFT: {
			ut32 shift = thumb_getshift (ao->a[3]);

			if ((shift != 0) && ((shift & 0x0000f010) != 0x00003000)) {
				return -1;
			}

			ut64 sufsel = m & (B_BIT | H_BIT | SIXTEEN_BIT);
			
			if (sufsel == B_BIT) {
				ao->o = 0x40fa80f0;
			} else
			if (sufsel == (B_BIT | SIXTEEN_BIT)) {
				ao->o = 0x20fa80f0;
			} else
			if (sufsel == H_BIT) {
				ao->o = 0x00fa80f0;
			} else {
				return -1;
			}

			ao->o |= (shift & 0x00000060) << 7;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		}
	} else
	if ((m = opmask (ao->op, "sxt", B_BIT | H_BIT | SIXTEEN_BIT))) {
		ut64 sufsel = m & (B_BIT | H_BIT | SIXTEEN_BIT);
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			ao->a[2] = "lsl 0";
		        }
			// intentional fallthrough
		case THUMB_REG_REG_SHIFT: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut32 shift = thumb_getshift (ao->a[2]);

			if ((reg1 > 15) && (reg2 > 15) && (shift != 0) && ((shift & 0x0000f010) != 0x00003000)) {
				return -1;
			}

			if (sufsel == B_BIT) {
				ao->o = 0x40b2;
				if ((shift == 0) && std_16bit_2reg (ao, m)) {
					return 2;
				}
				ao->o = 0x4ffa80f0;
			} else
			if (sufsel == (B_BIT | SIXTEEN_BIT)) {
				ao->o = 0x2ffa80f0;
			} else
			if (sufsel == H_BIT) {
				ao->o = 0x00b2;
				if ((shift == 0) && std_16bit_2reg (ao, m)) {
					return 2;
				}
				ao->o = 0x0ffa80f0;
			} else {
				return -1;
			}

			ao->o |= (shift & 0x00000060) << 7;
			ao->o |= reg1;
			ao->o |= reg2 << 8;
			return 4;
		        }
			break;
		}
	} else
	if ((m = opmask (ao->op, "tb", B_BIT | H_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		ut64 sufsel = m & (B_BIT | H_BIT);
		switch (argt) {
		case THUMB_BRACKREG_REGBRACK: {
			ut8 reg1 = getregmemstart (ao->a[0]);
			ut8 reg2 = getregmemend (ao->a[1]);

			if ((reg1 > 15) || (reg2 > 15)) {
				return -1;
			}

			if (sufsel == B_BIT) {
				ao->o = 0xd0e800f0;
				ao->o |= reg1 << 24;
				ao->o |= reg2 << 8;
				return 4;
			}
			ao->a[1][strlen (ao->a[1]) - 1] = '\0';
			ao->a[2] = "lsl 1]";
		        }
			// intentional fallthrough
		case THUMB_BRACKREG_REG_SHIFTBRACK: {
			ut8 reg1 = getregmemstart (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut32 shift = getshiftmemend (ao->a[2]);
			
			if ((reg1 > 15) || (reg2 > 15) || (shift != 0x00004000) || (sufsel != H_BIT)) {
				return -1;
			}

			ao->o = 0xd0e810f0;
			ao->o |= reg1 << 24;
			ao->o |= reg2 << 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "teq", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST: {
			ut8 reg = getreg (ao->a[0]);
			err = false;
			ut32 num = getthimmed12 (ao->a[1]);

			if (err || (reg > 15)) {
				return -1;
			}

			ao->o = 0x90f0000f;
			ao->o |= reg << 24;
			ao->o |= num;
			return 4;
		        }
			break;
		case THUMB_REG_REG: {
			ao->a[2] = "lsl 0";
		        }
			// intentional fallthrough
		case THUMB_REG_REG_SHIFT: {
			ao->o = 0x90ea000f;
			return std_32bit_2reg (ao, m, true);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "tst", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST: {
			ut8 reg1 = getreg (ao->a[0]);
			err = false;
			ut32 num = getthimmed12 (ao->a[1]);

			if (err || (reg1 > 15)) {
				return -1;
			}

			ao->o = 0x10f0000f;
			ao->o |= reg1 << 24;
			ao->o |= num;
			return 4;
		        }
			break;
		case THUMB_REG_REG: {
			ao->o = 0x0042;
			
			if (std_16bit_2reg (ao, m)) {
				return 2;
			}

			ao->a[2] = "lsl 0";
		        }
			// intentional fallthrough
		case THUMB_REG_REG_SHIFT: {
			ao->o = 0x10ea000f;
			return std_32bit_2reg (ao, m, true);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "uadd", EIGHT_BIT | SIXTEEN_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			if (m & EIGHT_BIT) {
				ao->o = 0x80fa40f0;
			} else
			if (m & SIXTEEN_BIT) {
				ao->o = 0x90fa40f0;
			} else {
				return -1;
			}

			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "uasx", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0xa0fa40f0;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "ubfx", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG_CONST_CONST: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut32 lsb = getnum (ao->a[2]);
			ut32 widthm1 = getnum (ao->a[3]) - 1;
			
			if ((reg1 > 15) || (reg2 > 15) || (lsb > 31) || ((31 - lsb) <= widthm1)) {
				return -1;
			}

			ao->o = 0xc0f30000;
			ao->o |= reg1;
			ao->o |= reg2 << 24;
			ao->o |= (lsb & 0x1c) << 2;
			ao->o |= (lsb & 0x3) << 14;
			ao->o |= widthm1 << 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "udiv", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0xb0fbf0f0;
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "uhadd", EIGHT_BIT | SIXTEEN_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		ut64 sufsel = m & (EIGHT_BIT | SIXTEEN_BIT);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			if (sufsel == EIGHT_BIT) {
				ao->o = 0x80fa60f0;
			} else 
			if (sufsel == SIXTEEN_BIT) {
				ao->o = 0x90fa60f0;
			} else {
				return -1;
			}
				
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "uhasx", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0xa0fa60f0;
			
			return std_32bit_3reg (ao, m, false);
			
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "uhsax", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0xe0fa60f0;
			
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "uhsub", EIGHT_BIT | SIXTEEN_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		ut64 sufsel = m & (EIGHT_BIT | SIXTEEN_BIT);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			if (sufsel == EIGHT_BIT) {
				ao->o = 0xc0fa60f0;
			} else 
			if (sufsel == SIXTEEN_BIT) {
				ao->o = 0xd0fa60f0;
			} else {
				return -1;
			}
				
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "umaal", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG_REG_REG: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getreg (ao->a[2]);
			ut8 reg4 = getreg (ao->a[3]);

			if ((reg1 > 15) || (reg2 > 15) || (reg3 > 15) || (reg4 > 15)) {
				return -1;
			}
			
			ao->o = 0xe0fb6000;
			ao->o |= reg1 << 4;
			ao->o |= reg2;
			ao->o |= reg3 << 24;
			ao->o |= reg4 << 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "umlal", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG_REG_REG: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getreg (ao->a[2]);
			ut8 reg4 = getreg (ao->a[3]);

			if ((reg1 > 15) || (reg2 > 15) || (reg3 > 15) || (reg4 > 15)) {
				return -1;
			}
			
			ao->o = 0xe0fb0000;
			ao->o |= reg1 << 4;
			ao->o |= reg2;
			ao->o |= reg3 << 24;
			ao->o |= reg4 << 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "umull", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG_REG_REG: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getreg (ao->a[2]);
			ut8 reg4 = getreg (ao->a[3]);

			if ((reg1 > 15) || (reg2 > 15) || (reg3 > 15) || (reg4 > 15)) {
				return -1;
			}
			
			ao->o = 0xa0fb0000;
			ao->o |= reg1 << 4;
			ao->o |= reg2;
			ao->o |= reg3 << 24;
			ao->o |= reg4 << 8;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "uqadd", EIGHT_BIT | SIXTEEN_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		ut64 sufsel = m & (EIGHT_BIT | SIXTEEN_BIT);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			if (sufsel == EIGHT_BIT) {
				ao->o = 0x80fa50f0;
			} else 
			if (sufsel == SIXTEEN_BIT) {
				ao->o = 0x90fa50f0;
			} else {
				return -1;
			}
				
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "uqasx", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0xa0fa50f0;
			
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "uqsax", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0xe0fa50f0;
			
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "uqsub", EIGHT_BIT | SIXTEEN_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		ut64 sufsel = m & (EIGHT_BIT | SIXTEEN_BIT);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			if (sufsel == EIGHT_BIT) {
				ao->o = 0xc0fa50f0;
			} else 
			if (sufsel == SIXTEEN_BIT) {
				ao->o = 0xd0fa50f0;
			} else {
				return -1;
			}
				
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "usad8", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0x70fb00f0;
			
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "usada8", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG_REG_REG: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut8 reg3 = getreg (ao->a[2]);
			ut8 reg4 = getreg (ao->a[3]);

			if ((reg1 > 15) || (reg2 > 15) || (reg3 > 15) || (reg4 > 15)) {
				return -1;
			}
			
			ao->o = 0x70fb0000;
			ao->o |= reg1;
			ao->o |= reg2 << 24;
			ao->o |= reg3 << 8;
			ao->o |= reg4 << 4;
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "usat", SIXTEEN_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_CONST_REG: {
			if (m & SIXTEEN_BIT) {
				ut8 reg1 = getreg (ao->a[0]);
				ut32 num = getnum (ao->a[1]);
				ut8 reg2 = getreg (ao->a[2]);

				if ((reg1 > 15) || (num > 15) || (reg2 > 15)) {
					return -1;
				}

				ao->o = 0xa0f30000;
				ao->o |= reg1;
				ao->o |= reg2 << 24;
				ao->o |= num << 8;
				return 4;
			}

			ao->a[3] = "lsl 0";
		        }
			// intentional fallthrough
		case THUMB_REG_CONST_REG_SHIFT: {
			ut8 reg1 = getreg (ao->a[0]);
			ut32 num = getnum (ao->a[1]);
			ut8 reg2 = getreg (ao->a[2]);
			ut32 shift = thumb_getshift (ao->a[3]);

			if ((reg1 > 15) || (num > 31) || (reg2 > 15) || (m & SIXTEEN_BIT) || ((shift & 0x00001000) != 0)) {
				return -1;
			}

			ao->o = 0x80f30000;
			ao->o |= reg1;
			ao->o |= (num & 0xf) << 8;
			ao->o |= (num >> 4 ) << 12;
			ao->o |= reg2 << 24;
			ao->o |= (shift & 0x00002000) << 16;
			ao->o |= (shift & 0x0000c070);
			return 4;
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "usax", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->o = 0xe0fa40f0;
			
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "usub", EIGHT_BIT | SIXTEEN_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		ut64 sufsel = m & (EIGHT_BIT | SIXTEEN_BIT);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			if (sufsel == EIGHT_BIT) {
				ao->o = 0xc0fa40f0;
			} else 
			if (sufsel == SIXTEEN_BIT) {
				ao->o = 0xd0fa40f0;
			} else {
				return -1;
			}
				
			return std_32bit_3reg (ao, m, false);
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "uxta", B_BIT | H_BIT | SIXTEEN_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		ut64 sufsel = m & (B_BIT | H_BIT | SIXTEEN_BIT);
		switch (argt) {
		case THUMB_REG_REG: {
			std_opt_2 (ao);
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG: {
			ao->a[3] = "lsl 0";
		        }
			// intentional fallthrough
		case THUMB_REG_REG_SHIFT: {
			if (ao->a[3] == NULL) { // double fallthrough
				std_opt_3 (ao);
			}
		        }
			// intentional fallthrough
		case THUMB_REG_REG_REG_SHIFT: {
			ut32 shift = thumb_getshift (ao->a[3]);

			if (shift && ((shift & 0x0000f010) != 0x00003000)) {
				return -1;
			}

			if (sufsel == B_BIT) {
				ao->o = 0x50fa80f0;
			} else
			if (sufsel == (B_BIT | SIXTEEN_BIT)) {
				ao->o = 0x30fa80f0;
			} else
			if (sufsel == H_BIT) {
				ao->o = 0x10fa80f0;
			} else {
				return -1;
			}

			ao->o |= (shift & 0x00000060) << 7;
			return (std_32bit_3reg (ao, m, false));
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "uxt", B_BIT | H_BIT | SIXTEEN_BIT))) {
		ut64 argt = thumb_selector (ao->a);
		ut64 sufsel = m & (B_BIT | H_BIT | SIXTEEN_BIT);
		switch (argt) {
		case THUMB_REG_REG: {
			if ((sufsel == B_BIT) || (sufsel == H_BIT)) {
				if (sufsel == B_BIT) {
					ao->o = 0xc0b2;
				} else {
					ao->o = 0x80b2;
				}
				if (std_16bit_2reg (ao, m)) {
					return 2;
				}
			}
			ao->a[2] = "lsl 0";
		        }
			// intentional fallthrough
		case THUMB_REG_REG_SHIFT: {
			ut8 reg1 = getreg (ao->a[0]);
			ut8 reg2 = getreg (ao->a[1]);
			ut32 shift = thumb_getshift (ao->a[2]);

			if ((reg1 > 15) || (reg2 > 15) || (shift && ((shift & 0x0000f010) != 0x00003000))) {
				return -1;
			}

			if (sufsel == B_BIT) {
				ao->o = 0x5ffa80f0;
			} else
			if (sufsel == (B_BIT | SIXTEEN_BIT)) {
				ao->o = 0x3ffa80f0;
			} else
			if (sufsel == H_BIT) {
				ao->o = 0x1ffa80f0;
			} else {
				return -1;
			}

			ao->o |= (shift & 0x00000060) << 7;
			ao->o |= reg1;
			ao->o |= reg2 << 8;
			return 4;
		        }
			break;
		default:
			return -1;

		}
	} else
	if ((m = opmask (ao->op, "wfe", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_NONE: {
			if (m & DOTW_BIT) {
				ao->o = 0xaff30280;
				return 4;
		        } else {
				ao->o = 0x20bf;
				return 2;
		        }
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "wfi", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_NONE: {
			if (m & DOTW_BIT) {
				ao->o = 0xaff30380;
				return 4;
		        } else {
				ao->o = 0x30bf;
				return 2;
		        }
		        }
			break;
		default:
			return -1;
		}
	} else
	if ((m = opmask (ao->op, "yield", 0))) {
		ut64 argt = thumb_selector (ao->a);
		switch (argt) {
		case THUMB_NONE: {
			if (m & DOTW_BIT) {
				ao->o = 0xaff30180;
				return 4;
		        } else {
				ao->o = 0x10bf;
				return 2;
		        }
		        }
			break;
		default:
			return -1;
		}
	}
	return 0;
}