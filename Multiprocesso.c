#include <unistd.h>
#include <signal.h>
#include "function.h"


/* Ritorna la parola successiva del file, acquisice caratteri finchè non incontra " (spazi) /r /n EOF"
   param: File in input da leggere, 
   		  intero di massimo numero di caratteri per riga */

char* prossima_parola(FILE *f,int caratteri_max){
	int spazio_max=(caratteri_max+1)*4; 
	char parola[spazio_max];
	char c='\0';
	strcpy(parola,"");

	while((c!=' ') && (c!='\r') && (c!='\n')  && (c!=EOF)){  
		c=fgetc(f);
		char temp[2]={c,'\0'};
		strcat(parola,temp);
	}

	if (c==EOF){
		strcpy(parola,"");
	}
	
	return strdup(parola);
}

/*Crea la pagina da stampare, acquisisce parole/caratteri dal file in input e li inserisce in una stringha 
se il numero di caratteri-(lunghezza della parola successiva)>0 inserisce la parola e continua a acquisire parola
se l'ultimo caratteri inserito è \r (quindi fine paragrafo), richiama la funzione "modifica stringa(..)", concatena alla riga della matrice e lascia la riga successiva vuota 
altrimenti  richiama "modifica stringa(..)" e la concatena alla riga corrente della matrice

param:File in input da leggere, 
	  distanza tra le collone, 
	  massimo numero i caratteri per righe,
	  massimo numero di righe, massimo numero di colonne, 
	  matrice dove memorizzare la pagine

ritorna una valore 1 se il file è terminato	  
	  */


int crea_pagina(int dist_colonne,int caratteri_max, int righe_max, int colonne_max,char **p,int pipeLC,char* last){ 
	int riga=0, colonna=0;
	int rimanenti=caratteri_max; //Numero di caratteri disponibili nella stringa
	int occupati=0; //numero di caratteri occupati
	int lunghezzaSbyte;
	int lunghezzaPchar,lunghezzaPbyte;//RIspettivamente lunghezza parola in numero di caratteri e byte
	int isEOF=0;
	char parola[(caratteri_max+1)*4];
	char stringa[caratteri_max*4];//Ogni volta che aumento la riga libero la stringa
	strcpy(stringa,""); //libero la stringa
	strcpy(parola,""); //libero la stringa
	for (int i=0; i<=righe_max;i++){
		strcpy(p[i],"");
	}
	while ((colonna<=colonne_max) && (!isEOF)) {
		
		if ((rimanenti>=0) || (stringa[strlen(stringa)-1]=='\r')){
			strcat(stringa,parola);
			strncpy(parola,"",caratteri_max);
			lunghezzaSbyte=strlen(stringa);

			if (stringa[lunghezzaSbyte-1]=='\r'){ //caso particolare: la riga finisce con \r fine paragrafo
				stringa[lunghezzaSbyte-1]='\0';
				aggiungi_spazi(stringa,caratteri_max-len_char(stringa));
				strcat(p[riga],stringa);//concateno la stringa modificata

				if (colonna!=colonne_max){
					aggiungi_spazi(p[riga],dist_colonne);
				}

				riga++;     //passo alla prossima riga
				if(riga>righe_max){ 
					riga=0;
					colonna++;
				}

				if (riga!=0){//Dopo il \r trovo lo \n
					aggiungi_spazi(p[riga],caratteri_max);

					if (colonna!=colonne_max){
						aggiungi_spazi(p[riga],dist_colonne);
					}

					riga++;     //passo alla prossima riga
					if(riga>righe_max){ 
						riga=0;
						colonna++;
					}
				}
				strncpy(stringa,"",caratteri_max); //libero la stringa 
				rimanenti=caratteri_max;	
			}

			if (last[0]!='\0'){ //mi salvo l'ultima parola
				strcpy(parola,last);
				strcpy(last,"");
			}
			
			else{
				if (read(pipeLC,parola,sizeof(parola))==-1){//prendo la prossima parola dalla pipe
					fprintf(stderr,"Errore nella lettura pipeLC\n");
					if (kill(0, SIGTERM) == -1) {
     		    	   perror("Errore nell'invio del segnale SIGTERM\n");
        			   exit(1);
    				}
				}
			}

			if (strcmp(parola,"")==0){//Se arriva una parola "" quindi un EOF modifica il valore di guardia del while isEOF
				isEOF=1;
			}

			if ((strcmp(parola,"\r")==0) || (strcmp(parola,"\n")==0) || (strcmp(parola," ")==0)){
				parola[0]='\0';
			}
			
			lunghezzaPchar=len_char(parola);
			lunghezzaPbyte=strlen(parola);

			if ((parola[lunghezzaPbyte-1]==' ') && (lunghezzaPchar-1==rimanenti)){ //se la parola entra nello spazio rimanente senza considerare lo spazio finale elimino lo spazio
				parola[lunghezzaPbyte-1]='\0';
				lunghezzaPchar--;
			}
			rimanenti-=lunghezzaPchar;
		}

		else {
			
			occupati=len_char(stringa);
			lunghezzaSbyte=strlen(stringa);
			if (stringa[lunghezzaSbyte-1]==' '){ //se termina con uno spazio allora rimuovo lo spazio e diminuisco di 1 occupati
				stringa[lunghezzaSbyte-1]='\0';
				occupati--;
			}
			
			modifica_stringa(stringa,caratteri_max);
			strcat(p[riga],stringa);//concateno la stringa modificata

			if (colonna!=colonne_max){
					aggiungi_spazi(p[riga],dist_colonne);
				}
			strncpy(stringa,"",caratteri_max); //libero la stringa

			riga++;
			if (riga>righe_max){
				riga=0;
				colonna++;
			}	
			
			rimanenti=caratteri_max-len_char(parola); // Elimino la lunghezza della parola dopo

		}	
		
		if ((colonna>colonne_max) || (isEOF)){
			strcpy(last,parola);
		}
	}
	return isEOF;

}






void multiprocesso(char* nomeFile,int colonne_max,int righe_max,int caratteri_max,int dist_colonne){
	pid_t pid;
	int pipeLC[2];//Pipe che collega Lettura e Creazione Pagina
	int pipeCS[2]; //Pipe che collega Creazione pagina e Stampa
	
	if (pipe(pipeLC)==-1){
		fprintf(stderr,"ERRORE: creazione della pipeLC\n");
		exit(1);
	}

	if (pipe(pipeCS)==-1){
		fprintf(stderr,"ERORRE: creazione della pipeCS\n");
		exit(1);
	}
   
	stdin=fopen(nomeFile,"r");
	if (!stdin){
		fprintf(stderr,"ERRORE: File   non trovato\n");
		exit(1);
	}

	stdout=fopen("output.txt","w");
	if (!stdout){
		fprintf(stderr,"ERRORE: file di output\n");
		exit(1);
	} 
	
	pid=fork();

	if (pid<0){
		perror("Errore nella fork\n");
		exit(1);
	}

	if (pid>0){		//Processo padre
		pid_t pid2;
		pid2=fork();
		if (pid2<0){
			
			perror("Errore nella fork\n");
			if (kill(0, SIGTERM) == -1) {
     		    perror("Errore nell'invio del segnale SIGTERM\n");
        		exit(1);
			}
		}
		if (pid2>0){ //processo padre Leggi da file
			close(pipeLC[0]); //Chiude la lettura
			char parola[(caratteri_max+1)*4];
			
			while(strcmp(parola,"")!=0){ //Finche non raggiungo la fine del file inserisco parole in pipe
			    strcpy(parola,prossima_parola(stdin,caratteri_max)); //Finche non raggiungo la fine del file inserisco parole in pipe

				if (len_char(parola)>caratteri_max){
					fprintf(stderr,"La parola %s è troppo grande per lo spazio dato\n",parola);
					if (kill(0, SIGTERM) == -1) {
     		       			perror("Errore nell'invio del segnale SIGTERM\n");
        		  			 exit(1);
					}
							
					if (write(pipeLC[1],"",sizeof(""))==-1){
						perror("Errore nella lettura pipeLC\n");
						if (kill(0, SIGTERM) == -1) {
     		       			perror("Errore nell'invio del segnale SIGTERM\n");
        		  			 exit(1);
						}
					}
				}

				if(write(pipeLC[1],parola,sizeof(parola))==-1)
				{
					perror("Errore nella scrittura pipeLC\n");
					if (kill(0, SIGTERM) == -1) {
     				   perror("Errore nell'invio del segnale SIGTERM\n");
        			   exit(1);
    				}
				}
			}

			if (fclose(stdin)!=0){
				perror("Errore chiusura del file input\n");
				if (kill(0, SIGTERM) == -1) {
     		       perror("Errore nell'invio del segnale SIGTERM\n");
        		   exit(1);
				}
    		}
			close(pipeLC[1]);
		}

		if (pid2==0){ //Processo figlio Crea Pagina
			close(pipeLC[1]); //Chiude la scrittura Lettura-Creazione
			close(pipeCS[0]); //Chiude la lettura Creazione - Stampa
			int spazio=((colonne_max*caratteri_max)+(colonne_max-1)*caratteri_max+1)*4;
			int isEOF=0;
			char last[(caratteri_max+1)*4];  // l'ultima parola ottenuta prima della fine del ciclo 
			strcpy(last,"");

			char **p;//puntatore alla pagina da creare
			p=(char**) calloc(righe_max,sizeof(char *));
			for(int r=0;r<righe_max;r++){
				p[r]=(char*) calloc((colonne_max*caratteri_max)+(colonne_max-1)*caratteri_max+1,sizeof(char*)*4);
			}

			while(!isEOF){// esco dal while quando ho il valore EOF
				isEOF=crea_pagina(dist_colonne,caratteri_max,righe_max-1,colonne_max-1,p,pipeLC[0],last);			
				for (int r = 0; r < righe_max; r++) { //invia sulla pipe la pagina
					if (strcmp(p[r],"")==0){break;}
					if (write(pipeCS[1],p[r],spazio)==-1){
						perror("Errore nella scrittura pipeCS\n");
						if (kill(0, SIGTERM) == -1) {
     		     			perror("Errore nell'invio del segnale SIGTERM\n");
        		 			exit(1);
    					}
					}
				}	
			}

			if (write(pipeCS[1],"\rEOF",sizeof("\rEOF"))==-1){// scrivo sulla pipe un valore impossibile da ottenere tramite la funzione prossima parola
				perror("Errore nella scrittura pipeCS\n");
				if (kill(0, SIGTERM) == -1) {
     		       perror("Errore nell'invio del segnale SIGTERM\n");
        		   exit(1);
    			}
			} 

			close(pipeLC[0]);
			close(pipeCS[1]);

			for(int r=0;r<righe_max;r++){
				free(p[r]);
			}
			free(p);
		}
	}

	if (pid==0){//Processo figlio Stampa Pagina
		close (pipeCS[1]); //Chiude la scrittura Creazione - Stampa

		long spazio=((colonne_max*caratteri_max)+(colonne_max-1)*caratteri_max+1)*4;
		int isEOF=0;

		while(!isEOF){

			char **p;//puntatore alla pagina da stampare
			p=(char**) calloc(righe_max,sizeof(char *));
			for(int r=0;r<righe_max;r++){
				p[r]=(char*) calloc((colonne_max*caratteri_max)+(colonne_max-1)*caratteri_max+1,sizeof(char*)*4);

            	if ((read(pipeCS[0], p[r], spazio))==-1){
					perror("Errore nella lettura pipeLC\n");
					if (kill(0, SIGTERM) == -1) {
     		     	  perror("Errore nell'invio del segnale SIGTERM\n");
        		 	  exit(1);
    				}	
				}
				if (strcmp(p[r],"\rEOF")==0){
					strcpy(p[r],"");
					isEOF=1;
					break;
				}
			}

			stampa_pagina(stdout,p,righe_max);

			for(int r=0;r<righe_max;r++){
				free(p[r]);
			}
			free(p);
		}

		close (pipeCS[0]); 

		//Chiusura dei file	
		if (fclose(stdout)!=0){
			fprintf(stderr,"ERRORE: chiusura del file output\n");
		}
	}
}




int main(int argc,char *argv[]){
	//Dichiarazione e inizializzazione delle variabili
	char *nomefile = "";
	int colonne_max= -1;
	int righe_max= -1;
	int caratteri_max= -1;
	int dist_colonne= -1;
	char c;
    while ((c = getopt(argc, argv, "i:d:r:a:c:")) != -1) {
        switch (c) {
        case 'i':    /* file input */
			nomefile = (char *)malloc((strlen(optarg) + 1) * sizeof(char));
            strcpy(nomefile, optarg);
            break;
        case 'd':    /* distanza tra colonne */
            dist_colonne = atoi(optarg);
            break;
        case 'r':    /* righe per colonna */
            righe_max = atoi(optarg);
            break;
        case 'a': /* numero di caratteri per riga */
            caratteri_max = atoi(optarg);
			break;
        case 'c': /* numero di colonne */
            colonne_max = atoi(optarg);
			break;
        }
    }
	if ((dist_colonne==-1) || (righe_max==-1) || (caratteri_max==-1) || (dist_colonne==-1)){
		exit(1);
		fprintf(stderr,"ERRORE; inserimento dati errato\n");
	}
	
	multiprocesso(nomefile,colonne_max,righe_max,caratteri_max,dist_colonne);
	free(nomefile);
	return 0;
}
