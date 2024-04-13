static void S_AL_SoundInfo(void)
{
	Com_Printf( "OpenAL info:\n" );
	Com_Printf( "  Vendor:         %s\n", qalGetString( AL_VENDOR ) );
	Com_Printf( "  Version:        %s\n", qalGetString( AL_VERSION ) );
	Com_Printf( "  Renderer:       %s\n", qalGetString( AL_RENDERER ) );
	Com_Printf( "  AL Extensions:  %s\n", qalGetString( AL_EXTENSIONS ) );
	Com_Printf( "  ALC Extensions: %s\n", qalcGetString( alDevice, ALC_EXTENSIONS ) );

	if(enumeration_all_ext)
		Com_Printf("  Device:         %s\n", qalcGetString(alDevice, ALC_ALL_DEVICES_SPECIFIER));
	else if(enumeration_ext)
		Com_Printf("  Device:         %s\n", qalcGetString(alDevice, ALC_DEVICE_SPECIFIER));

	if(enumeration_all_ext || enumeration_ext)
		Com_Printf("  Available Devices:\n%s", s_alAvailableDevices->string);

#ifdef USE_VOIP
	if(capture_ext)
	{
		Com_Printf("  Input Device:   %s\n", qalcGetString(alCaptureDevice, ALC_CAPTURE_DEVICE_SPECIFIER));
		Com_Printf("  Available Input Devices:\n%s", s_alAvailableInputDevices->string);
	}
#endif
}