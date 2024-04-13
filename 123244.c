static void r_cmd_java_print_cmd_help (RCmdJavaCmd *cmd) {
	eprintf ("[*] %s %s\n[+] %s\n\n", cmd->name, cmd->args, cmd->desc);
}