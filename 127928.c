static char * exif_get_markername(int marker)
{
	switch(marker) {
		case 0xC0: return "SOF0";
		case 0xC1: return "SOF1";
		case 0xC2: return "SOF2";
		case 0xC3: return "SOF3";
		case 0xC4: return "DHT";
		case 0xC5: return "SOF5";
		case 0xC6: return "SOF6";
		case 0xC7: return "SOF7";
		case 0xC9: return "SOF9";
		case 0xCA: return "SOF10";
		case 0xCB: return "SOF11";
		case 0xCD: return "SOF13";
		case 0xCE: return "SOF14";
		case 0xCF: return "SOF15";
		case 0xD8: return "SOI";
		case 0xD9: return "EOI";
		case 0xDA: return "SOS";
		case 0xDB: return "DQT";
		case 0xDC: return "DNL";
		case 0xDD: return "DRI";
		case 0xDE: return "DHP";
		case 0xDF: return "EXP";
		case 0xE0: return "APP0";
		case 0xE1: return "EXIF";
		case 0xE2: return "FPIX";
		case 0xE3: return "APP3";
		case 0xE4: return "APP4";
		case 0xE5: return "APP5";
		case 0xE6: return "APP6";
		case 0xE7: return "APP7";
		case 0xE8: return "APP8";
		case 0xE9: return "APP9";
		case 0xEA: return "APP10";
		case 0xEB: return "APP11";
		case 0xEC: return "APP12";
		case 0xED: return "APP13";
		case 0xEE: return "APP14";
		case 0xEF: return "APP15";
		case 0xF0: return "JPG0";
		case 0xFD: return "JPG13";
		case 0xFE: return "COM";
		case 0x01: return "TEM";
	}
	return "Unknown";
}