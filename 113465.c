void Con_PageDown( void ) {
	con.display += 2;
	if (con.display > con.current) {
		con.display = con.current;
	}
}