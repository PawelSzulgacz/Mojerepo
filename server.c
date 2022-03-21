#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>   
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

struct buf_elem
{
long mtype;
int first;
int second;
char mess[512];
};

void basicfile(int *p,int *g)
{
	int pp = 0;
	int gg = 0;
	char *ptr;
	char text[80] = "";
	char *line = NULL;
	int reader;
	size_t len = 0;
	FILE *cfile = fopen("config.txt","r");
	FILE *bfile = fopen("basic.txt","w");
	while((reader = getline(&line,&len,cfile)) != -1)
	{
	strcpy(text,line);
	ptr = strtok(line," ");
	if(strcmp(ptr,"o") == 0)
	{
	text[0] = '\0';
	pp++;
	strcat(text,"o ");
	ptr = strtok(NULL," ");
	strcat(text,ptr);
	strcat(text," ");
	ptr = strtok(NULL," ");
	strcat(text,ptr);
	strcat(text," \n");
	fputs(text,bfile);
	}
	else
	{
	gg++;
	fputs(text,bfile);
	}	
	}
	fclose(cfile);
	fclose(bfile);
	*p = pp;
	*g = gg;
}



int main(int argc, char *argv[]){
	struct buf_elem elem;
	char pusty[1] = "";
	int groups = 0;
	int people = 0;
	int holder = 1;
	int client;
	char *line = NULL;
	int reader;
	size_t len = 0;
	char *ptr;
	char *ptr2;
	char text[80];
	char loghelper[50];
	char loghelper2[50];
	int linecounter;
	int receiver;
	FILE *bfiler = fopen("basic.txt","r");
	FILE *cfiler = fopen("config.txt","r");

	basicfile(&people,&groups);
//zrobienie tablic z ludzmi w grupach i zalogowanymi
	int group_members[groups][people];
	int loged[people];
	for(int i=0;i<people+1;i++)
	{
	loged[i] = 0;	
	for(int j=0;j<groups+2;j++)
		group_members[j][i] = 0;
	}		
//zrobienie tablic z ludzmi w danych grupach
	while((reader = getline(&line,&len,bfiler)) != -1)
	{
	ptr = strtok(line," ");
	if(strcmp(ptr,"g") == 0)
	{
	for(int i=0;i<3;i++)
		ptr = strtok(NULL," ");
	while(ptr != NULL)
	{
	group_members[holder][atoi(ptr)] = 1;
	ptr = strtok(NULL," ");
	}
	holder++;
	}
	}
//tablice z nickami
fseek(bfiler,0,SEEK_SET);
int holderp = 0;
int holderg = 0;
char nicknames[people+1][40];
char groupnames[groups+1][40];
while((reader = getline(&line,&len,bfiler)) != -1)
{
	ptr = strtok(line," ");
	if(strcmp(ptr,"g") == 0)
	{
		ptr = strtok(NULL," ");
		ptr = strtok(NULL," ");
		holderg++;
		strcpy(groupnames[holderg],ptr);
	}
	else
	{
		ptr = strtok(NULL," ");
		ptr = strtok(NULL," ");
		holderp++;
		strcpy(nicknames[holderp],ptr);
	}
}

//obsluga
	int id = msgget(6661,IPC_CREAT|IPC_EXCL|0600);
	if(id == -1)
		id = msgget(6661,IPC_CREAT|0600);
	while(1)
	{
		printf("\n");
		msgrcv(id,&elem,sizeof(elem)-sizeof(long),1,0);
		if(elem.second == 1)			
		{//zalogowanie
			printf("Prosba o zalogowanie :");
			linecounter = 0;
			fseek(cfiler,0,SEEK_SET);
			strcpy(loghelper,elem.mess);//skopiowanie do helpera
			ptr2 = strtok(elem.mess," ");//przejscie do loginu
			while((reader = getline(&line,&len,cfiler)) != -1)
			{
			linecounter++;
			ptr = strtok(line," ");
			if(strcmp(ptr,"o") == 0)//spr czy osoba czy grupa
			{
			ptr = strtok(NULL," ");
			ptr = strtok(NULL," ");//przejscie do loginu
			strcpy(loghelper2,ptr);
			elem.mtype = 2; //typ wiadomosci
				if(strcmp(ptr,ptr2) == 0) //szukanie zgadzajacego sie loginu
				{
					printf("%s \n",ptr);
					if(loged[linecounter] == 0)
					{
						ptr = strtok(NULL," ");//przejscie do hasla
						strcat(loghelper2," ");
						strcat(loghelper2,ptr);//najpierw porowaneni loginow, a potem loginow + hasla
						
						if(strcmp(loghelper,loghelper2) == 0) //sprawdzanie czy haslo sie zgadza
						{
							elem.second = 0;
							elem.first = linecounter+3;
							strcpy(elem.mess,"Zalogowano\n");
							msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
							loged[linecounter] = 1;
							printf("%s ", "Potwierdzona\n");
							elem.first = people;
							elem.second = groups;
							msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);//informacja ile jest grup i osob
							break;
						}
						else
						{
							elem.second = 1;
							strcpy(elem.mess,"Zle haslo\n");
							msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
							printf("%s ", "Odrzucona(zle haslo)\n");
							break;
						}
					}
					else
					{
						elem.second = 1;
						strcpy(elem.mess,"Taka osoba juz jest zalogowana\n");
						msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
						printf("%s ", "Odrzucona(taka osoba jest juz zalogowana)\n");
						break;
					}
				}
			}
			}
			if(linecounter == groups+people)
			{
			elem.second = 1;
			strcpy(elem.mess,"Zly login\n");
			msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
			printf("Odrzucona(zly login) \n");
			}
		}
		else if(elem.second == 2)
		{//wylogowanie
			client = elem.first-3;
			loged[client] = 0;
			elem.mtype = elem.first + people;
			msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
			printf("Wylogowano %s\n",nicknames[client]);
			
		}
		else if(elem.second == 3)
		{//kto zalogowany
			client = elem.first-3;
			elem.mtype = elem.first + people;
			strcpy(text,pusty);
			for(int i=0;i<people+1;i++)
			{
				if(loged[i] == 1)
				{
					strcat(text,nicknames[i]);
					strcat(text,",");
				}
			}
			strcpy(elem.mess,text);
			msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
			printf("Wyslano informacje o zalogowanych uzytkownikach do %s\n",nicknames[client]);
		}
		else if(elem.second <= 3+groups)
		{//zapis do grupy
			client = elem.first-3;
			elem.mtype = elem.first + people;
			group_members[elem.second-3][client] = 1;
			elem.second -= 3;
			msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
			printf("Zapisano %s do grupy %s\n",nicknames[client],groupnames[elem.second]);		
		}
		else if(elem.second <= 3+2*groups)
		{//wypis z grupy
			client = elem.first-3;
			elem.mtype = elem.first + people;
			group_members[elem.second-3-groups][client] = 0;
			//for(int i=0;i<people+1;i++)
				//printf("%d - %d",i,group_members[2][i]);
			elem.second -= (3 + groups);
			msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
			printf("Wypisano %s z grupy %s\n",nicknames[client],groupnames[elem.second]);			
		}
		else if(elem.second <= 3+3*groups)
		{//kto w jakiej grupie
			client = elem.first-3;
			strcpy(elem.mess,pusty);
			elem.mtype = elem.first + people;
			holder = elem.second-3-2*groups;
			for(int i=0;i<people+1;i++)
			{
				if(group_members[holder][i] == 1)
				{
					strcat(elem.mess,nicknames[i]);
					strcat(elem.mess," ");
				}

			}
			msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
			printf("Wyslano %s informacje o ludziach w grupie %s\n",nicknames[client], groupnames[holder]);
		}
		else if(elem.second <= 3+4*groups)
		{//do jakiej grupy wiadomosc
			client = elem.first-3;
			receiver = elem.second-3-3*groups;
			if(group_members[receiver][client] == 1)//jesli klient nalezy do grupy
			{
			elem.second = receiver;
			for(int i=0;i<people+1;i++)
			{
				if(group_members[receiver][i] == 1 && i != client)
				{
					elem.mtype = i + 3;
					msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
				}
			}
			elem.mtype = elem.first + people;
			strcpy(elem.mess,"Wiadomosc wyslana");
			msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
			printf("Wiadomosc od %s wyslana do grupy %s\n",nicknames[client],groupnames[receiver]);
			}
			else
			{
					strcpy(elem.mess,"Nie nalezysz do tej grupy\n");
					elem.mtype = elem.first + people;
					msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
					printf("Nie wyslano wiadomosci od %s do grupy %s, poniewaz nie jest on w tej grupie\n",nicknames[client],groupnames[receiver]);
			}
		}
		else if(elem.second <= 3+4*groups+people)
		{//wiadomosc do osoby
			client = elem.first-3;
			receiver = elem.second-3-4*groups;
			elem.mtype = receiver + 3; //numer ze wzoru 
			elem.second = 0;
			msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);

			strcpy(elem.mess,"Wiadomosc wyslana");
			elem.mtype = elem.first + people;
			msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
			printf("Wiadomosc od %s wyslana do %s\n",nicknames[client],nicknames[receiver]);
		}
		else
		{
			//blad
		}
	}



	return 0;
}



