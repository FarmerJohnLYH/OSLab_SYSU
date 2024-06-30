#include "file.h"

const string SEQ_FILE("../sequential.txt");
const string QUERY_FILE("../query.txt");
const string SEQ_RESULT("seqQueryResult.txt");
const string BINARY_RESULT("binaryQueryResult.txt");
const string IND_RESULT("indQueryResult.txt");

const int NUM = 1000;
const int DATA_SIZE = NUM * NUM - NUM;

void generateData() {
	int start = 100000;

	vector<string> vIDs(NUM);
	for (int ii = 0; ii < NUM; ++ii)
		vIDs[ii]=to_string(start++);

	ofstream foutq(QUERY_FILE);
	for (int ii = 0; ii < NUM; ++ii)
	{
		int iFirst = rand() % NUM, iSecond = rand() % NUM;
		if (iFirst == iSecond)
		{
			--ii;
			continue;
		}
		foutq << vIDs[iFirst] << ',' << vIDs[iSecond] << endl;
	}
	foutq.close();
	
	ofstream fouts(SEQ_FILE);
	for (int ii = 0; ii < NUM; ++ii)
	{
		const string strFirst = vIDs[ii];
		for (int jj = 0; jj < NUM; ++jj)
		{
			if (ii == jj)
				continue;
			fouts << strFirst <<','<< vIDs[jj] << '\t'<< VALUE_TYPE(rand()%2) << endl;
		}
	}
	fouts.close();
}

void setGlobalParameter()
{
	ifstream fin(SEQ_FILE);
	if (!fin.is_open())
	{
		cout << "need to generate data first.";
		return;
	}
	string temp;
	getline(fin, temp);
	iLineSize = fin.tellg();
	fin.close();
}

VALUE_TYPE queryInSequential(const string & strQuery, ifstream & fin)
{
	fin.seekg(0, ios::beg);
	while (!fin.eof())
	{
		string temp;
		VALUE_TYPE value;
		fin >> temp >> value;
		if (temp.length() == 0)
			break;
		if (strQuery.compare(temp) == 0)
			return value;
	}
	return 0;
}

void testQuerySeq(bool useBinarySearch=false)
{
	ifstream finQ(QUERY_FILE);
	ifstream finS(SEQ_FILE);
	clock_t tStart, tEnd;
	vector<VALUE_TYPE> vResults;
	tStart=clock();
	while (!finQ.eof())
	{
		string strQuery;
		finQ >> strQuery;
		if (strQuery.length() == 0)
			break;
		if(!useBinarySearch)
			vResults.push_back(queryInSequential(strQuery, finS));
		else
			vResults.push_back(binaryQueryInSequential(strQuery, finS, DATA_SIZE));
	}
	tEnd=clock();
	double cost = (double)(tEnd - tStart) / CLOCKS_PER_SEC;
	cout << "average qtime: "<< cost*1000000000/vResults.size() <<"ns" << endl;

	string filename = useBinarySearch ? BINARY_RESULT : SEQ_RESULT;
	ofstream foutSR(filename);
	for (int ii = 0; ii < vResults.size(); ++ii)
		foutSR << vResults[ii];
	foutSR.close();
	finS.close();
	finQ.close();
}

void testQueryIndex()
{
	Index index;
	index.buildIndex(SEQ_FILE, DATA_SIZE, 3);
	ifstream finQ(QUERY_FILE);
	ifstream finS(SEQ_FILE);
	clock_t tStart, tEnd;
	vector<VALUE_TYPE> vResults;
	tStart=clock();
	while (!finQ.eof())
	{
		string strQuery;
		finQ >> strQuery;
		if (strQuery.length() == 0)
			break;
		vResults.push_back(index.search(strQuery, finS));
	}
	tEnd=clock();
	double cost = (double)(tEnd - tStart) / CLOCKS_PER_SEC;
	cout << "average qtime: "<<cost*1000000000/vResults.size() <<"ns" << endl;

	ofstream foutSR(IND_RESULT);
	for (int ii = 0; ii < vResults.size(); ++ii)
		foutSR << vResults[ii];
	foutSR.close();
	finS.close();
	finQ.close();
}

int main(int argc, char** argv)
{
	char action = 'g';
	int position = 1;
	if (argc > position)
		action = argv[position][0];
	--position;

	switch (action)
	{
	case 'g':
		generateData();
		break;
	case 'b':
		setGlobalParameter();
		testQuerySeq(true);
		break;
	case 's':
		setGlobalParameter();
		testQuerySeq(false);
		break;
	case 'i':
		setGlobalParameter();
		testQueryIndex();
		break;
	default:
		break;
	}

	ifstream fin("/proc/self/status");
	char line[128];
	while (fin.getline(line, 128))
		if (strncmp(line,"VmRSS:", 6)==0)
		{
			printf("memory: %d KB\n", atoi(line + 6));
			break;
		}
	fin.close();
	return 0;
}
