#ifndef INDICE_H
#define INDICE_H

#include "includes.h"

struct CelulaLista {
	//string num_doc; - eh a chave do map
	int tf;
	double peso;
};

//Termos do vocabulário (hash)
struct Vocabulo{
	//string termo; //-- eh a chave do hash
	double idf;
	int total_docs;
	map<int, CelulaLista> lista_invertida;
};

extern struct CelulaLista celulaLista;
extern struct Vocabulo vocabulo;
extern map<string, Vocabulo> indice_invertido;
extern int NUM_DOC;
extern float NUM_TOTAL_COLECAO;

class Indice
{
public:
	Indice(); //construtor da classe
	~Indice(); //destrutor da classe
	void ReadColection(string file);
	void imprimir_hash();
	vector<string> CleanDocument(vector<string> terms);
	string GetLineContent(string line);

private:
	
};

#endif
