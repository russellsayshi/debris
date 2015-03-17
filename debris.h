typedef struct {
	char* filename;
	char* fullDirectoryPath;
	int isDirectory;
	int type;
} file_information;
/*typedef struct {
	char* path;
	char* deletiondate;
	char* deletedwith;
	int valid;
} trash_metadata;*/
file_information* read_directory(char* toread, int* fileAmount);
void display_modal(char* toDisplay);
void doNothing();
char* getTrashPath();
char* getInfoPath();
int remove_directory(char* path);
void free_fileinformation(file_information* filearray, int numFiles);
char* getContainerAndFree(char* fullPath);
char* show_directory(char* location, int* directoryMovement);
void getmetadata(char* filename);
char* getoriginalpath(char* filename);
void display_help();
int getColor(char* color);
int confirm_action(char* message);
int removeMetadata(char* filename);
const char* const helpMessages[] = {"UP/DOWN: Moves", "h/?: Help (this page)", "q: Quit", "RIGHT: select directory", "r/Enter: Restore", "l: Reload file list", "LEFT: Up directory (if applicable)", "i: File information", "p: Display path to trash", "e: Empty trash", "c: Open config"};
char* const fileTypes[] = {": Unknown(0)", ": FIFO", ": Character device", ": Unknown(3)", ": Directory", ": Unknown(5)", ": Block device", ": Unknown(7)", ": File", ": Unknown(9)", ": Symbolic link", ": Unknown(B)", ": Socket"};
#define helpMessageNumber 11
