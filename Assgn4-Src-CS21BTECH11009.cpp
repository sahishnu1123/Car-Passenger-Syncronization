#include<iostream>
#include<semaphore.h>
#include<pthread.h>
#include<unistd.h>
#include<fstream>
#include<random>
#include<chrono>

using namespace std;
using namespace std::chrono;

sem_t car, passenger;
sem_t current_active_binary_semaphore;
int current_active_passenger = -1;

double p_average = 0;
double c_average = 0;

sem_t c_average_binary_semaphore;

int p;

int ret1 = sem_init(&car, 1, 0);
int ret2 = sem_init(&passenger, 1, 0);
int ret3 = sem_init(&current_active_binary_semaphore, 1, 1);
int ret4 = sem_init(&c_average_binary_semaphore,1,1);

int P,C,k;

struct p_pass{

    int index;
    double lp;
    int iterations;
    int *current_passenger_of_car;
    int *passenger_total_time;
};

typedef struct p_pass* Ppass;

bool finish = false;

struct c_pass{

    int index;
    double lc;
    int iterations;
    int *current_passenger_of_car;
    int *passenger_total_time;
};

typedef struct c_pass* Cpass;

void *passengers_func(void *thread_pass){

    Ppass passed_variables = (Ppass)thread_pass;

    int i = passed_variables->index;
    double lp = passed_variables->lp;
    int k = passed_variables->iterations;
    int *passenger_total_time = passed_variables->passenger_total_time;

    auto start = high_resolution_clock::now();

    ofstream ofile;
    ofile.open("output.txt", ios::app);

    for(int j = 0; j < k; j++){

        struct timespec ts0;
        timespec_get(&ts0, TIME_UTC);

        time_t tmNow0;
        tmNow0 = time(NULL);
        struct tm t0 = *localtime(&tmNow0);

        ofile<<"Passenger "<<i+1<<" enters the museum at "<<t0.tm_hour<<":"<<t0.tm_min<<":"<<t0.tm_sec<<":"<<ts0.tv_nsec/1000000<<endl;

        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

        std::default_random_engine generator (seed);
        std::exponential_distribution<double> exponential((double)1000/lp);

        double wander_time = exponential(generator);

        usleep(wander_time*1000000);

        struct timespec ts1;
        timespec_get(&ts1, TIME_UTC);

        time_t tmNow1;
        tmNow1 = time(NULL);
        struct tm t1 = *localtime(&tmNow1);

        ofile<<"Passenger "<<i+1<<" made a ride request at "<<t1.tm_hour<<":"<<t1.tm_min<<":"<<t1.tm_sec<<":"<<ts1.tv_nsec/1000000<<endl;

        sem_post(&car);

        sem_wait(&current_active_binary_semaphore);
        current_active_passenger = i;

        struct timespec ts2;
        timespec_get(&ts2, TIME_UTC);

        time_t tmNow2;
        tmNow2 = time(NULL);
        struct tm t2 = *localtime(&tmNow2);
        
        sem_wait(&passenger);
    }
    p++;

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    passenger_total_time[i] = duration.count();

    cout<<"Passenger "<<i+1<<": "<<(double)passenger_total_time[i]/1000<<endl;

    ofile<<"Passenger "<<i+1<<" finished all "<<k<<" tours\n";
    ofile.close();

    if(p == P){

        for(int i = 0; i < P; i++){

            p_average = p_average + (double)(passenger_total_time[i]);
        }

        cout<<"Passenger average: "<<(p_average/(P*1000))<<endl;

        cout<<"Car average: "<<(1000*c_average/C)<<endl;

        finish = true;

        exit(0);
    }

    return NULL;
}

void *cars_func(void *thread_pass){

    Cpass passed_variables = (Cpass) thread_pass;

    int i = passed_variables->index;
    double lc = passed_variables->lc;
    int *current_passenger_of_car = passed_variables->current_passenger_of_car;
    int *passenger_total_time = passed_variables->passenger_total_time;

    while(1 && !finish){

        if(!finish){

            sem_wait(&car);
        }

        if(!finish){

            ofstream ofile;
            ofile.open("output.txt", ios::app);

            // while(current_active_passenger == -1);
            current_passenger_of_car[i] = current_active_passenger;
            sem_post(&current_active_binary_semaphore);
            ofile<<"Car "<<i+1<<" accepts Passenger "<<current_passenger_of_car[i]+1<<"'s request\n";

            struct timespec ts1;
            timespec_get(&ts1, TIME_UTC);

            time_t tmNow1;
            tmNow1 = time(NULL);
            struct tm t1 = *localtime(&tmNow1);
            ofile<<"Passenger "<<current_passenger_of_car[i]+1<<" started riding at "<<t1.tm_hour<<":"<<t1.tm_min<<":"<<t1.tm_sec<<":"<<ts1.tv_nsec/1000000<<endl;

            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

            std::default_random_engine generator (seed);
            std::exponential_distribution<double> exponential((double)1000/lc);

            double safari_time = exponential(generator);

            sem_wait(&c_average_binary_semaphore);
            c_average = c_average + safari_time;
            sem_post(&c_average_binary_semaphore);

            usleep(safari_time*1000000);

            struct timespec ts0;
            timespec_get(&ts0, TIME_UTC);

            time_t tmNow0;
            tmNow0 = time(NULL);
            struct tm t0 = *localtime(&tmNow0);

            ofile<<"Car "<<i+1<<" has finished Passenger "<<current_passenger_of_car[i]+1<<"'s tour.\n";

            ofile<<"Passenger "<<current_passenger_of_car[i]+1<<" finished riding at "<<t0.tm_hour<<":"<<t0.tm_min<<":"<<t0.tm_sec<<":"<<ts0.tv_nsec/1000000<<endl;


            // sem_wait(&current_active_binary_semaphore);
            // current_active_passenger = -1;
            // sem_post(&current_active_binary_semaphore);

            sem_post(&passenger);
            ofile.close();

        }
    }

    exit(0);
    
    return NULL;
}

int main(int argc, char *argv[]){

    double lp, lc;

    ifstream ipfile;

    ipfile.open(argv[1]);

    ipfile>>P>>C>>lp>>lc>>k;    

    ipfile.close();

    ofstream ofile;
    ofile.open("output.txt");
    ofile<<"";
    ofile.close();

    pthread_t passengers[P];
    pthread_t cars[C];

    int *passenger_total_time = (int *)malloc(sizeof(int) * P);

    int *current_passenger_of_car = (int *)malloc(sizeof(int) * P);

    for(int i = 0; i < C; i++){

        Cpass thread_pass = (Cpass) malloc(sizeof(struct c_pass));

        thread_pass->index = i;
        thread_pass->lc = lc;
        thread_pass->iterations = k;
        thread_pass->current_passenger_of_car = current_passenger_of_car;
        thread_pass->passenger_total_time = passenger_total_time;

        pthread_create(&cars[i], NULL, &cars_func, (void *)thread_pass);
    }

    for(int i = 0; i < P; i++){

        Ppass thread_pass = (Ppass) malloc(sizeof(struct p_pass));

        thread_pass->index = i;
        thread_pass->lp = lp;
        thread_pass->iterations = k;
        thread_pass->current_passenger_of_car = current_passenger_of_car;
        thread_pass->passenger_total_time = passenger_total_time;

        pthread_create(&passengers[i], NULL, &passengers_func, (void *)thread_pass);
    }

    for(int i = 0; i < P; i++){

        pthread_join(passengers[i], NULL);
    }

    cout<<"Biyatch";

    finish = true;

    sem_post(&car);

    for(int i = 0; i < P; i++){

        pthread_join(cars[i], NULL);
    }
        
    return 0;
}