void Con_ToggleMenu_f( void ) {
	CL_KeyEvent( K_ESCAPE, qtrue, Sys_Milliseconds() );
	CL_KeyEvent( K_ESCAPE, qfalse, Sys_Milliseconds() );
}