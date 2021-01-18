#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <unordered_map>
#include <string>

#define FILE_NAME_LEN 1024

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: %s <input file>\n", argv[0]);
		exit(1);
	}

	char *input_file_path = argv[1];
	FILE *input_fp;
	input_fp = fopen(input_file_path, "r");
	if (input_fp == NULL) {
		printf("cannot open input file\n");
		exit(1);
	}

	char *buf = NULL;
	ssize_t read_size;
	size_t buf_size;

	std::unordered_map<ssize_t, FILE *> file_map;

	while ((read_size = getline(&buf, &buf_size, input_fp)) != -1) {
		ssize_t time, app_id, key_size, value_size, key;

		char *token = strtok(buf, ",");
		int token_index = 0;
		while (token != NULL) {
			switch (token_index) {
				case 0:
					time = atoll(token);
					break;
				case 1:
					app_id = atoll(token);
					break;
				case 3:
					key_size = atoll(token);
					break;
				case 4:
					value_size = atoll(token);
					if (value_size < 0) {
						value_size = 0;
					}
					break;
				case 5:
					key = atoll(token);
					break;
			}

			token = strtok(NULL, ",");
			++token_index;
		}

		if (file_map.find(app_id) == file_map.end()) {
			char file_name[FILE_NAME_LEN];
			sprintf(file_name, "m.%ld.out", app_id);
			FILE *file = fopen(file_name, "w");
			if (!file) {
				fprintf(stderr, "cannot open output file\n");
				exit(1);
			}
			file_map.insert(std::make_pair(app_id, file));
		}
		FILE *file = file_map.at(app_id);
		fprintf(file, "%ld,%ld,%ld,%ld\n", time, key, key_size, value_size);
	}

	for (auto i : file_map) {
		fclose(i.second);
	}
	return 0;
}

