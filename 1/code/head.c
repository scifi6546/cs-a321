#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
size_t strlen(char *str){
	size_t len = 0;
	while(str[len]!=0){
		len++;
	}
	return len;

}
void print(char* str){
	write(1,str,strlen(str));

}
int main(int argc,char** argv){
	int fd = open(argv[1],O_RDONLY);
	char buff[100];
	ssize_t read_num = read(fd,buff,100);
	print(buff);

}
