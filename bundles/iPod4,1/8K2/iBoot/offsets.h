/**
  * GreenPois0n Cynanide - iPod4,1/8B117/iBoot/offsets.h
  * Copyright (C) 2010 Chronic-Dev Team
  * Copyright (C) 2010 Joshua Hill
  * Copyright (C) 2010 Nicolas Haunold
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

#ifndef OFFSETS_H
#define OFFSETS_H

// 4.3.4/8K2

// Base address
#define TARGET_BASEADDR                   (0x5FF00000)

// Task functions
#define TARGET_TASK_RUNNING               (0x347C0) 
#define TARGET_TASK_LIST                  (0x3494C)

// BDev offsets
#define TARGET_BDEV_LIST                  (0x38B00)

// Filesystem offsets
#define TARGET_FS_MOUNT                   (0x1A0B4+1)
#define TARGET_FS_UNMOUNT                 (0x1A2CC+1)
#define TARGET_FS_LOAD_FILE               (0x1A264+1)

// Kernel offsets
#define TARGET_KERNEL_PHYMEM              (0x39BF0)

#endif // OFFSETS_H
