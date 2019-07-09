#ifndef __P1__H__//prevent overlap header file
#define __P1__H__//prevent overlap header file
#define MAX 3
#define MAX_INPUT_LEN 1000
#define MAX_BOUND 1048575
#define DUMP_NUM 160
typedef enum { help , dir , history , dump, dump_start, dump_start_end , edit , fill , 
			   reset , opcodemnmonic , opcodelist , type , touch , echo , date , assemble , quit , symbol, progaddr, loader, Bp, bp_clear, bp_set } Command; // command를 구분하기 위한 enum 선언이다
typedef enum { A, X, L, B, S, T, F, PC, SW}regi;
typedef struct _node
{
	char *data;
	char *data2;
	int num;
	struct _node * next;
} Node; // list Node를 정의한 structure

typedef struct _linkedList
{
	Node * head;
	Node * tail;
	int numOfData;
}List; // List structure
typedef struct _estab
{
	char section[50];
	char symname[50];
	int addr;
	int length;
	int datacheck;
}Estab;
typedef struct _IDX{
	int i;
	int j;
}Idx_info; //dump의 마지막 주소를 저장하는 structure이다

List History;
List Symbol;
int start_loc = 0;
int end_loc = 0;
int Progaddr=0; int Progaddr_set=0;
int bp[100]={0}; int bp_len=-1;
int bp_point = 0;
int reg[8]={0};
unsigned char memory[65536][16];
char program_name[MAX_INPUT_LEN] = {'\0'};
void OpPrint(List list[]);
void ListInit(List * plist);
//**********************************************//
// List를 초기화 해주는 함수이다               //
//*********************************************//
void LInsert(List * plist, char *str);
//*******************************************//
//  List에Insert기능을 위한 함수이다         //
//  History를 위한 기능이다                 //
//******************************************//
void LInsert1(List * plist, char *code, char *op,int num);
//*********************************************//
//    List에 Insert기능을 위한 함수이다       //
//    Opcode를 출력하기 위한 함수이다         //
//*********************************************//
void List_Free(List *plist);
//***********************************//
//    List를 free해주는 함수이다     //
//**********************************//
void Lprint(List * plist);
//*************************************//
//      History를 출력해주는기능을 하는 //
//      함수이다                       //
//************************************//
void FindOPCODE(List LIST[],char * str,char *str_copy);
//***********************************************//
//     Opcode를 hashtable에서 검색하는 함수이다  //
//     명령어가 존재하면 명령어 번호를 출력한다  //
//**********************************************//

int convert_to_command(char *str,char *strcopy);
//*****************************************************//
//   input이 어떤 명령어인지 반환해 주는 함수이다      //
// ****************************************************//
int hashfunc(int num,int len);
//****************************************************//
//    hashfunction의 기능을 하는 함수로서 num이 20보다//
//    크면 opcode와 문자열의 조합으로 value를 만든다  //
//***************************************************//
int commacheck(char *str);
//********************************//
//  문자열에 comma가 있는지 검사한다//
//*******************************//
void readfile(List list[]);
//***********************************************//
//   opcode.txt를읽어들여 hastable을 생성하는 함수 //
//   파일이 존재하지 않으면 프로그램을 종료한다   //
//**********************************************//
void print_help();
//*********************************************//
//   help 명령어를 위한 함수이다              //
//********************************************//
void print_dir();
//***************************************////************************************************//
//    opcodelist 명령어를 처리하는 함수이다      //
//    hasfunction에 저장된 opcode들을출력해준다  //
//                                               //
//***********************************************//
void dump_print(unsigned char memory[][16],Idx_info* idx,int cnt); 
//*************************************************//
//   명령어 dump를 처리하기 위한 함수이다          //
//   cnt에는 출력 되어야 할 개수가 저장된다        //
//************************************************//
void dump_print_start(unsigned char memory[][16],Idx_info* idx,char *input_str,char *str_copy);
//*****************************************************************//
//   명령어 dump와 시작 주소가 같이 입력했을때를 처리하는 함수이다 //
//*****************************************************************//
void dump_print_start_end(unsigned char memory[][16],Idx_info* idx,char *input_str,char *str_copy);
//******************************************************************//
// 명령어 dump와 시작주소와 끝주소가 같이 입력됐을때를 처리하는 함수이다//
//******************************************************************//
void Reset_Memory(unsigned char memory[][16]);
//***************************************************//
//     메모리를 모두 0으로 초기화 해주는 함수이다     //
//***************************************************//
void edit_memory(unsigned char memory[][16],char *input_str,char *str_copy);
//*********************************************************//
//  edit 명령어를 수행하는 함수이다 지정된 주소를 입력된  //
//  값으로 바꾸어 주는 기능을 한다                          //
//********************************************************//
void fill_memory(unsigned char memory[][16], char *input_str,char *str_copy);
//************************************************************************//
//  fill 명령어를 수행하는 함수이다 지정된 주소들을 입력된 //
//  값으로 바꾸어 주는 기능을 한다                        //
// *******************************************************//
void space_remover(char input_str[]);
//***************************************//
// 문자열 앞뒤 공백을 제거해주는 함수이다//
// **************************************//
void Echo(char *str);
//*********************************************************//
// Echo명령어를 추가구현한 함수이다 echo "string" 혹은    //
// echo string을 입력받아 그 값을 콘솔에 출력해준다       //
//*********************************************************//
void Touch(char *str);
//*********************************************************//
//  Touch 명령어를 추가구현한 함수이다 Touch filename을    //
// 입력하면 그 파일을 생성해 주는 기능을 한다              //
//*********************************************************//
void Type(char *str);
//**********************************************************//
//  cat 명령어를 추가구현한 함수이다 cat filename을입력하면 //
//  그 파일의 내용을 출력해 주는 기능을 한다                 //
//**********************************************************//
void Date_print();
//***************************************************//
//  date 명령어를 추가구현한 함수이다 date를 입력하면 //
//  현재 시간을 출력해주는 기능을 한다                //
// **************************************************//
void pass1(char *input_str,char *str_copy,List hashtable[]); // pass1 algorithm 구현
void pass2(char *filename,List hashtable[]); // pass2 algorithm
int GetRegisterNum(char Register[]); // Return register number
int Xcheck(char operand[]); // Check function whether use X regiter or not
void setaddr(char str[]); // set start_addr


void loader_pass1(char str[]); //This is function for linkingloader
void Estab_init(Estab estab[],int num); // This is function for Initiate estab
void ShowEstab(Estab estab[],int len); // This if function for showing Estable
void Edit(int addr,int num,int mode,int msb); // This is function for edit memory for given number
int Search_estab(Estab estab[],char str[],int len); // This is fuction for check if the symbol is already exist
void clear_bp(); // this is function for clear breaking point
void set_bp(char str[]); // this is function for set breaking point
void show_bp(); // this is function for showing breaking point
#endif//prevent overlap header file
