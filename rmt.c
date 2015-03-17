#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <libgen.h>
#include <pwd.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>

void handle_arg(char* str);
void handle_file(char* filename);
char* getDate();

const char* homedir;
const char* trashpath = "/.local/share/Trash/files";
const char* infopath = "/.local/share/Trash/info";
char** filesList = NULL;
int numFiles = 0;

int main(int argc, char** argv) {
	int i;
	if(argc == 1) {
		printf("Please specify files to place in the trash.\n");
	}
	for(i = 1; i < argc; i++) {
		char firstLetter = *argv[i];
		if(firstLetter != '-') {
			handle_file(argv[i]);
		} else {
			if(strlen(argv[i]) > 1) {
				char secondLetter = argv[i][1];
				if(secondLetter == '-') {
					char* temp = malloc(strlen(argv[i]));
					memset(temp, '\0', strlen(argv[i]));
					strcat(temp, &argv[i][2]);
					handle_arg(temp);
					free(temp);
				} else {
					int o;
					char* temp = malloc(sizeof(char)*2);
					for(o = 1; o < strlen(argv[i]); o++) {
						temp[0] = argv[i][o];
						temp[1] = '\0';
						handle_arg(temp);
					}
					free(temp);
				}
			} else {
				handle_arg(argv[i]);
			}
		}
	}
}
char* getFullPath(char* filename) {
	char* temp = malloc(sizeof(char) * 1024);
	memset(temp, '\0', 1024);
	getcwd(temp, 1024);
	strcat(temp, "/");
	strcat(temp, filename);
	return temp;
}
char* getDate() {
	time_t sec = time(NULL);
	struct tm* thetime;
	thetime = localtime(&sec);
	char* buff = malloc(50*sizeof(char));
	memset(buff, '\0', 50*sizeof(char));
	size_t size = strftime(buff, 49*sizeof(char), "%FT%T", thetime);
	return buff;
}
char* getTrashPath() {
	if((homedir = getenv("HOME")) == NULL) {
                homedir = getpwuid(getuid())->pw_dir;
        }
        char* path = malloc(strlen(homedir) + strlen(trashpath) + 1);
        memset(path, '\0', strlen(homedir) + strlen(trashpath) + 1);
        strcat(path, homedir);
        strcat(path, trashpath);
        return path;
}
char* getTrashPathFor(char* filename) {
	char* path = getTrashPath();
	char* buff = malloc(strlen(path) + strlen(filename) + 2);
	memset(buff, '\0', strlen(path) + strlen(filename) + 2);
	strcat(buff, path);
	strcat(buff, "/");
	strcat(buff, filename);
	free(path);
	return buff;
}
char* getInfoPath() {
        if((homedir = getenv("HOME")) == NULL) {
                homedir = getpwuid(getuid())->pw_dir;
        }
        char* path = malloc(strlen(homedir) + strlen(infopath) + 1);
        memset(path, '\0', strlen(homedir) + strlen(infopath) + 1);
        strcat(path, homedir);
        strcat(path, infopath);
        return path;
}
char* getInfoPathFor(char* filename) {
	const char* const extension = ".trashinfo";
        char* path = getInfoPath();
        char* buff = malloc(strlen(path) + strlen(filename) + 2 + strlen(extension));
        memset(buff, '\0', strlen(path) + strlen(filename) + 2 + strlen(extension));
        strcat(buff, path);
        strcat(buff, "/");
        strcat(buff, filename);
	strcat(buff, extension);
        free(path);
        return buff;
}
char* getTrashInfo(char* path) {
	char* temp = malloc(sizeof(char) * 1024);
	memset(temp, '\0', sizeof(char) * 1024);
	strcat(temp, "[Trash Info]\n");
	strcat(temp, "Path=");
	strcat(temp, path);
	strcat(temp, "\nDeletionDate=");
	char* date = getDate();
	strcat(temp, date);
	strcat(temp, "\n");
	free(date);
	return temp;
}
char** getFileArray() {
	char* fullPath = getTrashPath();
	DIR* dp;
        struct dirent *ep;
        dp = opendir(fullPath);
	numFiles = 0;
        if(dp != NULL) {
                while(ep = readdir(dp)) {
			numFiles++;
                }
        }
	char** charArray = malloc(sizeof(char*) * numFiles);
	closedir(dp);
	dp = opendir(fullPath);
	int i = 0;
	if(dp != NULL) {
		while(ep = readdir(dp)) {
			char* buff = malloc(strlen(ep->d_name)+1);
			memset(buff, '\0', strlen(ep->d_name)+1);
			strcat(buff, ep->d_name);
			charArray[i] = buff;
			//printf("Current string: %s, buffered: %s, array: %s\n", ep->d_name, buff, charArray[i]);
			i++;
		}
	}
	closedir(dp);
        free(fullPath);
	return charArray;
}
void handle_arg(char* str) {
	if(strcmp(str, "help") == 0 || strcmp(str, "h") == 0) {
		printf("Usage: rmt <filenames>...\n");
		printf("For a graphical representation, type debris.\n");
	} else {
		printf("Unknown arg: %s! Aborting.\n", str);
		exit(1);
	}
}
void freeArray(char** array) {
	int i;
	for(i = 0; i < numFiles; i++) {
		free(array[i]);
	}
	free(array);
}
void handle_file(char* filename) {
	//printf("Processing %s\n", filename);
	if(filesList != NULL) {
		freeArray(filesList);
	}
	filesList = getFileArray();
	int i;
	if(access(filename, F_OK) != -1) {
		//file exists
		char* temp = malloc(strlen(filename) + 1 + 2);
		memset(temp, '\0', strlen(filename) + 1 + 2);
		strcat(temp, filename);
		int hasAltered = 0;
		int alterChar = '2';
		int loop = 1;
		while(loop) {
			loop = 0;
			for(i = 0; i < numFiles; i++) {
				char* basename1;
				char* basename2;
				basename1 = basename(filesList[i]);
				basename2 = basename(temp);
				//printf("Comparing %s with %s, original %s\n", basename1, basename2, filesList[i]);
				if(strcmp(basename1, basename2) == 0) {
					if(hasAltered) { 
						temp[strlen(temp)-1] = ++alterChar;
					} else {
						strcat(temp, ".2");
						hasAltered = 1;
					}
					loop = 1;
					break;
				}
			}
		}
		if(hasAltered) {
			printf("File %s renamed to %s to prevent naming conflicts.\n", filename, temp);
		}
		char* renameTo = getTrashPathFor(temp);
		//printf("Renaming: %s to %s\n", filename, renameTo);
		int res = rename(filename, renameTo);
		if(res == -1) {
			char* errorstring = strerror(errno);
			printf("Fatal error while attempting to move file %s to trash! Code: %d, error message: %s\n", filename, errno, errorstring);
			exit(7);
		}
		char* fullPath = getFullPath(filename);
		char* toWrite = getTrashInfo(fullPath);
		free(renameTo);
		renameTo = getInfoPathFor(temp);
		FILE* fp = fopen(renameTo, "w");
		fprintf(fp, toWrite);
		fclose(fp);
		free(toWrite);
		free(renameTo);
		free(temp);
		free(fullPath);
	} else {
		//the user's a liar!
		printf("File '%s' doesn't exist.\n", filename);
	}
	freeArray(filesList);
	filesList = NULL;
}
