/*

The "file system" consists of a list of contiguous blocks in the flash memory.

Structure of leading page in a file:
<LEN-LL><LEN-LH><LEN-HL><LEN-HH> - four bytes showing the total length of the file in bytes (including the file header)
<12:name>       - 12 bytes allocated for zero-terminated name (up to 11 effective bytes in a file name)
<nnnn:data>     - the rest of the leading block is filled with actual file data

A leading page may be followed by other pages as indicated in the first two bytes.

*/

#ifndef FLASH_H
#define FLASH_H

#include "../common.h"
#include "hardware.h"

// the following parameters and functions are hardware-dependent
// ==================================================================================================

#define FLASH_SIZE_SECTORS      75          // total size of the available flash in number of erasable sectors

#define FLASH_WRITABLE_BYTES    256         // N of bytes in a writable page
#define FLASH_ERASABLE_BYTES    256         // N of bytes in an erasable sector

#define FLASH_START_ADDRESS     (_ROMSIZE - (FLASH_SIZE_SECTORS * FLASH_ERASABLE_BYTES))    // start address of the first sector in the available flash memory

// counting of the writable pages and erasable sectors starts from zero

unsigned char flash_readByte(long page, long offset);
void flash_readPage(long page, unsigned char *buffer);
void flash_writePage(long page, unsigned char *buffer);
void flash_erasePage(long page);

// ==================================================================================================

#define FLASH_ERASABLE_SECTOR   ((FLASH_ERASABLE_BYTES) / (FLASH_WRITABLE_BYTES))   // N of writable pages in erasable sector
#define FLASH_NUMBER_PAGES      ((FLASH_SIZE_SECTORS) * (FLASH_ERASABLE_SECTOR))    // N of writable pages in the flash

// flash storage array
//__at(FLASH_START_ADDRESS)
//const uint8_t STORAGE[FLASH_SIZE_SECTORS][FLASH_ERASABLE_BYTES] =
//        { [0 ... ((FLASH_SIZE_SECTORS) - 1)][0 ... ((FLASH_ERASABLE_BYTES) - 1)] = 0xFF };

//#define flash_readByte(p, offset)   *(FLASH_START_ADDRESS + ((long) (p) * FLASH_WRITABLE_BYTES) + (offset))

#define FLASH_FILE_HEADER       16      // size of file header - must not be greater than (FLASH_WRITABLE_BYTES)

// convert file length in bytes into number of writable pages needed for it
long flash_b2p(long len);

// create a new file and write data into it or append to an existing file
// (*name) filename; filenames are NOT checked for duplication with existing files
// (len) data length in bytes
// (*data) pointer to the buffer with data
// (opt) defines the type of operation - 'W' to create/overwrite, or 'A' to create/append
// the function will return the header page number of the new file, or -1 in case one cannot be created or writing error
long flash_writeFile(char *name, long len, void *data, char opt);

// read data from file
// (p) header page of the file to read
// (*offs) offset in bytes from the start of the file; the function updates it with the actual offset
// (maxlen) an optional parameter limiting the number of bytes to read; 0 if unlimited
// (*buffer) pointer to available buffer where the data will be read
// the function will return the number of actually read bytes
long flash_readFile(long p, long *offs, long maxlen, void *buffer);

// rename file
// (p) header page of the file to rename
// (*name) new name for the file
void flash_renameFile(long p, char *name);

// delete file
// (p) header page of the file to be deleted
void flash_deleteFile(long p);

// get file header
// (p) header page number
// (*len) file length in number of flash pages
// (**name) pointer to the filename
void flash_getHeader(long p, long *len, char **name);

// check for existence of a file with the specified name
// (*name) filename to check for
// the function will return page number of the file header if found, or -1 otherwise
long flash_findFile(char *name);

// erase the entire flash storage
void flash_eraseAll(void);

#endif
