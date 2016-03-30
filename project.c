#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>

struct _Node {
	struct _Node *prev;
	int data;
	struct _Node *next;
};

typedef struct _Node Node;

struct _List {
	Node *head;
	Node *tail;
	Node *curr; 
	int count; //실제 노드의 개수
	int wol_count[12];
};

typedef struct _List List;

enum kind_of_card {

	//0~9 광
	gwang = 0, //광
	bi_gwang = 1, //비광
	//10~ 십(끗 : 동물계)
	yeol = 10, //일반동물
	godori = 11, //특수동물(새)
	//20~ 오(띠)
	chungdan = 20,
	hongdan = 21,
	chodan = 22,
	bidan = 23,
	//30~ 피
	pi = 30,
	ssangpi = 31,
	gukjin = 32 //국진 (원래는 십 계열)

};

//링크드 리스트 관련 함수들
void create(List *); //더블 링크드 리스트의 생성
Node * append_tail(List *, int); //맨 끝에 노드를 삽입해주는 함수 리턴값 : 삽입에 성공한 노드의 주소를 반환, 실패시 NULL
Node * append_head(List *, int); //맨 처음에 노드를 삽입해주는 함수 리턴값 : 삽입에 성공한 노드의 주소를 반환, 실패시 NULL
void display(List *); //출력
Node * insert_before(List *, Node *, int); //해당 데이터의 앞부분에 노드를 삽입
Node * insert_after(List *, Node *, int); //해당 데이터의 뒷부분에 노드를 삽입
Node * delete_node(List *, Node *); //노드 삭제 성공시 NULL 반환, 실패시 그 노드의 주소를 반환
Node * search_node(List *, int); //해당 노드의 데이터가 있는 곳의 노드 주소를 못 찾으면 NULL, 찾으면 그 노드의 주소를 반환
void destroy_all(List *); //모두삭제, head와 tail 까지 해제
Node * search_by_index(List *, int); //0 : 첫번째 노드, 1 : 두번째 노드......
Node * find_by_pi(List *); //해당 리스트에 피가 있는지를 반환 (일단 피가 있으면 피의 주소를 리턴하고 피가 없고 쌍피만 있으면 쌍피의 주소값을 리턴) NULL : 없음. 주소값 : 해당 node에 있음

//save & load 관련 함수들
void save_data(); //저장할 것은 카드 dummy의 순서, 카드들의 배치 상태, 선이 누구인가, 점수는 어떻게 되는가, 몇뻑, 흔들기 여부, 몇 고, 배수 상태는 어떻게 되는가 등
void load_data(); //위의 내용을 로드함 (변수들에 값을 대입)
void init(); //로드한 것들을 토대로 초기화

//카드 이동 관련 함수들
void move_card(List *, int, List *); //카드 이동 (by data)
Node * move_card_by_index(List *, int, List *); //카드 이동 (by index)
Node * find_by_wol(List *, int); //검색해서 해당 월이 있는 노드의 데이터를 반환 (ex : 100, 101 모두 1월이다. 사용자가 1을 인자로 넘기면 100을 리턴)
Node * find_by_wol_refer_to_index(List *, int, int); //검색해서 해당 월이 있는 노드의 데이터를 반환 (ex : 100, 101 모두 1월이다. 사용자가 1을 인자로 넘기면 100을 리턴)
int get_wol(int); //월 값을 리턴 (1~12)
int is_pi(int); //피 값을 리턴 (0 : 피가 아님, 1 : 1피, 2 : 쌍피)
int is_gwang(int); //광 값을 리턴 (0 : 광이 아님, 1 : 광, 2 : 비광)
int is_yeol(int); //열 값을 리턴 (0 : 아님 1 : 그냥 동물, 2 : 고도리 동물)
int is_oh(int); //오(띠) 값을 리턴 (0 : 띠가 아님, 1 : 청단 2 : 홍단 3 : 초단 4 : 그냥 단)
void sort_cards(List *); //카드를 데이터 순서대로 오름차순으로 정렬함
void shuffle_cards(List *); //카드의 데이터를 섞음
void card_steal(char); //다른 플레이어로부터 피를 한 장씩 가져온다.

//게임 제어 관련 함수들
void draw(); //게임 창을 그린다. 사용자의 입력을 대기한다.
void draw2(); //단순히 현재 노드 상태에 대한 그리기만 수행한다.
void viewList(List *); //카드 정보를 출력한다. ex)1월 광 2월 피 .... 
void next_turn();
void show_property(int); //int data에 카드 속성을 넣으면 문자열로 출력해준다. (11 / 28 추가)
void sort_print_by_kind(List *list); //점수 계산이 쉽도록 출력
void show_help(); //도움말을 보여줌
void show_balance(); //잔고 보기
void stop_game(); //이번 판을 끝내고 다음 판 진행 준비하는 함수.
void restart_game();

//각종 게임에 관한 판단
int is_chongtong(List *); //총통 여부를 판별한다. 0 : 총통 아님, 1 : 총통임
int is_chongtong2(List *); //다시 판별
int is_hnndle(List *); //흔들기? 0 : 안흔들거임. 1 : 흔들거임(Auto)
int score_count(List *list); //by 최승욱, 인자 : 내가 가지고 있는 카드 list 구조체를 받아 거기에 대한 점수를 리턴
void check_score(); //점수가 났는가 체크
void check_final_score(); //최종 점수 계산(만)

//보조 함수들
void swap(int *, int *); //두 변수의 값들을 맞바꿈
int compare_int(const void *, const void *); //두 정수를 비교함
void setColor(int); //색상 지정해서 출력하기
int getch();

//전역 변수들
char turn = 'A'; //턴 (A, B, C)
char nturn = 'A'; //전에 이긴 사람 턴
char prev_go = 'X'; //전에 고한 사람
int hnndle[3] = {0, 0, 0}; //흔들었는가? (0 : 흔들지 않음, 1 : 한 번 흔듦, 2 : 두 번 흔듦)

int score_ingame[3] = {0, 0, 0}; //ingame_score
int high_score[3] = {3, 3, 3}; //이 점수 이상이 되어야 난 걸로 처리한다.

int score_multi[3] = {1, 1, 1}; //점수 배율 (1 : 1배 2 : 2배...)

int go_[3] = {0}; //[0] : a, [1] : b, [2] : c

int money[3] = {100000, 100000, 100000};
int money_multi[3] = {1, 1, 1}; //돈만 2배로 계산할 경우를 위한 변수. 자기가 뺏길 돈의 배율이라는 뜻.
int is_nagari = 0; //전 판에 나가리가 났으면 1, 아니면 0
int is_toggled = 0;//국진을 toggle(쌍피->열끗) 했으면 1, 아니면 0 
//current turn List * 두 개를 전역으로 할까.
FILE *fp;
List dummy, dropped, hand_1p, hand_2p, hand_3p, have_1p, have_2p, have_3p;
int buffer[48]; //빠른 섞기를 위한 버퍼 or 로드 시 카드들의 순서를 기억할 수 있는 배열 (dummy, dropped, hand_1p, 2p, 3p, have_1p, 2p, 3p 순서)
const int properties[48] = {
	//각 화투 카드의 속성을 정의한다.
	1 * 100 + gwang, 1 * 100 + hongdan, 1 * 100 + pi, 1 * 100 + pi,
	2 * 100 + godori, 2 * 100 + hongdan, 2 * 100 + pi, 2 * 100 + pi,
	3 * 100 + gwang, 3 * 100 + hongdan, 3 * 100 + pi, 3 * 100 + pi,
	4 * 100 + godori, 4 * 100 + chodan, 4 * 100 + pi, 4 * 100 + pi,
	5 * 100 + yeol, 5 * 100 + chodan, 5 * 100 + pi, 5 * 100 + pi,
	6 * 100 + yeol, 6 * 100 + chungdan, 6 * 100 + pi, 6 * 100 + pi,
	7 * 100 + yeol, 7 * 100 + chodan, 7 * 100 + pi, 7 * 100 + pi,
	8 * 100 + gwang, 8 * 100 + godori, 8 * 100 + pi, 8 * 100 + pi,
	9 * 100 + gukjin, 9 * 100 + chungdan, 9 * 100 + pi, 9 * 100 + pi,
	10 * 100 + yeol, 10 * 100 + chungdan, 10 * 100 + pi, 10 * 100 + pi,
	11 * 100 + gwang, 11 * 100 + ssangpi, 11 * 100 + pi, 11 * 100 + pi,
	12 * 100 + bi_gwang, 12 * 100 + yeol, 12 * 100 + bidan, 12 * 100 + ssangpi
};

int getch()
{
	struct termios oldt, newt;
	int ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON|ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
}

void save_data()
{
	//현재 상태를 파일로 저장.
	int i;
	fp = fopen("save.txt", "w");
	fprintf(fp, "turn=%c\n", turn);
	fprintf(fp, "nturn=%c\n", nturn);
	fprintf(fp, "prev_go = %c\n", prev_go);
	fprintf(fp, "hnndle[0]=%d\n", hnndle[0]);
	fprintf(fp, "hnndle[1]=%d\n", hnndle[1]);
	fprintf(fp, "hnndle[2]=%d\n", hnndle[2]);
	fprintf(fp, "score_ingame[0]=%d\n", score_ingame[0]);
	fprintf(fp, "score_ingame[1]=%d\n", score_ingame[1]);
	fprintf(fp, "score_ingame[2]=%d\n", score_ingame[2]);
	fprintf(fp, "high_score[0]=%d\n", high_score[0]);
	fprintf(fp, "high_score[1]=%d\n", high_score[1]);
	fprintf(fp, "high_score[2]=%d\n", high_score[2]);
	fprintf(fp, "score_multi[0]=%d\n", score_multi[0]);
	fprintf(fp, "score_multi[1]=%d\n", score_multi[1]);
	fprintf(fp, "score_multi[2]=%d\n", score_multi[2]);
	fprintf(fp, "go_[0]=%d\n", go_[0]);
	fprintf(fp, "go_[1]=%d\n", go_[1]);
	fprintf(fp, "go_[2]=%d\n", go_[2]);
	fprintf(fp, "money[0]=%d\n", money[0]);
	fprintf(fp, "money[1]=%d\n", money[1]);
	fprintf(fp, "money[2]=%d\n", money[2]);
	fprintf(fp, "money_multi[0]=%d\n", money_multi[0]);
	fprintf(fp, "money_multi[1]=%d\n", money_multi[1]);
	fprintf(fp, "money_multi[2]=%d\n", money_multi[2]);
	fprintf(fp, "is_nagari=%d\n", is_nagari);
	fprintf(fp, "is_toggled=%d\n", is_toggled);

	// dummy, dropped, hand_1p, hand_2p, hand_3p, have_1p, have_2p, have_3p 순서.
	(&dummy)->curr = (&dummy)->head->next;
	fprintf(fp, "dummy %d\n", (&dummy)->count);
	for (i=0; i < (&dummy)->count; i++)
	{
		fprintf(fp, "%d\n", (&dummy)->curr->data);
		(&dummy)->curr = (&dummy)->curr->next;
	}
	(&dropped)->curr = (&dropped)->head->next;
	fprintf(fp, "dropped %d\n", (&dropped)->count);
	for (i=0; i < (&dropped)->count; i++)
	{
		fprintf(fp, "%d\n", (&dropped)->curr->data);
		(&dropped)->curr = (&dropped)->curr->next;
	}
	(&hand_1p)->curr = (&hand_1p)->head->next;
	fprintf(fp, "hand_1p %d\n", (&hand_1p)->count);
	for (i=0; i < (&hand_1p)->count; i++)
	{
		fprintf(fp, "%d\n", (&hand_1p)->curr->data);
		(&hand_1p)->curr = (&hand_1p)->curr->next;
	}
	(&hand_2p)->curr = (&hand_2p)->head->next;
	fprintf(fp, "hand_2p %d\n", (&hand_2p)->count);
	for (i=0; i < (&hand_2p)->count; i++)
	{
		fprintf(fp, "%d\n", (&hand_2p)->curr->data);
		(&hand_2p)->curr = (&hand_2p)->curr->next;
	}
	(&hand_3p)->curr = (&hand_3p)->head->next;
	fprintf(fp, "hand_3p %d\n", (&hand_3p)->count);
	for (i=0; i < (&hand_3p)->count; i++)
	{
		fprintf(fp, "%d\n", (&hand_3p)->curr->data);
		(&hand_3p)->curr = (&hand_3p)->curr->next;
	}
	(&have_1p)->curr = (&have_1p)->head->next;
	fprintf(fp, "have_1p %d\n", (&have_1p)->count);
	for (i=0; i < (&have_1p)->count; i++)
	{
		fprintf(fp, "%d\n", (&have_1p)->curr->data);
		(&have_1p)->curr = (&have_1p)->curr->next;
	}
	(&have_2p)->curr = (&have_2p)->head->next;
	fprintf(fp, "have_2p %d\n", (&have_2p)->count);
	for (i=0; i < (&have_2p)->count; i++)
	{
		fprintf(fp, "%d\n", (&have_2p)->curr->data);
		(&have_2p)->curr = (&have_2p)->curr->next;
	}
	(&have_3p)->curr = (&have_3p)->head->next;
	fprintf(fp, "have_3p %d\n", (&have_3p)->count);
	for (i=0; i < (&have_3p)->count; i++)
	{
		fprintf(fp, "%d\n", (&have_3p)->curr->data);
		(&have_3p)->curr = (&have_3p)->curr->next;
	}
	fprintf(fp, "#고스톱 프로젝트 세이브 파일입니다.");
	fclose(fp);
	printf("saved");
	getch();getch();
}

void load_data()
{
	//현재 상태를 파일로부터 꺼내옴
	int i, cnt, buf;
	fp = fopen("save.txt", "r");
	if(!fp) {
		printf("로드할 데이터가 없습니다.\n");
		return;
	}
	fscanf(fp, "turn=%c\n", &turn);
	fscanf(fp, "nturn=%c\n", &nturn);
	fscanf(fp, "prev_go = %c\n", &prev_go);
	fscanf(fp, "hnndle[0]=%d\n", &hnndle[0]);
	fscanf(fp, "hnndle[1]=%d\n", &hnndle[1]);
	fscanf(fp, "hnndle[2]=%d\n", &hnndle[2]);
	fscanf(fp, "score_ingame[0]=%d\n", &score_ingame[0]);
	fscanf(fp, "score_ingame[1]=%d\n", &score_ingame[1]);
	fscanf(fp, "score_ingame[2]=%d\n", &score_ingame[2]);
	fscanf(fp, "high_score[0]=%d\n", &high_score[0]);
	fscanf(fp, "high_score[1]=%d\n", &high_score[1]);
	fscanf(fp, "high_score[2]=%d\n", &high_score[2]);
	fscanf(fp, "score_multi[0]=%d\n", &score_multi[0]);
	fscanf(fp, "score_multi[1]=%d\n", &score_multi[1]);
	fscanf(fp, "score_multi[2]=%d\n", &score_multi[2]);
	fscanf(fp, "go_[0]=%d\n", &go_[0]);
	fscanf(fp, "go_[1]=%d\n", &go_[1]);
	fscanf(fp, "go_[2]=%d\n", &go_[2]);
	fscanf(fp, "money[0]=%d\n", &money[0]);
	fscanf(fp, "money[1]=%d\n", &money[1]);
	fscanf(fp, "money[2]=%d\n", &money[2]);
	fscanf(fp, "money_multi[0]=%d\n", &money_multi[0]);
	fscanf(fp, "money_multi[1]=%d\n", &money_multi[1]);
	fscanf(fp, "money_multi[2]=%d\n", &money_multi[2]);
	fscanf(fp, "is_nagari=%d\n", &is_nagari);
	fscanf(fp, "is_toggled=%d\n", &is_toggled);

	// dummy, dropped, hand_1p, hand_2p, hand_3p, have_1p, have_2p, have_3p 순서.
	//1) 노드를 파괴한다.
	destroy_all(&dummy); destroy_all(&dropped); destroy_all(&hand_1p); destroy_all(&hand_2p); destroy_all(&hand_3p); destroy_all(&have_1p); destroy_all(&have_2p); destroy_all(&have_3p);
	//2) 재생성한다.
	create(&dummy); create(&dropped); create(&hand_1p); create(&hand_2p); create(&hand_3p); create(&have_1p); create(&have_2p); create(&have_3p);
	//3) 노드에 데이터를 순차적으로 넣는다.
	fscanf(fp, "dummy %d\n", &cnt);
	for (i = 0; i < cnt; i++)
	{
		fscanf(fp, "%d\n", &buf);
		append_tail(&dummy, buf);
	}
	fscanf(fp, "dropped %d\n", &cnt);
	for (i = 0; i < cnt; i++)
	{
		fscanf(fp, "%d\n", &buf);
		append_tail(&dropped, buf);
	}
	fscanf(fp, "hand_1p %d\n", &cnt);
	for (i = 0; i < cnt; i++)
	{
		fscanf(fp, "%d\n", &buf);
		append_tail(&hand_1p, buf);
	}
	fscanf(fp, "hand_2p %d\n", &cnt);
	for (i = 0; i < cnt; i++)
	{
		fscanf(fp, "%d\n", &buf);
		append_tail(&hand_2p, buf);
	}
	fscanf(fp, "hand_3p %d\n", &cnt);
	for (i = 0; i < cnt; i++)
	{
		fscanf(fp, "%d\n", &buf);
		append_tail(&hand_3p, buf);
	}
	fscanf(fp, "have_1p %d\n", &cnt);
	for (i = 0; i < cnt; i++)
	{
		fscanf(fp, "%d\n", &buf);
		append_tail(&have_1p, buf);
	}
	fscanf(fp, "have_2p %d\n", &cnt);
	for (i = 0; i < cnt; i++)
	{
		fscanf(fp, "%d\n", &buf);
		append_tail(&have_2p, buf);
	}
	fscanf(fp, "have_3p %d\n", &cnt);
	for (i = 0; i < cnt; i++)
	{
		fscanf(fp, "%d\n", &buf);
		append_tail(&have_3p, buf);
	}
	fclose(fp);
	printf("loaded");
	getch();getch();
}

int main(void)
{
	int i = 0;
	create(&dummy); create(&dropped); create(&hand_1p); create(&hand_2p); create(&hand_3p); create(&have_1p); create(&have_2p); create(&have_3p);
	//step 1 : dummy에 카드를 나눠 준 뒤 이것들을 섞는다.
	for (i = 0; i < 48; ++i)
		append_tail(&dummy, properties[i]);
	shuffle_cards(&dummy);
	//step 2 : dropped, hand 123 p 에 나누어 준다.
	for (i = 0; i < 6; ++i)
		move_card_by_index(&dummy, i, &dropped);
	for (i = 0; i < 7; ++i)
		move_card_by_index(&dummy, i, &hand_1p);
	for (i = 0; i < 7; ++i)
		move_card_by_index(&dummy, i, &hand_2p);
	for (i = 0; i < 7; ++i)
		move_card_by_index(&dummy, i, &hand_3p);

	sort_cards(&dropped); sort_cards(&hand_1p); sort_cards(&hand_2p); sort_cards(&hand_3p); //정렬

	//step 3 : 흔들기를 검사한다.
	is_hnndle(&hand_1p) ? (hnndle[0]=1) : (hnndle[0]=0);
	is_hnndle(&hand_2p) ? (hnndle[1]=1) : (hnndle[1]=0);
	is_hnndle(&hand_3p) ? (hnndle[2]=1) : (hnndle[2]=0);
	//step 4 : 그린다.
	system("clear");
	while (1) {
		draw();
	}
	return 0;
}

void create(List* ls)
{
	ls -> head = (Node *)calloc(1, sizeof(Node));
	ls -> tail = (Node *)calloc(1, sizeof(Node));
	ls -> head -> prev = ls -> head;
	ls -> head -> next = ls -> tail;
	ls -> tail -> prev = ls -> head;
	ls -> tail -> next = ls -> tail;
	ls -> curr = NULL;
	ls -> count = 0;
}

Node *insert_before(List *list, Node *node, int data)
{
	//node 전에 넣는다.
	list->curr = (Node *)calloc(1, sizeof(Node));
	list->curr->data = data;
	list->curr->prev = node->prev;
	list->curr->next = node;
	node->prev->next = list->curr;
	node->prev = list->curr;
	list->count += 1;
	list->wol_count[get_wol(list->curr->data)-1]++;
	return list->curr;
	//ㅁ ㅁ ㅁ  (삽입) node ㅁ ㅁ ㅁ
}

Node *insert_after(List *list, Node *node, int data)
{
	//node 후에 넣는다.
	list->curr = (Node *)calloc(1, sizeof(Node));
	list->curr->data = data;
	list->curr->prev = node;
	list->curr->next = node->next;
	node->next->prev = list->curr;
	node->next = list->curr;
	list->count += 1;
	list->wol_count[get_wol(list->curr->data)-1]++;
	return list->curr;
	//ㅁ ㅁ ㅁ node (삽입) ㅁ ㅁ ㅁ
}

Node *append_tail(List *list, int data)
{
	//이 함수의 목적 : tail 앞에다 데이터를 넣는다.
	list->curr = (Node *)calloc(1, sizeof(Node));
	if(list->curr != NULL) {
		list->curr->data = data;
		list->curr->prev = list->tail->prev;
		list->curr->next = list->tail;
		list->tail->prev->next = list->curr;
		list->tail->prev = list->curr;
		list->count += 1;
		list->wol_count[get_wol(list->curr->data)-1]++;
		//printf("zizizizi : %d\n", list->wol_count[get_wol(list->curr->data)-1]);
	}
	return list->curr;
}

Node *append_head(List *list, int data)
{
	//이 함수의 목적 : head 뒤에다 데이터를 넣는다.
	list->curr = (Node *)calloc(1, sizeof(Node));
	if (list->curr != NULL) {
		list->curr->data = data;
		list->curr->prev = list->head;
		list->curr->next = list->head->next;
		list->head->next->prev = list->curr;
		list->head->next = list->curr;
		list->count += 1;
		list->wol_count[get_wol(list->curr->data)-1]++;
	}
	return list->curr;
}
Node *delete_node(List *list, Node *node) {
	//이 함수의 목적 : 해당 주소의 노드를 삭제함
	list->curr = node;
	list->curr->prev->next = list->curr->next;
	list->curr->next->prev = list->curr->prev;
	list->wol_count[get_wol(list->curr->data)-1]--;
	free(list->curr);
	list->curr = NULL;
	list->count -= 1;
	return list->curr;
}

Node *search_node(List *list, int data)
{
	//이 함수의 목적 : 해당 노드를 찾아서 성공하면 node의 주소를 리턴하고 실패하면 NULL을 리턴한다.
	list->curr = list->head->next;
	while (list->curr != list->tail)
	{
		if (list->curr->data == data)
			return list->curr;
		list->curr = list->curr->next;
	}
	return NULL;
}

oy(List *list)
{
	Node *temp;
	list->curr = list->head->next;

	while (list->curr != list->tail)
	{
		temp = list->curr;
		list->wol_count[get_wol(list->curr->data)-1]--;
		list->curr = list->curr->next;
		free(temp);
	}
	free(list->head);
	free(list->tail);
	list->count = 0;
}

void display(List *list)
{
	int i = 0;
	list->curr = list->head->next;
	for (i = 0; i < list->count; i++)
	{
		printf("value : %d\n", list->curr->data);
		list->curr = list->curr->next;
	}
}

void move_card(List *from, int data, List *to)
{
	//from의 node를 delete하고 to의 노드를 생성 
	from->curr = search_node(from, data);
	if(from->curr == NULL) {
		return;
	}
	delete_node(from, from->curr);
	append_tail(to, data);
	//printf("카드를 이동했습니다.\n");
}
Node * move_card_by_index(List *from, int index, List *to)
{
	int data;
	data = search_by_index(from, index)->data;
	from->curr = search_node(from, data);
	if(from->curr == NULL) {
		return NULL; 
	}
	delete_node(from, from->curr);
	append_tail(to, data);
	return to->tail->prev;
}

void swap(int *lval, int *rval)
{
	int tmp;
	tmp = *lval;
	*lval = *rval;
	*rval = tmp;
}

int get_wol(int data)
{
	return data / 100; 
}


int is_yeol(int data)
{
	switch (data % 100)
	{
		case 10:
			return 1;
			break;
		case 11:
			return 2; //고도리용 동물
			break;
		default:
			return 0;
			break;
	}
}

int is_oh(int data)
{
	switch (data % 100)
	{
		case 20:
			return 1;
			break;
		case 21:
			return 2;
			break;
		case 22:
			return 3;
			break;
		case 23:
			return 4;
			break;
		default:
			return 0;
			break;
	}
}

int is_pi(int data)
{
	switch (data % 100)
	{
		case 30://피
			return 1;
			break;
		case 31://쌍피
			return 2;
			break;
		case 32: //국진계열
			return 3;
			break;
		default:
			return 0;
			break;
	}
}

int is_gwang(int data)
{
	switch (data - get_wol(data) * 100)
	{
		case 0:
			return 1; //광
			break;
		case 1:
			return 2; //비광
			break;
		default:
			return 0;
			break;
	}
}

Node * find_by_wol(List *list, int wol)
{
	//검색해서 해당 월이 있는 노드의 데이터를 반환 (ex : 100, 101 모두 1월이다. 사용자가 1을 인자로 넘기면 100을 리턴)
	int i;
	list->curr = list->head->next;
	for (i = 0; i < list->count; i++) { //data들을 모두 양자화(get_wol)하고 양자화 한것끼리 맞으면 원래 값을 리턴해주면 된다.
		if (get_wol(list->curr->data) == wol)
			return list->curr;
		list->curr = list->curr->next;
	}
	return NULL;
}
Node * find_by_wol_refer_to_index(List *list, int wol, int index)
{
	//검색해서 해당 월이 있는 노드의 데이터를 반환 (ex : 100, 101 모두 1월이다. 사용자가 1을 인자로 넘기면 100을 리턴)
	int i, icnt = 0;
	list->curr = list->head->next;
	for (i = 0; i < list->count; i++) { //data들을 모두 양자화(get_wol)하고 양자화 한것끼리 맞으면 원래 값을 리턴해주면 된다.
		if (get_wol(list->curr->data) == wol) {
			if(icnt == index)
				return list->curr;
			else
				icnt++;
		}
		list->curr = list->curr->next;
	}
	return NULL;
}

void sort_cards(List *list)
{
	int i;
	if (list->count <= 1)
		return;
	list->curr = list->head->next;
	for (i = 0; i < list->count; ++i)
	{
		buffer[i] = list->curr->data;
		list->curr = list->curr->next;
	}
	qsort(buffer, list->count, sizeof(int), compare_int);
	list->curr = list->head->next;
	for (i = 0; i < list->count; ++i)
	{
		list->curr->data = buffer[i];
		list->curr = list->curr->next;
	}
}


void shuffle_cards(List *list)
{
	//버퍼에 모든 data를 복사해 넣고 버퍼를 셔플한 뒤, 버퍼 인덱스 순서대로 링크드 리스트에 값을 집어넣는다.
	int i;
	int randValue;
	srand(time(NULL));
	list->curr = list->head->next;
	if (list->count <= 1) {
		printf("섞을 게 없어.");
		return;
	}
	//버퍼에 data복사 (링크드 리스트에 있는 모든 값을 들춰내서 buffer에 저장해야 할 것이다.)

	for (i = 0; i < list->count; ++i) {
		buffer[i] = list->curr->data;
		list->curr = list->curr->next;
	}

	for (i = list->count; i > 1; i--) {
		// 임의 숫자 추출
		randValue = rand() % i;
		// 값 교체
		swap(&buffer[randValue], &buffer[i - 1]);
	}
	list->curr = list->head->next;
	for (i = 0; i < list->count; ++i) {
		list->curr->data = buffer[i];
		list->curr = list->curr->next;
	}
}

int compare_int(const void *a, const void *b)
{
	return *(int *)a - *(int *)b;
}

void draw2()
{
	system("clear");
	printf(" > 고스톱 게임 <\n");
	printf("A %s", hnndle[0]?"\033[0;35m흔듦":"");
	setColor(0);
	printf(")");
	viewList(&hand_1p);
	printf("<A %2d점>", score_ingame[0]);
	sort_print_by_kind(&have_1p);
	printf("\n------------------------------------------------------------------------------\n");
	printf("B %s", hnndle[1]?"\033[0;35m흔듦":"");
	setColor(0);
	printf(")");
	viewList(&hand_2p);
	printf("<B %2d점>", score_ingame[1]);
	sort_print_by_kind(&have_2p);
	printf("\n------------------------------------------------------------------------------\n");
	printf("C %s", hnndle[2]?"\033[0;35m흔듦":"");
	setColor(0);
	printf(")");
	viewList(&hand_3p);
	printf("<C %2d점>", score_ingame[2]);
	sort_print_by_kind(&have_3p);
	printf("\n------------------------------------------------------------------------------\n");
	printf("<바닥>");
	viewList(&dropped);
	/////printf("\n<패산>");
	/////viewList(&dummy);
	printf("\n");	
}

void draw()
{
	char command[30];
	int i, is_command_right = 0; //사용자가 커멘드를 제대로 입력했는지 확인 
	int given_wol, new_dummy_wol; //내가 낸 월, 더미에 있던 월 (11/25저녁에 추가)
	List *current_turn_hand = turn == 'A' ? &hand_1p : turn == 'B' ? &hand_2p : &hand_3p;
	List *current_turn_have = turn == 'A' ? &have_1p : turn == 'B' ? &have_2p : &have_3p;
	new_dummy_wol = get_wol((&dummy)->head->next->data); //
	printf(" > 고스톱 게임 <\n");
	if(turn == 'A') {
		setColor(31);
		printf("***");
		setColor(0);
	}
	printf("A %s", hnndle[0]?"\033[0;35m흔듦":"");
	setColor(0);
	printf(")");
	viewList(&hand_1p);
	//score_ingame[0] = score_count(&have_1p);
	printf("<A %2d점>", score_ingame[0]);
	//viewList(&have_1p);
	sort_print_by_kind(&have_1p);
	printf("\n------------------------------------------------------------------------------\n");
	if(turn == 'B') {
		setColor(31);
		printf("***");
		setColor(0);
	}
	printf("B %s", hnndle[1]?"\033[0;35m흔듦":"");
	setColor(0);
	printf(")");
	viewList(&hand_2p);
	//score_ingame[1] = score_count(&have_2p);
	printf("<B %2d점>", score_ingame[1]);
	//viewList(&have_2p);
	sort_print_by_kind(&have_2p);
	printf("\n------------------------------------------------------------------------------\n");
	if(turn == 'C') {
		setColor(31);
		printf("***");
		setColor(0);
	}
	printf("C %s", hnndle[2]?"\033[0;35m흔듦":"");
	setColor(0);
	printf(")");
	viewList(&hand_3p);
	//score_ingame[2] = score_count(&have_3p);
	printf("<C %2d점>", score_ingame[2]);
	//viewList(&have_3p);
	sort_print_by_kind(&have_3p);
	printf("\n------------------------------------------------------------------------------\n");
	printf("<바닥>");
	viewList(&dropped);
	//	printf("\n------------------------------------------------------------------------------\n");
	/////printf("\n<패산>");
	/////viewList(&dummy);
	//	printf("%2d", (&dummy)->count);
	printf("\n");
	//총통 여부를 따져 본다. 
	if (is_chongtong(&dropped)) {
		score_ingame[turn-65] = 10;
		printf("총통입니다. %c 승리. / 10점\n", turn);
		check_final_score();
		getch();
		system("clear");
		return;
	}
	else if (is_chongtong(&hand_1p)) {
		score_ingame[0] = 10;
		printf("총통입니다. A 승리.\n");//게임 끝
		check_final_score();
		getch();
		system("clear");
		return;
	} else if (is_chongtong(&hand_2p)) {
		score_ingame[1] = 10;
		//nturn = 'B';
		printf("총통입니다. B 승리.\n");//게임 끝
		check_final_score();
		getch();
		system("clear");
		return;
	} else if (is_chongtong(&hand_3p)) {
		score_ingame[2] = 10;
		//nturn = 'C';
		printf("총통입니다. C 승리.\n");//게임 끝
		check_final_score();
		getch();
		system("clear");
		return;
	}
	//게임이 끝났는데 아무도 점수가 안 난 경우.
	if((&hand_1p)->count == 0 && (&hand_2p)->count == 0 && (&hand_3p)->count == 0) {
		printf("나가리입니다.\n");
		is_nagari = 1;
		getch();getch();
		system("clear");
		restart_game();
		return;
	}
	//명령 받기
	printf("\n명령 : ");
	scanf("%30s", command);
	//커맨드 처리
	if(strlen(command) == 1 && command[0] > '0' && command[0] < '8') { //일반모드, 카드 두장 중 선택 모드.
		switch(command[0]-'0')
		{
			case 1: case 2:	case 3: case 4:	case 5:	case 6:	case 7:
				//내가 낸 카드(&dropped->curr 가 될 것이다.) dropped에 옮기고 나서 dropped->curr 그 주소값을 dropped->curr에 놓는다.
				if(command[0]-'0' > current_turn_hand->count) { //노드의 개수보다 입력한 커멘드의 수가 큰 경우에는 함수가 끝남. 
					break; //추가.
				}

				(&dropped)->curr = move_card_by_index(current_turn_hand, command[0]-'0'-1, &dropped); //내가 카드를 딱 냈을 때 (내가 낸 카드가 참조하는 주소 : &dropped->curr)
				//printf("<<<<<<<<%d\n", (&dropped)->curr->data);
				//Node* move_card_by_index(List *from, int index, List *to) index의 노드를 from에서 to로 이동 

				if((&dropped)->curr != NULL) { //사용자가 낸 카드가 dropped->curr에 연결되지 않은 경우
					is_command_right = 1; // 사용자가 제대로 된 커멘드를 입력한 것.
					given_wol = get_wol((&dropped)->curr->data);
					//printf("내가 낸 월 : %d\n", given_wol);
					//	sort_cards(&dropped); //재정렬.
				}
				break;

			default:
				break;
		}
		if(is_command_right == 1)
		{
			switch((&dropped)->wol_count[given_wol-1] - 1) //일단 낸 패를 dropped에 옮기니까, 내가 낸 패의 개수는 빼야 함.
			{
				char wp;
				Node* temp = (&dropped)->curr;
				case 0:
				if(get_wol((&dummy)->head->next->data) == get_wol((&dropped)->curr->data)) //11/27 안정성을 위해 (&dropped)->curr->data)를 temp->next->data로 대체
				{
					printf(">>>>쪽입니다"); getch();getch();
					//(&dropped)->curr = temp;
					move_card(&dropped, (&dropped)->curr->data, current_turn_have);
					move_card(&dummy, (&dummy)->head->next->data, current_turn_have);
					card_steal(turn);
				}
				else if ((&dropped)->wol_count[get_wol((&dummy)->head->next->data) - 1] >= 0 && (&dropped)->wol_count[get_wol((&dummy)->head->next->data) - 1] <= 4){
					//printf(">>>>dropped에 있는 다른 월의 개수가 %d개입니다.\n", (&dropped)->wol_count[get_wol((&dummy)->head->next->data) - 1]);
					switch ((&dropped)->wol_count[get_wol((&dummy)->head->next->data) - 1])
					{
						case 0:
							//dummy to dropped (have로 가는건 없음)
							move_card(&dummy, (&dummy)->head->next->data, &dropped);
							break;
						case 1:
							//dummy to have_np (*2)
							move_card(&dropped, find_by_wol(&dropped, get_wol((&dummy)->head->next->data))->data, current_turn_have);
							move_card(&dummy, (&dummy)->head->next->data, current_turn_have);
							break;
						case 2:
							//dummy to have_np (*3) / select one
							//위 둘중에 하나를 가져가야 하는데 사용자의 input을 받겠다.
							while(1) {
								printf(">> 어떤 패를 가져가시겠습니까? <<\n");
								printf("1) "); show_property(find_by_wol(&dropped, get_wol((&dummy)->head->next->data))->data);
								printf("2) "); show_property(find_by_wol_refer_to_index(&dropped, get_wol((&dummy)->head->next->data), 1)->data);
								printf("\n >> ");
								wp=getch();
								if(wp == '1') {
									move_card(&dropped, find_by_wol(&dropped, get_wol((&dummy)->head->next->data))->data, current_turn_have);
									break;
								}
								else if (wp == '2') {
									move_card(&dropped, find_by_wol_refer_to_index(&dropped, get_wol((&dummy)->head->next->data), 1)->data, current_turn_have);
									break;
								}
								else
								{
									printf("\n");
									draw2(); //123456789
									continue;
								}
							}
							move_card(&dummy, (&dummy)->head->next->data, current_turn_have); //이건 무조건 가져감
							//printf(">>>>>>>>>>case0 중의 case 2\n");
							break;
						case 3:
							move_card(&dropped, find_by_wol(&dropped, get_wol((&dummy)->head->next->data))->data, current_turn_have);
							move_card(&dropped, find_by_wol(&dropped, get_wol((&dummy)->head->next->data))->data, current_turn_have);
							move_card(&dropped, find_by_wol(&dropped, get_wol((&dummy)->head->next->data))->data, current_turn_have);
							move_card(&dummy, (&dummy)->head->next->data, current_turn_have);
							card_steal(turn);
							break;
						default:
							printf("^^^^알 수 없는 에러 (default)\n");
							break;
					}

				}
				else {
					printf("^^^^알 수 없는 에러 (default)\n");
				}
				break;
				case 1:
				//복붙 시작
				if(get_wol((&dummy)->head->next->data) == get_wol((&dropped)->curr->data))
				{
					printf(">>>>설사입니다.\n"); getch();getch();
					move_card(&dummy, (&dummy)->head->next->data, &dropped);
				}
				else if ((&dropped)->wol_count[get_wol((&dummy)->head->next->data) - 1] >= 0 && (&dropped)->wol_count[get_wol((&dummy)->head->next->data) - 1] <= 4) {
					switch ((&dropped)->wol_count[get_wol((&dummy)->head->next->data) - 1])
					{
						case 0:
							//내가 낸 것을 have로 이동, 그리고 dropped의 같은 월짜리 하나가 have로 이동
							//printf("내가 낸 것?? : %d\n", (&dropped)->curr->data);getchar();getchar();
							//printf("%d\n", find_by_wol(&dropped, get_wol((&dropped)->curr->data))->data);
							//display(&dropped); //이걸 하고 난 다음에 segmentation fault가 뜨는 이유는 curr을 건드렸기 때문이다.
							temp = (&dropped)->curr; // **
							move_card(&dropped, find_by_wol(&dropped, get_wol(temp->data))->data, current_turn_have);
							//정확히는 이 문장 수행 직후 다음 move_card에서 segmentation fault가 뜬다. 아마도 curr을 건드려 버렸기 때문에 curr을 재지정해야 하는 것으로 보인다.
							//그런데, 그 전에 작성한 코드는 왜 잘 되었을까? 그냥 우연인가
							//어쨌든, 임시 변수를 도입해야 한다.
							(&dropped)->curr = temp; // **
							//dropped에 같은 월의 카드가 1장 있고, 해당 월의 카드. 더미에서 꺼냈는데 없는 경우
							move_card(&dropped, temp->data, current_turn_have);
							move_card(&dummy, (&dummy)->head->next->data, &dropped);
							break;
						case 1:
							//한장 을 꺼내서 한장 있던 걸 치고(case 1), 더미에서 뒤집었는데 다른 월이 한 장 있던 것과 더미에 있던 것이 맞는 경우. (case 1-1)
							//가져오는 순서 : 내가 낸 것d, 그와 같은 월이었던 것., 더미에서 꺼낸것, 더미에서 꺼낸 것과 같은 월의 카드
							//temp = (&dropped)->curr;
							temp = (&dropped)->curr;
							given_wol = get_wol(temp->data);
							move_card(&dropped, temp->data, current_turn_have); //내가 낸 것을 도로 먹기

							move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have); //바닥에 내가 친 것과 같은 것을 먹기

							given_wol = get_wol((&dummy)->head->next->data);
							move_card(&dummy, (&dummy)->head->next->data, current_turn_have); //더미에서 꺼낸 것 먹기.
							move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have); //더미에서 꺼냈던 월과 같은 월을 먹기

							//case 1 / 1의 코드 완성 : from 11 / 27 오후 7:57
							//temp = (&dropped)->curr;

							//move_card(&dropped, find_by_wol(&dropped, get_wol(temp->data))->data, current_turn_have);
							//그와 같은 월이었던 것.
							/*
							   temp = (&dummy)->head->next;
							   move_card(&dummy, temp->data, current_turn_have);
							   temp = (&dropped)->curr;
							   printf("%d 먹음\n",temp->data);
							   temp = (&dropped)->curr;
							   move_card(&dummy, find_by_wol(&dropped, get_wol(temp->data))->data, current_turn_have);
							   temp = (&dropped)->curr;
							   printf("%d 먹음\n", find_by_wol(&dropped, get_wol(temp->data))->data);
							 */
							break;
						case 2:
							//한장 을 꺼내서 한장 있던 걸 치고(case 1), 더미에서 뒤집었는데 다른 월이 두 장 있던 것과 더미에 있던 것이 맞는	 경우. (case 1-2)
							//가져오는 순서 : 내가 낸 것d, 그와 같은 월이었던 것, 더미에서 꺼낸 것
							//사용자의 입력(선택)을 기다린다. 그리고 나머지 두 장 중 한 장만 가져온다.
							temp = (&dropped)->curr;
							given_wol = get_wol(temp->data);
							move_card(&dropped, temp->data, current_turn_have);

							move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);

							given_wol = get_wol((&dummy)->head->next->data); //더미의 월을 가리키도록 한다.

							move_card(&dummy, (&dummy)->head->next->data, current_turn_have); //필수로 먹어야 함.
							while(1) {
								printf(">> 어떤 패를 가져가시겠습니까? <<\n");
								//printf("1) %d 2) %d\n >> ", show_property(find_by_wol(&dropped, given_wol)->data), show_property(find_by_wol_refer_to_index(&dropped, given_wol, 1)->data));
								printf("1) "); show_property(find_by_wol(&dropped, given_wol)->data);
								printf("2) "); show_property(find_by_wol_refer_to_index(&dropped, given_wol, 1)->data);
								printf("\n >> ");
								wp=getch();
								if(wp == '1') {
									move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);
									break;
								}
								else if (wp == '2') {
									move_card(&dropped, find_by_wol_refer_to_index(&dropped, given_wol, 1)->data, current_turn_have);
									break;
								}
								else
								{
									printf("\n");
									draw2(); //123456789
									continue;
								}
							}
							break;
						case 3:
							//printf(">>>>> 따닥입니다.\n"); getchar(); getchar();
							//한장 을 꺼내서 한장 있던 걸 치고(case 1), 더미에서 뒤집었는데 다른 월이 세 장 있던 것과 더미에 있던 것이 맞는 경우. (case 1-2)
							//가져오는 순서 : 내가 낸 것d, 그와 같은 월이었던 것, 더미에서 꺼낸 것, 더미에서 꺼낸 것과 같은 월의 카드, 더미에서 꺼낸 것과 같은 월의 카드
							//따닥이므로 상대방들로부터 다른 피를 한 장씩 뺏어온다.
							temp = (&dropped)->curr;
							given_wol = get_wol(temp->data);
							move_card(&dropped, temp->data, current_turn_have);

							move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);

							given_wol = get_wol((&dummy)->head->next->data);
							move_card(&dummy, (&dummy)->head->next->data, current_turn_have);
							move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);
							move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);
							move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);
							card_steal(turn);

							break;
						default:
							printf("규정되지 않는 경우가 발생. case : 1\n");
							break;
					}

				}
				else {
					printf("****규정되지 않은 경우가 발생하였습니다.\n");
					getchar();getchar();
				}
				break;
				case 2:
				//내가 낸 카드의 월이 dropped에 2장이 존재했던 경우.
				temp = (&dropped)->curr;
				if(get_wol((&dummy)->head->next->data) == get_wol(temp->data)) //11/27 안정성을 위한 패치 적용
				{
					printf(">>>>따닥입니다\n"); getch();getch();
					given_wol = get_wol(temp->data);
					move_card(&dropped, temp->data, current_turn_have); //11/27 안정성을 위한 패치 //내가 냈던 것을 먹고,

					move_card(&dummy, (&dummy)->head->next->data, current_turn_have); //더미에서 꺼낸 것을 먹고,

					move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have); //아직 확인안함
					move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have); //아직 확인안함
					//dropped 의 같은 월이었던 것 한 장을 먹고
					//dropped 의 같은 월이었던 것 한 장을 마저 먹는다.

					//11/27 오후 2시 36분 까지 작업하였음. (사용자의 선택을 기다려야 함 . 아직 작성 안됨)
					card_steal(turn);
				}
				else if ((&dropped)->wol_count[get_wol((&dummy)->head->next->data) - 1] >= 0 && (&dropped)->wol_count[get_wol((&dummy)->head->next->data) - 1] <= 4){
					//printf(">>>>dropped에 있는 다른 월의 개수가 %d개입니다.\n", (&dropped)->wol_count[get_wol((&dummy)->head->next->data) - 1]);
					switch ((&dropped)->wol_count[get_wol((&dummy)->head->next->data) - 1])
					{
						case 0:
							//printf("******************>>>>>\n"); getchar(); getchar();
							//(내가 꺼낸 것이 dropped에 2장 있는 경우 실행되어야 할 구문이 이 스위치에 걸림)
							temp = (&dropped)->curr;
							given_wol = get_wol(temp->data);
							move_card(&dropped, temp->data, current_turn_have);//내가 낸 카드 먹기
							while(1) {
								printf(">> 어떤 패를 가져가시겠습니까? <<\n");
								//printf("1) %d 2) %d\n >> ", , );
								printf("1) "); show_property(find_by_wol(&dropped, given_wol)->data);
								printf("2) "); show_property(find_by_wol_refer_to_index(&dropped, given_wol, 1)->data);
								printf("\n >> ");
								wp=getch();//scanf("%c", &wp);//getchar();wp=getch();//scanf("%c", &wp);
								if(wp == '1') {
									move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);
									break;
								}
								else if (wp == '2') {
									move_card(&dropped, find_by_wol_refer_to_index(&dropped, given_wol, 1)->data, current_turn_have);
									break;
								}
								else
								{
									printf("\n");
									draw2(); //123456789
									continue;
								}
							}
							move_card(&dummy, (&dummy)->head->next->data, &dropped);
							break;
						case 1:
							temp = (&dropped)->curr;
							given_wol = get_wol(temp->data);
							move_card(&dropped, temp->data, current_turn_have);//1 : 내가 낸 것을 먹는다.
							while(1) {
								printf(">> 어떤 패를 가져가시겠습니까? <<\n");
								//printf("1) %d 2) %d\n >> ", , );
								printf("1) "); show_property(find_by_wol(&dropped, given_wol)->data);
								printf("2) "); show_property(find_by_wol_refer_to_index(&dropped, given_wol, 1)->data);
								printf("\n >> ");
								wp=getch();//scanf("%c", &wp);
								if(wp == '1') {
									move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);
									break;
								}
								else if (wp == '2') {
									move_card(&dropped, find_by_wol_refer_to_index(&dropped, given_wol, 1)->data, current_turn_have);
									break;
								}
								else
								{
									printf("\n");
									draw2(); //123456789
									continue;
								}
							}
							given_wol = get_wol((&dummy)->head->next->data);
							//더미랑, 더미랑 같은거 가져감
							move_card(&dummy, (&dummy)->head->next->data, current_turn_have);
							move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);
							break;
						case 2:
							//dummy to have_np (*3) / select one
							//위 둘중에 하나를 가져가야 하는데 사용자의 input을 받겠다. (총 select : 2번)
							//1234567890
							//이족에서 버그 발견 !!!!
							temp = (&dropped)->curr;
							given_wol = get_wol(temp->data);
							move_card(&dropped, temp->data, current_turn_have);

							while(1) {
								printf(">> p어떤 패를 가져가시겠습니까? <<\n");
								//printf("1) %d 2) %d\n >> ", , );
								printf("1) "); show_property(find_by_wol(&dropped, given_wol)->data);
								printf("2) "); show_property(find_by_wol_refer_to_index(&dropped, given_wol, 1)->data);
								printf("\n >> ");
								wp=getch();//scanf("%c", &wp);
								if(wp == '1') {
									move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);
									break;
								}
								else if (wp == '2') {
									move_card(&dropped, find_by_wol_refer_to_index(&dropped, given_wol, 1)->data, current_turn_have);
									break;
								}
								else
								{
									printf("\n");
									draw2(); //123456789
									continue;
								}
							}


							given_wol = get_wol((&dummy)->head->next->data);
							move_card(&dummy, (&dummy)->head->next->data, current_turn_have);
							while(1) {
								//밑부분 맞음?
								printf(">> p어떤 패를 가져가시겠습니까? <<\n");
								//printf("1) %d 2) %d\n >> ", , );
								printf("1) "); show_property(find_by_wol(&dropped, given_wol)->data);
								printf("2) "); show_property(find_by_wol_refer_to_index(&dropped, given_wol, 1)->data);
								printf("\n >> ");
								wp=getch();//scanf("%c", &wp);
								if(wp == '1') {
									move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);
									break;
								}
								else if (wp == '2') {
									move_card(&dropped, find_by_wol_refer_to_index(&dropped, given_wol, 1)->data, current_turn_have);
									break;
								}
								else
								{
									printf("\n");
									draw2(); //123456789
									continue;
								}
							}
							break;
						case 3:
							move_card(&dropped, find_by_wol(&dropped, get_wol((&dummy)->head->next->data))->data, current_turn_have);
							move_card(&dropped, find_by_wol(&dropped, get_wol((&dummy)->head->next->data))->data, current_turn_have);
							move_card(&dropped, find_by_wol(&dropped, get_wol((&dummy)->head->next->data))->data, current_turn_have);
							move_card(&dummy, (&dummy)->head->next->data, current_turn_have);
							card_steal(turn);
							break;
						default:
							printf("^^^^알 수 없는 에러 (default)\n");
							break;
					}

				}
				else {
					printf("****무슨 경우인가요?\n");
					getchar();
				}	
				break;
				case 3:
				//내가 낸 카드의 월이 dropped에 3장이 존재했던 경우.
				temp = (&dropped)->curr;

				//자동따닥.
				//	printf(">>>>>따닥입니다."); getchar();getchar();
				//내꺼 먹고
				//드롭드 3개 먹고
				given_wol = get_wol(temp->data);
				move_card(&dropped, temp->data, current_turn_have);
				move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);
				move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);
				move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);
				card_steal(turn);

				if ((&dropped)->wol_count[get_wol((&dummy)->head->next->data) - 1] >= 0 && (&dropped)->wol_count[get_wol((&dummy)->head->next->data) - 1] <= 4){
					//printf(">>>>dropped에 있는 다른 월의 개수가 %d개입니다.\n", (&dropped)->wol_count[get_wol((&dummy)->head->next->data) - 1]);
					switch ((&dropped)->wol_count[get_wol((&dummy)->head->next->data) - 1])
					{
						case 0:
							//printf("******************>>>>>\n"); getchar(); getchar();
							//dummy to dropped (1장)
							//printf("case 3 / 0\n");
							move_card(&dummy, (&dummy)->head->next->data, &dropped);
							//11 / 28 00:47 작업
							break;
						case 1:
							//printf("case 3 / 1\n");
							given_wol = get_wol((&dummy)->head->next->data);
							move_card(&dummy, (&dummy)->head->next->data, current_turn_have);
							move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);
							break;
						case 2:
							//printf("case 3 / 2\n");
							given_wol = get_wol((&dummy)->head->next->data);
							while(1) {
								printf(">> 어떤 패를 가져가시겠습니까? <<\n");
								//printf("1) %d 2) %d\n >> ", , );
								printf("1) "); show_property(find_by_wol(&dropped, given_wol)->data);
								printf("2) "); show_property(find_by_wol_refer_to_index(&dropped, given_wol, 1)->data);
								printf("\n >> ");
								getchar();wp=getch();//scanf("%c", &wp);
								if(wp == '1') {
									move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);
									break;
								}
								else if (wp == '2') {
									move_card(&dropped, find_by_wol_refer_to_index(&dropped, given_wol, 1)->data, current_turn_have);
									break;
								}
								else
								{
									printf("\n");
									draw2(); //123456789
									continue;
								}
							}
							break;
						case 3:
							//printf("case 3 / 3\n");
							given_wol = get_wol((&dummy)->head->next->data);
							move_card(&dummy, (&dummy)->head->next->data, current_turn_have);
							move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);
							move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);
							move_card(&dropped, find_by_wol(&dropped, given_wol)->data, current_turn_have);
							card_steal(turn);
							break;
						default:
							printf("^^^^알 수 없는 에러 (default)\n");
							break;
					}

				}
				else {
					printf("****무슨 경우인가요?\n");
					getchar();
				}
				break;
			}
			sort_cards(&dropped); //재정렬.
			//싹쓸이 체크
			if ((&dropped)->count == 0 && (&dummy)->count > 0) //막판 쓸은 보통 인정하지 않는다.
			{
				printf(">>>>>쓸었습니다~"); getch();getch();
				card_steal(turn);
			}
			sort_cards(current_turn_have);
			check_score(); //점수가 났나?
			next_turn();
		}
	}
	else if(strlen(command) == 1)
	{
		switch(command[0])
		{
			case '9' :
				//국진의 토글처리 (쌍피 - > 열끗)
				if(search_node(current_turn_have, 932) != NULL && is_toggled == 0) {
					printf("9월 쌍피를 열끗으로 전환하였습니다."); getch();getch();
					is_toggled = 1;
					check_score(); //점수가 났나?
				}
				else {
					printf("9월 쌍피->9월 열끗 전환을 수행할 수 없습니다.\n%s", is_toggled == 1 ? "이미 토글 명령을 사용했습니다.":"소유자가 아닙니다.");
					getch();getch();
				}
				//11 / 28 오전 10시 2분
				break;
			case 'e' :
				exit(1);
				break;
			case 'h':
				show_help();
				break;
			case 'b':
				show_balance();
				break;
			default :
				break;
		}
	}
	else if (strlen(command) == 4)
	{
		if(strcmp(command, "save") == 0)
		{
			save_data();
		}
		else if(strcmp(command, "load") == 0)
		{
			load_data();
		}
	}
	system("clear");
}

void viewList(List *list)
{
	int i = 0, j;
	int eval;
	List *current_turn = turn == 'A' ? &hand_1p : turn == 'B' ? &hand_2p : &hand_3p;
	// :) 추천 패에 색깔을 주는 것이 좋다.
	for (i = 0; i < list->count; ++i) {
		//printf("listcount : %d\n", list->count);
		eval = search_by_index(list, i)->data;
		if(i%5 == 0) printf("\n"); //가독성을 위해서
		for (j = 1; j <= 12; ++j) {
			if((list == current_turn) && (find_by_wol(list, j) != NULL) && (find_by_wol(&dropped, j) != NULL) && (get_wol(eval) == j)) {
				setColor(35);
				break;
			}
		}
		if((list != &dropped) && (list != &dummy) && (list != &have_1p && list != &have_2p && list != &have_3p))
			printf("%d)", i+1);
		//if (list == current_turn || list == &have_1p || list == &have_2p || list == &have_3p || list == &dropped)
		show_property(eval);
		//else
		//printf("______ ");
		setColor(0);
		
	}
	printf("\n");
}


Node * search_by_index(List *list, int n)
{
	int i;
	list->curr = list->head->next;
	for (i = 0; i < n; ++i)
		list->curr = list->curr->next;
	return list->curr;
}

void setColor(int color)
{
	if(color != 0)
		printf("\033[%d;41m", 1);
	else
		printf("\033[0m");
	//printf("\033[%d;%dm", 1, color);
	//\033[0m <- 복구
}

/*
   색	코드	색	코드
   검정	0;30m	회색	1;30m
   빨강	0;31m	밝은 빨강	1;31m
   초록	0;32m	밝은 초록	1;32m
   갈색	0;33m	노랑	1;33m
   파랑	0;34m	밝은 파랑	1;34m
   보라	0;35m	회색	1;35m
   청록	0;36m	밝은 청록	1;35m
   [출처] [UNIX] 쉘 및 C언어에서 글자색 넣기|작성자 뱅수
 */


void next_turn()
{
	if(turn != 'C') {
		++turn;
	}
	else {
		turn = 'A';
	}	
}

int is_chongtong(List *list) //총통 여부를 판별한다. 0 : 총통 아님, 1 : 총통임
{
	int i;
	static int one_per = 0; //총통은 한 번만 체크해야 한다. 처음에 함수가 4번 호출되었으므로 4이상의 값이 들어오면 이 함수는 0을 리턴하고 무시한다. (11/25 저녁에 추가)
	if(one_per < 4) //
		one_per++;//
	else//
		return 0;//
	for (i = 0; i < 12; ++i)
		if(list->wol_count[i] == 4)
			return 1;
	return 0;
}

int is_chongtong2(List *list) //총통 여부를 다시 판별한다. 0 : 총통 아님, 1 : 총통임
{
	int i;
	for (i = 0; i < 12; ++i)
		if(list->wol_count[i] == 4)
			return 1;
	return 0;
}

int is_hnndle(List * list)
{
	int i;
	for(i=0; i<12; i++)
	{
		if(list->wol_count[i] == 3) // 같은 월을 3장 가지고 있는 경우 흔듦처리 : 이기면 점수 2배
			return 1;
	}

	return 0;
}	//// 이겼을 때 배율 *= 2 처리

void card_steal(char player)
{
	switch (player)
	{
		case 'A':
			(&have_2p)->curr = find_by_pi(&have_2p);
			(&have_3p)->curr = find_by_pi(&have_3p);
			if((&have_2p)->curr != NULL)
			{
				append_tail(&have_1p, (&have_2p)->curr->data);
				delete_node(&have_2p, (&have_2p)->curr);
			}

			if((&have_3p)->curr != NULL)
			{
				append_tail(&have_1p, (&have_3p)->curr->data);
				delete_node(&have_3p, (&have_3p)->curr);
			}
			break;

		case 'B':
			(&have_1p)->curr = find_by_pi(&have_1p);
			(&have_3p)->curr = find_by_pi(&have_3p);
			if((&have_1p)->curr != NULL)
			{
				append_tail(&have_2p, (&have_1p)->curr->data);
				delete_node(&have_1p, (&have_1p)->curr);
			}

			if((&have_3p)->curr != NULL)
			{
				append_tail(&have_2p, (&have_3p)->curr->data);
				delete_node(&have_3p, (&have_3p)->curr);
			}
			break;

		case 'C':
			(&have_1p)->curr = find_by_pi(&have_1p);
			(&have_2p)->curr = find_by_pi(&have_2p);
			if((&have_1p)->curr != NULL)
			{
				append_tail(&have_3p, (&have_1p)->curr->data);
				delete_node(&have_1p, (&have_1p)->curr);
			}

			if((&have_2p)->curr != NULL)
			{
				append_tail(&have_3p, (&have_2p)->curr->data);
				delete_node(&have_2p, (&have_2p)->curr);
			}
			break;

	}
	sort_cards(&have_1p);
	sort_cards(&have_2p);
	sort_cards(&have_3p);
}

Node * find_by_pi(List *list)
{
	//해당 리스트에 피가 있는지를 반환 (일단 피가 있으면 피의 주소를 리턴하고 피가 없고 쌍피만 있으면 쌍피의 주소값을 리턴) 
	//NULL을 반환하는 경우 피나 쌍피가 없음 
	int i;
	list->curr = list->head->next;
	for (i = 0; i < list->count; ++i)
	{
		//find pi (value of 1)
		if (is_pi(list->curr->data) == 1)
			return list->curr;
		list->curr = list->curr->next;
	}
	list->curr = list->head->next;
	for (i = 0; i < list->count; ++i)
	{
		if (is_pi(list->curr->data) == 2)
			return list->curr;
		list->curr = list->curr->next;
	}
	return NULL;
}

void show_property(int eval)
{
	char *josasik;
	//국진은 따로 처리하기로...
	if (eval == 932)
	{
		switch(is_toggled)
		{
			case 0 :
				printf(" 9월 쌍피 ");
				break;
			case 1 :
				printf(" 9월 열끗 ");
				break;
		}
		return;
	}
	if (is_gwang(eval) > 0) {
		printf(" %d월 %s광 ", get_wol(eval), get_wol(eval) == 12 ? "비" : "");
	}
	if (is_yeol(eval) > 0) {
		printf(" %d월 열끗%s ", get_wol(eval), is_yeol(eval) == 1 ? "" : "(새)");
	}
	if (is_oh(eval) > 0) {
		switch(is_oh(eval))
		{
			case 1:
				josasik = "청단";
				break;
			case 2:
				josasik = "홍단";
				break;
			case 3:
				josasik = "초단";
				break;
			default:
				josasik = "단";
				break;
		}
		printf(" %d월 %s ", get_wol(eval), josasik);	
	}
	if (is_pi(eval) > 0) {
		printf(" %d월 %s피 ", get_wol(eval), is_pi(eval) >= 2 ? "쌍" : "");
	}
}

int score_count(List *list)
{ //by 최승욱
	//멍텅구리 처리함? -> 이 함수에서 처리할 내용은 아니고 스톱 시 결정해야 할 내용임. (배율)
	//토글시 점수 처리함? (해결됨)
	int i;
	int tmp;
	int gwang = 0;
	int bi_gwang = 0;
	int dan = 0;
	int pi = 0;
	int yeol = 0;
	int hong = 0;
	int chung = 0;
	int cho = 0;
	int godori = 0;

	int result = 0;

	/* 리스트를 탐색하며 값을 저장한다.*/

	list->curr = list->head->next;
	for(i=0; i < list->count; i++){
		tmp = ((list->curr->data) % 100);

		//열 토글 처리시.
		if(list->curr->data == 932 && is_toggled == 1)
		{
			yeol++;
		}
		switch(tmp/10)
		{
			case 0:
				gwang++;
				if((tmp % 10) > 0)
					bi_gwang = 1;	
				break;

			case 1:
				yeol++;
				if((tmp % 10) > 0)
					godori++;
				break;

			case 2:
				dan++;
				if((tmp % 10) == 0)
					chung++;
				else if((tmp % 10) == 1)
					hong++;
				else if((tmp % 10) == 2)
					cho++;
				break;

			case 3:
				if(list->curr->data == 932 && is_toggled == 1) {
					//nothing
				}
				else {
					pi++;
					if((tmp % 10) > 0)
						pi++;
				}

				break;
		}

		list->curr = list->curr->next;
	}
	// 카드 수로 득점한 경우
	for( ; yeol >= 5; yeol--)
		result++;
	for( ; dan >= 5; dan--)
		result++;
	for( ; pi >= 10; pi--)
		result++;

	// 광 계산
	if(gwang == 3){
		result += 3;
		if(bi_gwang)
			result--;
	}
	else if(gwang == 4)
		result += 4;
	else if(gwang == 5)
		result += 15;

	// 단 계산
	if(chung == 3)
		result += 3;
	if(hong == 3)
		result += 3;
	if(cho == 3)
		result += 3;

	// 고도리 계산
	if(godori == 3)
		result += 5; //고도리 5점임 (수정됨)

	list->curr = list->head->next;//rewind
	return result;
}

void check_score()
{
	//자신의 턴에서만 검사함. 났는지 안 났는지 체크.
	//관련 변수 : go_[N] : 누가 몇 고를 하였는가, score_ingame[N] : 현재 카드 수, 고 횟수만을 따진 점수.
	char buf;
	//printf(">>>>점수 check 합니다. %c\n", turn);
	//getchar();getchar();
	score_ingame[0] = score_count(&have_1p);
	score_ingame[1] = score_count(&have_2p);
	score_ingame[2] = score_count(&have_3p);
	/*
	//고에 따른 점수 보정 작업 
	if(go_[0] == 1 || go_[0] == 2)
	{
	score_ingame[0]+= go_[0];
	}
	if(go_[1] == 1 || go_[1] == 2)
	{
	score_ingame[1]+= go_[1];
	}
	if(go_[2] == 1 || go_[2] == 2)
	{
	score_ingame[2]+= go_[2];
	}
	 */		//최종 계산 때만 고를 적용하도록 수정하였습니다. 12/10(수)

	List *myturn = turn == 'A' ? &hand_1p : turn == 'B' ? &hand_2p : &hand_3p;
	List *yourturn = turn == 'A' ? &hand_2p : turn == 'B' ? &hand_3p : &hand_1p;
	List *your2turn = turn == 'A' ? &hand_3p : turn == 'B' ? &hand_1p : &hand_2p;
	if (score_ingame[turn-65] >= high_score[turn-65])
	{
		//점수가 났을 경우
		draw2(); //사용자가 알기 쉽도록 다시 그려준다.
		high_score[turn-65] = score_ingame[turn-65] + 1; //전에 났던 점수보다 1점이라도 더 따야 하도록 기준 점수를 상향한다.
		//카드가 다 떨어진 경우 자동 스톱
		if(myturn->count < 1) {
			check_final_score();
			stop_game();
			return;
		}
		/*
		   if(myturn->count == 1 && (yourturn->count == 1||your2turn->count == 1)) {
		   check_final_score();
		   stop_game(); //카드가 다 떨어진 경우 자동 스톱
		   return;
		   }*/
		while(1) {
			printf("*** 플레이어 %c가 순 점수 : %d점 났습니다.\n", turn, score_ingame[turn-65]);
			printf(">> 고 하시겠습니까 ? <<\n");
			printf("g) 고  s) 스톱\n");
			printf(" >> ");
			//scanf("%c", &buf);
			buf = getch();
			if(buf=='g') {
				setColor(35);
				prev_go = turn; //이전에 go 한 사람.
				printf("%c가 %d고를 선언하였습니다.", turn, ++go_[turn-65]); // 987654321
				setColor(0);
				getch();
				/*		최종 계산 시 고를 적용하도록 수정. 12/10(수)
						if(go_[turn-65] == 1) {
				//printf("dbg : 1고");
				++score_ingame[turn-65]; //1고 시 1점이 증가한다.
				++high_score[turn-65];
				}
				else if(go_[turn-65] == 2) {
				++score_ingame[turn-65]; //2고 시 1점이 증가한다. (고 점수 : 2)
				++high_score[turn-65];
				}*/
				break;
			}
			else if(buf=='s') {
				check_final_score();				
				stop_game();
				break;
			}
			else {
				printf("\n");
				draw2(); //123456789

				continue;
			}
		}
		//getchar(); //이거 왜 했더라
	}
}



void sort_print_by_kind(List *list)
{
	//점수 계산이 쉽도록 카드의 종류 순서대로 출력해 준다.
	char i = 0, cnt = 0;
	printf("\n");
	// i  ) 광 출력 
	list->curr = list->head->next;
	for (i = 0; i < list->count; ++i)
	{
		if (is_gwang(list->curr->data) > 0) {
			show_property(list->curr->data);
			cnt = 1;
		}
		list->curr = list->curr->next;
	}
	if(cnt) printf("\n");
	cnt = 0;
	// ii ) 동물 출력
	list->curr = list->head->next;
	for (i = 0; i < list->count; ++i)
	{
		if (is_yeol(list->curr->data) > 0 || (list->curr->data == 932 && is_toggled == 1)) {
			show_property(list->curr->data);
			cnt = 1;
		}
		list->curr = list->curr->next;
	}
	if(cnt) printf("\n");
	cnt = 0;
	// iii) 단 출력
	list->curr = list->head->next;
	for (i = 0; i < list->count; ++i)
	{
		if (is_oh(list->curr->data) > 0) {
			show_property(list->curr->data);
			cnt = 1;
		}
		list->curr = list->curr->next;
	}
	if(cnt) printf("\n");
	cnt = 0;
	// iv ) 피 출력
	list->curr = list->head->next;
	for (i = 0; i < list->count; ++i)
	{
		if (is_pi(list->curr->data) > 0 && (list->curr->data != 932 || is_toggled == 0)) {
			show_property(list->curr->data);
			cnt = 1;
		}
		list->curr = list->curr->next;
	}
	list->curr = list->head->next; //rewind
}

void show_help()
{
	system("clear");
	printf(" 人人人人人人人人人人人人人人人人人人人人人人人人人人人人人人人人人人人人人人人人人人\n");
	printf("  >                                     도   움   말                                  \n");
	printf("  > 1 - 7 : 자신이 가지고 있는 카드 내기                                                  \n");
	printf("  > 1 - 2 : 다 가져가 버리면 짝이 맞지 않을 때 먹을 카드 한 장 선택                        \n");
	printf("  > g : 고                                                                          \n");
	printf("  > s : 스톱                                                                         \n");
	printf("  > 9 : 9월 쌍피를 열끗으로 전환. (한 번만 가능)                                           \n");
	printf("  > b : 플레이어들의 잔고 보기                                                           \n");
	printf("  > save : 게임 저장                                                                  \n");
	printf("  > load : 게임 불러오기                                                               \n");
	printf("  > e : 게임 종료                                                                     \n");
	printf(" Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y^Y\n");
	getchar(); getchar();
}

void stop_game()
{
	const int g = go_[turn-65] >= 2 ? 2 : go_[turn-65];
	is_nagari = 0;
	nturn = turn;
	printf("플레이어 %c가 스톱하였습니다! (총 점수 : %d = (%d + %d) * %d)", turn, (score_ingame[turn-65]+g)*score_multi[turn-65], score_ingame[turn-65], g, score_multi[turn-65]);
	getch();getch();
	restart_game();
}

void check_final_score()
{
	//이 함수는 자기 turn에 점수가 났고, 스톱했을 경우 호출된다. / 혹은 나가리가 날 경우 호출된다.
	//printf("<chk final scr>\n ");
	int i;
	int pi = 0, godori = 0, gwang = 0, yeol = 0;
	int eval1 = 0, eval2 = 0;
	List *my_have = turn == 'A' ? &have_1p : turn == 'B' ? &have_2p : &have_3p;
	List *you1 = turn == 'A' ? &have_2p : turn == 'B' ? &have_3p : &have_1p;
	List *you2 = turn == 'A' ? &have_3p : turn == 'B' ? &have_1p : &have_2p; //A B C, B C A, C A B

	int my_hand = turn == 'A' ? 0 : turn == 'B' ? 1 : 2;
	int you1_hand = turn == 'A' ? 1 : turn == 'B' ? 2 : 0;
	int you2_hand = turn == 'A' ? 2 : turn == 'B' ? 0 : 1; //A B C, B C A, C A B

	if(is_chongtong2(&dropped))
	{
		printf(">>총통dropped<<\n");
		if(is_nagari)
		{
			money[my_hand] += 4000;
			money[you1_hand] -= 2000;
			money[you2_hand] -= 2000;
		}
		else
		{
			money[my_hand] += 2000;
			money[you1_hand] -= 1000;
			money[you2_hand] -= 1000;
		}
		nturn = you1_hand + 65;
		restart_game();

		return;
	}
	else if(is_chongtong2(&hand_1p))
	{
		printf(">>총통A<<\n");

		if(is_nagari)
		{
			money[0] += 4000;
			money[1] -= 2000;
			money[2] -= 2000;
		}
		else
		{
			money[0] += 2000;
			money[1] -= 1000;
			money[2] -= 1000;
		}
		nturn = 'B';
		restart_game();

		return;
	}
	else if(is_chongtong2(&hand_2p))
	{
		printf(">>총통B<<\n");
		if(is_nagari)
		{
			money[1] += 4000;
			money[0] -= 2000;
			money[2] -= 2000;
		}
		else
		{
			money[1] += 2000;
			money[0] -= 1000;
			money[2] -= 1000;
		}
		nturn = 'C';
		restart_game();

		return;
	}
	else if(is_chongtong2(&hand_3p))
	{
		printf(">>총통C<<\n");

		if(is_nagari)
		{
			money[2] += 4000;
			money[0] -= 2000;
			money[1] -= 2000;
		}
		else
		{
			money[2] += 2000;
			money[0] -= 1000;
			money[1] -= 1000;
		}
		nturn = 'A';
		restart_game();

		return;
	}
	for (i = 0; i < 3; i++)
	{
		//흔들기에 따라서 점수를 증가시킨다. (추후에 두 번 흔든 경우도 따져서 수정해야됨)
		hnndle[i] ? score_multi[i]*=2,printf("[흔들기(%c)] ", i+65):1==1;
		//나가리에 따라서 점수를 증가시킨다.
		is_nagari ? score_multi[i]*=2,printf("[나가리] "):1==1;
		//1 : 3, 4 , 5 ... 고일 때 점수 배율을 증가한다.
		go_[i] >= 3 ? score_multi[i]*=2,printf("[3고(%c)] ", i+65):1==1;
		go_[i] >= 4 ? score_multi[i]*=2,printf("[4고(%c)] ", i+65):1==1;
		go_[i] >= 5 ? score_multi[i]*=2,printf("[5고(%c)] ", i+65):1==1;
		if(go_[i] >=1 && go_[i] <=2)
			printf("[%d고(%c)] ", go_[i], i+65);
	}

	/*멍텅구리에 따라서 점수를 증가시킨다.
	//멍텅구리 체크 (toggled 여부 고려)
	//멍텅구리-열을 7장 모았을 때 이 용어를 쓰며 자신의 점수를 두배로 만들어 준다. */
	my_have->curr = my_have->head->next; //rewind
	for (i=0;i<my_have->count;i++)
	{
		if(my_have->curr->data == 932)
		{
			if(is_toggled == 0)
				++yeol;
		}
		else
		{
			if(is_yeol(my_have->curr->data) > 0)
				++yeol;
		}
		my_have->curr = my_have->curr->next;
	}
	my_have->curr = my_have->head->next; //rewind
	if(yeol >= 7) {
		printf("[멍텅구리] ");
		score_multi[turn-65]*=2;
	}
	// 1, 2, 3 이상의 고에 대해 점수 증가를 실시한다. 12/10(수)
	if(go_[turn-65] >= 1){
		if(go_[turn-65] >= 2)	// 2고 이상일 경우 2점, 1고일 경우 1점 추가.
			score_ingame[turn-65]++;
		score_ingame[turn-65]++;
	}

	//피박 체크
	/*
	   피박은 피로 점수를 내야만 상대방에게 바가지를 씌울 수 있습니다.
	   피로 1점이라도 점수가 나야 피박을 씌울 수 있으며
	   상대방 피가 5장 이하일 경우 피박입니다. (5장 피박, 6장 피박 아님)
	   상대방 피가 아예 없으면 피박이 아닙니다.
	   ->돈만 2배로 계산한다고 한다.
	 */
	//i) pi가 10 이상인가? (피로 1점 이상이 나는가?)
	my_have->curr = my_have->head->next; //rewind
	for(i=0; i < my_have->count; i++) {
		if(my_have->curr->data == 932)
		{
			if(is_toggled == 0)
			{
				pi+=2;
			}
		}
		else
		{
			pi += is_pi(my_have->curr->data);
		}
		my_have->curr = my_have->curr->next;
	}
	if(pi >= 10)
		eval1 = 1;

	pi = 0; //여기서도 방심하면 안 된다.

	/*	필요없었던 조건인것 같습니다...			12/10(수)
	//ii) 고도리 5점(godori >= 3)에 pi가 10 이상인가?
	my_have->curr = my_have->head->next; //rewind
	for(i=0;i < my_have->count; i++)
	{
	if(is_yeol(my_have->curr->data) == 2)
	++godori;
	my_have->curr = my_have->curr->next;
	}
	if(godori >= 3 && pi >= 10)
	eval2 = 1;
	my_have->curr = my_have->head->next; //rewind
	 */
	//iii) 상대의 피가 1장 이상 5장 이하인가? (나를 제외한 다른 상대를 모두 고려)
	if(eval1)
	{
		//이제부터 피박인 상대를 찾아서 돈을 뺏어오는 배율을 올린다.
		you1->curr = you1->head->next; //rewind
		for(i=0; i < you1->count; i++) {
			if(you1->curr->data == 932)
			{
				if(is_toggled == 0)
				{
					pi+=2;
				}
			}
			else
			{
				pi += is_pi(you1->curr->data);
			}
			you1->curr = you1->curr->next;
		}
		you1->curr = you1->head->next; //rewind
		//		printf("dbg : you1's pi : %d\n", pi);
		if(pi>=1 && pi <= 5)
		{
			//you1은 피박이다.
			if(you1 == &have_1p)
			{
				printf("[A 피박] ");
				money_multi[0]*=2;
			}
			else if(you1 == &have_2p)
			{
				printf("[B 피박] ");
				money_multi[1]*=2;
			}
			else if(you1 == &have_3p)
			{
				printf("[C 피박] ");
				money_multi[2]*=2;
			}
			else
			{
				printf("There is an Error.\n");
				getchar();getchar();
			}
		}

		you2->curr = you2->head->next; //rewind
		pi = 0; //잊을뻔...

		for(i=0; i < you2->count; i++) {
			if(you2->curr->data == 932)
			{
				if(is_toggled == 0)
				{
					pi+=2;
				}
			}
			else
			{
				pi += is_pi(you2->curr->data);
			}
			you2->curr = you2->curr->next;
		}
		you2->curr = you2->head->next; //rewind
		//printf("dbg : you2's pi : %d\n", pi);
		if(pi>=1 && pi <= 5)
		{
			//you2는 피박이다.
			if(you2 == &have_1p)
			{
				printf("[A 피박] ");
				money_multi[0]*=2;
			}
			else if(you2 == &have_2p)
			{
				printf("[B 피박] ");
				money_multi[1]*=2;
			}
			else if(you2 == &have_3p)
			{
				printf("[C 피박] ");
				money_multi[2]*=2;
			}
			else
			{
				printf("There is an Error.\n");
				getchar();getchar();
			}
		}
	}
	//광박 체크
	//내가 광으로 났을 경우에 한한다.
	//광이 0개인 상대의 (뺏기는)돈의 배율을 올린다.

	//나는 광으로 난 것인가?
	eval1 = 0;

	my_have->curr = my_have->head->next; //rewind
	for(i=0;i < my_have->count; i++)
	{
		if(is_gwang(my_have->curr->data) > 0)
			++gwang;
		my_have->curr = my_have->curr->next;
	}
	if(gwang >= 3)
		eval1 = 1;

	gwang = 0; //다시 초기화

	if(eval1 == 1) {
		you1->curr = you1->head->next; //rewind
		for(i=0;i < you1->count; i++)
		{
			if(is_gwang(you1->curr->data) > 0)
				++gwang;
			you1->curr = you1->curr->next;
		}
		//printf("dbg : you1's gwang : %d\n", gwang);
		you1->curr = you1->head->next; //rewind
		if(gwang == 0)
		{
			if(you1 == &have_1p)
			{
				printf("[A 광박] ");
				money_multi[0]*=2;
			}
			else if(you1 == &have_2p)
			{
				printf("[B 광박] ");
				money_multi[1]*=2;
			}
			else if(you1 == &have_3p)
			{
				printf("[C 광박] ");
				money_multi[2]*=2;
			}
			else
			{
				printf("There is an Error.\n");
				getchar();getchar();
			}
		}
		gwang = 0; //아차 ;;
		you2->curr = you2->head->next; //rewind
		for(i=0;i < you2->count; i++)
		{
			if(is_gwang(you2->curr->data) > 0)
				++gwang;
			you2->curr = you2->curr->next;
		}
		//printf("dbg : you2's gwang : %d\n", gwang);
		you2->curr = you2->head->next; //rewind
		if(gwang == 0) {
			if(you2 == &have_1p)
			{
				printf("[A 광박] ");
				money_multi[0]*=2;
			}
			else if(you2 == &have_2p)
			{
				printf("[B 광박] ");
				money_multi[1]*=2;
			}
			else if(you2 == &have_3p)
			{
				printf("[C 광박] ");
				money_multi[2]*=2;
			}
			else
			{
				printf("There is an Error.\n");
				getchar();getchar();
			}
		}
	}

	//이제 돈을 뺏는다.
	//i)고박인가?				12/10(수)
	if(prev_go != turn && prev_go != 'X')
	{
		printf("[%c 고박] ", prev_go);
		if(my_have == &have_1p)
		{
			money[prev_go-65] -= 100 * score_ingame[0] * score_multi[0] * money_multi[1];
			money[prev_go-65] -= 100 * score_ingame[0] * score_multi[0] * money_multi[2];
			if(money[prev_go-65] < 0)
				money[prev_go-65] = 0;
			money[0] += 100 * score_ingame[0] * score_multi[0] * money_multi[1];
			money[0] += 100 * score_ingame[0] * score_multi[0] * money_multi[2];
		}
		else if(my_have == &have_2p)
		{
			money[prev_go-65] -= 100 * score_ingame[1] * score_multi[1] * money_multi[0];
			money[prev_go-65] -= 100 * score_ingame[1] * score_multi[1] * money_multi[2];
			if(money[prev_go-65] < 0)
				money[prev_go-65] = 0;
			money[1] += 100 * score_ingame[1] * score_multi[1] * money_multi[0];
			money[1] += 100 * score_ingame[1] * score_multi[1] * money_multi[2];
		}
		else if(my_have == &have_3p)
		{
			money[prev_go-65] -= 100 * score_ingame[2] * score_multi[2] * money_multi[0];
			money[prev_go-65] -= 100 * score_ingame[2] * score_multi[2] * money_multi[1];
			if(money[prev_go-65] < 0)
				money[prev_go-65] = 0;
			money[2] += 100 * score_ingame[2] * score_multi[2] * money_multi[0];
			money[2] += 100 * score_ingame[2] * score_multi[2] * money_multi[1];
		}
	}
	//ii)아닌 경우 일반적인 처리
	else if(my_have == &have_1p)
	{
		//2p, 3p 껄 뺏기 (점당 100원임)
		money[1] = money[1] - 100 * score_ingame[0] * score_multi[0] * money_multi[1];
		money[2] = money[2] - 100 * score_ingame[0] * score_multi[0] * money_multi[2];
		if(money[1] < 0)
			money[1] = 0;
		if(money[2] < 0)
			money[2] = 0;
		money[0] += 100 * score_ingame[0] * score_multi[0] * money_multi[1];
		money[0] += 100 * score_ingame[0] * score_multi[0] * money_multi[2];
	}
	else if(my_have == &have_2p)
	{
		//1p, 3p 껄 뺏기 (점당 100원임)
		money[0] = money[0] - 100 * score_ingame[1] * score_multi[1] * money_multi[0];
		money[2] = money[2] - 100 * score_ingame[1] * score_multi[1] * money_multi[2];
		if(money[0] < 0)
			money[0] = 0;
		if(money[2] < 0)
			money[2] = 0;
		money[1] += 100 * score_ingame[1] * score_multi[1] * money_multi[0];
		money[1] += 100 * score_ingame[1] * score_multi[1] * money_multi[2];
	}
	else
	{
		//1p, 2p 껄 뺏기 (점당 100원임)
		money[0] = money[0] - 100 * score_ingame[2] * score_multi[2] * money_multi[0];
		money[1] = money[1] - 100 * score_ingame[2] * score_multi[2] * money_multi[1];
		if(money[0] < 0)
			money[0] = 0;
		if(money[1] < 0)
			money[1] = 0;	
		money[2] += 100 * score_ingame[2] * score_multi[2] * money_multi[0];
		money[2] += 100 * score_ingame[2] * score_multi[2] * money_multi[1];
	}


	//score 값 초기화(stop_game() 함수를 위한 절차이다.) 12/10(수)
	if(go_[turn-65] >= 1){
		if(go_[turn-65] >= 2)	// 2고 이상일 경우 2점, 1고일 경우 1점 감소.
			score_ingame[turn-65]--;
		score_ingame[turn-65]--;
	}
}


void restart_game()
{
	int i = 0;
	if(!is_nagari)
	{
		if(nturn != 'A') {
			turn = nturn - 1; //이긴 사람이 선을 하도록 한다.
		}
		else {
			turn = 'C';
		}
	}
	destroy_all(&dummy); destroy_all(&dropped); destroy_all(&hand_1p); destroy_all(&hand_2p); destroy_all(&hand_3p); destroy_all(&have_1p); destroy_all(&have_2p); destroy_all(&have_3p);
	create(&dummy); create(&dropped); create(&hand_1p); create(&hand_2p); create(&hand_3p); create(&have_1p); create(&have_2p); create(&have_3p);
	//step 1 : dummy에 카드를 나눠 준 뒤 이것들을 섞는다.
	for (i = 0; i < 48; ++i)
		append_tail(&dummy, properties[i]);
	shuffle_cards(&dummy);
	//step 2 : dropped, hand 123 p 에 나누어 준다.
	for (i = 0; i < 6; ++i)
		move_card_by_index(&dummy, i, &dropped);
	for (i = 0; i < 7; ++i)
		move_card_by_index(&dummy, i, &hand_1p);
	for (i = 0; i < 7; ++i)
		move_card_by_index(&dummy, i, &hand_2p);
	for (i = 0; i < 7; ++i)
		move_card_by_index(&dummy, i, &hand_3p);

	sort_cards(&dropped); sort_cards(&hand_1p); sort_cards(&hand_2p); sort_cards(&hand_3p); //정렬

	//step 3 : 흔들기를 검사한다.
	is_hnndle(&hand_1p) ? (hnndle[0]=1) : (hnndle[0]=0);
	is_hnndle(&hand_2p) ? (hnndle[1]=1) : (hnndle[1]=0);
	is_hnndle(&hand_3p) ? (hnndle[2]=1) : (hnndle[2]=0);

	score_ingame[0] = 0;
	score_ingame[1] = 0;
	score_ingame[2] = 0;

	high_score[0] = 3;
	high_score[1] = 3;
	high_score[2] = 3;

	score_multi[0] = 1;
	score_multi[1] = 1;
	score_multi[2] = 1;

	go_[0] = 0;
	go_[1] = 0;
	go_[2] = 0;

	money_multi[0] = 1;
	money_multi[1] = 1;
	money_multi[2] = 1;
	prev_go = 'X';
}

void show_balance()
{
	system("clear");
	printf("플레이어들의 잔고\n");
	printf("A : %7d원\n", money[0]);
	printf("B : %7d원\n", money[1]);
	printf("C : %7d원\n", money[2]);
	getch();getch();
}

void destroy_all(List *list)
{
	Node *temp;
	list->curr = list->head->next;

	while (list->curr != list->tail)
	{
		temp = list->curr;
		list->wol_count[get_wol(list->curr->data)-1]--;
		list->curr = list->curr->next;
		free(temp);
	}
	free(list->head);
	free(list->tail);
	list->count = 0;
}
