#ifndef __LINKER__C__
#define __LINKER__C__
#include "20141544.c"
void clear_bp(){
	for(int i=0;i<100;i++){
		bp[i]=0;
	}
	bp_len = -1; // bp_len is variable for checking the number of break point
	printf("\t\t[ok] clear all breakpoints\n");
	return;
}
void set_bp(char str[]){
	char tmp[20]; int num; char trash;
	int check = sscanf(str,"%s %X %c",tmp,&num,&trash);
	if(check == 3){
		printf("invalid input\n");
		return;
	}
	LInsert(&History,str);

	bp[++bp_len]=num;
	printf("\t\t[ok] create breakpoint %X\n",num);
}
void show_bp(){
	if(bp_len==-1){ // if there is no break point
		printf("There is no break point\n");
		return;
	}
	printf("\t\tbreakpoint\n");
	printf("\t\t----------\n");
	for(int i=0;i<=bp_len;i++){
		printf("\t\t%X\n",bp[i]);
	}
}

void loader_pass1(char str[]){
	char filestr[50]; char trash1[50]; char trash2;
	char tmpstr[20]; char number[20];
	char tmp1[10], tmp2[10];
	char prog[3][20]; 
	int start_addr[3]={0};
	char Hname[3][10];
	char DTab[10][10]={'\0'};
	FILE *fp[3];
	int addr1,addr2;
	int line=0; int tmpaddr=0;
	Estab estab[50];	
	Estab_init(estab,50);
	int current_addr = Progaddr;
	int check =sscanf(str,"%s %s %s %s %c",trash1,prog[0],prog[1],prog[2],&trash2); // set program name
	if(Progaddr_set==0){ // if progaddr didn't set
		Progaddr = 0; // set it with 0
	}
	if(check==5){
		printf("invalid input\n");
		return;
	}
	int k=0;
	LInsert(&History,str);
	if(check==1){
		printf("invalid input. Please input name of the file\n");
		return;
	}
	for(int i=0;i<check-1;i++){ // check-1 is the number of file
		memset(tmp1,'\0',10);
		memset(tmp2,'\0',10);
		fp[i] = fopen(prog[i],"r");
		if(fp[i] == NULL){
			printf("File does not exist. Please check the file name\n");
			return;
		}
		fscanf(fp[i],"%[^\n]",filestr);
		if(line==0){ // this is for Hrecode
			tmpaddr = current_addr;
			start_addr[i] = current_addr;
			sscanf(filestr,"%s %s",tmpstr,number);
			strcpy(estab[k].section,tmpstr+1);
			strcpy(Hname[i],tmpstr+1);
			strcpy(tmp2,number+6);
			tmp1[6]='\0';
			addr1 = strtoul(tmp1,NULL,16);
			addr2 = strtoul(tmp2,NULL,16);
			estab[k].length = addr2-addr1;
			estab[k].datacheck=1;
			estab[k++].addr = current_addr; // end for Hrecord
			current_addr+= estab[k-1].length;
			line++;
		}
		// this is for Drecord
		if(check!=2){
			fscanf(fp[i],"%s",filestr);
			strcpy(estab[k].symname,filestr+1);
			while(1){
				fscanf(fp[i],"%s",filestr);
				if(strlen(filestr)>6){
					strcpy(tmp2,filestr+6);
					filestr[6]='\0';
					addr1 = strtoul(filestr,NULL,16);
					estab[k++].addr = tmpaddr + addr1;
					int check = Search_estab(estab,tmp2,k);
					if(check != -1){
						printf("Duplicate Definition Error!\n");
						return;
					}
					strcpy(estab[k].symname,tmp2);
				}
				else{
					addr1 = strtoul(filestr,NULL,16);
					estab[k++].addr = tmpaddr + addr1;
					line=0;
					break;
				}	
			}//end for while
		}
	}
	int j=1;
	int setbit=0;
	for(int i=0;i<check-1;i++){ // this is for Rrecord
		j=1;
		if(check!=2){
			strcpy(DTab[j++],Hname[i]);
			fscanf(fp[i],"%s",filestr);
			strcpy(DTab[j++],filestr+3);
			while(1){
				fscanf(fp[i],"%s",filestr);
				if(!strcmp(filestr,"."))
					break;
				strcpy(DTab[j++],filestr+2);
			} // end for DTab
		}
		while(1){ // this is for Trecord
			fscanf(fp[i],"%s",filestr);
			if(!strcmp(filestr,"."))
				continue;
			if(filestr[0]=='M' || filestr[0]=='E')
				break;
			strncpy(tmpstr,filestr+1,6); // T000000
			int addr = strtoul(tmpstr,NULL,16);
			addr+=start_addr[i];
			strncpy(tmpstr,filestr+7,2);
			tmpstr[2]='\0';
			int len = strtoul(tmpstr,NULL,16);
			for(int i=0;i<len;i++){
				strncpy(tmpstr,filestr+9+2*i,2);
				int tmp=strtoul(tmpstr,NULL,16);
				Edit(addr++,tmp,0,0);
			}
		}
		while(1){ // this is for Mrecord
			if(filestr[0]=='E') // when this is E record
				break;
			strncpy(tmpstr,filestr+1,6);
			filestr[7]='\0';
			int tmpaddr = strtoul(tmpstr,NULL,16);
			tmpaddr += start_addr[i];
			strncpy(tmpstr,filestr+10,2);
			tmpstr[2]='\0';
			int change_num = atoi(tmpstr);
			int change_addr = Search_estab(estab,DTab[change_num],k); //Search DTab str in Estable
			if(change_addr == -1){
				printf("Symbol does not defined. Please check Rrecord again\n");
				return;
			}
			if(filestr[9]=='+') 
				setbit = 1;
			else
				setbit = 2;
			if(change_addr<65536){
				Edit(tmpaddr+1,change_addr/256,setbit,1);
				Edit(tmpaddr+2,change_addr%256,setbit,0);
			}
			else{
				Edit(tmpaddr,change_addr/256*16,setbit,1);
				Edit(tmpaddr+1,change_addr/256,setbit,0);
				Edit(tmpaddr+2,change_addr%256,setbit,0);
			}
			fscanf(fp[i],"%s",filestr);
		}
	}
	ShowEstab(estab,k); // this is function for showing Estable
	for(int i=0;i<check-1;i++)
		fclose(fp[i]); // close file

	return;

}
void Edit(int addr,int num,int mode,int msb){// mode 0 : normal mode 1 : + mode 2 : -
	int i,j;
	i = addr/16;
	j = addr%16;

	if(mode==0)
		memory[i][j] = num; // this is for Trecord. Just fill addr with value
	else if(mode==1){ // This is mode for '+'
		int tmp = memory[i][j]+num;
		memory[i][j] += num;
		if(msb ==0 && tmp>=256){ // if there is carry
			memory[i][j-1]+=1;
			if(memory[i][j-2]==255) 
				memory[i][j-2]=0;
		}
	}
	else{ // This is mode for '-'
		int tmp = memory[i][j]-num;
		if(msb ==0 && tmp<0){ // if there is carry
			memory[i][j-1]-=1;
			if(memory[i][j-2]==0) 
				memory[i][j-2]=255;
		}
		memory[i][j] -= num;
		
	}
}

void ShowEstab(Estab estab[],int len){
	int total_length=0;
	printf("control		symbol		address		length\n");
	printf("section		name\n");
	printf("-------------------------------------------------------------\n");
	for(int i=0;i<len;i++){
		if(estab[i].section[0]!='\0')
			printf("%-15s %-15s %-15X\t%04X\n",estab[i].section,estab[i].symname,estab[i].addr,estab[i].length);
		else
			printf("%-15s %-15s %-15X\n",estab[i].section,estab[i].symname,estab[i].addr);
		total_length+=estab[i].length;
	}
	printf("------------------------------------------------------------\n");
	printf("\t\t\t\t\ttotal length %04X\n",total_length);
	return;
}
void Estab_init(Estab estab[],int num){
	for(int i=0;i<num;i++){
		estab[i].section[0]='\0';
		estab[i].symname[0]='\0';
		estab[i].datacheck=0;
		estab[i].length=0;
	}
}
int Search_estab(Estab estab[],char str[],int len){
	for(int i=0;i<len;i++){
		if(!strcmp(str,estab[i].section) || !strcmp(str,estab[i].symname)){
			return estab[i].addr;
		}
	}
	return -1;
}

#endif
