/************************
 * Nick Alexeev         *
 * Edward Hazelton      *
 * Prof. C. Lauter      *
 * CSCE A321            *
 * Assignment #1        *
 * Due: July 9th, 2020  *
 ************************/
int return_code=0;
#include "my_stdlib.h"
int main(int argc,char** argv){
	//parse command line arguments
	struct Arguments arg = arg_parser(argc,argv);
	//load file
	char* file_buffer = load_file(arg.f_name);
	//get range to print
	struct Vec2 head_index = get_head_index(file_buffer,arg.n);
	print_index(file_buffer,head_index.x,head_index.y);
	//free allocated data
	free(file_buffer);
	return return_code;
}
