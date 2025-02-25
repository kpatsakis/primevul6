void CLASS parse_mos(int offset)
{
  char data[40];
  int skip, from, i, c, neut[4], planes = 0, frot = 0;
  static const char *mod[] = {"",
                              "DCB2",
                              "Volare",
                              "Cantare",
                              "CMost",
                              "Valeo 6",
                              "Valeo 11",
                              "Valeo 22",
                              "Valeo 11p",
                              "Valeo 17",
                              "",
                              "Aptus 17",
                              "Aptus 22",
                              "Aptus 75",
                              "Aptus 65",
                              "Aptus 54S",
                              "Aptus 65S",
                              "Aptus 75S",
                              "AFi 5",
                              "AFi 6",
                              "AFi 7",
                              "AFi-II 7",
                              "Aptus-II 7",
                              "",
                              "Aptus-II 6",
                              "",
                              "",
                              "Aptus-II 10",
                              "Aptus-II 5",
                              "",
                              "",
                              "",
                              "",
                              "Aptus-II 10R",
                              "Aptus-II 8",
                              "",
                              "Aptus-II 12",
                              "",
                              "AFi-II 12"};
  float romm_cam[3][3];

  fseek(ifp, offset, SEEK_SET);
  while (1)
  {
    if (get4() != 0x504b5453)
      break;
    get4();
    fread(data, 1, 40, ifp);
    skip = get4();
    from = ftell(ifp);

// IB start
#ifdef LIBRAW_LIBRARY_BUILD
    if (!strcmp(data, "CameraObj_camera_type"))
    {
      stmread(imgdata.lens.makernotes.body, skip, ifp);
    }
    if (!strcmp(data, "back_serial_number"))
    {
      char buffer[sizeof(imgdata.shootinginfo.BodySerial)];
      char *words[4];
      int nwords;
      stmread(buffer, skip, ifp);
      nwords = getwords(buffer, words, 4, sizeof(imgdata.shootinginfo.BodySerial));
      strcpy(imgdata.shootinginfo.BodySerial, words[0]);
    }
    if (!strcmp(data, "CaptProf_serial_number"))
    {
      char buffer[sizeof(imgdata.shootinginfo.InternalBodySerial)];
      char *words[4];
      int nwords;
      stmread(buffer, skip, ifp);
      nwords = getwords(buffer, words, 4, sizeof(imgdata.shootinginfo.InternalBodySerial));
      strcpy(imgdata.shootinginfo.InternalBodySerial, words[0]);
    }
#endif
    // IB end
    if (!strcmp(data, "JPEG_preview_data"))
    {
      thumb_offset = from;
      thumb_length = skip;
    }
    if (!strcmp(data, "icc_camera_profile"))
    {
      profile_offset = from;
      profile_length = skip;
    }
    if (!strcmp(data, "ShootObj_back_type"))
    {
      fscanf(ifp, "%d", &i);
      if ((unsigned)i < sizeof mod / sizeof(*mod))
        strcpy(model, mod[i]);
    }
    if (!strcmp(data, "icc_camera_to_tone_matrix"))
    {
      for (i = 0; i < 9; i++)
        ((float *)romm_cam)[i] = int_to_float(get4());
      romm_coeff(romm_cam);
    }
    if (!strcmp(data, "CaptProf_color_matrix"))
    {
      for (i = 0; i < 9; i++)
        fscanf(ifp, "%f", (float *)romm_cam + i);
      romm_coeff(romm_cam);
    }
    if (!strcmp(data, "CaptProf_number_of_planes"))
      fscanf(ifp, "%d", &planes);
    if (!strcmp(data, "CaptProf_raw_data_rotation"))
      fscanf(ifp, "%d", &flip);
    if (!strcmp(data, "CaptProf_mosaic_pattern"))
      FORC4
      {
        fscanf(ifp, "%d", &i);
        if (i == 1)
          frot = c ^ (c >> 1);
      }
    if (!strcmp(data, "ImgProf_rotation_angle"))
    {
      fscanf(ifp, "%d", &i);
      flip = i - flip;
    }
    if (!strcmp(data, "NeutObj_neutrals") && !cam_mul[0])
    {
      FORC4 fscanf(ifp, "%d", neut + c);
      FORC3 cam_mul[c] = (float)neut[0] / neut[c + 1];
    }
    if (!strcmp(data, "Rows_data"))
      load_flags = get4();
    parse_mos(from);
    fseek(ifp, skip + from, SEEK_SET);
  }
  if (planes)
    filters = (planes == 1) * 0x01010101 * (uchar) "\x94\x61\x16\x49"[(flip / 90 + frot) & 3];
}