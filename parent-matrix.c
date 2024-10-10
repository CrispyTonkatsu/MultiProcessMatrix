#include <errno.h>
#include <stdio.h> /* fopen, fscanf, fclose */
#include <stdlib.h> /* exit, malloc          */
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int *get_matrix(const char *filename, int *width) {
  int value, *matrix;
  FILE *fp;

  /* Open the file in text/translated mode */
  fp = fopen(filename, "rt");
  if (!fp) {
    printf("Can't open file: %s\n", filename);
    exit(-1);
  }

  /* Read the width and allocate the matrix */
  int res = fscanf(fp, "%d", width);
  if (res == -1) return NULL;
  matrix = malloc(*width * *width * sizeof(int));
  if (!matrix) {
    printf("Can't malloc matrix\n");
    fclose(fp);
    exit(-2);
  }

  /* Read the vaules and put in the matrix */
  while (!feof(fp)) {
    int result = fscanf(fp, "%d", &value);
    if (result == -1) break;
    *matrix++ = value;
  }
  fclose(fp);

  /* Return the address of the matrix */
  return matrix - (*width * *width);
}

void print_matrix(int *matrix, int width) {
  int i, size = width * width;
  for (i = 0; i < size; i++) {
    printf("%8i", matrix[i]);
    if ((i + 1) % width == 0) printf("\n");
  }
  printf("\n");
}

int main(int argc, char **argv) {
  const int param_length = 30;
  int width = 0; /* width of the matrix   */
  int *matrix = NULL; /* the matrix read in    */

  if (argc < 3) {
    printf("Insufficient parameters supplied\n");
    return -1;
  }

  /* read in matrix values from file */
  /* don't forget to free the memory */
  matrix = get_matrix(argv[1], &width);

  /* print the matrix */
  print_matrix(matrix, width);

  // My unique key: 17
  key_t key = ftok(argv[0], 17); // NOLINT *magic*

  /* Creating the shared memory*/
  int shared_mem = shmget(
      key,
      1 + 2 * (sizeof(int) * width * width),
      IPC_CREAT | IPC_EXCL | 0600); // NOLINT *magic*
  if (shared_mem == -1) {
    return errno;
  }

  int *shared_matrix = (int *) shmat(shared_mem, NULL, 0);
  if (shared_matrix == (void *) -1) {
    int error = shmctl(shared_mem, IPC_RMID, NULL);
    if (error == -1) {
      return errno;
    }
    printf("Failed to attach");
    return errno;
  }

  shared_matrix[0] = width;
  for (int i = 0; i < width * width; i++) {
    shared_matrix[i + 1] = matrix[i];
    shared_matrix[i + 1 + (width * width)] = 0;
  }

  /* Making an array to store the processes */
  pid_t *processes = calloc(width * width, sizeof(int));
  if (processes == NULL) {

    int error = shmctl(shared_mem, IPC_RMID, NULL);
    if (error == -1) {
      free(processes);
      return errno;
    }
  }

  /* Fork a child for each matrix entry       */
  for (int i = 0; i < width * width; i++) {
    pid_t cur_pid = fork();
    if (cur_pid == 0) {
      processes[i] = cur_pid;

      char mem_id[param_length];
      int string_error = sprintf(mem_id, "%i", shared_mem);
      if (string_error < 0) return -1;

      char id[param_length];
      string_error = sprintf(id, "%i", i);
      if (string_error < 0) return -1;

      char row[param_length];
      string_error = sprintf(row, "%i", i / width);
      if (string_error < 0) return -1;

      char col[param_length];
      string_error = sprintf(col, "%i", i % width);
      if (string_error < 0) return -1;

      int error = execl(argv[2], argv[2], mem_id, id, row, col, NULL);
      if (error == -1) {
        free(processes);
        error = shmctl(shared_mem, IPC_RMID, NULL);
        if (error == -1) {
          free(processes);
          return errno;
        }
        return errno;
      }
    }

    if (cur_pid == -1) {
      free(processes);
      return errno;
    }
  }

  /* wait for children*/
  for (int i = 0; i < width * width; i++) {
    wait(&processes[i]);
  }

  /* print matrix from shared buffer */
  print_matrix(shared_matrix + 1 + width * width, width); // NOLINT *magic*

  /* cleanup */
  int error = shmdt(shared_matrix);
  if (error == -1) {
    free(processes);
    return errno;
  }

  error = shmctl(shared_mem, IPC_RMID, NULL);
  if (error == -1) {
    free(processes);
    return errno;
  }

  free(processes);
  return 0;
}
