
#include "function.hpp"
#include <iostream>

Function::Function(string function_name) : Pi(M_PI), E(exp(1))
{
	if(function_name == "SIN") 				this->function = &Function::Sin;	
	else if(function_name == "COS")			this->function = &Function::Cos;
	else if(function_name == "GAUSSIAN")
	{	
		this->function = &Function::Gaussian;
		param.push_back(1/sqrt(2 * Pi));
		param.push_back(0.0);
		param.push_back(1.0);
		param.push_back(0.0);
	}
	else if(function_name == "ABS")			this->function = &Function::Abs;
	else if(function_name == "IDENTITY")	this->function = &Function::Identity;
	else if(function_name == "SIGMOID")
	{		
		this->function = &Function::Sigmoid;
		param.push_back(1.0);
	}
	else
	{
		cerr << "error at Function::Function(): function '" << function_name << "' was not defined!" << endl;
		cerr << "\t-> 'IDENTITY' was used instead" << endl;
		this->function = &Function::Identity;
	}

	this->function_name = function_name;
}

double Function::eval(double input)
{
	return (this->*function)(input);
}

string Function::get_name()
{
	return function_name;
}

// Functions
double Function::Sin(double input)
{
	return sin(input);
}

double Function::Cos(double input)
{
	return cos(input);
}

double Function::Identity(double input)
{
	return input;
}

double Function::Gaussian(double input)
{
	return param.at(0) * exp(-pow(input-param.at(1),2)/(2*pow(param.at(2),2))) + param.at(3);
}

double Function::Abs(double input)
{
	return abs(input);
}

double Function::Sigmoid(double input)
{
	return 1/(1+exp(-param.at(0) * input));
}