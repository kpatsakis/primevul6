static int _TIFFGetMaxColorChannels( uint16 photometric )
{
    switch (photometric) {
	case PHOTOMETRIC_PALETTE:
	case PHOTOMETRIC_MINISWHITE:
	case PHOTOMETRIC_MINISBLACK:
            return 1;
	case PHOTOMETRIC_YCBCR:
	case PHOTOMETRIC_RGB:
	case PHOTOMETRIC_CIELAB:
            return 3;
	case PHOTOMETRIC_SEPARATED:
	case PHOTOMETRIC_MASK:
            return 4;
	case PHOTOMETRIC_LOGL:
	case PHOTOMETRIC_LOGLUV:
	case PHOTOMETRIC_CFA:
	case PHOTOMETRIC_ITULAB:
	case PHOTOMETRIC_ICCLAB:
	default:
            return 0;
    }
}