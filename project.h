
#ifndef kill
#include<signal.h>
#endif

#ifndef sockaddr_in
#include <netinet/in.h>
#endif

#ifndef inet_addr
#include <arpa/inet.h>
#endif


void SignalHandler(int sig)
{
    if (sig == SIGINT)
    {
        printf("\nViszlát!\n");
        exit(0);
    }

    if (sig == SIGUSR1)
    {
        fprintf(stderr, "Fájlon keresztüli küldés szolgáltatás nem elérhető!\n");
        
    }
    
    if (sig == SIGALRM)
    {
        fprintf(stderr, "Nem érkezett válasz a szervertől a megadott időn belül.\n");
        exit(1);
    }
    
}


void SendViaSocket(int *Values, int NumValues)
{
    int PORT_NO = 3333;

    int test;
    int s;                            // socket ID
    int bytes;                        // received/sent bytes
    int flag;                         // transmission flag
    char on;                          // sockopt option
    unsigned int server_size;         // length of the sockaddr_in server
    struct sockaddr_in server;        // address of server

    on   = 1;
    flag = 0;
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port        = htons(PORT_NO);
    server_size            = sizeof server;

    s = socket(AF_INET, SOCK_DGRAM, 0 );
    if ( s < 0 ) 
    {
        fprintf(stderr, "Socket creation error.\n");
        exit(2);
    }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof on);

    
    bytes = sendto(s, &NumValues, sizeof(NumValues), flag, (struct sockaddr *) &server, server_size);
    if ( bytes <= 0 ) 
    {
        fprintf(stderr, "Sending error.\n");
        exit(3);
    }
    signal(SIGALRM,SignalHandler);
    alarm(1);

    //printf (" %d bájtot küldtem a szervernek.\n", bytes);

    
    bytes = recvfrom(s, &test, sizeof(test), flag, (struct sockaddr *) &server, &server_size);
    if ( bytes < 0 ) 
    {
        fprintf(stderr, ": Receiving error.\n");
        exit(4);
    }
    alarm(0);

    //printf(" Server (%s:%d) visszaküldött egy értéket:  %d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port), test);

    if (test != NumValues)
    {
        ("A küldött és a fogadott érték különbözik!\n");
        close(s);    
        exit(6);
    }
    
    int check_size = NumValues * sizeof(NumValues);

    bytes = sendto(s, Values, check_size, flag, (struct sockaddr *) &server, server_size);
    if ( bytes <= 0 ) 
    {
        fprintf(stderr, "Sending error.\n");
        exit(3);
    }
    //printf (" %d bájtot küldtem a szervernek.\n", bytes);
    //printf("%d\n", check_size);

    bytes = recvfrom(s, &test, sizeof(test), flag, (struct sockaddr *) &server, &server_size);
    if ( bytes < 0 ) 
    {
        fprintf(stderr, ": Receiving error.\n");
        exit(4);
    }

    //printf("%d bájtot foglalt le a server\n", test);

    if (check_size != test)
    {
        ("A küldött és a fogadott érték különbözik!\n");
        close(s);    
        exit(6);
    }
    
    
    
    close(s);
        
}

int FindPID()
{
    DIR *d = opendir("/proc");
    struct dirent *entry;
    char path[19];
    FILE *f;
    int irt_byte;
    char line[50];
    int needed_value = 0;
    int num;
    int r = 0;

    while ((entry = readdir(d)) != NULL) 
    {
        if(((*entry).d_name[0] > 47) && ((*entry).d_name[0] < 58)) 
        {
            //printf("%s\t", (*entry).d_name);
            irt_byte = snprintf(path, sizeof(path), "/proc/%s/status", (*entry).d_name);
           
            f = fopen(path, "r");
            while (fgets(line, sizeof(line), f) != NULL)
            {
                //printf("#%s ", line);
                if(strstr(line, "Name:\tchart") != NULL || needed_value == 1)
                {
                    //futhat tovabb
                    //printf("#%s ", line);
                    //printf("path: %s\n", path);
                    needed_value = 1; 
                }
                else
                {
                    break;
                }
                
                char * token = strtok(line, "\t");
                while( token != NULL ) 
                {
                    
                    char *actual_string = token;
                    //printf( " %s\n", actual_string );

                    if (needed_value == 2)
                    {
                        num = atoi(actual_string);
                        needed_value = 3;
                        if (num == getpid())
                        {
                            //fusson tovabb
                        }
                        
                        else
                        {
                            //printf("lefut?");
                            return num;
                        }
                        
                        
                    }
                    
                    if (strcmp(actual_string, "Pid:") == 0 )
                    {
                        //printf("path: %s\n", path);
                        needed_value = needed_value + 1;
                        //printf("ez lefut egyaltalan?\n");
                    }
                    token = strtok(NULL, " ");
                }
            
            } 

        }
    }
    //printf("pid: %d\n", num);

    //printf("%d\n", irt_byte);
    //printf("%s\n", path);
    closedir(d);

    return -1;
}


void SendViaFile(int *Values, int NumValues)
{
    char path[100];
    int irt_byte;
    int pid;
    irt_byte = snprintf(path, sizeof(path), "/home/%s/Measurement.txt", getenv("LOGNAME"));
    //printf("\nsprintf: %d", irt_byte);
    FILE *f = fopen(path, "w");

    for (int i = 0; i < NumValues; i++)
    {
        fprintf(f, "%d\n", Values[i]);
    }
    fclose(f);
    pid = FindPID();
    //printf("\n%d\n", pid);

    if (pid == -1)
    {
        fprintf(stderr, "Nem található fogadó üzemmódban működő folyamat\n");
        exit(8);
    }
    else
    {
        
        kill(pid, SIGUSR1);
    }
    
    
}


int bin_to_dec(char arr[])
{
    int res = 0;

    if (arr[0] == '1')
    {
        res = res + 1;
    }
    if (arr[1] == '1')
    {
        res = res + 2;
    }
     if (arr[2] == '1')
    {
        res = res + 4;
    }
     if (arr[3] == '1')
    {
        res = res + 8;
    }
     if (arr[4] == '1')
    {
        res = res + 16;
    }
     if (arr[5] == '1')
    {
        res = res + 32;
    }
     if (arr[6] == '1')
    {
        res = res + 64;
    }
     if (arr[7] == '1')
    {
        res = res + 128;
    }
    
    return res;

}

void dec_to_bin(char arr[], int pcs)
{
    int maradek = 1;

    for (int i = 0; i < 32; i++)
    {
        maradek = pcs % 2;
        pcs = pcs / 2;
        //printf("#temp: %d ", temp_res);
        

        if (maradek + 48 == 48) // ASCII 48 = '0'
        {
            arr[i] = '0';
        }
        else
        {
            arr[i] = '1';
        }
 

    }

}

void fill_array(char arr_1[], char arr_2[], char arr_3[], char arr_4[], char base_arr[])
{
    // minden tomb feltoltese egy bajttal
    for (int i = 0; i < 32; i++)
    {
        if (i < 8)
        {
            arr_1[i] = base_arr[i];
        }
        else if (i >= 8 && i < 16)
        {
            for (int j = 0; j < 8; j++)
            {
                arr_2[j] = base_arr[i + j];
            }
            i = i + 7;
        }

        else if (i >= 16 && i < 24)
        {
            for (int j = 0; j < 8; j++)
            {
                arr_3[j] = base_arr[i + j];
            }
            i = i + 7;
        }

        else if (i >= 24 && i < 32)
        {
            for (int j = 0; j < 8; j++)
            {
                arr_4[j] = base_arr[i + j];
            }
            i = i + 7;
        }
        
    }

}

// 3. feladat BMP letrehozo fuggveny
void BMPcreator(int *Values, int NumValues)
{
    char bits[32];
    char elso_negyed[8];
    char masodik_negyed[8];
    char harmadik_negyed[8];
    char negyedik_negyed[8];

    
    dec_to_bin(bits, NumValues);

    fill_array(elso_negyed, masodik_negyed, harmadik_negyed, negyedik_negyed, bits);

    // 2-bol --> 10-be
    int first_byte = bin_to_dec(elso_negyed);
    int second_byte = bin_to_dec(masodik_negyed);
    int third_byte = bin_to_dec(harmadik_negyed);
    int fourth_byte = bin_to_dec(negyedik_negyed);
    //printf("%d, %d, %d, %d", first_byte, second_byte, third_byte, fourth_byte);
    
    
    int px_arr_line_size = NumValues;

    // megnezzuk: az NumValues erteke oszthato 32-vel
    while (px_arr_line_size % 32 != 0)
    {
        px_arr_line_size++;
    }
    
    //printf("bovitett sorszam : %d\n", px_arr_line_size);
    
    dec_to_bin(bits, ((NumValues * px_arr_line_size) / 8) + 62);
    fill_array(elso_negyed, masodik_negyed, harmadik_negyed, negyedik_negyed, bits);
    int file_size_1 = bin_to_dec(elso_negyed);
    int file_size_2 = bin_to_dec(masodik_negyed);
    int file_size_3 = bin_to_dec(harmadik_negyed);
    int file_size_4 = bin_to_dec(negyedik_negyed);

    
    char bmp_header [] = {
        0x42, 0x4d, file_size_1, file_size_2, file_size_3, file_size_4, 0x00, 0x00, 0x00, 0x00, 
        0x3e, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, first_byte, second_byte, 
        third_byte, fourth_byte, first_byte, second_byte, third_byte, fourth_byte, 0x01, 0x00, 0x01, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x61, 0x0f, 
        0x00, 0x00, 0x61, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 
        0xff, 0xff
    };

    int px_array_size = (NumValues * px_arr_line_size) / 8;
    char px_array[px_array_size];
    
    
    int file_1 = open("chart.bmp", O_CREAT|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    
    
    write(file_1, bmp_header, 62);

     
    // feltoltjuk a "hatterszinnel"
    for (int i = 0; i < px_array_size; i++)
    {
        px_array[i] = 0;
    }


    int temp_i = 2;
    int startpoint = ((NumValues / 2) * px_arr_line_size) / 8;
    int nextpoint;
    int current_point = startpoint;
    int startValue = 128;
    int eight_counter = 0;
    

    px_array[startpoint] = startValue;

    int j = 1;
    for (int i = 0; i < NumValues - 1; i++)
    {
        switch (temp_i) {
            case 1:
                startValue = 128;
                temp_i++;
                break;
            case 2:
                startValue = 64;
                temp_i++;
                eight_counter = 0;
                break;
            case 3:
                startValue = 32;
                temp_i++;
                break;
            case 4:
                startValue = 16;
                temp_i++;
                break;
            case 5:
                startValue = 8;
                temp_i++;
                break;
            case 6:
                startValue = 4;
                temp_i++;
                break;
            case 7:
                startValue = 2;
                temp_i++;
                break;
            case 8:
                startValue = 1;
                temp_i = 1;
                break;
        }
        
        
        if (Values[j] > Values[i])
        {   

            // ha tulmenne felul
            if (Values[j] >= NumValues)
            {
                nextpoint = (NumValues * px_arr_line_size) / 8 + eight_counter;
                px_array[nextpoint] = px_array[nextpoint] + startValue;
            }
            
            //ha tulmenen alul
            if (Values[j] <= (-NumValues))
            {
                nextpoint = (1 * px_arr_line_size) / 8 + eight_counter;
                px_array[nextpoint] = px_array[nextpoint] + startValue;
            }

        
            
            nextpoint = (current_point + (px_arr_line_size / 8) + eight_counter);

            if (startValue == 1)
            {
                eight_counter++;
            }
            

            if (px_array[nextpoint] != 0)
            {
                px_array[nextpoint] = px_array[nextpoint] + startValue;
            }
            else
            {
                px_array[nextpoint] = startValue;
            }
            
            current_point = nextpoint;
        }
        
        
        if (Values[j] < Values[i])
        {
            nextpoint = (current_point - (px_arr_line_size / 8) + eight_counter);

            if (startValue == 1)
            {
                eight_counter++;
            }

            if (px_array[nextpoint] != 0)
            {
                //printf("lefut?");
                px_array[nextpoint] = px_array[nextpoint] + startValue;
            }
            else
            {
                //printf("ez fut le?\n");
                px_array[nextpoint] = startValue;
            }
            
            current_point = nextpoint;
        }
        
        if (Values[j] == Values[i])
        {
            nextpoint = current_point + eight_counter;

            if (startValue == 1)
            {
                eight_counter++;
            }

            px_array[nextpoint] = px_array[nextpoint] + startValue;
            current_point = nextpoint;
        }
        

        j++;
    }
    
    
    lseek(file_1, 62, SEEK_SET);
    write(file_1, px_array, px_array_size);
    //printf("meret: %d\n", px_array_size);
    
    close(file_1);
   
}

void ReceiveViaFile()
{
    int *Array = (int*)malloc(100*sizeof(int)); 
    char path[100];
    int irt_byte;
    char line[100];
    int legth = 100;
    int i = 0;
    int num;
    
    irt_byte = snprintf(path, sizeof(path), "/home/%s/Measurement.txt", getenv("LOGNAME"));
    FILE *f = fopen(path, "r");

    while (fgets(line, sizeof(line), f) != NULL)
    {   
        //printf("%s", line);
        if (i >= 100)
        {
            legth++;
            Array = (int*)realloc(Array, legth * sizeof(int));
        }
        
        num = atoi(line);
        Array[i] = num;
        i++;
        
    }
/*
    for (int i = 0; i < legth; i++)
    {
        printf("%d ", Array[i]);
    }
*/
    BMPcreator(Array, legth);
    
    //printf("\na hossz: %d\n", legth);
    fclose(f);
    free(Array);
}

void ReceiveViaSocket()
{
    int PORT_NO = 3333;

    int err; 
    int test;
    int s;                            // socket ID
    int bytes;                        // received/sent bytes
    int flag;                         // transmission flag
    char on;                          // sockopt option
    unsigned int server_size;         // length of the sockaddr_in server
    unsigned int client_size;
    struct sockaddr_in client;
    struct sockaddr_in server;        // address of server
    int *Data;

    on   = 1;
    flag = 0;
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port        = htons(PORT_NO);
    server_size            = sizeof server;
    client_size            = sizeof client;


    s = socket(AF_INET, SOCK_DGRAM, 0 );
    if ( s < 0 ) 
    {
        fprintf(stderr, "Socket creation error.\n");
        exit(2);
    }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof on);

    err = bind(s, (struct sockaddr *) &server, server_size);
    if ( err < 0 ) 
    {
        fprintf(stderr, ": Binding error.\n");
        exit(7);
    }

    while(1)
    {
        //printf("\n Várakozás az értékre...\n");
        bytes = recvfrom(s, &test, sizeof(test), flag, (struct sockaddr *) &client, &client_size );
        if ( bytes < 0 ) {
            fprintf(stderr, ": Receiving error.\n");
            exit(4);
            }
        //printf (" %d bájtot kaptam a klienstől.\n Kliens értéke: %d\n", bytes, test);

        
        bytes = sendto(s, &test, sizeof(test), flag, (struct sockaddr *) &client, client_size);
        if ( bytes <= 0 ) {
            fprintf(stderr, "Sending error.\n");
            exit(3);
            }
        
        Data = (int*)malloc(sizeof(int) * (test));
        int size_of_arr = test * sizeof(int);

        bytes = recvfrom(s, Data, size_of_arr, flag, (struct sockaddr *) &client, &client_size );
        if ( bytes < 0 ) {
            fprintf(stderr, ": Receiving error.\n");
            exit(4);
            }
        //printf (" %d bájtot kaptam a klienstől.\n", bytes);

        bytes = sendto(s, &size_of_arr, sizeof(size_of_arr), flag, (struct sockaddr *) &client, client_size);
        if ( bytes <= 0 ) {
            fprintf(stderr, "Sending error.\n");
            exit(3);
            }

        //printf(" Visszaküldöm, hogy %d bájtot kaptam\n", size_of_arr);

        BMPcreator(Data, test);
        free(Data);

    }

    close(s);

}


int Measurment(int **Values)
{
    int db = **Values;
    int *Data;

    if (db == 100)
    {
        Data = (int*)malloc(sizeof(int) * (100));
    }
    else
    {
        Data = (int*)malloc(sizeof(int) * (1000));
    }
    
    // szomszedos ertekek generalasa 
    for (int i = 0; i < db; i++)
    {
        if (i == 0)
        {
            Data[0] = 0;
        }

        double random_value = (double)rand() / RAND_MAX;

        if (random_value < 0.428571)
        {
            Data[i + 1] = Data[i] + 1;
        } 
        else if (random_value < 0.428571 + 0.354839) 
        {
            Data[i + 1] = Data[i] - 1;

        } 
        else 
        {
            Data[i + 1] = Data[i];
        }
        
    }
    
    *Values = Data;
    
    
    return db;
}
