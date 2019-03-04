#include <iostream>
#include <wiringPi.h>
#include <ctime>
#include <chrono>
#include "Mini-Log/miniLog.hpp"

using namespace std::literals::chrono_literals;
#define GREATPIN 11
#define BADPIN 13
#define DOUBLEGREATPIN 15
#define DOUBLEBADPIN 16
#define MEDIUMPIN 18
constexpr auto debounce_time = 20ms;

static volatile int greatstate;
static volatile int badstate;
static volatile int doublegreatstate;
static volatile int doublebadstate;
static volatile int mediumstate;
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
	std::cout << "+";
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
	std::cout << "-";
        ballotBox << badLog;
	badstate = !badstate;
    }

    last_change = now;
}

void doublebad(void) {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_since_last_change = now-last_change;
    if ( elapsed_since_last_change > debounce_time) {
        PrintTime();
	std::cout << "--";
        ballotBox << badLog;
	doublebadstate = !doublebadstate;
    }

    last_change = now;
}

void doublegreat(void) {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_since_last_change = now-last_change;
    if ( elapsed_since_last_change > debounce_time) {
        PrintTime();
	std::cout << "++";
        ballotBox << badLog;
	doublegreatstate = !doublegreatstate;
    }

    last_change = now;
}

void medium(void) {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_since_last_change = now-last_change;
    if ( elapsed_since_last_change > debounce_time) {
        PrintTime();
	std::cout << "/";
        ballotBox << badLog;
	mediumstate = !mediumstate;
    }

    last_change = now;
}

void activateInterrupts(){
	try{
		pinMode(GREATPIN,	OUTPUT);
		pinMode(BADPIN	,	OUTPUT);
		pinMode(DOUBLEGREATPIN	,	OUTPUT);
		pinMode(DOUBLEBADPIN	,	OUTPUT);
		pinMode(MEDIUMPIN	,	OUTPUT);
		wiringPiISR(GREATPIN,	INT_EDGE_FALLING,	&great);
		wiringPiISR(BADPIN,	INT_EDGE_FALLING,	&bad);
		wiringPiISR(DOUBLEGREATPIN,	INT_EDGE_FALLING,	&doublegreat);
		wiringPiISR(DOUBLEBADPIN,	INT_EDGE_FALLING,	&doublebad);
		wiringPiISR(MEDIUMPIN,	INT_EDGE_FALLING,	&medium);
		greatstate	=	digitalRead(GREATPIN);
		badstate	=	digitalRead(BADPIN);
		doublegreatstate	=	digitalRead(DOUBLEGREATPIN);
		doublebadstate	=	digitalRead(DOUBLEBADPIN);
		mediumstate	=	digitalRead(MEDIUMPIN);
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
			"initial doublegreatstate is " << doublegreatstate << std::endl << 
			"initial  greatstate  is " << greatstate << std::endl
			"initial  mediumstate  is " << mediumstate << std::endl
			"initial  badstate  is " << badstate << std::endl
			"initial  doublebadstate  is " << doublebadstate << std::endl;
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
