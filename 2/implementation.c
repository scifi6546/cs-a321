/*

  MyFS: a tiny file-system written for educational purposes

  MyFS is 

  Copyright 2018-20 by

  University of Alaska Anchorage, College of Engineering.

  Contributors: Christoph Lauter
                ... Nicholas Alexeev
                ...

  and based on 

  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall myfs.c implementation.c `pkg-config fuse --cflags --libs` -o myfs

*/

#include <stddef.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>


/* The filesystem you implement must support all the 13 operations
   stubbed out below. There need not be support for access rights,
   links, symbolic links. There needs to be support for access and
   modification times and information for statfs.

   The filesystem must run in memory, using the memory of size 
   fssize pointed to by fsptr. The memory comes from mmap and 
   is backed with a file if a backup-file is indicated. When
   the filesystem is unmounted, the memory is written back to 
   that backup-file. When the filesystem is mounted again from
   the backup-file, the same memory appears at the newly mapped
   in virtual address. The filesystem datastructures hence must not
   store any pointer directly to the memory pointed to by fsptr; it
   must rather store offsets from the beginning of the memory region.

   When a filesystem is mounted for the first time, the whole memory
   region of size fssize pointed to by fsptr reads as zero-bytes. When
   a backup-file is used and the filesystem is mounted again, certain
   parts of the memory, which have previously been written, may read
   as non-zero bytes. The size of the memory region is at least 2048
   bytes.

   CAUTION:

   * You MUST NOT use any global variables in your program for reasons
   due to the way FUSE is designed.

   You can find ways to store a structure containing all "global" data
   at the start of the memory region representing the filesystem.

   * You MUST NOT store (the value of) pointers into the memory region
   that represents the filesystem. Pointers are virtual memory
   addresses and these addresses are ephemeral. Everything will seem
   okay UNTIL you remount the filesystem again.

   You may store offsets/indices (of type size_t) into the
   filesystem. These offsets/indices are like pointers: instead of
   storing the pointer, you store how far it is away from the start of
   the memory region. You may want to define a type for your offsets
   and to write two functions that can convert from pointers to
   offsets and vice versa.

   * You may use any function out of libc for your filesystem,
   including (but not limited to) malloc, calloc, free, strdup,
   strlen, strncpy, strchr, strrchr, memset, memcpy. However, your
   filesystem MUST NOT depend on memory outside of the filesystem
   memory region. Only this part of the virtual memory address space
   gets saved into the backup-file. As a matter of course, your FUSE
   process, which implements the filesystem, MUST NOT leak memory: be
   careful in particular not to leak tiny amounts of memory that
   accumulate over time. In a working setup, a FUSE process is
   supposed to run for a long time!

   It is possible to check for memory leaks by running the FUSE
   process inside valgrind:

   valgrind --leak-check=full ./myfs --backupfile=test.myfs ~/fuse-mnt/ -f

   However, the analysis of the leak indications displayed by valgrind
   is difficult as libfuse contains some small memory leaks (which do
   not accumulate over time). We cannot (easily) fix these memory
   leaks inside libfuse.

   * Avoid putting debug messages into the code. You may use fprintf
   for debugging purposes but they should all go away in the final
   version of the code. Using gdb is more professional, though.

   * You MUST NOT fail with exit(1) in case of an error. All the
   functions you have to implement have ways to indicate failure
   cases. Use these, mapping your internal errors intelligently onto
   the POSIX error conditions.

   * And of course: your code MUST NOT SEGFAULT!

   It is reasonable to proceed in the following order:

   (1)   Design and implement a mechanism that initializes a filesystem
         whenever the memory space is fresh. That mechanism can be
         implemented in the form of a filesystem handle into which the
         filesystem raw memory pointer and sizes are translated.
         Check that the filesystem does not get reinitialized at mount
         time if you initialized it once and unmounted it but that all
         pieces of information (in the handle) get read back correctly
         from the backup-file. 

   (2)   Design and implement functions to find and allocate free memory
         regions inside the filesystem memory space. There need to be 
         functions to free these regions again, too. Any "global" variable
         goes into the handle structure the mechanism designed at step (1) 
         provides.

   (3)   Carefully design a data structure able to represent all the
         pieces of information that are needed for files and
         (sub-)directories.  You need to store the location of the
         root directory in a "global" variable that, again, goes into the 
         handle designed at step (1).
          
   (4)   Write __myfs_getattr_implem and debug it thoroughly, as best as
         you can with a filesystem that is reduced to one
         function. Writing this function will make you write helper
         functions to traverse paths, following the appropriate
         subdirectories inside the file system. Strive for modularity for
         these filesystem traversal functions.

   (5)   Design and implement __myfs_readdir_implem. You cannot test it
         besides by listing your root directory with ls -la and looking
         at the date of last access/modification of the directory (.). 
         Be sure to understand the signature of that function and use
         caution not to provoke segfaults nor to leak memory.

   (6)   Design and implement __myfs_mknod_implem. You can now touch files 
         with 

         touch foo

         and check that they start to exist (with the appropriate
         access/modification times) with ls -la.

   (7)   Design and implement __myfs_mkdir_implem. Test as above.

   (8)   Design and implement __myfs_truncate_implem. You can now 
         create files filled with zeros:

         truncate -s 1024 foo

   (9)   Design and implement __myfs_statfs_implem. Test by running
         df before and after the truncation of a file to various lengths. 
         The free "disk" space must change accordingly.

   (10)  Design, implement and test __myfs_utimens_implem. You can now 
         touch files at different dates (in the past, in the future).

   (11)  Design and implement __myfs_open_implem. The function can 
         only be tested once __myfs_read_implem and __myfs_write_implem are
         implemented.

   (12)  Design, implement and test __myfs_read_implem and
         __myfs_write_implem. You can now write to files and read the data 
         back:

         echo "Hello world" > foo
         echo "Hallo ihr da" >> foo
         cat foo

         Be sure to test the case when you unmount and remount the
         filesystem: the files must still be there, contain the same
         information and have the same access and/or modification
         times.

   (13)  Design, implement and test __myfs_unlink_implem. You can now
         remove files.

   (14)  Design, implement and test __myfs_unlink_implem. You can now
         remove directories.

   (15)  Design, implement and test __myfs_rename_implem. This function
         is extremely complicated to implement. Be sure to cover all 
         cases that are documented in man 2 rename. The case when the 
         new path exists already is really hard to implement. Be sure to 
         never leave the filessystem in a bad state! Test thoroughly 
         using mv on (filled and empty) directories and files onto 
         inexistant and already existing directories and files.

   (16)  Design, implement and test any function that your instructor
         might have left out from this list. There are 13 functions 
         __myfs_XXX_implem you have to write.

   (17)  Go over all functions again, testing them one-by-one, trying
         to exercise all special conditions (error conditions): set
         breakpoints in gdb and use a sequence of bash commands inside
         your mounted filesystem to trigger these special cases. Be
         sure to cover all funny cases that arise when the filesystem
         is full but files are supposed to get written to or truncated
         to longer length. There must not be any segfault; the user
         space program using your filesystem just has to report an
         error. Also be sure to unmount and remount your filesystem,
         in order to be sure that it contents do not change by
         unmounting and remounting. Try to mount two of your
         filesystems at different places and copy and move (rename!)
         (heavy) files (your favorite movie or song, an image of a cat
         etc.) from one mount-point to the other. None of the two FUSE
         processes must provoke errors. Find ways to test the case
         when files have holes as the process that wrote them seeked
         beyond the end of the file several times. Your filesystem must
         support these operations at least by making the holes explicit 
         zeros (use dd to test this aspect).

   (18)  Run some heavy testing: copy your favorite movie into your
         filesystem and try to watch it out of the filesystem.

*/
size_t min(size_t a,size_t b){
	if(a>b){
		return b;
	}
	return a;

}
size_t max(size_t a,size_t b){
	if(a<b){
		return b;
	}
	return a;

}
void print_info(void* fsptr,size_t fssize,int* errnoptr);
/* Helper types and functions */
struct FAT_ENTRY{
	unsigned short used_size;
	unsigned short is_used;
	unsigned int next_block;

};

#define FAT_SIZE sizeof(struct FAT_ENTRY)
#define HEADER_SIZE 8
#define BLOCK_SIZE 4096
#define MAX_NAME_SIZE 32
#define MAX_PATH_LEN 255

size_t get_fat_size(void *fsptr,size_t fssize,int *errnoptr){
	return (fssize-HEADER_SIZE)/(FAT_SIZE+BLOCK_SIZE);
}
enum FileType{Directory,File};
struct DirEntry{
	char file_name[MAX_NAME_SIZE];
	enum FileType file_type;
	size_t file_block;
	struct timespec atime;
	struct timespec mtime;
};
void copy_dir_entry(struct DirEntry* dest,struct DirEntry *src){
	memcpy(dest->file_name,src->file_name,MAX_NAME_SIZE);
	dest->file_type=src->file_type;
	dest->file_block=src->file_block;
	dest->atime = src->atime;
	dest->mtime = src->mtime;

}
//BLOCK LAYOUT
//
//checks if the fs is built if it is invalid then try to build it
void try_build(void *fsptr, size_t fssize,int *errnoptr){
	unsigned long* ul_fsptr = fsptr;
	if(ul_fsptr[0]==0x00000005c1f16546){
		//already setup do nothing
		return;
	}
	ul_fsptr[0]=0x00000005c1f16546;
	struct FAT_ENTRY* fat_fsptr = (struct FAT_ENTRY*) ((char*) fsptr+HEADER_SIZE);
	size_t fat_size = get_fat_size(fsptr,fssize,errnoptr);
	for(size_t i =0;i<fat_size;i++){
		fat_fsptr[i].used_size=0;
		fat_fsptr[i].is_used=0;
		fat_fsptr[i].next_block=0;
	}
	fat_fsptr[0].used_size=0;
	fat_fsptr[0].is_used=1;


}
	
struct FAT_ENTRY* get_fat(void* fsptr,size_t fssize,int *errnoptr,size_t fat_num){
	char* c_fsptr = fsptr;
	c_fsptr+=HEADER_SIZE;
	struct FAT_ENTRY* f_fsptr =(struct FAT_ENTRY*) c_fsptr;
	return f_fsptr+fat_num;
	

}
size_t get_num_free_blocks(void *fsptr,size_t fssize,int *errnoptr){
	size_t free_num=0;
	for(size_t i=0;i<get_fat_size(fsptr,fssize,errnoptr);i++){
		struct FAT_ENTRY* f = get_fat(fsptr,fssize,errnoptr,i);
		if(f->is_used==0){
			free_num++;

		}

	}
	return free_num;

}
//does not allocate anu new bytes
void* load_block(void *fsptr,size_t fssize,int* errnoptr,size_t block_num,size_t* mem_size){
	struct FAT_ENTRY* fat = get_fat(fsptr,fssize,errnoptr,block_num);
	fsptr+=HEADER_SIZE+FAT_SIZE*get_fat_size(fsptr,fssize,errnoptr);
	fsptr+=block_num*BLOCK_SIZE;
	mem_size[0]=fat->used_size;
	return fsptr;
}
//allocates block and returns the allocated block
size_t alloc_block(void *fsptr,size_t fssize,int* errnoptr){
	char* c_fsptr = fsptr;
	c_fsptr+=HEADER_SIZE;
	struct FAT_ENTRY* f_fsptr =(struct FAT_ENTRY*) c_fsptr;
	for(size_t i=0;i<get_fat_size(fsptr,fssize,errnoptr);i++){
		if(f_fsptr[i].is_used==0){
			f_fsptr[i].is_used=1;
			return i;
		}

	}
	*errnoptr=1;
	return 0;
}
void alloc_data(void *fsptr,size_t fssize,int* errnoptr,void* data,size_t data_size){
	int first=1;
	struct FAT_ENTRY* fat = NULL;
	size_t num_blocks=data_size/get_fat_size(fsptr,fssize,errnoptr);
	if(num_blocks%get_fat_size(fsptr,fssize,errnoptr)!=0){
		num_blocks++;

	}
	for(size_t i=0;i<num_blocks;i++){
		size_t block = alloc_block(fsptr,fssize,errnoptr);
		size_t block_size=0;
		void* copy_to = load_block(fsptr,fssize,errnoptr,block,&block_size);
		size_t start_index = i*BLOCK_SIZE;
		size_t end = start_index + BLOCK_SIZE-1;
		size_t copy_len = BLOCK_SIZE;
		if(end+start_index>data_size){
			copy_len=data_size-start_index;

		}
		memcpy(copy_to,data+start_index,copy_len);
		if(first){
			first=0;
		}else{
			fat->next_block=block;
			fat = get_fat(fsptr,fssize,errnoptr,block);
		}

	}

}
//frees block and following children
int free_data(void* fsptr,size_t fssize,int* errnoptr,size_t block){
	struct FAT_ENTRY* current_block;
	while(1){
		current_block=get_fat(fsptr,fssize,errnoptr,block);
		block=current_block->next_block;
		current_block->used_size=0;
		current_block->is_used=0;
		current_block->next_block=0;
		if(block==0){
			return 0;

		}
		

	}
	return -1;

}
size_t read_data(void* fsptr,size_t fssize,int* errnoptr,size_t block,size_t start,size_t read_len,void* buff){
	size_t current_block=block;
	size_t read_size=0;
	size_t write_size=0;
	while(1){
		struct FAT_ENTRY* fat =get_fat(fsptr,fssize,errnoptr,current_block);
		if(*errnoptr!=0){
			return -1;

		}
		size_t t_mem_size=0;
		char* t_data = load_block(fsptr,fssize,errnoptr,current_block,&t_mem_size);
		if(*errnoptr!=0){
			return -1;
		}
		if(t_mem_size+read_size>start){
			size_t start_index=start-read_size;
			if(start<read_size){
				start_index=0;	
			}
			size_t end_index = t_mem_size;
			if(end_index-start_index+write_size>read_len){
				end_index=read_len-read_size+start_index;
			}
			memcpy(buff+write_size,t_data+start_index,end_index-start_index);
			read_size+=end_index-start_index;
			write_size+=end_index-start_index;
			if(write_size==read_len){
				return write_size;


			}
		}else{
			read_size+=t_mem_size;

		}
		if(fat->next_block!=0){
			current_block=fat->next_block;
		}else{
			return write_size;

		}
	}
	return -1;

}
int write_byte(void* fsptr,size_t fssize,int*errnoptr,size_t start_block,size_t write_addr,unsigned char to_write){
	size_t read_size=0;
	while(1){
		struct FAT_ENTRY* fat = get_fat(fsptr,fssize,errnoptr,start_block);
		//if is in current block
		if(write_addr>=read_size && write_addr<read_size+fat->used_size){
			size_t t_mem_size;
			unsigned char* data = load_block(fsptr,fssize,errnoptr,start_block,&t_mem_size);
			if(*errnoptr!=0){
				return -1;
			}
			data[write_addr-read_size]=to_write;
			return 0;
		}else if(write_addr>=read_size && write_addr<read_size+BLOCK_SIZE){
			//if inside of block but not allocated
			size_t addr_in_block = write_addr-read_size;
			size_t t_mem_size;
			unsigned char* data = load_block(fsptr,fssize,errnoptr,start_block,&t_mem_size);
			if(*errnoptr!=0){
				return -1;
			}
			memset(data+fat->used_size,0,addr_in_block-fat->used_size);
			data[addr_in_block]=to_write;
			//updating size;
			fat->used_size=addr_in_block+1;
		}else{
			//getting next in block
			read_size+=fat->used_size;
			if(fat->next_block!=0){
				start_block=fat->next_block;
			}else{
				size_t t_block = alloc_block(fsptr,fssize,errnoptr);
				struct FAT_ENTRY *t = get_fat(fsptr,fssize,errnoptr,t_block);
				t->used_size=0;
				if(*errnoptr!=0){
					return -1;
				}
				start_block=t_block;
				fat->next_block=start_block;

			}

		}

	}

}
int write_data(void* fsptr,size_t fssize,int* errnoptr,size_t block_number,size_t start,size_t write_len,const char* to_write){
	size_t current_block = block_number;
	size_t bytes_written=0;
	size_t bytes_traversed=0;
	while(1){
		struct FAT_ENTRY* fat = get_fat(fsptr,fssize,errnoptr,current_block);
		if(*errnoptr!=0){
			return -1;
		}
		//if block is smaller then need be
		if(fat->used_size<BLOCK_SIZE){
			//then set it to be the size needed
			size_t old_used_size = fat->used_size;

			fat->used_size = min(BLOCK_SIZE,write_len+start-bytes_traversed);
			size_t temp;
			void* disk_data = load_block(fsptr,fssize,errnoptr,current_block,&temp);
			if(*errnoptr!=0){
				return -1;
			}

			memset(disk_data+old_used_size,0,fat->used_size-old_used_size);

		}
		//if write is in block
		if(bytes_traversed+fat->used_size>start){
			size_t write_start = 0;
			//if write start is not at start of block
			if(bytes_traversed<start){
				write_start=start-bytes_traversed;
			}
			//if write_start is before block
			if(bytes_traversed>start){
				write_start=0;
			}
			size_t write_end = fat->used_size;
			//if write end is not at end of block
			if(bytes_written+fat->used_size<write_len){
				write_end = start+(write_len-bytes_written);
			}
			size_t temp;
			void* disk_data = load_block(fsptr,fssize,errnoptr,current_block,&temp);
			if(*errnoptr!=0){
				return -1;
			}
			memcpy(disk_data,to_write+bytes_written,write_end-write_start);
			bytes_written+=write_end-write_start;
			if(bytes_written==write_len){
				return bytes_written;

			}
		}
		bytes_traversed+=fat->used_size;
		if(fat->next_block!=0){
			current_block=fat->next_block;
		}else{
			size_t t_block = alloc_block(fsptr,fssize,errnoptr);
			struct FAT_ENTRY *t = get_fat(fsptr,fssize,errnoptr,t_block);
			t->used_size=0;
			if(*errnoptr!=0){
				return bytes_written;
			}
			current_block=t_block;
			fat->next_block=current_block;

		}

		
	}
}
//reallocates dat to proper size
int append_data(void *fsptr,size_t fssize,int* errnoptr,size_t block,size_t append_size,void* new_data){
	struct FAT_ENTRY* current_block;
	size_t size=0;
	while(1){

		current_block = get_fat(fsptr,fssize,errnoptr,block);
		size+=current_block->used_size;
		if(current_block->next_block==0){
			//increase size
			size_t index_in_block=0;
			void* memory = load_block(fsptr,fssize,errnoptr,block,&index_in_block);
			//first copy to end of current block
			if(index_in_block<BLOCK_SIZE){	
				size_t copy_size = min(BLOCK_SIZE-index_in_block,append_size);
				memcpy(memory+index_in_block,new_data,copy_size);
				current_block->used_size=copy_size+index_in_block;
				if(append_size+index_in_block<BLOCK_SIZE-index_in_block){
					return 0;
				}
			}
			while(1){
				current_block->next_block=alloc_block(fsptr,fssize,errnoptr);
				if(append_size>BLOCK_SIZE){

					void* t_memory = load_block(fsptr,fssize,errnoptr,block,&index_in_block);
					memcpy(t_memory,new_data,BLOCK_SIZE);
					current_block=get_fat(fsptr,fssize,errnoptr,current_block->next_block);
					current_block->used_size=BLOCK_SIZE;


				}
				if(append_size==BLOCK_SIZE){

					void* t_memory = load_block(fsptr,fssize,errnoptr,block,&index_in_block);
					memcpy(t_memory,new_data,BLOCK_SIZE);
					current_block=get_fat(fsptr,fssize,errnoptr,current_block->next_block);
					current_block->next_block=0;
					current_block->used_size=BLOCK_SIZE;
					return 0;
				}
				if(append_size<BLOCK_SIZE){
					//copy only part
					void* t_memory = load_block(fsptr,fssize,errnoptr,block,&index_in_block);
					memcpy(t_memory,new_data,append_size%FAT_SIZE);
					current_block=get_fat(fsptr,fssize,errnoptr,current_block->next_block);
					current_block->used_size=append_size%FAT_SIZE;
					return 0;

				}
				//then copy to new blocks

			}
		}
	}
	return -1;
}
//loads data at block
void* load_mem(void *fsptr,size_t fssize,int *errnoptr,size_t block_number,size_t *mem_size){
	size_t current_block=block_number;
	char* data=NULL;
	*mem_size=0;
	while(1){
		struct FAT_ENTRY* fat =get_fat(fsptr,fssize,errnoptr,current_block);
		size_t t_mem_size=0;
		char* t_data = load_block(fsptr,fssize,errnoptr,current_block,&t_mem_size);
		data = realloc(data,t_mem_size+*mem_size);
		memcpy(data+*mem_size,t_data,t_mem_size);
		*mem_size+=t_mem_size;
		if(fat->next_block==0){
			break;
		}
		
		current_block=fat->next_block;
	}
	return data;
}
size_t get_size(void* fsptr,size_t fssize,int*errnoptr,size_t block_number){
	size_t current_block=block_number;
	size_t size=0;
	while(1){
		struct FAT_ENTRY* fat =get_fat(fsptr,fssize,errnoptr,current_block);
		size+=fat->used_size;
		if(fat->next_block==0){
			break;
		}
		
	}
	return size;


}
//finds dir entry at path. if none is found errno is populated as nessacary
int write_path(void *fsptr,size_t fssize,int *errnoptr,const char *path,struct DirEntry to_write){
	//hardcode Root
	if(strcmp(path,"/")==0){
		*errnoptr=ENOTDIR;
		return -1;
	}
	size_t current_block = 0;
	assert(path[0]=='/');
	struct DirEntry* dir = NULL;
	size_t dir_size=0;
	char *t_path = calloc(MAX_NAME_SIZE,1);
	if(t_path==NULL){
		*errnoptr=1;
		return -1;

	}
	strncpy(t_path,path,MAX_NAME_SIZE);
	char* fname = strtok(t_path,"/");
	dir = load_mem(fsptr,fssize,errnoptr,current_block,&dir_size);
	dir_size/=sizeof(struct DirEntry);
	while(fname!=NULL){
		int found=0;
		for(ssize_t i=0;i<dir_size;i++){
			if(strncmp(fname,dir[i].file_name,MAX_NAME_SIZE)==0){
				if(dir[i].file_type==Directory){
					current_block=dir[i].file_block;
					free(dir);
					dir = load_mem(fsptr,fssize,errnoptr,current_block,&dir_size);
					dir_size/=sizeof(struct DirEntry);
					found=1;
				}else{
					*errnoptr=ENOTDIR;
					return -1;

				}
			}
		}
		if(found==0){
			*errnoptr=ENOTDIR;
			return -1;

		}
		char* temp = strtok(NULL,"/");
		if(temp==NULL){
			break;
		}else{
			fname=temp;
		}
	}
	for(ssize_t i=0;i<dir_size;i++){
		if(strcmp(fname,dir[i].file_name)==0){
			if(dir[i].file_type==Directory){
				copy_dir_entry(dir+i,&to_write);
				return 0;
			}else{
				*errnoptr=ENOTDIR;
				return -1;

			}
		}
	}
	errnoptr[0]=ENOTDIR;
	return -1;
}
char* get_parent_path(const char* str){
	size_t last = 0;
	for(size_t i=0;str[i]!=0;i++){
		if(str[i]=='/'){
			last=i;
		}
	}
	char* data = calloc(last+1,1);
	if(last!=0){
		memcpy(data,str,last);
		data[last]=0;
	}else{
		data[0]=0;

	}
	return data;
}
char* get_child_path(const char* str){
	size_t last = 0;
	for(size_t i=0;str[i]!=0;i++){
		if(str[i]=='/'){
			last=i;
		}
	}
	size_t data_size = strlen(str)-last;
	char* data = calloc(data_size,1);
	memcpy(data,str+last+1,data_size-1);
	data[data_size-1]=0;
	return data;

}
//finds dir entry at path. if none is found errno is populated as nessacary
struct DirEntry find_path(void *fsptr,size_t fssize,int *errnoptr,const char *path){
	*errnoptr =0;
	//hardcode Root
	if(strcmp(path,"/")==0 || strcmp(path,"")==0){
		struct DirEntry root;
		root.file_type=Directory;
		root.file_block=0;
		memset(root.file_name,0,MAX_NAME_SIZE);
		return root;
	}
	size_t current_block = 0;
	struct DirEntry* dir = NULL;
	size_t dir_size=0;
	char *t_path = get_parent_path(path);
	char* fname = strtok(t_path,"/");
	char* child_name = get_child_path(path);
	if(fname==NULL){
		//loading root block
		dir = load_mem(fsptr,fssize,errnoptr,0,&dir_size);
		dir_size/=sizeof(struct DirEntry);
	}
	dir = load_mem(fsptr,fssize,errnoptr,current_block,&dir_size);
	dir_size/=sizeof(struct DirEntry);
	while(fname!=NULL){
		int found=0;
		for(ssize_t i=0;i<dir_size;i++){
			if(strcmp(fname,dir[i].file_name)==0){
				if(dir[i].file_type==Directory){
					current_block=dir[i].file_block;
					free(dir);
					dir = load_mem(fsptr,fssize,errnoptr,current_block,&dir_size);
					dir_size/=sizeof(struct DirEntry);
					found=1;
				}else{
					*errnoptr=ENOTDIR;
					struct DirEntry t;
					t.file_block=0;
					free(t_path);
					free(child_name);
					
					return t;

				}
			}
		}
		if(found==0){
			*errnoptr=ENOTDIR;
			struct DirEntry t;
			t.file_block=0;
			free(fname);
			free(t_path);
			return t;

		}
		char* temp = strtok(NULL,"/");
		if(temp==NULL){
			break;
		}else{
			fname=temp;
		}
	}
	for(ssize_t i=0;i<dir_size;i++){
		if(strcmp(child_name,dir[i].file_name)==0){
			if(dir[i].file_type==Directory){
				struct DirEntry ret = dir[i];
				free(dir);
				free(t_path);
				free(child_name);

				return ret;
			}else{
				struct DirEntry ret = dir[i];
				free(dir);
				free(t_path);
				free(child_name);
				return ret;

			}
		}
	}
	errnoptr[0]=ENOENT;
	struct DirEntry t;
	t.file_block=0;
	free(t_path);
	free(child_name);
	return t;
}
//removes data
void remove_data(void* fsptr,size_t fssize,int* errnoptr,size_t* block,size_t start_index,size_t size_remove){
	size_t size_read = 0;

	void* data = load_mem(fsptr,fssize,errnoptr,*block,&size_read);
	if(*errnoptr!=0){
		return;

	}
	//moving data
	if(size_read<start_index+size_remove){
		*errnoptr=-1;
		return;

	}
	memmove(data+start_index,data+start_index+size_remove,size_read-(start_index+size_remove));
	free_data(fsptr,fssize,errnoptr,*block);
	if(*errnoptr!=0){
		return;

	}
	struct FAT_ENTRY *fat = get_fat(fsptr,fssize,errnoptr,*block);
	if(*errnoptr!=0){
		return;

	}
	fat->is_used=1;
	write_data(fsptr,fssize,errnoptr,*block,0,size_read-size_remove,data);
	if(*errnoptr!=0){
		return;

	}
	
	free(data);
	return;

	
	

}

void print_info(void* fsptr,size_t fssize,int* errnoptr){
	printf("block size: %i num_blocks: %li free blocks: %li\n",BLOCK_SIZE,get_fat_size(fsptr,fssize,errnoptr),get_num_free_blocks(fsptr,fssize,errnoptr));

}


/* End of helper functions */


/* Implements an emulation of the stat system call on the filesystem 
   of size fssize pointed to by fsptr. 
   
   If path can be followed and describes a file or directory 
   that exists and is accessable, the access information is 
   put into stbuf. 

   On success, 0 is returned. On failure, -1 is returned and 
   the appropriate error code is put into *errnoptr.

   man 2 stat documents all possible error codes and gives more detail
   on what fields of stbuf need to be filled in. Essentially, only the
   following fields need to be supported:

   st_uid      the value passed in argument
   st_gid      the value passed in argument
   st_mode     (as fixed values S_IFDIR | 0755 for directories,
                                S_IFREG | 0755 for files)
   st_nlink    (as many as there are subdirectories (not files) for directories
                (including . and ..),
                1 for files)
   st_size     (supported only for files, where it is the real file size)
   st_atim
   st_mtim

*/
int __myfs_getattr_implem(void *fsptr, size_t fssize, int *errnoptr,
                          uid_t uid, gid_t gid,
                          const char *path, struct stat *stbuf) {
	*errnoptr=0;
	try_build(fsptr,fssize,errnoptr);
	if(*errnoptr!=0){
		return -1;
	}
	struct DirEntry f = find_path(fsptr,fssize,errnoptr,path);
	if(*errnoptr!=0){
		return -1;
	}
	if(f.file_type==Directory){
		size_t len = 0;
		free(load_mem(fsptr,fssize,errnoptr,f.file_block,&len));
		stbuf->st_nlink=len/sizeof(struct DirEntry);
		stbuf->st_nlink+=2;
		stbuf->st_mode=S_IFDIR | 0755;
	}
	if(f.file_type==File){
		size_t len = 0;
		free(load_mem(fsptr,fssize,errnoptr,f.file_block,&len));
		stbuf->st_size=len;
		stbuf->st_mode=S_IFREG | 0755;
	}
	stbuf->st_atim = f.atime;
	stbuf->st_mtim = f.mtime;
	stbuf->st_uid=uid;
	stbuf->st_gid=gid;

	return 0;
}

/* Implements an emulation of the readdir system call on the filesystem 
   of size fssize pointed to by fsptr. 

   If path can be followed and describes a directory that exists and
   is accessable, the names of the subdirectories and files 
   contained in that directory are output into *namesptr. The . and ..
   directories must not be included in that listing.

   If it needs to output file and subdirectory names, the function
   starts by allocating (with calloc) an array of pointers to
   characters of the right size (n entries for n names). Sets
   *namesptr to that pointer. It then goes over all entries
   in that array and allocates, for each of them an array of
   characters of the right size (to hold the i-th name, together 
   with the appropriate '\0' terminator). It puts the pointer
   into that i-th array entry and fills the allocated array
   of characters with the appropriate name. The calling function
   will call free on each of the entries of *namesptr and 
   on *namesptr.

   The function returns the number of names that have been 
   put into namesptr. 

   If no name needs to be reported because the directory does
   not contain any file or subdirectory besides . and .., 0 is 
   returned and no allocation takes place.

   On failure, -1 is returned and the *errnoptr is set to 
   the appropriate error code. 

   The error codes are documented in man 2 readdir.

   In the case memory allocation with malloc/calloc fails, failure is
   indicated by returning -1 and setting *errnoptr to EINVAL.

*/
int __myfs_readdir_implem(void *fsptr, size_t fssize, int *errnoptr,
                          const char *path, char ***namesptr) {
	*errnoptr=0;
	struct DirEntry d = find_path(fsptr,fssize,errnoptr,path);
	if(*errnoptr!=0){
		return -1;

	}
	if(d.file_type!=Directory){
		*errnoptr=ENOTDIR;
		return -1;
	}

	size_t memory_size=0;
	struct DirEntry* dirs = load_mem(fsptr,fssize,errnoptr,d.file_block,&memory_size);	
	if(*errnoptr!=0){
		return -1;
	}
	namesptr[0]=calloc(memory_size/sizeof(struct DirEntry)+2,sizeof(char*));


	for(size_t i=0;i<memory_size/sizeof(struct DirEntry);i++){
		namesptr[0][i]=calloc(strlen(dirs[i].file_name)+1,sizeof(char));
		strcpy(namesptr[0][i],dirs[i].file_name);

	}
	int out = memory_size/sizeof(struct DirEntry);
	return out;
}

/* Implements an emulation of the mknod system call for regular files
   on the filesystem of size fssize pointed to by fsptr.

   This function is called only for the creation of regular files.

   If a file gets created, it is of size zero and has default
   ownership and mode bits.

   The call creates the file indicated by path.

   On success, 0 is returned.

   On failure, -1 is returned and *errnoptr is set appropriately.

   The error codes are documented in man 2 mknod.

*/
int __myfs_mknod_implem(void *fsptr, size_t fssize, int *errnoptr,
                        const char *path) {
	*errnoptr=0;
	try_build(fsptr,fssize,errnoptr);
	if(*errnoptr!=0){
		return -1;
	}
	//1. strip off last part of path
	//a. finding last /
	size_t last = strrchr(path,'/')-path;
	char* t_path = get_parent_path(path);
	struct DirEntry f = find_path(fsptr,fssize,errnoptr,t_path);
	if(*errnoptr!=0){
		return -1;
	}
	//Next build new dir entry
	size_t next_dir_len = strlen(path)-1-last;
	if(next_dir_len>MAX_NAME_SIZE){
		*errnoptr=ENAMETOOLONG;
		return -1;

	}
	
	struct DirEntry new_f;
	char* new_name = get_child_path(path);
	memcpy(new_f.file_name,new_name,strlen(new_name)+1);
	new_f.file_block=alloc_block(fsptr,fssize,errnoptr);
	new_f.file_type=File;
	free(new_name);


	
	//Finally append
	append_data(fsptr,fssize,errnoptr,f.file_block,sizeof(struct DirEntry),&new_f);
	return 0;
}

/* Implements an emulation of the unlink system call for regular files
   on the filesystem of size fssize pointed to by fsptr.

   This function is called only for the deletion of regular files.

   On success, 0 is returned.

   On failure, -1 is returned and *errnoptr is set appropriately.

   The error codes are documented in man 2 unlink.

*/
int __myfs_unlink_implem(void *fsptr, size_t fssize, int *errnoptr,
                        const char *path) {
	//a. finding last /
	char* t_path = get_parent_path(path);
	struct DirEntry f= find_path(fsptr,fssize,errnoptr,path);
	
	struct DirEntry parent= find_path(fsptr,fssize,errnoptr,t_path);
	size_t dirs_size;
	struct DirEntry* dirs = load_mem(fsptr,fssize,errnoptr,parent.file_block,&dirs_size);
	for(size_t i=0;i<dirs_size/sizeof(struct DirEntry);i++){
		if(strcmp(f.file_name,dirs[i].file_name)==0){
			if(dirs[i].file_type!=File){
				*errnoptr=EISDIR;
				return -1;
			}
			free_data(fsptr,fssize,errnoptr,dirs[i].file_block);
			remove_data(fsptr,fssize,errnoptr,&parent.file_block,i*sizeof(struct DirEntry),sizeof(struct DirEntry));
			free(t_path);
			return 0;


		}
	}

	free(t_path);
	*errnoptr=ENOENT;
  	return -1;
}

/* Implements an emulation of the rmdir system call on the filesystem 
   of size fssize pointed to by fsptr. 

   The call deletes the directory indicated by path.

   On success, 0 is returned.

   On failure, -1 is returned and *errnoptr is set appropriately.

   The function call must fail when the directory indicated by path is
   not empty (if there are files or subdirectories other than . and ..).

   The error codes are documented in man 2 rmdir.

*/
int __myfs_rmdir_implem(void *fsptr, size_t fssize, int *errnoptr,
                        const char *path) {
	char* t_path = get_parent_path(path);
	struct DirEntry f= find_path(fsptr,fssize,errnoptr,path);
	
	struct DirEntry parent= find_path(fsptr,fssize,errnoptr,t_path);
	size_t dirs_size;
	struct DirEntry* dirs = load_mem(fsptr,fssize,errnoptr,parent.file_block,&dirs_size);
	for(size_t i=0;i<dirs_size/sizeof(struct DirEntry);i++){
		if(strcmp(f.file_name,dirs[i].file_name)==0){
			if(dirs[i].file_type!=Directory){
				*errnoptr=ENOTDIR;
				return -1;
			}
			free_data(fsptr,fssize,errnoptr,dirs[i].file_block);
			remove_data(fsptr,fssize,errnoptr,&parent.file_block,i*sizeof(struct DirEntry),sizeof(struct DirEntry));
			free(t_path);
			return 0;


		}
	}

	free(t_path);
	*errnoptr=ENOENT;
  	return -1;
}

/* Implements an emulation of the mkdir system call on the filesystem 
   of size fssize pointed to by fsptr. 

   The call creates the directory indicated by path.

   On success, 0 is returned.

   On failure, -1 is returned and *errnoptr is set appropriately.

   The error codes are documented in man 2 mkdir.

*/
int __myfs_mkdir_implem(void *fsptr, size_t fssize, int *errnoptr,
                        const char *path) {
  /* STUB */
	*errnoptr=0;
	try_build(fsptr,fssize,errnoptr);
	if(*errnoptr!=0){
		return -1;
	}
	//1. strip off last part of path
	//a. finding last /
	size_t last = strrchr(path,'/')-path;
	char* t_path = get_parent_path(path);
	struct DirEntry f = find_path(fsptr,fssize,errnoptr,t_path);
	if(*errnoptr!=0){
		return -1;
	}
	//Next build new dir entry
	size_t next_dir_len = strlen(path)-1-last;
	if(next_dir_len>MAX_NAME_SIZE){
		*errnoptr=ENAMETOOLONG;
		return -1;

	}
	
	struct DirEntry new_f;
	char* new_name = get_child_path(path);
	memcpy(new_f.file_name,new_name,strlen(new_name)+1);
	new_f.file_block=alloc_block(fsptr,fssize,errnoptr);
	new_f.file_type=Directory;
	free(new_name);


	
	//Finally append
	append_data(fsptr,fssize,errnoptr,f.file_block,sizeof(struct DirEntry),&new_f);
	return 0;

}

/* Implements an emulation of the rename system call on the filesystem 
   of size fssize pointed to by fsptr. 

   The call moves the file or directory indicated by from to to.

   On success, 0 is returned.

   On failure, -1 is returned and *errnoptr is set appropriately.

   Caution: the function does more than what is hinted to by its name.
   In cases the from and to paths differ, the file is moved out of 
   the from path and added to the to path.

   The error codes are documented in man 2 rename.

*/
int __myfs_rename_implem(void *fsptr, size_t fssize, int *errnoptr,
                         const char *from, const char *to) {
	char* t_path = get_parent_path(from);
	struct DirEntry f= find_path(fsptr,fssize,errnoptr,from);
	
	struct DirEntry parent= find_path(fsptr,fssize,errnoptr,t_path);
	size_t dirs_size;
	struct DirEntry* dirs = load_mem(fsptr,fssize,errnoptr,parent.file_block,&dirs_size);
	struct DirEntry file;

	int found=0;
	for(size_t i=0;i<dirs_size/sizeof(struct DirEntry);i++){
		if(strcmp(f.file_name,dirs[i].file_name)==0){

			remove_data(fsptr,fssize,errnoptr,&parent.file_block,i*sizeof(struct DirEntry),sizeof(struct DirEntry));
			copy_dir_entry(&file,dirs+i);
			char* to_name = get_child_path(to);

			strncpy(file.file_name,to_name,MAX_NAME_SIZE);
			free(to_name);
			found=1;
			break;


		}
	}
	if(found==0){
		free(t_path);
		free(dirs);
		*errnoptr=EBADF;
	
	}
	char* to_path = get_parent_path(to);
	struct DirEntry new_parent= find_path(fsptr,fssize,errnoptr,to_path);
	
	append_data(fsptr,fssize,errnoptr,new_parent.file_block,sizeof(struct DirEntry),&file);
	free(to_path);
	return 0;
}

/* Implements an emulation of the truncate system call on the filesystem 
   of size fssize pointed to by fsptr. 

   The call changes the size of the file indicated by path to offset
   bytes.

   When the file becomes smaller due to the call, the extending bytes are
   removed. When it becomes larger, zeros are appended.

   On success, 0 is returned.

   On failure, -1 is returned and *errnoptr is set appropriately.

   The error codes are documented in man 2 truncate.

*/
int __myfs_truncate_implem(void *fsptr, size_t fssize, int *errnoptr,
                           const char *path, off_t offset) {

	struct DirEntry f= find_path(fsptr,fssize,errnoptr,path);
	size_t size=get_size(fsptr,fssize,errnoptr,f.file_block);
	if(offset>size){
		append_data(fsptr,fssize,errnoptr,f.file_block,offset-size,calloc(offset-size,1));
	}else{
		remove_data(fsptr,fssize,errnoptr,&f.file_block,offset,size-offset);
	}
	return 0;
}

/* Implements an emulation of the open system call on the filesystem 
   of size fssize pointed to by fsptr, without actually performing the opening
   of the file (no file descriptor is returned).

   The call just checks if the file (or directory) indicated by path
   can be accessed, i.e. if the path can be followed to an existing
   object for which the access rights are granted.

   On success, 0 is returned.

   On failure, -1 is returned and *errnoptr is set appropriately.

   The two only interesting error codes are 

   * EFAULT: the filesystem is in a bad state, we can't do anything

   * ENOENT: the file that we are supposed to open doesn't exist (or a
             subpath).

   It is possible to restrict ourselves to only these two error
   conditions. It is also possible to implement more detailed error
   condition answers.

   The error codes are documented in man 2 open.

*/
int __myfs_open_implem(void *fsptr, size_t fssize, int *errnoptr,
                       const char *path) {

	*errnoptr=0;
	struct DirEntry f= find_path(fsptr,fssize,errnoptr,path);
	if(*errnoptr!=0){
		*errnoptr=ENOENT;
		return -1;
	}
	if(f.file_type!=File){
		*errnoptr=EISDIR;
		return -1;
	}
	return 0;
}

/* Implements an emulation of the read system call on the filesystem 
   of size fssize pointed to by fsptr.

   The call copies up to size bytes from the file indicated by 
   path into the buffer, starting to read at offset. See the man page
   for read for the details when offset is beyond the end of the file etc.
   
   On success, the appropriate number of bytes read into the buffer is
   returned. The value zero is returned on an end-of-file condition.

   On failure, -1 is returned and *errnoptr is set appropriately.

   The error codes are documented in man 2 read.

*/
int __myfs_read_implem(void *fsptr, size_t fssize, int *errnoptr,
                       const char *path, char *buf, size_t size, off_t offset) {
	*errnoptr=0;
	struct DirEntry f= find_path(fsptr,fssize,errnoptr,path);
	if(f.file_type==Directory){
		*errnoptr=EINVAL;
		return -1;

	}
	if(*errnoptr!=0){
		return -1;
	}
	size= read_data(fsptr,fssize,errnoptr,f.file_block,offset,size,buf);
	if(*errnoptr!=0){
		return -1;
	}
	return size;
}

/* Implements an emulation of the write system call on the filesystem 
   of size fssize pointed to by fsptr.

   The call copies up to size bytes to the file indicated by 
   path into the buffer, starting to write at offset. See the man page
   for write for the details when offset is beyond the end of the file etc.
   
   On success, the appropriate number of bytes written into the file is
   returned. The value zero is returned on an end-of-file condition.

   On failure, -1 is returned and *errnoptr is set appropriately.

   The error codes are documented in man 2 write.

*/
int __myfs_write_implem(void *fsptr, size_t fssize, int *errnoptr,
                        const char *path, const char *buf, size_t size, off_t offset) {
	struct DirEntry f= find_path(fsptr,fssize,errnoptr,path);
	if(*errnoptr!=0){
		return -1;
	}

	write_data(fsptr,fssize,errnoptr,f.file_block,offset,size,buf);
	if(*errnoptr!=0){
		return -1;
	}
	return size;
}

/* Implements an emulation of the utimensat system call on the filesystem 
   of size fssize pointed to by fsptr.

   The call changes the access and modification times of the file
   or directory indicated by path to the values in ts.

   On success, 0 is returned.

   On failure, -1 is returned and *errnoptr is set appropriately.

   The error codes are documented in man 2 utimensat.

*/
int __myfs_utimens_implem(void *fsptr, size_t fssize, int *errnoptr,
                          const char *path, const struct timespec ts[2]) {
  /* STUB */
	struct DirEntry f= find_path(fsptr,fssize,errnoptr,path);
	f.atime=ts[0];
	f.mtime=ts[1];
	write_path(fsptr,fssize,errnoptr,path,f);

  return 0;
}

/* Implements an emulation of the statfs system call on the filesystem 
   of size fssize pointed to by fsptr.

   The call gets information of the filesystem usage and puts in 
   into stbuf.

   On success, 0 is returned.

   On failure, -1 is returned and *errnoptr is set appropriately.

   The error codes are documented in man 2 statfs.

   Essentially, only the following fields of struct statvfs need to be
   supported:

   f_bsize   fill with what you call a block (typically 1024 bytes)
   f_blocks  fill with the total number of blocks in the filesystem
   f_bfree   fill with the free number of blocks in the filesystem
   f_bavail  fill with same value as f_bfree
   f_namemax fill with your maximum file/directory name, if your
             filesystem has such a maximum

*/
int __myfs_statfs_implem(void *fsptr, size_t fssize, int *errnoptr,
                         struct statvfs* stbuf) {
	errnoptr=0;
	try_build(fsptr,fssize,errnoptr);
	stbuf->f_bsize=BLOCK_SIZE;
	stbuf->f_blocks=get_fat_size(fsptr,fssize,errnoptr);
	if(errnoptr!=0){
		return -1;
	}
	stbuf->f_bfree=get_num_free_blocks(fsptr,fssize,errnoptr);
	if(errnoptr!=0){
		printf("statfs failed\n");
		return -1;
	}
	stbuf->f_bavail = stbuf->f_bfree;
	stbuf->f_namemax=MAX_NAME_SIZE;
	return 0;
}

