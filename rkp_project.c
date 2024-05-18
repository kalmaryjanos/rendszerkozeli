#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include "project.h"
#include <signal.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <omp.h>


int main(int argc, char *argv[]) 
{
    srand(time(NULL));

    time_t T1;
    int T2 = time(&T1);
    struct tm *T4;

    T4 = localtime(&T1);
    int min = (*T4).tm_min;

    //printf("perc: %d\n",(*T4).tm_min);

    // 2. feladat perc kondiciok

    int result;
    int result_1;

    if (min > 0 && min < 15)
    {
        result = ((min - 0) * 60) + (*T4).tm_sec;
    }
    
    else if (min > 15 && min < 30)
    {
        result = ((min - 15) * 60) + (*T4).tm_sec;
    }

    else if (min > 30 && min < 45)
    {
        result = ((min - 30) * 60) + (*T4).tm_sec;
    }

    else if (min > 45)
    {
        result = ((min - 45) * 60) + (*T4).tm_sec;
    }

    else 
    {
        result = (*T4).tm_sec;
    }
    

    //printf("%d\n", result);

    if (result <= 100)
    {
        result_1 = 100;
    }

    else if (result > 100)
    {
        result_1 = result;
    }
    
    
    //printf("%d\n", (*T4).tm_sec);
    int *res_point = &result_1;
    int pcs_of_values;

    
    int arguments_1 = 0;    // elso parancssori argumentum
    int arguments_2 = 0;    // masodik parancssori argumentum
    
    signal(SIGUSR1, SignalHandler);
    signal(SIGINT, SignalHandler);

    // 1. parancssori argumetum kezeles
    if (strcmp(argv[0], "./chart") != 0)
    {
        fprintf(stderr, "Futtatható állomány neve nem megfelelő!\n");
        return 1;
    }
    
    if (argc == 2) 
    { 
        if (strcmp(argv[1], "--version") == 0) {
            #pragma omp parallel sections
            {
                #pragma omp section
                {
                printf("1.0\n");
                }
                #pragma omp section
                {
                printf("2024.05.02.\n");
                }
                #pragma omp section
                {
                printf("Kalmár János\n");
                }
            }

            return EXIT_SUCCESS;
        }

        else if (strcmp(argv[1], "--help") == 0)
        {
            printf(" Lehetséges parancssori argumetumok száma: 1 || 2\n --version: program verziószáma, elkészültének dátuma, fejlsztő neve\n --help: segédlet\n Kétféle üzemmód: -send(küldő), -receive(fogadó) (előbbi az alapértelmezett)\n Kétféle kommunikációs mód: -file, -socket (előbbi az alapértelmezett)\n");
            return EXIT_SUCCESS;
        }

        else if (strcmp(argv[1], "-send") == 0)
        {
            //printf("#sendben illetve fájlban van\n");
            pcs_of_values = Measurment(&res_point);
            //printf("darabszam: %d\n", pcs_of_values);

            SendViaFile(res_point, pcs_of_values);
            //ReceiveViaFile();
            //BMPcreator(res_point, pcs_of_values);
            free(res_point);
            return EXIT_SUCCESS;
            
        }

        else if (strcmp(argv[1], "-receive") == 0)
        {
            //printf("#receiveben illetve fájlban van\n");
            signal(SIGUSR1, ReceiveViaFile);
            while (1)
            {
                pause();
            }
            
        }

        else if (strcmp(argv[1], "-file") == 0)
        {
            //printf("#sendben illetve fájlban van\n");

            pcs_of_values = Measurment(&res_point);
            //printf("darabszam: %d\n", pcs_of_values);
            SendViaFile(res_point, pcs_of_values);
            free(res_point);
            return EXIT_SUCCESS;
            
        }

        else if (strcmp(argv[1], "-socket") == 0)
        {
            //printf("#sendben illetve socket van\n");

            pcs_of_values = Measurment(&res_point);
            //printf("darabszam: %d\n", pcs_of_values);
            SendViaSocket(res_point, pcs_of_values);
            free(res_point);
            return EXIT_SUCCESS;

            
        }

        else
        {
            fprintf(stderr, " Lehetséges parancssori argumetumok száma: 1 || 2\n --version: program verziószáma, elkészültének dátuma, fejlsztő neve\n --help: segédlet\n Kétféle üzemmód: -send(küldő), -receive(fogadó) (előbbi az alapértelmezett)\n Kétféle kommunikációs mód: -file, -socket (előbbi az alapértelmezett)\n");
            return EXIT_SUCCESS;
        }
        
        
    }
        
    else if (argc == 3 && (strcmp(argv[1], argv[2]) != 0))
    {
        if ((strcmp(argv[1], "-send") == 0) || (strcmp(argv[1], "-receive") == 0))
        {
            arguments_1 = arguments_1 + 5;
        }

        if ((strcmp(argv[2], "-send") == 0) || (strcmp(argv[2], "-receive") == 0))
        {
            arguments_1 = arguments_1 + 5;
        }

        if (((strcmp(argv[1], "-file") == 0) || (strcmp(argv[1], "-socket") == 0)) || ((strcmp(argv[2], "-file") == 0) || (strcmp(argv[2], "-socket") == 0)))
        {
            arguments_2 = arguments_2 + 5;
        }
        

        if (arguments_1 != 5)
        {
            fprintf(stderr, " Lehetséges parancssori argumetumok száma: 1 || 2\n --version: program verziószáma, elkészültének dátuma, fejlsztő neve\n --help: segédlet\n Kétféle üzemmód: -send(küldő), -receive(fogadó) (előbbi az alapértelmezett)\n Kétféle kommunikációs mód: -file, -socket (előbbi az alapértelmezett)\n");
            return EXIT_SUCCESS;
        }

        if (arguments_2 != 5)
        {
            fprintf(stderr, " Lehetséges parancssori argumetumok száma: 1 || 2\n --version: program verziószáma, elkészültének dátuma, fejlsztő neve\n --help: segédlet\n Kétféle üzemmód: -send(küldő), -receive(fogadó) (előbbi az alapértelmezett)\n Kétféle kommunikációs mód: -file, -socket (előbbi az alapértelmezett)\n");
            return EXIT_SUCCESS;
        }

        if (((strcmp(argv[1], "-send") == 0) && (strcmp(argv[2], "-file") == 0)) || ((strcmp(argv[1], "-file") == 0) && (strcmp(argv[2], "-send") == 0)))   // -send -file eset
        {
            //printf("#sendben és fileban vagyunk\n");

            pcs_of_values = Measurment(&res_point);
            //printf("darabszam: %d\n", pcs_of_values);
            SendViaFile(res_point, pcs_of_values);
            free(res_point);
            return EXIT_SUCCESS;
        }

        else if (((strcmp(argv[1], "-send") == 0) && (strcmp(argv[2], "-socket") == 0)) || ((strcmp(argv[1], "-socket") == 0) && (strcmp(argv[2], "-send") == 0)))  // -send -socket
        {
            //printf("#sendben és socketben vagyunk\n");

            pcs_of_values = Measurment(&res_point);
            //printf("darabszam: %d\n", pcs_of_values);
            SendViaSocket(res_point, pcs_of_values);
            free(res_point);
            return EXIT_SUCCESS;

            
        }

        else if (((strcmp(argv[1], "-receive") == 0) && (strcmp(argv[2], "-socket") == 0)) || ((strcmp(argv[1], "-socket") == 0) && (strcmp(argv[2], "-receive") == 0)))    // -receive -socket
        {
            //printf("#receiveben és socketben vagyunk\n");
            ReceiveViaSocket();
        }
        
        else if (((strcmp(argv[1], "-receive") == 0) && (strcmp(argv[2], "-file") == 0)) || ((strcmp(argv[1], "-file") == 0) && (strcmp(argv[2], "-receive") == 0)))    // -receive -file
        {
            //printf("#receiveben és fileban vagyunk\n");
            signal(SIGUSR1, ReceiveViaFile);
            while (1)
            {
                pause();
            }
        }
        
        
           
    }
    else
    {
        fprintf(stderr, " Lehetséges parancssori argumetumok száma: 1 || 2\n --version: program verziószáma, elkészültének dátuma, fejlsztő neve\n --help: segédlet\n Kétféle üzemmód: -send(küldő), -receive(fogadó) (előbbi az alapértelmezett)\n Kétféle kommunikációs mód: -file, -socket (előbbi az alapértelmezett)\n");
        return EXIT_SUCCESS;
    }
// - -  -  -   -   -   -   -   -   -   -   -   -   -   -   -   -   --  -   -   -   -   -   -   -   -   -   -   -   -


    
   
    
    return EXIT_SUCCESS;
}