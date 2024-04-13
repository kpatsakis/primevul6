static void exif_process_SOFn (uchar *Data, int marker, jpeg_sof_info *result)
{
/* 0xFF SOSn SectLen(2) Bits(1) Height(2) Width(2) Channels(1)  3*Channels (1)  */
	result->bits_per_sample = Data[2];
	result->height          = php_jpg_get16(Data+3);
	result->width           = php_jpg_get16(Data+5);
	result->num_components  = Data[7];

/*	switch (marker) {
		case M_SOF0:  process = "Baseline";  break;
		case M_SOF1:  process = "Extended sequential";  break;
		case M_SOF2:  process = "Progressive";  break;
		case M_SOF3:  process = "Lossless";  break;
		case M_SOF5:  process = "Differential sequential";  break;
		case M_SOF6:  process = "Differential progressive";  break;
		case M_SOF7:  process = "Differential lossless";  break;
		case M_SOF9:  process = "Extended sequential, arithmetic coding";  break;
		case M_SOF10: process = "Progressive, arithmetic coding";  break;
		case M_SOF11: process = "Lossless, arithmetic coding";  break;
		case M_SOF13: process = "Differential sequential, arithmetic coding";  break;
		case M_SOF14: process = "Differential progressive, arithmetic coding"; break;
		case M_SOF15: process = "Differential lossless, arithmetic coding";  break;
		default:      process = "Unknown";  break;
	}*/
}