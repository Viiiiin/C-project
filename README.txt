L' Homework è diviso in due file .c: Monoprocesso.c e Multiprocesso.c, è presente inoltre il file function.h al cui interno sono presenti funzioni usate in entrambe le versioni

Il Makefile permette di compilare i file.c:

	make monoprocesso -> compila il monoprocesso restituendo un file monoprocesso.out
	
	make multiprocesso -> compila il multiprocesso restituendo in file multiprocesso.out
	
	oppure
	
	make -> compila entrambi

Eseguire 
	make clean -> pulisce monoprocesso.out e multiprocesso.out


Per eseguire digitare il comando formato:

./nome -i char* -d int -r int -a int -c int

NB:l'ordine dei flag non è rilevante

Dove:
	nome: monoprocesso.out, multiprocesso.out
	-i  : nome del file di input
	-d  : distanza tra le colonne
	-r  : numero di righe per colonna
	-a  : caratteri per righa
	-c  : numero di colonne


Esempio:

./monoprocesso.out -i input.txt -d 4 -r 42 -a 25 -c 3 

./multiprocesso.out -i input.txt -d 4 -r 42 -a 25 -c 3

Creano/sovrascrivono il file output.txt
