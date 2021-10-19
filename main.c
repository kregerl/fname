#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

typedef struct {
	char* first;
	char* second;
} str_pair_t;

str_pair_t str_split(char* str, char delimeter);
str_pair_t get_filename_from_path(char* path);

int main(int argc, char** argv) {
	
	opterr = 0;

	int option;
	int filenames_length = 1;
	bool a_flag = false;
	bool p_flag = false;
	bool e_flag = false;
	char** filenames = malloc(filenames_length * sizeof(char*));
	char* append_text = NULL;
	char* prepend_text = NULL;
	char usage[] = "usage: %s filename [-a append_text] [-p prepend_text] -e\n";


	while ((option = getopt(argc, argv, "a:p:eh")) != -1) {
		switch(option) {
			case 'a': {	
				a_flag = true;
				append_text = optarg;
			} break;
			case 'p': {
				p_flag = true;
				prepend_text = optarg;
			} break;
			case 'e': {
				e_flag = true;
			} break;
			case 'h': {
				printf(usage, argv[0]);
				exit(0);
			} break;
			case '?': {
				if (optopt == 'a' || optopt == 'p') {
					fprintf(stderr, "%s: missing string after operand '-%c'\n", argv[0], optopt);
					printf(usage, argv[0]);
					exit(1);
				} else {
					fprintf(stderr, "%s: unknown argument: %c\n", argv[0], optopt);
					printf(usage, argv[0]);
					exit(1);
				}
			} break;
		}
	}

	if (!a_flag && !p_flag) {
		fprintf(stderr, "%s: missing arguments\n", argv[0]);
		printf(usage, argv[0]);
		exit(1);
	}
	if (e_flag && !a_flag) {
		fprintf(stderr, "%s: [-e] cannot be specified without [-a]\n", argv[0]);
		printf(usage, argv[0]);
		exit(1);
	}

	if (argc > optind) {
		filenames[0] = argv[optind];
		int difference = argc - optind;
		if (difference > 0) {
			filenames_length += difference;
			filenames = realloc(filenames, filenames_length * sizeof(char*));
		}
		for (int i = 0; i < difference; i++) {
			struct stat st;
			int ret = stat(argv[optind + i], &st);
			if (ret == -1) {
				fprintf(stderr, "%s: argument '%s' is not a file\n", argv[0], argv[optind + i]);
				exit(1);

			} else {
				filenames[i] = argv[optind + i];
			}
			filenames = realloc(filenames, filenames_length * sizeof(char*));
		}
	} else {
		printf("%s: missing filename\n", argv[0]); 
		printf(usage, argv[0]);
		exit(1);
	}

	for (int i = 0; i < filenames_length - 1; i++) {
		str_pair_t name = get_filename_from_path(filenames[i]);
		str_pair_t result = str_split(name.second, '.');
		size_t total_size = 1;
		
		if (name.first) {
			total_size += strlen(name.first);
		}
		if (result.first) {
			total_size += strlen(result.first);
		}
		if (result.second) {
			total_size += strlen(result.second);
		}
		if (append_text) {
			total_size += strlen(append_text);
		}
		if (prepend_text) {
			total_size += strlen(prepend_text);
		}

		char* buffer = malloc(total_size * sizeof(char));
		memset(buffer, 0, total_size);

		if (name.first) {
			strcat(buffer, name.first);
			free(name.first);
		}

		if (p_flag) {
			strcat(buffer, prepend_text);
		}
		if (result.first) {
			strcat(buffer, result.first);
			free(result.first);
		}
		if (e_flag) {
			if (result.second) {
				strcat(buffer, result.second);
				free(result.second);
			}
			if (a_flag) {
				strcat(buffer, append_text);
			}	
		} else {
			if (a_flag) {
				strcat(buffer, append_text);
			}
			if (result.second) {
				strcat(buffer, result.second);
				free(result.second);
			}

		}
		if (name.second) {
			free(name.second);
		}
		int ret = rename(filenames[i], buffer);

		if (ret == 0) {
			printf("Renamed file <%s> to: %s\n", filenames[i],  buffer);
		} else {
			printf("Error: Could not rename file");
		}
			
		free(buffer);
	}

	free(filenames);
}


str_pair_t str_split(char* str, char delimeter) {
	str_pair_t result;
	result.first = NULL;
	result.second = NULL;
	size_t len = strlen(str);

	size_t last_delim_idx = -1;
	for (int i = 0; i < len; i++) {
		if (str[i] == delimeter) {
			last_delim_idx = i;
		}
	}
	result.first = malloc((len - last_delim_idx + 1) * sizeof(char));
	strncpy(result.first, str, last_delim_idx);
	result.first[last_delim_idx] = '\0';

	result.second = malloc((last_delim_idx + 1) * sizeof(char));
	for (int i = 0; i < last_delim_idx; i++) {
		result.second[i] = str[last_delim_idx + i];
	}
	result.second[last_delim_idx] = '\0';
	
	return result;
}

str_pair_t get_filename_from_path(char* path) {
	char* path_copy = malloc((strlen(path) + 1) *  sizeof(char));
	strcpy(path_copy, path);
	path_copy[strlen(path)] = '\0';

	str_pair_t result;
	result.first = NULL;
	result.second = NULL;
	char* token = strtok(path_copy, "/");
	char* last_token = token;
	while (token != NULL) {
		last_token = token;
		token = strtok(NULL, "/");
	}
	int path_len = strlen(path) - strlen(last_token);
	result.first = calloc(path_len + 1, sizeof(char));
	result.first = strncpy(result.first, path, strlen(path) - strlen(last_token));
	result.first[path_len] = '\0';
	result.second = last_token;
	return result;
}





