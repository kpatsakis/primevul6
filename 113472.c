void Con_DrawInput (void) {
	int		y;

	if ( clc.state != CA_DISCONNECTED && !(Key_GetCatcher( ) & KEYCATCH_CONSOLE ) ) {
		return;
	}

	y = con.vislines - ( SMALLCHAR_HEIGHT * 2 );

	re.SetColor( con.color );

	SCR_DrawSmallChar( con.xadjust + 1 * SMALLCHAR_WIDTH, y, ']' );

	Field_Draw( &g_consoleField, con.xadjust + 2 * SMALLCHAR_WIDTH, y,
		SCREEN_WIDTH - 3 * SMALLCHAR_WIDTH, qtrue, qtrue );
}