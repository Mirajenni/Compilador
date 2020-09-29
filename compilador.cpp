#include <stdio.h>
#include "parser.h"

int main(int argc, char **argv) {
	FILE *arquivo = fopen(argv[1], "r");

	if (arquivo == NULL) {
		cout << "Problema ao abrir arquivo " << argv[1];
	}

	else {
		if(parser(arquivo)==1) {
			cout << "\n";
			fclose (arquivo);
		}
		else {//Erro, mas todos os erros já foram coletados no parser.
			fclose(arquivo);
		}
	}

	return 0;
}
