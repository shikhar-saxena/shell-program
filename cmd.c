#include "headers.h"
#include "config.h"
#include "utils.h"
#include "cmd.h"

const char* CMDS[CMD_SIZE] = {
	"exit", "pwd", "echo", "cd", "ls", "repeat"
};

int (*func[])(int, char**) = {&cmd_exit, &cmd_pwd, &cmd_echo, &cmd_cd, &ls, &repeat};

int execute(int argc, char** argv) {

	for(int i = 0; i < CMD_SIZE; i++)
	    if(strcmp(CMDS[i], argv[0]) == 0)
	        return (func[i])(argc, argv);

	return 0;
}

//-------------------------------------
// Shell Commands
// - Each command receives its arguments
// - Processes them
// - Returns status after processing
// - 0: success, 1: failure
// - At status = 1, terminate shell
//-------------------------------------


//-------------------------------------
// Shell Builtins
//-------------------------------------

int cmd_exit(int argc, char** argv) {
	return 1;
}

int cmd_pwd(int argc, char**argv) {
	// pwd will already be set so display on stdout
	printf("%s\n", pwd);
	return 0;
}

int cmd_echo(int argc, char** argv) {
	for(int i = 1; i < argc; i++)
		printf("%s ", argv[i]);

	printf("\n");
	return 0;
}

int cmd_cd(int argc, char** argv) {
	
	switch(argc) {
		case 1: 				// cd
			change_old_pwd(); 	// Set old_pwd to pwd
			strcpy(pwd, home); 	// cd to home
			return 0;
		
		case 2:
			// cd - or cd ~-
			if(strcmp(argv[1], "-") == 0 || strcmp(argv[1], "~-") == 0) {
				
				if(old_pwd != NULL) {
					// Swap pwd and old_pwd
					char* temp = old_pwd;
					old_pwd = pwd;
					pwd = temp;
				}
				else fprintf(stderr, "cd: OLDPWD not set\n");

				return 0;
			}

			// Path given as second argument		
			char* str;

			// Make Path
			str = (argv[1][0] == '~') ? make_path(argv[1]) : argv[1];

			// Call chdir function
			if(chdir(str) == -1)
				fprintf(stderr, "cd: %s: %s\n", argv[1], strerror(errno));
			
			else set_pwd(); // set pwd and old_pwd
			
			// free str if it was relative path
			if(argv[1][0] == '~') str_free(str);

			return 0;
		
		default:
			fprintf(stderr, "cd: too many arguments\n");
	}

	return 0;
}


//-------------------------------------
// Other Commands
//-------------------------------------

// Display month
void display_month(int mon) {
	switch(mon) {
		case 0: printf("Jan"); break;
		case 1: printf("Feb"); break;
		case 2: printf("Mar"); break;
		case 3: printf("Apr"); break;
		case 4: printf("May"); break;
		case 5: printf("Jun"); break;
		case 6: printf("Jul"); break;
		case 7: printf("Aug"); break;
		case 8: printf("Sep"); break;
		case 9: printf("Oct"); break;
		case 10: printf("Nov"); break;
		case 11: printf("Dec");
	}
}

// To display modification time
void disp_time(struct timespec *st_mtim) {
	struct tm* local_time = localtime(&st_mtim->tv_sec);
	printf("\t");
	display_month(local_time->tm_mon);
	printf(" %d\t", local_time->tm_mday);

	// If within six months
	if(difftime(time(NULL), st_mtim->tv_sec) < 15552000) {
		// Display hour and minute
		printf("%2d:%2d ", local_time->tm_hour, local_time->tm_min);
	}
	else // Display year
		printf("%d ", local_time->tm_year);
}

// Displays l flag options
void ls_display_verbose(char* path, char* file_name) {

	struct stat stat_buf;
	stat(path, &stat_buf);

	// File type
	if(S_ISREG(stat_buf.st_mode)) printf("-");
	else if(S_ISDIR(stat_buf.st_mode)) printf("d");
	else if(S_ISCHR(stat_buf.st_mode)) printf("c");
	else if(S_ISFIFO(stat_buf.st_mode)) printf("p");
	else if(S_ISBLK(stat_buf.st_mode)) printf("b");
	else if(S_ISLNK(stat_buf.st_mode)) printf("l");
	else if(S_ISSOCK(stat_buf.st_mode)) printf("s");
	else printf("?");

	// File Mode
	printf((stat_buf.st_mode & S_IRUSR) ? "r" : "-");
    printf((stat_buf.st_mode & S_IWUSR) ? "w" : "-");
    printf((stat_buf.st_mode & S_IXUSR) ? "x" : "-");
    printf((stat_buf.st_mode & S_IRGRP) ? "r" : "-");
    printf((stat_buf.st_mode & S_IWGRP) ? "w" : "-");
    printf((stat_buf.st_mode & S_IXGRP) ? "x" : "-");
    printf((stat_buf.st_mode & S_IROTH) ? "r" : "-");
    printf((stat_buf.st_mode & S_IWOTH) ? "w" : "-");
    printf((stat_buf.st_mode & S_IXOTH) ? "x" : "-");

    // Number of hard links
    printf("\t%ld", stat_buf.st_nlink);

    // Owner and Group Name
    printf(" %s", getpwuid(stat_buf.st_uid)->pw_name);
    printf(" %s", getgrgid(stat_buf.st_gid)->gr_name);

	// File Size
	printf("\t%ld ", stat_buf.st_size);

	// Display modification time
	disp_time(&stat_buf.st_mtim);
}

// Displays ls output for file/directory 'file_name' at path 'path'
// Input: Flags about Options (whether set or not)
void ls_file_display(int a_flag, int l_flag, char* path, char* file_name) {

	if(!a_flag && file_name[0] == '.') return;

	if(l_flag) ls_display_verbose(path, file_name);

	// Display Name (ls default behaviour)
	printf("%s\n", file_name);
}

// Opens a DIR stream for directory 'path'
// Calls ls_file_display to display the ls information
void ls_dir(int a_flag, int l_flag, char* path) {

	DIR* open_path; 

	if((open_path = opendir(path)) == NULL) {
		fprintf(stderr, "ls: \'%s\': %s\n", path, strerror(errno));
		return;
	}

	struct dirent* content; // content in this directory/file
	while((content = readdir(open_path)) != NULL) {
		
		// Make path for each file_name in the stream
		size_t path_size = strlen(path);
		char* file_path = malloc(path_size + content->d_reclen + 1);

		size_t i;
		for(i = 0; i < path_size; i++) 
			file_path[i] = path[i];

		file_path[i++] = '/';

		for(; i <= content->d_reclen + path_size; i++)
			file_path[i] = content->d_name[i - path_size - 1];

		file_path[i] = '\0';

		// Call ls_file_display
		ls_file_display(a_flag, l_flag, file_path, content->d_name);

		free(file_path);
	}

	closedir(open_path);
}

int ls(int argc, char** argv) {

	optind = 1; // Reset to 1 for reading
	
	int opt;
	int a_flag, l_flag; // Set flags if they are given in input

	a_flag = l_flag = 0;

	while((opt = getopt(argc, argv, "al")) != -1) {
		switch(opt) {
			case 'a': a_flag = 1; break;
			case 'l': l_flag = 1; break;
			case '?':
				// Invalid Option (since our options don't take arguments)
				fprintf(stderr, "ls: invalid option -- \'%c\'\n", optopt);
				return 0; // Don't proceed further
		}
	}

	// Flag to display directory name 
	int disp_dir_flag = 0; // Set if more than one argument
	char* path;

	if(optind == argc) { // ls <no_arguments>
		
		if(l_flag) {
			struct stat stat_buf;
			stat(pwd, &stat_buf);
			printf("total %ld\n", stat_buf.st_blocks);
		}
		ls_dir(a_flag, l_flag, pwd);
		return 0;
	}

	if(optind != argc - 1)  // More than One arguement
		disp_dir_flag = 1;

	for(; optind < argc; optind++)  {

		// Get absolute path
		path = (argv[optind][0] == '~') ? make_path(argv[optind]) : argv[optind];

		struct stat stat_buf;

		// Check if path is file or directory
		if(stat(path, &stat_buf) == -1) {
			fprintf(stderr, "ls: \'%s\': %s\n", argv[optind], strerror(errno));
			continue;
		}

		if(S_ISDIR(stat_buf.st_mode)) {
			if(disp_dir_flag) // Display directory name
				printf("%s:\n", path);

			if(l_flag)
				printf("total %ld\n", stat_buf.st_blocks);
			
			ls_dir(a_flag, l_flag, path);
		}
		else // Call ls_file_display
			ls_file_display(a_flag, l_flag, path, path);

		if(optind != argc - 1) printf("\n");
		if(argv[optind][0] == '~') str_free(path);
	}

	return 0;
}

int repeat(int argc, char** argv) {
	if(argc < 3) {
		fprintf(stderr, "repeat: less arguments\n");
		return 0;
	}

	// Convert 2nd argument to numeric type
	long int val; char* end_ptr;
	val = strtol(argv[1], &end_ptr, 10);

	if((end_ptr == argv[1]) || (*end_ptr != '\0')){ // Invalid
		fprintf(stderr, "repeat: invalid argument \'%s\'\n", argv[1]);
		return 0;
	}

	// Execute the given command
	for(long int i = 1; i <= val; i++) {
		if(execute(argc - 2, (argv + 2)) == 1) return 1;
	}
	return 0;
}