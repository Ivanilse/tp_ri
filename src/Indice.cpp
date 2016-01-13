#include "Indice.h"
#include "includes.h"

int NUM_DOC;
//flag que indica qual o bloco de linhas devem ser lidas para serem indexados os termos
bool flag_linha = false;
float NUM_TOTAL_COLECAO = 1239;

// // Celulas da lista invertida
// struct CelulaLista {
// 	//string num_doc; - eh a chave do map
// 	int tf;
// 	double peso;
// };

// //Termos do vocabulário (hash)
// struct Vocabulo{
// 	//string termo; //-- eh a chave do hash
// 	double idf;
// 	int total_docs;
// 	map<int, CelulaLista> lista_invertida;
// };

map<string, Vocabulo> indice_invertido;

Indice::Indice()
{
	cout << "Começando a criação do índice invertido!!" << endl;
}

Indice::~Indice()
{
	cout << "Finalizando índice invertido!!" << endl;
}

bool CanReadLine(string line) {
	//verifica se os  dois primeiros caracteres da linha sao aqueles onde devemos capturar os termos
	string tag;
	tag = line.substr(0, 2); //retorna os dois primeiros caracteres da linha

	if (tag == "AU" || tag == "TI" || tag == "AB" || tag == "EX" ||
		tag == "SO" || tag == "MJ" || tag == "MN") {
		flag_linha = true;
		return true;
	} else if (tag == "RN") {
		//Pega apenas o conteudo da linha
		NUM_DOC = atoi((line.substr(3, line.length()-1)).c_str());
	} else if (!(line[0] == ' ')) {
		flag_linha = false;
		return false;
	}
	return false;
}

string Indice::GetLineContent(string line) {
	// Retorna o conteúdo da linha
	return line.substr(3, line.length());
}


bool IsCaracter(char c)
{
/*Essa função verifica a existência digitios e caracteres especiais*/

	if (isdigit(c))	{
		return true;
	}

	switch (c){
	case '.':
	case '"':
	case ',':
	case '\'':
	case '(':
	case ')':
	case '[':
	case ']':
	case ':':
	case '%':
	case '+':
	case '<':
	case '>':
	case '=':
	case '&':
	case '?':
	case '/':
	case '-':
	case '$':
	case '#':
	case '\\':
	case ' ':
	case ';':
		return true;
	default:
		return false;
	}
}

vector<string> Indice::CleanDocument(vector<string> terms)
{
/*Essa função remove todos os termos que são stopwords, dígitos e caracteres especiais*/
	vector<string>::iterator it;
	vector<string> terms_clean;

	ifstream stopword_file;
	stopword_file.open("../stopwords.txt");
	unordered_map<string, string> stopwords;
	string line;

	while(getline(stopword_file, line)){
		line = line.substr(0, line.length() - 1); //cada linha corresponde a uma stopword (0 -1 é pra retirar o \n)
		stopwords[line];
	}

	for (it = terms.begin(); it != terms.end(); it++) {
		it->erase(remove_if(it->begin(), it->end(), &IsCaracter), it->end()); //remove caracteres especiais e digitos
		transform(it->begin(), it->end(), it->begin(), ::tolower); //transforma em minusculo

		if (!(*it).empty()) {
			if (stopwords.find(*it) == stopwords.end()){//insere no vector caso não seja uma stopword
				terms_clean.push_back(*it);
			}
		}
	}

	 // for (int i = 0; i < terms_clean.size(); ++i) {
	 // 	cout << terms_clean[i] << endl;
	 // }

	return terms_clean;
}

void indexaDocumento(vector<string> termos) {
	vector<string>::iterator it;
	string palavra;
	Vocabulo voc;
	CelulaLista celula;
	Vocabulo voc_aux;
	CelulaLista cel_aux;
	map<string, Vocabulo>::iterator it_hash;
	map<int, CelulaLista>::iterator it_cel;

	for (it = termos.begin(); it != termos.end(); it++) {
		voc.total_docs = 0;
		voc_aux.total_docs = 0;
		celula.tf = 0;
		cel_aux.tf = 0;
		voc.idf = 0;
		voc_aux.idf = 0;
		celula.peso = 0;
		cel_aux.peso = 0;

		palavra = *it;
		//se não existe a palavra no hash
		//contabilizar o numero de docs
		//criar elemento lista invertida
		//tf=1
		if (indice_invertido.find(palavra) == indice_invertido.end()) {
			voc.total_docs = 1;
			//voc.idf = log2(NUM_TOTAL_COLECAO / voc.total_docs);
			celula.tf = 1;
			//celula.peso = voc.idf * celula.tf;
			voc.lista_invertida[NUM_DOC] = celula;
			indice_invertido[palavra] = voc;
			//se existe
		} else {

			it_hash = indice_invertido.find(palavra);
			voc_aux = (*it_hash).second;
			//Se nao existe o documento na lista invertida
			//Contabiliza o numero de documentos que o termo aparece
			//e cria uma nova celula na lista invertida
			if (voc_aux.lista_invertida.find(NUM_DOC)
					== voc_aux.lista_invertida.end()) {
				voc_aux.total_docs++;
				celula.tf = 1;
				voc_aux.lista_invertida[NUM_DOC] = celula;
				//Se já existe o documento
			} else {
				//voc_aux.total_docs = (*it_hash).second.total_docs;
				it_cel = voc_aux.lista_invertida.find(NUM_DOC);
				cel_aux = (*it_cel).second;
				cel_aux.tf++;
				voc_aux.lista_invertida[NUM_DOC] = cel_aux;
			}

			indice_invertido[palavra] = voc_aux;
		}
	}

}
void Indice::imprimir_hash() {

	map<string, Vocabulo>::iterator iti;
	map<int, CelulaLista>::iterator it_cel;
	float acum = 0;
	int aux = 0;
	char linha[255];

	ofstream file("hash.txt");
	for (iti = indice_invertido.begin(); iti != indice_invertido.end(); ++iti) {

		sprintf(linha, "%s ->  totaldocs:%d -- idf: %f\n", iti->first.c_str(),
				iti->second.total_docs, (*iti).second.idf);
		file << linha;

		for (it_cel = iti->second.lista_invertida.begin();
				it_cel != iti->second.lista_invertida.end(); ++it_cel) {

			string palavra = (*iti).first;
			sprintf(linha, "(%d, %d) peso: %f \n", it_cel->first,
					it_cel->second.tf, it_cel->second.peso);

			file << linha;
		}
	}
	file.close();

}

void Indice::ReadColection(string file_name) {

	vector<string> termos, termos_clean;
	string line, buffer;

	ifstream arq(file_name.c_str());

	if (!arq.is_open()) {
		cout << "erro ao abrir arquivo " << file_name << endl;
		//return NULL;
	}

	//Faz a leitura de cada linha do arquivo
	while (getline(arq, line)) {
		//cout << "abriuuuuu o/" << endl;
		if ((line.empty()) && (termos.size() > 0)) {
			termos_clean = CleanDocument(termos);
			indexaDocumento(termos_clean);
			termos.clear();
			termos_clean.clear();
		}
		//checa se a linha deve ser lida
		if (CanReadLine(line) || flag_linha) {
			buffer = GetLineContent(line);
			//cout << buffer << endl;
		 	istringstream iss(buffer);
		 	copy(istream_iterator<string>(iss), istream_iterator<string>(),
		 		back_inserter<vector<string> >(termos));
		}
	}

	map<string, Vocabulo>::iterator it_hash;
	map<int, CelulaLista>::iterator it_cel;

	//computa o idf de cada lista invertida e o peso de cada termo na lista invertida
	for (it_hash = indice_invertido.begin(); it_hash != indice_invertido.end(); ++it_hash){
		(*it_hash).second.idf = log2(NUM_TOTAL_COLECAO/(*it_hash).second.total_docs);

		for (it_cel = it_hash->second.lista_invertida.begin(); it_cel != it_hash->second.lista_invertida.end(); ++it_cel){
			(*it_cel).second.peso = (*it_hash).second.idf * (*it_cel).second.tf;
		}
	}
}
