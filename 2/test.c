#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <sys/stat.h>
#include <string.h>

struct Foo{
	char d[100];
};
int main(int argc,char** argv){

	       int fstatat(int dirfd, const char *pathname, struct stat *statbuf,
			                          int flags);
	struct stat stat_s;
	printf("querning path: %s\n",argv[1]);
	int status = stat(argv[1],&stat_s);
	if(status!=0){
		printf("%s\n",strerror(errno));
		printf("%i\n",errno);
	}else{
		printf("all good\n");

	}
}
