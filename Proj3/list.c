#ifndef __list__C__
#define __list__C__
#include "20141544.h"
void ListInit(List * plist) { // 리스트를 초기화 시키는 함수이다.
	plist->head = (Node*)malloc(sizeof(Node)); // Dummy노드를 생성한다
	plist->head->next = NULL;
	plist->tail = plist->head;
	plist->numOfData = 0;
}
void LInsert(List * plist, char *str) { // list insert 함수이다. History를 위한 insert이다.
	Node * newNode = (Node *)malloc(sizeof(Node));
	newNode->data = (char *)malloc(sizeof(char)*(strlen(str) + 1));
	newNode->next = NULL;
	strcpy(newNode->data, str);
	plist->tail->next = newNode;
	plist->tail = newNode;
	plist->numOfData++;
}
/*void LInsert_Symbol(SymbolList *plist,char *label,int loc){
  SymNode *NewNode = (SymNode *)malloc(sizeof(SymNode));
  NewNode -> Label = (char *)malloc(sizeof(char) * (strlen(label)+1));
  Locctr = loc;
  }*/
void LInsert1(List * plist, char *code, char *op,int num){ //list insert 함수이다. Opcode를 위한 hashtable을 만드는데 사용한다.
	Node * newNode = (Node *)malloc(sizeof(Node));
	newNode->data = (char *)malloc(sizeof(char) *(strlen(code)+1));
	strcpy(newNode->data,code);
	newNode->data2 = (char *)malloc(sizeof(char) *(strlen(op)+1));
	strcpy(newNode->data2,op);
	newNode->num = num;
	newNode->next=NULL;
	plist->tail->next = newNode;
	plist->tail = newNode;
	plist->numOfData++;
}
void LInsert_Symbol(List *plist,char *label,int loc){
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->data = (char *)malloc(sizeof(char) * (strlen(label)+1));
	strcpy(newNode->data,label);
	newNode->num = loc;
	int len = plist->numOfData;
	Node *tmp;
	Node *before = plist->head;
	if(plist->numOfData == 0){
		plist->tail->next = newNode;
		plist->tail = newNode;
	}
	else{
		tmp = plist->head->next;
		while(len--){
			if(tmp == NULL)
				break;
			if(strcmp(tmp->data,newNode->data)>0){ // strcmp for arrange to down-order
				before = tmp;
				tmp = tmp->next;
			}
			else
				break;
		}
		if(tmp == NULL){
			plist->tail->next = newNode;
			plist->tail = newNode;
		}
		else{
			newNode->next = before->next;
			before->next = newNode;
		}
	}
	plist->numOfData++;
}
int Symbol_Find(List *plist,char *Label,int *addr){
	if(plist->numOfData==0)
		return 1;
	Node *tmp = plist->head->next;
	int num=plist->numOfData;
	while(num--){
		if(!strcmp(tmp->data,Label)){
			*addr = tmp->num;
			return -1;
		}
		tmp=tmp->next;
	}
	return 1;
}
void List_Free(List *plist){ // list를 free시키는 함수이다.
	if(plist->numOfData == 0)
		return;
	Node *delNode = plist->head;
	while(plist->head->next!=NULL){
		plist->head=plist->head->next;
		free(delNode);
		delNode = plist->head;
	}
	free(delNode);
	plist->numOfData--;
	plist->head=NULL;
	plist->tail=NULL;
	plist->numOfData=0;
}

void Lprint(List * plist) {  //History 출력용 함수이다
	int i=1;
	if(plist->head==NULL){
		printf("List is empty\n");
		return; }
	Node *tmp = plist->head->next;
	while (tmp!= NULL) {
		printf("%d  %s\n",i++,tmp->data);
		tmp = tmp->next;
	}
}
void Symbol_print(List *plist){  // Symbol_print_function
	if(plist->head == NULL){ 
		printf("Now Symbol table is empty\n");
		return; }
	Node *tmp = plist->head->next;
	int cnt = plist->numOfData;
	while(cnt--){
		printf("\t%s\t%04X\n",tmp->data,tmp->num);
		tmp = tmp->next;
	}
	return;
}
int SearchOPCODE(List LIST[],char * opcode,char format[],int *opNum){
	char *token = NULL;
	char copystr[50];
	strcpy(copystr,opcode);
	Node * tmp; int check=0;
	if(opcode[0] == '+'){
		token = strtok(copystr,"+"); // if operand start with '+'
		strcpy(copystr,token);
		check=1;
	}
	for(int i=0;i<20;i++){  
		tmp = LIST[i].head->next;
		for(int j=0;j<LIST[i].numOfData;j++){
			if(!strcmp(tmp->data,copystr)){
				if(check==1){
					strcpy(format,tmp->data2); 
					if(!strcmp(format,"2") || !strcmp(format,"1")) // if operand start with '+' and format is 1 or 2
						return 0; // 
					*opNum = tmp->num;
					return 1;}
				else{
					strcpy(format,tmp->data2);
					*opNum = tmp->num;
					return 1;}
			}
			tmp = tmp->next;
		}
	}
	return -1;
}
#endif
