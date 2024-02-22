#include <unistd.h>
#include "function.h"


/* Ritorna la parola successiva del file, acquisice caratteri finchè non incontra " (spazi) /r /n EOF"
   param: File in input da leggere, intero di massimo numero di caratteri per riga*/
char *prossima_parola(FILE *f,int caratteri_max){
	int spazio_max=(caratteri_max+1)*4; //Spazio massimo in una riga considerando UTF-8 da massimo 4byte
	char parola[spazio_max];
	char c='\0';
	strcpy(parola,"");

	while((c!=' ') && (c!='\r') && (c!='\n')  && (c!=EOF)){  
		c=fgetc(f);
		char temp[2]={c,'\0'};
		strcat(parola,temp);
		
	}
	return strdup(parola);
}




/*Crea la pagina da stampare, acquisisce parole/caratteri dal file in input e li inserisce in una stringha 
se il numero di caratteri-(lunghezza della parola successiva)>0 inserisce la parola e continua a acquisire parola
se l'ultimo caratteri inserito è \r (quindi fine paragrafo), richiama la funzione "modifica stringa(..)", concatena alla riga della matrice e lascia la riga successiva vuota 
altrimenti  richiama "modifica stringa(..)" e la concatena alla riga corrente della matrice

param:File in input da leggere, distanza tra le collone, massimo numero i caratteri per righe,massimo numero di righe, massimo numero di colonne, matrice dove memorizzare la pagine*/

void crea_pagina(FILE *f, int dist_colonne,int caratteri_max, int righe_max, int colonne_max,char **p){
	int riga=0, colonna=0;
	int rimanenti=caratteri_max; //Numero di caratteri disponibili nella stringa
	int occupati=0; //numero di caratteri occupati
	int lunghezzaSbyte; //lunghezza stringa in byte
	int lunghezzaPchar,lunghezzaPbyte; //RIspettivamente lunghezza parola in numero di caratteri e byte
	char parola[(caratteri_max+1)*4];
	char stringa[caratteri_max*4];//Ogni volta che aumento la riga libero la stringa
	strcpy(stringa,""); //libero la stringa
	strcpy(parola,""); //libero la stringa

	for (int i=0; i<=righe_max;i++){ //libero le stringhe in p
		strcpy(p[i],"");
	}
	while ((colonna<=colonne_max) && (!feof(f))) {
		
		if ((rimanenti>=0) || (stringa[strlen(stringa)-1]=='\r')){
			strcat(stringa,parola); //concateno la parola
			strncpy(parola,"",caratteri_max); //Svuoto parola
			lunghezzaSbyte=strlen(stringa);

			if (stringa[lunghezzaSbyte-1]=='\r'){ //caso particolare: la riga finisce con \r fine paragrafo
				stringa[lunghezzaSbyte-1]='\0';
				aggiungi_spazi(stringa,caratteri_max-len_char(stringa));
				strcat(p[riga],stringa);//concateno la stringa modificata

				if (colonna!=colonne_max){ //se non mi trovo all'ultima colonna non inserisco spazi per distanziare le colonne
					aggiungi_spazi(p[riga],dist_colonne);
				}

				riga++;     // Passo alla prossima riga controllando che non vada oltre le righe massime, altrimenti torna alla prima riga della prossima colonna
				if(riga>righe_max){ 
					riga=0;
					colonna++;
				}
				
				if (riga!=0){//Dopo il \r devo lasciare una riga vuota(solo spazi), solo se non è la prima riga della colonna
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
			
			strcpy(parola,prossima_parola(f,caratteri_max));//prendo la prossima parola
			if ((strcmp(parola,"\r")==0) || (strcmp(parola,"\n")==0) || (strcmp(parola," ")==0)){ //se è una stringa formata da solo \r o \n o " " libero la stringa
				parola[0]='\0';
			}

			lunghezzaPchar=len_char(parola);
			lunghezzaPbyte=strlen(parola);

			if ((parola[lunghezzaPbyte-1]==' ') && (lunghezzaPchar-1==rimanenti)){ //se la parola entra nello spazio rimanente senza considerare lo spazio dopo elimino lo spazio
				parola[lunghezzaPbyte-1]='\0';
				lunghezzaPchar--;
			}
			
			if (lunghezzaPchar>caratteri_max){
				printf("La parola %s è troppo grande per lo spazio dato\n",parola);
				return exit(1);
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
		if (colonna>colonne_max){
			fseek(f,-strlen(parola),SEEK_CUR); //Se mi trovo all'ultima riga dell'ultima colonna torno indietro nel file di byte pari a parola
		}
	}
}





void monoprocesso(char* nomeFile,int colonne_max,int righe_max,int caratteri_max,int dist_colonne){

	char **p;//puntatore alla pagina da creare e stampare
	p=(char**) calloc(righe_max,sizeof(char *)); //allocazione dello spazio
	for(int r=0;r<righe_max;r++){
	    p[r]=(char*) calloc((colonne_max*caratteri_max)+(colonne_max-1)*caratteri_max+1,sizeof(char*)*4);
	}

	//Apertura/Creazione dei file e controllo
	stdin=fopen(nomeFile,"r");
	if (!stdin){
		fprintf(stderr,"ERRORE: File  non trovato\n");
		exit(1);
	 
	}

	stdout=fopen("output.txt","w");
	if (!stdout){
		  fprintf(stderr,"ERRORE: file di output\n");
		  exit(1);
	}

	while(!feof(stdin)){
		crea_pagina(stdin,dist_colonne,caratteri_max,righe_max-1,colonne_max-1,p);
		stampa_pagina(stdout,p,righe_max);
    }
	
	//Chiusura dei file
	if (fclose(stdin)!=0){
		fprintf(stderr,"ERRORE: chiusura del file input\n");
	}

	if (fclose(stdout)!=0){
		fprintf(stderr,"ERRORE: chiusura del file output\n");
	}
	
	for(int r=0;r<righe_max;r++){ //Libero la memoria allocata
	   free(p[r]);
	}
	free(p);
}   


int main(int argc,char *argv[]){
	
	//Dichiarazione e inizializzazione delle variabili
	char *nomefile = "";
	int colonne_max= -1;
	int righe_max= -1;
	int caratteri_max=-1;
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
		fprintf(stderr,"ERRORE: inserimento dati errato\n");
		exit(1);
	}
	
	monoprocesso(nomefile,colonne_max,righe_max,caratteri_max,dist_colonne);
	free(nomefile);
	return 0;
}
