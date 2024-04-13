drv_init(TERMINAL_CONTROL_BLOCK * TCB)
{
    TERMINAL *trm;

    AssertTCB();

    trm = (TERMINAL *) TCB;

    TCB->info.initcolor = VALID_STRING(initialize_color);
    TCB->info.canchange = can_change;
    TCB->info.hascolor = ((VALID_NUMERIC(max_colors) && VALID_NUMERIC(max_pairs)
			   && (((set_foreground != NULL)
				&& (set_background != NULL))
			       || ((set_a_foreground != NULL)
				   && (set_a_background != NULL))
			       || set_color_pair)) ? TRUE : FALSE);

    TCB->info.caninit = !(exit_ca_mode && non_rev_rmcup);

    TCB->info.maxpairs = VALID_NUMERIC(max_pairs) ? max_pairs : 0;
    TCB->info.maxcolors = VALID_NUMERIC(max_colors) ? max_colors : 0;
    TCB->info.numlabels = VALID_NUMERIC(num_labels) ? num_labels : 0;
    TCB->info.labelwidth = VALID_NUMERIC(label_width) ? label_width : 0;
    TCB->info.labelheight = VALID_NUMERIC(label_height) ? label_height : 0;
    TCB->info.nocolorvideo = VALID_NUMERIC(no_color_video) ? no_color_video
	: 0;
    TCB->info.tabsize = VALID_NUMERIC(init_tabs) ? (int) init_tabs : 8;

    TCB->info.defaultPalette = hue_lightness_saturation ? _nc_hls_palette : _nc_cga_palette;

    /*
     * If an application calls setupterm() rather than initscr() or
     * newterm(), we will not have the def_prog_mode() call in
     * _nc_setupscreen().  Do it now anyway, so we can initialize the
     * baudrate.
     */
    if (NC_ISATTY(trm->Filedes)) {
	TCB->drv->td_mode(TCB, TRUE, TRUE);
    }
}