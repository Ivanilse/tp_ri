#include <iostream>
#include "Indice.h"
#include "Consulta.h"
#include "includes.h"

int main(int argc, char const *argv[])
{

	Consulta *consulta = new Consulta();
	consulta->GeraIndiceInvertido();

	consulta->calcular_peso_colecao();

	consulta->calcular_norma_colecao();

	consulta->processaConsulta();
		
	delete consulta;

	return 0;
}