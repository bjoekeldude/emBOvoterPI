#include <iostream>
#include <wiringPi.h>
#include <ctime>
#include <chrono>
#include "Mini-Log/miniLog.hpp"

using namespace std::literals::chrono_literals;
#define GREATPIN 27
#define BADPIN 25
constexpr auto debounce_time = 20ms;

static volatile int greatstate;
static volatile int badstate;
static auto last_change = std::chrono::system_clock::now();
const std::string log_path {"./emBOvotes"};

miniLog::miniLogger ballotBox{log_path, "emBO++ Ballot-Box"};
miniLog::miniMessage badLog{miniLog::miniMessage_T::status, std::string{"-"}};
miniLog::miniMessage goodLog{miniLog::miniMessage_T::status, std::string{"+"}};




void PrintTime ()
{
    auto time1 = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(time1); 
    std::cout << std::ctime(&time);
}

void great(void) {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_since_last_change = now-last_change;
    if ( elapsed_since_last_change > debounce_time) {
        PrintTime();
	std::cout << "This was good!\n";
        ballotBox << goodLog;
	greatstate = !greatstate;
    }

    last_change = now;
}

void bad(void) {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_since_last_change = now-last_change;
    if ( elapsed_since_last_change > debounce_time) {
        PrintTime();
	std::cout << "This was bad!\n";
        ballotBox << badLog;
	badstate = !badstate;
    }

    last_change = now;
}

void activateInterrupts(){
	try{
		pinMode(GREATPIN,	OUTPUT);
		pinMode(BADPIN	,	OUTPUT);
		wiringPiISR(GREATPIN,	INT_EDGE_FALLING,	&great);
		wiringPiISR(BADPIN,	INT_EDGE_FALLING,	&bad);
		greatstate	=	digitalRead(GREATPIN);
		badstate	=	digitalRead(BADPIN);
	}
	catch(const std::exception& e){
		std::cout << " activating Interrupts caught a std:exception, with message '"
			<< e.what() << "'\n";
	}
}

struct MillisecondTimer_t{
    struct timespec timeOut, remains;
    MillisecondTimer_t(int ms){
	    timeOut.tv_sec = 0;
	    timeOut.tv_nsec= 1000*1000*ms;
    }
};

void initMessage(){
	std::cout << 	"emBO++ Votersystem successfully launched!" << std::endl <<
			"initial greatstate is " << greatstate << std::endl << 
			"initial  badstate  is " << badstate << std::endl;
}

int main(void) {
    
    MillisecondTimer_t timer{10};
	    
	    
    try{
    	wiringPiSetup();
    	activateInterrupts();
    }
    catch(std::exception &e){
	    std::cout << " The initialization caught a std::exception, with message '"
		    << e.what() << "'\n";
	    return 1;
    }
    
    initMessage();
    
    for (;;) {
        nanosleep(&timer.timeOut,&timer.remains); //wait for interrupt
    }
}
