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



class Value
{
public:
	int value_type;//0 for num,1 for function
	int num_value;
	Term *function_tree;
	std::vector<parameter> paras;
	Activation_Record* access_link;
	Value()
	{
		value_type = 0;
		num_value = 0;
		function_tree = NULL;
	}
};

class function
{
public:
	std::string name;
	Term *function_tree;
	Activation_Record* access_link;
	//std::set<std::string> available_var;
	std::vector<parameter> paras;
	function()
	{
		function_tree = NULL;
		access_link = NULL;
	}
};

class variable
{
public:
	std::string name;
	Value var_value;
};

class Activation_Record
{
public:
	Activation_Record* access_link;
	Activation_Record* control_link;
	std::vector<variable*> vars;
	std::vector<function> funs;
	Value return_value;
	int is_returned;
	Activation_Record()
	{
		is_returned = 0;
		return_value.value_type = 0;
		return_value.num_value = 0;
		return_value.function_tree = NULL;
		access_link = NULL;
		control_link = NULL;
	}
	Activation_Record(Activation_Record* a)
	{
		is_returned = a->is_returned;
		return_value = a->return_value;
		access_link = a->access_link;
		control_link = a->control_link;
		for (std::vector<variable*> ::iterator it = a->vars.begin(); it != a->vars.end(); it++)
		{
			vars.push_back(*it);
		}
		for (std::vector<function> ::iterator it = a->funs.begin(); it != a->funs.end(); it++)
		{
			funs.push_back(*it);
		}
	}
};

#endif
