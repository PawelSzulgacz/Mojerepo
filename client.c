#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>   
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

struct buf_elem
{
long mtype;
int first;
int second;
char mess[512];
};


int main(int argc, char *argv[]){
	struct buf_elem elem;//wysylanie
	struct buf_elem elem2;//odbior
	char nicknames[100][40];
	char groupnames[50][40];
	int id;
	int whattodo2 = 0;
	int number;
	int numberservermess;
	int people;
	int groups;
	int loged = 0;
	int doweknow = 0;
	char *line = NULL;
	int reader;
	size_t len = 0;
	char *ptr;
	int who;
	char term;
	id = msgget(6661,IPC_CREAT|IPC_EXCL|0600);
	if(id == -1)
		id = msgget(6661,IPC_CREAT|0600);



	while(1)
	{
		elem.mtype = 1;
		elem.first = number;
		if(loged == 0)
		{//zaloguj
			elem.second = 1;
			elem.mtype = 1;
			printf("Zaloguj sie: login haslo\n");
			fgets(elem.mess,sizeof(elem.mess),stdin);
			elem.mess[strcspn(elem.mess, "\n")] = 0;
			msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);//wysylka loginu
			msgrcv(id,&elem,sizeof(elem)-sizeof(long),2,0);//odbior wiad
			printf("%s",elem.mess);
			if(elem.second != 1)
			{
				number = elem.first;//przydzielenie wlasnego numeru
				printf("%s",elem.mess);
				loged = 1;
				msgrcv(id,&elem,sizeof(elem)-sizeof(long),2,0);//ile jest osob i grup
				people = elem.first;
				groups = elem.second;
				numberservermess = number + people;//numerek moj(numer + 3)  + liczba ludzi
				elem.first = number;
					if(doweknow == 0)//tablica z nazwami grup i uzytkownikow
					{

						int holderp = 0;
						int holderg = 0;
						FILE *bfiler = fopen("basic.txt","r");
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
					}
			}
		}
		else
		{
			printf("Co chcesz zrobic:\n1 - Wyslij wiadomosc do osoby\n2 - Wyslij wiadomosc do grupy\n3 - Odbierz wiadomosc\n4 - Inne\n");
			scanf("%d",&whattodo2);
			fgets(elem.mess,sizeof(elem.mess),stdin);
			if(whattodo2 == 1)
			{//wyslij wiadomosc do osoby
				printf("Do kogo?\n");
				for(int i=1;i<people+1;i++)
				{
					printf("%d - %s\n",i,nicknames[i]);
				}
				if(scanf("%d%c", &who, &term) != 2 || term != '\n')
				{
					printf("Niepoprawna wartosc\n");
					fgets(elem.mess,sizeof(elem.mess),stdin);
				}
				else
				{
					if(who <= people && who > 0)
					{
						printf("Tresc wiadomosci:");
						fgets(elem.mess,sizeof(elem.mess),stdin);//bo scanf zostawia spacje
						elem.second = 3+4*groups+who;
						msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
						msgrcv(id,&elem2,sizeof(elem)-sizeof(long),numberservermess,0);
						printf("%s\n",elem2.mess);
					}
					else
						printf("Nie ma takiego uzytkownika\n");
				}
			}
			else if(whattodo2 == 2)
			{//wyslij wiadomosc do grupy
				printf("Do ktorej grupy?\n");
				for(int i=1;i<groups+1;i++)
				{
					printf("%d - %s\n",i,groupnames[i]);
				}
				if(scanf("%d%c", &who, &term) != 2 || term != '\n')
				{
					printf("Niepoprawna wartosc\n");
					fgets(elem.mess,sizeof(elem.mess),stdin);
				}
				else
				{
					if(who <= groups && who > 0)
					{
						printf("Tresc wiadomosci:");
						fgets(elem.mess,sizeof(elem.mess),stdin);//bo scanf zostawia spacje
						elem.second = 3+3*groups+who;
						msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
						msgrcv(id,&elem2,sizeof(elem)-sizeof(long),numberservermess,0);
						printf("%s\n",elem2.mess);
					}
					else
						printf("Nie ma takiej grupy\n");
				}

			}
			else if(whattodo2 == 3)
			{//odbierz wiadomosc
				if(msgrcv(id,&elem,sizeof(elem)-sizeof(long),number,IPC_NOWAIT) == -1)
				{
					printf("Brak wiadomosci\n");
				}
				else
				{
				if(elem.second == 0)
					printf("Wiadomosc od %s: %s\n",nicknames[elem.first-3],elem.mess);
				else
					printf("Wiadomosc od %s do grupy %s: %s\n",nicknames[elem.first-3],groupnames[elem.second],elem.mess);
				}
			}
			else if(whattodo2 == 4)
			{
			printf("1 - Wyloguj/zakoncz prace z programem\n2 - Sprawdz kto jest zalogowany\n3 - Zapisz do grupy\n4 - Wypisz z grupy\n5 - Sprawdz dostepne grupy\n6 - Sprawdz kto jest zapisany do grupy\n");
				scanf("%d",&whattodo2);
				fgets(elem.mess,sizeof(elem.mess),stdin);
				if(whattodo2 == 1)
				{//wyloguj
					elem.mtype = 1;
					elem.first = number;
					elem.second = 2;
					msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
					msgrcv(id,&elem2,sizeof(elem)-sizeof(long),numberservermess,0);
					printf("Poprawnie wylogowano\n");
					loged = 0;
					printf("1 - Zaloguj\n2 - Wyjdz z programu\n");
					while(1)
					{
						scanf("%d",&who);
						fgets(elem.mess,sizeof(elem.mess),stdin);
						if(who == 1)
						break;
						else if(who == 2)
						return 0;
						else printf("Blad, prosze podac jeszcze raz");
					}
					
				}
				else if(whattodo2 == 2)
				{//sprawdz kto zalogowany
					elem.second = 3;
					msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
					msgrcv(id,&elem2,sizeof(elem)-sizeof(long),numberservermess,0);
					printf("Zalogowani %s\n",elem2.mess);

				}
				else if(whattodo2 == 3)
				{//zapisz do grupy
					printf("Do ktorej grupy?\n");
					for(int i=1;i<groups+1;i++)
					{
					printf("%d - %s\n",i,groupnames[i]);
					}

					if(scanf("%d%c", &who, &term) != 2 || term != '\n')
					{
						printf("Niepoprawna wartosc\n");
						fgets(elem.mess,sizeof(elem.mess),stdin);
					}
					else
					{
						if(who <= groups && who > 0)
						{
							elem.second = who + 3;
							msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
							msgrcv(id,&elem2,sizeof(elem)-sizeof(long),numberservermess,0);
							printf("Zapisano do grupy %s\n",groupnames[elem2.second]);
						}
						else
							printf("Nie ma takiej grupy\n");
					}


				}
				else if(whattodo2 == 4)
				{//wypisz z grupy
					printf("Do ktorej grupy?");
					for(int i=1;i<groups+1;i++)
					{
					printf("%d - %s\n",i,groupnames[i]);
					}

					if(scanf("%d%c", &who, &term) != 2 || term != '\n')
					{
						printf("Niepoprawna wartosc\n");
						fgets(elem.mess,sizeof(elem.mess),stdin);
					}
					else
					{
						if(who <= groups && who > 0)
						{
							elem.second = who + groups + 3;
							msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
							msgrcv(id,&elem2,sizeof(elem)-sizeof(long),numberservermess,0);
							printf("Wypisano z grupy %s\n",groupnames[elem2.second]);
						}
						else
							printf("Nie ma takiej grupy\n");
					}



				}
				else if(whattodo2 == 5)
				{//sprawdz dostepne grupy
					for(int i=1;i<groups+1;i++)
					{
					printf("%d - %s\n",i,groupnames[i]);
					}
				}
				else if(whattodo2 == 6)
				{//sprawdz kto jest zapisany do grupy
					printf("Do ktorej grupy?\n");
					for(int i=1;i<groups+1;i++)
					{
					printf("%d - %s\n",i,groupnames[i]);
					}

					if(scanf("%d%c", &who, &term) != 2 || term != '\n')
					{
						printf("Niepoprawna wartosc\n");
						fgets(elem.mess,sizeof(elem.mess),stdin);
					}
					else
					{
						if(who <= groups && who > 0)
						{
							elem.second = who + 2*groups + 3;
							msgsnd(id,&elem,sizeof(elem)-sizeof(long),0);
							msgrcv(id,&elem2,sizeof(elem)-sizeof(long),numberservermess,0);
							printf("Do grupy %s zapisani sa: %s\n",groupnames[who],elem2.mess);
						}
						else
							printf("Nie ma takiej grupy\n");
					}
				}
			}
		}
	}

	return 0;
}
