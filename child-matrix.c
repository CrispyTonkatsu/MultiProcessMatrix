#include <errno.h>
#include <stdio.h> /* fopen, fscanf, fclose */
#include <stdlib.h> /* exit, malloc          */
#include <sys/shm.h>

int indices_to_pos(int row, int col, int width) {
  return (row * width) + (col % width);
}

int main(int argc, char **argv) {
  if (argc < 5) {
    printf("Insufficient parameters supplied\n");
    return -1;
  }

  int shared_mem = (int) strtol(argv[1], NULL, 0);
  int *shared_matrix = (int *) shmat(shared_mem, NULL, 0);
  if (shared_matrix == (void *) -1) {
    return errno;
  }

  int width = shared_matrix[0];
  int child_id = (int) strtol(argv[2], NULL, 0);
  int row = (int) strtol(argv[3], NULL, 0);
  int column = (int) strtol(argv[4], NULL, 0);

  int *matrix = shared_matrix + 1;
  int *write_pos = shared_matrix + 1 + width * width + child_id;
  for (int i = 0; i < width; i++) {
    int row_i = matrix[indices_to_pos(row, i, width)];
    int col_i = matrix[indices_to_pos(i, column, width)];
    *write_pos += row_i * col_i;
  }

  return 0;
}
