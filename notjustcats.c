/*
 * Author :: JL Garofalo (Username: Garofal)
 * Class  :: CPSC 3220
 * Instructor :: Dr. Sorber
 * Date       :: 12-04-2015
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

/* FAT12 Constants */
#define SECTOR_SIZE 512
#define FAT_SIZE (512 * 9)
#define ROOT_SIZE (512 * 14)
#define DATA_SIZE (512 * 2847)
#define MAX_BYTES (512 * 2879)
#define DATA_LOC (SECTOR_SIZE + (FAT_SIZE * 2) + ROOT_SIZE)
#define FAT2_LOC (SECTOR_SIZE + FAT_SIZE)
#define ROOT_LOC (SECTOR_SIZE + (FAT_SIZE * 2))


/* Global Variables */
char bootBlock[SECTOR_SIZE];
char FAT1[FAT_SIZE];
char FAT2[FAT_SIZE];
char rootDirectory[ROOT_SIZE];
char data[DATA_SIZE];

/* Struct to hold two entries */
typedef struct Entries{
	uint16_t first;
	uint16_t second;
} Entries;

/* Helper Functions */
 /* Function getEntries */
	//Purpose: Calculates the entry values from three bytes (specific to FAT12)
	Entries getEntries(char *firstByte, char *secondByte, char *thirdByte){
		Entries myEntries;
		// Split the second byte up
		uint16_t second = (uint16_t) strtol(secondByte, &secondByte, 16);
		uint16_t nib1 = (second >> 4); //Dest: Second
		uint16_t nib2 = ((second & 0x0f) << 8); //Dest: First

		//Place the nibs into their respective entries
		uint16_t first = (uint16_t) strtol(firstByte, &firstByte, 16);
		myEntries.first = (nib2 | first);
		uint16_t third = (uint16_t) strtol(thirdByte, &thirdByte, 16);
		myEntries.second = (nib1 | (third << 4));

		return myEntries;
	}

 /* Function: getSector */
	uint16_t getSector(uint16_t fatEntry){
		return (uint16_t)(33 + fatEntry - 2);
	}

 /* Function: translateDisk  */
	 void translateDisk(unsigned char *disk){
	 	//Read the contents of the disk into the appropriate data structures
	 	int counter = 0;
	 	while(counter < MAX_BYTES){
	 		char nextByte = disk[counter];
	 		if(nextByte == '\0') break;

	 		if(counter < SECTOR_SIZE){
	 			bootBlock[counter] = nextByte;
	 		} else if(counter < FAT2_LOC) {
	 			FAT1[counter-SECTOR_SIZE] = nextByte;
	 		} else if(counter < ROOT_LOC){
	 			FAT2[counter-FAT2_LOC] = nextByte;
	 		} else if(counter < DATA_LOC){
	 			rootDirectory[counter-ROOT_LOC] = nextByte;
	 		} else {
	 			data[counter-DATA_LOC] = nextByte;
	 		}
	 		counter++;
	 	}
	 }
 /* Function: getDisk        */
	 unsigned char *getDisk(char *filepath){
	 	unsigned char *disk;
	 	unsigned int length;
	 	struct stat filebuf;

	 	int fp = open(filepath, O_RDONLY);
	 	//Assert file was read properly
	 	assert(fp >= 0);
	 	assert(fstat(fp, &filebuf) >= 0);

	 	length = (unsigned int) filebuf.st_size;
	 	disk = (unsigned char *)mmap(0, length, PROT_READ, MAP_FILE|MAP_PRIVATE, fp, 0);
	 	assert(disk != 0);
	 	disk[length] = '\0';
	 	return disk;
	 }

/* Removeable Debug Functions */
 /* Check Contents           */
 void checkContents(){
 	 printf("\n\nBoot\n");
 	for(int i = 0; i < 10; i++){
 		printf("%c\n", bootBlock[i]);
 	}
	printf("\n\nFAT1\n");
 	for(int i = 0; i < 10; i++){
 		printf("%c", FAT1[i]);
 	}

 	printf("\n\nFAT2\n");
 	for(int i = 0; i < 10; i++){
 		printf("%c", FAT2[i]);
 	}

 	printf("\n\nData\n");
 	for(int i = 0; i < 10; i++){
 		printf("%c", data[i]);
 	}
 }

/* Main */

 int main(int argc, char *argv[]){
 	assert(argc == 3);

 	//Program Input Parameters
 	char *diskpath = argv[1];
 	char *destDirectory = argv[2];

 	//Get Disk
 	unsigned char *disk = getDisk(diskpath);
 	translateDisk(disk);



 	return 0;
 }