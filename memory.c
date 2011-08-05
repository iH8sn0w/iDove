/**
  * GreenPois0n Cynanide - memory.c
  * Copyright (C) 2010 Chronic-Dev Team
  * Copyright (C) 2010 Joshua Hill
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sha.h"
#include "lock.h"
#include "memory.h"
#include "common.h"
#include "commands.h"

int memory_init() {
	//printf("Initializing memory\n");
	cmd_add("memory", &memory_cmd, "display, search, and manipulate memory");
	return 0;
}

int memory_cmd(int argc, CmdArg* argv) {
	char* bytes = NULL;
	char* action = NULL;
	char* source = NULL;
	char* storage = NULL;
	char* destination = NULL;
	unsigned int size = 0;

	if(argc < 4) {
		puts("usage: memory <search/dump/copy> [options]\n");
		puts("  search <address> <size> <bytes>\tfind address of specified byte sequence\n");
		puts("  copy <from> <to> <size>        \tcopy memory from one address to another\n");
		puts("  move <from> <to> <size>        \tmove memory from one address to another\n");
		puts("  sha1 <from> <to> <address>     \tsha1 memory from one address to another\n");
		return 0;
	}

	action = argv[1].string;
	if(argc == 5) {
		if(!strcmp(action, "search")) {
			bytes = argv[4].string;
			size = argv[3].uinteger;
			source = (void*) argv[2].uinteger;
			return memory_search(source, size, bytes);
		}
	}


	if(argc == 5) {
		if(!strcmp(action, "copy")) {
			size = argv[4].uinteger;
			source = (char*) argv[2].uinteger;
			destination = (char*) argv[3].uinteger;
			return memcpy(destination, source, size);
		}
	}

	if(argc == 5) {
		if(!strcmp(action, "move")) {
			size = argv[4].uinteger;
			source = (char*) argv[2].uinteger;
			destination = (char*) argv[3].uinteger;
			return memmove(destination, source, size);
		}
	}

	if(argc == 5) { 
		if(!strcmp(action, "sha1")) {
			source = (char*) argv[2].uinteger;
			destination = (char*) argv[3].uinteger;
			storage = (char*) argv[4].uinteger;
			return memory_sha1(source, destination, storage);
		}
	}

	return 0;
}

int memory_sha1(char* source,char* destination,char* storage) {
int meha = source; // top of shsh
int mehb = destination; //end of shsh
dohexdump(source, mehb - meha,storage);
}

inline int sha_endian_flip(unsigned int x)
{
return (x>>24) | ((x<<8) & 0x00FF0000) | ((x>>8) & 0x0000FF00) | (x<<24);
}


int shait(char* text, char* address)
{
int i;
static SHA1Context sha; 
SHA1Reset(&sha);
SHA1Input(&sha, (const unsigned char *) text, strlen(text));
SHA1Result(&sha);
int* addr= address;
for(i = 0; i < 5 ; i++)
{
//printf("%08X", sha.Message_Digest[i]);
*addr= sha_endian_flip(sha.Message_Digest[i]);
addr++;
}
}

void dohexdump(unsigned char* buf, unsigned int len, char* address) {
  char* blah1 = (char*)malloc(len * 2 + 1);
  int i;
  for (i = 0; i < len; i++) {
    sprintf(blah1 + i * 2, "%02X", buf[i]);
  }
	shait(blah1,address);
	return blah1;
}

int memory_search(char* source, unsigned int size, char* bytes) {
	int i = 0;
	char buffer[64];
	unsigned int byte = 0;

	int length = strlen(bytes) / 2;
	if(length <= 0 || length >= 64) {
		printf("byte sequence is too long\n");
		return -1;
	}

	memset(buffer, '\0', 64);
	for(i = 0; i < length; i++) {
		sscanf(bytes, "%02x", &byte);
		buffer[i] = byte;
		bytes += 2;
	}

	for(i = 0; i < size; i++) {
		if(!memcmp(&source[i], buffer, length)) {
			char* chair = (char*)malloc(length);
			char* chair2 = (char*)malloc(length);
			printf("Byte Sequence is at: 0x%X\n", &source[i]);
			sprintf(chair, "0x%X", &source[i]);
			nvram_set_var("auto-boot",chair);
			return (int) &source[i];
		}
	}
	puts("unable to find byte sequence\n");
	return -1;
}
