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
void List_Free(List *plist){ // list를 free시키는 함수이다.
	Node *delNode = plist->head;
	while(plist->head->next!=NULL){
		plist->head=plist->head->next;
		free(delNode);
		delNode = plist->head;
	}
	free(delNode);
	plist->head=NULL;
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
