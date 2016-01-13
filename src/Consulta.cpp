#include "Consulta.h"
#include "Indice.h"
#include "includes.h"

int NUM_CONSULTA;
map<int, map<string, float> > matriz_peso;
map<string, Consultas> hash_consulta;
map<int, float> norma_colecao;
vector<int> ranking;
Indice *indice = new Indice();

Consulta::Consulta()
{
	cout << "Iniciando processo de consulta!!" << endl;
}

Consulta::~Consulta()
{
	cout << "Finalizando processo de consulta!!" << endl;
}

void Consulta::GeraIndiceInvertido()
{

	string files[6] = {
			"../cfc/cf74",
			"../cfc/cf75",
			"../cfc/cf76",
			"../cfc/cf77",
			"../cfc/cf78",
			"../cfc/cf79" };

	indice->ReadColection(files[0]);
	indice->ReadColection(files[1]);
	indice->ReadColection(files[2]);
	indice->ReadColection(files[3]);
	indice->ReadColection(files[4]);
	indice->ReadColection(files[5]);

	indice->imprimir_hash();

	delete indice;

}

void calcular_peso_consulta() {
	map<string, Consultas>::iterator it;
	map<string, Vocabulo>::iterator it_hash;
	Vocabulo voc;
	string termo;

	for (it = hash_consulta.begin(); it != hash_consulta.end(); it++) {
		//cout << (*it).first << " = > tf: " << (*it).second.tf << endl;

		termo = (*it).first;

		//Caso não exista a palavra no hash o peso é zero
		if (indice_invertido.find(termo) == indice_invertido.end()) {
			hash_consulta[termo].peso = 0;
		} else {
			it_hash = indice_invertido.find(termo);
			voc = (*it_hash).second;
			//cout << "idf " << voc.idf << endl;
			(*it).second.peso = voc.idf * (*it).second.tf;
			hash_consulta[termo].peso = (*it).second.peso;
		}

	}
}

void imprimirRelevantes(vector<int> relevantes) {
	vector<int>::iterator i;
	cout << "relevantes [" << relevantes.size() << "] ";
	for (i = relevantes.begin(); i != relevantes.end(); i++) {
		cout << (*i) << " ";
	}
	cout << endl;

}

//Ordena vetor de ranking do mais similar ao menos similar e imprime na tela
void imprimirRanking(map<float, int> sim_total) {
	map<float, int>::iterator it_sim;
	map<float, int>::reverse_iterator rit_sim;
	vector<int>::iterator it;

	for (rit_sim = sim_total.rbegin(); rit_sim != sim_total.rend(); ++rit_sim) {

		//cout << "sim [" << rit_sim->second << "] =" << rit_sim->first;
		ranking.push_back(rit_sim->second);
	}

	cout << "Consulta " << "[" << ranking.size() << "] ";

	//imprime documentos do ranking na tela
	for (it = ranking.begin(); it != ranking.end(); it++) {
		cout << (*it) << " ";
	}
	cout << endl;

}

void calcularSimilaridadeTotal(map<int, float> acum) {
	map<int, float>::iterator it_acum; //_colecao
	map<int, float>::iterator it_norma;
	int num_doc;
	map<float, int> sim_total;

	for (it_acum = acum.begin(); it_acum != acum.end(); it_acum++) {
		num_doc = it_acum->first;
		it_norma = norma_colecao.find(num_doc);

		if (((*it_acum).second != 0) || ((*it_norma).second != 0))
			//sim_total[num_doc] = (*it_acum).second / (*it_norma).second;
			sim_total[(*it_acum).second / (*it_norma).second] = num_doc;
		else {
			//sim_total[num_doc] = 0.0;
			sim_total[0.0] = num_doc;
		}

	}

	imprimirRanking(sim_total);

}


void calcularSimilaridade() {
	map<string, Consultas>::iterator it_cons;		//consulta
	map<string, Vocabulo>::iterator it_hash;		//hash
	map<int, CelulaLista>::iterator it_lista;		//lista invertida
	map<int, float> acum;		//acumulador
	float sim_parcial; //similaridade
	Vocabulo vocabulo;
	int num_doc;

	string termo_cons; //termo consulta

	for (it_cons = hash_consulta.begin(); it_cons != hash_consulta.end();
			it_cons++) {
		termo_cons = (*it_cons).first;

		//Caso exista a palavra no hash
		if (indice_invertido.find(termo_cons) != indice_invertido.end()) {
			it_hash = indice_invertido.find(termo_cons);
			vocabulo = (*it_hash).second;
			//cout << "\n" <<(*it_hash).first <<endl;
			for (it_lista = vocabulo.lista_invertida.begin();
					it_lista != vocabulo.lista_invertida.end(); ++it_lista) {
				num_doc = (*it_lista).first;

				sim_parcial = (*it_lista).second.peso * (*it_cons).second.peso; //peso do doc/termo * peso termo/consulta

				if (acum.find(num_doc) == acum.end()) {
					acum[num_doc] = sim_parcial;
				} else {
					acum[num_doc] += sim_parcial;
				}

			}
		}

	}
	calcularSimilaridadeTotal(acum);
}

//FUNCOES PRECISAO E REVOCACAO
int busca_binaria(int qtd, vector<int> relevantes, int elemento) {
	int inicio = 0, fim = qtd - 1, meio;
	while (inicio <= fim) {
		meio = (inicio + fim) / 2;
		if (elemento == relevantes[meio])
			return meio;
		else if (elemento < relevantes[meio])
			fim = meio - 1;
		else
			inicio = meio + 1;
	}
	return -1;
}

void calcularMetricas(vector<int> relevantes) {
	vector<int>::iterator it;
	vector<double>::iterator j;
	int indice, doc_atual, k;
	vector<int> docs_encontrados;
	vector<double> revocacao, precisao;

	//Quantidade de acertos
	double acerto = 0, p = 0;
	for (it = ranking.begin(); it != ranking.end(); it++) {
		doc_atual = *it;
		//indica posicao no relevantes[]
		indice = busca_binaria(relevantes.size(), relevantes, doc_atual);

		if (indice != -1) {
			docs_encontrados.push_back(doc_atual);
			revocacao.push_back(((acerto + 1) / relevantes.size()) * 100);
			precisao.push_back(((acerto + 1) / (p + 1)) * 100);
			acerto++;
		}
		p++;
	}

	cout << "Documentos relevantes encontrados [" << docs_encontrados.size() << "] ";

	for (it = docs_encontrados.begin(); it != docs_encontrados.end(); it++) {
		cout << (*it) << " ";
	}
	cout << endl;
	cout << endl;

	//Interpolação
	float media_precisao[11], PR_interpolada[11], maximo;
	int flag, i;

	// procura os 11 pontos de interpolacao da precisao
	for (i = 0; i < 11; i++)
		PR_interpolada[i] = 999;
		indice = acerto - 1;
	//ultimo ponto de revocacao
	if (revocacao[indice] == 100) {
		PR_interpolada[10] = precisao[indice];
		indice--;
	} else
		PR_interpolada[10] = 0;
	for (i = 9; i >= 0; i--) {
		maximo = 999;
		while (i * 10 <= revocacao[indice] && indice >= 0) {
			if (maximo == 999)
				maximo = precisao[indice];
			else if (maximo < precisao[indice])
				maximo = precisao[indice];
			indice--;
		}
		PR_interpolada[i] = maximo;
	}
	maximo = PR_interpolada[10];
	for (i = 9; i >= 0; i--)
		if (PR_interpolada[i] == 999)
			PR_interpolada[i] = maximo;
		else
			maximo = PR_interpolada[i];
	for (i = 0; i < 11; i++){
		if (PR_interpolada[i] < PR_interpolada[i+1])
		{
			PR_interpolada[i] = PR_interpolada[i+1];
		}
	}

	for(i=0;i<11;i++){
		if (PR_interpolada[i] < PR_interpolada[i+1]){
			PR_interpolada[i] = PR_interpolada[i+1];
		}
	      cout << "Revocacao[" << i*10 << "] Precisao[" << PR_interpolada[i] << "]" << endl;
	}

	float map = 0;
	for (int i = 0; i < 11; ++i)
	{
		map+= PR_interpolada[i];
	}

	cout << "MAP interpolado: " << map/11 << "\%" << endl;

	float count_relevante = 0;
	for (int i = 0; i < 10; ++i)
	{
		if(busca_binaria(relevantes.size(), relevantes, ranking[i]) != -1){
			count_relevante++;
		}
	}

	cout << "P@10: " << (count_relevante/10)*100 << "\%" << endl << endl;
}

void calculosConsulta(vector<string> termos_consulta, vector<int> relevantes,
	int qtd_relev) {
	Consultas consulta;
	Consultas consulta_aux;
	vector<string>::iterator it;
	map<string, Consultas>::iterator it_cons;
	string termo;
	//float norma;

	for (it = termos_consulta.begin(); it != termos_consulta.end(); it++) {
		termo = *it;

		if (hash_consulta.find(termo) == hash_consulta.end()) { // nao existe
			consulta.tf = 1;
			consulta.num_consulta = NUM_CONSULTA;
			consulta.qtd_relevantes = qtd_relev;
			consulta.relevantes = relevantes;
			hash_consulta[termo] = consulta;
		} else {
			it_cons = hash_consulta.find(termo);
			consulta = (*it_cons).second;
			consulta.tf++;
			hash_consulta[termo] = consulta;
		}

	}

	cout << "Consulta " << NUM_CONSULTA << endl;

	calcular_peso_consulta();

	imprimirRelevantes(relevantes);

	calcularSimilaridade();

	calcularMetricas(relevantes);

	ranking.clear();
	hash_consulta.clear();
}


void Consulta::processaConsulta() {
	string line;
	string arq_query =
			"../cfc/cfquery";
	ifstream arq(arq_query.c_str());
	int qtd_relev = 0;

	vector<string> consulta, consulta_clean;
	vector<int> relevantes;

	if (!arq.is_open()) {
		cout << "erro ao abrir arquivo " << arq << endl;
	}

	for (int i = 0; i < 100; i++) {
		if (line.substr(0, 2).compare("QN") != 0) {
			getline(arq, line);
		}
		NUM_CONSULTA = atoi(line.substr(3, line.length()).c_str()); //numero da consulta

		getline(arq, line);
		while (line.substr(0, 2).compare("NR") != 0) {
			string buffer = indice->GetLineContent(line);
			istringstream iss(buffer);
			copy(istream_iterator<string>(iss), istream_iterator<string>(),
					back_inserter<vector<string> >(consulta));
			getline(arq, line);
		}

		qtd_relev = atoi(line.substr(3, line.length()).c_str());

		getline(arq, line);

		bool flag; //flag para manipular documentos relevantes
		line = line.substr(2, line.length());
		while ((line.substr(0, 2) != "QN") && (!arq.eof())) {
			flag = true;
			stringstream ss;
			ss << line;
			while (ss >> line) {
				if (flag) {
					relevantes.push_back(atoi(line.c_str()));
					flag = false;
				} else {
					flag = true;
				}
			}

			getline(arq, line);
		}

		consulta_clean = indice->CleanDocument(consulta);
		calculosConsulta(consulta_clean, relevantes, qtd_relev);
		consulta.clear();
		consulta_clean.clear();
		relevantes.clear();
	}

}

void Consulta::calcular_peso_colecao() {
	map<string, Vocabulo>::iterator iti; //termo
	map<int, CelulaLista>::iterator it_cel; //doc - lista invertida
	map<string, float> vetor_termo;
	map<int, map<string, float> >::iterator find_mat; //termo

	//int aux=0;
	for (iti = indice_invertido.begin(); iti != indice_invertido.end(); iti++) {
		for (it_cel = iti->second.lista_invertida.begin();
				it_cel != iti->second.lista_invertida.end(); it_cel++) {

			(*iti).second.idf = log2(
					NUM_TOTAL_COLECAO / (*iti).second.total_docs);
			(*it_cel).second.peso = (*iti).second.idf * (*it_cel).second.tf;

			indice_invertido[(*iti).first].idf = (*iti).second.idf;

			//matriz de pesos (map de map) [documento][termo] = peso;
			//if(((*it_cel).first).compare("00167 ")==0){
			if (matriz_peso.find((*it_cel).first) == matriz_peso.end()) {
				vetor_termo.clear();
				vetor_termo[(*iti).first] = (*iti).second.idf
						* (*it_cel).second.tf; //doc com seu peso
				matriz_peso[(*it_cel).first] = vetor_termo;
			} else {
				find_mat = matriz_peso.find(NUM_DOC);
				vetor_termo = (*find_mat).second;
				vetor_termo[(*iti).first] = (*iti).second.idf
						* (*it_cel).second.tf; //doc com seu peso
				matriz_peso[(*it_cel).first] = vetor_termo;
			}

		}

	}
}

void Consulta::calcular_norma_colecao() {
	map<int, map<string, float> >::iterator i; //documento
	map<string, float>::iterator j; //termo
	float acum = 0;
	int aux = 0;
	for (i = matriz_peso.begin(); i != matriz_peso.end(); i++) {
		acum = 0;
		for (j = (*i).second.begin(); j != (*i).second.end(); j++) {
			acum += powf((*j).second, 2); //elevar ao quadrado e acumular
		}
		norma_colecao[(*i).first] = sqrt(acum);
	}

}