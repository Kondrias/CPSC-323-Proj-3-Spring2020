#pragma once
#include <string>
using namespace std;

struct SymbolTable {
	string id;
	int memoryLoc;
	string type;
};
struct InstructTable {
	int step=1;
	string Op;
	int Oprnd;
};
