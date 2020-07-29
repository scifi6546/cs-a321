# Design
The filesystem consists of three portions the first is a magic number. Next there is the file allocation table that is an 
array of structs. Finally there is an array of blocks. The magic number is 8 bytes long and it is the number 0x00000005c1f16546.
The fat entries contain the following fields: the number of bytes used in the block, the next block and a 8 bit integer indicating if
the file is used. Files and folders are stored as a linked list of blocks and the next block entry of the file allocation table entry indactes
the next child block. 
# Diffiulties
I had a lot of difficulty debugging my file system and making sure that it does not corrupt itself.
# Testing
I tested the code by attempting to use the file system in a normal way. When some strange behivor was found it was noted and it was duplicated with simpler actions inoder to find out exactly what broke.
