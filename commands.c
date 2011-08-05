/**
  * GreenPois0n Cynanide - commands.c
  * Copyright (C) 2010 Chronic-Dev Team
  * Copyright (C) 2010 Joshua Hill
  * Copyright (C) 2010 Cyril Cattiaux
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

//This is for the pie :)
#include <ctype.h>

#include "image.h"
#include "lock.h"
#include "common.h"
#include "commands.h"
#include "functions.h"
#include "coprocessor.h"

int gCmdCount = 0;
Bool gCmdHasInit = FALSE;

CmdInfo** gCmdCommands = NULL;

unsigned char* gCmdListEnd = NULL;
unsigned char* gCmdListBegin = NULL;
int(*jump_to)(int flags, void* addr, int phymem) = NULL;

/*
 * Private Functions
 */

void* find_cmd_list_begin() {
#ifdef TARGET_CMD_LIST_BEGIN 
return (void*) TARGET_CMD_LIST_BEGIN;
#endif	
	unsigned int reference = find_reference(TARGET_BASEADDR, TARGET_BASEADDR, 0x40000, "save current environment to flash");
	if(reference == 0) {
		printf("Unable to find saveenv description reference\n");
		return 0;
	}

	int i = 0;
	for(i = 0; i < 0x80; i += 4) {
		unsigned int* command = reference-i;
		if(*command == NULL) {
			return command+1;
		}
	}
	return 0;
}

void* find_cmd_list_end() {
#ifdef TARGET_CMD_LIST_END
return (void*) TARGET_CMD_LIST_END;
#endif
	int i = 0;
	if(gCmdListBegin != NULL) {
		for(i = 0; i < 0x20; i++) {
			unsigned int* value = (unsigned int*) gCmdListBegin+i;
			if(!strncmp((const char*) *value, "auto-boot", 0x10) || *value == NULL) {
				return value;
			}
		}
	}
	return 0;
}

void* find_jump_to() {
#ifdef TARGET_JUMP_TO
return (void*) TARGET_JUMP_TO;
#endif
	void* bytes = NULL;
	if(strstr((char*) (TARGET_BASEADDR + 0x200), "n72ap")) {
		bytes = patch_find(TARGET_BASEADDR, 0x40000, "\xf0\xb5\x03\xaf\x04\x1c\x15\x1c", 8);
		bytes++;
	} else {
		bytes = patch_find(TARGET_BASEADDR, 0x40000, "\x80\xb5\x00\xaf\x04\x46\x15\x46", 8);
		bytes++;
	}
	return bytes;
}

int cmd_init() {
	if(gCmdHasInit) return 0;

	int i = 0;
	gCmdCount = 0;
	gCmdHasInit = TRUE;
	gCmdCommands = (CmdInfo**) (LOADADDR + 0x01800000);

	// add our essential commands
	cmd_add("help", &cmd_help, "display all available commands");
	cmd_add("detect", &cmd_detect, "Detect if factory or iTunes restored.");
	cmd_add("pie", &cmd_pie, "The pie.");
	jump_to = find_jump_to();
	if(jump_to == NULL) {
		
	} else {
		cmd_add("jump", &cmd_jump, "shutdown current image and jump into another");
	}
	return 0;
}

void cmd_add(char* name, CmdFunction handler, char* description) {
	CmdInfo* command = NULL;
	if(gCmdCount >= MAX_COMMANDS) {
		puts("Maximum Commands Reached\n");
		return;
	}

	//command = (CmdInfo*) malloc(sizeof(CmdInfo));
	command = (CmdInfo*) (LOADADDR + 0x01800000) + (gCmdCount * sizeof(CmdInfo));
	command->name = name;
	command->handler = handler;
	command->description = description;
	gCmdCommands[gCmdCount] = command;
	gCmdCount++;
}

int cmd_get_arg(CmdArg arg) {
	if (arg.type==CMDARG_TYPE_INTEGER) {
		return arg.uinteger;
	} else {
		char* ret = arg.string;
		int len = strlen(ret);
		// strip leading and trailing quotes if any
		if (len > 0 && (ret[len-1]=='"' || ret[len-1]=='\'')) ret[len-1]=0;
		if (ret[0]=='"' || ret[0]=='\'') ret = &ret[1];
		return (int) ret;
	}
}

void cmd_start() {
	printf_begin();
}

/*
 * Public Functions
 */

int cmd_detect() {
	//Load Apple Logo to Load Addresses.
	image_load(0x6C6F676F, (LOADADDR+2000000), 0x100000);
	int i = 0;
	char buffer[64];
	char* bytes = "50726F642D466163746F7279";
	char* source = (LOADADDR+2000000);
	unsigned int size = 0x10000;
	unsigned int byte = 0;

	int length = strlen(bytes) / 2;
	memset(buffer, '\0', 64);
	for(i = 0; i < length; i++) {
		sscanf(bytes, "%02x", &byte);
		buffer[i] = byte;
		bytes += 2;
	}

	for(i = 0; i < size; i++) {
		if(!memcmp(&source[i], buffer, length)) {
			// WE ARE FACTORY!!!!
			puts("* Device is Factory-restored.\n");
			puts("=========================\n");
			nvram_set_var("auto-boot","factory");
			return 0;		
			}
	}
	puts("* Device is iTunes-restored.\n");
	puts("=========================\n");
	nvram_set_var("auto-boot","itunes");
	return 0;
}

int cmd_help(int argc, CmdArg* argv) {
	int i = 0;
	enter_critical_section();
	printf("Commands:\n");
	for(i = 0; i < gCmdCount; i++) {
		printf("  %s \t", gCmdCommands[i]->name);
		if(strlen(gCmdCommands[i]->name) < 5) printf(" \t");
		printf("%s\n", gCmdCommands[i]->description);
	}
	printf("\n");
	exit_critical_section();

	return 0;
}

int cmd_jump(int argc, CmdArg* argv) {
	int i = 0;
	void* address = NULL;
	if(argc < 2) {
		puts("usage: jump <address>\n");
		return 0;
	}
	if(argc == 2) {
		address = (void*) argv[1].uinteger;
		jump_to(0, address, 0);
	}
	if(argc == 4) {
		address = (void*) argv[1].uinteger;
		jump_to(argv[2].uinteger, argv[2].uinteger, argv[2].uinteger);
	}

	return 0;
}

int cmd_pie(int argc, CmdArg* argv) {
if(argc >= 3) {
int i = 0;
for(i = 1; i < argc; i++) {
	enter_critical_section();
	hexdump_pie(argv[1].uinteger,argv[2].uinteger);
	exit_critical_section();
}
return 0;
}
puts("\nusage: pie <address> <length>\n\n");
}

unsigned char* patch_find(unsigned char* start, int length, unsigned char* find, int size) {
	int i = 0;
	for(i = 0; i < length; i++) {
		if(!memcmp(&start[i], find, size)) {
			return &start[i];
		}
	}
	return NULL;
}

void hexdump_pie(unsigned char* buf, unsigned int len) {
  char* blah = (char*)malloc(len * 2 + 1);
  int i;
  for (i = 0; i < len; i++) {
    sprintf(blah + i * 2, "%02X", buf[i]);
  }
	nvram_set_var("auto-boot",blah);
}

void clear_icache() {
    __asm__("mov r0, #0");
    __asm__("mcr p15, 0, r0, c7, c5, 0");
    __asm__("mcr p15, 0, r0, c7, c5, 4");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
};

