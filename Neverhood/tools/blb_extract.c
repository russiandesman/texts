#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


struct BLBHeader
{
	uint32_t id1;
	uint16_t id2;
	uint16_t extDataSize;
	uint32_t fileSize;
	uint32_t fileCount;
};

/*
 * id1 -- is always 0x02004940.
   id2 -- is always 0x07.
   extDataSize -- the size of the shift array of the BLB file (see below).
   fileSize -- the size of BLB file.
   fileCount -- the number of files stored in BLB file.
   After the header comes the array of (fileCount) file IDs. Each file ID is a DWORD identifying a file in BLB archive.
   After the file IDs array comes the array of (fileCount) resource entries. Each resource entry contains the info on a file in BLB archive. Each such entry has the following format:*/
struct BLBFileID
{
	uint32_t id;
};
struct BLBDirEntry
{
	uint8_t type;
	uint8_t comprType;
	uint16_t extDataIndex;
	uint32_t timeStamp;
	uint32_t offset;
	uint32_t diskSize;
	uint32_t outSize;
};   

char* get_filetype(char* buf, int type)
{
	switch(type)
	{
		case 0x02: sprintf(buf,"sprite/graphic file"); break;
		case 0x03: sprintf(buf,"palette file"); break;
		case 0x07: sprintf(buf,"sound effect"); break;
		case 0x08: sprintf(buf,"music"); break;
		case 0x0A: sprintf(buf,"video file"); break;
		default: sprintf(buf, "--unknown 0x%X", type);
	}
	return buf;
}

bool interesting(int type, int compress)
{
	bool result = true;
	switch(type)
	{
		case 0x02:
		case 0x03:
		case 0x07:
		case 0x08:
		case 0x0A: result = false; break;
	}
	return result && (compress != 0x65);
}

void extract_file(FILE* parent, const struct BLBDirEntry* description, const char* prefix)
{
	char buf[512];
	long int current_pos = ftell(parent);
	fseek(parent, description->offset, SEEK_SET);
	sprintf
	(
		buf,
		"%stype_%02X_compress_%02x_outsize_%d.unknown", 
		prefix,
		description->type,
		description->comprType,
		description->outSize
	);
	void* copybuffer = malloc(description->diskSize);
	fread(copybuffer, description->diskSize, 1, parent);
	FILE* output = fopen(buf, "wb");
	fwrite(copybuffer, description->diskSize, 1, output);
	fclose(output);
	fseek(parent, current_pos, SEEK_SET);
}

void process_file(char* filename, char* prefix)
{
	FILE* f = fopen(filename, "rb");
	struct BLBHeader header;
	fread(&header, sizeof(header), 1, f);
	printf("file: %s\n", filename);
	printf
	(
		"id1=0x%08x, id2=0x%04x, extSize=%d, filesize=%d, filecount=%d\n", 
		header.id1,
		header.id2,
		header.extDataSize,
		header.fileSize,
		header.fileCount
	);
	for ( int i = 0; i < header.fileCount; i++ )
	{
		struct BLBFileID fileid;
		fread(&fileid, sizeof(fileid), 1, f);
//		printf("fileid = 0x%08x\n", fileid.id);
	}
	for ( int i = 0; i < header.fileCount; i++ )
	{
		struct BLBDirEntry entry;
		char buf[512];
		fread(&entry, sizeof(entry), 1, f);
		if ( interesting(entry.type, entry.comprType) )
		{
			printf("type = %s, compressed=0x%02x, disksize=%d, finalsize=%d, offset=%d\n", get_filetype(buf, entry.type), entry.comprType, entry.diskSize, entry.outSize, entry.offset);
			extract_file(f, &entry, prefix);
			
		}
	}
	
	fclose(f);
	
}


int main(int argc, char** argv)
{
	for ( int i = 1; i < argc; i++ )
	{
		char buf[50];
		sprintf(buf, "source_%d_", i);
		process_file(argv[i], buf);
	}
	return 0;
}
