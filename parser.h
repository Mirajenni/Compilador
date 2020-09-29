#ifndef PARSER
#define PARSER

#include <stdlib.h> 
#include "scanner.h"



struct GCI {
	int operacao;
	char lexema[20];
	int tipo;
};


bool parser (FILE *arquivo);
void bloco (FILE *arquivo, Scanner **token);
void declaracao_variavel(FILE *arquivo, Scanner **token);
void comando (FILE *arquivo, Scanner **token);
void comando_basico(FILE *arquivo, Scanner **token);
void iteracao(FILE *arquivo, Scanner **token);
void expressao_relacional(FILE *arquivo, Scanner **token);
void atribuicao(FILE *arquivo, Scanner **token);
GCI *expressao_aritmetica(FILE *arquivo, Scanner **token);
GCI *expressao_aritmetica_linha(FILE *arquivo, Scanner **token);
void expressao_relacional(FILE *arquivo, Scanner **token);
GCI *termo(FILE *arquivo, Scanner **token);
GCI *fator(FILE *arquivo, Scanner **token);

Scanner *proximoToken (FILE *arquivo);

/*
Para as variáveis, sugere-se que a tabela de símbolos seja uma lista encadeada onde os nós serão registros com os atributos das variáveis: lexema e tipo.
O aluno pode modificar a tabela se encontrar utilidade para outro tipo de atributo ou se achar necessário incluir constantes com seus tipos e valores.

Como em toda lista encadeada, precisamos de um nó que aponta para a "cabeça" da lista. Chamemos este nó de "tabela".
----------------------------------------------------------------------------------------------------------------------
Na ativação de um bloco, guarde o conteúdo de "tabela" e adicione as novas variáveis no inicio da tabela de símbolos.
Na desativação, restaure o valor de "tabela", eliminando assim todas as variáveis declaradas nesse bloco.
*Lembre de desalocar todos os nós com as variáveis do bloco sendo desativado.*
----------------------------------------------------------------------------------------------------------------------
A busca a partir de "tabela" sempre encontrará o identificador mais recentemente declarado, por isso as variáveis devem ser incluídas no início da tabela de símbolos.
*/

struct Tabela {
	int tipo;
	char lexema[20];
	int escopo;
	struct Tabela *proximo;	
};

enum tipos_variavel {VALOR_INTEIRO=1, VALOR_FLUTUANTE, VALOR_CHARACTER, VARIAVEL_INTEIRA, VARIAVEL_FLUTUANTE, VARIAVEL_CHARACTER, VAZIO};
//Os operadores estão previamente definidos no SCANNER
void criar_tabela(Tabela **simbolo);
void inserir_tabela(Tabela **simbolo, int tipo, char lexema[20], int escopo, Scanner *token);
void buscar_tabela(Tabela *simbolo, int tipo, char lexema[20], int escopo, Scanner *token);
void remover_mesmo_escopo(Tabela **simbolo, int escopo);
int look_up(Tabela *simbolo, Scanner *token);


/*
Qualquer comando que relacionar duas ou mais entidades (como variáveis e constantes) deverá verificar a compatibilidade de seus tipos.
O tipo char (constantes char) é compatível apenas com ele mesmo. Seu compilador deve aceitar expressões aritméticas e relacionais com variáveis e literais do tipo char. Ou seja, qualquer operação entre operandos char, resulta no tipo char.
Os tipos numéricos float e int são compatíveis, porém não se pode atribuir um float a um int. Além disso, dividindo-se dois inteiros (variáveis ou constantes) o tipo resultante é float
Variáveis devem ter sido declaradas antes de ser usadas, e só podem ser usadas observando-se as regras padrão de escopo. Não podem haver variáveis com o mesmo nome no mesmo escopo, mas em escopos diferentes (e.g., sub-blocos) são permitidas.
A tabela de símbolos deve ser utilizada para pesquisa da existência da variável e seu tipo, e deve dar suporte ao mecanismo de escopo explicado no projeto do parser.
*/
int retornar_tipo_variavel(Scanner *token);

#endif 