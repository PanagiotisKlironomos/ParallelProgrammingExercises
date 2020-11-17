/* Copyright (C) 2019 Panagiotis Klironomos - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU GENERAL PUBLIC LICENSE 
 */
/* mpicc -o e2 e2.c */
/* mpirun -np 4 ./e2 */
/* mpirun -np 3 ./e2 */
/* mpirun -np 2 ./e2 */
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char** argv)
{
	/* μεταβλητές λειτουργικότητας MPI */
	/* MPI functionality variables */
	int rankID,processorsNum,namelen;
	char proc_name[MPI_MAX_PROCESSOR_NAME];
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rankID);
	MPI_Comm_size(MPI_COMM_WORLD, &processorsNum);
	MPI_Datatype data_type;
	MPI_Get_processor_name(proc_name, &namelen);
	MPI_Status status;
	/* κοινές μεταβλητές */
	/* common variables */
	int amountOfNumbers,i,j,dutyNum,choice=5;
	double *loc_A , *loc_B , *loc_C , *loc_D, *loc_res;
	/* μεταβλητές που χρησιμοποιεί μόνο ο processor 0 */
	/* variables used only by processor 0 */
	double * A, *B, *C, *D;
	double *Sum, *Mul_CwithB_Col, *Mul_A_rowB_Col, *MulCD;
	double sum;
	char sChoice[10];
	int scanAmountOfNumbers(int processorsNum);
	int scanPEqualAmountOfNumbers(int processorsNum) ;
	int scanAmountOfNumbersNoConstraints(int processorsNum) ;
	void allocateDouble2DSquareMatrix(int amountOfNumbers, double** matrix);
	void allocateDoubleMatrix(int size, double** matrix);
	void fillDoubleRowMatrix(int amountOfNumbers,double* matrix,char name);
	void fillDoubleColumnMatrix(int amountOfNumbers,double* matrix,char name);
	void fillDouble2DSquareMatrix(int amountOfNumbers,double* matrix,char name);
	void printDoubleRowMatrix(int amountOfNumbers,double* matrix,char name);
	void printDoubleColumnMatrix(int amountOfNumbers,double* matrix,char name);
	void printDouble2DSquareMatrix(int amountOfNumbers,double* matrix,char name);
	void calculateSum (int rankID, int amountOfNumbers, int processorsNum, int dutyNum, double* C, double* D, double *Sum, double *loc_C, double *loc_D, double *loc_res);
	void calculateMul_CwithB_Col (int rankID, int amountOfNumbers, int processorsNum, int dutyNum, double* C, double* B, double *Mul_CwithB_Col, double *loc_C,double *loc_res);
	void calculateMul_A_rowB_Col (int rankID, int amountOfNumbers, int processorsNum, int dutyNum, double* A, double* B, double *Mul_A_rowB_Col, double *loc_A,double *loc_B , double sum);
	void calculateMulCwithD (int rankID, int amountOfNumbers, int processorsNum, int dutyNum, double* C, double* D, double *MulCD, double *loc_C,double *loc_D, double *loc_res, MPI_Status status);

	do{/* Λούπα ελέγχου εξόδου */
		/* Exit control loop */

		if (rankID == 0)
		{
			do{/* Εισαγωγή επιλογής Ν με έλεγχο */
				/*controlled N input   */
				system ("clear");
				printf("\nΕισάγετε μία από τις παρακάτω επιλογές μενού\nEnter one of the following menu options\n");
				printf("\n\t 1. (Άθροισμα δύο δισδιάστατων πινάκων C(NxN) + D(NxN)  \n \t    (Sum of two two-dimensional arrays C(NxN)+D(NxN))\n");
				printf("\n\t 2. (Πολ/μός δισδιάστατου πίνακα με διάνυσμα-στήλη C(NxΝ)*Β(Νx1)) \n\t    (two-dimensional array multiplication with vector-column C(NxN)*B(Nx1))\n");
				printf("\n\t 3. (Πολ/μός διανύσματος-γραμμής με διάνυσμα-στήλη Α(1xΝ)*Β(Νx1)) \n\t    (Vector-line multiplication with vector-column A(1xN)*B(Nx1))\n");
				printf("\n\t 4. (Πολ/μός δυσδιάστατων πινάκων C(NxN) * D(NxN) ) \n\t    (two-dimensional arrays multiplication C(NxN)*D(NxN))\n");
				printf("\n\t 5. Έξοδος\n\t    Exit\n");
				scanf("%s",sChoice);
				choice = atoi(sChoice);
			}while (choice < 1 || choice > 5);
		}
		/* Broadcast από τον proc. 0 σε όλες τις διεργασίες το choice */
		/* proc. 0 Broadcasts choice   to all other processes*/
		MPI_Bcast(&choice, 1, MPI_INT, 0, MPI_COMM_WORLD);

		switch(choice)
		{
			case 1: calculateSum (rankID,amountOfNumbers,processorsNum,dutyNum,C,D,Sum,loc_C,loc_D, loc_res);  break;
			case 2: calculateMul_CwithB_Col (rankID,amountOfNumbers,processorsNum,dutyNum, C, B,Mul_CwithB_Col, loc_C,loc_res);	break;
			case 3: calculateMul_A_rowB_Col (rankID,amountOfNumbers, processorsNum,  dutyNum,  A, B, Mul_A_rowB_Col, loc_A, loc_B, sum); break;
			case 4: calculateMulCwithD (rankID,amountOfNumbers, processorsNum,  dutyNum, C, D, MulCD, loc_C,loc_D, loc_res, status); break;

			case 5:   break;
		}

	} while (choice !=5 );
	MPI_Finalize();
	return 0;
}

/********** Συναρτήσεις **********/

int scanAmountOfNumbers(int processorsNum) {
	int amountOfNumbers;
	char sChoice[10];
	do {  /* Εισαγωγή πλήθους Ν με έλεγχο */  /* controlled input of  N number*/
		system("clear");
		printf("Δώσε πλήθος αριθμών (N) \n Give number of numbers (N)\n");
		if ( (amountOfNumbers%processorsNum) !=0)
			printf ("Το Ν πρέπει να είναι ακέραιο πολλαπλάσιο των %d επεξεργαστών\nN must be an integer multiple of %d processors\n",processorsNum,processorsNum);
		scanf("%s",sChoice);
		amountOfNumbers = atoi(sChoice);
	}while (amountOfNumbers <= 0 || (amountOfNumbers%processorsNum) !=0);
	return amountOfNumbers;
}

int scanPEqualAmountOfNumbers(int processorsNum) {
	int amountOfNumbers;
	char sChoice[10];
	do {  /* Εισαγωγή πλήθους Ν με έλεγχο */ /* controlled input of  N number*/
		system("clear");
		printf("Δώσε πλήθος αριθμών (N) \n Give number of numbers (N)\n");
		if ( amountOfNumbers != processorsNum )
			printf ("Το Ν πρέπει να είναι ίσο με τους  %d επεξεργαστές \nN must be equal to %d processors \n",processorsNum,processorsNum);
		scanf("%s",sChoice);
		amountOfNumbers = atoi(sChoice);
	}while (amountOfNumbers != processorsNum);
	return amountOfNumbers;
}

int scanAmountOfNumbersNoConstraints(int processorsNum) {
	int amountOfNumbers;
	char sChoice[10];
	do {  /* Εισαγωγή πλήθους Ν με έλεγχο */  /* controlled input of  N number*/
		system("clear");
		printf("Δώσε πλήθος αριθμών (N) \n Give number of numbers (N)\n");
		scanf("%s",sChoice);
		amountOfNumbers = atoi(sChoice);
	}while (amountOfNumbers <= 0);
	return amountOfNumbers;
}

void allocateDoubleMatrix(int size, double** matrix)
{
    *matrix = (double *)malloc(size * sizeof(double));
    if (matrix==NULL) {
		printf("Δεν ήταν δυνατή η εκχώρηση μνήμης \n Could not allocate memory \n");
		exit(1);
    }
}

void allocateDouble2DSquareMatrix(int amountOfNumbers, double** matrix)
{
    *matrix = (double *)malloc(amountOfNumbers*amountOfNumbers * sizeof(double));
    if (matrix==NULL) {
		printf("Δεν ήταν δυνατή η εκχώρηση μνήμης \n Could not allocate memory \n");
		exit(1);
    }
}

void fillDoubleRowMatrix(int amountOfNumbers,double* matrix,char name)
{
	int j;
    for(j=0; j<amountOfNumbers; j++){
		do{
			printf("Δώσε το περιεχόμενο του %c[0][%d] : \nGive the content of %c[0][%d] : \n",name,j,name,j);
			while(getchar() != '\n');
		}while(scanf("%lf",&matrix[j])!=1);
    }
}

void fillDoubleColumnMatrix(int amountOfNumbers,double* matrix,char name)
{
	int j;
    for(j=0; j<amountOfNumbers; j++){
		do{
			printf("Δώσε το περιεχόμενο του %c[%d][0] : \nGive the content of %c[%d][0] : \n",name,j,name,j);
			while(getchar() != '\n');
		}while(scanf("%lf",&matrix[j])!=1);
    }
}

void fillDouble2DSquareMatrix(int amountOfNumbers,double* matrix,char name)
{
	int i,j;
	for(i=0; i<amountOfNumbers; i++){
		for(j=0; j<amountOfNumbers; j++){
			do{
				printf("Δώσε το περιεχόμενο του %c[%d][%d] : \nGive the content of %c[%d][%d] : \n",name,i,j,name,i,j);
				while(getchar() != '\n');
			}while(scanf("%lf",&matrix[i*amountOfNumbers+j])!=1);
		}
	}
}

void printDoubleRowMatrix(int amountOfNumbers,double* matrix,char name)
{
	int j;
	printf("\n     Πίνακας %c \n Matrix %c\n",name,name);
    for(j=0; j<amountOfNumbers; j++){
		printf("%c[0][%d]=%5.2lf ",name,j,matrix[j]);
    }
	printf("\n");
}

void printDoubleColumnMatrix(int amountOfNumbers,double* matrix,char name)
{
	int j;
	printf("\n     Πίνακας %c \n Matrix %c\n",name,name);
    for(j=0; j<amountOfNumbers; j++){
		printf("%c[%d][0]=%5.2lf ",name,j,matrix[j]);
		printf("\n");
    }
	printf("\n");
}
void printDouble2DSquareMatrix(int amountOfNumbers,double* matrix,char name)
{
	int i,j;
	printf("\n     Πίνακας %c \n Matrix %c\n",name,name);
	for(i=0; i<amountOfNumbers; i++){
		for(j=0; j<amountOfNumbers; j++){
			printf("%c[%d][%d]=%5.2lf ",name,i,j,matrix[i*amountOfNumbers+j]);
		}
		printf("\n");
	}
	printf("\n");
}

void calculateSum (int rankID, int amountOfNumbers, int processorsNum, int dutyNum, double* C, double* D, double *Sum, double *loc_C, double *loc_D, double *loc_res)
{
	int i,j;
	if (rankID==0){
		amountOfNumbers = scanAmountOfNumbers(processorsNum);
		/* allocate Πινακα C */ /* allocate C Matrix */
		allocateDouble2DSquareMatrix(amountOfNumbers, &C);
		/* allocate Πινακα D */ /* allocate D Matrix */
		allocateDouble2DSquareMatrix(amountOfNumbers, &D);
		/* allocate Πινακα Sum */ /* allocate Sum Matrix */
		allocateDouble2DSquareMatrix(amountOfNumbers, &Sum);
		/*Υπολογισμός πλήθους που αντιστοιχεί σε κάθε processor */
		/* Calculate the number corresponding to each processor */
		dutyNum=amountOfNumbers*amountOfNumbers/processorsNum;
		/* Γέμισμα Πινάκων */ /* Filling Matrices */
		fillDouble2DSquareMatrix(amountOfNumbers, C,'C');
		fillDouble2DSquareMatrix(amountOfNumbers, D,'D');
		/* Εκτύπωση Πινάκων */ /* PrintingMatrices */
		printDouble2DSquareMatrix(amountOfNumbers, C,'C');
		printDouble2DSquareMatrix(amountOfNumbers, D,'D');
	}
	/* Broadcast από τον proc. 0 σε όλες τις διεργασίες το amountOfNumbers */
	/*proc 0 Broadcasts amountOfNumbers to all other processes*/
	MPI_Bcast(&amountOfNumbers, 1, MPI_INT, 0, MPI_COMM_WORLD);
	/*και το dutyNum */
	/*as well as dutyNum */
	MPI_Bcast(&dutyNum, 1, MPI_INT, 0, MPI_COMM_WORLD);
	/* allocate τοπικού Πινακα C */ /* allocate local C Matrix */
	allocateDoubleMatrix(dutyNum, &loc_C);
	/* allocate τοπικού Πινακα D */ /* allocate local D Matrix */
	allocateDoubleMatrix(dutyNum, &loc_D);
	/* allocate τοπικού Πινακα loc_res */ /* allocate local loc_res Matrix */
	allocateDoubleMatrix(dutyNum, &loc_res);
	/* Scatter Πινάκων C και D στους αντίστοιχους τοπικούς loc_C, loc_D */
	/* Scatter C and D matrices into the corresponding local loc_C, loc_D  matrices*/
	MPI_Scatter(C, dutyNum, MPI_DOUBLE, loc_C, dutyNum, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(D, dutyNum, MPI_DOUBLE, loc_D, dutyNum, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	/*εκχώρηση στο loc_res[i] κάθε διεργασίας */
	/*το αντίστοιχο άθροισμα loc_C[i] + loc_D[i] */
	/* assign to loc_res [i] of each process */
	/* the corresponding sum loc_C [i] + loc_D [i] */
	for(i=0; i<dutyNum; i++)
		loc_res[i]=loc_C[i] + loc_D[i];
	/* Δοκιμαστική εκτύπωση loc_res ΚΑΘΕ processor */
	/* Test print loc_res for EVERY processor */
	for(i=0; i<dutyNum/amountOfNumbers; i++){
		for(j=0; j<amountOfNumbers; j++){
			printf("P%d[%d][%d]= %5.2lf  ",rankID,i,j,loc_res[i*(dutyNum/amountOfNumbers)+j] );
		}
		printf("\n");
	}
	/* Gather Πινάκων loc_res στον πίνακα Sum */
	/*Gather loc_res matrices to Sum Matrix*/
	MPI_Gather(loc_res,dutyNum,MPI_DOUBLE,Sum,dutyNum,MPI_DOUBLE,0,MPI_COMM_WORLD);
	/* Eκτύπωση Sum */ /* Print Sum */
	if (rankID==0){
		printDouble2DSquareMatrix(amountOfNumbers, Sum,'S');
	}

	free (loc_C);
	free (loc_D);
	free (loc_res);
	if (rankID==0){
		free (C);
		free (D);
		printf("\n Πιέστε enter για συνέχεια \n");
		system("/bin/bash -c 'read -sn1'"); /*system("pause");*/
	}
}


void calculateMul_CwithB_Col (int rankID, int amountOfNumbers, int processorsNum, int dutyNum, double* C, double* B, double *Mul_CwithB_Col, double *loc_C,double *loc_res)
{
	int i,j;
	int *  sendcounts, * displs, *recvcounts, *displs1;
	sendcounts = (int * )malloc (processorsNum * sizeof(int));
	recvcounts = (int * )malloc (processorsNum * sizeof(int));
	displs = (int * )malloc (processorsNum * sizeof(int));
	displs1 = (int * )malloc (processorsNum * sizeof(int));
	/*Για να δουλέψει η reduce με το MAXLOC χρειάζεται αυτό το struct */
	/*για να βάλει τα δεδομένα*/
	/* In order for reduce to work with MAXLOC  this struct is needed*/
	/* for data entry */
	struct {
		double value;
		int   index;
	}max , rootmax;
	if (rankID==0){
		amountOfNumbers = scanAmountOfNumbersNoConstraints(processorsNum);
		/* allocate Πινακα C */ /*allocate C Matrix*/
		allocateDouble2DSquareMatrix(amountOfNumbers, &C);
	}
	/* Broadcast από τον proc. 0 σε όλες τις διεργασίες το amountOfNumbers */
	/*proc0 broadcasts amountOfNumbers to all other processes*/
	MPI_Bcast(&amountOfNumbers, 1, MPI_INT, 0, MPI_COMM_WORLD);
	/* allocate Πινακα B  από όλες τις διεργασίες*/
	/* allocate B Matrix for all other processes */
	allocateDoubleMatrix(amountOfNumbers, &B);
	if (rankID==0){
		/* allocate Πινακα Mul_CwithB_Col */ /* allocate Mul_CwithB_Col Matrix*/
		allocateDoubleMatrix(amountOfNumbers, &Mul_CwithB_Col);
		/*Υπολογισμός πλήθους που αντιστοιχεί σε κάθε processor */
		/* Calculate the number corresponding to each processor */
		dutyNum=amountOfNumbers/processorsNum;
		/* Υπολογισμός φόρτου κάθε διεργασίας */
		/* Μεταβλήτές που χρησιμοποιούνται μόνο για υπολογισμό start,end ανά process*/
		/* Load calculation of each process */
		/* Variables used only for start, end per process calculation */
		int duty = amountOfNumbers /processorsNum;
		int dutyNumModulo = amountOfNumbers %processorsNum;
		int start,end, processNum;
		/* Υπολογισμός start , end για τα processes εκτός της process 0*/
		/* Start, end calculation for processes outside process 0 */
		for(processNum=1; processNum<processorsNum; processNum++){
			if (processNum < dutyNumModulo) {
			/*Τα processes που τους αντιστοιχεί μέρος του υπολοίπου	*/
			/* παίρνουν duty + 1 νούμερα το καθένα*/
			/* The processes corresponding to part of the rest */
			/* get duty + 1 numbers each*/
				start = processNum * (duty + 1);
				end = start + duty;
			} else {
			/*Στα υπόλοιπα processes αντιστοιχούν dutyNum αριθμοι αλλά */
			/* μετατοπισμένοι κατά  υπόλοιπο (dutyModulo) θέσεις πίνακα */
			/* The rest of the processes correspond to dutyNum numbers but */
			/* shifted by modulo (dutyModulo) table positions */
				start = processNum * duty + dutyNumModulo;
				end = start + (duty - 1);
			}
			dutyNum= end-start+1;
			sendcounts[processNum]= dutyNum*amountOfNumbers;
			displs[processNum]= start*amountOfNumbers;
			recvcounts[processNum]= dutyNum;
			displs1[processNum]= start;
		}
		/*Υπολογισμός start, end για την root process*/
		/* Start, end calculation for the root process */
		start = 0;
		if (0 < dutyNumModulo) {
			end = duty;
		} else {
	    		end = duty - 1;
		}
		dutyNum= end+1;
		sendcounts[0]= dutyNum*amountOfNumbers;
		displs[0]= start*amountOfNumbers;
		recvcounts[0]= dutyNum;
		displs1[0]= start;


		/* Γέμισμα Πινάκων */ /*Filling Matrices*/
		fillDouble2DSquareMatrix(amountOfNumbers, C,'C');
		fillDoubleColumnMatrix(amountOfNumbers, B,'B');
		/* Εκτύπωση Πινάκων */ /*Printing Matrices*/
		printDouble2DSquareMatrix(amountOfNumbers, C,'C');
		printDoubleColumnMatrix(amountOfNumbers, B,'B');
	}

	/*Broadcast από τον proc. 0 του πίνακα Β */
	/*proc0 broadcasts B matrix*/
	MPI_Bcast( B,amountOfNumbers,MPI_DOUBLE,0,MPI_COMM_WORLD);
	MPI_Bcast( sendcounts,processorsNum,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast( displs,processorsNum,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast( recvcounts,processorsNum,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast( displs1,processorsNum,MPI_INT,0,MPI_COMM_WORLD);

	/* allocate τοπικού Πινακα C */  /* allocate local C Matrix */ 
	allocateDoubleMatrix(sendcounts[rankID] * amountOfNumbers, &loc_C);
	/* allocate τοπικού Πινακα loc_res */  /* allocate local loc_res Matrix */ 
	allocateDoubleMatrix( amountOfNumbers , &loc_res);



	/* Scatterv Πινάκα C στον αντίστοιχους τοπικούς loc_C */
	/* Scatterv C Matrix to corresponding local loc_C matrices*/
	MPI_Scatterv(C,  sendcounts, displs, MPI_DOUBLE, loc_C, sendcounts[rankID], MPI_DOUBLE, 0, MPI_COMM_WORLD);

	/*εκχώρηση στο loc_res[i] κάθε διεργασίας */
	/*το αντίστοιχο γινόμενο loc_C[γραμμή] * B[στήλη] */
	/* assign to loc_res [i] of each process */
	/* the corresponding product loc_C[row]*B[column] */
	for(i=0; i<sendcounts[rankID]/amountOfNumbers ; i++)
	{
		for(j=0; j<amountOfNumbers; j++){
			/* νιοστό κελί τελικού μονοδιάστατου πίνακα στήλη =         */
			/* το άθροισμα των γινομένων  των κελιών νιοστής γραμμής πίνακα C */
			/* με τα αντίστοιχης τάξης κελιά του πίνακα στήλη Β         */
			/* loc_res[0] = loc_C[0][0]*B[0] + loc_C[0][1]*B[1] + loc_C[0][2]*B[2]..*/
			/* loc_res[1] = loc_C[1][0]*B[0] + loc_C[1][1]*B[1] + loc_C[1][2]*B[2]..*/
			/* n cell of final one-dimensional column matrix =  */
			/*the sum of the products of the n row cells  of C Matrix */
			/* with the corresponding order cells in the column table B */
			loc_res[i]+=loc_C[(i*amountOfNumbers) +j] * B[j];
			/* Δοκιμαστική εκτύπωση loc_res[i] ΚΑΘΕ processor  για κάθε j επανάληψη*/
			/* Test print loc_res [i] for EVERY processor for each j iteration */
			//printf("P%d%dloc_res[%d]+=%5.2lf * %5.2lf \n",rankID,sendcounts[rankID],i,loc_C[(i*amountOfNumbers) +j], B[j] );
		}
	}

	/* Gatherv Πινάκων loc_res στον πίνακα Mul_CwithB_Col, */
	/*Gatherv loc_res Matrices to Mul_CwithB_Col matrix*/
	MPI_Gatherv(loc_res,recvcounts[rankID],MPI_DOUBLE,Mul_CwithB_Col,recvcounts,displs1,MPI_DOUBLE,0,MPI_COMM_WORLD);
	/*Κάθε διεργασία υπολογίζει το δικό της max*/
	/*every process calculates its own max*/
	for(i=0; i<sendcounts[rankID]/amountOfNumbers ; i++)
	{
		if (loc_res[i] > max.value) {
			max.value=loc_res[i] ;
			max.index=  i ;
			for (j=0; j<rankID;j++){
				max.index+=sendcounts[j]/amountOfNumbers;
			}
		}
	}

	/* Reduce του max καθε processor στην max μεταβλητή του processor 0*/
	/* reduce of processor's max to max variable of processor0*/
	MPI_Reduce(&max, &rootmax, 1, MPI_DOUBLE_INT, MPI_MAXLOC, 0, MPI_COMM_WORLD);
	/* Eκτύπωση Mul_CwithB_Col  και rootmax*/
	/* Mul_CwithB_Col  and rootmax print*/
	if (rankID==0){
		printDoubleColumnMatrix(amountOfNumbers,Mul_CwithB_Col,'M');
		printf("Το μέγιστο στοιχείο του πίνακα είναι το %5.2lf και είναι στην θέση [%d][0] του πίνακα Μ\nThe max element of the matrix is %5.2lf  and its located in [%d][0] position of Μ matrix \n", rootmax.value, rootmax.index, rootmax.value, rootmax.index);
	}

	free (loc_C);
	free (B);
	free (loc_res);
	free (sendcounts);
	free(displs);
	if (rankID==0){
		free (C);
		printf("\n Πιέστε enter για συνέχεια \n Press enter to continue\n");
		system("/bin/bash -c 'read -sn1'"); /*system("pause");*/
	}
}

void calculateMul_A_rowB_Col (int rankID, int amountOfNumbers, int processorsNum, int dutyNum, double* A, double* B, double *Mul_A_rowB_Col, double *loc_A,double *loc_B, double sum)
{
	int i,j;
	if (rankID==0){
		amountOfNumbers = scanAmountOfNumbers(processorsNum);
		/* allocate Πινακα A */ /*allocate A Matrix*/
		allocateDoubleMatrix(amountOfNumbers, &A);
		/* allocate Πινακα B */ /*allocate A Matrix*/
		allocateDoubleMatrix(amountOfNumbers, &B);
		/* allocate Πινακα Mul_A_rowB_Col */ /*allocate Mul_A_rowB_Col Matrix*/
		allocateDoubleMatrix(1, &Mul_A_rowB_Col);
		/*Υπολογισμός πλήθους που αντιστοιχεί σε κάθε processor */
		/* Calculate the number corresponding to each processor */
		dutyNum=amountOfNumbers/processorsNum;
		/* Γέμισμα Πινάκων */ /*Filling Matrices*/
		fillDoubleRowMatrix(amountOfNumbers, A,'A');
		fillDoubleColumnMatrix(amountOfNumbers, B,'B');
		/* Εκτύπωση Πινάκων */ /*Printing Matrices*/
		printDoubleRowMatrix(amountOfNumbers, A,'A');
		printDoubleColumnMatrix(amountOfNumbers, B,'B');
	}
	/*Broadcast από τον proc. 0 του dutyNum*/
	/*proc0 broadcasts dutyNum*/
	MPI_Bcast(&dutyNum, 1, MPI_INT, 0, MPI_COMM_WORLD);
	/* allocate Πινακα loc_A */ /*allocate loc_A Matrix*/
	allocateDoubleMatrix(dutyNum, &loc_A);
	/* allocate Πινακα loc_B */ /*allocate loc_B Matrix*/
	allocateDoubleMatrix(dutyNum, &loc_B);
	/* Scatter Πινάκα A στους αντίστοιχους τοπικούς loc_Α */
	/*Scatter A Matrix to corresponding local loc_A matrices*/
	MPI_Scatter(A, dutyNum, MPI_DOUBLE, loc_A, dutyNum, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	/* Scatter Πινάκα Β στους αντίστοιχους τοπικούς loc_Β */
	/*Scatter B Matrix to corresponding local loc_B matrices*/
	MPI_Scatter(B, dutyNum, MPI_DOUBLE, loc_B, dutyNum, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	/* Υπολογισμός γινομένου A[i]*B[i] και άθροιση στο sum */
	/* Product calculation A[i]*B[i] and sum in sum */
	for (i=0,sum=0;i<dutyNum; i++)
	{
		loc_A[i]*= loc_B[i];
		sum+=loc_A[i];
		/* Δοκιμαστική εκτύπωση loc_A (αποτελέσματος) ΚΑΘΕ processor */
		/* Test print loc_A (result) for EVERY processor */
		//printf("P%d[%d]= %5.2lf  ",rankID,i,loc_A[i] );
	}
	/* Reduce του sum καθε processor στo Mul_A_rowB_col του processor 0*/
	/* Sum reduce of every processor to  processor0 Mul_A_rowB_col */
	MPI_Reduce(&sum, Mul_A_rowB_Col, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rankID==0){
		printDoubleRowMatrix(1, Mul_A_rowB_Col,'M');
	}

	free(loc_B);
	free(loc_A);
	if (rankID==0){
		free(Mul_A_rowB_Col);
		free (A);
		free (B);
		printf("\n Πιέστε enter για συνέχεια \nPress enter to continue \n");
		system("/bin/bash -c 'read -sn1'"); /*system("pause");*/
	}
}

void calculateMulCwithD (int rankID, int amountOfNumbers, int processorsNum, int dutyNum, double* C, double* D, double *MulCD, double *loc_C,double *loc_D, double *loc_res, MPI_Status status)
{
	int i,j,a,flag=0;
	if (rankID==0){
		amountOfNumbers = scanPEqualAmountOfNumbers(processorsNum);
		/* allocate Πινακα C */ /*allocate C Matrix*/
		allocateDouble2DSquareMatrix(amountOfNumbers, &C);
		/* allocate Πινακα D */ /*allocate D Matrix*/
		allocateDouble2DSquareMatrix(amountOfNumbers, &D);
		/* allocate Πινακα Sum */ /*allocate Sum Matrix*/
		allocateDouble2DSquareMatrix(amountOfNumbers, &MulCD);
		/*Υπολογισμός πλήθους που αντιστοιχεί σε κάθε processor */
		/* Calculate the number corresponding to each processor */
		dutyNum=amountOfNumbers/processorsNum;
		/* Γέμισμα Πινάκων */ /*Filling Matrices*/
		fillDouble2DSquareMatrix(amountOfNumbers, C,'C');
		fillDouble2DSquareMatrix(amountOfNumbers, D,'D');
		/* Εκτύπωση Πινάκων */ /*Printing Matrices*/
		printDouble2DSquareMatrix(amountOfNumbers, C,'C');
		printDouble2DSquareMatrix(amountOfNumbers, D,'D');
	}
	/* Broadcast από τον proc. 0 σε όλες τις διεργασίες το amountOfNumbers */
	/*proc0 broadcasts amountOfNumber to all other processes*/
	MPI_Bcast(&amountOfNumbers, 1, MPI_INT, 0, MPI_COMM_WORLD);
	/*και το dutyNum */ /*as well as dutyNum*/
	MPI_Bcast(&dutyNum, 1, MPI_INT, 0, MPI_COMM_WORLD);
	/* allocate τοπικού Πινακα C */ /*allocate local C Matrix*/
	allocateDoubleMatrix(dutyNum*amountOfNumbers, &loc_C);
	/* allocate τοπικού Πινακα D */ /*allocate local D Matrix*/
	allocateDoubleMatrix(dutyNum*amountOfNumbers, &loc_D);
	/* allocate τοπικού Πινακα loc_res */ /*allocate local loc_res Matrix*/
	allocateDoubleMatrix(dutyNum*amountOfNumbers, &loc_res);
	/* Scatter Πινάκων C και D στους αντίστοιχους τοπικούς loc_C, loc_D */
	/*Scatter C and D Matrices to corresponding local loc_C and loc_D matrices*/
	MPI_Scatter(C, (dutyNum*amountOfNumbers), MPI_DOUBLE, loc_C, (dutyNum*amountOfNumbers), MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(D, (dutyNum*amountOfNumbers), MPI_DOUBLE, loc_D, (dutyNum*amountOfNumbers), MPI_DOUBLE, 0, MPI_COMM_WORLD);

	for(i=0; i<amountOfNumbers; i++)
		loc_res[i]=0;
	/* Ο κάθε επεξεργαστής έχει μία γραμμή του C, όμως επειδή κάθε επεξεργαστής έχει διαφορετικό κελί της στήλης του D να επεξεργαστεί*/
	/*προσαρμόζουμε τους υπολογισμούς που θα κάνει με βάση το ποιο κελί της στήλης του D έχει*/
	/* ο 1ος επεξεραστής έχει το 1ο κελί , ο 2ος το 2ο κτλ κτλ... */
	/* άρα ο κάθε επεξεργαστής κάνει υπολογισμούς ξεκινώντας απο το αντίστοιχο rankID κελί του C  */
	/*αν ξεκινώντας απο το rankID φτάσει στο τέλος της γραμμής του C που έχει , ξεκινάει πάλι από το 0 όπως φαίνεται στο else if block*/
	/* Each processor has a line of C, but because each processor has a different cell of column D to process */
	/* we adjust the calculations it will make based on which cell in column D has */
	/* the 1st processor has the 1st cell, the 2nd the 2nd etc etc ... */
	/* so each processor makes calculations starting from the corresponding rankID cell of C */
	/* if starting from rankID reaches the end of its C line, it starts again from 0 as shown in else if block */
	for(i=0; i<amountOfNumbers; i++){
		if (flag==1)
			a++;
		else if ( (i+rankID) >= amountOfNumbers) {
			a=0; flag=1;
		}
		else
			a=i+rankID;

		for(j=0; j<amountOfNumbers; j++){
			loc_res[j]+=loc_C[a] * loc_D[j];
			/* Δοκιμαστική εκτύπωση loc_res (αποτελέσματος) ΚΑΘΕ processor σε κάθε i επανάληψη */
			/* Test print loc_res (result) for EVERY processor in each i iteration */
			//printf("%dloc_res[%d] += %5.2lf * %5.2lf; ",rankID, j ,loc_C[a] , loc_D[j] );
		}
		/*έπειτα ο κάθε επεξεργαστής στέλνει στο προηγούμενο του την γραμμή του D που έχει*/
		/* ο επεξεραστής 0 την στέλνει στον επεξεργαστή με το μεγαλύτερο rankID */
		/* then each processor sends to its predecessor the D line it has */
		/* processor 0 sends it to the processor with the highest rankID */
		if (rankID ==0 )
			MPI_Send(loc_D, amountOfNumbers, MPI_DOUBLE, processorsNum-1, 0,MPI_COMM_WORLD);
		else
			MPI_Send(loc_D, amountOfNumbers, MPI_DOUBLE, rankID-1, rankID,MPI_COMM_WORLD);
		if (rankID== processorsNum-1 )
			MPI_Recv(loc_D, amountOfNumbers, MPI_DOUBLE, 0,0,MPI_COMM_WORLD, &status);
		else
			MPI_Recv(loc_D, amountOfNumbers, MPI_DOUBLE, rankID+1,rankID+1,MPI_COMM_WORLD, &status);

	}


	MPI_Gather(loc_res,amountOfNumbers,MPI_DOUBLE,MulCD,amountOfNumbers,MPI_DOUBLE,0,MPI_COMM_WORLD);


	free (loc_C);
	free (loc_D);
	free (loc_res);
	if (rankID==0){
		printDouble2DSquareMatrix(amountOfNumbers, MulCD,'M');
		free (C);
		free (MulCD);
		free (D);
		printf("\n Πιέστε enter για συνέχεια \n Press Enter to Continue\n");
		system("/bin/bash -c 'read -sn1'"); /*system("pause");*/
	}
}
