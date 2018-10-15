#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <sstream>
#include "Heap.h"


using namespace std;
typedef bool (*Command_t)(string args,map<string,void*>&);


//Function prototypes
static string getCommand(string prompt);
static bool exitTerminal(string args,map<string,void*>&) {return false;}
static bool showHelp(string args,map<string,void*>&);
static bool mallocData(string args,map<string,void*>&);
static bool callocData(string args,map<string,void*>&);
static bool reallocData(string args,map<string,void*>&);
static bool freeData(string args,map<string,void*>&);
static bool printRegister(string args,map<string,void*>&);
static bool printAddress(string args,map<string,void*>&);
static bool dumpHeap(string args,map<string,void*>&);
static bool listRegisters(string args,map<string,void*>&);


//
// Main testing utility
//
int main(void) {

	//void* ptr = my_malloc(999999);

	cout << "Heap Testing Utility" << endl;
	cout << "--------------------" << endl;
	cout << "Type 'h' for list of commands" << endl;
	cout << "Type 'q' to quit" << endl << endl;

	map<char,Command_t> commands;
	map<string,void*> regs;
	commands['q'] = exitTerminal;
	commands['h'] = showHelp;
	commands['m'] = mallocData;
	commands['c'] = callocData;
	commands['r'] = reallocData;
	commands['f'] = freeData;
	commands['a'] = printAddress;
	commands['p'] = printRegister;
	commands['d'] = dumpHeap;
	commands['l'] = listRegisters;


	//Accept commands until it ends
	bool running = true;
	while (running) {
		string args, command = getCommand("> ");
		stringstream ss(command);
		char c = 0;
		
		ss >> c;
		getline(ss,args);
		if (c == 0) {continue;}
		try {
			//Execute the command from a function pointer
			if (!commands.at(c)(args,regs)) {running = false;}
		} catch (...) {
			cout << "Invalid command '" << c << "'" << endl;
		}
	}

	return 0;
}


//
// Get a command from the terminal
//
static string getCommand(string prompt) {
	string command;
	cout << prompt;
	getline(cin,command);
	return command;
}



//
// Show the help menu
//
static bool showHelp(string args,map<string,void*>& regs) {
	cout << "  h - Show the help screen" << endl;
	cout << "  q - Quit the program" << endl << endl;
	cout << "  m <reg> <bytes> - Malloc <bytes> into <reg>" << endl;
	cout << "  c <reg> <bytes> - Calloc <bytes> into <reg>" << endl;
	cout << "  r <reg> <bytes> - Realloc <reg> to <bytes>" << endl;
	cout << "  f <reg> - Free <reg>" << endl << endl;
	cout << "  a <reg> - Print out address of <reg>" << endl;
	cout << "  p <reg> - Print out heap info of <reg>" << endl;
	cout << "  d - Dump the contents of the heap" << endl;
	cout << "  l - List all registers" << endl;
	return true;
}


//
// Parse <reg> <size> arguments
//
struct Parsed {
	string reg;
	long size;
	Parsed(const string& s, long size) : reg(s), size(size) {}
};
static Parsed parseAllocArgs(const string& args) {
	stringstream ss(args);
	string reg;
	string num;
	long size;

	//Parse arguments
	ss >> reg >> num;
	size = atol(num.c_str());

	if (reg == "") {cout << "Please specify a register" << endl; return Parsed("",0);}
	if (size == 0) {cout << "Size cannot be 0" << endl; return Parsed("",0);}
	if (size < 0) {cout << "Size cannot be negative" << endl; return Parsed("",0);}
	return Parsed(reg,size);
}

static string parseRegArgs(const string& args) {
	stringstream ss(args);
	string reg;

	ss >> reg;
	if (reg == "") {cout << "Please specify a register" << endl;}
	return reg;
}


//
// Malloc
//
static bool mallocData(string args, map<string,void*>& regs) {
	Parsed p = parseAllocArgs(args);
	if (p.size == 0) {return true;}

	try {
		regs.at(p.reg);
		cout << "Register '" << p.reg << "' is already allocated" << endl;
	} catch (...) {
		regs[p.reg] = my_malloc(p.size);
		printf("%s = %p\n",p.reg.c_str(),regs[p.reg]);
	}
	return true;
}

//
// Calloc
//
static bool callocData(string args,map<string,void*>& regs) {
	Parsed p = parseAllocArgs(args);
	if (p.size == 0) {return true;}

	try {
		regs.at(p.reg);
		cout << "Register '" << p.reg << "' is already allocated" << endl;
	} catch (...) {
		regs[p.reg] = my_calloc(1,p.size);
		printf("%s = %p\n",p.reg.c_str(),regs[p.reg]);
	}
	return true;
}


//
// Realloc
//
static bool reallocData(string args,map<string,void*>& regs) {
	Parsed p = parseAllocArgs(args);
	if (p.size == 0) {return true;}

	try {
		void* ptr = regs.at(p.reg);
		void* new_ptr = my_realloc(ptr,p.size);
		if (!new_ptr) {cout << "Failed to reallocate register '" << p.reg << "' to size " << p.size << endl; return true;}
	
		regs[p.reg] = new_ptr;
		printf("%s = %p\n",p.reg.c_str(),regs[p.reg]);
	} catch (...) {
		cout << "Register '" << p.reg << "' does not exist" << endl;
	}

	return true;
}


//
// Free
//
static bool freeData(string args,map<string,void*>& regs) {
	
	string reg = parseRegArgs(args);
	if (reg == "") {return true;}

	try {
		void* ptr = regs.at(reg);
		my_free(ptr);
		regs.erase(reg);
		cout << "Freed register '" << reg << "'" << endl;
	} catch (...) {
		cout << "Register '" << reg << "' does not exist" << endl;
	}
	return true;
}



//
// Print out the heap contents of a single register
//
static bool printRegister(string args,map<string,void*>& regs) {
	string reg = parseRegArgs(args);
	if (reg == "") {return true;}

	try {
		void* ptr = regs.at(reg);
		print_heap_entry(ptr);
	} catch (...) {
		cout << "Register '" << reg << "' does not exist" << endl;
	}
	return true;
}


//
// Print out the address of a register
//
static bool printAddress(string args,map<string,void*>& regs) {
	string reg = parseRegArgs(args);
	if (reg == "") {return true;}

	try {
		void* ptr = regs.at(reg);
		printf("%s = %p\n",reg.c_str(), ptr);
	} catch (...) {
		cout << "Register '" << reg << "' does not exist" << endl;
	}
	return true;
}


//
// Dump all the contents of the heap
//
static bool dumpHeap(string args,map<string,void*>& regs) {
	dump_heap();
	return true;
}


//
// List all registers
//
static bool listRegisters(string args,map<string,void*>& regs) {
	for (auto it = regs.cbegin(); it != regs.cend(); ++it) {
		printf("%s = %p\n",it->first.c_str(),it->second);
	}
	return true;
}
