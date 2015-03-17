#include <ncurses.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pwd.h>
#include "debris.h"

static int cols = 0;
static int rows = 0;
static int shouldPromptForConfirm = 1;
const char* homedir;
const char* trashpath = "/.local/share/Trash/files";
const char* infopath = "/.local/share/Trash/info";

int main(int argv, char** argc) {
	if((homedir = getenv("HOME")) == NULL) {
                homedir = getpwuid(getuid())->pw_dir;
        }
	int trashColors[2];
	int selectedColors[2];
	int directoryColors[2];
	chdir(homedir);
	int configExists = 0;
	if(access(".debris.config", F_OK) != -1) {
		configExists = 1;
		//configuration exists
		FILE* fp = fopen(".debris.config", "r");
		if(fp == NULL) {
			perror("Unable to read from config.");
		} else {
			int lineNum = 0;
			char* buff = malloc(100*sizeof(char));
			char* val1 = malloc(50*sizeof(char));
			char* val2 = malloc(50*sizeof(char));
			char* val3 = malloc(50*sizeof(char));
			memset(buff, '\0', sizeof(char)*100);
			memset(val1, '\0', sizeof(char)*50);
			memset(val2, '\0', sizeof(char)*50);
			memset(val3, '\0', sizeof(char)*50);
			int num = 0;
			while(fgets(buff, 100*sizeof(char), fp)) {
				if(buff[strlen(buff)-1] == '\n') {
					buff[strlen(buff)-1] = '\0';
				}
				if(num == 0) {
					if(strcmp(buff, "[Debris Config]") != 0) {
						//printf("%d\n", strcmp(buff, "[Debris Config]"));
						perror("Corrupted config file! Attempting rewrite");
						//exit(6);
						configExists = 0; //tricking below code into rewriting config
					} else {
						num++;
						continue;
					}
				}
				char firstChar = *buff;
				if(firstChar == '#') {
					num++;
					continue;
				}
				sscanf(buff, "%[^,:]:%s %s", val1, val2, val3);
				//printf("Val1: %s, Val2: %s, Val3: %s\n", val1, val2, val3);
				if(strcmp(val1, "ConfirmPrompt") == 0) {
					int val1int = 0;
					sscanf(buff, "%[^,:]:%d", val1, &val1int);
					shouldPromptForConfirm = val1int;
				} else if(strcmp(val1, "TrashColor") == 0) {
					trashColors[0] = getColor(val2);
					trashColors[1] = getColor(val3);
				} else if(strcmp(val1, "SelectionColor") == 0) {
					selectedColors[0] = getColor(val2);
					selectedColors[1] = getColor(val3);
				} else if(strcmp(val1, "DirectoriesColor") == 0) {
					directoryColors[0] = getColor(val2);
					directoryColors[1] = getColor(val3);
				}
				num++;
			} 
			free(buff);
			free(val1);
			free(val2);
			free(val3);
			fclose(fp);
		}
	}
	if(!configExists) { //done in other if instead of else to allow manipulation in previous if
		//configuration doesn't exist
		FILE* fp = fopen(".debris.config", "w");
		if(fp == NULL) {
			perror("Unable to write default config.");
		} else {
			fprintf(fp, "[Debris Config]\n");
			fprintf(fp, "ConfirmPrompt: 1\n");
			fprintf(fp, "TrashColor: CYAN BLACK\n");
			fprintf(fp, "SelectionColor: BLUE BLACK\n");
			fprintf(fp, "DirectoriesColor: YELLOW BLACK\n");
			fprintf(fp, "#Colors can be BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE\n");
			fclose(fp);
		}
	}
	int size = 0;
	char* path = getTrashPath();
	/*printf("Size: %d\n", size);
	printf("Location: %p\n", allFiles);
	int i;
	for(i = 0; i < size; i++) {
		printf("\nIteration %d\n", i);
		printf("  Location: %p\n", &allFiles[i]);
		printf("  Filename: %s\n", allFiles[i].filename);
		printf("  isDirectory: %d\n", allFiles[i].isDirectory);
		if(allFiles[i].isDirectory) {
			printf("  fullDirectoryPath: %s\n", allFiles[i].fullDirectoryPath);
		}
	}
	int a = 0;
	a+=1;
	if(a) {
		return 0;
	}*/
	initscr();
	start_color();
	init_pair(1, trashColors[0], trashColors[1]); //trash
	init_pair(2, selectedColors[0], selectedColors[1]); //selection
	init_pair(3, directoryColors[0], directoryColors[1]); //directories
        refresh();
        noecho();
        cbreak();
	curs_set(0);
        keypad(stdscr, TRUE);
	getmaxyx(stdscr, rows, cols);
	int directoryMovement = 0; //for each folder entered, it is incremented in order to keep track of hierarchy
	while((path = show_directory(path, &directoryMovement)) != NULL) {
		/*endwin();
		printf("Run completed.\n");
		printf("Next path: %s, directory: %d, dptr: %p\n", path, directoryMovement, &directoryMovement);
		getch();
		initscr();*/
	}
	endwin();
	return 0;
}
int getColor(char* color) {
	if(strcmp(color, "YELLOW") == 0) {
		return COLOR_YELLOW;
	} else if(strcmp(color, "GREEN") == 0) {
		return COLOR_GREEN;
	} else if(strcmp(color, "BLUE") == 0) {
		return COLOR_BLUE;
	} else if(strcmp(color, "RED") == 0) {
		return COLOR_RED;
	} else if(strcmp(color, "BLACK") == 0) {
		return COLOR_BLACK;
	} else if(strcmp(color, "MAGENTA") == 0) {
		return COLOR_MAGENTA;
	} else if(strcmp(color, "CYAN") == 0) {
		return COLOR_CYAN;
	} else {
		return COLOR_WHITE;
	}
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
void display_modal(char* toDisplay) {
	clear();
	mvprintw(0, 0, toDisplay);
	refresh();
	getch();
}
char* getContainerAndFree(char* fullPath) {
	char* duplicate = malloc(strlen(fullPath)+1);
	memset(duplicate, '\0', strlen(fullPath)+1);
	strcat(duplicate, fullPath);
	free(fullPath);
	int numSlashes = 0;
	int i = 0;
	while(duplicate[i] != '\0') {
		if(duplicate[i] == '/') {
			numSlashes++;
		}
		i++;
	}
	int numSlashesSearch = 0;
	i = 0;
	while(numSlashesSearch < numSlashes) {
		if(duplicate[i] == '/') {
			numSlashesSearch++;
		}
		i++;
	}
	duplicate[i-1] = '\0';
	return duplicate;
}
char* show_directory(char* location, int* directoryMovement) {
	//mvprintw(0, 0, location);
	//getch();
	int size = 0;
	file_information* allFiles = read_directory(location, &size);
        char c = 0;
	int offset = 0;
	int selection = 0;
	int cont = 1;
	int reload = 0;
	int refreshDisplay = 0;
        while(cont) {
		switch(c) {
			case 3: //up arrow
				selection--;
				break;
			case 2: //down arrow
				selection++;
				break;
			case 5: //right arrow
				if(allFiles[selection].isDirectory == 1) {
					free(location);
					char* temp = malloc(strlen(allFiles[selection].fullDirectoryPath)+1);
					memset(temp, '\0', strlen(allFiles[selection].fullDirectoryPath)+1);
					strcat(temp, allFiles[selection].fullDirectoryPath);
					free_fileinformation(allFiles, size);
					(*directoryMovement)++;
					return temp;
				} else {
					display_modal("That is not a directory.");
				}
				break;
			case 4: //left arrow
				if(*directoryMovement == 0) {
					display_modal("Can't go back a directory.");
				} else {
					free_fileinformation(allFiles, size);
					(*directoryMovement)--;
					//clear();
					//mvprintw(0, 0, "%s", getContainerAndFree(location));
					//refresh();
					//getch();
					//exit(0);
					return getContainerAndFree(location);
				}
				break;
			case 10: //enter
			case 114: //r
				if(size == 0) {
					display_modal("Nothing to restore.");
				}
				if(*directoryMovement != 0) {
					display_modal("You can only restore files and folders from the topmost directory.");
				} else {
					int res = confirm_action("Are you sure you want to restore? [Y/n]");
					if(res) {
						char* originalPath = getoriginalpath(allFiles[selection].filename);
						if(access(originalPath, F_OK) != -1) {
							//file exists in location user attempted to restore to
							res = confirm_action("A file already exists in the location to which you are trying to restore the file. Would you like to attempt to restore anyway? [Y/n]");
							if(!res) {
								free(originalPath);
								break;
							}
						}
						if(originalPath == NULL) {
							display_modal("Unable to extract path from metadata.");
						} else {
							char* p = getTrashPath();
							int res = chdir(p);
							if(res != 0) {
								display_modal("Unable to set directory in order to move file.");
							} else {
								int renameresult = rename(allFiles[selection].filename, originalPath);
								if(renameresult != 0) { //move didn't happen successfully
									printf("Failed moving file back to location '%s'! Error code: %d, reason: %s\n", originalPath, errno, strerror(errno));
									free(originalPath);
									break;
								}
								removeMetadata(allFiles[selection].filename);
								reload = 1;
							}
							free(p);
						}
						free(originalPath);
					}
				}
				break;
			case 99: //c, config
				doNothing();
				char* editor = getenv("EDITOR");
             			if(editor == NULL) {
              				editor = "nano";
                		}
				char* buff = malloc(strlen(editor) + 1 + strlen(homedir) + strlen("/.debris.config") + 1);
				memset(buff, '\0', strlen(editor) + 1 + strlen(homedir) + strlen("/.debris.config") + 1);
				sprintf(buff, "%s %s%s", editor, homedir, "/.debris.config");
				endwin();
				system(buff);
				free(buff);
				refreshDisplay = 1;
				c = 0;
				initscr();
				curs_set(0);
				break;
			case 108: //l, reload
				reload = 1;
				break;
			case 112: //p, get trash path
				doNothing();
				char* path = getTrashPath();
				display_modal(path);
				free(path);
				break;
			case 63: //question mark
			case 104: //h
				display_help(rows);
				break;
			case 101: //e, empty
				if(confirm_action("Empty trash? [Y/n]")) {
					empty_trash();
				}
				reload = 1;
				break;
			case 105: //i, metadata
				if(size == 0) {
					display_modal("Nothing to inspect.");
					break;
				}
				if(*directoryMovement != 0) {
					display_modal("Cannot display info for item of a subdirectory.");
					break;
				}
				getmetadata(allFiles[selection].filename);
				break;
			case 113: //q
				cont = 0;
				break;
		}
		if(selection < 0) {
			selection = 0;
		} else if(selection >= size) {
			selection = size-1;
		} else if(selection < offset) {
			offset--;
		} else if(selection > offset + rows - 2) {
			offset++;
		}
		if(offset < 0) {
			offset = 0;
		} else if(size + 1 < rows) {
			offset = 0;
		} else if(offset + rows > size + 1) {
			offset = size - rows + 1;
		}
		clear();
		attron(COLOR_PAIR(1));
		mvprintw(0, 0, "Debris Trash Viewer");
		attroff(COLOR_PAIR(1));
		int i;
		int row = 1 + offset;
		if(size == 0) {
			mvprintw(1, 0, "Directory empty.");
		} else {
			for(i = 0; i + offset < size; i++) {
				char* curr = "";
				if(i >= rows - 1) {
					break;
				}
				int isDirectory = allFiles[i+offset].isDirectory;
				if(selection == i+offset) {
					attron(COLOR_PAIR(2));
					curr = fileTypes[allFiles[i+offset].type];
				} else if(isDirectory) {
					attron(COLOR_PAIR(3));
				}
				mvprintw(row-offset, 0, "%s%s", allFiles[i+offset].filename, curr);
				if(selection == i+offset) {
					attroff(COLOR_PAIR(2));
				} else if(isDirectory) {
					attroff(COLOR_PAIR(3));
				}
				row++;
			}
		}
		refresh();
		if(reload) {
			return location;
		}
                if(cont && !refreshDisplay) {
			c = getch(); //3 is up, 2 is down
		}
		if(refreshDisplay) {
			refreshDisplay = 0;
		}
	}
	free(location);
	free_fileinformation(allFiles, size);
	return NULL;
}
int empty_trash() {
	char* fullPath = getTrashPath();
	if(chdir(fullPath) != 0) {
		display_modal("Unable to open path to empty.");
		return 1;
	}
	DIR* dp;
	struct dirent *ep;
	dp = opendir(fullPath);
	if(dp != NULL) {
		while(ep = readdir(dp)) {
			if(ep->d_type == 4) { //directory
				if(!strcmp(ep->d_name, ".") || !strcmp(ep->d_name, "..")) {
					continue; //skip over . and ..
				}
				remove_directory(ep->d_name);
			} else {
				unlink(ep->d_name);
			}
		}
	}
	free(fullPath);
	fullPath = getInfoPath();
        if(chdir(fullPath) != 0) {
                display_modal("Unable to open path to empty info.");
                return 1;
        }
	closedir(dp);
        dp = opendir(fullPath);
        if(dp != NULL) {
                while(ep = readdir(dp)) {
                        unlink(ep->d_name);
                }
        }
	closedir(dp);
	free(fullPath);
}
int confirm_action(char* message) {
	if(!shouldPromptForConfirm) {
		return 1;
	}
	clear();
	mvprintw(0, 0, message);
	refresh();
	char c = getch();
	if(c == 121 || c == 89) { //y
		return 1;
	}
	return 0;
}
void doNothing() {} //hack to fix compiler error
void display_help() {
	int offset = 0;
	int size = helpMessageNumber;
	int cont = 1;
	while(cont) {
		clear();
		attron(A_BOLD);
		mvprintw(0, 0, "Help: [Q]uit");
		attroff(A_BOLD);
		int i;
		for(i = 1; i < rows; i++) {
			int rowNumber = i - 1 + offset;
			if(rowNumber >= 0 && rowNumber < size) {
				mvprintw(i, 0, helpMessages[rowNumber]);
			}
		}
		refresh();
		char c = getch();
		if(c == 2) { //down
			if(offset + rows <= size) {
				offset++;
			}
		} else if(c == 3) { //up
			offset--;
			if(offset < 0) {
				offset = 0;
			}
		} else if(c == 113) { //q
			cont = 0;
		}
	}
}
void free_fileinformation(file_information* fileinformation, int size) {
	int i;
	for(i = 0; i < size; i++) {
		if(fileinformation[i].filename != NULL) {
			free(fileinformation[i].filename);
		}
		if(fileinformation[i].fullDirectoryPath != NULL) {
			free(fileinformation[i].fullDirectoryPath);
		}
	}
	free(fileinformation);
}
file_information* read_directory(char* toread, int* fileAmount) {
	//printf("I'M JUST STARTIN' OUT\r\n");
	//endwin();
	//printf("AWUDDAFUKE\r\n");
	DIR* dp = NULL;
	struct dirent *pent = NULL;
	//printf("RING A DING DING WHO INVENTED THIS THING\r\n");
	dp = opendir(toread);
	//printf("%p\r\n", dp);
	//printf("I ALMOST TRIED\r\n");
	if(dp == NULL) {
		endwin();
		perror("Unable to open directory");
		exit(1);
	}
	int numFiles = 0;
	//printf("I TRIED SO HARD AND GOT SO FAR\r\n");
	while(pent = readdir(dp)) {
		if(pent == NULL) {
			endwin();
			perror("Could not properly initialize struct containing directory listing while attempting to determine the number of items in the directory");
			exit(4);
		}
		if(strcmp(pent->d_name, ".") == 0 || strcmp(pent->d_name, "..") == 0) {
			continue;
		}
		numFiles++;
	}
	closedir(dp);
	dp = opendir(toread);
	if(dp == NULL) {
		endwin();
		perror("Unable to open directory second time.");
		exit(1);
	}
	*fileAmount = numFiles;
	file_information* file_information_array = malloc(sizeof(file_information) * numFiles);
	pent = NULL;
	int currentFile = 0;
	while(pent = readdir(dp)) {
		if(pent == NULL) {
			endwin();
			perror("Could not properly initialize struct containing directory listing");
			exit(3);
		}
		if(strcmp(pent->d_name, ".") == 0 || strcmp(pent->d_name, "..") == 0) {
                	continue;
                }
		if(currentFile > numFiles) {
			endwin();
			perror("Directory changed while reading.");
			exit(2);
		}
		file_information_array[currentFile].filename = NULL;
		file_information_array[currentFile].isDirectory = 0;
		file_information_array[currentFile].fullDirectoryPath = NULL;
		file_information_array[currentFile].type = 8;
		if(pent->d_type == 4) { //directory
			int len = strlen(pent->d_name);
			len += strlen(toread) + 2;
			char* strptr = malloc(len);
			memset(strptr, '\0', sizeof(strptr));
			strcat(strptr, toread);
			strcat(strptr, "/");
			strcat(strptr, pent->d_name);
			//mvprintw(linenum, 0, strptr);
			//printf("%s\n", strptr);
			file_information_array[currentFile].isDirectory = 1;
			file_information_array[currentFile].fullDirectoryPath = strptr;
			char* filestring = malloc(strlen(pent->d_name) + 1);
			memset(filestring, '\0', sizeof(filestring));
			strcat(filestring, pent->d_name);
			file_information_array[currentFile].filename = filestring;
			file_information_array[currentFile].type = 4;
		} else { //file
			//printf("Filename: %s, Type: %d\n", pent->d_name, pent->d_type);
			//mvprintw(linenum, 0, pent->d_name);
			file_information_array[currentFile].isDirectory = 0;
			file_information_array[currentFile].fullDirectoryPath = NULL;
			char* filestring = malloc(strlen(pent->d_name) + 1);
                        memset(filestring, '\0', sizeof(filestring));
                        strcat(filestring, pent->d_name);
			file_information_array[currentFile].filename = filestring;
			file_information_array[currentFile].type = pent->d_type;
		}
		currentFile++;
	}
	closedir(dp);
	//printf("THAT'S AN OLD ONE FRIEND\r\n");
	return file_information_array;
}
int removeMetadata(char* filename) {
        FILE* fp; //TODO: reduce code duplication
        char* filepath = getInfoPath();
        char* concatFile = malloc(strlen(filepath) + strlen(filename) + (12 * sizeof(char))); //10 characters for .trashinfo, 1 for /, and one more for the null terminator
        memset(concatFile, '\0', strlen(filepath) + strlen(filename) + (12 * sizeof(char)));
        strcat(concatFile, filepath);
        strcat(concatFile, "/");
        strcat(concatFile, filename);
        strcat(concatFile, ".trashinfo");
	int retval = unlink(concatFile);
	free(filepath);
	free(concatFile);
	return retval;
}
char* getoriginalpath(char* filename) {
	FILE* fp; //TODO: reduce code duplication
        char* filepath = getInfoPath();
        char* concatFile = malloc(strlen(filepath) + strlen(filename) + (12 * sizeof(char))); //10 characters for .trashinfo, 1 for /, and one more for the null terminator
        memset(concatFile, '\0', strlen(filepath) + strlen(filename) + (12 * sizeof(char)));
        strcat(concatFile, filepath);
        strcat(concatFile, "/");
        strcat(concatFile, filename);
        strcat(concatFile, ".trashinfo");
        fp = fopen(concatFile, "r");
        if(fp == NULL) {
                endwin();
                printf("%s\n", concatFile);
                perror("Unable to open file metadata!");
                exit(5);
        }
        char* line = NULL;
        size_t len = 0;
        ssize_t read;
        clear();
        int lineNum = 0;
        const char* const idealFirstLine = "[Trash Info]";
	const char* const weAreLookingFor = "Path=";
	char* newString = NULL;
        while((read = getline(&line, &len, fp)) != -1) {
                if(line[read-1] == '\n') {
                        line[read-1] = '\0';
                }
                if(lineNum == 0) {
                        if(strcmp(line, idealFirstLine) != 0) {
                                mvprintw(0, 0, "Trash metadata corrupted.", line);
                                break;
                        }
		}
		char* tempFirstFiveCharacters = malloc(strlen(line));
		memset(tempFirstFiveCharacters, '\0', strlen(line));
		strcat(tempFirstFiveCharacters, line);
		tempFirstFiveCharacters[5] = '\0';
		if(strcmp(tempFirstFiveCharacters, weAreLookingFor) == 0) {
			newString = malloc(strlen(line) + 1 - 5);
			memset(newString, '\0', strlen(line) + 1 - 5);
			strcat(newString, line + (sizeof(char) * 5));
			break;
		}
		free(tempFirstFiveCharacters);
		lineNum++;
	}
	fclose(fp);
	free(filepath);
	free(concatFile);
	return newString;
}
void getmetadata(char* filename) {
	FILE* fp;
	char* filepath = getInfoPath();
	char* concatFile = malloc(strlen(filepath) + strlen(filename) + (12 * sizeof(char))); //10 characters for .trashinfo, 1 for /, and one more for the null terminator
	memset(concatFile, '\0', strlen(filepath) + strlen(filename) + (12 * sizeof(char)));
	strcat(concatFile, filepath);
	strcat(concatFile, "/");
	strcat(concatFile, filename);
	strcat(concatFile, ".trashinfo");
	fp = fopen(concatFile, "r");
	if(fp == NULL) {
		endwin();
		printf("%s\n", concatFile);
		perror("Unable to open file metadata!");
		exit(5);
	}
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	clear();
	int lineNum = 0;
	const char* const idealFirstLine = "[Trash Info]";
	while((read = getline(&line, &len, fp)) != -1) {
		if(line[read-1] == '\n') {
			line[read-1] = '\0';
		}
		if(lineNum == 0) {
			if(strcmp(line, idealFirstLine) != 0) {
				mvprintw(0, 0, "Trash metadata corrupted.");
				break;
			} else {
				attron(A_BOLD);
				mvprintw(0, 0, "[E]dit [Q]uit");
				attroff(A_BOLD);
			}
		} else {
			mvprintw(lineNum, 0, "%s", line);
		}
		lineNum++;
	}
	refresh();
	char c = getch();
	if(c == 69 || c == 101) { //e
		char* editor = getenv("EDITOR");
		if(editor == NULL) {
			editor = "nano";
		}
		endwin();
		char* command;
		int filenamesize = strlen(filename);
		int newfilenamesize = 1;
		int i;
		for(i = 0; i < filenamesize; i++) {
			if(filename[i] == '\'') {
				newfilenamesize+=6;
			} else {
				newfilenamesize++;
			}
		}
		char* newfilename = malloc(newfilenamesize);
		int o = 0;
		for(i = 0; i < newfilenamesize; i++) {
			if(i == newfilenamesize-1) {
				newfilename[i] = '\0';
			} else {
				if(filename[o] == '\'') {
					newfilename[i] = '\'';
					i++;
					newfilename[i] = '"';
					i++;
					newfilename[i] = '\'';
					i++;
					newfilename[i] = '"';
					i++;
					newfilename[i] = '\'';
				} else {
					newfilename[i] = filename[o];
				}
			}
			o++;
		}
		//chdir(infopath);
		//printf("%s\n", newfilename);
		free(concatFile);
		concatFile = malloc(strlen(filepath) + strlen(newfilename) + (12 * sizeof(char))); //10 characters for .trashinfo, 1 for /, and one more for the null terminator
	        memset(concatFile, '\0', strlen(filepath) + strlen(newfilename) + (12 * sizeof(char)));
	        strcat(concatFile, filepath);
	        strcat(concatFile, "/");
	        strcat(concatFile, newfilename);
	        strcat(concatFile, ".trashinfo");
		asprintf(&command, "%s '%s'", editor, concatFile);
		//system("echo $PWD; sleep 10");
		system(command);
		free(command);
		free(newfilename);
		initscr(); 
		curs_set(0);
	}
	if(line) {
		free(line);
	}
	fclose(fp);
	free(filepath);
	free(concatFile);
	//return NULL;
}
int remove_directory(char* path) {
	DIR* dir = opendir(path);
	//endwin();
	//printf("Opening dir %s\r\n", path);
	//getch();
	size_t path_size = strlen(path);
	int r = -1;
	if(dir) {
		struct dirent *p;
		r = 0;
		while(!r && (p = readdir(dir))) {
			int r2 = -1;
			char* buff;
			size_t len;
			if(!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
				continue; //don't search . or .. directories
			}
			//printf("    Opening file %s\r\n", p->d_name);
			len = path_size + strlen(p->d_name);
			buff = malloc(len + 2); //one extra for slash, one for null terminator
			if(buff) {
				memset(buff, '\0', len+2);
				struct stat statbuff;
				strcat(buff, path);
				strcat(buff, "/");
				strcat(buff, p->d_name);
				//snprintf(buff, len, "%s/%s", path, p->d_name);
				//printf("    Buff is: %s\r\n", buff);
				if(p->d_type == 4) { //directory
					//printf("    Maybe removing %s\r\n", buff);
					//if(S_ISDIR(statbuff.st_mode)) {
					//	printf("    Removing %s\r\n", buff);
						r2 = remove_directory(buff); //RECURSIVE STUFF
					//}
				} else {
					r2 = unlink(buff);
					//printf("    Just unlinked %s\r\n", buff);
				}
				free(buff);
			} else {
				perror("Out of memory!\n");
				exit(8);
			}
			r = r2;
			//getch();
		}
		closedir(dir);
	}
	if(!r) {
		r = rmdir(path);
	}
	//getch();
	//initscr();
	return r;
}
int resizeterm(int newrows, int newcols) {
	rows = newrows;
	cols = newcols;
	return OK;
}
