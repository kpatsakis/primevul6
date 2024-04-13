void CLASS parseSamsungMakernotes(int base, unsigned tag, unsigned type, unsigned len, unsigned dng_writer)
{
   if (tag == 0x0002) {
     if (get4() == 0x2000) {
       imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Samsung_NX;
     } else if (!strncmp(model, "NX mini", 7)) {
       imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Samsung_NX_M;
     } else {
       imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_FixedLens;
       imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_FixedLens;
     }

   } else if (tag == 0x0003) {
     imgdata.lens.makernotes.CamID = unique_id = get4();

   } else if (tag == 0x0043) {
     int temp = get4();
     if (temp) {
       imgdata.other.CameraTemperature = (float)temp;
       if (get4() == 10)
         imgdata.other.CameraTemperature /= 10.0f;
     }

    } else if ((tag == 0xa002) && (dng_writer != AdobeDNG)) {
        stmread(imgdata.shootinginfo.BodySerial, len, ifp);

   } else if (tag == 0xa003) {
     imgdata.lens.makernotes.LensID = get2();
     if (imgdata.lens.makernotes.LensID)
       imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Samsung_NX;

   } else if (tag == 0xa005) {
     stmread(imgdata.lens.InternalLensSerial, len, ifp);

   } else if (tag == 0xa019) {
     imgdata.lens.makernotes.CurAp = getreal(type);

   } else if (tag == 0xa01a) {
     imgdata.lens.makernotes.FocalLengthIn35mmFormat = get4() / 10.0f;
     if (imgdata.lens.makernotes.FocalLengthIn35mmFormat < 10.0f)
       imgdata.lens.makernotes.FocalLengthIn35mmFormat *= 10.0f;
   }
}