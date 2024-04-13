void CLASS getOlympus_CameraType2 ()
{

  if (OlyID != 0x0ULL) return;

  int i=0;

  fread(imgdata.makernotes.olympus.CameraType2, 6, 1, ifp);
  while ((i < 6) &&
         imgdata.makernotes.olympus.CameraType2[i]) {
     OlyID = OlyID << 8 | imgdata.makernotes.olympus.CameraType2[i];
     if (isspace(imgdata.makernotes.olympus.CameraType2[i+1]))
       imgdata.makernotes.olympus.CameraType2[i+1] = '\0';
     i++;
  }

  setOlympusBodyFeatures(OlyID);
  return;
}