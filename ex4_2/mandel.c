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

/*TODO header file for m(un)map*/

#include <sys/mman.h>
//#include <singnal.h>


#include "mandel-lib.h"

#define MANDEL_MAX_ITERATION 100000

int NPROCS;
sem_t *sem;

/***************************
 * Compile-time parameters *
 ***************************/

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

void compute_and_output_mandel_line(int child_nr)
{
        /*
         * A temporary array, used to hold color values for the line being drawn
         */
        int color_val[x_chars],line;

        for (line =child_nr ; line < y_chars; line += NPROCS) {
                compute_mandel_line(line, color_val);

                sem_wait(&sem[line % NPROCS]);
                output_mandel_line(1, color_val);
                sem_post(&sem[(line % NPROCS + 1) % NPROCS]);
           }
}
/*
 * Create a shared memory area, usable by all descendants of the calling
 * process.
 */
void *create_shared_memory_area(unsigned int numbytes)
{
        int pages;
        void *addr;

        if (numbytes == 0) {
                fprintf(stderr, "%s: internal error: called for numbytes == 0\n", __func__);
                exit(1);
        }

        /*
         * Determine the number of pages needed, round up the requested number of
         * pages
         */
        pages = (numbytes - 1) / sysconf(_SC_PAGE_SIZE) + 1;

        /* Create a shared, anonymous mapping for this number of pages */
        /* TODO:  
                addr = mmap(...)
        */
        addr = mmap(NULL,pages * sysconf(_SC_PAGE_SIZE), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,-1 , 0);
        return addr;
}

void destroy_shared_memory_area(void *addr, unsigned int numbytes) {
        int pages;

        if (numbytes == 0) {
                fprintf(stderr, "%s: internal error: called for numbytes == 0\n", __func__);
                exit(1);
        }

        /*
         * Determine the number of pages needed, round up the requested number of
         * pages
         */
        pages = (numbytes - 1) / sysconf(_SC_PAGE_SIZE) + 1;

        if (munmap(addr, pages * sysconf(_SC_PAGE_SIZE)) == -1) {
                perror("destroy_shared_memory_area: munmap failed");
                exit(1);
        }
}

int main(int argc,char **argv)
{
        int ret , i, status;
        pid_t *pid;
        int size;

        NPROCS = atoi(argv[1]);

        xstep = (xmax - xmin) / x_chars;
        ystep = (ymax - ymin) / y_chars;

        pid = (pid_t *)malloc(NPROCS*sizeof(pid_t));
        size = NPROCS*sizeof(sem_t);
        sem = create_shared_memory_area(size);


        //Initialize semaphores
        for(i=0; i < NPROCS; i++){

                ret = sem_init(&sem[i], 1, 0);
                if (ret) {
                        perror("process_create");
                        exit(1);
                }

        }

        sem_post(&sem[0]);

        for(i=0; i < NPROCS; i++) {
                pid[i]=fork();

                if(pid[i]<0) {
                        perror("fork");
                        exit(1);
                }


                if(pid[i] == 0) {
                        compute_and_output_mandel_line(i);
                        exit(1);
                }

        }


        while(wait(&status)>0)
        destroy_shared_memory_area(sem ,size);


        reset_xterm_color(1);
        return 0;
}

