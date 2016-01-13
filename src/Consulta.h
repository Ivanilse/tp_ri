#ifndef CONSULTA_H
#define CONSULTA_H

#include "includes.h"

//consulta
struct Consultas{
	//termo é o índice desse vocabulario
	string num_consulta;
	float tf;
	float peso;
	float norma;
	vector<int> relevantes;
	int qtd_relevantes;
};

struct Sim{
	float sim_parcial;
	float norma_doc;
};

class Consulta
{
public:
	Consulta(); //construtor da classe
	~Consulta(); //destrutor da classe
	void GeraIndiceInvertido();	
	void calcular_peso_colecao();
	void calcular_norma_colecao();
	void processaConsulta();
private:
	
};

#endif