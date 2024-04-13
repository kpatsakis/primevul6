static ut64 r_cmd_java_get_input_num_value(RCore *core, const char *input_value){
	ut64 value = input_value ? r_num_math (core->num, input_value) : 0;
	return value;
}