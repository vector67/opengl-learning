#ifndef GENETIC_ENCODING_CPP
#define GENETIC_ENCODING_CPP

#include "genetic_encoding.hpp"
#include <stdio.h>
#include <cstring>

using namespace ANN_USM;

void connection_gene::c_g(int innovation, int in, int out, double weight, bool enable)
{
	this->innovation=innovation;
	this->in=in;
	this->out=out;
	this->weight=weight;
	this->enable=enable;
	this->exist=true;
}
void connection_gene::c_g(bool exist)
{
	this->exist=exist;
}

void node_gene::n_g(int node, gene_type type, string function)
{
	this->node = node;
	this->type = type;
	this->exist = true;
	this->function = new Function(function);
}

void node_gene::n_g(bool exist)
{
	this->exist = exist;
}

void Genetic_Encoding::add_connection(int innovation, int in, int out, double weight)
{
	int list_size(Lconnection_genes.size());

	if(innovation >= list_size)
	{
		connection_gene missing_connection_gene;
		connection_gene new_connection;

		missing_connection_gene.c_g(false); // conection that not exist in this genoma
		new_connection.c_g(innovation,in,out,weight,true);

		for (int i = 0; i < innovation-list_size; ++i)
		{
			Lconnection_genes.push_back(missing_connection_gene);
		}
		Lconnection_genes.push_back(new_connection);
	}

	else 
	{
		if(Lconnection_genes[innovation].exist)
			cerr << "ERROR::In function add_connection, you wanted to add a connection gene with an innovation that already exists" << endl;	
		else
			Lconnection_genes[innovation].c_g(innovation,in,out,weight,true);
	}

	Lnode_genes.at(out).increase_incoming_connection();
}



void Genetic_Encoding::add_connection(connection_gene conn)
{
	int list_size(Lconnection_genes.size());

	if(conn.innovation >= list_size)
	{
		connection_gene missing_connection_gene;
		connection_gene new_connection;

		missing_connection_gene.c_g(false); // conection that not exist in this genoma
		new_connection.c_g(conn.innovation,conn.in,conn.out,conn.weight, conn.enable);

		for (int i = 0; i < conn.innovation-list_size; ++i)
		{
			Lconnection_genes.push_back(missing_connection_gene);
		}
		Lconnection_genes.push_back(new_connection);
	}
	else 
	{
		if(Lconnection_genes[conn.innovation].exist)
			cerr << "ERROR::In function add_connection, you wanted to add a connection gene with an innovation that already exists" << endl;	
		else
			Lconnection_genes[conn.innovation].c_g(conn.innovation,conn.in,conn.out,conn.weight,conn.enable);
	}

	Lnode_genes.at(conn.out).increase_incoming_connection();
}

void Genetic_Encoding::add_node(int node, int row , gene_type type, int function)
{
	int list_size(Lnode_genes.size());

	string function_str;
	switch(function)
	{
		case 0: function_str = "IDENTITY"; 	break;
		case 1: function_str = "SIGMOID"; 	break;
		case 2: function_str = "GAUSSIAN"; 	break;
		case 3: function_str = "ABS"; 		break;
		case 4: function_str = "SIN"; 		break;
		case 5: function_str = "COS"; 		break;
	}

	if(node >= list_size)
	{
		node_gene missing_node_gene;
		node_gene new_node;

		missing_node_gene.n_g(false);
		new_node.n_g(node, type, function_str);
		new_node.row=row;
		for (int i = 0; i < node - list_size; ++i)
		{
			Lnode_genes.push_back(missing_node_gene);
		}
		Lnode_genes.push_back(new_node);
	}
	else
	{
		if(Lnode_genes[node].exist)
			cerr << "ERROR::In function add_node , you wanted to add a node gene with a node number that already exists" << endl;	
		else
		{
			Lnode_genes[node].n_g(node, type, function_str);
			Lnode_genes[node].row = row;
		}
	}

	if(type == INPUT)
	{
		this->Lnode_genes.at(node).increase_incoming_connection();
		this->input_nodes.push_back(node);
	}
	else if(type == OUTPUT) this->output_nodes.push_back(node);
}

void Genetic_Encoding::add_node(node_gene node)
{
	int list_size(Lnode_genes.size());

	if(node.node >= list_size)
	{
		node_gene missing_node_gene;
		node_gene new_node;

		missing_node_gene.n_g(false);
		new_node.n_g(node.node, node.type, node.function->get_name());

		for (int i = 0; i < node.node - list_size; ++i)
		{
			Lnode_genes.push_back(missing_node_gene);
		}
		new_node.row = node.row;
		Lnode_genes.push_back(new_node);
	}
	else
	{
		if(Lnode_genes[node.node].exist)
			cerr << "ERROR::In function add_node , you wanted to add a node gene with a node number that already exists" << endl;	
		else
		{
			Lnode_genes[node.node].n_g(node.node, node.type, node.function->get_name());
			Lnode_genes[node.node].row = node.row;
		}
	}

	if(node.type == INPUT)
	{
		this->Lnode_genes.at(node.node).increase_incoming_connection();
		this->input_nodes.push_back(node.node);
	}
	else if(node.type == OUTPUT) this->output_nodes.push_back(node.node);
}

ostream & operator<<(ostream & o, Genetic_Encoding & encoding) 
{
	o << encoding.JSON();	
	return o;
}

string Genetic_Encoding::JSON()
{
	stringstream o;
	o << "{\n\t\"Genetic_Encoding\":\n\t{\n\t\t\"nodes\":\n\t\t[\n";

	int node_size(Lnode_genes.size());
	int connection_size(Lconnection_genes.size());

	for (int i = 0; i < node_size; ++i)
	{
		if(Lnode_genes[i].exist)
			o << "\t\t\t{\"exist\": " << Lnode_genes[i].exist  << ",\"node\": " <<Lnode_genes[i].node << ",\"type\": " << Lnode_genes[i].type << ", \"row\": " << Lnode_genes[i].row << ", \"function\": " << Lnode_genes[i].function->get_name();
		else
			o << "\t\t\t{\"exist\": " << Lnode_genes[i].exist ;
		

		if(i<node_size-1)
			o <<  "},\n";
		else
			o <<  "}\n";
		
	}

	o << "\t\t],\n\t\t\"connection_genes\":\n\t\t[\n";
	for (int i = 0; i < connection_size; ++i)
	{
		if(Lconnection_genes[i].exist)
			o << "\t\t\t{\"exist\": " << Lconnection_genes[i].exist << ",\"innovation\": " << Lconnection_genes[i].innovation << ",\"in\": " << Lconnection_genes[i].in << ",\"out\": " << Lconnection_genes[i].out << ",\"weight\": " << Lconnection_genes[i].weight << ",\"enable\": " << Lconnection_genes[i].enable;
		else
			o << "\t\t\t{\"exist\": " << Lconnection_genes[i].exist;
		
		if(i<connection_size-1)
			o <<  "},\n";
		else
			o <<  "}\n";
	}
	
	o << "\t\t]\n\t}\n}";
	return o.str();
}


void Genetic_Encoding::save(char path[])
{
	ofstream file;
	file.open (path);
	file << JSON();
	file.close();
}

void Genetic_Encoding::load(char path[])
{
	node_gene Nnew_node;
	connection_gene Cnew_node;

	Lconnection_genes.clear(); 
	Lnode_genes.clear();

	ifstream file (path);
	file.seekg (0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);
	char buffer[length]; // In JSON format
	file.read (buffer,length);
	file.close();
	
	int node;
	int type;
	int innovation;
	int in;
	int out;
	int enable;
	int contador(0);
	int row;

	char * pch;
	char delimiters[] = " \n\":\t{},[";

	bool connection(false);
	bool exist;

	double weight;

	//Function * function;

	pch = strtok (buffer,delimiters);
	
	do
	{
		pch = strtok (NULL, delimiters);
		if(!(pch[0] == ']')){
			if(connection){
				if(!strncmp(pch, "exist",5)){
					pch = strtok (NULL, delimiters);
					exist = atoi(pch);
					if(exist){
						pch = strtok (NULL, delimiters);
						pch = strtok (NULL, delimiters);
						innovation = atoi(pch);
						pch = strtok (NULL, delimiters);
						pch = strtok (NULL, delimiters);
						in = atoi(pch);
						pch = strtok (NULL, delimiters);
						pch = strtok (NULL, delimiters);
						out = atoi(pch);
						pch = strtok (NULL, delimiters);
						pch = strtok (NULL, delimiters);
						weight = (double)atof(pch);
						pch = strtok (NULL, delimiters);
						pch = strtok (NULL, delimiters);
						enable = atoi(pch);
						Cnew_node.c_g(innovation, in, out, weight, (bool)enable);
						this->add_connection(Cnew_node);
					}
					else{
						Cnew_node.c_g(false);
						Lconnection_genes.push_back(Cnew_node);
					}
				} 
			}
			else{
				if(!strncmp(pch, "exist",5)){
					pch = strtok (NULL, delimiters);
					exist = atoi(pch);
					if(exist){
						pch = strtok (NULL, delimiters);
						pch = strtok (NULL, delimiters);
						node = atoi(pch);
						pch = strtok (NULL, delimiters);
						pch = strtok (NULL, delimiters);
						type = atoi(pch);
						pch = strtok (NULL, delimiters);
						pch = strtok (NULL, delimiters);
						row = atoi(pch);
						Nnew_node.row=row;
						pch = strtok (NULL, delimiters);
						pch = strtok (NULL, delimiters);
						Nnew_node.n_g(node, (gene_type) type, string(pch));
						this->add_node(Nnew_node);
					}
					else{
						Nnew_node.n_g(false);
						Lnode_genes.push_back(Nnew_node);
					}
				} 
			}
		}
		else
		{
			contador=contador+1;
			connection = true;
			if(contador==2)
			{
				break;
			}
		}
	}
	while (pch != NULL);
}

//===================================================================

node_gene::node_gene()
{
	this->incoming_connections = 0;
	this->accumulative_result = 0;
	this->counter = 0;
	this->final_result = 0;
}

double node_gene::get_final_result()
{
	return this->final_result;
}

bool node_gene::is_ready()
{
	return this->incoming_connections == this->counter;
}

void node_gene::increase_incoming_connection()
{
	this->incoming_connections++;
}

void node_gene::decrease_incoming_connection()
{
	this->incoming_connections--;
}

vector <double> Genetic_Encoding::eval(std::vector<double> input_values)
{
	vector<double> outputs;

	//cout << "Genetic_Encoding::eval" << endl;

	if(input_values.size() != this->input_nodes.size())
	{
		cerr << "error in function 'Genome::eval'. Number of input values differ from number of input nodes." << endl;
	}
	else
	{
		// Spread the initial values through the network
		for (int i = 0; i < (int)this->input_nodes.size(); i++)
			this->spread_final_result(this->input_nodes.at(i), input_values.at(i));

		// Recollect all the final results from the output nodes
		for (int i = 0; i < (int)this->output_nodes.size(); i++)
			outputs.push_back(this->Lnode_genes.at(this->output_nodes.at(i)).get_final_result());			
	}

	//cout << "\toutput.size(): " << outputs.size() << endl;
	//cout << "\toutput: " << outputs.at(0) << endl;

	return outputs;
}

vector<connection_gene> Genetic_Encoding::get_outgoing_connections(int node)
{
	vector<connection_gene> outgoing_connections;

	for (int i = 0; i < (int)this->Lconnection_genes.size(); i++)
	{
		if(	this->Lconnection_genes.at(i).exist && \
			this->Lconnection_genes.at(i).enable && \
			this->Lconnection_genes.at(i).in == node)
			outgoing_connections.push_back(this->Lconnection_genes.at(i));
	}
	
	return outgoing_connections;
}

void Genetic_Encoding::spread_final_result(int node, double value)
{
	//================================
	//cout << "\tnode: " << node << " | value: " << value << " | incoming_connections: " << Lnode_genes.at(node).incoming_connections;

	// Evaluates the node
	this->Lnode_genes.at(node).eval(value);

	if(this->Lnode_genes.at(node).is_ready())
	{
		this->Lnode_genes.at(node).counter = 0;

		value = this->Lnode_genes.at(node).get_final_result();

		vector<connection_gene> outgoing_connections = this->get_outgoing_connections(node);

		for (int i = 0; i < (int)outgoing_connections.size(); i++)
		{
			this->spread_final_result(outgoing_connections.at(i).out, value * outgoing_connections.at(i).weight);
		}
	}
}

void node_gene::eval(double value)
{
	//cout << " | accumulative_result(b): " << this->accumulative_result;

	this->accumulative_result += value;
	this->counter++;

	//cout << " | accumulative_result(a): " << this->accumulative_result << " | counter: " << counter << endl;
	
	if(this->is_ready())
	{
		this->final_result = this->function->eval(this->accumulative_result);
		this->accumulative_result = 0;
	}
}

#endif