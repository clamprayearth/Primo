#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int number, numTid, divisao, pronto;
clock_t start, end;
double cpu_time_used;

int calculIndex(int atualIndex, int numLin, int numCol){
	/*Esta função calcula o valor do indice para cada coluna referente a matriz(thread, intervalo)*/
	if (numLin%numCol == 0){ //<-- primeiro verifica se o intervalo caberá na divisão thread/intervalo
		return numLin/numCol;
	} else {
		if(atualIndex%2 == 0) //<-- Caso contrário ele divide o intervalo entre as colunas par e impar 
			return numLin/numCol;
		if(atualIndex%2 != 0)
			return numLin/numCol + 1;	
	}	
}

void *funcao(void *num){
	/* Esta funcao faz o calculo dos numeros primos do intervalo dado. */
	printf("\nThread iniciada\n");
	
	int *intervalo = (int *)num;
	int isPrimo = 1;
	
	for(int i=divisao; i>=0; i--){
		for(int j=2; j<intervalo[i]; j++){
			if(intervalo[i]%j == 0){
				isPrimo = 0;
				break;
			}
		}
		if((isPrimo == 1) && (intervalo[i] != 1) && (intervalo[i] != 0))
			printf(" %d ", intervalo[i]);
		isPrimo = 1;
	}	
	
	free(intervalo);
	pronto = 1;
	printf("\nthread terminou");
	pthread_exit(NULL);
}

int main (int argc, char *argv []){
	start = clock(); //<-- Inicio do cronometro para calcular o tempo de execução
	
	//number = O maior número do intervalo; numTid = numero de thread 
	number = atoi (argv[1]);
	numTid = atoi (argv[2]);
	//status = valor de retorno da chamada de função pthread_create 
	int status;
	//maximo = Valor a ser incrementado para obter o intervalo 
	int maximo = 1;
	
	//alocação dinamica de vetor para as thread
	pthread_t *tid = malloc (numTid * sizeof (pthread_t));
	
	for(int i=0; i<numTid; i++) {	
		
		pthread_attr_t attr;
		
		pthread_attr_init (&attr);
		//Primeiro é determinado o tamanho da coluna da matriz(tread, intervalo)
		divisao = calculIndex(i, number, numTid);
		
		//alocação dinamica de vetor para o intervalo
		int *intervalo = malloc (divisao * sizeof(int));
		
	
		for(int j=0; j<divisao; j++, maximo++){
			intervalo[j] = maximo;	
		}
		
		//Semaforo para as treads
		pronto = 0;
		status = pthread_create(&tid[i], &attr, funcao, intervalo);
		while (!pronto) {}
		
		if (status != 0) {
			printf("Oops. pthread create returned error code %d\n", status);
			exit(-1);
		}
	}
	
	for(int i=0; i<numTid; i++) {
		pthread_join (tid[i], NULL);	
	}
	free(tid);
	end = clock(); //<-- Fim do cronometro para calcular o tempo de execução
	
	//Calculo do tempo final
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	
	printf("\nTempo de exec: %f", cpu_time_used);
	return 0;
}