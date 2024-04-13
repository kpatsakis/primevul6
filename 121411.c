visibleScan(int mode)
{
    const char *result = "?";
#define DATA(name) case name: result = #name; break
    switch (mode) {
	DATA(scanMods);
	DATA(scanKey);
	DATA(scanColon);
	DATA(scanFunc);
	DATA(scanArgs);
    }
#undef DATA
    return result;
}