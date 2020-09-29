#include "parser.h"
#include <sstream>
#include <string>
//LIMPAR O LIXO DE MEMORIA DO LEXEMA NO GCI!!!!!!!!!!
/* Variáveis para a TABELA DE SÍMBOLOS */

int escopo;
char identificador[20];
int tipo_variavel;
Tabela **simbolo = new Tabela*;
/* ---------------------------------- */
int ifs, temp, whiles, do_whiles;


int retornar_tipo_variavel(Scanner *token) {
	if(strcmp(token->lexema, "int")==0) {
		return VARIAVEL_INTEIRA;
	}
	else if (strcmp(token->lexema, "float")==0) {
		return VARIAVEL_FLUTUANTE;
	}
	else { //não tem como vir erro, pois o scanner trata!
		return VARIAVEL_CHARACTER;
	}
}

Scanner *proximoToken (FILE *arquivo) {
	Scanner *token;
	token = scanner(arquivo);
	return token;
}

GCI *fator (FILE *arquivo, Scanner **token) {
	GCI *resultado = new(GCI);
	*token = proximoToken(arquivo);
	if((*token)->tipo==ABRE_PARENTESES) {
		GCI *resultado_expressao = expressao_aritmetica(arquivo, token);
		if((*token)->tipo==FECHA_PARENTESES) {
			return resultado_expressao;
		}
		else {
			if((*token)->tipo!=ERRO) {
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Esperado ')' após expressão aritmética!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
			}
			exit(0);
		}
	}
	else if((*token)->tipo == ID) { //semantico - fazer lookup
		resultado->tipo = look_up(*simbolo, *token); //buscar o tipo do mesmo lexema dentro da tabela de símbolos
		strcpy(resultado->lexema, (*token)->lexema);
		return resultado;
	}
	else if((*token)->tipo == VALOR_FLOAT){ //semantico
		resultado->tipo = VALOR_FLUTUANTE;
		strcpy(resultado->lexema, (*token)->lexema);
		return resultado;
	}
	else if((*token)->tipo == VALOR_INT) { //semantico
		resultado->tipo = VALOR_INTEIRO;
		strcpy(resultado->lexema, (*token)->lexema);
		return resultado;
	}
	else if ((*token)->tipo == VALOR_CHAR) { //semantico
		resultado->tipo = VALOR_CHARACTER;
		strcpy(resultado->lexema, (*token)->lexema);
		return resultado;
	}
	else {
		if((*token)->tipo!=ERRO) {
			printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Esperado id, char, float, int ou '('!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
		}
		exit(0);
	}
}

GCI *termo (FILE *arquivo, Scanner **token) { 
	GCI *resultado_fator_1 = new (GCI), *resultado_fator_2 = new (GCI);
	int operacao, tipo;
	resultado_fator_1 = fator(arquivo, token);
	*token = proximoToken(arquivo);
	while((*token)->tipo==DIVISAO || (*token)->tipo==VEZES) {
		operacao = (*token)->tipo;
		resultado_fator_2 = fator(arquivo, token);
		*token = proximoToken(arquivo);
		if(resultado_fator_2->tipo>=1 && resultado_fator_2->tipo<=6) {
			if((resultado_fator_1->tipo==VALOR_INTEIRO || resultado_fator_1->tipo==VARIAVEL_INTEIRA) && (resultado_fator_2->tipo==VALOR_FLUTUANTE || resultado_fator_2->tipo == VARIAVEL_FLUTUANTE)) {
				tipo = VALOR_FLUTUANTE;
			}
			else if((resultado_fator_1->tipo==VALOR_INTEIRO || resultado_fator_1->tipo==VARIAVEL_INTEIRA) && (resultado_fator_2->tipo==VALOR_CHARACTER || resultado_fator_2->tipo == VARIAVEL_CHARACTER)) {
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Operação entre dois tipos incompatíveis respectivamente: INT e CHAR!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
				exit(0);
			}
			else if((resultado_fator_1->tipo==VALOR_INTEIRO || resultado_fator_1->tipo==VARIAVEL_INTEIRA) && (resultado_fator_2->tipo==VALOR_INTEIRO || resultado_fator_2->tipo == VARIAVEL_INTEIRA)) {
				if(operacao==DIVISAO) 
					tipo = VALOR_FLUTUANTE;
				else 
					tipo = VALOR_INTEIRO;
			}
			else if((resultado_fator_1->tipo==VALOR_FLUTUANTE || resultado_fator_1->tipo==VARIAVEL_FLUTUANTE) && (resultado_fator_2->tipo==VALOR_INTEIRO || resultado_fator_2->tipo==VARIAVEL_INTEIRA)) {
				tipo = VALOR_FLUTUANTE;
			}
			else if((resultado_fator_1->tipo==VALOR_FLUTUANTE || resultado_fator_1->tipo==VARIAVEL_FLUTUANTE) && (resultado_fator_2->tipo==VALOR_FLUTUANTE || resultado_fator_2->tipo==VARIAVEL_FLUTUANTE)) {
				tipo = VALOR_FLUTUANTE;
			}
			else if((resultado_fator_1->tipo==VALOR_FLUTUANTE || resultado_fator_1->tipo==VARIAVEL_FLUTUANTE) && (resultado_fator_2->tipo==VALOR_CHARACTER || resultado_fator_2->tipo==VARIAVEL_CHARACTER)) {
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Operação entre dois tipos incompatíveis respectivamente: FLOAT e CHAR!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
				exit(0);
			}
			else if((resultado_fator_1->tipo==VALOR_CHARACTER || resultado_fator_1->tipo==VARIAVEL_CHARACTER) && (resultado_fator_2->tipo==VALOR_FLUTUANTE || resultado_fator_2->tipo==VARIAVEL_FLUTUANTE)) {
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Operação entre dois tipos incompatíveis: CHAR só opera com CHAR!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
				exit(0);
			}
			else if((resultado_fator_1->tipo==VALOR_CHARACTER || resultado_fator_1->tipo==VARIAVEL_CHARACTER) && (resultado_fator_2->tipo==VALOR_INTEIRO || resultado_fator_2->tipo==VARIAVEL_INTEIRA)) {
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Operação entre dois tipos incompatíveis: CHAR só opera com CHAR!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
				exit(0);
			}
			else {//char com char NÃO. PODE SER >>> ) * 3 
				tipo = VALOR_CHARACTER;
			}

			if((resultado_fator_1->tipo==VARIAVEL_FLUTUANTE || resultado_fator_1->tipo==VALOR_FLUTUANTE) || (resultado_fator_2->tipo==VALOR_FLUTUANTE || resultado_fator_2->tipo==VARIAVEL_FLUTUANTE)) {
				int num = temp;
				if(resultado_fator_1->tipo == VALOR_INTEIRO || resultado_fator_1->tipo == VARIAVEL_INTEIRA) {
					printf("\nT%d = (CAST)%s", temp, resultado_fator_1->lexema);
					resultado_fator_1->tipo = VALOR_FLUTUANTE;
					strcpy(resultado_fator_1->lexema, "T");
					if(num>=10) {
						for(int i = 2; i > 0; i--) {
							resultado_fator_1->lexema[i] = num % 10 + '0';
							num = num/10;
						}
						resultado_fator_1->lexema[3] = '\0';
					}
					else {
						resultado_fator_1->lexema[1] = num + '0';
						resultado_fator_1->lexema[2] = '\0';
					}
				}
				if(resultado_fator_2->tipo == VALOR_INTEIRO || resultado_fator_2->tipo == VARIAVEL_INTEIRA) {
					printf("\nT%d = (CAST)%s", temp, resultado_fator_2->lexema);
					resultado_fator_2->tipo = VALOR_FLUTUANTE;
					strcpy(resultado_fator_2->lexema, "T");
					if(num>=10) {
						for(int i = 2; i > 0; i--) {
							resultado_fator_2->lexema[i] = num % 10 + '0';
							num = num/10;
						}
						resultado_fator_2->lexema[3] = '\0';
					}
					else {
						resultado_fator_2->lexema[1] = num + '0';
						resultado_fator_2->lexema[2] = '\0';
					}
				}
				temp++;
			}

			int num = temp;
			if(operacao == DIVISAO) { //precisam receber as temps
				printf("\nT%d = %s / %s", temp, resultado_fator_1->lexema, resultado_fator_2->lexema);
				strcpy(resultado_fator_1->lexema, "T");
				if(num>=10) {
					for(int i = 2; i > 0; i--) {
						resultado_fator_1->lexema[i] = num % 10 + '0';
						num = num/10;
					}
					resultado_fator_1->lexema[3] = '\0';
				}
				else {
					resultado_fator_1->lexema[1] = num + '0';
					resultado_fator_1->lexema[2] = '\0';
				}
			}
			else {
				printf("\nT%d = %s * %s", temp, resultado_fator_1->lexema, resultado_fator_2->lexema);
				strcpy(resultado_fator_1->lexema,"T");
				if(num>=10) {
					for(int i = 2; i > 0; i--) {
						resultado_fator_1->lexema[i] = num % 10 + '0';
						num = num/10;
					}
					resultado_fator_1->lexema[3] = '\0';
				}
				else {
					resultado_fator_1->lexema[1] = num + '0';
					resultado_fator_1->lexema[2] = '\0';
				}
			}
			temp++;
		}
		resultado_fator_1->tipo = tipo;
	}
	if((resultado_fator_1->tipo==VALOR_INTEIRO || resultado_fator_1->tipo==VARIAVEL_INTEIRA) && (resultado_fator_2->tipo==VALOR_INTEIRO || resultado_fator_2->tipo==VARIAVEL_INTEIRA)) {
		if(operacao==DIVISAO) {
			resultado_fator_1->tipo = VALOR_FLUTUANTE;
		}
	}
	return resultado_fator_1;
}

void bloco (FILE *arquivo, Scanner **token) {
	if((*token)->tipo == ABRE_CHAVES) {
		escopo++;
		*token = proximoToken(arquivo);
		while((strcmp((*token)->lexema, "int")==0) || (strcmp((*token)->lexema, "float")==0) || ((strcmp((*token)->lexema, "char"))==0)) {
			declaracao_variavel(arquivo, token);
		}
		Tabela *auxiliar = *simbolo;
		while(auxiliar!=NULL) {
			auxiliar = auxiliar->proximo;
		}
		while((*token)->tipo==ABRE_CHAVES || (*token)->tipo==ID || (strcmp((*token)->lexema, "do")==0) || (strcmp((*token)->lexema, "while")==0) || (strcmp((*token)->lexema, "if")==0)) {
			comando(arquivo, token);
		}
		if((*token)->tipo==FECHA_CHAVES) {
			remover_mesmo_escopo(simbolo, escopo);
			escopo--;
			*token = proximoToken(arquivo);
			return;
		}
		else {
			if((*token)->tipo!=ERRO) {
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Esperado '}' após declaração de variável ou comando!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);
			}
			exit(0);
		}
	}
	else { 
		if((*token)->tipo!=ERRO) {
			printf("\n\nERRO na linha %d, coluna %d: Esperado atribuição após int main ()!\n", (*token)->linha, (*token)->coluna);
		}
		exit(0);
	}
}


void declaracao_variavel(FILE *arquivo, Scanner **token) {
	memset(identificador, 0, sizeof(identificador));
	tipo_variavel = retornar_tipo_variavel(*token); //pegou o TIPO (int, float ou char)

	*token = proximoToken(arquivo);
	if((*token)->tipo==ID) {
		strcpy(identificador, (*token)->lexema); //pegou o nome da VARIÁVEL (identificador)
		*token = proximoToken(arquivo);
		if((*token)->tipo==PONTO_E_VIRGULA) { //já pode inserir na TABELA DE SÍMBOLOS
			inserir_tabela(simbolo, tipo_variavel, identificador, escopo, *token); //o ESCOPO global já é ditado por bloco
			*token = proximoToken(arquivo);
			return;
		}
		else { //Pode ser erro, fim de arquivo ou {,id}*	
			inserir_tabela(simbolo, tipo_variavel, identificador, escopo, *token); //Inserir mesmo assim
			while((*token)->tipo==VIRGULA) { //cuidado com ,float seila
				memset(identificador, 0, sizeof(identificador));
				*token = proximoToken(arquivo);
				if ((*token)->tipo!=ID && (*token)->tipo!=ERRO) {
					printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Esperado ID após vírgula!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);
					exit(0);
				}
				strcpy(identificador, (*token)->lexema); //pegou o nome da VARIÁVEL (identificador)
				inserir_tabela(simbolo, tipo_variavel, identificador, escopo, *token); //já tem (exemplo): int x, y, z
				*token = proximoToken(arquivo);
			}
			if((*token)->tipo == PONTO_E_VIRGULA) {
				*token = proximoToken(arquivo);
				return;
			}
			else {
				if((*token)->tipo!=ERRO) {
					printf("\n\nERRO na linha %d, coluna %d: Esperado: ponto e virgula ou outros ID's!\n", (*token)->linha, (*token)->coluna);
				}
				exit(0);
			}
		}
	}
	else {
		if((*token)->tipo!=ERRO) {
			printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Esperado ID após tipo!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);
		}
		exit(0);
	}
}

void comando (FILE *arquivo, Scanner **token) {
	if((*token)->tipo==ID || (*token)->tipo==ABRE_CHAVES) {
		comando_basico(arquivo, token);
		return;
	}
	else if ((strcmp((*token)->lexema, "do")==0) || (strcmp((*token)->lexema, "while")==0)) {
		iteracao(arquivo, token);
		return;
	}
	else { //if "("<expr_relacional>")" <comando> {else <comando>}?, ou fim de arquivo etc
		if(strcmp((*token)->lexema, "if")==0) {
			*token = proximoToken(arquivo);
			if((*token)->tipo == ABRE_PARENTESES) {
				expressao_relacional(arquivo, token);
				if((*token)->tipo==FECHA_PARENTESES) {
					int i = ifs;
					cout << "\nIF T" << temp-1 << " == 0 GOTO ELSE" << i;
					ifs++;
					*token = proximoToken(arquivo);
					comando(arquivo, token); 
					cout << "\nGOTO FIM DO IF"	 << i;
					cout << "\nELSE" << i << ":";
					if((strcmp((*token)->lexema, "else")==0)) {
						*token = proximoToken(arquivo);
						comando(arquivo, token);
						cout << "\nFIM DO IF" << i << ":";
						ifs++;
						return;
					}
					else {
						cout << "\nFIM DO IF" << i << ":";
						return;
					}
				}
				else {
					if((*token)->tipo!=ERRO) {
						printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Esperado ')' após expressão relacional!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
					}
					exit(0);
				}
			}
			else {
				if((*token)->tipo!=ERRO) {
					printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Esperado '(' após if!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);
				}
				exit(0);
			}
		}
		else {
			return;
		}
	}
}

void comando_basico(FILE *arquivo, Scanner **token) {
	if((*token)->tipo==ID) {
		atribuicao(arquivo, token);
	}
	else if((*token)->tipo==ABRE_CHAVES){
		bloco(arquivo, token);
	}
	else {
		if((*token)->tipo!=ERRO) {
			printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Esperado atribuição ou bloco!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);
		}
		exit(0);
	}
}


GCI *expressao_aritmetica_linha(FILE *arquivo, Scanner **token) { 
	GCI *resultado_expressao = new (GCI), *resultado_termo = new (GCI);
	int tipo, operacao;
	if((*token)->tipo==SOMA || (*token)->tipo==SUBTRACAO) {
		operacao = (*token)->tipo;
		resultado_termo = termo(arquivo, token);
		resultado_termo->operacao = operacao;
		resultado_expressao = expressao_aritmetica_linha(arquivo, token);
		if(resultado_expressao->tipo>=1 && resultado_expressao->tipo<=6) {
			if ((resultado_termo->tipo==VALOR_INTEIRO || resultado_termo->tipo==VARIAVEL_INTEIRA) && (resultado_expressao->tipo==VALOR_INTEIRO || resultado_expressao->tipo==VARIAVEL_INTEIRA)) {
				tipo = VALOR_INTEIRO;
			}

			else if((resultado_termo->tipo==VALOR_INTEIRO || resultado_termo->tipo==VARIAVEL_INTEIRA) && (resultado_expressao->tipo==VALOR_FLUTUANTE || resultado_expressao->tipo==VARIAVEL_FLUTUANTE)) {
				tipo = VALOR_FLUTUANTE;
			}
			else if((resultado_termo->tipo==VALOR_INTEIRO || resultado_termo->tipo==VARIAVEL_INTEIRA) && (resultado_expressao->tipo==VALOR_CHARACTER || resultado_expressao->tipo==VARIAVEL_CHARACTER)) {
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Operação entre dois tipos incompatíveis respectivamente: INT e CHAR!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
				exit(0);
			}
			else if((resultado_termo->tipo==VALOR_FLUTUANTE || resultado_termo->tipo==VARIAVEL_FLUTUANTE) && (resultado_expressao->tipo==VALOR_INTEIRO || resultado_expressao->tipo==VARIAVEL_INTEIRA)) {
				tipo = VALOR_FLUTUANTE;
			}
			else if((resultado_termo->tipo==VALOR_FLUTUANTE || resultado_termo->tipo==VARIAVEL_FLUTUANTE) && (resultado_expressao->tipo==VALOR_CHARACTER || resultado_expressao->tipo==VARIAVEL_CHARACTER)) {
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Operação entre dois tipos incompatíveis respectivamente: FLOAT e CHAR!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
				exit(0);
			}
			else if((resultado_termo->tipo==VALOR_FLUTUANTE || resultado_termo->tipo==VARIAVEL_FLUTUANTE) && (resultado_expressao->tipo==VALOR_FLUTUANTE || resultado_expressao->tipo==VARIAVEL_FLUTUANTE)) {
				tipo = VALOR_FLUTUANTE;
			}
			else if((resultado_termo->tipo==VALOR_CHARACTER || resultado_termo->tipo==VARIAVEL_CHARACTER) && (resultado_expressao->tipo==VALOR_FLUTUANTE || resultado_expressao->tipo==VARIAVEL_FLUTUANTE)) {
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Operação entre dois tipos incompatíveis respectivamente: CHAR e FLOAT!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
				exit(0);
			}
			else if((resultado_termo->tipo==VALOR_CHARACTER || resultado_termo->tipo==VARIAVEL_CHARACTER) && (resultado_expressao->tipo==VALOR_INTEIRO || resultado_expressao->tipo==VARIAVEL_INTEIRA)) {
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Operação entre dois tipos incompatíveis respectivamente: FLOAT e INT!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
				exit(0);
			}
			else {	
				tipo = VALOR_CHARACTER;
			}
			if((resultado_termo->tipo==VARIAVEL_FLUTUANTE || resultado_termo->tipo==VALOR_FLUTUANTE) || (resultado_expressao->tipo==VALOR_FLUTUANTE || resultado_expressao->tipo==VARIAVEL_FLUTUANTE)) {
				int num = temp;
				if(resultado_termo->tipo == VALOR_INTEIRO || resultado_termo->tipo == VARIAVEL_INTEIRA) {
					printf("\nT%d = (CAST)%s", temp, resultado_termo->lexema);
					resultado_termo->tipo = VALOR_FLUTUANTE;
					strcpy(resultado_termo->lexema, "T");
					if(num>=10) {
						for(int i = 2; i > 0; i--) {
							resultado_termo->lexema[i] = num % 10 + '0';
							num = num/10;
						}
						resultado_termo->lexema[3] = '\0';
					}
					else {
						resultado_termo->lexema[1] = num + '0';
						resultado_termo->lexema[2] = '\0';
					}
				}
				if(resultado_expressao->tipo == VALOR_INTEIRO || resultado_expressao->tipo == VARIAVEL_INTEIRA) {
					printf("\nT%d = (CAST)%s", temp, resultado_expressao->lexema);
					resultado_expressao->tipo = VALOR_FLUTUANTE;
					strcpy(resultado_expressao->lexema, "T");
					if(num>=10) {
						for(int i = 2; i > 0; i--) {
							resultado_expressao->lexema[i] = num % 10 + '0';
							num = num/10;
						}
						resultado_expressao->lexema[3] = '\0';
					}
					else {
						resultado_expressao->lexema[1] = num + '0';
						resultado_expressao->lexema[2] = '\0';
					}
				}
				temp++;
			}


			int num = temp;
			if(resultado_expressao->operacao==SOMA) {
				printf("\nT%d = %s + %s", temp, resultado_termo->lexema, resultado_expressao->lexema);
				strcpy(resultado_termo->lexema,"T");
				if(num>=10) {
					for(int i = 2; i > 0; i--) {
						resultado_termo->lexema[i] = num % 10 + '0';
						num = num/10;
					}
				}
				else {
					resultado_termo->lexema[1] = num + '0';
				}
			}
			else {
				printf("\nT%d = %s - %s", temp, resultado_termo->lexema, resultado_expressao->lexema);
				strcpy(resultado_termo->lexema,"T");
				if(num>=10) {
					for(int i = 2; i > 0; i--) {
						resultado_termo->lexema[i] = num % 10 + '0';
						num = num/10;
					}
				}
				else {
					resultado_termo->lexema[1] = num + '0';
				}
			}
			temp++;
		}
		//resultado_termo->tipo = tipo;
		return resultado_termo;
	}
	else { 
		if(resultado_expressao!=NULL) {
			if(resultado_expressao->tipo>=1 && resultado_expressao->tipo<=6) //se existe etc
				return resultado_expressao;
		}
		return resultado_termo;
	}
}	

GCI *expressao_aritmetica(FILE *arquivo, Scanner **token) {
	GCI *resultado_termo, *resultado_expressao;
	int tipo;
	resultado_termo = termo(arquivo, token); 
	resultado_expressao = expressao_aritmetica_linha(arquivo, token);
	if(resultado_expressao!=NULL) {
		if(resultado_expressao->tipo>=1 && resultado_expressao->tipo<=6) {//aqui gera codigo
			if ((resultado_termo->tipo==VALOR_INTEIRO || resultado_termo->tipo==VARIAVEL_INTEIRA) && (resultado_expressao->tipo==VALOR_INTEIRO || resultado_expressao->tipo==VARIAVEL_INTEIRA)) {
				tipo = VALOR_INTEIRO;
			}
			else if((resultado_termo->tipo==VALOR_INTEIRO || resultado_termo->tipo==VARIAVEL_INTEIRA) && (resultado_expressao->tipo==VALOR_FLUTUANTE || resultado_expressao->tipo==VARIAVEL_FLUTUANTE)) {
				tipo = VALOR_FLUTUANTE;
			}
			else if((resultado_termo->tipo==VALOR_INTEIRO || resultado_termo->tipo==VARIAVEL_INTEIRA) && (resultado_expressao->tipo==VALOR_CHARACTER || resultado_expressao->tipo==VARIAVEL_CHARACTER)) {
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Operação entre dois tipos incompatíveis respectivamente: INT e CHAR!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
				exit(0);
			}
			else if((resultado_termo->tipo==VALOR_FLUTUANTE || resultado_termo->tipo==VARIAVEL_FLUTUANTE) && (resultado_expressao->tipo==VALOR_INTEIRO || resultado_expressao->tipo==VARIAVEL_INTEIRA)) {
				tipo = VALOR_FLUTUANTE;
			}
			else if((resultado_termo->tipo==VALOR_FLUTUANTE || resultado_termo->tipo==VARIAVEL_FLUTUANTE) && (resultado_expressao->tipo==VALOR_CHARACTER || resultado_expressao->tipo==VARIAVEL_CHARACTER)) {
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Operação entre dois tipos incompatíveis respectivamente: FLOAT e CHAR!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
				exit(0);
			}
			else if((resultado_termo->tipo==VALOR_FLUTUANTE || resultado_termo->tipo==VARIAVEL_FLUTUANTE) && (resultado_expressao->tipo==VALOR_FLUTUANTE || resultado_expressao->tipo==VARIAVEL_FLUTUANTE)) {
				tipo = VALOR_FLUTUANTE;
			}
			else if((resultado_termo->tipo==VALOR_CHARACTER || resultado_termo->tipo==VARIAVEL_CHARACTER) && (resultado_expressao->tipo==VALOR_FLUTUANTE || resultado_expressao->tipo==VARIAVEL_FLUTUANTE)) {
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Operação entre dois tipos incompatíveis respectivamente: CHAR e FLOAT!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
				exit(0);
			}
			else if((resultado_termo->tipo==VALOR_CHARACTER || resultado_termo->tipo==VARIAVEL_CHARACTER) && (resultado_expressao->tipo==VALOR_INTEIRO || resultado_expressao->tipo==VARIAVEL_INTEIRA)) {
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Operação entre dois tipos incompatíveis respectivamente: FLOAT e INT!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
				exit(0);
			}
			else {	
				tipo = VALOR_CHARACTER;
			}


			if((resultado_termo->tipo==VARIAVEL_FLUTUANTE || resultado_termo->tipo==VALOR_FLUTUANTE) || (resultado_expressao->tipo==VALOR_FLUTUANTE || resultado_expressao->tipo==VARIAVEL_FLUTUANTE)) {
				int num = temp;
				if(resultado_termo->tipo == VALOR_INTEIRO || resultado_termo->tipo == VARIAVEL_INTEIRA) {
					printf("\nT%d = (CAST)%s", temp, resultado_termo->lexema);
					resultado_termo->tipo = VALOR_FLUTUANTE;
					strcpy(resultado_termo->lexema, "T");
					if(num>=10) {
						for(int i = 2; i > 0; i--) {
							resultado_termo->lexema[i] = num % 10 + '0';
							num = num/10;
						}
						resultado_termo->lexema[3] = '\0';
					}
					else {
						resultado_termo->lexema[1] = num + '0';
						resultado_termo->lexema[2] = '\0';
					}
				}
				if(resultado_expressao->tipo == VALOR_INTEIRO || resultado_expressao->tipo == VARIAVEL_INTEIRA) {
					printf("\nT%d = (CAST)%s", temp, resultado_expressao->lexema);
					resultado_expressao->tipo = VALOR_FLUTUANTE;
					strcpy(resultado_expressao->lexema, "T");
					if(num>=10) {
						for(int i = 2; i > 0; i--) {
							resultado_expressao->lexema[i] = num % 10 + '0';
							num = num/10;
						}
						resultado_expressao->lexema[3] = '\0';
					}
					else {
						resultado_expressao->lexema[1] = num + '0';
						resultado_expressao->lexema[2] = '\0';
					}
				}
				temp++;
			}
			int num = temp;
			if(resultado_expressao->operacao==SOMA) {
				printf("\nT%d = %s + %s", temp, resultado_termo->lexema, resultado_expressao->lexema);
				strcpy(resultado_termo->lexema,"T");
				if(num>=10) {
					for(int i = 2; i > 0; i--) {
						resultado_termo->lexema[i] = num % 10 + '0';
						num = num/10;
					}
					resultado_termo->lexema[3] = '\0';
				}
				else {
					resultado_termo->lexema[1] = num + '0';
					resultado_termo->lexema[2] = '\0';
				}
			}
			else {
				printf("\nT%d = %s - %s", temp, resultado_termo->lexema, resultado_expressao->lexema);
				strcpy(resultado_termo->lexema,"T");
				if(num>=10) {
					for(int i = 2; i > 0; i--) {
						resultado_termo->lexema[i] = num % 10 + '0';
						num = num/10;
					}
					resultado_termo->lexema[3] = '\0';
				}
				else {
					resultado_termo->lexema[1] = num + '0';
					resultado_termo->lexema[2] = '\0';
				}
			}
			temp++;
			return resultado_termo;
		}
	}
	if(resultado_termo->tipo == VARIAVEL_INTEIRA || resultado_termo->tipo == VARIAVEL_FLUTUANTE || resultado_termo->tipo == VARIAVEL_CHARACTER) { //casting para retornar valores, não variáveis
		if(resultado_termo->tipo == VARIAVEL_INTEIRA) {
			resultado_termo->tipo = VALOR_INTEIRO;
		}
		else if(resultado_termo->tipo == VARIAVEL_FLUTUANTE) {
			resultado_termo->tipo = VALOR_FLUTUANTE;
		}
		else {
			resultado_termo->tipo = VALOR_CHARACTER;
		}
	}
	return resultado_termo;
}

void expressao_relacional(FILE *arquivo, Scanner **token) { 
	GCI *resultado_expressao_1, *resultado_expressao_2;
	int operacao;
	resultado_expressao_1 = expressao_aritmetica(arquivo, token);
	//cout << "\nRES: " << resultado_expressao_1->lexema;
	if((*token)->tipo==MENOR_QUE || (*token)->tipo==MAIOR_QUE || (*token)->tipo==MENOR_OU_IGUAL || (*token)->tipo==MAIOR_OU_IGUAL || (*token)->tipo==IGUAL_IGUAL || (*token)->tipo==DIFERENTE) {
		operacao = (*token)->tipo;
		resultado_expressao_2 = expressao_aritmetica(arquivo, token); 
		//cout << "\nRES: " << resultado_expressao_2->lexema;
		if((resultado_expressao_1->tipo==VALOR_INTEIRO || resultado_expressao_1->tipo==VARIAVEL_INTEIRA) && (resultado_expressao_2->tipo==VALOR_CHARACTER || resultado_expressao_2->tipo == VARIAVEL_CHARACTER)) {
			printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Operação entre dois tipos incompatíveis respectivamente: INT e CHAR!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
			exit(0);
		}
		else if((resultado_expressao_1->tipo==VALOR_FLUTUANTE || resultado_expressao_1->tipo==VARIAVEL_FLUTUANTE) && (resultado_expressao_2->tipo==VALOR_CHARACTER || resultado_expressao_2->tipo==VARIAVEL_CHARACTER)) {
			printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Operação entre dois tipos incompatíveis respectivamente: FLOAT e CHAR!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
			exit(0);
		}
		else if((resultado_expressao_1->tipo==VALOR_CHARACTER || resultado_expressao_1->tipo==VARIAVEL_CHARACTER) && (resultado_expressao_2->tipo!=VALOR_CHARACTER && resultado_expressao_2->tipo!=VARIAVEL_CHARACTER)) {
			printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Operação entre dois tipos incompatíveis: CHAR só opera com CHAR!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
			exit(0);
		}
	}
	else {
		if((*token)->tipo!=ERRO) {
			printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Esperado operação relacional (<=, >=, <, >, ==, !=)!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
		}
		exit(0);
	}
	if((resultado_expressao_1->tipo==VARIAVEL_FLUTUANTE || resultado_expressao_1->tipo==VALOR_FLUTUANTE) || (resultado_expressao_2->tipo==VALOR_FLUTUANTE || resultado_expressao_2->tipo==VARIAVEL_FLUTUANTE)) {
		int num = temp;
		if(resultado_expressao_1->tipo == VALOR_INTEIRO || resultado_expressao_1->tipo == VARIAVEL_INTEIRA) {
			printf("\nT%d = (CAST)%s", temp, resultado_expressao_1->lexema);
			resultado_expressao_1->tipo = VALOR_FLUTUANTE;
			strcpy(resultado_expressao_1->lexema, "T");
			if(num>=10) {
				for(int i = 2; i > 0; i--) {
					resultado_expressao_1->lexema[i] = num % 10 + '0';
					num = num/10;
				}
				resultado_expressao_1->lexema[3] = '\0';
			}
			else {
				resultado_expressao_1->lexema[1] = num + '0';
				resultado_expressao_1->lexema[2] = '\0';
			}
		}
		if(resultado_expressao_2->tipo == VALOR_INTEIRO || resultado_expressao_2->tipo == VARIAVEL_INTEIRA) {
			printf("\nT%d = (CAST)%s", temp, resultado_expressao_2->lexema);
			resultado_expressao_2->tipo = VALOR_FLUTUANTE;
			strcpy(resultado_expressao_2->lexema, "T");
			if(num>=10) {
				for(int i = 2; i > 0; i--) {
					resultado_expressao_2->lexema[i] = num % 10 + '0';
					num = num/10;
				}
				resultado_expressao_2->lexema[3] = '\0';
			}
			else {
				resultado_expressao_2->lexema[1] = num + '0';
				resultado_expressao_2->lexema[2] = '\0';
			}
		}
		temp++;
	}
	if(operacao==MENOR_QUE) {
		printf("\nT%d = %s < %s", temp, resultado_expressao_1->lexema, resultado_expressao_2->lexema);
	}
	else if(operacao==MAIOR_QUE) {
		printf("\nT%d = %s > %s", temp, resultado_expressao_1->lexema, resultado_expressao_2->lexema);
	}
	else if(operacao==MENOR_OU_IGUAL) {
		printf("\nT%d = %s <= %s", temp, resultado_expressao_1->lexema, resultado_expressao_2->lexema);
	}
	else if(operacao==MAIOR_OU_IGUAL) {
		printf("\nT%d = %s >= %s", temp, resultado_expressao_1->lexema, resultado_expressao_2->lexema);
	}
	else if(operacao==IGUAL_IGUAL) {
		printf("\nT%d = %s == %s", temp, resultado_expressao_1->lexema, resultado_expressao_2->lexema);
	}
	else {
		printf("\nT%d = %s != %s", temp, resultado_expressao_1->lexema, resultado_expressao_2->lexema);
	}
	temp++;
}


void iteracao(FILE *arquivo, Scanner **token) {
	if(strcmp((*token)->lexema, "while")==0) {
		int i = whiles;
		*token = proximoToken(arquivo);
		cout << "\nWHILE_BEGIN" << i << ":";
		if((*token)->tipo==ABRE_PARENTESES) {
			expressao_relacional(arquivo, token);
			if((*token)->tipo==FECHA_PARENTESES) {
				cout << "\nIF T" << temp-1 << " == 0 GOTO END_WHILE" << i;
				whiles++;
				*token = proximoToken(arquivo);
				comando(arquivo, token);
				cout << "\nGOTO WHILE_BEGIN" << i << ":";
				cout << "\nEND_WHILE" << i << ":";
				return;
			}
			else {
				if((*token)->tipo!=ERRO) {
					printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Esperado ')'!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
				}
				exit(0);
			}
		}
		else {
			if((*token)->tipo!=ERRO) {
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Esperado '(' após 'while'!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
			}
			exit(0);
		}
	}
	else if(strcmp((*token)->lexema, "do")==0) { 
		int i = do_whiles;
		do_whiles++;
		cout << "\nDO_WHILE_BEGIN" << i << ":";
		*token = proximoToken(arquivo);
		comando(arquivo, token);
		if(strcmp((*token)->lexema, "while")==0) {
			*token = proximoToken(arquivo);
			if((*token)->tipo==ABRE_PARENTESES) {
				expressao_relacional(arquivo, token);
				if((*token)->tipo==FECHA_PARENTESES) {
					*token = proximoToken(arquivo);
					if((*token)->tipo==PONTO_E_VIRGULA) {
						*token = proximoToken(arquivo);
						cout << "\nIF T" << (temp-1)	 << " == 1 GOTO DO_WHILE_BEGIN" << i;
						return;	
					}
					else {
						if((*token)->tipo!=ERRO) {
							printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Esperado ';' após fechamento de 'do while'!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
						}
						exit(0);
					}
				}
				else {
					if((*token)->tipo!=ERRO) {
						printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Esperado ')' após expressão relacional!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
					}
					exit(0);
				}
			}
			else {
				if((*token)->tipo!=ERRO) {
					printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Esperado '(' após 'while'!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
				}
				exit(0);
			}
		}
		else {
			if((*token)->tipo!=ERRO) {
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Esperado comando após 'do'!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
			}
			exit(0);	
		}
	}
	else {
		return;
	}
}

void atribuicao(FILE *arquivo, Scanner **token) { //procura na tabela de simbolos para comparação de tipos na atribuição
	int resultado;
	char lexema[20];
	GCI *resultado_expressao;
	resultado = look_up(*simbolo, *token);
	strcpy(lexema, (*token)->lexema);
	*token = proximoToken(arquivo);
	if((*token)->tipo == IGUAL) {
		resultado_expressao = expressao_aritmetica(arquivo, token);
		if(resultado==VARIAVEL_INTEIRA && (resultado_expressao->tipo==VALOR_FLUTUANTE || resultado_expressao->tipo==VARIAVEL_FLUTUANTE)) {
			printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': INT não pode receber FLOAT!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
			exit(0);
		}
		else if (resultado==VARIAVEL_CHARACTER && (resultado_expressao->tipo!=VALOR_CHARACTER && resultado_expressao->tipo!=VARIAVEL_CHARACTER)) {
			printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': CHAR só pode receber CHAR!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
			exit(0);
		}
		else if (resultado!=VARIAVEL_CHARACTER && (resultado_expressao->tipo==VALOR_CHARACTER || resultado_expressao->tipo==VARIAVEL_CHARACTER)) {
			printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Apenas CHAR pode receber CHAR!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
			exit(0);
		}
		else {//Quando não é nenhum erro semântico continua
			if((*token)->tipo==PONTO_E_VIRGULA) { //Devo fazer algo para mudar o tipo do atribuidor?
				if((resultado==VARIAVEL_FLUTUANTE || resultado==VALOR_FLUTUANTE) && (resultado_expressao->tipo==VARIAVEL_INTEIRA || resultado_expressao->tipo==VALOR_INTEIRO)) {
					printf("\n%s = (CAST)%s", lexema, resultado_expressao->lexema);
				}
				else {
					printf("\n%s = %s", lexema, resultado_expressao->lexema);
				}
				*token = proximoToken(arquivo);
				return;
			}
			else {
				if((*token)->tipo!=ERRO) {
					printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Expressão aritmética mal construída ou esquecimento do ';' após expressão aritmética!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
				}
				exit(0);
			}
		}
	}
	else {
		if((*token)->tipo!=ERRO) {
			printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Esperado '=' após ID!\n", (*token)->linha, (*token)->coluna, (*token)->lexema);	
		}
		exit(0);
	}
}


bool parser (FILE *arquivo) {
	Scanner *token;
	token = scanner(arquivo);
	if(token->tipo!=FIM && token->tipo!=ERRO) {
		if(strcmp(token->lexema, "int")==0) {
			token = scanner(arquivo); 
			if(token->tipo!=FIM && token->tipo!=ERRO) {	
				if(strcmp(token->lexema, "main")==0) {
					token = scanner(arquivo); 
					if(token->tipo!=FIM && token->tipo!=ERRO) {
						if(strcmp(token->lexema, "(")==0) {
							token = scanner(arquivo); 
							if(token->tipo!=FIM && token->tipo!=ERRO) {
								if(strcmp(token->lexema, ")")==0) {
									token = proximoToken(arquivo);
									bloco(arquivo, &token);
									if(token->tipo!=FIM) {
										if(token->tipo!=ERRO) {
											printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Não pode haver mais código após fechamento do int main () {}!\n", token->linha, token->coluna, token->lexema);
										}
										return 0;
									}
									else {
										return 1;
									}
								}
								else {
									printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Falta da inicialização ')' após '('!\n", token->linha, token->coluna, token->lexema);
									return 0;
								}
							}
							else if (token->tipo == FIM) {
								printf("\n\nERRO na linha %d, coluna %d: Encontrado fim de arquivo após '('. Token esperado: ')'!\n", token->linha, token->coluna);
								return 0;
							}
							else {
								return 0;
							}
						}
						else {
							printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Falta da inicialização '(' após 'main'!\n", token->linha, token->coluna, token->lexema);
							return 0;
						}
					}
					else if (token->tipo == FIM) {
						printf("\n\nERRO na linha %d, coluna %d: Encontrado fim de arquivo após 'main'. Token esperado: '('!\n", token->linha, token->coluna);
						return 0;
					}
					else {
						return 0;
					}
				}
				else {
					printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Falta da inicialização 'main' após 'int'!\n", token->linha, token->coluna, token->lexema);
					return 0;
				}
			}
			else if (token->tipo == FIM) {
				printf("\n\nERRO na linha %d, coluna %d: Encontrado fim de arquivo após 'int'. Token esperado: 'main'!\n", token->linha, token->coluna);
				return 0;
			}
			else {
				return 0;
			}
		}
		else {
			printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Falta da inicialização 'int'!\n", token->linha, token->coluna, token->lexema);
			return 0;
		}
	}
	else if (token->tipo == FIM) { //Arquivo vazio
		printf("\n\nERRO na linha 1, coluna 1: Arquivo vazio e falta da inicialização 'int'!\n");
		return 0;
	}
	else { //ERRO
		return 0;
	}
}


/*  ~ TABELA DE SÍMBOLOS ~ */

void inserir_tabela(Tabela **simbolo, int tipo, char lexema[20], int escopo, Scanner *token) {
	Tabela *auxiliar = new Tabela;
	if(*simbolo == NULL) {
		auxiliar->escopo = escopo;
		auxiliar->tipo = tipo;
		strcpy(auxiliar->lexema, lexema);
		auxiliar->proximo = NULL;
		*simbolo = auxiliar;
	}
	else { //Fazer busca para depois inserir
		buscar_tabela(*simbolo, tipo, lexema, escopo, token);
		auxiliar->tipo = tipo;
		auxiliar->escopo = escopo;
		strcpy(auxiliar->lexema, lexema);
		auxiliar->proximo = *simbolo;
		*simbolo = auxiliar;
	}
}

void buscar_tabela(Tabela *simbolo, int tipo, char lexema[20], int escopo, Scanner *token) {
	while(simbolo!=NULL) {
		if((strcmp((simbolo->lexema), lexema)==0) && (simbolo->escopo==escopo)) {
			if(simbolo->tipo == tipo) {
				printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Redefinição do tipo da variável '%s'!\n", token->linha, token->coluna, token->lexema, simbolo->lexema); //ajeitar o tipo	
				exit(0);
			}
			printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Variável '%s' já declarada!\n", token->linha, token->coluna, token->lexema, simbolo->lexema);
			exit(0);	
		}
		simbolo = simbolo->proximo;
	}
}

void remover_mesmo_escopo(Tabela **simbolo, int escopo) {
	Tabela *auxiliar = *simbolo;
	while(auxiliar!=NULL && auxiliar->escopo==escopo) {
		if(auxiliar->escopo == escopo) {
			*simbolo = auxiliar->proximo;
			free(auxiliar);
			auxiliar = *simbolo;
		}
	}
}

int look_up(Tabela *simbolo, Scanner *token) {
	while(simbolo!=NULL) {
		if(strcmp(simbolo->lexema, token->lexema)==0) {
			return simbolo->tipo;
		}
		simbolo = simbolo->proximo;
	}
	printf("\n\nERRO na linha %d, coluna %d, ultimo token lido '%s': Variável '%s' não declarada!\n", token->linha, token->coluna, token->lexema, token->lexema);
	exit(0);	
}