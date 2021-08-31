#include <pthread.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <iostream>

using namespace std;

#define POINTS 1000000

struct thread_data
{
    int thread_id;
    int inside = 0;
    int count = 0;
};

void *MonteCarlo(void *threadarg)
{
    struct thread_data *data;
    data = (struct thread_data *) threadarg;

    double x, y;
    int i;

    for(i = 0; i < data->count; i++)
    {
        x = ((double)rand()) / ((double)RAND_MAX);
        y = ((double)rand()) / ((double)RAND_MAX);

        x *= x;
        y *= y;

        if((x + y) < 1)
            data->inside++;
    }

    pthread_exit(NULL);
}


int main (int argc,  char *argv[]) {

    if(argc != 3)
        return 1;

    int threadcount = atoi(argv[1]);
    int points = atoi(argv[2]);

//    int threadcount =  10;
//    int points = 1000000;

    std::cout << "Threadcount: " << threadcount << std::endl;
    std::cout << "Points: " << points << std::endl;

    if(threadcount < 1 || threadcount > 10)
        return 1;

    if(points < 10 || points > 1000000)
        return 1;

    pthread_t threads[threadcount];
    struct thread_data data[threadcount];

    int s = (int)floor(points/threadcount);
    int s0 = s + points%threadcount;

    int rc;
    pthread_attr_t attr;
    void *status;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(int i = 0; i < threadcount - 1; i++)
    {
        data[i].count = s;
    }

    data[threadcount-1].count = points - (threadcount - 1) * s;


    srand(0);

    std::chrono::time_point<std::chrono::system_clock> start, end;

    start = std::chrono::system_clock::now();

    for(int i = 0; i < threadcount; i++)
    {
        rc = pthread_create(&threads[i], &attr, MonteCarlo, &data[i]);

        if(rc)
        {
            std::cout << "Error: Unable to create thread " << rc << std::endl;
            exit(-1);
        }
    }


    pthread_attr_destroy(&attr);

    for(int i = 0; i < threadcount; i++)
    {
        rc = pthread_join(threads[i], &status);

        if(rc)
        {
            std::cout << "Error: unable to join " << rc << std::endl;
            exit(-1);
        }
    }
    end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Time elapsed: " << elapsed.count() << std::endl;

    int sum = 0;

    for(int i = 0; i < threadcount; i++)
        sum += data[i].inside;

    double ratio = (double)sum/(double)points;
    double pi = 4 * ratio;

    printf("Calculated pi: %f\n", pi);
    printf("Delta pi: %f\n", pi-M_PI);

    pthread_exit(NULL);
}

