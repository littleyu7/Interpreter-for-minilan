#ifndef TERM_CPP
#define TERM_CPP
#include "Term.h"
#include <iostream>
#include <fstream>
#include<sstream>
#include<string>
#include<cstdlib>
#include <stack>

#define UNDEFINED 0x7fffffff

#ifndef DEBUG_MODE
#define DEBUG_MODE 1
#endif

std::ifstream file_input("input.txt");
std::ofstream file_output("output.txt");

bool isnumber(const std::string &str) {
	size_t i;
	for (i = 0; i < str.size(); i++)
		if (!isdigit(str[i]))
			return false;
	return true;
}

bool isvar(const std::string &str) {
	size_t i;
	for (i = 0; i < str.size(); i++)
		if ('a'>str[i] || 'z'<str[i])
			return false;
	return true;
}

bool isCommandtoken(const std::string&str) {
	if (str == "Var" ||
		str == "Assign" ||
		str == "Call" ||
		str == "Read" ||
		str == "Print" ||
		str == "If" ||
		str == "While" ||
		str == "Return"
		)
		return true;
	return false;
}

bool isExprtoken(const std::string &str) {
	if (str == "Plus" ||
		str == "Minus" ||
		str == "Mult" ||
		str == "Div" ||
		str == "Mod" ||
		str == "Apply"
		)
		return true;
	return false;
}

bool isBoolExprtoken(const std::string &str) {
	if (str == "Lt" ||
		str == "Gt" ||
		str == "Eq" ||
		str == "And" ||
		str == "Or" ||
		str == "Negb"
		)
		return true;
	return false;
}

Term* parse(std::istream& input, std::string pretext = "", Term* father = nullptr, bool ExprNeeded = 0)
{
	if (input.eof()) return father;
	if (pretext == "") input >> pretext;
	if (DEBUG_MODE)std::cout << pretext << ' ';
	Term *cur_term = new Term();
	std::string next_text;
	if (isnumber(pretext)) {
		cur_term->kind = Expr;
		cur_term->subtype = Number;
		cur_term->number = atoi(pretext.data());
		if (DEBUG_MODE)std::cout << cur_term->kind << ' ';
	}
	else if (isvar(pretext)) {
		if (ExprNeeded) {
			cur_term->kind = Expr;
			cur_term->subtype = VarName;
		}
		else {
			cur_term->kind = Name;
		}
		cur_term->name = pretext;
		if (DEBUG_MODE)std::cout << cur_term->kind << ' ';
	}
	else if (pretext == "Begin") {
		cur_term->kind = Block;
		if (DEBUG_MODE)std::cout << cur_term->kind << ' ' << '\n';
		input >> next_text;
		while (next_text != "End") {
			Term *new_term = parse(input, next_text, cur_term);
			if (new_term == nullptr) {
				std::cout << "Warning: Missing Commands/Functions.\n";
			}
			else if (new_term->kind != Command		&&
				new_term->kind != Function) {
				std::cout << "Error: Command/Function needed\n";
				std::cout << "Type recieved is " << new_term->kind << std::endl;
				return nullptr;
			}
			if (input.eof()) break;
			input >> next_text;
		}
		if (DEBUG_MODE)std::cout << "Block End\n";
	}

	else if (pretext == "Function") {
		cur_term->kind = Function;
		if (DEBUG_MODE)std::cout << cur_term->kind << ' ';
		Term *new_variable = parse(input, "", cur_term);
		if (new_variable == nullptr || new_variable->kind != Name) {
			std::cout << "Error: Function name not found\n";
			return nullptr;
		}
		if (input.eof()) {
			std::cout << "Error: Function Parameter not found\n";
			return nullptr;
		}
		input >> next_text;
		if (next_text != "Paras") {
			std::cout << "Error: Function Parameter not found\n";
			return nullptr;
		}
		if (input.eof()) {
			std::cout << "Error: Function Parameters not found\n";
			return nullptr;
		}
		input >> next_text;
		while (next_text != "Begin") {
			Term *new_name = parse(input, next_text, cur_term);
			if (new_name == nullptr) {
				std::cout << "Error: Variable name needed for Parameters\n";
				return nullptr;
			}
			else if (new_name->kind != Name) {
				std::cout << "Error: Name needed for Parameters.\n";
				std::cout << "Type Recieved :" << new_name->kind << '\n';
				return nullptr;
			}
			if (input.eof()) {
				std::cout << "Error: Program section needed for Function.\n";
				return nullptr;
			}
			input >> next_text;
		}

		Term *new_pro = parse(input, next_text, cur_term);
		if (new_pro == nullptr || new_pro->kind != Block) {
			std::cout << "Error: Program section needed for Function\n";
			return nullptr;
		}
		if (DEBUG_MODE)std::cout << "Function section End\n";
	}

	else if (isCommandtoken(pretext)) {
		cur_term->kind = Command;
		if (DEBUG_MODE)std::cout << cur_term->kind << ' ';
		if (pretext == "Var") {
			cur_term->subtype = Declaration;
			input >> next_text;
			while (next_text != "End") {
				Term *new_name = parse(input, next_text, cur_term);
				if (new_name == nullptr) {
					std::cout << "Error: Variable name needed for Declaration\n";
					return nullptr;
				}
				else if (new_name->kind != Name) {
					std::cout << "Error: Name needed for Declaration.\n";
					return nullptr;
				}
				if (input.eof()) break;
				input >> next_text;
			}
		}
		else if (pretext == "Assign") {
			cur_term->subtype = Assign;
			Term *new_name, *new_expr;
			new_name = parse(input, "", cur_term);
			if (new_name == nullptr || new_name->kind != Name) {
				std::cout << "Error:Assignment: Variable name needed\n";
				return nullptr;
			}
			new_expr = parse(input, "", cur_term, true);
			if (new_expr == nullptr || new_expr->kind != Expr) {
				std::cout << "Error:Assignment: Expr needed\n";
				return nullptr;
			}
		}
		else if (pretext == "Call") {
			cur_term->subtype = Call;
			Term *new_functionname;
			new_functionname = parse(input, "", cur_term);
			if (new_functionname == nullptr || new_functionname->kind != Name) {
				std::cout << "Error: Function call:Function name needed\n";
				return nullptr;
			}
			if (input.eof()) {
				std::cout << "Error: Function call:Argument section needed\n";
				return nullptr;
			}
			input >> next_text;
			if (next_text == "Argus") {
				input >> next_text;
				while (next_text != "End") {
					Term *new_argu = parse(input, next_text, cur_term, true);
					if (new_argu == nullptr) {
						std::cout << "Error: Term needed for Argument\n";
					}
					else if (new_argu->kind != Expr) {
						std::cout << "Error: Expr needed for Argument\n";
					}
					if (input.eof()) {
						std::cout << "Error: Missing End for Argument Section\n";
						return nullptr;
					}
					input >> next_text;
				}
			}
		}
		else if (pretext == "Read") {
			cur_term->subtype = Read;
			Term *new_name = parse(input, "", cur_term);
			if (new_name == nullptr || new_name->kind != Name) {
				std::cout << "Error: Read:Variable name needed\n";
				return nullptr;
			}
		}
		else if (pretext == "Print" || pretext == "Return") {
			cur_term->subtype = pretext == "Print" ? Print : Return;
			Term *new_expr = parse(input, "", cur_term, true);
			if (new_expr == nullptr || new_expr->kind != Expr) {
				std::cout << "Error: Print/Return:Expr needed\n";
				return nullptr;
			}
		}
		else if (pretext == "If") {
			cur_term->subtype = If;
			Term *new_boolexpr = parse(input, "", cur_term);
			if (new_boolexpr == nullptr || new_boolexpr->kind != BoolExpr) {
				std::cout << "Error: If case:BoolExpr needed\n";
				return nullptr;
			}

			Term *new_pro1 = parse(input, "", cur_term);
			if (new_pro1 == nullptr || new_pro1->kind != Block) {
				std::cout << "Error: Program block needed for If-Then case\n";
				return nullptr;
			}
			if (input.eof()) {
				std::cout << "Error: If case:Else case needed\n";
				return nullptr;
			}
			input >> next_text;
			if (next_text != "Else") {
				std::cout << "Error: If case:Else case needed\n";
				return nullptr;
			}
			Term *new_pro2 = parse(input, "", cur_term);
			if (new_pro2 == nullptr || new_pro2->kind != Block) {
				std::cout << "Error: Program block needed for If-Else case\n";
				return nullptr;
			}
		}
		else if (pretext == "While") {
			cur_term->subtype = While;
			Term *new_boolexpr;
			new_boolexpr = parse(input, "", cur_term);
			if (new_boolexpr == nullptr || new_boolexpr->kind != BoolExpr) {
				std::cout << "Error: While case:BoolExpr needed\n";
				return nullptr;
			}
			Term *new_pro = parse(input, "", cur_term);
			if (new_pro == nullptr || new_pro->kind != Block) {
				std::cout << "Error: Program block needed for While case\n";
				return nullptr;
			}
		}
		if (DEBUG_MODE)std::cout << "Command section End\n";
	}
	else if (isExprtoken(pretext)) {
		cur_term->kind = Expr;
		if (DEBUG_MODE)std::cout << cur_term->kind << ' ';
		if (pretext == "Plus" || pretext == "Minus" ||
			pretext == "Mult" || pretext == "Div" ||
			pretext == "Mod") {
			if (pretext == "Plus") cur_term->subtype = Plus;
			else if (pretext == "Minus") cur_term->subtype = Minus;
			else if (pretext == "Mult") cur_term->subtype = Mult;
			else if (pretext == "Div") cur_term->subtype = Div;
			else if (pretext == "Mod") cur_term->subtype = Mod;
			Term *new_expr1, *new_expr2;
			new_expr1 = parse(input, "", cur_term, true);
			if (new_expr1 == nullptr || new_expr1->kind != Expr) {
				std::cout << "Error:Inside Expr: First Expr needed\n";
				return nullptr;
			}
			new_expr2 = parse(input, "", cur_term, true);
			if (new_expr2 == nullptr || new_expr2->kind != Expr) {
				std::cout << "Error:Inside Expr: Second Expr needed\n";
				return nullptr;
			}
		}
		else if (pretext == "Apply") {
			cur_term->subtype = Apply;
			Term *new_name;
			new_name = parse(input, "", cur_term);
			if (new_name == nullptr || new_name->kind != Name) {
				std::cout << "Error: Appfun:Function name needed\n";
				return nullptr;
			}
			if (input.eof()) {
				std::cout << "Error: Function call:Argument section needed\n";
				return nullptr;
			}
			input >> next_text;
			if (next_text == "Argus") {
				input >> next_text;
				while (next_text != "End") {
					Term *new_term = parse(input, next_text, cur_term, true);
					if (new_term == nullptr) {
						std::cout << "Error: Term needed for Argument\n";
					}
					else if (new_term->kind != Expr) {
						std::cout << "Error: Expr needed for Argument\n";
					}
					if (input.eof()) {
						std::cout << "Error: Missing End for Argument Section\n";
						return nullptr;
					}
					input >> next_text;
				}
			}
		}
		if (DEBUG_MODE)std::cout << "Expr section End\n";
	}
	else if (isBoolExprtoken(pretext)) {

		cur_term->kind = BoolExpr;
		if (DEBUG_MODE)std::cout << cur_term->kind << ' ';
		if (pretext == "Lt") cur_term->subtype = Lt;
		else if (pretext == "Gt") cur_term->subtype = Gt;
		else if (pretext == "Eq") cur_term->subtype = Eq;
		else if (pretext == "And") cur_term->subtype = And;
		else if (pretext == "Or") cur_term->subtype = Or;
		else if (pretext == "Negb") cur_term->subtype = Negb;
		if (pretext == "Lt" || pretext == "Gt" || pretext == "Eq") {
			Term *new_expr1, *new_expr2;
			new_expr1 = parse(input, "", cur_term, true);
			if (new_expr1 == nullptr || new_expr1->kind != Expr) {
				std::cout << "Error:Inside Boolexpr: First Expr needed\n";
				return nullptr;
			}
			new_expr2 = parse(input, "", cur_term, true);
			if (new_expr2 == nullptr || new_expr2->kind != Expr) {
				std::cout << "Error:Inside Boolexpr: Second Expr needed\n";
				return nullptr;
			}
		}
		else if (pretext == "And" || pretext == "Or") {
			Term *new_expr1, *new_expr2;
			new_expr1 = parse(input, "", cur_term);
			if (new_expr1 == nullptr || new_expr1->kind != BoolExpr) {
				std::cout << "Error:Inside Boolexpr: First BoolExpr needed\n";
				return nullptr;
			}
			new_expr2 = parse(input, "", cur_term);
			if (new_expr2 == nullptr || new_expr2->kind != BoolExpr) {
				std::cout << "Error:Inside Boolexpr: Second BoolExpr needed\n";
				return nullptr;
			}
		}
		else if (pretext == "Negb") {
			Term *new_expr;
			new_expr = parse(input, "", cur_term);
			if (new_expr == nullptr || new_expr->kind != BoolExpr) {
				std::cout << "Error:Inside Negb: BoolExpr needed\n";
				return nullptr;
			}
		}
		if (DEBUG_MODE)std::cout << "Boolexpr section End\n";
	}
	if (father != nullptr&&cur_term != nullptr) {
		father->sons.push_back(cur_term);
		cur_term->father = father;
	}

	return cur_term;
}

int tabs = 0;

void printtabs() {
	for (int i = 0; i<tabs; i++) {
		std::cout << "   ";
	}
}

void addtab() { tabs++; }

void removetab() { tabs--; }

void Term::print() {
	if (kind == Block) {
		std::cout << "Begin\n" << std::flush;
		addtab();
		for (std::list<Term*>::iterator i = sons.begin(); i != sons.end(); i++) {
			printtabs();
			(*i)->print();
		}
		removetab();
		printtabs();
		std::cout << "End\n" << std::flush;
	}
	else if (kind == Function) {
		std::cout << "Function " << std::flush;
		std::list<Term*>::iterator i = sons.begin();
		(*i)->print();
		std::cout << "Paras " << std::flush;
		for (i++; i != sons.end(); i++) {
			(*i)->print();
		}
	}
	else if (kind == Command) {
		std::string showwords[] = { "Var","Assign","Read","Print","Return" };
		if (subtype == Declaration || subtype == Assign ||
			subtype == Read || subtype == Print || subtype == Return) {
			std::cout << showwords[subtype] << ' ' << std::flush;
			for (std::list<Term*>::iterator i = sons.begin(); i != sons.end(); i++)(*i)->print();
			if (subtype == Declaration)std::cout << "End";
			std::cout << "\n" << std::flush;
		}
		else if (subtype == If) {
			std::cout << "If " << std::flush;
			std::list<Term*>::iterator i = sons.begin();
			(*(i++))->print();
			(*(i++))->print();
			printtabs();
			std::cout << "Else " << std::flush;
			(*i)->print();
		}
		else if (subtype == While) {
			std::cout << "While " << std::flush;
			std::list<Term*>::iterator i = sons.begin();
			(*(i++))->print();
			(*i)->print();
		}
		else if (subtype == Call) {
			std::cout << "Call " << std::flush;
			std::list<Term*>::iterator i = sons.begin();
			(*(i++))->print();
			std::cout << " Argus " << std::flush;
			for (i; i != sons.end(); ++i)(*i)->print();
			std::cout << "End " << std::flush;
		}
	}
	else if (kind == Expr) {
		std::string showwords[] = { "Plus","Minus","Mult","Div","Mod" };
		if (subtype == Plus || subtype == Minus ||
			subtype == Mult || subtype == Div ||
			subtype == Mod) {
			std::cout << showwords[subtype - Plus] << ' ' << std::flush;
			for (std::list<Term*>::iterator i = sons.begin(); i != sons.end(); i++)(*i)->print();
		}
		else if (subtype == Number) std::cout << number << ' ' << std::flush;
		else if (subtype == VarName)std::cout << name << ' ' << std::flush;
		else if (subtype == Apply) {
			std::cout << "Apply " << std::flush;
			std::list<Term*>::iterator i = sons.begin();
			(*(i++))->print();
			std::cout << " Argus " << std::flush;
			for (i; i != sons.end(); ++i)(*i)->print();
			std::cout << "End " << std::flush;
		}
	}
	else if (kind == BoolExpr) {
		std::string showwords[] = { "Lt","Gt","Eq","And","Or","Negb" };
		if (subtype == Lt || subtype == Gt ||
			subtype == Eq || subtype == And ||
			subtype == Or || subtype == Negb) {
			std::cout << showwords[subtype - Lt] << ' ' << std::flush;
			for (std::list<Term*>::iterator i = sons.begin(); i != sons.end(); i++)(*i)->print();
		}
	}
	else if (kind == Name) {
		std::cout << name << ' ' << std::flush;
	}
}

bool execute(Activation_Record* aclink, Term *t);

bool define_function(Activation_Record* aclink,Term *t)
{
	function new_fun;
	std::list<Term *>::iterator itor = t->sons.begin();
	new_fun.name = (*itor)->name;
	new_fun.access_link = aclink;
	itor++;
	while (itor!=t->sons.end())
	{
		if ((*itor)->kind ==Block)
		{
			new_fun.function_tree = *itor;
			break;
		}
		else if ((*itor)->kind!=Name)
		{
			std::cout << "Function Defination Error" << std::endl;
			return false;
		}
		parameter para;
		para.type = 0;
		para.name = (*itor)->name;
		new_fun.paras.push_back(para);
		itor++;
	}
	aclink->funs.push_back(new_fun);
	return true;
}

bool read_var(Activation_Record* aclink, std::string name, std::istream& input)
{
	int x;
	input >> x;
	while (aclink != NULL)
	{
		for (int i = 0; i < aclink->vars.size(); i++)
		{
			if (aclink->vars[i].name == name)
			{
				aclink->vars[i].value = x;
				return true;
			}
		}
		aclink = aclink->access_link;
	}
	return false;
}


bool get_varvalue(Activation_Record* aclink, std::string name, int &value)
{
	while (aclink != NULL)
	{
		for (int i = 0; i < aclink->vars.size(); i++)
		{
			if (aclink->vars[i].name == name)
			{
				value = aclink->vars[i].value;
				return true;
			}
		}
		aclink = aclink->access_link;
	}
	return false;
}



bool define_var(Activation_Record* aclink, std::string name)
{
	for (int i = 0; i < aclink->vars.size(); i++)
	{
		if (aclink->vars[i].name == name)
		{
			std::cout << "Duplicate Defination!" << std::endl;
			return false;
		}
	}
	variable newvar;
	newvar.name = name;
	newvar.value = UNDEFINED;
	aclink->vars.push_back(newvar);
	return true;
}

bool assign_var(Activation_Record* aclink, std::string name, int value)
{
	while (aclink != NULL)
	{
		for (int i = 0; i < aclink->vars.size(); i++)
		{
			if (aclink->vars[i].name == name)
			{
				aclink->vars[i].value = value;
				return true;
			}
		}
		aclink = aclink->access_link;
	}
	std::cout << "Var not found!" << std::endl;
	return false;
}


bool evaluate_expr(Activation_Record* aclink, Term* t, int &value)
{
	if (t->kind != Expr)
	{
		std::cout << "Not a expression!" << std::endl;
		return false;
	}
	if (t->subtype == Number)
	{
		value = t->number;
		return true;
	}
	if (t->subtype == VarName)
	{
		return get_varvalue(aclink, t->name, value);
	}
	if (t->subtype == Plus)
	{
		int lvalue, rvalue;
		std::list<Term*>::iterator i = t->sons.begin();
		if (evaluate_expr(aclink, *i, lvalue))
		{
			i++;
			if (i == t->sons.end())
			{
				std::cout << "Lack of right expression!" << std::endl;
				return false;
			}
			if (evaluate_expr(aclink, *i, rvalue))
			{
				value = lvalue + rvalue;
				return true;
			}
			else return false;
		}
		else return false;
	}

	if (t->subtype == Minus)
	{
		int lvalue, rvalue;
		std::list<Term*>::iterator i = t->sons.begin();
		if (evaluate_expr(aclink, *i, lvalue))
		{
			i++;
			if (i == t->sons.end())
			{
				std::cout << "Lack of right expression!" << std::endl;
				return false;
			}
			if (evaluate_expr(aclink, *i, rvalue))
			{
				value = lvalue - rvalue;
				return true;
			}
			else return false;
		}
		else return false;
	}

	if (t->subtype == Mult)
	{
		int lvalue, rvalue;
		std::list<Term*>::iterator i = t->sons.begin();
		if (evaluate_expr(aclink, *i, lvalue))
		{
			i++;
			if (i == t->sons.end())
			{
				std::cout << "Lack of right expression!" << std::endl;
				return false;
			}
			if (evaluate_expr(aclink, *i, rvalue))
			{
				value = lvalue * rvalue;
				return true;
			}
			else return false;
		}
		else return false;
	}

	if (t->subtype == Div)
	{
		int lvalue, rvalue;
		std::list<Term*>::iterator i = t->sons.begin();
		if (evaluate_expr(aclink, *i, lvalue))
		{
			i++;
			if (i == t->sons.end())
			{
				std::cout << "Lack of right expression!" << std::endl;
				return false;
			}
			if (evaluate_expr(aclink, *i, rvalue))
			{
				value = lvalue / rvalue;
				return true;
			}
			else return false;
		}
		else return false;
	}

	if (t->subtype == Mod)
	{
		int lvalue, rvalue;
		std::list<Term*>::iterator i = t->sons.begin();
		if (evaluate_expr(aclink, *i, lvalue))
		{
			i++;
			if (i == t->sons.end())
			{
				std::cout << "Lack of right expression!" << std::endl;
				return false;
			}
			if (evaluate_expr(aclink, *i, rvalue))
			{
				value = lvalue % rvalue;
				return true;
			}
			else return false;
		}
		else return false;
	}

	if (t->subtype == Apply)
	{
		Activation_Record* find_function = aclink;
		while (find_function != NULL)
		{
			for (int i = 0; i < find_function->funs.size(); i++)
			{
				std::list<Term*>::iterator itor = t->sons.begin();
				if (find_function->funs[i].name == (*itor)->name)
				{
					if (t->sons.size() - 1 != find_function->funs[i].paras.size())
					{
						std::cout << "Numbers of parameters do not match!" << std::endl;
						return false;
					}
					Activation_Record* fun_aclink = new Activation_Record();
					fun_aclink->access_link = find_function->funs[i].access_link;
					itor++;
					for (int j = 0; j < find_function->funs[i].paras.size(); j++)
					{
						std::string para_name = find_function->funs[i].paras[j].name;
						if (find_function->funs[i].paras[j].type == 0)
						{
							define_var(fun_aclink, para_name);
							int para_value;
							evaluate_expr(aclink, (*itor), para_value);
							assign_var(fun_aclink, para_name, para_value);
						}
						itor++;
					}
					execute(fun_aclink, find_function->funs[i].function_tree);
					value = fun_aclink->return_value;
					return true;
				}
			}
			find_function = find_function->access_link;
		}
	}

	return true;
}

bool check_bool(Activation_Record* aclink, Term *t)
{
	if (t->subtype == Lt || t->subtype == Gt || t->subtype == Eq)
	{
		int lvalue, rvalue;
		std::list<Term*>::iterator itor = t->sons.begin();
		evaluate_expr(aclink, *itor, lvalue);
		itor++;
		evaluate_expr(aclink, *itor, rvalue);
		switch (t->subtype)
		{
		case Lt:
			return lvalue < rvalue;
		case Gt:
			return lvalue > rvalue;
		case Eq:
			return lvalue == rvalue;
		default:
			break;
		}
	}
	else if (t->subtype == Negb)
	{
		std::list<Term*>::iterator itor = t->sons.begin();
		return !check_bool(aclink, *itor);
	}
	else
	{
		bool lbool, rbool;
		std::list<Term*>::iterator itor = t->sons.begin();
		lbool = check_bool(aclink, *itor);
		itor++;
		rbool = check_bool(aclink, *itor);
		switch (t->subtype)
		{
		case And:
			return lbool&&rbool;
		case Or:
			return lbool || rbool;
		default:
			break;
		}
	}
}


bool execute(Activation_Record* aclink, Term *t)
{
	if (aclink->is_returned == 1)return true;
	if (t->kind == Block)
	{
		std::list<Term *>::iterator itor = t->sons.begin();
		while (itor != t->sons.end())
		{
			if (execute(aclink, *itor) == false)
				return false;
			itor++;
		}
	}
	else if (t->kind == Function)
	{
		return define_function(aclink, t);
	}
	else if (t->kind == Command)
	{
		std::list<Term *>::iterator itor = t->sons.begin();
		std::string var_name = (*itor)->name;

		switch (t->subtype)
		{
		case Declaration :
			{
				if ((*itor)->kind != Name)
				{
					std::cout << "Declaration error!" << std::endl;
					return false;
				}
				define_var(aclink, var_name);
				break;
			}
		case Assign:
			{
				if ((*itor)->kind != Name)
				{
					std::cout << "Assignment error!" << std::endl;
					return false;
				}
				int value;
				itor++;
				if (evaluate_expr(aclink, *itor, value) == false)
				{
					return false;
				}
				assign_var(aclink, var_name, value);
				break;
			}
		case Read:
			{
				read_var(aclink, var_name, file_input);
				break;
			}
		case Print:
			{
				int value;
				if (evaluate_expr(aclink, *itor, value) == false)
				{
					return false;
				}
				file_output << value << " ";
				break;
			}
		case If:
			{
				if ((*itor)->kind != BoolExpr)
				{
					std::cout << "Not a bool expression!" << std::endl;
					return false;
				}
				if (check_bool(aclink, *itor))
				{
					itor++;
					Activation_Record* new_link = new Activation_Record();
					new_link->access_link = aclink;
					execute(new_link, *itor);
					if (new_link->is_returned == 1)
					{
						aclink->is_returned = 1;
						aclink->return_value = new_link->return_value;
					}
				}
				else
				{
					itor++;
					itor++;
					Activation_Record* new_link = new Activation_Record();
					new_link->access_link = aclink;
					execute(new_link, *itor);
					if (new_link->is_returned == 1)
					{
						aclink->is_returned = 1;
						aclink->return_value = new_link->return_value;
					}
				}
				break;
			}
		case While:
			{
				if ((*itor)->kind != BoolExpr)
				{
					std::cout << "Not a bool expression!" << std::endl;
					return false;
				}
				Activation_Record* new_link = new Activation_Record();
				new_link->access_link = aclink;
				std::list<Term *>::iterator itor_condition = itor;
				itor++;
				while (check_bool(aclink, *itor_condition))
				{
					execute(new_link, *itor);
				}
				if (new_link->is_returned == 1)
				{
					aclink->is_returned = 1;
					aclink->return_value = new_link->return_value;
				}
				break;
			}
		case Call:
			{
				for (int i = 0; i < aclink->funs.size(); i++)
				{
					std::list<Term*>::iterator itor = t->sons.begin();
					if (aclink->funs[i].name == (*itor)->name)
					{
						if (t->sons.size() - 1 != aclink->funs[i].paras.size())
						{
							std::cout << "Numbers of parameters do not match!" << std::endl;
							return false;
						}
						Activation_Record* fun_aclink = new Activation_Record();
						fun_aclink->access_link = aclink->funs[i].access_link;
						itor++;
						for (int j = 0; j < aclink->funs[i].paras.size(); j++)
						{
							std::string para_name = aclink->funs[i].paras[j].name;
							if (aclink->funs[i].paras[j].type == 0)
							{
								define_var(fun_aclink, para_name);
								int para_value;
								evaluate_expr(aclink, (*itor), para_value);
								assign_var(fun_aclink, para_name, para_value);
							}
							itor++;
						}
						execute(fun_aclink, aclink->funs[i].function_tree);
					}
				}
				break;
			}
		case Return:
			{
				int return_value;
				evaluate_expr(aclink, *itor, return_value);
				aclink->is_returned = 1;
				aclink->return_value = return_value;
				break;
			}
		default:
			break;
		}
	}
	return true;
}




#if DEBUG_MODE
using namespace std;
int main()
{
	ifstream input("innerFact.mini");
	Term *t = parse(input);
	std::cout << "Parsing Finished\n\n";
	input.close();
	Activation_Record* root_link=new Activation_Record();
	execute(root_link,t);
	file_input.close();
	file_output.close();
	//getchar();
}
#endif
#endif
