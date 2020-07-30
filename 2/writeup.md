# Design
The class filesystem uses blocks as its basic unit of data store. Information about the blocks is stored in a file allocation table in the beginning of the filesystem, right after the magic number 0x00000005c1f16546. Files and directories larger then a block (4096 bytes) are stored as a singly linked list of blocks. A blocks used capacity and next block is stored in the file allocation table as the used_size and next_block fields.
## File System layout
A directory listing is stored in the same manner as a regular file, as a linked list of blocks. A directory listing contains an array of __myfs_dir_entry_struct_t structs. The __myfs_dir_entry_struct_t structs contains all of the metadata for the file or directory linked. The __myfs_dir_entry_struct_t contains the index of the block that the file data resides in. The root directory is located in the 0th block.
## Algorythm for loading files


The filesystem consists of three portions the first is a magic number. Next there is the file allocation table that is an 
array of structs. Finally there is an array of blocks. The magic number is 8 bytes long and it is the number 0x00000005c1f16546.
The fat entries contain the following fields: the number of bytes used in the block, the next block and a 8 bit integer indicating if
the file is used. Files and folders are stored as a linked list of blocks and the next block entry of the file allocation table entry indactes
the next child block. 
# Diffiulties
I had a lot of difficulty debugging my file system and making sure that it does not corrupt itself.
# Testing
I tested the code by attempting to use the file system in a normal way. When some strange behivor was found it was noted and it was duplicated with simpler actions inoder to find out exactly what broke.
