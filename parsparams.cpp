#include <fstream>
#include <iostream>

using namespace std;

struct ControlParameters 
{
	bool fromUser; //данные от пользователя
	unsigned packetCount; //количество имитируемых пакетов.
	unsigned packetSize; //количество отсчётов в имитируемом пакете
	unsigned maxLevel; //максимально возможное значение отсчёта сигнала
	unsigned A; 
    unsigned B;
	unsigned procDelay; //длительность времени обработки в миллисекундах (не менее)
};

unsigned getDigits(string curLine);

bool parsParams(string fileName, ControlParameters& params){

	string line;
	ifstream in(fileName.c_str());

	if (in.is_open()){

		cout <<"File '" << fileName.c_str() << "' succesfully found." << endl;
		unsigned i = 0;
		
		while(getline(in, line)) {
			++i;
			switch(i) {
				case 1:
					params.packetCount = getDigits(line);
					break;
				case 2:
					params.packetSize = getDigits(line);
					break;
				case 3:
					params.maxLevel = getDigits(line);
					break;
				case 4:
					params.A = getDigits(line);
					break;
				case 5:
					params.B = getDigits(line);
					break;
			}
		}
		in.close();
		if (i > 4)
			params.fromUser = false;
		else
			return false;
	}
	else {
		in.close();
		cout << "File '"<< fileName.c_str() << "' not found!" << endl;
		return false;
	}
	return true;
}

unsigned getDigits(string curLine) {
	string result = "";
	char tmp;

	for (int i = 0; i < curLine.length(); ++i){
		tmp = curLine[i];

		if ('0' <= tmp && tmp <= '9')
			result += curLine[i];
		else
			if (result != "")
				break;
	}

	return atoi(result.c_str());
}