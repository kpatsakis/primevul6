static ut64 thumb_selector(char *args[]) {
	collect_list(args);
	ut64 res = 0;
	ut8 i;
	for (i = 0; i < 15; i++) {
		if (args[i] == NULL) {
			break;
		}
		if (getreg (args[i]) != -1) {
			res |= 1 << (i*4);
			continue;
		}
		err = false;
		getnum (args[i]);
		if (!err) {
			res |= 2 << (i*4);
			continue;
		}
		err = false;   	
		thumb_getshift (args[i]);
		if (!err) {
			res |= 3 << (i*4);
			continue;
		}
		if (getcoproc (args[i]) != -1) {
			res |= 4 << (i*4);
			continue;
		}
		if (getcoprocreg (args[i]) != -1) {
			res |= 5 << (i*4);
			continue;
		}
		if (getregmemstart (args[i]) != -1) {
			res |= 6 << (i*4);
			continue;
		}
		if (getregmemstartend (args[i]) != -1) {
			res |= 7 << (i*4);
			continue;
		}
		err = false;
		getnummemend(args[i]);
		if (!err) {
			res |= 8 << (i*4);
			continue;
		}
		err = false;
		getnummemendbang(args[i]);
		if (!err) {
			res |= 9 << (i*4);
			continue;
		}
		if (getregmembang (args[i]) != -1) {
			res |= 0xa << (i*4);
			continue;
		}
		if (getreglist (args[i]) != -1) {
			res |= 0xb << (i*4);
			continue;
		}
		if (getregmemend (args[i]) != -1) {
			res |= 0xc << (i*4);
			continue;
		}
		if (getshiftmemend (args[i]) != -1) {
			res |= 0xd << (i*4);
			continue;
		}
		err = false;
		getnumbang(args[i]);
		if (!err) {
			res |= 0xe << (i*4);
			continue;
		}
		res |= 0xf << (i*4);
	}
	err = false;
	return res;
}