#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define STDOUT 1
#define STDERR 2

#define da_append(xs, x) \
	if(xs.count >= xs.capacity) {\
		xs.capacity = xs.capacity * 2 + 1;\
		xs.items = realloc(xs.items, xs.capacity*sizeof(x));\
	}\
	xs.items[xs.count++] = x;

#define ptr_da_append(xs, x) \
	if(xs->count >= xs->capacity) {\
		xs->capacity = xs->capacity * 2 + 1;\
		xs->items = realloc(xs->items, xs->capacity*sizeof(x));\
	}\
	xs->items[xs->count++] = x;

struct class {
	char* name; 
	char* other;
	int startMin, endMin;
};

struct day {
	int index; 
	struct class* items;
	size_t count;
	size_t capacity;
};

struct schedule {
	struct day* items; 
	size_t count; //I don't want to assume a 5 day schedule ok
	size_t capacity;
};

struct schedule* readSchedule(char* filename){
	FILE* f = fopen(filename, "r");
	if(f == NULL){
		fprintf(stderr, "Error opening file\n");
		exit(2);
	}

	char buf[512];

	int bytes; 
	int section = 0;

	struct { char* key; struct class value; }* hash = NULL;

	struct class emptyClass = { 0 }; 
	emptyClass.name = NULL; //juuust to make sure
	hmdefault(hash, emptyClass);
	char* curr;
	char* token;
	
	int line = 0;
	int day = 0;
	struct schedule* out = calloc(sizeof(struct schedule), 1);

	while(fgets(buf, sizeof(buf), f) != NULL){
		line++; //1 index
		if(strchr(buf, '\n') == NULL){
			fprintf(stderr, "Input too long, make it shorter! Stack space doesn't grow on trees!\n");
			exit(2);
		}
		if(section == 0){
			if(strcmp(buf, "SCHEDULE\n") == 0){
				section += 1;
			}else{
				curr = buf;
				struct class c = { 0 }; 
				token = strsep(&curr, ",");
				if(token == NULL){
					fprintf(stderr, "Error reading line %d\n", line);
					exit(2);
				}
				c.name = (char*) (calloc(strlen(token) + 1, sizeof(char))); 
				strcpy(c.name, token);
				
				token = strsep(&curr, ",");
				if(token == NULL){
					fprintf(stderr, "Error reading line %d\n", line);
					exit(2);
				}
				int hour, minute;
				sscanf(token, "%d:%d", &hour, &minute);
				c.startMin = hour*60 + minute;

				token = strsep(&curr, ",");
				if(token == NULL){
					fprintf(stderr, "Error reading line %d\n", line);
					exit(2);
				}
				sscanf(token, "%d:%d", &hour, &minute);
				c.endMin = hour*60 + minute;

				token = strsep(&curr, ",");
				if(token != NULL){
					token[strcspn(token, "\n")] = '\0';
					c.other = (char*) (calloc(strlen(token) + 1, sizeof(char))); 
					strcpy(c.other, token);
				}
				//printf("Class %s starts at %d and ends at %d (in minutes since 0)\n", c->name, c->startMin, c->endMin);

				shput(hash, c.name, c);
			
			}
		}else if(section == 1){

			curr = buf;
			struct day d = { 0 };
			d.index = day;
			while( (token = strsep(&curr, ",")) != NULL){
				if(token[0] == '\n') continue;
				token[strcspn(token, "\n")] = '\0';
			
				struct class c = shget(hash, token);
				//printf("%p\n", c);
				if(c.name == NULL){
					fprintf(stderr, "Undefined class >%s< at line %d", token, line);
					exit(3);
				}
				da_append(d, shget(hash, token));
			}
			ptr_da_append(out, d);
			day++; //worst naming scheme oat???
		}

	}
	
	return out;

	fclose(f);
	free(hash);
}


int main(int argc, char** argv){
	
	if(argc < 2){
		return 0;
	}
	time_t rawTime;
	struct tm* local;
	const char *weekdays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	int bytes = 0;

	rawTime = time(NULL);
	local = localtime(&rawTime);

	char* today = malloc(250); 
	bytes = sprintf(today, "%s %02d:%02d:%02d", weekdays[local->tm_wday], local->tm_hour, local->tm_min, local->tm_sec);
	if(bytes < 0){
		fprintf(stderr, "Failed to create string\n");
		exit(1);
	}
	
	if(bytes < 0){
		fprintf(stderr, "Failed to write string\n");
		exit(1);
	}
	struct schedule* data = readSchedule(argv[1]);

	struct day d = data->items[local->tm_wday]; 

	int currentMin = local->tm_hour * 60 + local->tm_min;

	for(int i=0; i<d.count; i++){
		struct class c = d.items[i]; 
		if(currentMin < c.endMin){
			printf("%s", c.name);
			if(c.other != NULL){
				printf("-%s", c.other);
			}
			printf(",");
			printf("time until end: %02d:%02d", c.endMin - currentMin-1, 60-local->tm_sec); 
			if(i < d.count-1){
				printf("next: %s", d.items[i+1].name);
			}
			return 0;
			
		}
	}
	printf("Nothing left today :)");

	return 0;
}
