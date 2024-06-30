#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>

//#define SHOW_DETAILS

using namespace std;

class Matrix {
public:
	Matrix(int n, int m=1, string matrixName="") :rows(n), columns(m), name(matrixName), data(NULL) {
		initial();
	}
	
	~Matrix() { free(); }

	void read(string filename) {
		ifstream fin(filename.c_str());
		for (int ii = 0; ii < rows; ++ii)
			for (int jj = 0; jj < columns; ++jj)
				fin >> data[ii][jj];
		fin.close();
	}

	void show() {
		#ifdef SHOW_DETAILS
		cout << "Matrix " << name << ":" << endl;
		for (int ii = 0; ii < rows; ++ii) {
			for (int jj = 0; jj < columns; ++jj)
				cout << data[ii][jj] << '\t';
			cout << endl;
		}
		#endif
	}

	int* operator [](int ii) {
		return data[ii];
	}

private:
	int** data;
	int rows;
	int columns;
	string name;

	void initial() {
		free();
		if (rows < 1) {
			data = NULL;
			return;
		}
		data = new int* [rows];
		for (int ii = 0; ii < rows; ++ii)
			data[ii] = columns > 0 ? new int[columns] : NULL;
	}
	void free() {
		if (data)
		{
			for (int ii = 0; ii < rows; ++ii) {
				if (data[ii]) {
					delete[] data[ii];
					data[ii] = NULL;
				}
			}
			delete[] data;
			data = NULL;
		}
	}
};

class State {
public:
	vector<int> resource;
	Matrix claim;
	Matrix alloc;
	string dataFolder;
	
	State(int n, int m) :numOfProcess(n), numOfResource(m), claim(n, m, "claim"), alloc(n, m, "allocation"), resource(m),available(m),dataFolder(".") {}

	void show() {
		#ifdef SHOW_DETAILS
		claim.show();
		alloc.show();
		cout << "Vector resource:" << endl;
		for (int ii = 0; ii < numOfResource; ++ii)
			cout << resource[ii] << '\t';
		cout << endl;
		cout << "Vector available:" << endl;
		for (int ii = 0; ii < numOfResource; ++ii)
			cout << available[ii] << '\t';
		cout << endl;
		#endif
	}

	void setStudentID();
	string studentID;

	void test1()
	{
		alloc.read(dataFolder+"/alloc1.txt");
		test();
	}

	void test2()
	{
		alloc.read(dataFolder+"/alloc2.txt");
		test();
	}

	void test3()
	{
		alloc.read(dataFolder+"/alloc3.txt");
		test();
	}

private:
	vector<int> available;
	int numOfProcess, numOfResource;
	

	void calculateAvailable() {
		for (int ii = 0; ii < numOfResource; ++ii)
		{
			int sum = 0;
			for (int jj = 0; jj < numOfProcess; ++jj)
				sum += alloc[jj][ii];
			available[ii] = resource[ii] - sum;
		}
	}

	bool safe();

	void test() {
		claim.read(dataFolder+"/claim.txt");
		ifstream fin(dataFolder+"/resource.txt");
		for (int ii = 0; ii < numOfResource; ++ii)
			fin >> resource[ii];
		fin.close();
		calculateAvailable();

		show();
		if (safe())
			cout << "safe" << '\t';
		else
			cout << "unsafe" << '\t';
	}
};