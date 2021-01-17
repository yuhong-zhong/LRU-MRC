#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <unordered_map>
#include <string>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: %s <input file> <output file>\n", argv[0]);
    exit(1);
  }

  char *input_file_path = argv[1];
  char *output_file_path = argv[2];

  FILE *input_fp;
  FILE *output_fp;

  input_fp = fopen(input_file_path, "r");
  if (input_fp == NULL) {
    printf("cannot open input file\n");
    exit(1);
  }

  output_fp = fopen(output_file_path, "w");
  if (output_fp == NULL) {
    printf("cannot open output file\n");
    exit(1);
  }

  char *buf = NULL;
  ssize_t read_size;
  size_t buf_size;

  std::unordered_map<std::string, size_t> id_map;
  size_t max_id = 1;

  while ((read_size = getline(&buf, &buf_size, input_fp)) != -1) {
    char *token = strtok(buf, ",");
    int token_index = 0;
    while (token != NULL) {
      if (token_index != 0) {
        fprintf(output_fp, ",");
      }
      if (token_index == 1) {
        std::string key(token);
        if (id_map.find(key) != id_map.end()) {
          fprintf(output_fp, "%ld", id_map.at(key));
        } else {
          id_map.insert(std::make_pair(key, max_id));
          fprintf(output_fp, "%ld", max_id);
          ++max_id;
        }
      } else {
        fprintf(output_fp, "%s", token);
      }

      token = strtok(NULL, ",");
      ++token_index;
    }
  }
}

