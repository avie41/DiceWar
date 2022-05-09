#include <fstream>
#include <string>
#include <cstdlib>
#include <iostream>
#include<unistd.h>

int main(int argc, char* argv[]) {
	unsigned int n;
	if (argc >= 2) n= (unsigned int)atoi(argv[1]);
	else n = 100;

	std::string strat1 = "StrategyDummy.dll";
	std::string strat2 = "pStrategy.dll";
	if (argc >= 3) strat2 = std::string(argv[2]);
	if (argc >= 4) strat1 = std::string(argv[3]);
	system("del output.txt");
	
	char* wd = (char*)malloc(1000 * sizeof(char));
	char* tmp = getcwd(wd, 1000);
	if (!tmp) return(-1);
	std::string command = std::string(wd) + "\\bin\\x64\\Debug\\DiceWars.exe -r Referee.dll -m GenMap.dll -s " +strat1 + " -s "+ strat2;
	//std::cout << command << std::endl;
	free(wd);

	for (unsigned int i = 0; i < n; ++i) {
		std::system(command.c_str());
	}

	std::cout << strat1 << " vs " << strat2<<std::endl;

	int total = 0;
	std::ifstream file("output.txt");
	if (file.fail()) return(-1);
	else {
		std::string line;
		std::cout << "resultats des parties:" << std::endl;
		for (unsigned int i = 1; std::getline(file, line); ++i) {
			std::cout << line << '\t';
			if (line == "1") total++;
		}
	}
	file.close();

	std::cout << std::endl << (float(total) / n) * 100 << "% de winrate" << std::endl;
	system("pause");
	system("del output.txt");
	return 0;
}