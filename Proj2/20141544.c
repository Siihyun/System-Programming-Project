#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include "20141544.h"
#include "list.c"
List History;
List Symbol;
int start_loc=0; int end_loc=0;
char program_name[MAX_INPUT_LEN]={'\0'};
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
		scanf("%[^\n]",str); // 개행 전까지 읽어들임
		getchar();
		strcpy(strcopy,str);
		space_remover(str);
		if(str[0]=='\0') // 개행 문자를 받거나 space나 tab만 받았을 시 무시한다
			continue;
		command = convert_to_command(str,strcopy); // 입력받은 문자열이 어떤 command인지를 검사한다
		switch(command){
			case help:
				print_help(); break;
			case dir :
				print_dir(); break;
			case history :
				Lprint(&History); break;
			case dump :
				dump_print(memory,&idx,DUMP_NUM); break;
			case dump_start :
				dump_print_start(memory,&idx,str,strcopy); break;
			case dump_start_end :
				dump_print_start_end(memory,&idx,str,strcopy); break;
			case edit :
				edit_memory(memory,str,strcopy); break;
			case fill :
				fill_memory(memory,str,strcopy); break;
			case reset :
				Reset_Memory(memory); break;
			case opcodemnmonic :
				FindOPCODE(hashtable,str,strcopy); break;
			case opcodelist :
				OpPrint(hashtable); break;
			case type :
				Type(str); break;
			case touch :
				Touch(str); break;
			case echo :
				Echo(str); break;
			case date :
				Date_print(); break;
			case assemble :
				pass1(str,strcopy,hashtable); break;
		//	case Symbol :
			case quit :
				List_Free(&History); // 프로그램을 종료하기전 list를 free한다
				for(int i=1;i<20;i++)
					List_Free(&hashtable[i]);
				return 0;
			case symbol :
				Symbol_print(&Symbol); break;
			default : 
				printf("Invalid input\n"); break;
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
	printf("assemble filename\n");
	printf("type filename\n");
	printf("symbol\n");
	printf("echo str\n");
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
	else if(!strncmp("type ",str,5)){
		LInsert(&History,strcopy); return 11; }
	else if(!strncmp("touch ",str,5)){
		LInsert(&History,strcopy); return 12; }
	else if(!strncmp("echo ",str,4)){
		LInsert(&History,strcopy); return 13;}
	else if(!strcmp("date",str)){
		LInsert(&History,strcopy); return 14;}
	else if(!strncmp("assemble ",strcopy,9))
		return 15;
	else if(!strcmp("quit",str) || !strcmp("q",str)){
		LInsert(&History,strcopy); return 16;}
	else if(!strcmp("symbol",str)){
		LInsert(&History,strcopy); return 17;}
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
void Type(char *str){
	char trash[100]; char filename[100];  
	sscanf(str,"%s %[^\n]",trash,filename);
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
void pass1(char *input_str,char *strcopy,List hashtable[]){
	start_loc = 0; end_loc = 0;
	char str[MAX_INPUT_LEN]; char copystr[MAX_INPUT_LEN]={'\0'}; char format[10]; char filename[100]; 
	int check; int result; char trash[50]; int trash1; int opNum; int line = 5;
	check = sscanf(input_str,"%s %s %s",str,filename,trash);
	if(check!=2){
		printf("invalid input\n"); return; }
	FILE *fp = fopen(filename,"r");
	int loc; int cnt=0;
	if(Symbol.numOfData != 0){
		List_Free(&Symbol);
	}
	ListInit(&Symbol);
	if(fp == NULL){
		printf("File does not exist\n"); return; }
	LInsert(&History,input_str);
	FILE *fp2 = fopen("tmp.txt","w");
	if(fp == NULL){
		printf("File open error.\n"); return;}
	char data1[50]={'\0'}; char data2[50]={'\0'}; char data3[50]={'\0'}; 
	char data4[50]={'\0'}; char data5[50]={'\0'}; char data6[50]={'\0'}; //this is storage for tmpdata
	char label[50]={'\0'}; char opcode[50]={'\0'}; char operand[50]={'\0'};
	char ch1=0; int i1=0;
	while(1){
		memset(str,'\0',sizeof(str));
		i1 = 0;
		while(ch1!='\n' || ch1!=EOF){
			ch1=fgetc(fp);
			if(ch1=='\n'){
				str[i1++] = '\n';
				str[i1] = '\0';
				break;
			}
			else if(ch1 == EOF)
				return;
			else
				str[i1++] = ch1;
		}
		strcpy(copystr,str);
		space_remover(str);
		if(!strcmp(str,"\n")){
			fprintf(fp2,"%s",copystr);	
		}
		else
			break;
	}
	check = sscanf(str,"%s %s %s %s",data1,data2,data3,trash);
	if(check == 4 ){
		List_Free(&Symbol);
		printf("Input Error line %d\n",line); return; }
	if(check == 2){	
		strcpy(data3,data2); strcpy(data2,data1);
			}
	if(!strcmp(data2,"START")){ 
		if(!strcmp(data3,"0")){ // if addr start 0
			loc = 0;
			start_loc = 0;
		}
		else{
			loc = atoi(data3); // convert to integer
			start_loc = loc;
			if(loc == 0){ // if start addr does not a number
				printf("invalid start location line %d\n",line);
				List_Free(&Symbol);
				return; }
		}
			fprintf(fp2,"%04X %s",loc,copystr);
			strcpy(program_name,data1);
		}
	else if(!strcmp(data1,"START")){
		if(!strcmp(data2,"0"))
			loc = 0;
		else{
			loc = atoi(data2);
			if(loc ==0){
				printf("invalid start location line %d\n",line);
				List_Free(&Symbol);
				return; }
			strcpy(data3,data2);
			strcpy(data2,data1);
		}
		fprintf(fp2,"%04X %s",loc,copystr);
		strcpy(program_name,"SP_prog");
	}
	else {
		fseek(fp,0L,SEEK_SET);
		strcpy(program_name,"SP_prog");
		loc = 0;
	}
	while(1){
	    int	i=0; char ch=0;
		memset(str,'\0',sizeof(str));
		memset(copystr,'\0',sizeof(copystr));
		label[0] = '\0'; opcode[0]='\0'; operand[0]='\0';// initialize
		while(ch!='\n' || ch!=EOF){
			ch = fgetc(fp);
			if(ch=='\n'){
				str[i++]='\n';
				str[i]='\0';
				break;
			}
			else if(ch == EOF)  
				return;
			else
				str[i++]=ch;
		}
		strcpy(copystr,str);
		space_remover(str);
		if(!strcmp(str,"\n")){
			fprintf(fp2,"%s",str);
			continue;
		}
		int commaresult = commacheck(str); // check if string has comma in it
		check = sscanf(str,"%s %s %s %s",data1,data2,data3,trash);
		if(commaresult == 1){
			check = sscanf(str,"%s %s %s %s %s %s",data1,data2,data3,data4,data5,data6);
			if(check == 6){
				printf("input error. %s line %d\n",str,line);
				List_Free(&Symbol);
				return;
			}
			else if(check == 5){
				if(!strcmp(data4,",")){
					printf("input error. %s line %d\n",str,line);
					List_Free(&Symbol);
					return;
				}
				else{
					strcat(data3,data4);
					strcat(data3,data5);
					data4[0] = '\0'; data5[0] = '\0';
					check = 3;
				}
			}
			else if(check == 4){
				if(!strcmp(data3,",")){
						strcat(data2,data3);
						strcat(data2,data4);
						data3[0] = '\0'; data4[0] = '\0';
						check = 2;
				}
				else{
					strcat(data2,data3); data3[0] ='\0';
					check = 3;}
			}
			else if(check == 3){
				strcat(data2,data3);
				data3[0] = '\0';
				check = 2;
			}
			else if(check==1){
				printf("input error %s line %d\n",str,line);
				List_Free(&Symbol);
				return;
			}

		}
		if(!strcmp(data1,".")){  // if it is a comment line
			fprintf(fp2,"%s",str); // write it intermediate file
			line += 5;
			continue; }
		if(!strcmp(data1,"BASE")){ 
			fprintf(fp2,"%04X %10s",loc,copystr); 
			line +=5;
			continue; }
		if(!strcmp(data1,"END")){
			fprintf(fp2,"%-10X %15s",loc,copystr);
			end_loc = loc;
			break;
		}
		if(check==4){
			printf("Invalid input error. line %d\n",line);
			List_Free(&Symbol);
			return; }
		else if(check==3){ // there are label, opcode, and operand in the sentence
			strcpy(label,data1); strcpy(opcode,data2); strcpy(operand,data3);}
		else if(check==2){ // there are opcode and operand in the sentence
			strcpy(opcode,data1); strcpy(operand,data2);}
		else if(check==1){ // there is only operand in it
			strcpy(opcode,data1); }
		if(label[0]!='\0' && commaresult == 0){
			result = Symbol_Find(&Symbol,label,&trash1);
			if(result == -1){ // symbol is already exist in table
				printf("duplicate symbol %s error line %d\n",label,line);
				List_Free(&Symbol);
				return; };
			LInsert_Symbol(&Symbol,label,loc);
		}
	
		//search OPTAB for OPCODE
		if(label[0] >='0' && label[0] <= '9'){
			printf("Label cannot start with number line %d\n",line);
			List_Free(&Symbol);
			return;
		}
		result = SearchOPCODE(hashtable,opcode,format,&opNum);
		if(result == 1){ // non+ operation
			if(!strcmp(format,"1"))
				cnt = 1;
			else if(!strcmp(format,"2")) 
				cnt=2; //2bytes
			else if(opcode[0] == '+')
				cnt=4; //4bytes
			else
				cnt = 3;
		}
		
		else if(!strcmp(opcode,"WORD"))
			cnt=3; 
		else if(!strcmp(opcode,"RESW"))
			cnt=3 * (atoi(operand));
		else if(!strcmp(opcode,"RESB"))
			cnt= atoi(operand);
		else if(!strcmp(opcode,"BYTE")){
			if(operand[1] != '\'' && operand[strlen(operand)-1] !='\''){
				printf("BYTE INPUT ERROR line %d\n",line);
				List_Free(&Symbol);
				return;
			}
			if(operand[0] == 'X'){
				cnt = 1;
				for(int i=2;i<strlen(operand)-1;i++){
					if((operand[i] >= '0' && operand[i]<= '9') || (operand[i] >='A' && operand[i] <='F'))
						continue;
					else{
						printf("BYTE input error line %d\n",line);
						List_Free(&Symbol);
						return;
					}
				}
			}
			else if(operand[0] == 'C'){
				cnt = strlen(operand)-3;
			}
			else{
				printf("BYTE format Error line %d\n",line);
				List_Free(&Symbol);
				return; }
		}
		else{
			printf("opcode %s is does not exist line %d\n",opcode,line);
			List_Free(&Symbol);
			return;
		} 
		fprintf(fp2,"%04X %10s",loc,copystr);
		line+=5;
		loc+=cnt;
		}
	fclose(fp); fclose(fp2);
	pass2(filename,hashtable);
	return;
}
void pass2(char *filename,List hashtable[]){
	char lstfilename[50]; 
	char objfilename[50];
	int locarr[300]={0};
	char objcodearr[300][20]={'\0'};
	char operandarr[300][30] = {'\0'};
	int loclen=0;
	char * tok = strtok(filename,".");  int line=5;
	strcpy(lstfilename,tok);
	strcpy(objfilename,lstfilename);
	strcat(lstfilename,".lst"); // add .lst to filename
	strcat(objfilename,".obj");
	FILE *fpR = fopen("tmp.txt","r"); // open intermediate file
	FILE *fpW = fopen(lstfilename,"w"); // open filename.lst
	FILE *fpObj = fopen(objfilename,"w");
	
	if(fpR == NULL){
		printf("Intermediate file does not exist\n"); return; }
	if(fpW == NULL){
		printf("File open error\n"); return; }
	if(fpObj == NULL){
		printf("File open error\n"); return; }
	fprintf(fpObj,"%c",'H');
	fprintf(fpObj,"%s %06X%06X\n",program_name,start_loc,end_loc);
	// start	
	char str[MAX_INPUT_LEN]; char format[10]; int opNum; int addr; 
	int check; int result=0;  int loc; int base_addr=0; 
	char data1[50]; char data2[50]; char data3[50];  // this is temp arr 
	char label[50]; char opcode[50]; char operand[50]; char tmparr[50];
	char copystr[MAX_INPUT_LEN];
	char objectcode[10]; char register1[10]; char register2[10]; 
	char *ptr; char trash1[50]; 
	int bit=0; // this is bit for x,b,p,e check
	int end_flag = 0;
	while(1){
		char ch = 0; int i = 0;
		memset(str,'\0',sizeof(str));
		memset(copystr,'\0',sizeof(copystr));
		while(1){
			ch = fgetc(fpR);
			if(ch == '\n'){
				str[i] = '\0';
				break;
			}
			else if(ch == EOF){
				//fclose(fpR);
				//fclose(fpW);
				end_flag = 1;
				break; }
			else
				str[i++] = ch;
		}
		if(end_flag == 1) break;
		strcpy(copystr,str);
		space_remover(str);
		if(str[0]=='\0'){
			fprintf(fpW,"%s\n",copystr);
			continue;
		}
		label[0]='\0'; opcode[0]='\0'; operand[0]='\0'; bit=0;
		memset(objectcode,'\0',sizeof(objectcode)); // initialize objectcode arr
		check = sscanf(str,"%x %s %s %s %s",&loc,data1,data2,data3,trash1);
	//	printf("check : %d, %x data1 :%s data2 :%s data3 :%s trash: %s\n",check,loc,data1,data2,data3,trash1);
		locarr[loclen] = loc;
		if(!strncmp(str,".",1)){ // there is a comment line
			fprintf(fpW,"%-10d %-30s\n",line,str); line+=5;
			continue; }
		if(!strcmp(data1,"START") || !strcmp(data2,"START")){
			fprintf(fpW,"%-10d %-30s\n",line,copystr);
			line+=5;
			continue; }
		if(!strcmp(data1,"BASE")){
			fprintf(fpW,"%-10d %-30s\n",line,copystr); line+=5;
			result = Symbol_Find(&Symbol,data2,&base_addr); // save base addr
			continue;
			if(result == 1){
				printf("BASE addr Error\n"); return; }
		}
		if(!strcmp(data1,"END")){
			fprintf(fpW,"%-15d %-7s%-s\n",line,data1,data2); line+=5;
			continue;
		}
		if(check == 5){
			if(!commacheck(str)){
				printf("input error line %d\n",line);
				List_Free(&Symbol);
				return; }
			if(!strcmp(data3,",")){
				strcpy(opcode,data1);
				strcpy(operand,data2);
				strcat(operand,data3);
				strcat(operand,trash1);
				strcpy(operandarr[loclen],operand);
			}
			else{	
				strcpy(label,data1);
				strcpy(opcode,data2);
				strcpy(operand,data3);
				strcat(operand,trash1);
				strcpy(operandarr[loclen],operand);
			}
		}
		if(check==4){  // there are label, opcode, and operand
			if(commacheck(str)){
				strcpy(opcode,data1);
				strcpy(operand,data2);
				strcat(operand,data3);
				strcpy(operandarr[loclen],operand);
			}
			else{
				strcpy(label,data1);
				strcpy(opcode,data2);
				strcpy(operand,data3);
				strcpy(operandarr[loclen],operand);
			}
		}
		if(check==3){ // there are opcode and operand
			strcpy(opcode,data1);
			strcpy(operand,data2);
			strcpy(operandarr[loclen],operand);
		}
		if(check==2){ // there is only opcode ex : RSUB
			strcpy(opcode,data1);
		}
		//search OPTAB or OPCODE
		if(SearchOPCODE(hashtable,opcode,format,&opNum)==1 ){ // opcodefind 
			if(!strcmp(format,"1")){
				sprintf(tmparr,"%02X",opNum);
				strcpy(objectcode,tmparr);
			}
			if(!strcmp(format,"2")){
				sprintf(tmparr,"%02X",opNum);
				strcpy(objectcode,tmparr);
				int res = commacheck(operand);
				sscanf(operand,"%s , %s",register1,register2);
				if(res ==0){
					sprintf(tmparr,"%d",GetRegisterNum(register1));
					strcat(objectcode,tmparr);
					strcat(objectcode,"0");
				}
				else{
					if(!strcmp(opcode,"CLEAR") || !strcmp(opcode,"SVC") || !strcmp(opcode,"TIXR" )){
							printf("Register format error line %d\n",line);
							List_Free(&Symbol);
							return;
					}
					char *ptr = strtok(operand,",");
					strcpy(register1,ptr);
					ptr = strtok(NULL,",");
					strcpy(register2,ptr);
					sprintf(tmparr,"%d",GetRegisterNum(register1));
					strcat(objectcode,tmparr);
					sprintf(tmparr,"%d",GetRegisterNum(register2));
					strcat(objectcode,tmparr);
				}
			} // format 2 end
			else{ // format 3 or format 4
				int form = 3; int ni; // ni is check for n,i bits
				if(opcode[0]=='+'){
					form = 4;
				}
				if(operand[0]=='#'){ // if format4 and operand start with #
					ptr = operand+1;
					strcpy(operand,ptr);
					ni = 1;
					if(Symbol_Find(&Symbol,operand,&addr)){
						if(operand[0] >= '0' && operand[0] <='9'){
							int tmp = atoi(operand); 
							char arr[100];
							sprintf(arr,"%02X",opNum+ni);
							strcat(objectcode,arr);
							if(form == 4){
								strcat(objectcode,"1");
								sprintf(arr,"%05X",tmp);
							}
							else{
								sprintf(arr,"%03X",tmp);
								strcat(objectcode,"0");
							}
							strcat(objectcode,arr);
							fprintf(fpW,"%-10d %-30s %-10s\n",line,copystr,objectcode); line+=5;
							strcpy(objcodearr[loclen++],objectcode);
							continue; 
						}
					}
					else{
						printf("operand %s line %d error\n",operand,line);
						List_Free(&Symbol);
						return;
					}
				}
				else if(operand[0]=='@'){
					ptr = strtok(operand,"@");
					strcpy(operand,ptr);
					ni = 2;
				}
				else{
					ni = 3;
				}
			
				if(commacheck(operand) && Xcheck(operand)){ // this is check for using x bit
					ptr = strtok(operand,",");
					strcpy(operand,ptr);
					bit+=8;
				}
				if(Symbol_Find(&Symbol,operand,&addr)==1 && operand[0]!='\0'){
					printf("operand error line %s %d\n",str,line);
					List_Free(&Symbol);
					return;
				}
				opNum+=ni;
				sprintf(tmparr,"%02X",opNum); // opcode + ni is included in the object code
				strcpy(objectcode,tmparr);
				memset(tmparr,'\0',sizeof(tmparr));
				if(form == 4){
					strcat(objectcode,"1");
					char tmparr[50];
					sprintf(tmparr,"%05X",addr);
					strcat(objectcode,tmparr);
				}
				else{ // this is format 3
					int tmp_addr;  char addr_str[30];
					if(operand[0]=='\0'){ // there is no need for check if this opcode use x,b,p,e bits. ex : RSUB
						strcat(objectcode,"0000");
					}
					else{	
						// this is check for base relative or pc relative
						if(addr-loc-3 >= 2048  || addr-loc-3 < 4096){ // we should use base relative
							bit+= 4;
							tmp_addr = addr-base_addr;
						}
						if(addr-loc-3 < 2048 && addr-loc-3 >= -2048){ // we use pc relative
							bit+=2;
							tmp_addr = addr-loc-3;
						}
						sprintf(addr_str,"%03X",tmp_addr);
						if(tmp_addr<0){
							char *ptr = addr_str+5;
							strcpy(addr_str,ptr);
						}
						sprintf(tmparr,"%X",bit); // this is used for x,b,p,e
						strcat(objectcode,tmparr);
						strcat(objectcode,addr_str); // this is process for including dis/addr to obejectcode		
					}
				} // format 3 end
			} // format 3,4 end
	
		}
		else if(!strcmp(opcode,"RESW") || !strcmp(opcode,"RESB")){ // if opcode is RESW or RESB
			fprintf(fpW,"%-10d %-10s\n",line,copystr); line+=5;
			objcodearr[loclen++][0] = '\0';
			continue;
		}
		else if(!strcmp(opcode,"BYTE")){ // BYTE
			if(operand[0]=='C'){
				for(int i=2;i<strlen(operand)-1;i++){
					memset(tmparr,'\0',sizeof(tmparr));
					sprintf(tmparr,"%X",operand[i]);
					strcat(objectcode,tmparr);
				}
			}
			else if(operand[0]=='X'){
				int k=0;
				for(int i=2;i<strlen(operand)-1;i++)
					objectcode[k++] = operand[i];
			}
			else{
				printf("input Format Error line %d\n",line); 
				List_Free(&Symbol);
				return;
			}
		}
		
		else if(!strcmp(opcode,"WORD")){ 
			if(strlen(operand)>6){
				printf("Word range error line %d\n",line);
				List_Free(&Symbol);
				return; 
			}
			for(int i=0;i<strlen(operand);i++){
				if(operand[i] <='0' || operand[i] >= '9'){
					printf("Word should be followed by number. line %d\n",line);
					List_Free(&Symbol);
					return;
				}
			}
			int num = atoi(operand);
			sprintf(objectcode,"%06d",num);
		}
		else{
			printf("opcode %s does not exist. line %d\n",opcode,line);
			List_Free(&Symbol);
			return;
		}
		fprintf(fpW,"%-10d %-30s %-10s\n",line,copystr,objectcode);
		strcpy(objcodearr[loclen++],objectcode);
		line+=5;

	} // this is end for while

	// this is for .obj file
	int cutter = 0;
	int remember = 0;
	char Tstr[1000]={'\0'};

	for(int i =0;i<loclen;i++){
		while(cutter <= 30 || objcodearr[i][0] != '\0'){
			if(strlen(objcodearr[i])/2 + cutter > 29 || objcodearr[i][0] == '\0'){ // if strlen over 29(1D) or there is RESW or RESB
				char tmp[100]; char tmp1[100];  // write it on the file
				if(strlen(Tstr)!=0){
					sprintf(tmp,"%06X",remember);
					fprintf(fpObj,"%s","T");
					fprintf(fpObj,"%s",tmp);
					sprintf(tmp1,"%02X",(unsigned)strlen(Tstr)/2);
					fprintf(fpObj,"%s",tmp1);
					fprintf(fpObj,"%s\n",Tstr);
				}

				memset(Tstr,'\0',sizeof(Tstr));
				strcat(Tstr,objcodearr[i]);
				cutter = strlen(objcodearr[i])/2;
				if(objcodearr[i][0] == '\0')
					remember = locarr[i+1];
				else
					remember = locarr[i];
				break;
				}

			else{
				cutter += strlen(objcodearr[i]) / 2;
				strcat(Tstr,objcodearr[i++]);		
			}
		}
	}
	int tmplen = 0;
	char  mtmparr[50] = {'\0'};
	for(int i = 0;i<loclen;i++){  // This is for M______
		if(strlen(objcodearr[i]) == 8 && operandarr[i][0] !='#'){
			fprintf(fpObj,"%s","M");
			sprintf(mtmparr,"%06X",tmplen+1);
			strcat(mtmparr,"05");
			fprintf(fpObj,"%s\n",mtmparr);
		}	
		tmplen += strlen(objcodearr[i])/2 ;
	}
	fprintf(fpObj,"%s\n","E000000");
	fclose(fpObj);
	fclose(fpR);
	fclose(fpW);
	printf("output file : [%s] , [%s]\n",lstfilename,objfilename);
	return;
}
int Xcheck(char opcode[]){
	int len = strlen(opcode);
	for(int i=0;i<len;i++){
		if(opcode[i] == 'X')
			return 1;
	}
	return 0;
}
int GetRegisterNum(char Register[]){
	if(!strcmp(Register,"A")) return 0;
	if(!strcmp(Register,"X")) return 1;
	if(!strcmp(Register,"L")) return 2;
	if(!strcmp(Register,"B")) return 3;
	if(!strcmp(Register,"S")) return 4;
	if(!strcmp(Register,"T")) return 5;
	if(!strcmp(Register,"F")) return 6;
	if(!strcmp(Register,"PC")) return 8;
	if(!strcmp(Register,"SW")) return 9;

	return -1;
}


