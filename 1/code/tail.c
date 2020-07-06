#include "my_stdlib.h"
int main(int argc,char** argv){
	char f_name[20] = "test.txt";
	//char* buff = load_file(f_name);
	struct Arguments arg = arg_parser(argc,argv);
	char* file_buffer = load_file(arg.f_name);
	struct Vec2 head_index = get_tail_index(file_buffer,arg.n);
	print_index(file_buffer,head_index.x,head_index.y);
	free(file_buffer);
	return 0;
}
