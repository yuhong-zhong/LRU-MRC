#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unordered_map>
#include "avl_tree.h"

struct request_t {
	bool valid;
	ssize_t id;
	ssize_t key_size;
	ssize_t value_size;
};

void twitter_parse_request(char *buf, request_t *request) {
	request->valid = true;
	char *token = strtok(buf, ",");
	int token_index = 0;
	while (token != NULL) {
		switch (token_index) {
			case 1:
				request->id = atoll(token);
			case 2:
				request->key_size = atoll(token);
			case 3:
				request->value_size = atoll(token);
		}
		token = strtok(NULL, ",");
		++token_index;
	}
}

int main(int argc, char *argv[]) {
	if (argc < 4) {
		fprintf(stderr, "Usage: %s <step size> <upper bound> <trace files>\n", argv[0]);
		exit(1);
	}
	int num_files = argc - 3;
	ssize_t step_size = atoll(argv[1]);
	ssize_t upper_bound = atoll(argv[2]);

	size_t *histogram = (size_t *) malloc(sizeof(size_t) * (2 + upper_bound / step_size));
	memset(histogram, 0, sizeof(size_t) * (2 + upper_bound / step_size));
	avl_tree<ssize_t, ssize_t> seq_to_size_tree;
	unordered_map<ssize_t, ssize_t> id_to_seq_map;

	char *buf = NULL;
	ssize_t read_size;
	size_t buf_size;

	ssize_t cur_seq = 0;
	for (int file_index = 0; file_index < num_files; ++file_index) {
		/* open file */
		FILE *fp;
		fp = fopen(argv[3 + file_index], "r");
		if (fp == NULL) {
			fprintf(stderr, "cannot open file %s\n", argv[3 + file_index]);
			exit(1);
		}

		while ((read_size = getline(&buf, &buf_size, fp)) != -1) {
			/* read one request from the file */
			request_t request;
			twitter_parse_request(buf, &request);
			if (!request.valid) {
				continue;
			}

			/* calculate reuse distance */
			if (id_to_seq_map.find(request.id) != id_to_seq_map.end()) {
				ssize_t seq = id_to_seq_map.at(request.id);
				ssize_t reuse_distance = seq_to_size_tree.sum_value_greater(seq, false);
				if (reuse_distance / step_size > upper_bound / step_size) {
					++histogram[1 + upper_bound / step_size];
				} else {
					++histogram[reuse_distance / step_size];
				}

				id_to_seq_map.erase(request.id);
				seq_to_size_tree.remove(seq);
			} else {
				++histogram[1 + upper_bound / step_size];
			}
			id_to_seq_map.insert(make_pair(request.id, cur_seq));
			seq_to_size_tree.insert(cur_seq, request.key_size + request.value_size);

			++cur_seq;
			if (cur_seq % 1000000 == 0) {
				fprintf(stderr, "cur_seq: %ld\r", cur_seq);
			}
		}
		fclose(fp);
	}
	fprintf(stderr, "\n");

	for (ssize_t i = 0; i < 2 + upper_bound / step_size; ++i) {
		printf("%ld\n", histogram[i]);
	}
}

