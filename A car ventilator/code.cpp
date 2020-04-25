// Using a pthread mutex to get rid of the race condition
// Must be compiled with the  -fpermissive and -lpthread options
// g++ -fpermissive norace.cpp -lpthread
// Can ignore the two warnings
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <utility>
#include <unistd.h>
#include <queue>
#include <sstream>
using namespace std;

static int counter = 0;
static pthread_mutex_t lock;
static pthread_cond_t ok = PTHREAD_COND_INITIALIZER;
static int MaxCars;
static int MaxSouthCars;
static int MaxNorthCars;
static int CurrentCars;
static int CurrentSouthCars;
static int CurrentNorthCars;
static int NCrossed=0;
static int SCrossed=0;
static int TCrossed=0;
static int Waited=0;
struct Cars 
{
	int arrivalDelay;
	string cartype;
	int crossingTime;
};

bool enterTunnel(string type)
{
  if(type=="S")
  {
    if(CurrentSouthCars+1>MaxSouthCars || CurrentCars + 1 > MaxCars)
    {
      return false;
    }
  }
  else
  {

    if(CurrentSouthCars+1>MaxSouthCars || CurrentCars+1> MaxCars)
    {
      return false;
    }
  }
  pthread_mutex_lock(&lock);
  Waited= Waited+1;
	while ((1 + CurrentCars) > MaxCars)
	{
		pthread_cond_wait(&ok, &lock);
	}
  CurrentCars += 1;
  if(type=="S")
  {
      CurrentSouthCars=CurrentSouthCars+1;
      cout<<"Southbound car #"<< CurrentSouthCars<< " enters the tunnel"<< endl;
  }
    else{
      CurrentNorthCars=CurrentNorthCars+1;
      cout<<"Northbound car #"<< CurrentNorthCars<< " enters the tunnel"<< endl;
    }
	pthread_cond_signal(&ok);
	pthread_mutex_unlock(&lock);
	return true;

}
void leaveTunnel(string type)
{
	  pthread_mutex_lock(&lock);
		CurrentCars -= 1;
    if(type=="S")
    {
      SCrossed=SCrossed+1;
      cout<<"Southbound car #"<< CurrentSouthCars<< " exits the tunnel"<< endl;
      CurrentSouthCars=CurrentSouthCars-1;

    }
    else{
      NCrossed=NCrossed+1;

      cout<<"Northbound car #"<< CurrentNorthCars<< " exits the tunnel"<< endl;
      CurrentNorthCars=CurrentNorthCars -1;
    }
		pthread_cond_signal(&ok);
    pthread_mutex_unlock(&lock);
}
void *Car(void *arg)
{
	Cars c = *((Cars*)arg);
  if(c.cartype=="S")
  {
    cout<< "Southbound car #"<< CurrentSouthCars+1<< " arrives at the tunnel"<< endl;
  }
  else
  {
    cout<< "Northbound car #"<< CurrentNorthCars+1<< " arrives at the tunnel"<< endl;
  }
	if (enterTunnel(c.cartype)== false && c.cartype == "S")
	{
	}
  else if(enterTunnel(c.cartype)== false && c.cartype == "N")
  {

  }
	else
	{
		sleep(c.crossingTime);
		leaveTunnel(c.cartype);
	}
	pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
  pthread_mutex_init(&lock, NULL);
	string input;
	deque<Cars> allcars;

	if (argc < 2)
	{
		cout << "You did not enter enough parameters, please try again" << endl;
		return 0;
	}
  string file =argv[1];
  ifstream myfile(file);
  if (myfile.is_open())
  {
    myfile >> MaxCars;
    myfile >> MaxNorthCars;
    myfile >> MaxSouthCars;
    cout<<"Maximum Number of Cars in the Tunnel:"<< MaxCars<< endl;
    cout<<"Maximum Number of Northbound Cars in the Tunnel:"<< MaxNorthCars<< endl;
    cout<< "Maximum Number of Southbound Cars in the Tunnel:"<<MaxSouthCars<< endl;
	while(getline(myfile, input))
	{
    Cars c;

		stringstream ss(input);

		ss >> c.arrivalDelay;
		ss >> c.cartype;
		ss >> c.crossingTime;
		allcars.push_back(c);
		counter++;
  }

  }


  deque<pthread_t> children;

	for (int k = 0; k < allcars.size(); k++)
	{
    sleep(allcars.at(k).arrivalDelay);
    pthread_t tid;
    pthread_create(&tid, NULL, Car, (void *) &allcars.at(k));
    children.push_back(tid);
	}


	for (int n = 0; n < children.size(); n++)
	{
		pthread_join(children[n],NULL);
	}
  cout<<NCrossed<< " Northbound car(s) crossed the tunnel"<< endl;
  cout<<SCrossed<< " Southbound car(s) crossed the tunnel"<< endl;
  cout<<Waited<<" car(s) had to wait"<< endl;

	return 0;

} // main
