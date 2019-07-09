#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include "20141544.h"
#include "list.c"
List History;
int main(void){
	unsigned char memory[65536][16]={0}; // memory로 사용될 배열
	char str[MAX_INPUT_LEN]; char strcopy[MAX_INPUT_LEN]; 
	int command=0;	List hashtable[20]; //hashtable로 사용될 리스트와 명령어 구분을 위한 변수 선언
	for(int i=0;i<20;i++) // list 초기화
		ListInit(&hashtable[i]);	
	readfile(hashtable); // opcode.txt파일을 읽어들임
	ListInit(&History); // list 초기화
	Idx_info idx = {0,0}; // dump에서 주소를 저장한  구조체인 idx 구조체 변수 선언
	while(1){
		printf("sicsim> ");
		memset(str,'\0',sizeof(str)); // 문자열 초기화
		memset(strcopy,'\0',sizeof(strcopy));
		scanf("%[^\n]s",str); // 개행 전까지 읽어들임
		getchar();
		strcpy(strcopy,str);
		space_remover(str);
		if(str[0]=='\0') // 개행 문자를 받거나 space나 tab만 받았을 시 무시한다
			continue;
		command = convert_to_command(str,strcopy); // 입력받은 문자열이 어떤 command인지를 검사한다
		switch(command){
			case help:
				print_help();
				break;
			case dir :
				print_dir();
				break;
			case history :
				Lprint(&History);
				break;
			case dump :
				dump_print(memory,&idx,160);
				break;
			case dump_start :
				dump_print_start(memory,&idx,str,strcopy);
				break;
			case dump_start_end :
				dump_print_start_end(memory,&idx,str,strcopy);
				break;
			case edit :
				edit_memory(memory,str,strcopy);
				break;
			case fill :
				fill_memory(memory,str,strcopy);
				break;
			case reset :
				Reset_Memory(memory);
				break;
			case opcodemnmonic :
				FindOPCODE(hashtable,str,strcopy);
				break;
			case opcodelist :
				OpPrint(hashtable);
				break;
			case cat :
				Cat(str);
				break;
			case touch :
				Touch(str);
				break;
			case echo :
				Echo(str);
				break;;
			case date :
				Date_print();
				break;
			case quit :
				List_Free(&History); // 프로그램을 종료하기전 list를 free한다
				for(int i=1;i<20;i++)
					List_Free(&hashtable[i]);
				return 0;
			default : 
				printf("Invalid input\n");
				break;
		}
	}
	return 0;
}
void space_remover(char input_str[]){ // 문자열 앞뒤 공백을 지워주는 함수이다
	int len;
	int i=0; int tmp=0;
	while(input_str[i] ==' ' || input_str[i] == '	')  // 앞의 공백을 지우기 위한 while문이다
		i++;
	for(unsigned int k=i;k<=strlen(input_str);k++)
		input_str[tmp++]=input_str[k];

	len = strlen(input_str)-1;

	while(input_str[len] == ' ' || input_str[len] == '	') // 뒤의 공백을 지우기 위한 while문 이다
		len--;
	input_str[len+1] = '\0';
	return;
}
void print_help(){ // 명렁어 help를 수행하는 함수이다
	printf("\nh[elp]\n");
	printf("d[ir]\n");
	printf("q[uit]\n");
	printf("hi[story]\n");
	printf("du[mp] [start , end]\n");
	printf("e[dit] address, value\n");
	printf("f[ill] start , end, value\n");
	printf("reset\n");
	printf("opcode mnemonic\n");
	printf("opcodelist\n");
	printf("echo str\n");
	printf("touch filename\n");
	printf("cat filename\n");
	printf("date\n\n");
}
void print_dir(){ // 명령어 dir을 수행하기 위한 함수이다
	DIR *dir_pointer;
	struct dirent *dir_entry;
	struct stat buf;
	int i=0;

	dir_pointer = opendir("./"); // 현재 경로를 저장한다
	if(dir_pointer==NULL)
		return;
	while(1){ // 모든 경로를 출력하기 위한 while문
		dir_entry = readdir(dir_pointer);
		if(dir_entry == NULL)
			break;
		stat(dir_entry->d_name,&buf);

		if(S_ISDIR(buf.st_mode)){ // directory인지 검사한다
			printf("\t\t%20s/", dir_entry->d_name);
			i++;}
		else if((buf.st_mode & S_IEXEC)==S_IEXEC){ // 실행파일인지 검사한다
			printf("\t\t%20s*",dir_entry->d_name);
			i++;}			
		else{
			printf("\t\t%20s",dir_entry->d_name); // 나머지 파일들 처리
			i++;	
		    }
		if(i%MAX==0) // 3개씩 출력하고 개행한다
			printf("\n");

	}
	closedir(dir_pointer);
	printf("\n");

	return;

}
int commacheck(char *str){ // 문자열에 comma가 있는지 검사하는 함수이다
	int len = strlen(str);
	for(int i=0;i<len;i++){
		if(str[i] == ',')
			return 1;
	}
	return 0;
}

int convert_to_command(char *str,char *strcopy){ // 문자열이 어떤 명령어를 포함하는지 검사하는 함수이다

	if(!strcmp("h",str) || (!strcmp("help",str))){
		LInsert(&History,strcopy); return 0; }
	else if(!strcmp("d",str) || (!strcmp("dir",str))){
		LInsert(&History,strcopy); return 1; }
	else if(!strcmp("hi",str) || (!strcmp("history",str))){
		LInsert(&History,strcopy); return 2; }
	else if(!strcmp("du",str) || (!strcmp("dump",str))){
		LInsert(&History,strcopy); return 3; }
	else if(!strncmp("du ",str,3) || !strncmp("dump ",str,5) || !strncmp("dump	",str,5)  || !strncmp("du	",str,5)){
		if(!commacheck(str))
			return 4;
		else
			return 5;
	}
	else if(!strncmp("edit ",str,5) || !strncmp("e ",str,2) || !strncmp("edit	",str,5) || !strncmp("e	",str,2))
		return 6;
	else if(!strncmp("fill ",str,5) || !strncmp("fill	",str,5) || !strncmp("f ",str,2) || !strncmp("f	",str,2))
		return 7;
	else if(!strcmp("reset",str)){
		LInsert(&History,strcopy); return 8; }
	else if(!strncmp("opcode ",str,7))
		return 9;
	else if(!strcmp("opcodelist",str)){
		LInsert(&History,strcopy); return 10; }
	else if(!strncmp("cat ",str,4)){
		LInsert(&History,strcopy); return 11; }
	else if(!strncmp("touch ",str,5)){
		LInsert(&History,strcopy); return 12; }
	else if(!strncmp("echo ",str,4)){
		LInsert(&History,strcopy); return 13;}
	else if(!strcmp("date",str)){
		LInsert(&History,strcopy); return 14;}
	else if(!strcmp("quit",str) || !strcmp("q",str)){
		LInsert(&History,strcopy); return 15;}
	else
		return -1;

}
void fill_memory(unsigned char memory[][16], char *input_str,char *str_copy){ // fill 명령어를 담당하는 함수
	char trash[MAX_INPUT_LEN]; // fill을 걸러낼 변수
	int start , end , value; char trash1; // trash1은 input이 제대로 됐는지 검사하기 위한 변수로서, 제대로 된 input이라면 아무것도 저장되지 말아야함.
	int idx_i, idx_j; int check=0; // idx_i 와 idx_j는 인덱스를 저장하기 위한 변수이고 check는 sscanf의 반환값을 받아서 input check에 사용한다.
	int count;
	check = sscanf(input_str,"%s %x ,%x ,%x %c",trash,&start,&end,&value,&trash1); 
	if(check!=4){
		printf("Invalid input\n"); return;}
	if(start > end || start > MAX_BOUND || end > MAX_BOUND || start<0){ // boundary error check
		printf("Boundary Error\n"); return;}
	if(value < 0 || value > 255){
		printf("Value boundary Error\n");
		return; }
	LInsert(&History,str_copy);
	count = end-start+1;
	idx_i = start/16;
	idx_j = start%16;

	while(count--){
		if(idx_j==16){
			idx_i++; idx_j=0;
		}
		memory[idx_i][idx_j++]=value; // memory를 채워준다
	}
}
void edit_memory(unsigned char memory[][16],char *input_str,char *str_copy){ // edit 명령어를 처리하는 함수
	char trash[MAX_INPUT_LEN]; // edit을 걸러낼 변수
	int addr; int value; char trash1; // trash1도 마찬가지로 input check에 사용한다.
	int idx_i,idx_j; int check=0;
	check = sscanf(input_str,"%s %x ,%x %c",trash,&addr,&value,&trash1);
	if(check!=3){
		printf("Invalid input\n"); return;
	}
	if(addr > MAX_BOUND || addr<0){
		printf("Boundary Error\n");
		return; }
	if(value < 0 || value > 255){
		printf("Value boundary Error\n");
		return; }
	LInsert(&History,str_copy);
	idx_i = addr/16;
	idx_j = addr%16;
	memory[idx_i][idx_j] = value;
}
void Reset_Memory(unsigned char memory[][16]){ // 메모리 배열을 초기화 해주는 함수
	for(int i=0;i<65536;i++)
		for(int j=0;j<16;j++)
			memory[i][j]=0;
}
void readfile(List list[]){ // opcode.txt를 읽어들여서 hashtable을 완성시켜주는 함수이다.
	FILE *fp = fopen("opcode.txt","r");
	int num; char code[20]; char op[20];
	if(fp == NULL){
		printf("File open error\n");
		return;
	}
	while(fscanf(fp,"%x %s %s",&num,code,op)!=EOF) // 파일이 끝날때까지
		LInsert1(&list[hashfunc(num,strlen(code))],code,op,num); // hashtable에명령어들을 저장한다.
	fclose(fp);
}
int hashfunc(int num,int len){ // hash function으로, 명령어 number와 명령어의 길이를 더한값을 7로 곱하고 20으로 나누어 return한다
	if(num>=20)
		return (num+len)*7%20;
	else
		return num;
}
void OpPrint(List list[]){ // list를 print하기 위한 함수
	Node *tmp;
	for(int i=0;i<20;i++){ // 20개의 list를 출력한다
		tmp = list[i].head->next;
		printf("%d : ",i);
		if(list[i].numOfData==0){ // 아무것도 저장되지 않은 list를 위한 if문
			printf("empty\n");
			continue;
		}
		printf("[%s,%02X] ",tmp->data,tmp->num);
		tmp=tmp->next;
		for(int j=1;j<list[i].numOfData;j++){
			printf("->[%s,%02X] ",tmp->data,tmp->num);
			tmp=tmp->next;
		}
		printf("\n");
	}
}

void dump_print(unsigned char memory[][16],Idx_info* idx,int cnt){ // 명령어 dump를 처리하기 위한 함수이다.
	if(idx->i == 65536){
		idx->i = 0; idx->j=0;
	}
	int start_idx = idx->j;
	int i=idx->i,j=idx->j,k=0; int tmp;
	if(j!=0){ // 처음 출력해야될 인덱스의 열이 0번째가 아니면 공백을 출력한다
		printf("%05X ",i*16);
		for(int i=j;i>0;i--)
			printf("   ");
	}
	while(cnt--){ // cnt에 출력 해야 될 주소의 개수가 저장되어있다.
		if(i == 65536){ // 마지막 주소까지 출력하면 주소를 초기화 해준다.
			idx->i=0; idx->j=0;
			return;
		}
		if(j==0){ // 줄이 바뀌면
			start_idx = 0;
			printf("%05X ",i*16); // 시작주소를 출력해준다
			printf("%02X ",memory[i][j++]);
			continue;
		}
		if(j==15){ // 줄이 끝나면
			printf("%02X ",memory[i][j]);
			printf(" ; ");
			for(k=0;k<start_idx;k++)
				printf(".");
			for(k=start_idx;k<=15;k++){ // 아스키코드를 출력해준다
				if(memory[i][k]>=32 && memory[i][k] <= 128)
					printf("%c",memory[i][k]);
				else
					printf(".");
			}
			printf("\n");
			i++;	j=0;
			continue;
		}
		printf("%02X ",memory[i][j++]); // 보통의 경우 출력
	}
	tmp = j;
	if(tmp!=0){ // 출력이 줄의 끝에서 끝나지 않은 경우에 뒷부분 공백을 출력해준다.
		while(tmp<16){
			printf("   ");
			tmp++;
		}
		printf(" ; ");
		for(k=0;k<start_idx;k++)
			printf(".");
		for(k=start_idx;k<j;k++){ // 아스키코드를 출력해준다.
			if(memory[i][k]>=32 && memory[i][k] <= 126)
				printf("%c",memory[i][k]);
			else
				printf(".");
		}
		for(k=j;k<16;k++)
			printf(".");
	}
	
	idx->i = i; // index를 저장한다
	idx->j = j; // index를 저장한다
	printf("\n");
}
void dump_print_start(unsigned char memory[][16],Idx_info* idx,char *input_str,char *str_copy){ // dump와 시작주소가 같이 입력됐을때를 처리하기 위한 함수이다.
	char trash[MAX_INPUT_LEN]; // trash는 "dump"를 저장할 배열이다
	int start; char trash1; // trash1은 input_check를 위한 변수이다. 제대로 된 input이 들어온다면 아무것도 저장되지 말아야한다.
	int check = sscanf(input_str,"%s %x %c",trash,&start,&trash1);
	if(check!=2){
		printf("Invalid input\n");
		return;
	}
	if(start>MAX_BOUND || start<0){ // boundary error check
		printf("Boundary Error\n");
		return; }
	LInsert(&History,str_copy); // 제대)로 된 입력이면 history에 추가한다
	idx->i = start/16; // 시작 인덱스를 정한다
	idx->j = start%16;

	dump_print(memory,idx,160); // dump_print함수에서 print한다
}
void dump_print_start_end(unsigned char memory[][16],Idx_info* idx,char *input_str,char *str_copy){ // dump와 시작주소와 끝주소가 같이 입력됐을때를 처리하기 위한 함수이다.
	char trash[MAX_INPUT_LEN]; char trash1=0; // trash는 "dump"를 저장할 배열이다. trash1은 input check를 위한 변수이다.
	int start,end,check;
	check = sscanf(input_str,"%s %x , %x %c",trash,&start,&end,&trash1);
	if(check!=3){
		printf("Invalid Input\n");
		return; }
	if(start > end || start>MAX_BOUND || end>MAX_BOUND || start<0 || end<0){ // boundary error check
		printf("Boundary Error\n");
		return; }
	LInsert(&History,str_copy); // 제대로된 입력이 들어오면 history에 추가한다
	idx->i = start/16; // 시작 index를 저장한다
	idx->j = start%16;

	dump_print(memory,idx,end-start+1); 
}
void FindOPCODE(List LIST[],char * str,char *str_copy){ // list를 검색해서 opcode를 찾아주는 함수이다
	char code[100];
	char dump[100];
	char trash;
	Node *tmp; 
	int check=sscanf(str,"%s %s %c",dump,code,&trash);
	if(check!=2){
		printf("Invalid input\n");
		return;
	}
	for(int i=0;i<20;i++){ // 20개의 리스트에서 명령어를 검색한다
		tmp = LIST[i].head->next;
		for(int j=0;j<LIST[i].numOfData;j++){
			if(!strcmp(tmp->data,code)){ // 명령어를 찾으면
				printf("opcode is %02X\n",tmp->num); // 출력 후 종료한다
				LInsert(&History,str_copy);
				return;
			}
			tmp = tmp->next;
		}
	}
	printf("opcode is not exist\n"); 
}
void Echo(char * str){
	space_remover(str);
	for(int i=5;i<1000;i++){
		if(str[i]=='\0')
			break;
		if(str[i]=='\"')
			continue;
		printf("%c",str[i]);
	}
	printf("\n");
	return;
}
void Touch(char *str){
	char trash[100]; char filename[100]; 
	sscanf(str,"%s %s",trash,filename);
	space_remover(filename);
	FILE * fp = fopen(filename,"r");
	if(fp != NULL){
		printf("file is already exist\n");
		fclose(fp);
		return;
	}
	fp = fopen(filename,"w");
	printf("%s is generated\n",filename);
	fclose(fp);
	return ;
}
void Cat(char *str){
	char trash[100]; char filename[100]; 
	sscanf(str,"%s %s",trash,filename);
	space_remover(filename);
	FILE *fp = fopen(filename,"r");
	if(fp == NULL){
		printf("%s is not exist\n",filename);
		return; 
	}
	char temp;
	while(fscanf(fp,"%c",&temp)!=EOF)
		printf("%c",temp);
	fclose(fp);
	return;
}
void Date_print(){
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	printf("now: %d-%d-%d %d:%d:%d\n",tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
	tm.tm_hour, tm.tm_min, tm.tm_sec);
		  
	return;
}
