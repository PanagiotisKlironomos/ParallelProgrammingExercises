
/* Copyright (C) 2019 Panagiotis Klironomos - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU GENERAL PUBLIC LICENSE 
 */
/*
sudo apt install libopenmpi-dev
export PATH="$PATH:/home/root/.openmpi/bin" 
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/home/root/.openmpi/lib" 
su panos
mpicc -o e1_e1.c
mpirun -oversubscribe  -np 5 ./e1
*/
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char** argv){

MPI_Status status;   
MPI_Init(&argc, &argv);  
int rank,size;
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Comm_size(MPI_COMM_WORLD, &size);
int amountOfNumbers,i,j,dutyNum,choice=0;
char sAmountOfNumbers[100];
char sChoice[10];
double * data,* localVector, * tempMatrix;
int tag0=0,tag1=10,tag2=20,tag3=30,tag4=40,tag5=50,tag6=60,tag7=70,tag8=80,tag9=90,tag10=100;
double average,var,localSum=0,localVar=0,maxMinusMin,min;
int processNum; /* μεταβλητές που χρησιμοποιεί */ /*variables thar are used */
double finalSum=0,finalVar=0,max; /* μόνο η root process */ /*only by the root process*/
   while(choice !=2){
         average=0,var=0,localSum=0,localVar=0,maxMinusMin=0,min=0; /* αρχικοποίηση μεταβλητών */ /*variables initialisation */
	if (rank==0) {
		finalSum=0,finalVar=0,max=0; /* αρχικοποίηση μεταβλητών root process */ /*root process variables initialisation */
		do{ /* Εισαγωγή επιλογής μενού με έλεγχο */ /*controlled choice entry */
			system("clear");
			printf(" _         _____                                            \n");          
			printf("/ |       |  ___)                                           \n"); 
			printf("- |_ __   | |_    ___ _   _ __  ______ _  __  __            \n"); 
			printf("| | '_ \\  |  _)  / _ ( \\ / /  \\/ /  ._| |/  \\/ /        \n"); 
			printf("| | | | | | |___| |_) \\ v ( ()  ( () )| ( ()  <            \n"); 
			printf("|_|_| | | |_____|  __/ | | \\__/\\_\\__/  \\_\\__/\\_\\     \n"); 
			printf("      | |       | |    | |  ΠΑΝΑΓΙΩΤΗΣ ΚΛΗΡΟΝΟΜΟΣ           \n"); 
			printf(" _____|_|_      |_|    |_|  __    __          __            \n");          
			printf(" (   _   )  AM:711151128    \\ \\   \\ \\         \\ \\                   \n"); 
			printf("  | | | | __  __ ___   __  __\\ \\   \\ \\   _ __  \\ \\   ___  _   _     \n"); 
			printf("  | | | |/  \\/ // _ \\ /  \\/ / > \\   > \\ | '_ \\  > \\ / _ \\| | | |  \n"); 
			printf("  | | | ( ()  <| |_) ( ()  < / ^ \\ / ^ \\| | | |/ ^ ( (_) | |_| |        \n"); 
			printf("  |_| |_|\\__/\\_|  __/ \\__/\\_/_/ \\_/_/ \\_|_| | /_/ \\_\\___/ \\___/  \n"); 
			printf("               | |                          | |                           \n"); 
			printf("               |_|                          |_|                           \n"); 
	 		printf("\nΕισάγετε μία από τις παρακάτω επιλογές μενού\nEnter one of the menu choices below\n");
			printf("\n1. Συνέχεια 2. Έξοδος\n1. Continue 2. Exit\n");
			scanf("%s",sChoice);
			choice = atoi(sChoice);
		}while (choice != 1 && choice !=2);  
		/*Σε κάθε διεργασία γίνετε send η επιλογή μενού */
		/* In each process the menu option is sent */
		for(processNum=1; processNum<size; processNum++){
			MPI_Send(&choice,1,MPI_INT,processNum,tag0,MPI_COMM_WORLD);
		}
		/*Αν η επιλογή δεν ήταν τερματισμού συνεχίζει η root process */
		/* If the option was not  terminated the root process continues */
		if (choice!=2) {
			printf("Δώσε πλήθος αριθμών\nEnter number of numbers \n");
 			/*Διάβασμα πλήθους αριθμών με έλεγχο */
			/* Read a number of numbers with control */			
			while(scanf("%d",&amountOfNumbers) != 1){
				printf("Παρακαλώ εισάγετε ακέραιο: \nEnter an integer:\n");
				while(getchar() != '\n');
			    }
			/*Δέσμευση κατάλληλου χώρου ανάλογα με το πλήθος αριθμών */
			    /* Commit space depending on the number of numbers */
			data = (double*) malloc (amountOfNumbers*sizeof(double));
			if (data==NULL) {
				printf("Δεν ήταν δυνατή η εκχώρηση μνήμης\nCould not allocate memory \n");
				exit(1);
			}
			/*Διάβασμα αριθμών με έλεγχο */
			/* Controlled Read of numbers  */
			for(i=0; i<amountOfNumbers; i++){
				do{
					printf("Δώσε τον %do αριθμό: \nGive  number %d: \n",i+1,i+1);
					while(getchar() != '\n');
				}while(scanf("%lf",&data[i])!=1);
				if(i==0){
					min=data[i];
					max=data[i];
				}
				if(data[i]>max)
					max=data[i];
				else if(data[i]<min)
					min=data[i];
			}
			/* Μεταβλήτές που χρησιμοποιούνται μόνο για υπολογισμό start,end ανά process*/
			/* Variables used only for start, end per process calculation */
			int duty = amountOfNumbers/size;
			int dutyModulo = amountOfNumbers%size;
			int start,end;   
			maxMinusMin= max-min;
			/* Υπολογισμός start , end για τα processes εκτός της process 0*/
			/* Start, end calculation for processes outside process 0 */
			for(processNum=1; processNum<size; processNum++){
				if (processNum < dutyModulo) {
					/*Τα processes που τους αντιστοιχεί μέρος του υπολοίπου	*/
					/* παίρνουν duty + 1 νούμερα το καθένα*/
					/* The processes that correspond to them part of the modulo */
					/*get duty + 1 numbers each */
	    				start = processNum * (duty + 1);
	    				end = start + duty;
				} else {
					/*Στα υπόλοιπα processes αντιστοιχούν duty αριθμοι αλλά */
					/* μετατοπισμένοι κατά  υπόλοιπο (dutyModulo) θέσεις πίνακα */
					/* The rest of the processes correspond to "duty" numbers but * /
					/* shifted by modulo (dutyModulo) table positions */
	    				start = processNum * duty + dutyModulo;
	    				end = start + (duty - 1);
				}
				dutyNum= end-start+1;
				/*Σε κάθε διεργασία γίνετε send το πλήθος αριθμών, ο αριθμός προσθέσεων */
				/*που πρέπει να κάνει (dutyNum) και το αντίστοιχο κομμάτι πίνακα */
				/* In each process send the number of numbers, the number of additions */
				/* to do (dutyNum) and the corresponding part of the table */
				MPI_Send(&amountOfNumbers,1,MPI_INT,processNum,tag1,MPI_COMM_WORLD);
				MPI_Send(&dutyNum,1,MPI_INT,processNum,tag2,MPI_COMM_WORLD);
				MPI_Send(&data[start],dutyNum,MPI_DOUBLE,processNum,tag3,MPI_COMM_WORLD);
			}
			/*Υπολογισμός start, end για την root process*/
			/* Start, end calculation for the root process */
			start = 0;
			if (0 < dutyModulo) {
	    			end = duty;
			} else {
	    			end = duty - 1;
			}
			dutyNum= end+1;
		}
	}
	else
	{
		/*Όλες οι διεργασίες εκτός της root κάνουν receive την επιλογή μενού*/
		/* All processes except root receive the menu option */
		MPI_Recv(&choice,1,MPI_INT,0,tag0,MPI_COMM_WORLD,&status);
		/*Και αν δεν είναι επιλογή τερματισμού */
		/* And if it is not an end option */
		if (choice!=2) {
			/*Κάνουν receive το πλήθος αριθμών , τον αριθμό προσθέσεων που πρέπει να κάνουν */
			/*και αφού δεσμεύσουν dutyNum χώρο για τον δικό τους πίνακα data, */
			/*κάνουν receive τον πίνακα data*/
			/* They receive the number of numbers, the number of additions they have to make */
			/* and after allocating "dutyNum" space for their own data table, */
			/* receive the data table */
			MPI_Recv(&amountOfNumbers,1,MPI_INT,0,tag1,MPI_COMM_WORLD,&status);
			MPI_Recv(&dutyNum,1,MPI_INT,0,tag2,MPI_COMM_WORLD,&status);
			data = (double*) malloc (dutyNum*sizeof(double));
			if (data==NULL) {
				printf("Δεν ήταν δυνατή η εκχώρηση μνήμης\nCould not allocate memory \n");
				exit(2);
			}
			MPI_Recv(&data[0],dutyNum,MPI_DOUBLE,0,tag3,MPI_COMM_WORLD,&status);
		}
	}
	if (choice!=2){
	/**********************
	Eύρεση του μέσου όρου
	**********************/
	/**********************
	Find the average
	*********************/
		/*Υπολογιστικό μέρος όλων των processes*/
		/* Computational part of all processes */
		for(i=0; i<dutyNum; i++)
			localSum+=data[i];  /*Κάθε process προσθέτει dutyNum αριθμούς*/ 
							/* Each process adds dutyNum numbers */
		if(rank!=0)/*Όλες οι processes εκτός της root στέλνουν στην root το κομμάτι που υπολόγισαν*/
				/* All processes except root send to root the part they calculated */
			MPI_Send(&localSum,1,MPI_DOUBLE,0,tag4,MPI_COMM_WORLD);
		else{ /*και η root process υπολογίζει τον τελικό μέσο όρο*/
			  /* and the root process calculates the final average */
			finalSum=localSum;
			printf("Αποτέλεσμα μετά τήν πρόσθεση της διεργασίας %d στο finalSum : %f\nResult after adding process%d to finalSum:%f \n",rank,finalSum,rank,finalSum);
			for(processNum=1; processNum<size; processNum++){
				/*αφού κάνει receive το κάθε localsum των υπόλοιπων processes*/
				/* after receiving each localsum of the other processes */
				MPI_Recv(&localSum,1,MPI_DOUBLE,processNum,tag4,MPI_COMM_WORLD,&status);
				finalSum+=localSum;
				printf("Αποτέλεσμα μετά τήν πρόσθεση της διεργασίας %d στο finalSum : %f\nResult after adding process%d to finalSum:%f \n",processNum,finalSum,processNum,finalSum);
			}
			average = finalSum/amountOfNumbers;
			printf("\n\nΟ μέσος όρος είναι (finalSum/%d) : %0.2lf\nThe average is (finalSum /%d) : %0.2lf \n\n",amountOfNumbers,average,amountOfNumbers,average);
		}

	/**********************
	Eύρεση της διασποράς
	**********************/
	/**********************
	Finding the dispersion
	*********************/
		/*η root process στέλνει σε όλες τις διεργασίες τον μέσο όρο*/
		/* root process sends the average to all processes */
		if (rank==0) {
			for(processNum=1; processNum<size; processNum++){
				MPI_Send(&average,1,MPI_DOUBLE,processNum,tag5,MPI_COMM_WORLD);
			}
		}
		/*οι διεργασίες εκτός της root κάνουν receive τον μέσο όρο*/
		/* processes outside of root receive the average */
		else {		
			MPI_Recv(&average,1,MPI_DOUBLE,0,tag5,MPI_COMM_WORLD,&status);
		}
		/*Υπολογιστικό μέρος όλων των processes*/
		/* Computational part of all processes */
		for(i=0; i<dutyNum; i++)
			localVar += (data[i]-average)*(data[i]-average);/*Κάθε process υπολογίζει dutyNum τετραγωνικούς όρους*/ /* Each process calculates dutyNum squared terms */
		/*Όλες οι processes εκτός της root στέλνουν στην root το κομμάτι που υπολόγισαν*/
		/* All processes except root send to root the part they calculated */
		if(rank!=0)
			MPI_Send(&localVar,1,MPI_DOUBLE,0,tag6,MPI_COMM_WORLD);
		/*και η root process υπολογίζει την τελική διασπορά*/	
		/* and the root process calculates the final dispersion */		
		else{ 
			finalVar=localVar;
			printf("Αποτέλεσμα μετά τήν πρόσθεση της διασποράς της διεργασίας %d στο finalVar: %f\nResult after adding spread of process%d to finalVar:%f \n",rank,finalVar,rank,finalVar);
			for(processNum=1; processNum<size; processNum++){
				/*αφού κάνει receive το κάθε localvar των υπόλοιπων processes*/
				/* after receiving each localvar of the other processes */
				MPI_Recv(&localVar,1,MPI_DOUBLE,processNum,tag6,MPI_COMM_WORLD,&status);
				finalVar+=localVar;
				printf("Αποτέλεσμα μετά τήν πρόσθεση της διασποράς της διεργασίας %d στο finalVar: %f\nResult after adding spread of process%d to finalVar:%f \n",processNum,finalVar,processNum,finalVar);
			}
			var = finalVar/amountOfNumbers;
			printf("\n\nΗ διασπορά είναι finalVar/%d): %0.2lf\nDispersion is finalVar/%d): %0.2lf \n\n",amountOfNumbers,var,amountOfNumbers,var);
		}
	/***********************
	Eύρεση του διανύσματος
	***********************/
	/***********************
	Find the vector
	***********************/
		/*η root process στέλνει σε όλες τις διεργασίες τον min όρο και τον παρονομαστή (Xmax-Xmin)*/
		/* the root process sends the min term and the denominator (Xmax-Xmin) to all processes */
		if (rank==0) {
			for(processNum=1; processNum<size; processNum++){
				MPI_Send(&min,1,MPI_DOUBLE,processNum,tag7,MPI_COMM_WORLD);
				MPI_Send(&maxMinusMin,1,MPI_DOUBLE,processNum,tag8,MPI_COMM_WORLD);
			}
			/* και φτιάχνει το διάνυσμα μεγέθους όσο και το πλήθος των αριθμών */
			/* and makes the size of vector according to the number of numbers */
			localVector = (double*) malloc (amountOfNumbers * sizeof(double));
			if (localVector==NULL) {
				printf("Δεν ήταν δυνατή η εκχώρηση μνήμης\nCould not allocate memory \n");
				exit(3);
			}
		}
		else {	
			/*οι διεργασίες εκτός της root κάνουν receive τον min όρο και τον παρονομαστή (Xmax-Xmin)*/	
			/* processes outside of root receive the min term and the denominator (Xmax-Xmin) */
			MPI_Recv(&min,1,MPI_DOUBLE,0,tag7,MPI_COMM_WORLD,&status);
			MPI_Recv(&maxMinusMin,1,MPI_DOUBLE,0,tag8,MPI_COMM_WORLD,&status);
			/* και φτιάχνουν το δικό τους μικρότερο localVector διάνυσμα μεγέθους dutyNum */
			/* and make their own smaller localVector vector size dutyNum */
			localVector = (double*) malloc (dutyNum * sizeof(double));
			if (localVector==NULL) {
				printf("Δεν ήταν δυνατή η εκχώρηση μνήμης\nCould not allocate memory \n");
				exit(4);
			}
			
		}
		/*Υπολογιστικό μέρος όλων των processes*/
		/* Computational part of all processes */
		for(i=0; i<dutyNum; i++) /*Κάθε process υπολογίζει dutyNum στοιχεία του διανύσματος*/
							/* Each process calculates dutyNum elements of the vector */
			localVector[i] = (data[i]-min) / maxMinusMin *100;
		/*Όλες οι processes εκτός της root στέλνουν στην root το κομμάτι που υπολόγισαν*/
		/*και κάνουν free τους πίνακες που είχαν allocated*/
		/* All processes except root send to root the part they calculated */
		/* and free the tables they had allocated */
		if(rank!=0) {
			MPI_Send(&dutyNum,1,MPI_INT,0,tag9,MPI_COMM_WORLD);
			MPI_Send(localVector,dutyNum,MPI_DOUBLE,0,tag10,MPI_COMM_WORLD);
			free(localVector);
			free(data);
		}
		/*και η root process υπολογίζει το τελικό διάνυσμα*/
		/* and the root process calculates the final vector */
		if (rank==0){
			i=dutyNum;
			for(processNum=1; processNum<size; processNum++){
			/*αφού κάνει receive σε ένα δικό της προσωρινό πίνακα το κάθε localvector των υπόλοιπων processes*/
			/* after it receives in its own temporary table every localvector of other processes */
				MPI_Recv(&dutyNum,1,MPI_INT,processNum,tag9,MPI_COMM_WORLD,&status);
				tempMatrix = (double *) malloc (dutyNum * sizeof(double));
				if (tempMatrix==NULL) {
					printf("Δεν ήταν δυνατή η εκχώρηση μνήμης\nCould not allocate memory \n");
					exit(5);
				}
				MPI_Recv(tempMatrix,dutyNum,MPI_DOUBLE,processNum,tag10,MPI_COMM_WORLD,&status);
				for(j=0;j<dutyNum;j++){
					localVector[i]=tempMatrix[j];
					i++; 				
				}
				free(tempMatrix);
			}
		
			printf("Διάνυσμα (Vector) δi = ((xi – xmin) / (xmax – xmin)) * 100\n");
			for(i=0;i<amountOfNumbers;i++)
				printf("Vector[%d]= %lf\n",i,localVector[i]);
			printf("Πιέστε Enter για συνέχεια\nPress Enter to continue \n");
			system("/bin/bash -c 'read -sn1'"); //system("pause");
			free(localVector);
			free(data);
		}
	}	
   }
  if (rank==0)
	printf("Καλή συνέχεια και από τις %d διεργασίες\n Bye from %d processes\n",size,size);
  MPI_Finalize();
  return (0);
 }
