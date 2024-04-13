AppendToSelectionBuffer(TScreen *screen, unsigned c, String selection)
{
    int which = TargetToSelection(screen, selection);
    SelectedCells *scp = &(screen->selected_cells[okSelectionCode(which)]);
    unsigned six;
    Char ch;

    /* Decode base64 character */
    if (c >= 'A' && c <= 'Z')
	six = c - 'A';
    else if (c >= 'a' && c <= 'z')
	six = c - 'a' + 26;
    else if (c >= '0' && c <= '9')
	six = c - '0' + 52;
    else if (c == '+')
	six = 62;
    else if (c == '/')
	six = 63;
    else
	return;

    /* Accumulate bytes */
    switch (screen->base64_count) {
    case 0:
	screen->base64_accu = six;
	screen->base64_count = 6;
	break;

    case 2:
	ch = CharOf((screen->base64_accu << 6) + six);
	screen->base64_count = 0;
	AppendStrToSelectionBuffer(scp, &ch, (size_t) 1);
	break;

    case 4:
	ch = CharOf((screen->base64_accu << 4) + (six >> 2));
	screen->base64_accu = (six & 0x3);
	screen->base64_count = 2;
	AppendStrToSelectionBuffer(scp, &ch, (size_t) 1);
	break;

    case 6:
	ch = CharOf((screen->base64_accu << 2) + (six >> 4));
	screen->base64_accu = (six & 0xF);
	screen->base64_count = 4;
	AppendStrToSelectionBuffer(scp, &ch, (size_t) 1);
	break;
    }
}