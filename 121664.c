void CLASS parseRicohMakernotes(int base, unsigned tag, unsigned type, unsigned len, unsigned dng_writer)
{
   if (tag == 0x0005) {
     char buffer[17];
     int c;
     int count = 0;
     fread(buffer, 16, 1, ifp);
     buffer[16] = 0;
     FORC(16) {
       if ((isspace(buffer[c])) || (buffer[c] == 0x2D) || (isalnum(buffer[c])))
         count++;
       else
         break;
     }
     if (count == 16) {
       if (strncmp(model, "GXR", 3)) {
         sprintf(imgdata.shootinginfo.BodySerial, "%8s", buffer + 8);
       }
       buffer[8] = 0;
       sprintf(imgdata.shootinginfo.InternalBodySerial, "%8s", buffer);

     } else {
       sprintf(imgdata.shootinginfo.BodySerial, "%02x%02x%02x%02x",
                buffer[4], buffer[5], buffer[6], buffer[7]);
       sprintf(imgdata.shootinginfo.InternalBodySerial, "%02x%02x%02x%02x",
                buffer[8], buffer[9], buffer[10], buffer[11]);
     }

   } else if ((tag == 0x1001) && (type == 3)) {
     imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_FixedLens;
     imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_FixedLens;
     imgdata.lens.makernotes.CameraFormat = LIBRAW_FORMAT_APSC;
     imgdata.lens.makernotes.LensID = -1;
     imgdata.lens.makernotes.FocalType = 1;
     imgdata.shootinginfo.ExposureProgram = get2();

   } else if (tag == 0x1002) {
     imgdata.shootinginfo.DriveMode =  get2();

   } else if (tag == 0x1006) {
     imgdata.shootinginfo.FocusMode =  get2();

   } else if ((tag == 0x100b) && (type == 10)) {
     imgdata.other.FlashEC = getreal(type);

   } else if ((tag == 0x1017) && (get2() == 2)) {
     strcpy(imgdata.lens.makernotes.Attachment, "Wide-Angle Adapter");

   } else if (tag == 0x1500) {
     imgdata.lens.makernotes.CurFocal = getreal(type);

   } else if ((tag == 0x2001) && !strncmp(model, "GXR", 3)) {
     short ntags, cur_tag;
     fseek(ifp, 20, SEEK_CUR);
     ntags = get2();
     cur_tag = get2();
     while (cur_tag != 0x002c)
     {
       fseek(ifp, 10, SEEK_CUR);
       cur_tag = get2();
     }
     fseek(ifp, 6, SEEK_CUR);
     fseek(ifp, get4() + 20, SEEK_SET);
     stread(imgdata.shootinginfo.BodySerial, 12, ifp);
     get2();
     imgdata.lens.makernotes.LensID = getc(ifp) - '0';
     switch (imgdata.lens.makernotes.LensID)
     {
     case 1:
     case 2:
     case 3:
     case 5:
     case 6:
       imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_FixedLens;
       imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_RicohModule;
       break;
     case 8:
       imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Leica_M;
       imgdata.lens.makernotes.CameraFormat = LIBRAW_FORMAT_APSC;
       imgdata.lens.makernotes.LensID = -1;
       break;
     default:
       imgdata.lens.makernotes.LensID = -1;
     }
     fseek(ifp, 17, SEEK_CUR);
     stread(imgdata.lens.LensSerial, 12, ifp);
   }
}