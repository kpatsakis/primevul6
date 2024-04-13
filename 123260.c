static int r_cmd_java_handle_help (RCore * core, const char * input) {
	ut32 i = 0;
	const char **help_msg = (const char**)malloc (sizeof(char *) * END_CMDS*4);
	help_msg[0] = "Usage:";
	help_msg[1] = "java [cmd] [arg..] ";
	help_msg[2] = r_core_plugin_java.desc;
	for (i = 0; i <END_CMDS; i++) {
		RCmdJavaCmd *cmd = &JAVA_CMDS[i];
		help_msg[3+(i*3)+0] = cmd->name;
		help_msg[3+(i*3)+1] = cmd->args;
		help_msg[3+(i*3)+2] = cmd->desc;
	}
	help_msg[3+(i*3)] = NULL;
	r_core_cmd_help (core, help_msg);
	free (help_msg);
	return true;
}