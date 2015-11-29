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
 //Size Constants 
 	//Memory (in Bytes)
	#define SECTOR_SIZE 512
	#define FAT_SIZE (512 * 9)
	#define ROOT_SIZE (512 * 14)
	#define DATA_SIZE (512 * 2847)
	#define MAX_BYTES (512 * 2879)
 	#define DIR_ENTR_SIZE 32
 	//File System Components
 	#define SECTOR_DIRECTORIES 16
 	#define FAT_ENTRIES ((FAT_SIZE * (2/3)) + 1)

 //Location Constants
	#define DATA_LOC (SECTOR_SIZE + (FAT_SIZE * 2) + ROOT_SIZE)
	#define FAT2_LOC (SECTOR_SIZE + FAT_SIZE)
	#define ROOT_LOC (SECTOR_SIZE + (FAT_SIZE * 2))

/* Struct to hold two entries */
typedef struct Entries{
	uint16_t first;
	uint16_t second;
} Entries;

typedef struct File {
	unsigned char *filename;
	unsigned char *extension;
	unsigned char *attributes;
	unsigned char *reserved;
	unsigned char *creationTime;
	unsigned char *creationDate;
	unsigned char *lastAccessed;
	unsigned char *lastModTime;
	unsigned char *lastModDate;
	unsigned char *firstLogCluster; 
	unsigned char *fileSize; 		//(in Bytes)
	char *filepath;
	int isValid;
} File;

/* Global Variables */
char bootBlock[SECTOR_SIZE];
uint16_t *fat1[FAT_ENTRIES];
uint16_t *fat2[FAT_ENTRIES];
unsigned char rootDirectory[ROOT_SIZE];
unsigned char data[DATA_SIZE];

char *destPath;



/* Helper Functions======================================================================*/

	/* Function: getSector      */
		uint16_t getSector(uint16_t fatEntry){
			return (uint16_t)(33 + fatEntry - 2);
		}

	/* Function: convertEndian */
		//Purpose: Converts a unsigned char *hex in little endian to uint32_t in big endian
		uint32_t convertEndian(unsigned char *hex, int numBytes){
			uint32_t result = 0;
			for(int i = 0; i < numBytes; i++){
				uint32_t temp = ((uint32_t) hex[i]) << (unsigned) (8 * i);
				result = result | temp;
			}
			return result;
		}

	/* Function: printFile     */
		//Purpose: Print formatted file information to stdout
		void printFile(File *file){
			assert(file != NULL);
			printf("FILE\t");
			assert(file->filename != NULL);
			if(file->isValid){
				printf("NORMAL\t");
			} else {
				printf("DELETED\t");
			}
			assert(file->filepath != NULL);
			printf("%s", file->filepath);
			int i = 0;
			while(file->filename[i] != ' ' && file->filename[i] != '.' && i < 8){
				printf("%c", file->filename[i]);
				i++;
			}
			printf(".");
			assert(file->extension != NULL);
			i = 0;
			while(file->extension[i] != '.' && i < 3){
				printf("%c", file->extension[i]);
				i++;
			}
			printf("\t");
			assert(file->fileSize != NULL);
			printf("%d", convertEndian(file->fileSize, 4));

			printf("\n");
		}

/*	Exploration Functions==================================================================*/
	/* Function: createFile    */
		File *createFile(char *raw, char *filepath){
			File *nextFile = malloc(sizeof(File));
			int currentInd = 0;

			//Check if the file is valid
			if(0xE5 != raw[currentInd]){
				nextFile->isValid = 1;
			} else {
				nextFile->isValid = 0;
			}

			//Read in filename-------------------------------------------
			assert(currentInd == 0);
			nextFile->filename = (unsigned char *)malloc(8 * sizeof(char));
			for(int i = 0; i < 8; i++){
				nextFile->filename[i] = raw[currentInd+i];
			}
			currentInd += 8;

			//Read in extension------------------------------------------
			assert(currentInd == 8);
			nextFile->extension = (unsigned char *)malloc(3 * sizeof(char));
			for(int i = 0; i < 3; i++){
				nextFile->extension[i] = raw[currentInd+i];
			}
			currentInd += 3;

			//Read in attributes-----------------------------------------
			assert(currentInd == 11);
			nextFile->attributes = (unsigned char *)malloc(1 * sizeof(char));
			nextFile->attributes[0] = raw[currentInd];
			currentInd++;

			//Read in reserved-------------------------------------------
			assert(currentInd == 12);
			nextFile->reserved = (unsigned char *)malloc(2 * sizeof(char));
			for(int i = 0; i < 2; i++){
				nextFile->reserved[i] = raw[currentInd+i];
			}
			currentInd += 2;

			//Read in creationTime---------------------------------------
			assert(currentInd  == 14);
			nextFile->creationTime = (unsigned char *)malloc(2 * sizeof(char));
			for(int i = 0; i < 2; i++){
				nextFile->creationTime[i] = raw[currentInd+i];
			}
			currentInd += 2;

			//Read in creationDate---------------------------------------
			assert(currentInd == 16);
			nextFile->creationDate = (unsigned char *)malloc(2 * sizeof(char));
			for(int i = 0; i < 2; i++){
				nextFile->creationDate[i] = raw[currentInd+i];
			}
			currentInd += 2;

			//Read in lastAccessed info----------------------------------
			assert(currentInd == 18);
			nextFile->lastAccessed = (unsigned char *)malloc(2 * sizeof(char));
			for(int i = 0; i < 2; i++){
				nextFile->lastAccessed[i] = raw[currentInd+i];
			}
			currentInd += 4;

			//Read in lastModified Time----------------------------------
			assert(currentInd == 22);
			nextFile->lastModTime = (unsigned char *)malloc(2 * sizeof(char));
			for(int i = 0; i < 2; i++){
				nextFile->lastModTime[i] = raw[currentInd+i];
			}
			currentInd += 2;

			//Read in lastModified Date-------------------------------------------
			assert(currentInd  == 24);
			nextFile->lastModDate = (unsigned char *)malloc(2 * sizeof(char));
			for(int i = 0; i < 2; i++){
				nextFile->lastModDate[i] = raw[currentInd+i];
			}
			currentInd += 2;

			//Read in first logical cluster---------------------------------------
			assert(currentInd == 26);
			nextFile->firstLogCluster = (unsigned char *)malloc(2 * sizeof(char));
			for(int i = 0; i < 2; i++){
				nextFile->firstLogCluster[i] = raw[currentInd+i];
			}
			currentInd += 2;

			//Read in file size---------------------------------------------------
			assert(currentInd == 28);
			nextFile->fileSize = (unsigned char *)malloc(4 * sizeof(char));
			for(int i = 0; i < 4; i++){
				nextFile->fileSize[i] = raw[currentInd+i];
			}
			currentInd += 4;

			nextFile->filepath = (char *) malloc(strlen(filepath));
			for(int i = 0; i < strlen(filepath); i++){
				nextFile->filepath[i] = filepath[i];
			}

			return nextFile;
		}

	/* Function: exploreFile   */
		//Purpose: Finds the file's contents and saves it to the user's specified directory
		void exploreFile(File *file){

		}

	/* Function: exploreDirectory */
		//Purpose: Explores a subdirectory for files contained within
		void exploreDirectory(File *file){

		}

	/* Function: exploreRoot  */
		void exploreRoot(){
			assert(rootDirectory != NULL);

			int currentInd = 0;
			while((currentInd + DIR_ENTR_SIZE) <= ROOT_SIZE){
				if(rootDirectory[currentInd] != 0x00){
					char *rawData =(char *) malloc(DIR_ENTR_SIZE * sizeof(char));
					for(int i = 0; i <  DIR_ENTR_SIZE; i++){
						rawData[i] = rootDirectory[currentInd];
						currentInd++;
					}

					char *filepath = (char *) malloc(sizeof(char));
					filepath = "/";

					File *nextFile = createFile(rawData, filepath);
					assert(nextFile != NULL);

					//If file isn't a subdirectory, print and explore it
						//else explore subdirectory
					if((convertEndian(nextFile->fileSize, 4)) != 0){
						printFile(nextFile);
						//exploreFile
					} else {
						//exploreDirectory
					}
				}
				currentInd += 32;
			}

		}

	/* Function: getEntries     */
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

/* Disk Utility Functions==============================================================*/

	/* Function: translateDisk  */
		void translateDisk(unsigned char *disk){
			//Read the contents of the disk into the appropriate data structures
		char FAT1[FAT_SIZE];
		char FAT2[FAT_SIZE];

			int counter = 0;
			while(counter < MAX_BYTES){
				unsigned char nextByte = disk[counter];

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

			//Format the FATs correctly
			int i = 0;
			int entryInd = 0;
			char *one = malloc(1);
			char *two = malloc(1);
			char *three = malloc(1);
			while(i+3 < FAT_SIZE){
				*one = FAT1[i];
				*two = FAT1[i+1];
				*three = FAT1[i+2];

				Entries e = getEntries(one, two, three);
				fat1[entryInd] = malloc(sizeof(uint16_t));
				fat1[entryInd+1] = malloc(sizeof(uint16_t));
				*fat1[entryInd] = e.first;
				*fat1[entryInd+1] = e.second;

				i += 3;
				entryInd += 2;
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
			disk = (uint8_t *) mmap(0, length, PROT_READ, MAP_FILE|MAP_PRIVATE, fp, 0);
			assert(disk != 0);

			return disk;
		}


/* Main */

 int main(int argc, char *argv[]){
 	assert(argc == 3);

 	//Program Input Parameters
 	char *diskpath = argv[1];			//Path of disk image
 	destPath = malloc(strlen(argv[2])); //Directory path for file output
 	destPath = argv[2];

 	//Get Disk
 	unsigned char *disk = getDisk(diskpath);
 	translateDisk(disk);

 	exploreRoot();


 	return 0;
 }