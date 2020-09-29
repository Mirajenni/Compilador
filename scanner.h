#ifndef SCANNER
#define SCANNER

#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <ctype.h>
#include <algorithm>

using namespace std;


struct Scanner {
	int tipo;
	int linha;
	int coluna;
	char lexema[20];
};

enum tipos {PALAVRA_RESERVADA = 1, ID, VALOR_FLOAT, VALOR_INT, VALOR_CHAR, ABRE_CHAVES, FECHA_CHAVES, ABRE_PARENTESES, FECHA_PARENTESES, PONTO_E_VIRGULA, VIRGULA, MENOR_OU_IGUAL, MENOR_QUE, MAIOR_OU_IGUAL, MAIOR_QUE, IGUAL_IGUAL, IGUAL, DIFERENTE, SOMA, SUBTRACAO, VEZES, DIVISAO, FIM, ERRO};


void inicializarScanner(Scanner *token);
Scanner* scanner(FILE *arquivo);
void inserirToken(Scanner *token, int linha, int coluna, char lexema[20], int tipo);
#endif