#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <vector>
#include <iostream>
#include <fstream> 
#include <queue>
#include <bits/stdc++.h>

using namespace std;
//Determining number of ATMs
#define ATM_NUMBERS 10
//Determining nanosecond multiplier to convert milisecond to nanosecond
#define NANO_SECOND_MULTIPLIER 1000000


//define struct customer to hold customer attributes
struct customer 
{
    string sleep_time;
    string ATM_instance;
    string bill_type;
    string amount;
    int customerid;
};

//creating global ATMs threads
pthread_t ATMs[ATM_NUMBERS];
//creating global customers vector that holds struct customer to hold customers
vector<struct customer> customers;
//creating global atm_queue to hold queues that holds customer in order to create shared memory
vector<queue<struct customer> > atm_queue;
//creating arr to pass indexes when creating ATMs threads
int arr[10] = {0 , 1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9};

//creating global customer_n to hold customer number
int customer_n;
//creating global CableTV integer to count amounts
int CableTV = 0 ;
//creating global Electiricity integer to count amounts
int Electricity = 0;
//creating global Gas integer to count amounts
int Gas = 0 ;
//creating global Telecommunication integer to count amounts
int Telecommunication = 0;
//creating global Water integer to count amounts
int Water = 0;

//creating fstream outfile to write <nameOfTheInputFile>_log.txt file
fstream outfile;
//creating pthread_mutex_t output to lock and unlock outfile when it is used
pthread_mutex_t output;
//creating pthread_mutex_t CableTV_m to lock and unlock CableTv integer when it is used
pthread_mutex_t CableTV_m;
//creating pthread_mutex_t Electricity_m to lock and unlock Electricity integer when it is used
pthread_mutex_t Electricity_m;
//creating pthread_mutex_t Gas_m to lock and unlock Gas integer when it is used
pthread_mutex_t Gas_m;
//creating pthread_mutex_t Telecommunication_m to lock and unlock Telecommunication integer when it is used
pthread_mutex_t Telecommunication_m;
//creating pthread_mutex_t Water_m to lock and unlock Water integer when it is used
pthread_mutex_t Water_m;
//creating pthread_mutex_t queue_mutex to lock and unlock atm_queues
pthread_mutex_t queue_mutex[ATM_NUMBERS];

//creating customer_counter to count customers who are served in ATMs
int customer_counter = 0;
//creating pthread_mutex_t customer_number to lock and unlock customer_counter
pthread_mutex_t customer_number;


//ATMs threads function
void* atm(void* arg) 
{ 
    //getting index of queue and assinging it to integer order
    int order = *(int *)arg;
    //while loop that enables to run until all customers are served
    while(customer_counter != customer_n)
    {
        //while loop that controls if customer thread writes to related atm queue 
        while(!atm_queue[order].empty())
        {
            //locking related queue_mutex wrt related queue
            pthread_mutex_lock(&queue_mutex[order]);
            //getting front struct customer from queue and assining it customer 
            struct customer customer = atm_queue[order].front();
            //checking if bill type equals to "cableTV"
            int check1 = strcmp((customer.bill_type).c_str(), "cableTV");
            //if it equals
            if(check1 == 0)
            {
                //locking CableTV_m mutex increment it
                pthread_mutex_lock(&CableTV_m);
                CableTV += stoi(customer.amount);
                //unlocking CableTV_m to release 
                pthread_mutex_unlock(&CableTV_m);
            }

            //checking if bill type equals to "electricity"
            int check2= strcmp((customer.bill_type).c_str(), "electricity");
            //if it equals
            if(check2 == 0)
            {
                //locking Electricity_m mutex increment it
                pthread_mutex_lock(&Electricity_m);
                Electricity += stoi(customer.amount);
                //unlocking Electricity_m to release 
                pthread_mutex_unlock(&Electricity_m);
            }
            //checking if bill type equals to "gas"
            int check3 = strcmp((customer.bill_type).c_str(), "gas");
            //if it equals
            if(check3 == 0)
            {
                //locking Gas_m mutex increment it
                pthread_mutex_lock(&Gas_m);
                Gas += stoi(customer.amount);
                //unlocking Gas_m to release
                pthread_mutex_unlock(&Gas_m);
            }
            //checking if bill type equals to "telecommunication"
            int check4 = strcmp((customer.bill_type).c_str(), "telecommunication");
            //if it equals
            if(check4 == 0)
            {
                //locking Telecommunication_m mutex increment it
                pthread_mutex_lock(&Telecommunication_m);
                Telecommunication += stoi(customer.amount);
                //unlocking Telecommunication_m to release
                pthread_mutex_unlock(&Telecommunication_m);
            }
            //checking if bill type equals to "water"
            int check5 = strcmp((customer.bill_type).c_str(), "water");
            //if it equals
            if(check5 == 0)
            {
                //locking Water_m mutex increment it
                pthread_mutex_lock(&Water_m);
                Water += stoi(customer.amount);
                //unlocking Water_m to release
                pthread_mutex_unlock(&Water_m);
            }
            
            //locking output mutex to write outfile
            pthread_mutex_lock(&output);
            //writing outfile to which customer served and it's id,amount and bill type
            outfile << "Customer" << customer.customerid <<"," << customer.amount << "TL," << customer.bill_type << endl;
            //unlocking output mutex to release
            pthread_mutex_unlock(&output);
            
            //popping related atm_queue to enable customer threads to push it
            atm_queue[order].pop();
            //locking customer_number mutex to increment it
            pthread_mutex_lock(&customer_number);
            customer_counter++;
            //unlocking customer_number mutex to release
            pthread_mutex_unlock(&customer_number);
            
            //unlocking related queue_mutex to release
            pthread_mutex_unlock(&queue_mutex[order]);
           
        }

    }
  
    return NULL; 
}

//CUSTOMER threads function
void* customer(void* arg) 
{ 
    //getting struct customer and assigning it to customer_temp  
    struct customer customer_temp = *(struct customer *)arg;
    //assigning customer' sleep time to duration
    int duration = std::stoi(customer_temp.sleep_time);
    //finding index of atm_queue
    int atm_instance = std::stoi(customer_temp.ATM_instance) -1;

    timespec sleepTime = {0};
    //converting miliseconds to nanoseconds
    sleepTime.tv_nsec=duration*NANO_SECOND_MULTIPLIER;
    //sleeping CUSTOMER threads
    nanosleep(&sleepTime, NULL);

    //locking related queue_mutex wrt related queue
    pthread_mutex_lock(&queue_mutex[atm_instance]);
    //pushing related customer to proper queue
    atm_queue[atm_instance].push(customer_temp);
    //unlocking related queue_mutex to release
    pthread_mutex_unlock(&queue_mutex[atm_instance]);
    
    return NULL; 
}


int main(int argc, char** argv)
{

//pushing 10 queues to atm_queue vector to hold ATMs queues
queue<struct customer> q;
for( int i=0 ; i<10 ; i++ ) atm_queue.push_back(q);

//reading input file name
string input_file = argv[1];
//creating ifstream file to read input file
std::ifstream file(input_file);
//removing .txt from input file
string out = input_file.substr(0, input_file.size()-4);
string add("_log.txt");
//adding _log.txt to removed string to get output file name
out.append(add);
//opening writable and readable file to write outputs
outfile.open(out,ios::trunc | ios::out | ios::in);
string str;
//getting customer number from input file
getline(file,str);
customer_n = std::stoi(str);

//parsing customers from input file and splitting line according to ","
std::string delimiter = ",";
int counter = 0 ;
while (counter != customer_n)
{
    getline(file,str);
    
    size_t pos = 0;
    int position = 0 ;
    std::string token;
    struct customer c ;

    while ((pos = str.find(delimiter)) != std::string::npos) 
    {
        
        token = str.substr(0, pos);
       
        position++;
       
        // if statement checks first argument of line and assigns it to struct customer sleep time
        if(position == 1)
        {
            c.sleep_time = token;
        }
        // if statement checks second argument of line and assigns it to struct customer ATM instance
        else if(position == 2)
        {
            c.ATM_instance = token;
        }
        // if statement checks third argument of line and assigns it to struct customer bill type
        else
        {
            c.bill_type = token ;
        }
        
        str.erase(0, pos + delimiter.length());
    }
    
    //assigning removed string that holds just amount and assigns it to struct customer amount
    c.amount = str;
    //assigning counter+1 (1 to number of customers) to struct customer id
    c.customerid = counter+1;
    //pushing struct customer to customers vector
    customers.push_back(c);

    counter++;
}

int i = 0;
int error;

//creating ATMs threads and passing argument as index to acces related queue
while(i < ATM_NUMBERS)
{
    error = pthread_create(&(ATMs[i]),NULL,&atm, (void *)&arr[i]);

    if (error != 0) 
        printf("\nThread can't be created : [%s]", strerror(error)); 
  
    i++; 
}

//declaring CUSTOMER threads
pthread_t CUSTOMER[customer_n];
int j = 0;
int err;

//creating CUSTOMER threads and passing argument as struct customer to get related infos
while(j < customer_n)
{ 
    err = pthread_create(&(CUSTOMER[j]),NULL,&customer,(void *)&customers[j]);

    if (err != 0) 
        printf("\nThread can't be created : [%s]", strerror(err)); 
    
    j++; 
}


//joining CUSTOMER threads
for(int k =0 ; k < customer_n ; k++) pthread_join(CUSTOMER[k], NULL);
//joining ATMs threads
for(int k = 0 ; k < ATM_NUMBERS ; k++) pthread_join(ATMs[k],NULL);

//writing All payments are completed from main thread to output file
outfile << "All payments are completed." << endl;
//writing related bill type and total amount for each from main thread to output file
outfile << "CableTv: " << CableTV << "TL" <<  endl;
outfile << "Electricity: " << Electricity << "TL" <<  endl;
outfile << "Gas: " << Gas << "TL" <<  endl;
outfile << "Telecommunication: " << Telecommunication << "TL" <<  endl;
outfile << "Water: " << Water << "TL" <<  endl;
//closing output file
outfile.close();

return 0 ;

}
