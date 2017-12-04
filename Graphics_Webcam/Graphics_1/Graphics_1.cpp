// Graphics_1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CImg.h"
#include <string>
#include <iostream>
#include "functions.h"
#include "escapi.h"
#include <thread>
#include <chrono>
#include <ctime>
#include <stdlib.h>
#include <fstream>


using namespace cimg_library;
using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
/*int main() {
	
	loop(20);

	return 0;
}*/


// LedControl.cpp 2.0.cpp : Defines the entry point for the console application.
//

void mainMenu();
void ON1();
void ON2();
void OFF1();
void OFF2();
void HALF1();
void HALF2();
int BRIGHTNESS(int num);
void onOFF();
void schedule();
void motionCamera();
void invalidInput();
int counter = 0;
int errorControl = 0;
int checkTime = 0;
bool camStat = false; 
bool camError = false;
int delay = 20;
bool checkON = false;


struct date {
	int day, month, year;
	int hour, minute, second;
};

int alarmMenu();
int currentAlarms[9];
date currentDates[9];
bool currentTypes[9];
int vacant = 0;
ofstream outfile;



void setup() {
	date temporary; temporary.day = 0;
	for (int i = 0; i < 9; i++) {
		currentAlarms[i] = -1;
		currentDates[i] = temporary;
		currentTypes[i] = false; //false will be timer, true will be alarm
	}
}



int checkValue(char number) {
	return number - '0';
}



int turnDouble(int first, int second) {
	return first * 10 + second;
}



int turnQuad(int first, int second, int third, int fourth) {
	return first * 1000 + second * 100 + third * 10 + fourth;
}



string findType(bool type) {
	if (type) {
		return "(Weekly)";
	}
	else {
		return "(One-time)";
	}
}


int findDays(date someData) { //keep in mind these are supposed to be 0 based months from 0-11!
	int days = 0;
	if (someData.month == 0) {
		days = 31;
	}
	else if (someData.month == 1) {
		days = 59;
	}
	else {
		days = 59 + (someData.month - 1) * 31 - (someData.month - 1) / 2;
	}
	return days + someData.day;
}



int monthDays(date current) {
	if (current.month == 2) {
		return 28;
	}
	else if (current.month % 2 == 1) {
		return 31;
	}
	else {
		return 30;
	}
}

string dayOfWeek(date target) {
	tm time_in = { target.second, target.minute, target.hour, target.day, target.month - 1, target.year - 1900 };
	time_t time_temp = mktime(&time_in);
	const tm * time_out = localtime(&time_temp);
	int today = time_out->tm_wday;

	if (today == 0) {
		return "Sunday";
	}
	else if (today == 1) {
		return "Monday";
	}
	else if (today == 2) {
		return "Tuesday";
	}
	else if (today == 3) {
		return "Wednesday";
	}
	else if (today == 4) {
		return "Thursday";
	}
	else if (today == 5) {
		return "Friday";
	}
	else {
		return "Saturday";
	}
}


long long findSeconds(date currentDate, date newDate) {
	long long second = 0;
	second += newDate.second - currentDate.second;
	second += newDate.minute * 60 - currentDate.minute * 60;
	second += newDate.hour * 3600 - currentDate.hour * 3600;
	//Do special function for month
	second += findDays(newDate) * 3600 * 24 - findDays(currentDate) * 3600 * 24;
	second += newDate.year * 3600 * 24 * 365 - currentDate.year * 3600 * 24 * 365;
	return second;
}



void alarm() {
	int savedIndex = vacant;

	for (int i = 0; i < currentAlarms[vacant]; i++) {
		sleep_for(1s);
		if (currentAlarms[savedIndex] == -1) {
			cout << "Alarm has been deleted\n";
			logging("Alarm deleted");
			return;
		}
	}
	if (checkON) {//Turn on LED
		OFF2();
	}
	else
	{
		ON2();
	} //Turn on LED
	currentAlarms[savedIndex] = -1;
}



void alarm2() {
	int savedIndex = vacant;

	for (int i = 0; i < currentAlarms[vacant]; i++) {
		sleep_for(1s);
		if (currentAlarms[savedIndex] == -1) {
			cout << "Alarm has been deleted\n";
			logging("Alarm deleted");
			return;
		}
	}
	if (checkON) {//Turn on LED
		OFF2();
	}else
	{
		ON2();
	}
		while (true) {
		for (int i = 0; i < 60 * 60 * 24 * 7; i++) {
			sleep_for(1s);
			if (currentAlarms[savedIndex] == -1) {
				cout << "Alarm has been deleted\n";
				cout << "\n";
				logging("Alarm deleted");
				return;
			}
		}
		if (checkON) {//Turn on LED
			OFF2();
		}
		else
		{
			ON2();
		}
	}
}



date currentDate() {
	date current;
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	time(&rawtime);
	timeinfo = gmtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%d-%m-%Y %I:%M:%S%p", timeinfo);
	string parsedTime(buffer);

	cout << "GMT: " << parsedTime << "\n";

	char currentDay1 = parsedTime[0];
	char currentDay2 = parsedTime[1];
	char currentMonth1 = parsedTime[3];
	char currentMonth2 = parsedTime[4];
	char currentYear1 = parsedTime[6];
	char currentYear2 = parsedTime[7];
	char currentYear3 = parsedTime[8];
	char currentYear4 = parsedTime[9];
	char currentHour1 = parsedTime[11];
	char currentHour2 = parsedTime[12];
	char currentMinute1 = parsedTime[14];
	char currentMinute2 = parsedTime[15];
	char currentSecond1 = parsedTime[17];
	char currentSecond2 = parsedTime[18];
	char AMPM = parsedTime[19];

	current.day = turnDouble(checkValue(currentDay1), checkValue(currentDay2));
	current.month = turnDouble(checkValue(currentMonth1), checkValue(currentMonth2));
	current.year = turnQuad(checkValue(currentYear1), checkValue(currentYear2), checkValue(currentYear3), checkValue(currentYear4));
	current.hour = (turnDouble(checkValue(currentHour1), checkValue(currentHour2)) - 5 + 24) % 24;
	current.minute = turnDouble(checkValue(currentMinute1), checkValue(currentMinute2));
	current.second = turnDouble(checkValue(currentSecond1), checkValue(currentSecond2));

	//Figuring out some edge cases after swapping to EST-time, you should put this into a function!
	if (AMPM == 'P') {
		current.hour += 12;
		current.hour %= 24;
	}

	if (current.hour == 7) {
		current.hour = 19;
	}
	else if (current.hour == 19) {
		current.hour = 7;
	}

	if (currentHour1 == '0' && checkValue(currentHour2) < 5) {
		current.day -= 1;
	}

	if (current.day == 0) {
		current.month -= 1;
		if (current.month == 2) {
			current.day = 28;
		}
		else if (current.month % 2 == 0) {
			current.day = 30;
		}
		else {
			current.day = 31;
		}
	}

	if (current.month == 0) {
		current.year -= 1;
	}

	cout << "EST: " << current.day << "-" << current.month << "-" << current.year << " " << current.hour << ":" << current.minute << ":" << current.second << "\n";
	return current;
}



void countDown(date current) { //Mainly for testing purposes, want exact pinpoint target date.
							   //Printing cases

	date next;
	cout << "Enter day, month, year, hour, minute with spaces between: ";
	cin >> next.day >> next.month >> next.year >> next.hour >> next.minute;
	next.second = 0;
	cout << "Destination Time: " << next.day << "-" << next.month << "-" << next.year << " " << next.hour << ":" << next.minute << ":" << "00" << "\n";

	date fixedTime = currentDate();
	long long second = findSeconds(fixedTime, next);
	cout << "Alarm in: " << second << "s\n";
	cout << "\n";
	logging("New alarm in: " + to_string(second));

	//Create a new thread that starts the timer!

	for (int i = 0; i < 9; i++) {
		if (currentAlarms[i] == -1) {
			vacant = i;
			break;
		}
	}

	currentAlarms[vacant] = second; //We make the currentAlarms[i] value not equal to -1, which will allow the thread to run.
	currentDates[vacant] = next; //Subsequently, we will also record the date of when the alarm/LED will ring/flash.
	currentTypes[vacant] = false;
	thread t1(alarm);
	t1.detach();

	alarmMenu(); //Return to our main interface.
}


void alarmClock(date current) {
	//giving us the right date.
	tm time_in = { current.second, current.minute, current.hour, current.day, current.month - 1, current.year - 1900 };
	time_t time_temp = mktime(&time_in);
	const tm * time_out = localtime(&time_temp);
	int today = time_out->tm_wday;

	if (today == 0) {
		today = 7;
	}
	system("cls");
	cout << "Days are numbered in order from 1-7. (Ex. Monday is 1, Tuesday is 2).  Today is " << today << " \n";
	cout << "Enter target day, hour, and minute with spaces between them (Ex. 1 12 30 9 (Sets Schedule for Monday 12:30 PM): ";
	if (errorControl == 1) {
		cout << "ERROR: Invalid input. Please select a valid option to continue" << endl;
		errorControl = 0;
	}
	date target;
	int targetDay, targetHour, targetMinute, targetSecond;
	targetSecond = 0;

	cin >> targetDay; 
	if (cin.fail())
	{
		invalidInput();
		alarmClock(current);
	}else
	{
		if (targetDay > 7 || targetDay < 1) {
			errorControl = 1;
			alarmClock(current);
		}
	}
	cin >> targetHour;
	if (cin.fail()){
		invalidInput();
		alarmClock(current);
	}else
	{
		if (targetHour > 23 || targetHour < 0) {
			errorControl = 1;
			alarmClock(current);
		}
	}
	cin >> targetMinute; //Taking in input for the target time that the alarm will sound.
	if(cin.fail()){
		invalidInput();
		alarmClock(current);
	}else
	{
		if (targetMinute > 60 || targetMinute < 0) {
			errorControl = 1;
			alarmClock(current);
		}
	}
	// Comment on what the code below does \/\/\/\/\/

	int change = targetDay - today;
	if (change <= 0) {
		if (change != 0) {
			change += 7;
		}
		else { //If the change is zero days
			if (targetHour > current.hour) {
				change = 0;
			}
			else if (targetHour == current.hour) {
				if (targetMinute > current.minute) {
					change = 0;
				}
				else {
					change = 7;
				}
			}
			else {
				change = 7;
			}
		}
	}

	//Change is x days away, we can update our structure and recalculate the seconds

	target.day = current.day + change; //This might leak over the month's max days. 
	target.month = current.month; //Error checking remember!
	target.year = current.year;
	target.hour = targetHour;
	target.minute = targetMinute;
	target.second = targetSecond;
	//What we should do is just map all of this junk properly into a structure, and the shove the 
	//structure into the function and pray it works

	if (target.day > monthDays(current)) {
		target.day = target.day % monthDays(current);
		target.month += 1;
		if (target.month > 11) {
			target.month = 1;
			target.year += 1;
		}
	}

	date fixedTime = currentDate();

	long long second = findSeconds(fixedTime, target);
	cout << "Alarm will sound in: " << second << "s\n";
	cout << "\n";
	logging("Alarm will sound in: " + to_string(second));

	//Below is all for multithreading purposes. Our vacancy number will determine which thread we're working with.//
	for (int i = 0; i < 9; i++) {
		if (currentAlarms[i] == -1) {
			vacant = i;
			break;
		}
	}

	currentAlarms[vacant] = second; //We make the currentAlarms[i] value not equal to -1, which will allow the thread to run.
	currentDates[vacant] = target; //Subsequently, we will also record the date of when the alarm/LED will ring/flash.
	currentTypes[vacant] = true;
	thread t1(alarm2);
	t1.detach();

	alarmMenu(); //Return to our main interface.
}



int alarmMenu() {
	date current;
	current = currentDate();

	system("cls");
	cout << "LED LIGHT CONTROL SYSTEM" << "\n";
	cout << "Choose your setting" << "\n";
	cout << "(1) SET SCHEDULE" << "\n";
	cout << "(2) SET TIMER (TEST PLEASE IGNORE)" << "\n";
	cout << "(3) MANAGE SCHEDULE" << "\n";
	cout << "(4) BACK" << "\n";
	if (errorControl == 1) {
		cout << "ERROR: Invalid input. Please select a valid option to continue" << endl;
		errorControl = 0;
	}
	int command;
	cin >> command;


	if (cin.fail())
	{
		invalidInput();
		alarmMenu();
	}
	else {

		if (command == 2) {
			countDown(current);
		}
		else if (command == 1) {
			alarmClock(current);
		}
		else if (command == 3) { //Low functionality right now, but we'll try to make it work. 
			system("cls");
			cout << "LED LIGHT CONTROL SYSTEM" << "\n";
			cout << "CURRENT SCHEDULES" << "\n";
			for (int i = 0; i < 9; i++) {
				if (currentAlarms[i] == -1) {
					cout << i + 1 << ". " << "No alarm set.\n";
				}
				else {
					cout << i + 1 << ". " << dayOfWeek(currentDates[i]) << " " << currentDates[i].hour << ":" << currentDates[i].minute << " " << findType(currentTypes[i]) << "\n";
				}
			}

			int deleteIndex;
			cout << "Enter 0 to return to main\n";
			cout << "What schedule would you like to delete? ";
			cin >> deleteIndex; deleteIndex -= 1;

			while (deleteIndex < 0 && deleteIndex > 9) {
				cout << "0 to return to main\n";
				cout << "What schedule would you like to delete? ";
				cin >> deleteIndex; deleteIndex -= 1;
			}

			if (deleteIndex >= 0 && deleteIndex <= 9) { //If we input() -1, then it won't delete anything.
				currentAlarms[deleteIndex] = -1;
			}
			else {
				cout << "Returning back to main\n";
			}


			sleep_for(1s);
			alarmMenu();
		}
		else if (command == 4)
		{
			mainMenu();
		}else
		{
			errorControl = 1;
			alarmMenu();
		}
	}

	return 0;
}

void invalidInput() {
	errorControl = 1;
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
void mainMenu() {
	system("cls");
	cout << "LED LIGHT CONTROL SYSTEM" << "\n";
	cout << "Choose your setting" << "\n";
	cout << "(1) TURN ON/OFF LIGHTS" << "\n";
	cout << "(2) SET SCHEDULE" << "\n";
	cout << "(3) MOTION CAMERA SETTINGS" << "\n";
	cout << "(4) EXIT" << "\n";
}

void ON1() {
	system("klink.exe -i id_rsa root@192.168.3.1  fast-gpio set 0 1");
	logging("LED 1 on");
}
void HALF1() {
	system("klink.exe -i id_rsa root@192.168.3.1 fast-gpio pwm 0 4000 1");
	logging("LED 1 50%");
}
void OFF1() {
	system("klink.exe -i id_rsa root@192.168.3.1 fast-gpio set 0 0");
	logging("LED 1 off");
}

void ON2() {
	system_no_output("klink.exe -i id_rsa root@192.168.3.1  fast-gpio set 1 1");
	checkON = true;
	logging("LED 2 on");
}
void HALF2() {
	system("klink.exe -i id_rsa root@192.168.3.1 fast-gpio pwm 1 4000 1");
	checkON = true;
	logging("LED 2 50%");
}
void OFF2() {
	system_no_output("klink.exe -i id_rsa root@192.168.3.1 fast-gpio set 1 0");
	checkON = false;
	logging("LED 2 off");
}
void ON3() {
	system("klink.exe -i id_rsa root@192.168.3.1 /mnt/usb/AllON");
	checkON = true;
	logging("LED 1/2 on");

}
void HALF3() {
	system("klink.exe -i id_rsa root@192.168.3.1 /mnt/usb/AllHALF");
	checkON = true;
	logging("LED 1/2 50%");
}
void OFF3() {
	system("klink.exe -i id_rsa root@192.168.3.1 /mnt/usb/AllOFF");
	checkON = false;
	logging("LED 1/2 off");
}

int BRIGHTNESS(int num) {
	system("cls");
	cout << "LED LIGHT CONTROL SYSTEM" << "\n";
	cout << "(1) OFF" << "\n";
	cout << "(2) 50%" << "\n";
	cout << "(3) ON" << "\n";
	if (errorControl == 1) {
		cout << "ERROR: Invalid input. Please select a valid option to continue" << endl;
		errorControl = 0;
	}
	int input;
	cin >> input;
	if (cin.fail()) {
		invalidInput();
		BRIGHTNESS(num);
	}
	else {
		if (input == 1 || input == 2 || input == 3) {
			if (num == 1) {
				if (input == 1) {
					OFF1();
					mainMenu();
				}
				else if (input == 2) {
					HALF1();
					mainMenu();
				}
				else if (input == 3) {
					ON1();
					mainMenu();
				}
			}
			else if (num == 2) {
				if (input == 1) {
					OFF2();
					mainMenu();
				}
				else if (input == 2) {
					HALF2();
					mainMenu();
				}
				else if (input == 3) {
					ON2();
					mainMenu();
				}
			}
			else if (num == 3) {
				if (input == 1) {
					OFF3();
					mainMenu();
				}
				else if (input == 2) {
					HALF3();
					mainMenu();
				}
				else {
					ON3();
					mainMenu();
				}
			}
		}
		else {
			errorControl = 1;
			BRIGHTNESS(num);
		}
	}
	return 0;
}

void onOFF() {
	system("cls");
	cout << "LED LIGHT CONTROL SYSTEM" << "\n";
	cout << "Choose your setting" << "\n";
	cout << "(1) LIGHT 1" << "\n";
	cout << "(2) LIGHT 2" << "\n";
	cout << "(3) BOTH" << "\n";
	cout << "(4) BACK" << "\n";

	if (errorControl == 1) {
		cout << "ERROR: Invalid input. Please select a valid option to continue" << endl;
		errorControl = 0;
	}

	int input;
	cin >> input;
	if (cin.fail()) {
		invalidInput();
		onOFF();
	}
	else {
		if (input == 4) {
			mainMenu();
		}
		else if (input == 1 || input == 2 || input == 3) {
			BRIGHTNESS(input);
		}
		else {
			errorControl = 1;
			onOFF();
		}
	}
}

void schedule() {
	system("cls");
	cout << "LED LIGHT CONTROL SYSTEM" << "\n";
	cout << "Choose your setting" << "\n";
	cout << "(1) CHECK TIME" << "\n";
	cout << "(2) SET SCHEDULE" << "\n";
	cout << "(3) BACK" << "\n";
	if (checkTime == 1) {
		system("time/T");
		checkTime = 0;
	}
	if (errorControl == 1) {
		cout << "ERROR: Invalid input. Please select a valid option to continue" << endl;
		errorControl = 0;
	}

	int input;
	cin >> input;
	if (cin.fail()) {
		invalidInput();
		schedule();
	}
	else {
		if (input == 3) {
			mainMenu();
			checkTime = 0;
		}
		else if (input == 1) {
			checkTime = 1;
			schedule();
		}
		else if (input == 2) {
			alarmMenu();
		}
		else {
			errorControl = 1;
			schedule();
		}
	}
}

void motionCamera() {
	system("cls");
	cout << "LED LIGHT CONTROL SYSTEM" << "\n";
	cout << "Choose your setting" << "\n";
	cout << "(1) START" << "\n";
	cout << "(2) STOP" << "\n";
	cout << "(3) CHANGE DELAY" << "\n";
	cout << "(4) EXIT" << "\n";
	
	if (camStat)
	{
		cout << "Motion Camera is currently ON. DELAY: " << delay << endl;
	}else
	{
		cout << "Motion Camera is currenly OFF. DELAY: " << delay << endl;
	}
	if (camError)
	{
		cout << "ERROR: Camera is already ON" << endl;
		camError = false;
	}
	if (errorControl == 1) {
		cout << "ERROR: Invalid input. Please select a valid option to continue" << endl;
		errorControl = 0;
	}
	
	int input;
	cin >> input;
	if (cin.fail())
	{
		invalidInput();
		motionCamera();
	}
	else {
		if (input == 1)
		{
			if (camStat)
			{
				camError = true;
				motionCamera();
			}
			else {
				setCamera(true);
				thread camThread(loop, delay);
				camThread.detach();
				camStat = true;
				mainMenu();
			}
		}
		else if (input == 2)
		{
			setCamera(false);
			camStat = false;
			mainMenu();

		}
		else if (input == 3)
		{
			cout << "Enter new DELAY" << endl;
			cin >> delay;
			if (cin.fail())
			{
				invalidInput();
				motionCamera();
			}
			else {
				if (camStat) {
					setCamera(false);
					Sleep(6000);
					setCamera(true);
					thread camThread(loop, delay);
					camThread.detach();
					mainMenu();
				}
				else
				{
					motionCamera();
				}
			}

		}
		else if (input == 4)
		{
			mainMenu();
		}else
		{
			errorControl = 1;
			motionCamera();
		}
	}
}

void logging(string information) { //This is the logging function, call it every time you cout something.
	time_t timeNow;
	if (!outfile.is_open()) {
		cout << "Logging file could not be opened. Please check the files or create a log.";
		return;
	}
	SYSTEMTIME *time = new SYSTEMTIME();
	GetSystemTime(time);


	outfile << time->wHour <<":" << time->wMinute << ":" << time->wSecond << "\t  " << information << "\r\n";
}

int main() {
	outfile.open("logfile.txt");
	setup();

	mainMenu();
	while (counter != 10) {
		int input;
		cin >> input;
		if (cin.fail()) {
			invalidInput();
		}
		else {
			if (input == 1) {
				onOFF();
			}
			else if (input == 4) {
				counter = 10;
			}
			else if (input == 2) {
				schedule();
			}
			else if (input == 3) {
				motionCamera();
			}
			else {
				cout << "ERROR: Invalid input. Please select a valid option to continue" << endl;
			}
		}
	}
	outfile.close();
	return 0;
}


