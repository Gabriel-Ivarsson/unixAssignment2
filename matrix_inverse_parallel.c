/***************************************************************************
 *
 * Sequential version of Matrix Inverse
 * An adapted version of the code by Hkan Grahn
 ***************************************************************************/

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SIZE 4096

typedef double matrix[MAX_SIZE][MAX_SIZE];

int N;            /* matrix size                */
int maxnum;       /* max number of element*/
char *Init;       /* matrix init type   */
int PRINT;        /* print switch               */
matrix A;         /* matrix A           */
matrix I = {0.0}; /* The A inverse matrix, which will be initialized to the
                     identity matrix */

pthread_barrier_t barrier;
struct threadArgs {
  int id;
  int p;
  int col;
  int row;
  int pivalue;
  int multiplier;
};

/* forward declarations */
void find_inverse(void);
void Init_Matrix(void);
void Print_Matrix(matrix M, char name[]);
void Init_Default(void);
int Read_Options(int, char **);
void *child(void *params);
void matrix_to_identity(int p, int col, double pivalue);
void matrix_elimination(int p, int row, int col, double multiplier);
void parallel_find_inverse();
void *start_parallel_elimination(void *params);
void *start_parallel_identity(void *params);

int main(int argc, char **argv) {
  printf("Matrix Inverse\n");
  int i, timestart, timeend, iter;

  Init_Default();           /* Init default values      */
  Read_Options(argc, argv); /* Read arguments   */
  Init_Matrix();            /* Init the matrix  */

  // Sequential
  // printf("Starting Sequential\n");
  // find_inverse();
  // printf("Sequential done\n");

  Init_Default();           /* Init default values      */
  Read_Options(argc, argv); /* Read arguments   */
  Init_Matrix();            /* Init the matrix  */

  // Parallel
  printf("Starting parallel\n");
  parallel_find_inverse();
  printf("Parallel done\n");

  // print
  if (PRINT == 1) {
    Print_Matrix(A, "End: Input");
    Print_Matrix(I, "Inversed");
  }
}

void *child(void *params) {
  struct threadArgs *args = (struct threadArgs *)params;
  int id = args->id;
  int p = args->p;
  int col = args->col;
  int row = args->row;
  int pivalue = args->pivalue;
  int multiplier = args->multiplier;
  // printf("Proccess %d starting work...\n", id);
  matrix_to_identity(p, id, pivalue);
  pthread_barrier_wait(&barrier);
  free(args);
  // printf("Proccess %d done and freed.\n", id);
  return NULL;
}

void *start_parallel_identity(void *params) {
  struct threadArgs *args = (struct threadArgs *)params;
  int id = args->id;
  int p = args->p;
  int col = args->col;
  int row = args->row;
  int pivalue = args->pivalue;
  int multiplier = args->multiplier;
  // printf("Proccess %d starting work...\n", id);
  matrix_to_identity(p, id, pivalue);
  pthread_barrier_wait(&barrier);
  free(args);
  // printf("Proccess %d done and freed.\n", id);
  return NULL;
}

void *start_parallel_elimination(void *params) {
  struct threadArgs *args = (struct threadArgs *)params;
  int id = args->id;
  int p = args->p;
  int col = args->col;
  int row = args->row;
  int pivalue = args->pivalue;
  int multiplier = args->multiplier;
  // printf("Proccess %d starting work...\n", id);
  matrix_elimination(p, row, col, multiplier);
  pthread_barrier_wait(&barrier);
  free(args);
  // printf("Proccess %d done and freed.\n", id);
  return NULL;
}

void start_children(void *workerFunc, struct threadArgs *args) {
  pthread_t *children;
  int id = 0;
  pthread_barrier_init(&barrier, NULL, N);

  children = malloc(N * sizeof(pthread_t));
  for (id = 0; id < N; id++) {
    args = malloc(sizeof(struct threadArgs));
    args->id = id;
    args->col = id;
    pthread_create(&(children[id]), NULL, workerFunc, (void *)args);
  }
  for (id = 0; id < N; id++) {
    pthread_join(children[id], NULL);
  }
  free(children);
}

void parallel_find_inverse() {
  int row, p;     // 'p' stands for pivot (numbered from 0 to N-1)
  double pivalue; // pivot value

  /* Bringing the matrix A to the identity form */
  for (p = 0; p < N; p++) { /* Outer loop */
    pivalue = A[p][p];
    // create thread args and start worker threads with matrix_to_identity
    // function.
    struct threadArgs *identityArgs;
    identityArgs->pivalue = pivalue;
    identityArgs->p = p;
    start_children(start_parallel_identity, identityArgs);
    // printf("thread %d hit first barrier\n", col);
    pthread_barrier_wait(&barrier);
    assert(A[p][p] == 1.0);
    // Print_Matrix(A, "End: Input");
    // Print_Matrix(I, "Inversed");

    // Elimination
    double multiplier;
    for (row = 0; row < N; row++) {
      multiplier = A[row][p];
      if (row != p) // Perform elimination on all except the current pivot row
      {
        // create thread args and start worker threads with matrix_elimination
        // function.
        struct threadArgs *eliminationArgs;
        eliminationArgs->multiplier = multiplier;
        eliminationArgs->p = p;
        start_children(start_parallel_elimination, identityArgs);
        // printf("thread %d hit second barrier\n", col);
        assert(A[row][p] == 0.0);
      }
    }
  }
}

void matrix_to_identity(int p, int col, double pivalue) {
  A[p][col] = A[p][col] / pivalue; /* Division step on A */
  I[p][col] = I[p][col] / pivalue; /* Division step on I */
}

void matrix_elimination(int p, int row, int col, double multiplier) {
  A[row][col] =
      A[row][col] - A[p][col] * multiplier; /* Elimination step on A */
  I[row][col] =
      I[row][col] - I[p][col] * multiplier; /* Elimination step on I */
}

void find_inverse() {
  int row, col, p; // 'p' stands for pivot (numbered from 0 to N-1)
  double pivalue;  // pivot value

  /* Bringing the matrix A to the identity form */
  for (p = 0; p < N; p++) { /* Outer loop */
    pivalue = A[p][p];
    for (col = 0; col < N; col++) {
      A[p][col] = A[p][col] / pivalue; /* Division step on A */
      I[p][col] = I[p][col] / pivalue; /* Division step on I */
    }
    assert(A[p][p] == 1.0);
    Print_Matrix(A, "End: Input");
    Print_Matrix(I, "Inversed");

    // Elimination
    double multiplier;
    for (row = 0; row < N; row++) {
      multiplier = A[row][p];
      if (row != p) // Perform elimination on all except the current pivot row
      {
        for (col = 0; col < N; col++) {
          A[row][col] =
              A[row][col] - A[p][col] * multiplier; /* Elimination step on A */
          I[row][col] =
              I[row][col] - I[p][col] * multiplier; /* Elimination step on I */
        }
        assert(A[row][p] == 0.0);
      }
    }
  }
}

void Init_Matrix() {
  int row, col;

  // Set the diagonal elements of the inverse matrix to 1.0
  // So that you get an identity matrix to begin with
  for (row = 0; row < N; row++) {
    for (col = 0; col < N; col++) {
      if (row == col)
        I[row][col] = 1.0;
    }
  }

  printf("\nsize      = %dx%d ", N, N);
  printf("\nmaxnum    = %d \n", maxnum);
  printf("Init    = %s \n", Init);
  printf("Initializing matrix...");

  if (strcmp(Init, "rand") == 0) {
    for (row = 0; row < N; row++) {
      for (col = 0; col < N; col++) {
        if (row == col) /* diagonal dominance */
          A[row][col] = (double)(rand() % maxnum) + 5.0;
        else
          A[row][col] = (double)(rand() % maxnum) + 1.0;
      }
    }
  }
  if (strcmp(Init, "fast") == 0) {
    for (row = 0; row < N; row++) {
      for (col = 0; col < N; col++) {
        if (row == col) /* diagonal dominance */
          A[row][col] = 5.0;
        else
          A[row][col] = 2.0;
      }
    }
  }

  printf("done \n\n");
  if (PRINT == 1) {
    // Print_Matrix(A, "Begin: Input");
    // Print_Matrix(I, "Begin: Inverse");
  }
}

void Print_Matrix(matrix M, char name[]) {
  int row, col;

  printf("%s Matrix:\n", name);
  for (row = 0; row < N; row++) {
    for (col = 0; col < N; col++)
      printf(" %5.2f", M[row][col]);
    printf("\n");
  }
  printf("\n\n");
}

void Init_Default() {
  N = 5;
  Init = "fast";
  maxnum = 15.0;
  PRINT = 1;
}

int Read_Options(int argc, char **argv) {
  char *prog;

  prog = *argv;
  while (++argv, --argc > 0)
    if (**argv == '-')
      switch (*++*argv) {
      case 'n':
        --argc;
        N = atoi(*++argv);
        break;
      case 'h':
        printf("\nHELP: try matinv -u \n\n");
        exit(0);
        break;
      case 'u':
        printf("\nUsage: matinv [-n problemsize]\n \
          [-D] show default values \n \
          [-h] help \n \
          [-I init_type] fast/rand \n \
          [-m maxnum] max random no \n \
          [-P print_switch] 0/1 \n");
        exit(0);
        break;
      case 'D':
        printf("\nDefault:  n         = %d ", N);
        printf("\n          Init      = rand");
        printf("\n          maxnum    = 5 ");
        printf("\n          P         = 0 \n\n");
        exit(0);
        break;
      case 'I':
        --argc;
        Init = *++argv;
        break;
      case 'm':
        --argc;
        maxnum = atoi(*++argv);
        break;
      case 'P':
        --argc;
        PRINT = atoi(*++argv);
        break;
      default:
        printf("%s: ignored option: -%s\n", prog, *argv);
        printf("HELP: try %s -u \n\n", prog);
        break;
      }
}