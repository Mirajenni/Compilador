#include "scanner.h"
#include <stdio.h>


vector <string> palavras_reservadas = { "main", "if", "else", "while", "do", "for", "int", "float", "char" };

int linha = 1;
int coluna = 0;

void inicializarScanner(Scanner *token) {
	token->linha = 0;
	token->coluna = 0;
	memset(token->lexema, 0,sizeof(token->lexema));
	token->tipo = 0;
}

void inserirToken(Scanner *token, int linha, int coluna, char lexema[20], int tipo) {
	token->tipo = tipo;
	strcpy(token->lexema, lexema);
	token->linha = linha;
	token->coluna = coluna;
}

Scanner *scanner(FILE *arquivo){
	Scanner *token = new Scanner;
	inicializarScanner(token);
	static char look_ahead = 13; //Começar com qualquer caracter independente para pegar o próximo.
	int ascii;
	char lexema[20];

	while (1) { 
		memset(lexema, 0, sizeof(lexema));
		int i = 0;
		int flag = 0;
		while (look_ahead == ' ') { //Ignorar todos os espaços
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
			coluna++;
		}
		
		if (look_ahead == '\n') { // Cuidar quando o proximo for EOF (pra nao contar linha)
			while (look_ahead == '\n') { //Enquanto for ENTER
				ascii = fgetc(arquivo);
				look_ahead = (char)ascii;
				if(look_ahead!=-1){
					coluna = 0;
					linha++;
				}
			}
		}
		
		else if(look_ahead == '\r') { //BUG de Carriage Return?
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
		}

		else if(look_ahead=='\t') { //Encontrou um TAB
			coluna=coluna+4;
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
		}

		else if (isalpha(look_ahead) || look_ahead == '_') {// Para PALAVRA RESERVADA ou ID
			while (isalpha(look_ahead) || isdigit(look_ahead) || look_ahead == '_') {
				lexema[i] = look_ahead;
				ascii = fgetc(arquivo);
				look_ahead = (char)ascii;
				i++;
				coluna++;
			}

			for (int i = 0; i<9; i++) {
				if (lexema == palavras_reservadas[i]) {
					inserirToken(token, linha, coluna, lexema, PALAVRA_RESERVADA);
					flag = 1;
					return token;
				}
			}
 
			if(flag==0) {
				inserirToken(token, linha, coluna, lexema, ID);
				return token;
			}
		}

		else if (look_ahead == '.') { //.DIGITO é FLOAT
			lexema[i] = look_ahead;
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
			i++;
			coluna++;
			if(isdigit(look_ahead)) {
				while(isdigit(look_ahead)) {
					lexema[i] = look_ahead;
					ascii = fgetc(arquivo);
					look_ahead = (char)ascii;	
					i++;
					coluna++;
					inserirToken(token, linha, coluna, lexema, VALOR_FLOAT);
					return token;		
				}
			}
			else { //Caso seja outra coisa .ALGO
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Float mal formado!\n", linha, coluna, lexema);
				inserirToken(token, linha, coluna, lexema, ERRO);
				return token;	
			}	
		}

		else if (isdigit(look_ahead)) { // Para INT e FLOAT
			while (isdigit(look_ahead)) { 
				lexema[i] = look_ahead;
				ascii = fgetc(arquivo);
				look_ahead = (char)ascii;
				i++;
				coluna++;
				if (look_ahead == '.') {
					lexema[i] = look_ahead;
					ascii = fgetc(arquivo);
					look_ahead = (char)ascii;
					i++;
					flag = 1;
					coluna++;
					if(!isdigit(look_ahead)) { //se for 2. sem digito após...
						lexema[i] = look_ahead;
						ascii = fgetc(arquivo);
						look_ahead = (char)ascii;
						i++;
						coluna++;
						printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Float mal formado!\n", linha, coluna, lexema);
						inserirToken(token, linha, coluna, lexema, ERRO);
						return token;
					}
				}
			}
			if (flag == 0) {
				inserirToken(token, linha, coluna, lexema, VALOR_INT);
				return token;

			}
			else {
				inserirToken(token, linha, coluna, lexema, VALOR_FLOAT);
				return token;
			}
		}

		else if (look_ahead == '{') { //Abre chaves
			lexema[i] = look_ahead;
			coluna++;
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
			inserirToken(token, linha, coluna, lexema, ABRE_CHAVES);
			return token;
		}

		else if(look_ahead == '(') {
			lexema[i] = look_ahead;
			coluna++;
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
			inserirToken(token, linha, coluna, lexema, ABRE_PARENTESES);
			return token;
		}

		else if(look_ahead == '}') {
			lexema[i] = look_ahead;
			coluna++;
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
			inserirToken(token, linha, coluna, lexema, FECHA_CHAVES);
			return token;
		}

		else if(look_ahead == ')') {
			lexema[i] = look_ahead;
			coluna++;
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
			inserirToken(token, linha, coluna, lexema, FECHA_PARENTESES);
			return token;
		}

		else if(look_ahead == ';') {
			lexema[i] = look_ahead;
			coluna++;
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
			inserirToken(token, linha, coluna, lexema, PONTO_E_VIRGULA);
			return token;
		}

		else if(look_ahead == ',') {
			lexema[i] = look_ahead;
			coluna++;
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
			inserirToken(token, linha, coluna, lexema, VIRGULA);
			return token;
		}

		else if(look_ahead == '<') { // Pode ser menor ou menor ou igual
			lexema[i] = look_ahead;
			coluna++;
			i++;
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
			if(look_ahead == '=') {
				lexema[i] = look_ahead;
				ascii = fgetc(arquivo);
				look_ahead = (char)ascii;
				coluna++;
				inserirToken(token, linha, coluna, lexema, MENOR_OU_IGUAL);
				return token;
			}
			else {
				inserirToken(token, linha, coluna, lexema, MENOR_QUE);
				return token;
			}
		}

		else if(look_ahead == '>') { // Pode ser maior ou maior ou igual
			lexema[i] 	= look_ahead;
			coluna++;
			i++;
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
			if(look_ahead == '=') {
				lexema[i] = look_ahead;
				ascii = fgetc(arquivo);
				look_ahead = (char)ascii;
				coluna++;	
				inserirToken(token, linha, coluna, lexema, MAIOR_OU_IGUAL);
				return token;
			}
			else {
				inserirToken(token, linha, coluna, lexema, MAIOR_QUE);
				return token;
			}
		}

		else if(look_ahead == '=') { // Poder ser igual ou igual igual
			lexema[i] = look_ahead;
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
			i++;
			coluna++;
			if (look_ahead == '=') {
				lexema[i] = look_ahead;
				ascii = fgetc(arquivo);
				look_ahead = (char)ascii;
				coluna++;
				inserirToken(token, linha, coluna, lexema, IGUAL_IGUAL);
				return token;	
			}
			else {
				inserirToken(token, linha, coluna, lexema, IGUAL);
				return token;
			}
		}

		else if (look_ahead == '!') { // Diferente
			lexema[i] = look_ahead;
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
			i++;
			coluna++;
			if (look_ahead == '=') {
				lexema[i] = look_ahead;
				ascii = fgetc(arquivo);
				look_ahead = (char)ascii;
				i++;
				coluna++;
				inserirToken(token, linha, coluna, lexema, DIFERENTE);
				return token;
			}
			else {
				lexema[i]=look_ahead;
				coluna++;
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Exclamação não sucedida por '='!\n", linha, coluna, lexema); 
				inserirToken(token, linha, coluna, lexema, ERRO);
				return token;
			}
		}

		else if(look_ahead == '+') {
			lexema[i] 	= look_ahead;
			coluna++;
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
			inserirToken(token, linha, coluna, lexema, SOMA);
			return token;
		}

		else if(look_ahead == '-') {
			lexema[i] 	= look_ahead;
			coluna++;
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
			inserirToken(token, linha, coluna, lexema, SUBTRACAO);
			return token;
		}

		else if (look_ahead == '*') {
			lexema[i] 	= look_ahead;
			coluna++;
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
			inserirToken(token, linha, coluna, lexema, VEZES);
			return token;
		}

		else if (look_ahead == '/') { // Pode ser: divisão, comentário multilinha ou comentário simples
			lexema[i] = look_ahead;
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
			coluna++;
			if(look_ahead=='/') { // Comentário simples
				while (look_ahead !='\n' && look_ahead != -1) { //Enquanto nao for enter ou EOF
					ascii = fgetc(arquivo);
					look_ahead = (char)ascii;
					coluna++;
				}
			}
			else if(look_ahead == '*') { // Comentário multilinha DANDO PROBLEMA NA LEITURA DE COLUNA
				coluna++; //Como coluna começa com 0, para não contabilizar errado...
				while (1) {
					ascii = fgetc(arquivo);
					look_ahead = (char)ascii;
					coluna++;
					if(look_ahead == EOF) {
						printf("\n\nERRO na linha %d, coluna %d: Fim de arquivo dentro de comentário!\n", linha, coluna); 
						inserirToken(token, linha, coluna, lexema, ERRO);
						return token;
					}
					else if (look_ahead == '*') {
						flag = 1;
					}
					else if (look_ahead == '/' && flag == 1) { //Cuidar no fim do comentário pra não ir pra divisão EX: \*oi*/ 
						ascii = fgetc(arquivo); // Alimentar o look ahead antes de sair
						look_ahead = (char)ascii; //Não contei coluna pois por algum motivo contei coluna a mais em algum momento(?)
						break;
					}
					else if (look_ahead == '\n') { //Deu ENTER no comentário
						linha++; //se for pegar linha e coluna certa tem que ser aq
						coluna = 0;
						flag = 0;
					}
					else {
						flag = 0;
					}
				}
			}
			else {// DIVISÃO
				coluna++;
				inserirToken(token, linha, coluna, lexema, DIVISAO);
				return token;
			}
		}

		else if (look_ahead == 39) { //Encontrou aspas simples
			lexema[i] = look_ahead;
			ascii = fgetc(arquivo);
			look_ahead = (char)ascii;
			i++;
			coluna++;
			if(isalpha(look_ahead) || isdigit(look_ahead)) { //'a'... 
				lexema[i] = look_ahead;
				ascii = fgetc(arquivo);
				look_ahead = (char)ascii;
				i++;
				coluna++;
				if(look_ahead == 39) { //Fecha aspas simples
					lexema[i] = look_ahead;
					ascii = fgetc(arquivo);
					look_ahead = (char)ascii;
					coluna++;	
					inserirToken(token, linha, coluna, lexema, VALOR_CHAR);
					return token;
				}
				else {
					lexema[i] = look_ahead;
					ascii = fgetc(arquivo);
					look_ahead = (char)ascii;
					coluna++;	
					printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s'': Char mal formado!\n", linha, coluna, lexema);
					inserirToken(token, linha, coluna, lexema, ERRO);
					return token;	
				}
			}
			else {
				lexema[i] = look_ahead;
				ascii = fgetc(arquivo);
				look_ahead = (char)ascii;
				coluna++;	
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s'': Char mal formado!\n", linha, coluna, lexema);
				inserirToken(token, linha, coluna, lexema, ERRO);
				return token;	
			}	
		}

		else if (look_ahead == EOF) { //EOF
			inserirToken(token, linha, coluna, lexema, FIM);
			return token;
		}

		else { // Erro
			coluna++;
			printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%c': Caracter inválido\n", linha, coluna, look_ahead); 
			inserirToken(token, linha, coluna, lexema, ERRO);
			return token;
		}
	}
}

