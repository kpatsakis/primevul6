void printusage (void)
{
	printf("usage: sfconvert infile outfile [ options ... ] [ output keywords ... ]\n");
	printf("\n");

	printf("Where keywords specify format of input or output soundfile:\n");
	printf("    format f       file format f (see below)\n");
	printf("    compression c  compression format c (see below)\n");
	printf("    byteorder e    endian (e is big or little)\n");
	printf("    channels n     n-channel file (1 or 2)\n");
	printf("    integer n s    n-bit integer file, where s is one of\n");
	printf("                       2scomp: 2's complement signed data\n");
	printf("                       unsigned: unsigned data\n");
	printf("    float m        floating point file, maxamp m (usually 1.0)\n");
	printf("\n");

	printf("Currently supported file formats are:\n");
	printf("\n");
	printf("    aiff    Audio Interchange File Format\n");
	printf("    aifc    AIFF-C File Format\n");
	printf("    next    NeXT/Sun Format\n");
	printf("    wave    MS RIFF WAVE Format\n");
	printf("    bics    Berkeley/IRCAM/CARL Sound File Format\n");
	printf("    smp     Sample Vision Format\n");
	printf("    voc     Creative Voice File\n");
	printf("    nist    NIST SPHERE Format\n");
	printf("    caf     Core Audio Format\n");
	printf("\n");

	printf("Currently supported compression formats are:\n");
	printf("\n");
	printf("    ulaw    G.711 u-law\n");
	printf("    alaw    G.711 A-law\n");
	printf("    ima     IMA ADPCM\n");
	printf("    msadpcm MS ADPCM\n");
	printf("    flac    FLAC\n");
	printf("    alac    Apple Lossless Audio Codec\n");
	printf("\n");
}