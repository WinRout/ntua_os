/*
 * mandel.c
 *
 * A program to draw the Mandelbrot Set on a 256-color xterm.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <semaphore.h>

#include <pthread.h>
#include <signal.h>

#include "mandel-lib.h"

#define MANDEL_MAX_ITERATION 100000

/***************************
 * Compile-time parameters *
 ***************************/

 typedef struct //new: thread struct
 {
         pthread_t tid;
         int current_line;
         sem_t mutex;
 }new_thread;

int threads; //new: total threads used
new_thread *thread_ptr; //new: array of new_thread (s)

/*
 * Output at the terminal is is x_chars wide by y_chars long
*/
int y_chars = 50;
int x_chars = 90;

/*
 * The part of the complex plane to be drawn:
 * upper left corner is (xmin, ymax), lower right corner is (xmax, ymin)
*/
double xmin = -1.8, xmax = 1.0;
double ymin = -1.0, ymax = 1.0;

/*
 * Every character in the final output is
 * xstep x ystep units wide on the complex plane.
 */
double xstep;
double ystep;

/*
 * This function computes a line of output
 * as an array of x_char color values.
 */
void compute_mandel_line(int line, int color_val[])
{
	/*
	 * x and y traverse the complex plane.
	 */
	double x, y;

	int n;
	int val;

	/* Find out the y value corresponding to this line */
	y = ymax - ystep * line;

	/* and iterate for all points on this line */
	for (x = xmin, n = 0; n < x_chars; x+= xstep, n++) {

		/* Compute the point's color value */
		val = mandel_iterations_at_point(x, y, MANDEL_MAX_ITERATION);
		if (val > 255)
			val = 255;

		/* And store it in the color_val[] array */
		val = xterm_color(val);
		color_val[n] = val;
	}
}

/*
 * This function outputs an array of x_char color values
 * to a 256-color xterm.
 */
void output_mandel_line(int fd, int color_val[])
{
	int i;

	char point ='@';
	char newline='\n';

	for (i = 0; i < x_chars; i++) {
		/* Set the current color, then output the point */
		set_xterm_color(fd, color_val[i]);
		if (write(fd, &point, 1) != 1) {
			perror("compute_and_output_mandel_line: write point");
			exit(1);
		}
	}

	/* Now that the line is done, output a newline character */
	if (write(fd, &newline, 1) != 1) {
		perror("compute_and_output_mandel_line: write newline");
		exit(1);
	}
}

void* compute_and_output_mandel_line(void* arg) //changed
{
	int line = *(int*) arg;
	/*
	 * A temporary array, used to hold color values for the line being drawn
	 */
	int color_val[x_chars];
	int i;
	for(i = line; i < y_chars; i += threads)
	{
		compute_mandel_line(i, color_val);
		sem_wait(&thread_ptr[(i % threads)].mutex);
		output_mandel_line(STDOUT_FILENO, color_val);
		sem_post(&thread_ptr[((i % threads)+1) % threads].mutex);
	}
	return NULL;
}

//function used in signal(SIGINT, sighandler) in main()
void sighandler()
{
  reset_xterm_color(1); //reset terminal color
  printf("%s", "\n");   //change line in terminal
  exit(1);              //exit program
}

int main(int argc, char* argv[])
{

  //if termination signal is detected go to sighandler
  signal(SIGINT, sighandler);

	int line;

	threads = atoi(argv[1]); //new: read number of threads

	xstep = (xmax - xmin) / x_chars;
	ystep = (ymax - ymin) / y_chars;

	thread_ptr = (new_thread *)malloc(threads * sizeof(new_thread)); //new: make space in heap
	if (thread_ptr == NULL)
	{
		fprintf(stderr, "Not enough memory for allocation.\n");
		exit(1);
	}

	if ((sem_init(&thread_ptr[0].mutex, 0, 1)) == -1) //new: intialize first lock
	{
		fprintf(stderr, "Semaphore.\n");
		exit(1);
	}


	for (line = 1; line < threads; line++) //new: initialize "locks"
	{
		if ((sem_init(&thread_ptr[line].mutex, 0, 0)) == -1)
		{
			fprintf(stderr, "Semaphore.\n");
			exit(1);
		}
	}


	for (line = 0; line < threads; line++) //new: create threads
	{
		thread_ptr[line].current_line = line;
		if ((pthread_create(&thread_ptr[line].tid, NULL,
					compute_and_output_mandel_line, &thread_ptr[line].current_line)) != 0)
					{
						/* The pthread_create function returns 0 on success */
						fprintf(stderr, "Error at thread(s) creation.\n");
						exit(1);
					}
	}



	for (line = 0; line < threads; line++) //new: termination of threads
	{
		if ((pthread_join(thread_ptr[line].tid, NULL)) != 0)
		{
			/* As pthread_create, pthread_join function returns 0 on success */
			fprintf(stderr, "Error at thread(s) termination.\n");
			exit(1);
		}
	}

	for (line = 0; line < threads; line++) //new: destroy semaphores
	{
		sem_destroy(&thread_ptr[line].mutex);
	}
	free(thread_ptr);
	reset_xterm_color(1);
	return 0;
}
