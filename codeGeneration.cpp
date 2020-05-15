#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <string.h>
#include "tables.h"
using namespace std;

//readings of files
ifstream myFile;
string keywords[7] = {"int", "bool", "input", "output", "if", "else", "while"};
string separators[10] = {"'(){}[],:;"};
string operators[8] = {"*", "+", "-", "=", "/", ">", "<", "%"};
string temp= "";
char ch;
int memoryLoc=5000;
int currentMem=memoryLoc;
SymbolTable st[100];
string exId[100]={""};
InstructTable instrTable[100];
string lastId="";
int instrCounter=0;
int counterId=0;
int itCounter=0;
int stCounter=0;
int indexJump=0;
void A(string);
void I();
void lexical(string);
//finding index
int index(string word){
	for (int i=0; i<stCounter;i++){
		if(st[i].id==word){
			return i;
		}
	}
return -1;
}
bool containsKey(string word){
	for (int i=0; i<7;i++){
    if (keywords[i]== word)
    	return true;
  }
		return false;
}
bool containsNum(string word){
		if (isdigit(word[0]))
			return true;
		else
      return false;
}
bool containsBool(string word){
  if (word=="true"||word=="false"){
    return true;
  }
  else
    return false;
}
bool containsIdent(string word){
	for(int i=0;i<=counterId;i++){
		if(exId[i]==word){
			return true;
		}
	}
	return false;
}
string nextToken(){
	string token="";
	do{
		ch=myFile.get();
		if(ch != ' '&& ch !=','&& ch !=';' && ch !='\n' && ch != '&')
		token +=ch;
		}
		while(ch != ' '&& ch != '\n' && ch != '&');
		return token;
}
int get_address(string word){
	for(int i=0;i<stCounter;i++){
		if(st[i].id==word){
			return st[i].memoryLoc;
		}
	}
	return 0;
}
int gen_instr(string op, int oprnd){
	if(itCounter!=0){
		instrTable[itCounter].step=instrTable[itCounter-1].step+1;
	}
	instrTable[itCounter].Op=op;
	instrTable[itCounter].Oprnd=oprnd;
	itCounter++;
	instrCounter++;
}
void F(string token){
	if(containsIdent(token)){
		gen_instr("PUSHM",get_address(token));
	}
	else if(containsNum(token)){
		gen_instr("PUSHI",stoi(token));
	}
	else{
		cout<<"id Expected"<<endl;
	}
}
void T(string token){
	F(token);
}
void Ep(){
	string tokenEp="";
	if(ch!=myFile.eof()){
		tokenEp=nextToken();
	}
	if(tokenEp=="+"){
		tokenEp=nextToken();
		T(tokenEp);
		gen_instr("ADD",-1);
	}
	else if(tokenEp=="-"){
		tokenEp=nextToken();
		T(tokenEp);
		gen_instr("SUB",-1);
	}
	else if(tokenEp=="*"){
		tokenEp=nextToken();
		T(tokenEp);
		gen_instr("MUL",-1);
	}
	else if(tokenEp=="/"){
		tokenEp=nextToken();
		T(tokenEp);
		gen_instr("DIV",-1);
	}
}
void E() {
	string tokenE=nextToken();
	if(containsIdent(tokenE)){
		T(tokenE);
		Ep();
	}
	else if(containsBool(tokenE)){
		if(tokenE=="true")
			gen_instr("PUSHI",1);
		else
			gen_instr("PUSHI",0);
	}
	else if(containsNum(tokenE)){
		int number=stoi(tokenE);
		gen_instr("PUSHI",number);
	}
	else{
		cout<<"Error Unknown id "<<tokenE<<endl;
		system("pause");
		exit(0);
		}
	}
void C(){
	string tokenC=nextToken();
	lastId=tokenC;
		F(tokenC);
		string firstToken=tokenC;
		tokenC=nextToken();
		if(tokenC==">"){
		  tokenC=nextToken();
			F(tokenC);
			gen_instr("GRT",-1);
			indexJump=itCounter;
			gen_instr("JUMPZ",-1);
			instrCounter=0;
		}
		else if(tokenC=="<"){
			tokenC=nextToken();
			F(tokenC);
			gen_instr("LES",-1);
			indexJump=itCounter;
			gen_instr("JUMPZ",-1);
			instrCounter=0;
		}
		else if(tokenC=="=="){
			tokenC=nextToken();
			F(tokenC);
			gen_instr("EQU",-1);
			indexJump=itCounter;
			gen_instr("JUMPZ",-1);
			instrCounter=0;
		}
		else if(tokenC=="!="){
			tokenC=nextToken();
			F(tokenC);
			gen_instr("NEQ",-1);
			indexJump=itCounter;
			gen_instr("JUMPZ",-1);
			instrCounter=0;
		}
		else if(tokenC== ">="){
			tokenC =nextToken();
			F(tokenC);
			gen_instr("GEQ",-1);
			indexJump=itCounter;
			gen_instr("JUMPZ", -1);
			instrCounter=0;
		}
		else if(tokenC=="<="){
			tokenC=nextToken();
			F(tokenC);
			gen_instr("LEQ", -1);
			indexJump=itCounter;
			gen_instr("JUMPZ", -1);
			instrCounter=0;
		}
	}
void I(){
	string tokenI=nextToken();
	if(tokenI=="(")
	C();
	tokenI=nextToken();
	if(tokenI==")"){
		tokenI=nextToken();
		if(tokenI=="{"){
			tokenI=nextToken();
			do{
				lexical(tokenI);
				tokenI=nextToken();
			}
			while(tokenI !="}");
			instrTable[indexJump].Oprnd=instrTable[indexJump].step+instrCounter+1;
		}
	}
}
void ELSE(){
	string tokenE=nextToken();
	if(tokenE=="{"){
		tokenE=nextToken();
		do{
			lexical(tokenE);
			tokenE=nextToken();
		}
		while(tokenE !="}");
		instrTable[indexJump].Oprnd=instrTable[indexJump].step+instrCounter+1;
	}
}
void A(string word){
	string save=word;
	lastId=word;
	string tokenA=nextToken();
	if(tokenA=="="){
		E();
		gen_instr("POPM",get_address(save));
	}
	else{
		cout<<"= expected, and received "<<tokenA<<endl;
		system("pause");
		exit(0);
	}
}
void W(){
	string tokenW=nextToken();
	if(tokenW=="("){
		int indexJumpBack=itCounter;
		C();
		tokenW=nextToken();
		if(tokenW==")"){
			tokenW=nextToken();
			if(tokenW=="{"){
				tokenW=nextToken();
				do{
					lexical(tokenW);
					tokenW=nextToken();
				}
				while(tokenW !="}");
				gen_instr("JUMP",indexJumpBack);
				instrTable[indexJump].Oprnd=instrTable[indexJump].step+instrCounter+1;
			}
		}
	}
}
void INPUT(){
	string tokenI=nextToken();
	gen_instr("STDIN",-1);
	if(tokenI=="("){
		tokenI=nextToken();
		gen_instr("POPM",get_address(tokenI));
		tokenI=nextToken();
		if(tokenI !=")"){
			cout<<"Expecting ')' "<<endl;
			exit(0);
		}
	}
}
void OUTPUT(){
	string tokenO=nextToken();
	if(tokenO=="("){
		tokenO=nextToken();
		F(tokenO);
		tokenO=nextToken();
		gen_instr("STDOUT",-1);
		if(tokenO!=")"){
			cout<<"Expecting ')' "<<endl;
			exit(0);
		}
	}
}
void lexical(string word){
	if(containsKey(word)){
		if(word=="int"||word=="bool")
		st[stCounter].type=word;
		else if(word=="if"){
			I();
		}
		else if(word=="else"){
			ELSE();
		}
		else if(word=="while"){
			gen_instr("LABEL",-1);
			W();
		}
		else if(word=="input"){
			INPUT();
		}
		else if(word=="output"){
			OUTPUT();
		}
	}
	else if(!containsIdent(word)){
		exId[counterId]=word;
		counterId++;
		st[stCounter].id=word;
		st[stCounter].memoryLoc=currentMem;
		stCounter++;
		currentMem++;
		if(st[stCounter].type==""){
			st[stCounter].type=st[stCounter-1].type;
		}
	}
	else if(containsIdent(word)){
		A(word);
	}
	temp="";
}
int main() {
	st[0].memoryLoc=memoryLoc;
	string filenameIn;
	cout <<"Enter file to read:" << endl;
	cin >> filenameIn;
	myFile.open(filenameIn);
	//error if could not open so exit
	if (!myFile.is_open()){
		cout << "Error\n";
		exit(0);
	}
	//implementation of lex
	while (!myFile.eof()){
		temp=nextToken();
		if (ch=='&') {
			break;
		}
	lexical(temp);
}
  cout<<endl;
	cout<<"Instruction Table" << endl;
	cout<<"Step					Op					Oprnd" << endl;
	for(int i=0;i<itCounter;i++){
		cout<<instrTable[i].step<<"				|	" << instrTable[i].Op << "				|	"<<instrTable[i].Oprnd<<endl;
	}
	cout<<endl<<endl;
	cout<<"Symbol Table" << endl;
	cout<<"Identifier				Memory Location				Type"			 << endl;
	for (int i=0;i<stCounter;i++){
		cout<<st[i].id << "				|	" << st[i].memoryLoc << "				|	" << st[i].type << endl;
	}
	system("pause");
	return 0;
}
