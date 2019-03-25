#include <iostream>
#include <chrono>
//Added by rémi
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <math.h>
#include <string>
#include <algorithm>
#include <ctime>
#ifdef _WIN32
#include <Windows.h>
#endif
#include <omp.h>

#include <GLFW/glfw3.h>

#include "fastdensity/fastdensity.hpp"

using namespace std;

/* flobal vars */

unsigned int point_nb = 1000000;
unsigned int seed = 5;
unsigned int width = 1000;
unsigned int height = 600;
unsigned int mapSize = 32;
double stddev = 5.68;


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

/* This fonction is only here as debug purpose. Check if OpenMP work (call it on the main)*/
static void hello_World_OpenMp() {

	unsigned int n_thread = omp_get_max_threads();
	omp_set_num_threads(n_thread);

	#pragma omp parallel for
	for (int i = 0; i < 4; i++) {
		cout << "thread numéros : " << omp_get_thread_num() << endl;
	}
}

/*	return_current_time_and_date_as_string :
 *		Create a string representativ of the date and time
 *  @return :
 *		A string with shape "YYYY-MM-DD-HH:MM:SS"
 */

static string return_current_time_and_date_as_string() {
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	string s(30, '\0');
	strftime(&s[0], s.size(), "%Y-%m-%d_%Hh%Mm%Ss", localtime(&in_time_t));
	s.erase(remove(s.begin(), s.end(), '\0'), s.end());
	cout << s << endl;
	return s;
}

/*	Generation_1 :
 *		Generate a tab filled with random points placed in a closed box
 *	@params : 
 *		width & heigth ; the limit of the box where points will be generate
 *		n ; number of points that should be generated
 *		seed ; the seed wanted for the random number generation, will refresh it
 *  @return :
 *		A 1 dimensional array of int, with all the even indexes are the x value of the point and all the odd indexes are the y value of it
 */

static vector<int> generation_1(unsigned int seeds=seed, bool display=false) 
{
	vector<int> point_Array;
	srand(seeds); //seed of the rng && should reset the seed at it's starting point.

	/* array generation */
	for (unsigned int i = 0; i < point_nb; i++) {
		point_Array.push_back(rand()%(width+1));
		point_Array.push_back(rand()%(height+1));
	}

	/* array display */
	if (display) {
		for (unsigned int i = 0; i < point_nb * 2; i += 2) {
			cout << "le point : (" << point_Array[i] << " ; " << point_Array[i + 1] << ") est dans notre tableau" << endl;
		}
	}
	return point_Array;
}


/*	Generation_2 :
 *		Generate a tab filled with random points following a normal distribution (gaussian) with mean = 0
 *	@params :
 *		stddev ; the standard deviation that'll follow the distribution
 *		n ; number of points that should be generated
 *		seed ; the seed wanted for the random number generation, will refresh it
 *  @return :
 *		A 1 dimensional array of int, with all the even indexes are the x value of the point and all the odd indexes are the y value of it
 */

static vector<int> generation_2(unsigned int seeds=seed, bool display=false)
{
	//normal distribution construction
	//when using n<4 as seed, it generate some patern of 0, don't know why. But work perfectly fine with other seed values
	default_random_engine generator(seeds); //seed of the rng && should reset the seed at it's starting point.
	normal_distribution<double> distribution (0.0,stddev);

	vector<int> point_Array;

	/* array generation */
	for (unsigned int i = 0; i < point_nb; i++) {
		point_Array.push_back(ceil(distribution(generator)));
		point_Array.push_back(ceil(distribution(generator)));
	}

	/* array display */
	if (display) {
		for (unsigned int i = 0; i < point_nb * 2; i += 2) {
			cout << "le point : (" << point_Array[i] << " ; " << point_Array[i + 1] << ") est dans notre tableau" << endl;
		}
	}
	return point_Array;
}

/*	Generation_1_Simple_Array :
 *		Generate a tab filled with random values between 0 and limit
 *	@params :
 *		limit ; the maximum a point randomly generated can achieved
 *		n ; number of values that should be generated
 *		seed ; the seed wanted for the random number generation, will refresh it
 *  @return :
 *		A 1 dimensional array of int
 */

/*static vector<int> generation_1_Simple_Array(int limit, unsigned int n, int seed, bool display = false)
{
	vector<int> point_Array;
	srand(seed); //seed of the rng && should reset the seed at it's starting point.

	// array generation
	for (unsigned int i = 0; i < n; i++) {
		point_Array.push_back(rand() % (limit + 1));
	}

	// array display
	if (display) {
		for (unsigned int i = 0; i < n; i++) {
			cout << "le point " << i << " à comme valeurs en X : " << point_Array[i] << endl;
		}
	}
	return point_Array;
}*/


/*	Generation_2_Simple_Array :
 *		Generate a tab filled with random values for points following a normal distribution (gaussian) with mean = 0
 *	@params :
 *		stddev ; the standard deviation that'll follow the distribution
 *		n ; number of values that should be generated
 *		seed ; the seed wanted for the random number generation, will refresh it
 *  @return :
 *		A 1 dimensional array of int, of size n
 */

/*static vector<int> generation_2_Simple_Array (double stddev, unsigned int n, int seed, bool display = false)
{
	//normal distribution construction
	default_random_engine generator(seed); //seed of the rng && should reset the seed at it's starting point.
	normal_distribution<double> distribution(0.0, stddev);

	vector<int> point_Array;

	// array generation
	for (unsigned int i = 0; i < n; i++) {
		point_Array.push_back(ceil(distribution(generator)));
	}

	// array display
	if (display) {
		for (unsigned int i = 0; i < n; i ++) {
			cout << "le point " << i << " à comme valeurs en X ou Y : " << point_Array[i] << endl;
		}
	}
	return point_Array;
}*/

/*
DEBUG FUNCTION
display a 2 dimensional array given as param on the standard output
*/
template< typename T >
static void display_2Darray(vector<vector<T>> tab) {

	cout << endl;

	for (unsigned int i = 0; i < tab.size(); i++) {
		for (unsigned int j = 0; j < tab[i].size(); j++) {
			cout << tab[i][j] << " | ";
		}
		cout << endl;
	}
}

template< typename T >
static boolean isEqual_2DArray(vector<vector<T>> tab1, vector<vector<T>> tab2) {
	for (unsigned int i = 0; i < tab1.size(); i++) {
		for (unsigned int j = 0; j < tab1[i].size(); j++) {
			if (tab1[i][j] != tab2[i][j]) { return false; }
		}
	}
	return true;
}

/*
 * Compute_densityMap_CPU_simple_1Tab :
 * @params :
 *	tab : A 1 dimensional array of number, with all the even indexes are the x value of the point and all the odd indexes are the y value of it
 *  maxX, maxY, minX, minY : the  global extremums of the tab array
 *  mapSize : the size of density map wanted, (if 256 then the density map will generated will be 256^2
 * @return :
 *	the densityMap
 *
 * compute_densityMap_CPU_multThread_1Tab do the same with a bit of multi threading
 */
template< typename T >
vector<vector<unsigned int>> compute_densityMap_CPU_simple_1Tab(vector<T>tab, T maxX, T minX, T maxY, T minY) {
	vector<vector<unsigned int>> densityMap(mapSize, vector<unsigned int>(mapSize));
	T Xrange = maxX - minX;
	T Yrange = maxY - minY;
	double tempX, tempY;
	for (unsigned int i = 0; i < point_nb*2; i+=2) {
		tab[i] = tab[i] + minX * (-1);
		tab[i + 1] = tab[i + 1] + minY * (-1);
		tempX = round(((double)tab[i] / (double)Xrange) * (mapSize-1));
		tempY = round(((double)tab[i+1] /(double)Yrange) * (mapSize-1));
		densityMap[tempX][tempY]++;
	}
	return densityMap;
}

template< typename T >
vector<vector<unsigned int>> compute_densityMap_CPU_multThread_1Tab(vector<T>tab, T maxX, T minX, T maxY, T minY) {
	vector<vector<unsigned int>> densityMap(mapSize, vector<unsigned int>(mapSize));
	T Xrange = maxX - minX;
	T Yrange = maxY - minY;
	#pragma omp parallel for
	for (int i = 0; i < point_nb*2; i += 2) {
		tab[i] = tab[i] + minX * (-1);
		tab[i + 1] = tab[i + 1] + minY * (-1);
		tab[i] = round(((double)tab[i] / (double)Xrange) * (mapSize - 1));
		tab[i + 1] = round(((double)tab[i + 1] / (double)Yrange) * (mapSize - 1));
	}
	for (int i = 0; i < point_nb*2; i += 2) {
		densityMap[tab[i]][tab[i+1]]++;
	}
	return densityMap;
}

/*
 * Compute_densityMap_CPU_simple_2Tabs :
 * @params :
 *	tabX : A 1 dimensional array of number containing all the X axis values for points
 *  tabY : A 1 dimensional array of number containing all the Y axis values for points
 *  maxX, maxY, minX, minY : the  global extremums of the tab array
 *  mapSize : the size of density map wanted, (if 256 then the density map will generated will be 256^2
 * @return :
 *	the densityMap
 *
 * compute_densityMap_CPU_multThread_2Tabs do the same with a bit of multi threading
 */
template< typename T >
vector<vector<unsigned int>> compute_densityMap_CPU_simple_2Tabs(vector<T>tabX, vector<T>tabY, T maxX, T minX, T maxY, T minY) {
	vector<vector<unsigned int>> densityMap(mapSize, vector<unsigned int>(mapSize));
	T Xrange = maxX - minX;
	T Yrange = maxY - minY;
	for (unsigned int i = 0; i < point_nb; i ++) {
		tabX[i] = tabX[i] + minX * (-1);
		tabY[i] = tabY[i] + minY * (-1);
		tabX[i] = round(((double)tabX[i] / (double)Xrange) * (mapSize - 1));
		tabY[i] = round(((double)tabY[i] / (double)Yrange) * (mapSize - 1));
		densityMap[tabX[i]][tabY[i]]++;
	}
	return densityMap;
}

template< typename T >
vector<vector<unsigned int>> compute_densityMap_CPU_multThread_2Tabs(vector<T>tabX, vector<T>tabY, T maxX, T minX, T maxY, T minY) {
	vector<vector<unsigned int>> densityMap(mapSize, vector<unsigned int>(mapSize));
	T Xrange = maxX - minX;
	T Yrange = maxY - minY;
	#pragma omp parallel for
	for (int i = 0; i < point_nb; i ++) {
		tabX[i] = tabX[i] + minX * (-1);
		tabY[i] = tabY[i] + minY * (-1);
		tabX[i] = round(((double)tabX[i] / (double)Xrange) * (mapSize - 1));
		tabY[i] = round(((double)tabY[i] / (double)Yrange) * (mapSize - 1));
	}
	for (int i = 0; i < point_nb; i++) {
		densityMap[tabX[i]][tabY[i]]++;
	}
	return densityMap;
}

/*	@temporary
 *  add_Benchmark :
 *		Create an entry in a CSV file that contain the whole information about the execution of a Benchmark
 *		For the Moment, just getting informations for creating an array of point following one of both génération. Then making some calulation with the array,
 *		finally store the result in a CSV file.
 *		In future should only take the pre calculated array as paramaters.
 *	@params :
 *		filename ; the file name of CSV
 *		point_nb ; number of point inside the array
 *		seed ; the seed used for generating the array
 *		width & height ; referes to generation 1 params, they got -1 as default value in case generation2 is used instead of generation1
 *		stddev ; the standard deviation that'll follow the distribution (for generation2)
 *		entry_type ; an integer used as identifier for the type of algorithm used for generating the density map (1 => normal / 2 => Multi threaded / 3 => GPU )
 */

static void one_entry_benchmark(string filename, unsigned int entry_type, unsigned int number, bool two_tabs = false) {
	ofstream file_pointer;
	/* opens an existing csv file or creates a new file. Every output operation will be push at the end of file.
		flag in : Open for input operations (reading essentially);
		flag out : Open for output operations.
		flag app : 	All output operations are performed at the end of the file, appending the content to the current content of the file.
	*/
	file_pointer.open(filename, ios::out | ios::app);

	long totalTime = 0;
	vector<int> point_Array;
	vector<int> point_ArrayX;
	vector<int> point_ArrayY;
	vector<vector<unsigned int>> densityMap;
	unsigned int seeds = seed;
	string generation = "";
	string entry_name = "";

	for (unsigned int nb = 0; nb < number; nb++) {
		/* Changing the seed for every loop, used for forcing new data on cache.
			I choose this way of changing the seed for keeping the same seed for future calcul process.
			Will probably need a preload of every points on the cache for equalise the calcul process.
		*/
		seeds = seeds + 1;
		if (!two_tabs) {
			point_Array = generation_2(seeds);
			generation = "2";
		}
		else {
			point_Array = generation_1(seeds);
			generation = "1";
		}

		//determining the min X & Y, the max X & Y
		int maxY = point_Array[0], maxX = point_Array[0], minY = point_Array[0], minX = point_Array[0];
		for (unsigned int i = 0; i < point_nb * 2; i += 2) {
			if (point_Array[i] < minX) { minX = point_Array[i]; }
			else if (point_Array[i] > maxX) { maxX = point_Array[i]; }
			if (point_Array[i + 1] < minY) { minY = point_Array[i + 1]; }
			else if (point_Array[i + 1] > maxY) { maxY = point_Array[i + 1]; }
		}

		if (two_tabs) {
			for (unsigned int i = 0; i < point_nb * 2; i += 2) {
				point_ArrayX.push_back(point_Array[i]);
				point_ArrayY.push_back(point_Array[i + 1]);
			}
		}

		switch (entry_type){

			case 1 :
				entry_name = "CPU_densityMap_simple";
				if (two_tabs) {
					auto start = chrono::steady_clock::now();
					densityMap = compute_densityMap_CPU_simple_2Tabs(point_ArrayX, point_ArrayY, maxX, minX, maxY, minY);
					auto end = chrono::steady_clock::now();
					auto diff = end - start;
					auto elapsed_time = chrono::duration <long, nano>(diff).count();
					totalTime += elapsed_time;
					//Insert data to file filename
					std::cout << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time << endl;
					file_pointer << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time;
				}
				else {
					auto start = chrono::steady_clock::now();
					densityMap = compute_densityMap_CPU_simple_1Tab(point_Array, maxX, minX, maxY, minY);
					auto end = chrono::steady_clock::now();
					auto diff = end - start;
					auto elapsed_time = chrono::duration <long, nano>(diff).count();
					totalTime += elapsed_time;
					//Insert data to file filename
					std::cout << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time << endl;
					file_pointer << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time;
				}
				break;

			case 2 :
				entry_name = "CPU_densityMap_multThread";
				if (two_tabs) {
					auto start = chrono::steady_clock::now();
					densityMap = compute_densityMap_CPU_multThread_2Tabs(point_ArrayX, point_ArrayY, maxX, minX, maxY, minY);
					auto end = chrono::steady_clock::now();
					auto diff = end - start;
					auto elapsed_time = chrono::duration <long, nano>(diff).count();
					totalTime += elapsed_time;
					//Insert data to file filename
					std::cout << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time << endl;
					file_pointer << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time;
				}
				else {
					auto start = chrono::steady_clock::now();
					densityMap = compute_densityMap_CPU_multThread_1Tab(point_Array, maxX, minX, maxY, minY);
					auto end = chrono::steady_clock::now();
					auto diff = end - start;
					auto elapsed_time = chrono::duration <long, nano>(diff).count();
					totalTime += elapsed_time;
					//Insert data to file filename
					std::cout << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time << endl;
					file_pointer << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time;
				}
				break;

			case 3 :
				entry_name = "GPU_densityMap";
				if (two_tabs) {
					auto start = chrono::steady_clock::now();
					//densityMap = compute_densityMap_GPU_2Tabs(point_ArrayX, point_ArrayY, maxX, minX, maxY, minY);
					auto end = chrono::steady_clock::now();
					auto diff = end - start;
					auto elapsed_time = chrono::duration <long, nano>(diff).count();
					totalTime += elapsed_time;
					//Insert data to file filename
					std::cout << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time << endl;
					file_pointer << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time;
				}
				else {
					auto start = chrono::steady_clock::now();
					//densityMap = compute_densityMap_GPU_1Tab(point_Array, maxX, minX, maxY, minY);
					auto end = chrono::steady_clock::now();
					auto diff = end - start;
					auto elapsed_time = chrono::duration <long, nano>(diff).count();
					totalTime += elapsed_time;
					//Insert data to file filename
					std::cout << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time << endl;
					file_pointer << entry_name << to_string(nb) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << elapsed_time;
				}
				break;
		}
		//uncomment this if you want to display the generated density map on the shell
		//display_2Darray(densityMap);
	}

	totalTime = totalTime / number;

	std::cout << "Mean_On_" << to_string(number) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << totalTime << endl;
	file_pointer << "Mean_On_" << to_string(number) << ", " << point_nb << ", " << seed << ", " << stddev << ", " << width << ", " << height << ", " << "generation" << generation << ", " << mapSize << ", " << totalTime << "\n";

	file_pointer.close();
}

/* Used for launching every entry of the benchmark */

static void create_Benchmark(string filename, bool two_tabs = false) {

	string CPU = "";
	string CPU_nb = "";
	string Thread_nb = to_string(omp_get_max_threads());
	string RAM = "";
	string GPU = "";

	/* Get computeur components informations */
	#ifdef _WIN32
	SYSTEM_INFO siSysInfo;
	ULONGLONG ram_buffer;
	// Copy the hardware information to the SYSTEM_INFO structure. 
	GetSystemInfo(&siSysInfo);
	GetPhysicallyInstalledSystemMemory(&ram_buffer);

	switch (siSysInfo.wProcessorArchitecture)
	{
	case 9:
		CPU = "x64 AMD or Intel"; break;
	case 5:
		CPU = "ARM"; break;
	case 12:
		CPU = "ARM64"; break;
	case 6:
		CPU = "Intel titanium-based"; break;
	case 0:
		CPU = "x86"; break;
	case 0xffff:
		CPU = "Unknown architecture"; break;
	}

	CPU_nb = to_string(siSysInfo.dwNumberOfProcessors);

	RAM = to_string(ram_buffer) + "bytes";

	#endif
	#ifdef _unix_
		//TODO
	#endif
	#ifdef _MACH_
		//TODO
	#endif


	/* File initialisation*/

	ifstream file_pointer_read;
	file_pointer_read.open(filename, ios::in);
	string reading;
	getline(file_pointer_read, reading);
	file_pointer_read.close();
	ofstream file_pointer;
	/* opens an existing csv file or creates a new file. Every output operation will be push at the end of file.
		flag in : Open for input operations (reading essentially);
		flag out : Open for output operations.
		flag app : 	All output operations are performed at the end of the file, appending the content to the current content of the file.
	*/
	file_pointer.open(filename, ios::out | ios::app);
	if (!file_pointer)
	{
		cout << "Error in creating file!!! " << filename << endl;
	}
	if (reading == "") {
		file_pointer << "the computeur informations are : \n";
		file_pointer << "CPU_Architecture : " << CPU << "\n";
		file_pointer << "CPU_number : " << CPU_nb << "\n";
		file_pointer << "Thread_number : " << Thread_nb << "\n";
		file_pointer << "RAM capacity : " << RAM << "\n";
		file_pointer << "GPU_information : " << GPU << "\n";
		file_pointer << "\n";
		file_pointer << "Benchmark Name, number of points, seed used, stddev, width, height, generation used, densityMap size, elapsed_time (ns), \n";
	}
	else {
		cout << "Error, the file " << filename << "wasn't empty" << endl;
	}

	/* launch every entry */
	one_entry_benchmark(filename, 1, 10, two_tabs);
	one_entry_benchmark(filename, 2, 10, two_tabs);
	one_entry_benchmark(filename, 3, 10, two_tabs);
}

boolean full_test_tab_equality() {
	vector<int> point_Array;
	vector<int> point_ArrayX;
	vector<int> point_ArrayY;
	vector<vector<unsigned int>> densityMap_reference;
	vector<vector<unsigned int>> densityMap_tested;

	point_Array = generation_2();

	//determining the min X & Y, the max X & Y
	int maxY = point_Array[0], maxX = point_Array[0], minY = point_Array[0], minX = point_Array[0];
	for (unsigned int i = 0; i < point_nb * 2; i += 2) {
		if (point_Array[i] < minX) { minX = point_Array[i]; }
		else if (point_Array[i] > maxX) { maxX = point_Array[i]; }
		if (point_Array[i + 1] < minY) { minY = point_Array[i + 1]; }
		else if (point_Array[i + 1] > maxY) { maxY = point_Array[i + 1]; }
	}

	densityMap_reference = compute_densityMap_CPU_simple_1Tab(point_Array, maxX, minX, maxY, minY);
	densityMap_tested = compute_densityMap_CPU_multThread_1Tab(point_Array, maxX, minX, maxY, minY);
	if (!isEqual_2DArray(densityMap_reference, densityMap_tested)) { cout << "multi threaded density with 1 tab generation isn't equal to basic generation" << endl; display_2Darray(densityMap_reference); display_2Darray(densityMap_tested); return false; }
	/*densityMap_tested = compute_densityMap_GPU_1Tab(point_Array, maxX, minX, maxY, minY);
	if (!isEqual_2DArray(densityMap_reference, densityMap_tested)) { cout << "GPU density with 1 tab generation isn't equal to basic generation" << endl; return false; }*/

	point_Array = generation_1();

	//determining the min X & Y, the max X & Y
	maxY = point_Array[0];
	maxX = point_Array[0];
	minY = point_Array[0];
	minX = point_Array[0];
	for (unsigned int i = 0; i < point_nb * 2; i += 2) {
		if (point_Array[i] < minX) { minX = point_Array[i]; }
		else if (point_Array[i] > maxX) { maxX = point_Array[i]; }
		if (point_Array[i + 1] < minY) { minY = point_Array[i + 1]; }
		else if (point_Array[i + 1] > maxY) { maxY = point_Array[i + 1]; }
	}

	for (unsigned int i = 0; i < point_nb * 2; i += 2) {
		point_ArrayX.push_back(point_Array[i]);
		point_ArrayY.push_back(point_Array[i + 1]);
	}

	densityMap_reference = compute_densityMap_CPU_simple_2Tabs(point_ArrayX, point_ArrayY, maxX, minX, maxY, minY);
	densityMap_tested = compute_densityMap_CPU_multThread_2Tabs(point_ArrayX, point_ArrayY, maxX, minX, maxY, minY);
	if (!isEqual_2DArray(densityMap_reference, densityMap_tested)) { cout << "multi threaded density with 2 tabs generation isn't equal to basic generation" << endl; return false; }
	/*densityMap_tested = compute_densityMap_GPU_2Tabs(point_ArrayX, point_ArrayY, maxX, minX, maxY, minY);
	if (!isEqual_2DArray(densityMap_reference, densityMap_tested)) { cout << "GPU density with 2 tabs generation isn't equal to basic generation" << endl; return false; }*/


}


int main() {
  FFastDensity ff(256,256);
  float x[] = {1,2,3};
  float y[] = {1,2,3};

  //hello_World_OpenMp();

  if (!full_test_tab_equality()) {
	  cout << "----------------------------" << endl
		   << " /!\\ Equality tests aren't validated /!\\ " << endl
		   << "----------------------------" << endl;
  }

  /* ALL_PARAMS of the speed test. */
  cout << "the test will have the following params :" << endl;
  cout << " number of point to generate : " << point_nb << endl;
  cout << "seed for the tests : " << seed << endl;
  cout << "window width : " << width << endl;
  cout << "window height : " << height << endl;
  cout << "standard deviation for the normal distribution : " << stddev << endl;
  /*int answer = -1;
  bool display;
  cout << "display Y/N (resp 1/0) :" << endl;
  cin >> answer;
  answer == 0 ? display = false : display = true;*/
  string bench_name;

  /*vector<int> temp1 = generation_1(window_width, window_height, points_nb, seed, display);
  vector<int> temp2 = generation_2(stddev, points_nb, seed, display);*/

  /* getting the date for naming the file */
  stringstream ss;
  ss << "bench_" << return_current_time_and_date_as_string() << ".csv";
  string filename = ss.str();
  create_Benchmark(filename);

  /*add_Benchmark(filename, points_nb, seed, stddev, 1, window_width, window_height, true);
  add_Benchmark(filename, points_nb, seed, stddev, 1);
  add_Benchmark(filename, points_nb, seed, stddev, 1, window_width, window_height, true);*/
  

  /*auto start = chrono::steady_clock::now();
  ff.add(3, x, y);
  auto end = chrono::steady_clock::now();
  auto diff = end - start;
  cout << chrono::duration <double, nano> (diff).count() << " ns" << endl;
  */
  if (!glfwInit())
    return -1;

  GLFWwindow* window = glfwCreateWindow(640, 360, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwSetKeyCallback(window, key_callback);
  glfwMakeContextCurrent(window);
  /* Get opengl resources from now */

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
