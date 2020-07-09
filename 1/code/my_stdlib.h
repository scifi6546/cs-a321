#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
struct Vec2{
	ssize_t x;
	ssize_t y;
};
const ssize_t ALLOC_SIZE=0x400;
size_t strlen(const char *str){
	size_t len = 0;
	while(str[len]!=0){
		len++;
	}
	return len;
}
void error(char* str){
	return_code=1;
	if(write(2,str,strlen(str))){
		return_code=1;
	}
}
void print(char* str){
	if(str==NULL){
		error("str is null pointer\n");
		return;

	}
	write(1,str,strlen(str));
}
void print_index(char* str,ssize_t start,ssize_t end){
	if(strlen(str)<end){
		error("Error invalid end size");
		return;
	}
	if(write(1,str+start,end-start)==-1){
		error("failed to write to stdio\n");

	}

}
//returns 1 if the strings are equal 0 if they are not
char my_strcmp(const char *a,const char *b){
	ssize_t i=0;
	while(1){
		if(a[i]==0 && b[i]==0){
			return 1;
		}
		if(a[i]==0||b[i]==0){
			return 0;
		}
		if(a[i]!=b[i]){
			return 0;
		}
		i++;
	}
}
char* load_file(char* f_name){
	int fd = 0;
	if(f_name==NULL){
		fd = 0;
	}else{
		fd = open(f_name,O_RDONLY);
	}
	if(fd==-1){
		error("Error: file \"");
		error(f_name);
		error("\" can not be read\n");
	}
	size_t alloc_size = ALLOC_SIZE;
	size_t read_size=0;
	char* buff = calloc(ALLOC_SIZE,sizeof(char));
	while(1){
		ssize_t bytes_read = read(fd,buff+read_size,alloc_size-read_size);
		if(bytes_read+read_size!=alloc_size){
			read_size+=bytes_read;
			break;
		}else{
			read_size+=bytes_read;
			char* new_buff = (char*) realloc(buff,alloc_size*2);
			if(new_buff==NULL){
				error("Error: reallocation failed\n");
				break;
			}else{
				buff = new_buff;
				alloc_size*=2;
			}

		}

	}
	if(alloc_size<read_size+1){
		char* new_buff = (char*) realloc(buff,read_size+1);
		if(new_buff==NULL){
			error("Failure in allocation\n");
		}else{
			buff = new_buff;
			alloc_size=read_size+1;
		}
		

	}
	buff[read_size+1]=0;
	if(fd!=0){
		close(fd);

	}
	return buff;
}
struct Arguments{
	int n;
	char* f_name;
};
long my_pow(long base,long pow){
	long out=1;
	for(long i = 0;i<pow;i++){
		out*=base;
	}
	return out;
}
//Converts string to int
long to_int(char* str){
	long out =0;
	long len = strlen(str);
	for(ssize_t i = 0;i<len;i++){
		if(str[i]>0x39||str[i]<0x30){
			error("Syntax error: invalid length");
		}
		out*=10;
		out+=(str[i]-0x30);
	}
	return out;
}
struct Arguments arg_parser(int argc,char** argv){
	struct Arguments args;
	args.n = 10;
	args.f_name = NULL;
	char in_n = 0;
	const char n[20] = "-n";
	const char nose[20] = "--nose";
	for(int i=1;i<argc;i++){
		if(my_strcmp(n,argv[i]) || my_strcmp(nose,argv[i])){
			in_n = 1;
		}else{
			if(in_n){
				args.n = to_int(argv[i]);
				in_n=0;
			}else{
				args.f_name = argv[i];
			}

		}
	}
	return args;
}
struct Vec2 get_head_index(const char* str,int n){
	ssize_t len = strlen(str);
	struct Vec2 out;
	out.x = 0;
	out.y = 0;
	int new_line = 1;
	for(ssize_t i = 0;i<len;i++){
		if(str[i]=='\n'){
			if(new_line==n){
				out.y++;
				break;

			}
			out.y++;
			new_line++;
		}else{
			out.y++;

		}
	}
	return out;
}
struct Vec2 get_tail_index(const char* str,int n){
	ssize_t len = strlen(str);
	struct Vec2 out;
	out.x = len-1;
	out.y = len-1;
	int new_line = 1;
	for(ssize_t i = len-1;i>=0;i--){
		if(str[i]=='\n'){
			if(new_line==n){
				out.x--;
				break;

			}
			out.x--;
			new_line++;
		}else{
			out.x--;

		}
	}
	return out;
}
