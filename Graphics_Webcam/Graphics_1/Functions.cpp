#include "stdafx.h"
//#include <GL/glew.h>
#define GLFW_DLL
//#include <GLFW/glfw3.h>


#include "CImg.h"
#include <string>
#include <iostream>
#include "functions.h"
#include <windows.h>
#include <thread>
#include <string>
#include <ShellAPI.h>

//#include <winapifamily.h>

#include "escapi.h"


using namespace cimg_library;
using namespace std;



bool movement = false;

bool camera = true;

void captureOnce(string name)
{
	int devices = setupESCAPI();

	if (devices == 0)
	{
		printf("ESCAPI initialization failure or no devices found.\n");
		return;
	}

	/* Set up capture parameters.
	* ESCAPI will scale the data received from the camera
	* (with point sampling) to whatever values you want.
	* Typically the native resolution is 320*240.
	*/

	//struct SimpleCapParams capture;

	SimpleCapParams* capture = new struct SimpleCapParams();
	int width = 420;
	int height = 420;

	capture->mWidth = width;
	capture->mHeight = height;
	capture->mTargetBuf = new int[width * height];


	CImg<unsigned char> img(width, height, 1, 4);  // Define a 640x400 color image with 8 bits per color component.
	img.fill(0);                           // Set pixel values to 0 (color : black)
										   //unsigned char purple[] = { 255,0,255 };        // Define a purple color
										   //img.draw_text(100, 100, "Hello World", purple); // Draw a purple "Hello world" at coordinates (100,100).


	/* Initialize capture - only one capture may be active per device,
	* but several devices may be captured at the same time.
	*
	* 0 is the first device.
	*/

	//initCapture(0, capture);

	if (initCapture(0, capture) == 0)
	{
		printf("Capture failed - device may already be in use.\n");
		return;
	}

	/* request a capture */
	doCapture(0);


	while (isCaptureDone(0) == 0)
	{
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			img(j, i, 0, 3) = (capture->mTargetBuf[j + i * width] >> 24) & 0xff;
			img(j, i, 0, 0) = (capture->mTargetBuf[j + i * width] >> 16) & 0xff;
			img(j, i, 0, 1) = (capture->mTargetBuf[j + i * width] >> 8) & 0xff;
			img(j, i, 0, 2) = (capture->mTargetBuf[j + i * width] >> 0) & 0xff;
		}
	}




	name = name + ".gif";

	char *cstr = &name[0u];

	img.save(cstr);

	delete capture;

}

void eigen(CImg<unsigned char> a, CImg<unsigned char> b)
{
	CImg<unsigned char> eigenImg(a.width(), a.height(), 1, 3);

	cimg_forXY(eigenImg, x, y)
	{
		eigenImg(x, y, 0) = abs(a(x, y, 0) - b(x, y, 0));
		eigenImg(x, y, 1) = abs(a(x, y, 1) - b(x, y, 1));

		eigenImg(x, y, 2) = abs(a(x, y, 2) - b(x, y, 2));
	}

	eigenImg.save("eigenImg.gif");
}

bool deltaImage(cimg_library::CImg<unsigned char> image, int red, int green, int blue)
{
	const int dim = image.width() * image.height();
	double avgDE = 0;
	cimg_forXY(image, x, y)
	{

		unsigned char pointr = *image.data(x, y, 0, 0);
		unsigned char pointg = *image.data(x, y, 0, 1);
		unsigned char pointb = *image.data(x, y, 0, 2);

		float deltaR = pow((2 * abs(pointr - red)), 2);
		float deltaG = pow((4 * abs(pointg - green)), 2);
		float deltaB = pow((3 * abs(pointb - blue)), 2);

		avgDE += sqrt(deltaR + deltaG + deltaB);
	}

	avgDE /= dim;

	//cout << "Average deltaE: " << avgDE << endl;
	logging("Average deltaE: "+ to_string(avgDE));

	if (avgDE >= 40)
	{
		return true;
	}
	return false;
}

void loop(int delay)
{
	thread test(lightDelay, delay);
	test.detach();
	remove("ab.gif");
	captureOnce("ab");
	for (;;) {
		if (camera == false)
		{
			return;
		}
		CopyFile(L"ab.gif", L"a.gif", FALSE);
		Sleep(2500);
		if (camera == false)
		{
			return;
		}
		captureOnce("ab");
		Sleep(2500);


		CImg<unsigned char> img1("a.gif");
		CImg<unsigned char> img2("ab.gif");

		eigen(img1, img2);

		CImg<unsigned char> img3("eigenImg.gif");
		
		movement = deltaImage(img3, 0, 0, 0);

		if (movement)
		{
			//cout << "turn on" << endl;
			logging("movement detected; light on");
			system_no_output("klink.exe -i id_rsa root@192.168.3.1  fast-gpio set 0 1");
		}
	}
}

void lightDelay(int delay)
{
	int i = 0;

	Sleep(7005);

	for (i = 0; i < delay; i++)
	{
		Sleep(1003);
		//cout << "i: " << i << endl;
		logging("i: " + to_string(i));
		if (movement)
		{
			movement = false;
			i = 0;
		}
		if (camera == false)
		{
			return;
		}
	}
	//cout << "turn off" << endl;
	logging("delay terminated; light off");
	system_no_output("klink.exe -i id_rsa root@192.168.3.1 fast-gpio set 0 0");
	lightDelay(delay);
}

void setCamera(bool a)
{
	camera = a;
}

int system_no_output(std::string command)
{
	command.insert(0, "/C ");

	SHELLEXECUTEINFOA ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = "cmd.exe";
	ShExecInfo.lpParameters = command.c_str();
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;

	if (ShellExecuteExA(&ShExecInfo) == FALSE)
		return -1;

	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

	DWORD rv;
	GetExitCodeProcess(ShExecInfo.hProcess, &rv);
	CloseHandle(ShExecInfo.hProcess);

	return rv;
}


