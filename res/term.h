/*Term.h
为Minilan语言编写的Term类，用于处理程序文件，将其转化为语法树。
*/

#ifndef TERM_H
#define TERM_H
#include<string>
#include<list>
#include <vector>
#include <set>
#include<iostream>
enum TermKind {
	Block = 0,
	Function,
	Command,
	Expr,
	BoolExpr,
	Name,
};
enum TermSubtype {
	Declaration = 0, Assign = 1, Read = 2, Print = 3, Return = 4, If, While, Call,
	Number, VarName, Plus, Minus, Mult, Div, Mod, Apply,
	Lt, Gt, Eq, And, Or, Negb,
};
class Term {
public:
	TermKind kind;
	TermSubtype subtype;
	Term* father;
	std::list<Term*> sons;
	int number;
	std::string name;

	Term() {}
	Term(TermKind kind) { this->kind = kind; }
	void print();
};

Term* parse(std::istream& input, std::string pretext, Term* father, bool NameorExpr);

class parameter
{
public:
	int type;//0 for var , 1 for function
	std::string name;
	Term *function_tree;
};

class Activation_Record;

class function
{
public:
	std::string name;
	Term *function_tree;
	Activation_Record* access_link = NULL;
	std::set<std::string> available_var;
	std::vector<parameter> paras;
};

class variable
{
public:
	std::string name;
	int value;
};

class Activation_Record
{
public:
	Activation_Record* access_link = NULL;
	Activation_Record* control_link = NULL;
	std::vector<variable> vars;
	std::vector<function> funs;
	int return_value;
	int is_returned = 0;
};

#endif
