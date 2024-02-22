#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Dato un puntatore a una stringa e un intero n, aggiunge spazi quanti n 
	param: puntatore a stringa, intero numero di spazi*/
void aggiungi_spazi(char *stringa,int n_spazi){ 
	  for (int i=0;i<n_spazi;i++){
			strcat(stringa," ");
		}
}
/* Ritorna il numero effettivo di caratteri occupati dalla stringa passata in input	
	param: puntatore a stringa*/
int len_char(char *s){ 
	int i=0;
	int c=0;
	int j=0;
	while(s[i]!='\0'){
		if ((s[i]>=0) ){
			i++;} 
		else{
			while((s[i]<0) && (j<4)){i++;
			j++;}	
			}
		c++;
		j=0;
		}
	return c;
}

/*Ritorna il numero di parola della stringa
param: puntatore a stringa,
	 intero massimo numero di caratteri per riga*/
int conta_parole(char *s,int caratteri_max){
 	int contatore = 0;
    char s_cpy[caratteri_max*4];
    char *token;
    
    strcpy(s_cpy,s); 
    token= strtok(s_cpy, " "); // divide la stringa in token separati da delim
    while (token != NULL) { // finché ci sono token nella stringa
        contatore++; // incrementa il contatore
        token = strtok(NULL, " "); // passa al token successivo
    }
    return contatore;
}


/*Ritorna il numero di spazi della stringa passata in input
  param:puntatore a stringa*/

int conta_spazi(char *s){ 
	int spazi=0;
	int lunghezza = strlen(s);
	for (int i=0;i<=lunghezza;i++){
		if ( s[i]==' '){
			spazi++;
		}
	}
	return spazi;
}

/* Accetta in ingresso un puntatore a una stringa e il numero di carattari massimo della stringa, 
	aggiunge spazi tra parole in modo tale da occupare tutti i caratteri
	param:puntatore a stringa, intero massimo numero di caratteri per riga*/

void modifica_stringa(char *s,int caratteri_max){
	int spazi_tra_parole,spazi; //spazi tra una parola e un'altra
	int resto; // resto della divisione per il calcolo di spazi_tra_parole
    int occupati=len_char(s);
	char s_cpy[caratteri_max*4];
    strcpy(s_cpy,"\0");
    strcpy(s_cpy,s); //Copio la stringa in input
    spazi=conta_spazi(s_cpy);
	int n_parole=conta_parole(s_cpy,caratteri_max);
	
	char *token =strtok(s_cpy, " "); // divido parola per parola
    
	if (n_parole<=1){//se stringa formata da una sola parola la concateno alla stringa vuota e aggiungo spazi ai posti rimanenti
		aggiungi_spazi(s,caratteri_max-occupati);
	}
	else{
        strcpy(s,""); //svuoto la stringa

		spazi_tra_parole= (caratteri_max-(occupati-spazi))/(n_parole-1);
		resto= (caratteri_max-(occupati-spazi))%(n_parole-1);


		while(token!=NULL){ //inserisco nella stringa parola per parola aggiungendo spazi_tra_parole per ogni parola
			strcat(s,token); 
			
			if ((resto!=0) && (resto==n_parole-1)){ //se la divisione da resto allora aggiungo uno spazio in piu alle ultime parole
				spazi_tra_parole++;
				
	            }
	        if (n_parole>1){ 
    
            aggiungi_spazi(s,spazi_tra_parole);
	        }
			n_parole--; 
			token=strtok(NULL," ");

		}
	}
}
/*Scorre la matrice passata in input riga per riga e la stampa sul file in output
  param: File di output, 
  		 matrice, 
		 intero massimo numero righe, 
		 intero massimo numero colonne, 
		 intero distanza massima
  return: 0 se il file è terminato */			
          

int stampa_pagina(FILE *f,char **p,int righe_max){
	char *delim="\n%%%\n";
	for (int r = 0; r < righe_max; r++) {
                fprintf(f,"%s",p[r]);
                if (strcmp(p[r],"")==0){// se trova una stringa vuota termina
                    return 0;
                }
				 fprintf(f,"\n"); // Va a capo alla fine di ogni riga
            }
	fprintf(f,"%s",delim);
	return 1;
	}
