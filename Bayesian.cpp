#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>

#include "prototype.h"

using namespace std;

// 番兵ノード
//const Node *FAKE = new Node();

// 入力値
static input in[TEST_POINT];

// ランダム関数
double seed;

int intrnd ()
{
	double const a    = 16807;
	double const m    = 2147483647;
    	double temp = (double) (seed * a);

	seed = (int) (temp - m * floor( temp / m ));

	return seed;
}
float rand_0to1()
{
	return ( (float)(intrnd()) / 2147483647.0 );
}

float random_number()
{
	float r;
	for (; (r = rand_0to1() ) >= 1.0; ){
		//cout << "random number >= " << r << endl;
	}
	return ( r );

}


/*
float random_number()
{
	ifstream ifs("random_number.txt");
	string buff;
	double r;
	if(getline(ifs, buff) and ifs) {

		stringstream ss(buff);
		ss >> r;
		cout << r << endl;
	}
/*
	static ifstream ifs;
	if(!ifs){
		ifs.open("random_number.txt");
		if( ifs.fail() ){
			cout << "Don't Open Random File\n";
			exit (0);
		}
	}
	double r = 0;
	string buff;
	getline(ifs, buff);
	stringstream ss(buff);
	ss >> r;

	return r;
}*/

class Random_number
{
private:
	ifstream ifs;
	string buff;
	double r;

	Random_number()
	{
		ifs.open("random_number.txt");
	}
	Random_number(const Random_number& rhs);
	Random_number& operator=(const Random_number& rhs);

public:
	static Random_number* getInstance()
	{
		static Random_number r;
		return &r;
	}

	double get(){
		buff.erase();
		if( getline(ifs, buff) && ifs ){
			stringstream ss( buff );
			ss >> r;
			return r;
		}
		else{
			cout << "random_number file is EOF\n";
			exit(1);
		}
	}

};

Random_number* run = Random_number::getInstance();

// 構文木から配列に変換(後置記法)
void syntax_array(Node *p, string *str)
{
	*str = *str + p->string;
	if(p->left != NULL){
		syntax_array(p->left, str);
	}
	if(p->right != NULL){
		syntax_array(p->right, str);
	}
}

// 後置記法表示
void show_tree(Node *p)
{
	if(p != NULL){
		string str;
		syntax_array(p, &str);
		cout << str << endl;
	}
}

// 構文木情報
void _tree_info(Node *p)
{
	if(p == NULL){
		return ;
	}

	printf("0x%-7x\t", p);
	printf("0x%-7x\t", p->parent);
	printf("%4d\t", p->no);
	printf("%4d\t", p->depth);
	printf("%4s\t", p->string.c_str());
	printf("0x%-7x\t", p->left);
	printf("0x%-7x\n", p->right);

	if(p->left != NULL){
		_tree_info(p->left);
	}
	if(p->right != NULL){
		_tree_info(p->right);
	}
}

void tree_info(Node *p)
{
	printf("%-8s\t", "adress");
	printf("%-8s\t", "parent");
	printf("%4s\t", "No");
	printf("%4s\t", "depth");
	printf("%4s\t", "str");
	printf("%-8s\t", "left");
	printf("%-8s\n", "right");
	_tree_info(p);
}

// ノード情報
void node_info(Node *p)
{
	printf("%-8s\t", "adress");
	printf("%-8s\t", "parent");
	printf("%4s\t", "No");
	printf("%4s\t", "depth");
	printf("%4s\t", "str");
	printf("%-8s\t", "left");
	printf("%-8s\n", "right");
	printf("0x%-7x\t", p);
	printf("0x%-7x\t", p->parent);
	printf("%4d\t", p->no);
	printf("%4d\t", p->depth);
	printf("%4s\t", p->string.c_str());
	printf("0x%-7x\t", p->left);
	printf("0x%-7x\n", p->right);
}

// ファイル行数カウント
int get_line_num(FILE *fp)
{
	int line = 0;
	char buf[1024];
	while(NULL != fgets(buf, 1024, fp)){
		line++;
	}
	fseek(fp, 0, SEEK_SET);
	return line;
}

// 比較関数
int df_comp(const void* c1, const void* c2)
{
	const df_word* a = (const df_word*)c1;
	const df_word* b = (const df_word*)c2;

	int tmp1 = a->wordid;
	int tmp2 = b->wordid;

	return tmp1 - tmp2;
}

int tf_comp(const void* c1, const void* c2)
{
	const tf_word* a = (const tf_word*)c1;
	const tf_word* b = (const tf_word*)c2;

	int tmp1 = a->wordid;
	int tmp2 = b->wordid;

	return tmp1 - tmp2;
}

int f_comp( const void *c1, const void *c2 )
{
  const best_f* a = (const best_f*)c1;
  const best_f* b = (const best_f*)c2;

  int tmp1 = a->f;
  int tmp2 = b->f;

  return tmp1 - tmp2;
}

//df_info配列ソート
bool Df_infoWord(const df_word& rLeft, const df_word& rRight)
{
	return rLeft.wordid < rRight.wordid;
}

VS split(string str, string delim)
{
  VS items;
  size_t dlm_idx;
  if(str.npos == (dlm_idx = str.find_first_of(delim))) {
    items.push_back(str.substr(0, dlm_idx));
  }
  while(str.npos != (dlm_idx = str.find_first_of(delim))) {
    if(str.npos == str.find_first_not_of(delim)) {
      break;
    }
    items.push_back(str.substr(0, dlm_idx));
    dlm_idx++;
    str = str.erase(0, dlm_idx);
    if(str.npos == str.find_first_of(delim) && "" != str) {
      items.push_back(str);
      break;
    }
  }
  return items;
}

// stringからintへ変換
int stoi(string s)
{
	int i;
	stringstream ss(s.data());
	ss >> i;
	return i;
}

// stringからdoubleへ変換
double stod(string s)
{
	double d;
	istringstream iss;
	iss.str(s);
	iss >> d;
	return d;
}

// 番号の再設定
long int _tree_number_set(Node *p, long int i)
{
	p->no = i++;
	if(p->left != NULL){
		i = _tree_number_set(p->left, i);
	}
	if(p->right != NULL){
		i = _tree_number_set(p->right, i);
	}
	return i;
}

void tree_number_set(Node *p)
{
	long int i = 0;
	p->no = i++;
	if(p->left != NULL){
		i = _tree_number_set(p->left, i);
	}
	if(p->right != NULL){
		i = _tree_number_set(p->right, i);
	}
}

// 木のコピー
Node *tree_copy(Node *p)
{
	if(p == NULL){
		return NULL;
	}
	else{
		//char *str = new char[LINE_WIDE];
		//memset(str, 0, sizeof(str));
		string str;
		syntax_array(p, &str);
		Node *q = make_tree(str);
		//delete [] str;
		return q;
	}
}

// 構文木の消去
Node *free_tree(Node *p)
{
	if(p != NULL){
		if(p->left != NULL){
			free_tree(p->left);
		}
		if(p->right != NULL){
			free_tree(p->right);
		}
		delete p;
	}
	return NULL;
}

// 構文木の長さの探査
int tree_length(Node *p)
{
	if(p == NULL){
		return 0;
	}
	else{
		//char *str = new char[LINE_WIDE];
		//memset(str, 0, sizeof(str));
		string str;
		syntax_array(p, &str);
		//int l = strlen(str);
		//delete [] str;
		int l = str.length();
		return l;
	}
}

// 交叉点のポインタの走査
Node *_key_serch(Node *p, const int x, int *count)
{
	Node *q = NULL;

	if(x == *count){
		return p;
	}

	*count += 1;
	if(p->left != NULL){
		q = _key_serch(p->left, x, count);
	}
	if((p->right != NULL) && (q == NULL)){
		q = _key_serch(p->right, x, count);
	}

	return q;
}

Node *key_serch(Node *p, const int x)
{
	Node *q = NULL;
	int count = 1;

	if(x == 0){
		return p;
	}
	else if(x >= tree_length(p)){
		return NULL;
	}

	if(p->left != NULL){
		q = _key_serch(p->left, x, &count);
	}
	if((p->right != NULL) && (q == NULL)){
		q = _key_serch(p->right, x, &count);
	}

	return q;
}

//構文木の構造チェック  エラー = 1
int tree_error_check(Node *p)
{
	int flg = 0;
//	cout << p->string << endl;

	if(p == NULL){
		return -1;
	}
	else if( (p->string == "s") || (p->string == "c") || (p->string == "l") || (p->string == "e") ){
		if((p->right != NULL) || (p->left == NULL)){
			return 1;
		}
	}
	else if( (p->string == "+") || (p->string == "*") || (p->string == "-") || (p->string == "/") ){
		if((p->right == NULL) || (p->left == NULL)){
			return 1;
		}
	}

	if(p->left != NULL){
		flg = tree_error_check(p->left);
	}
	if(p->right != NULL){
		flg = tree_error_check(p->right);
	}
	return flg;
}

//子ノード同士の交換
void node_swap1(Node *p1, Node *p2)
{
	Node *tmp = NULL;
	Node *head1 = NULL, *head2 = NULL;
	int pflg1 = 0, pflg2 = 0;

	//p1の親ノードの左右確認
	head1 = p1->parent;
	if(head1->left == p1){
		pflg1 = 1;
	}
	//p2の親ノードの左右確認
	head2 = p2->parent;
	if(head2->left == p2){
		pflg2 = 1;
	}

	if(pflg1 == 0){
		tmp = head1->right;
		if(pflg2 == 0){
			head1->right = head2->right;
			head2->right = tmp;
			head2->right->parent = head2;
		}
		else{
			head1->right = head2->left;
			head2->left  = tmp;
			head2->left->parent = head2;
		}
		head1->right->parent = head1;
	}
	else{
		tmp = head1->left;
		if(pflg2 == 0){
			head1->left = head2->right;
			head2->right = tmp;
			head2->right->parent = head2;
		}
		else{
			head1->left = head2->left;
			head2->left = tmp;
			head2->left->parent = head2;
		}
		head1->left->parent = head1;
	}
}

//ルートノードと子ノードの交換
void node_swap2(Node **p1, Node *p2)
{
	Node *tmp, *head = p2->parent;
	int flg = 0;
;
	if(head->left == p2){
		flg = 1;
	}

	tmp = *p1;
	*p1 = p2;
	if(flg == 0){
		head->right = tmp;
		head->right->parent = head;
	}
	else{
		head->left = tmp;
		head->left->parent = head;
	}
}

//ルートノード同士の交換
void node_swap3(Node **p1, Node **p2)
{
	Node *tmp;
	tmp = *p1;
	*p1 = *p2;
	*p2 = tmp;
}

// 演算子の選択
int random_function()
{
	static const int function[8] = { '+', '-', '*', '/', 's', 'c', 'l', 'e' };
	//return ( function[(int) (random_number() * N_FUNCTION)] );
	return ( function[(int) (run->get() * N_FUNCTION)]);
}
int random_terminal()
{
	//static const int terminal[3] = { 'T', 'D', 'N' };
	static const int terminal = 'x';
	return ( terminal );
	//return ( terminal[(int) (random_number() * N_TERMINAL)] );
	//return ( terminal[(int) (run->get() * N_TERMINAL)] );
}

// 木の生成
long int rnd_tree(string buff, long int size)
{
	long int dangling_limbs = 1, i;

	for(i = 0;( (dangling_limbs > 0) && (i < size) );i++){
		//if(random_number() > (double)(dangling_limbs*dangling_limbs+1)/(double)(size-i)){
		if(run->get() > (double)(dangling_limbs*dangling_limbs+1)/(double)(size-i)){
			buff[i] = random_function();
			if((buff[i] == '+') || (buff[i] == '-') || (buff[i] == '*') || (buff[i] == '/')){
				dangling_limbs++;
			}
		}
		else{
			buff[i] = random_terminal();
			dangling_limbs--;
		}
	}
	buff[i] = '\0';

	if(dangling_limbs != 0){
		cout << "Error\n\n";
		exit(1);
	}

	return i;
}

// 数値返却
double mach_string(string str, double x)
{
	if(str == "x"){
		return x;
	}
	else{
		return (double)atof(str.c_str());
	}
}

// 木の計算
double prog_value(Node *p, double x)
{
	double l = 0.0, r = 0.0;

	if(p->left != NULL){
		l = prog_value(p->left, x);
	}
	if(p->right != NULL){
		r = prog_value(p->right, x);
	}
	if((p->left == NULL) && (p->right == NULL)){
		return mach_string(p->string, x);
	}

	if(p->string == "+"){
		return (l + r);
	}
	else if(p->string == "-"){
		return (l - r);
	}
	else if(p->string == "*"){
		return (l * r);
	}
	else if(p->string == "/"){
		if(r == 0.0){
			return 0.0;
		}
		else{
			return (l / r);
		}
	}
	else if(p->string == "s"){
		return (sin(l));
	}
	else if(p->string == "c"){
		return (cos(l));
	}
	else if(p->string == "e"){
		return (exp(l));
	}
	else if(p->string == "l"){
		if(l == 0){
			return 0.0;
		}
		else{
			return (log(l));
		}
	}
	else{
		return -1.0;
	}
}

double test_fitness(Node *p, double x)
{
	double calc = prog_value(p, x);

	if(__isnan(calc) || __isinf(calc)){
		return 0.0;
	}
	else{
		return calc;
	}
}

// 木の深さ探査
void _depth_serch(Node *p, int d, int *m)
{
	if(p->left != NULL){
		_depth_serch(p->left, ++d, m);
	}
	if(p->right != NULL){
		_depth_serch(p->right, ++d, m);
	}

	if((p->left == NULL) && (p->right ==NULL)){
		if(d > *m){
			*m = d;
		}
	}
}

int depth_serch(Node *p)
{
	int d = 0;
	int m = 0;
	_depth_serch(p, d, &m);
	return m-1;
}

// 配列から構文木の生成
int _make_tree(Node **root, Node *parent, string str, size_t num, int depth)
{
	Node *p;

	//cout << "num = " << num << "\t" << str[num] << endl;
	if((p = new Node()) == NULL){
		cout << "Malloc Error\n";
		exit(-1);
	}
	else if(str.length() <= num){
		return 0;
	}

	p->parent = parent;
	p->no = (int)num;
	p->depth = depth++;
	p->string = str[num];
	p->left = NULL;
	p->right = NULL;

	if(*root == NULL){
		*root = p;
	}
	else if(parent->left == NULL){
		parent->left = p;
	}
	else if( (parent->string != "s") || (parent->string != "c") || (parent->string != "l") || (parent->string != "e") ){
		parent->right = p;
	}
	//node_info(p);

	switch(str[num]){
		case 's':
		case 'c':
		case 'e':
		case 'l':
			//while((str[num] != 'T') && (str[num] != 'D') && (str[num] != 'F')){
			while( str[num] != 'x' ){
				parent = p;
				num++;
				if((str[num] == '+') || (str[num] == '-') || (str[num] == '*') || (str[num] == '/')){
					num = _make_tree(root, p, str, num, depth);
				}

				//cout << "num = " << num << "\t" << str[num] << endl;
				if((p = new Node()) == NULL){
					cout << "Malloc Error\n";
					exit(-1);
				}
				else if(str.length() <= num){
					return 0;
				}
				p->parent = parent;
				p->no = num;
				p->depth = depth++;
				p->string = str[num];
				p->left = NULL;
				p->right = NULL;
				if(parent->left == NULL){
					parent->left = p;
				}
				//node_info(p);
			}
			break;
		case '+':
		case '-':
		case '*':
		case '/':
			num = _make_tree(root, p, str, num+1, depth);
			if(parent == NULL){
				num = _make_tree(root, p, str, num+1, depth);
			}
			else{
				num = _make_tree(root, p, str, num+1, depth);
				if( (p->string == "s") || (p->string == "c") || (p->string == "l") || (p->string == "e") ){
					parent->right = p;
				}
			}
			break;
	}
	return num;
}

// 構文木の構築
Node *make_tree(string str)
{
	Node *root = NULL;

	//cout << "make_tree\n" << str << endl;
	if(str.length() > 1){
		_make_tree(&root, NULL, str, 0, 0);
	}
	else{
		Node *p;
		if( (p = new Node()) == NULL ){
			cout << "Malloc Error\n";
			exit(-1);
		}
		p->parent = NULL;
		p->no = 0;
		p->depth = 0;
		p->string = str[0];
		p->left = NULL;
		p->right = NULL;
		root = p;
	}

	return root;
}

// 構文木をランダムに生成
int _make_tree(Node **root, Node *parent, int num, int d)
{
	string buff;
	//if( random_number() > (d / DEPTH) ){
	if( run->get() > (d / DEPTH) ){
		buff += random_function();
	}
	else{
		buff += random_terminal();
	}
	//cout << buff << endl;

	Node *p;
	if( (p = new Node()) == NULL){
		cout << "Malloc Error\n";
	}

	p->parent = parent;
	p->no	   = num;
	p->depth  = d++;
	p->string = buff;
	p->left   = NULL;
	p->right  = NULL;

	if(*root == NULL){
		*root = p;
	}
	else if(parent->left == NULL){
		parent->left = p;
	}
	else if( (parent->string != "s") || (parent->string != "c") || (parent->string != "l") || (parent->string != "e") ){
		parent->right = p;
	}
	//node_info(p);

	if( (buff == "s") || (buff == "c") || (buff == "e") || (buff == "l") ){
		//while( (buff != "T") && (buff != "D") ){
		while( buff != "x" ){
			parent = p;
			num++;
			buff.erase();

			//if( random_number() > (d / DEPTH) ){
			if( run->get() > (d / DEPTH) ){
				buff += random_function();
			}
			else{
				buff += random_terminal();
			}
			if( (buff == "+") || (buff == "-") || (buff == "*") || (buff == "/") ){
				num = _make_tree(root, p, num, d);
			}

			if((p = new Node()) == NULL){
				cout << "Malloc Error\n";
				exit(-1);
			}
			p->parent = parent;
			p->no = num;
			p->depth = d++;
			p->string = buff;
			p->left = NULL;
			p->right = NULL;
			if(parent->left == NULL){
				parent->left = p;
			}
			//node_info(p);
		}
	}
	else if( (buff == "+") || (buff == "-") || (buff == "*") || (buff == "/") ){
		num = _make_tree(root, p, num+1, d);
		if(parent == NULL){
			num = _make_tree(root, p, num+1, d);
		}
		else{
			num = _make_tree(root, p, num+1, d);
			if( (p->string == "s") || (p->string == "c") || (p->string == "l") || (p->string == "e") ){
				parent->right = p;
			}
		}
	}

	return 0;
}

Node *make_tree()
{
	Node *root = NULL;
	_make_tree(&root, NULL, 0, 0);
	//cout << endl;
	//tree_info(root);
	return root;
}

// 完全２分木の作成
void grow(Node *p)
{
	if(p->depth == DEPTH){
			return ;
		}

		Node *q;
		if( (q = new Node()) == NULL ){
			cout << "Malloc Error\n";
			exit(-1);
		}
		q->parent = p;
		q->depth  = p->depth + 1;
		q->no	   = -1;
		q->string = "F";

		if( (p->string == "s") || (p->string == "c") || (p->string == "l") || (p->string == "e") ){
			if(p->right == NULL){
				q->left   = NULL;
				q->right  = NULL;
				p->right = q;
				grow(q);
			}
		}
		else if( (p->string == "F") || (p->string == "L")  ){
			grow(q);
			if(p->left == NULL){
				p->left = q;
				grow(p);
			}
			else if(p->right == NULL){
				p->right = q;
			}
		}
		else if( (p->left == NULL) && (p->right == NULL) ){
			q->parent = p->parent;
			q->depth  = p->depth;
			q->string = "F";
			q->left   = p;
			q->right  = NULL;
			if( p->parent->left == p){
				p->parent->left = q;
			}
			else{
				p->parent->right = q;
			}

			p->parent = q;
			p->depth++;
			grow(p);
			if( q->right == NULL){
				grow(q);
			}
		}
}

void grow_tree(Node *p)
{
	vector<Node *>q;
	terminal_depth(p, q);
	vector<Node *>::iterator it;
	for(it = q.begin();it != q.end();it++){
		grow(*it);
	}
}

// 最大深度未満のターミナルの探査
void terminal_depth(Node *p, vector<Node *>&q)
{
	if((p->left == NULL) && (p->right == NULL)){
		if(p->depth < DEPTH){
			q.push_back(p);
		}
	}
	else if( ((p->left != NULL) && (p->right == NULL)) && (p->depth < DEPTH) ){
		q.push_back(p);
	}
	if(p->left != NULL){
		terminal_depth(p->left, q);
	}
	if(p->right != NULL){
		terminal_depth(p->right, q);
	}
}


// 確率分布の作成
Proba **probability_distribution(Node *n[])
{
	int length = tree_length(n[0]);
	string s[ELITE_POP];
	Proba **p = new Proba*[length + 1];

	for(int i = 0;i < ELITE_POP;i++){
		string str;
		syntax_array(n[i], &str);
		s[i] = str;
	}


	for(int i = 0;i < length;i++){
		map<char, int> date;

		// 各演算子のカウント
		for(int j = 0;j < ELITE_POP;j++){
			if( (date.find(s[j][i]) == date.end())  ){
				date[ s[j][i] ] = 1;
			}
			else{
				date[ s[j][i] ]++;
			}
		}

		// 各演算子の確率計算
		Pair *pa = new Pair[date.size()];
		map<char, int>::iterator it = date.begin();

		int k = 0;
		pa[k].string  = (*it).first;
		pa[k].percent = (double)(*it).second / ELITE_POP;
		//cout << endl << pa[k].string << " : " << pa[k].percent << endl;
		++it;
		++k;

		while( it != date.end() ){
			pa[k].string  = (*it).first;
			pa[k].percent = pa[k-1].percent + ( (double)(*it).second / ELITE_POP );
			//cout << pa[k].string << " : " << pa[k].percent << endl;
			++it;
			++k;
		}

		// Probaの実体の確保
		p[i] = new Proba();
		p[i]->pair = pa;
		p[i]->length = (int)date.size();
	}

	p[length] = new Proba();
	p[length]->pair = NULL;
	p[length]->length = -1;

	return p;
}

// 確率分布から次世代木の生成
Node **distribution_newtrees(Proba **p)
{
	Node **trees = new Node*[POP_SIZE];
	int a = 0;
	while(a < POP_SIZE){
		string str;
		for(int i = 0;p[i]->length != -1;i++){
			//double r = random_number();
			double r = run->get();
			int j;
			for(j = 0;j < p[i]->length;j++){
				if(r < p[i]->pair[j].percent){
					break;
				}
			}
			str += p[i]->pair[j].string;
		}
		trees[a] = make_tree(str);
		//show_tree(trees[a]);
		a++;
	}

	string temp;
	for(int i = 0;i < POP_SIZE;i++){
		string s;
		syntax_array(trees[i], &s);
		if(s.find('F', 0) == string::npos){
			temp = s;
			//cout << endl << temp << endl;
			break;
		}
	}

	if( temp.empty() ){
		const static string temple[3] = {"e*ssTD", "*-lTeD", "*lT/DD"};
		//temp = temple[(int)(random_number() * 3.0)];
		temp = temple[(int)(run->get() * 3.0)];
	}

	for(int i = 0;i < POP_SIZE;i++){
		string s;
		syntax_array(trees[i], &s);
		if(s.find('F', 0) != string::npos){
			trees[i] = free_tree(trees[i]);
			trees[i] = make_tree(temp);
		}
	}

	return trees;
}

// 最小の数を探す
int min(int a, int b, int c)
{
	return a > b ? (b > c ? c : b) : (a > c ? c : a);
}

// レーベンシュタイン距離を求める
int levenshtein_distance(string str1, string str2)
{
	int lenstr1 = str1.size() + 1;
	int lenstr2 = str2.size() + 1;
	int d[lenstr1][lenstr2];
	int i1 = 0, i2 = 0, cost = 0;

	for (;i1 < lenstr1; i1++) d[i1][0] = i1;
	for (;i2 < lenstr2; i2++) d[0][i2] = i2;

	for (i1 = 1; i1 < lenstr1; i1++) {
		for (i2 = 1; i2 < lenstr2; i2++) {
			cost = str1[i1 - 1] == str2[i2 - 1] ? 0 : 1;
			d[i1][i2] = min(d[i1 - 1][i2] + 1, d[i1][i2 - 1] + 1, d[i1 - 1][i2 - 1] + cost);
		}
	}

	return d[lenstr1 - 1][lenstr2 - 1];
}

// 標準偏差
double standard_deviation(const int dis[])
{
	int x = 0;
	double ave = 0;

	for(int i = 0;i < POP_SIZE;i++){
		ave += dis[i];
	}
	ave /= POP_SIZE;

	for(int i = 0;i < POP_SIZE;i++){
		x += pow( (dis[i] - ave), 2.0 );
	}
	x /= POP_SIZE;

	return ( sqrt(x) );
}

// 分散半径
int deviation_radius(const int dis[])
{
	int min = EDIT_DISTANCE, max = 0;
	for(int i = 0;i < POP_SIZE;i++){
		if(min > dis[i]){
			min = dis[i];
		}
		if(max < dis[i]){
			max = dis[i];
		}
	}
	return (max - min);
}

// 関数同定評価
void function_identification(double fitness[], Node **pop)
{
	for(int i = 0;i < POP_SIZE;i++){
		double total_error = 0.0;
		for(int j = 0;j < TEST_POINT;j++){
			total_error += fabs( in[j].y - test_fitness(pop[i], in[j].x) );
		}
		fitness[i] = TEST_POINT / (total_error + TEST_POINT);
	}
}

// 一様交叉
void cross_over(Node *p1, Node *p2, Node **c1, Node **c2, long int size)
{
	int cp1, cp2;
	Node *q1, *q2;

	do{
		*c1 = free_tree(*c1);
		*c2 = free_tree(*c2);

		*c1 = tree_copy(p1);
		*c2 = tree_copy(p2);

		//cp1 = random_number() * tree_length(*c1);
		//cp2 = random_number() * tree_length(*c2);
		cp1 = run->get() * tree_length(*c1);
		cp2 = run->get() * tree_length(*c2);

		if((cp1 != 0) && (cp2 != 0)){			//子同士の交換
			q1 = key_serch(*c1, cp1);
			q2 = key_serch(*c2, cp2);
			node_swap1(q1, q2);
		}
		else if((cp1 == 0) && (cp2 != 0)){		//rootと子の交換
			q2 = key_serch(*c2, cp2);
			node_swap2(c1, q2);
		}
		else if((cp1 != 0) && (cp2 == 0)){
			q1 = key_serch(*c1, cp1);
			node_swap2(c2, q1);
		}
		else{									//root同士の交換
			node_swap3(c1, c2);
		}
	}while((tree_length(*c1) > size) || (tree_length(*c2) > size));

	tree_number_set(*c1);
	tree_number_set(*c2);
}

//突然変異
void mutate(Node *p, Node **c, unsigned int size){
	int cp;
	char *buf = new char[LINE_WIDE];
	Node *q, *mt = 0;

	do{
		//突然変異点の決定
		*c = free_tree(*c);
		*c = tree_copy(p);
		cp = random_number() * tree_length(*c);
		q = key_serch(*c, cp);

		//突然変異木の作成
		do{
			memset(buf, 0, LINE_WIDE);
			mt = free_tree(mt);
			rnd_tree(buf, size);
			mt = make_tree(buf);
		}while(tree_error_check(mt));

		if(cp == 0){
			node_swap3(c, &mt);
		}
		else{
			node_swap2(&mt, q);
		}
	}while(tree_length(*c) > size);		//size以下に調整

	tree_number_set(*c);
	delete [] buf;
}

//ランダム選択
void myselect(int *mum, int *dad)
{
	*mum = POP_SIZE * random_number();
	do{
		*dad = POP_SIZE * random_number();
	}while(*mum == *dad);
}

//ルーレット選択
void roulette(int *mum, int *dad, double *rou){
	int i;
	double r;
	double f_val[POP_SIZE];
	double rulette[POP_SIZE];
	double sum_f_val = 0.0;

	//f値の合計を計算
	for(i = 0;i < POP_SIZE;i++){
		if(rou[i] > 0.0){
			f_val[i] = rou[i];
		}
		else{
			f_val[i] = 0.0;
		}
		sum_f_val += f_val[i];
	}

	//sum_f_valが0の場合ランダム選択
	if(sum_f_val == 0){
		myselect(mum, dad);
		return ;
	}

	//mum選択
	rulette[0] = f_val[0] / sum_f_val;				//各個体が選択される確率の設定
	for(i = 1;i < POP_SIZE;i++){
		rulette[i] = rulette[i-1] + (f_val[i] / sum_f_val);
	}
	r = random_number();						//0.0〜1.0の乱数発生
	for(i = 0;i < POP_SIZE;i++){
		if(r <= rulette[i]){
			*mum = i;
			break;
		}
	}

	//dad選択
	f_val[*mum] = 0.0;
	sum_f_val = 0.0;
	for(i = 0;i < POP_SIZE;i++){
		sum_f_val += f_val[i];
	}
	rulette[0] = f_val[0] / sum_f_val;				//各個体が選択される確率の設定
	for(i = 1;i < POP_SIZE;i++){
		rulette[i] = rulette[i-1] + (f_val[i] / sum_f_val);
	}
	r = random_number();						//0.0〜1.0の乱数発生
	for(i = 0;i < POP_SIZE;i++){
		if(r <= rulette[i]){
			*dad = i;
			break;
		}
	}
}

// 初期設定
void init()
{
	// 乱数設定
	time_t now;
	seed = time(&now);
	srand(seed);

	// 入力読み込み
	FILE *fp = fopen("input_x_y.txt", "r");
	assert(fp);
	int a;
	fscanf(fp, "%d", &a);

	int x = ( rand() / (double)RAND_MAX) * (a - TEST_POINT);
	char *s = new char[256];
	for(int i = 0;i < x;i++){
		memset(s, 0, sizeof(s));
		fgets(s, 256, fp);
	}

	for(int i = 0;i < TEST_POINT;i++){
		fscanf(fp, "%d%lf", &in[i].x, &in[i].y);
		//cout << in[i].x << " " << in[i].y << endl;
	}
	//cout << endl;

	delete [] s;
	fclose(fp);

	/*vector<char> c;
	char cs;
	ifstream is("input_x_y.txt");
	string buf;
	stringstream ss;
	is >> ss;
	ss >> a;

	int start = ( rand() / (double)RAND_MAX) * (a - TEST_POINT);
	for(int i = 0;i < start;i++){
		getline(is, buf);
		buf.clear();
	}

	for(int i = 0;i < TEST_POINT;i++){
		getline(is, buf);
		buf >> ss;
		ss >> in[i].x >> cs >> in[i].y;
		buf.clear();
		ss.clear();
	}*/
}

int main()
{
	Node *pop[POP_SIZE];
	Node *new_pop[POP_SIZE];

	int dis[POP_SIZE], dr = 0;
	double sd = 0;

	// 初期設定
	init();

	// 初期個体生成
	string center_point_str;
	pop[0] = make_tree();
	syntax_array(pop[0], &center_point_str);
	pop[0] = free_tree(pop[0]);

	//cout << "初期個体" << endl;
	for(int i = 0;i < POP_SIZE;i++){
		string st;
		Node *f = NULL;
		do{
			f = free_tree(f);
			st.erase();

			f = make_tree();
			syntax_array(f, &st);
			dis[i] = levenshtein_distance(center_point_str, st);
		}while( dis[i] > EDIT_DISTANCE );

		pop[i] = f;
		new_pop[i] = NULL;
		//show_tree(pop[i]);
	}
	//cout << endl;

	double best_fitness = -1.0, gsd = 0.0, gdr = 0.0;
	int g;
	string gstr;
	for(g = 1;(best_fitness < 0.9) && (g <= MAX_GEN);g++){
		//cout << "世代数 : " << g << endl;
		// 標準偏差 分散半径
		sd = standard_deviation(dis);
		dr = deviation_radius(dis);
		gsd = sd;
		gdr = dr;

		//cout << "標準偏差 : " << sd << "\t分散半径 : " << dr << endl;

		// fitness計算
		double fitness[POP_SIZE];
		function_identification(fitness, pop);
		/*for( int i = 0;i < POP_SIZE;i++){
			cout << fitness[i] << " : ";
			show_tree(pop[i]);
		}*/

		double best = -100;
		int    best_no;
		for(int i = 0;i < POP_SIZE;i++){
			if(best < fitness[i]){
				best_no = i;
				best = fitness[i];
			}
		}
		best_fitness = best;
		syntax_array(pop[best_no], &gstr);
		/*cout << "best pop : ";
		show_tree(pop[best_no]);
		cout << "best fitness : " << best << endl;*/

		/**/
		cout << g << ":" << sd << ":" << dr << ":" << best << ":";
		show_tree(pop[best_no]);
		/**/

		// 個体進化
		for(int i = 0;i < POP_SIZE;i++){
			int mum, dad;
			double r = run->get();
			Node *no = NULL;

			// 個体選択
			//myselect(&mum, &dad);			// (ランダム)
			roulette(&mum, &dad, fitness);	//(ルーレット選択)
			//cout << "mum = " << mum << " dad = " << dad << endl;

			if(r < PMUT){
				// 突然変異
				mutate(pop[mum], &new_pop[i], MAX_PROG_SIZE);
			}
			else if(r < PCROSS + PMUT){
				// 一様交叉
				cross_over(pop[mum], pop[dad], &new_pop[i], &no, MAX_PROG_SIZE);
				no = free_tree(no);
			}
			else{
				// コピー
				new_pop[i] = tree_copy(pop[mum]);
			}

			//show_tree(new_pop[i]);
		}

		// 分散半径中心
		center_point_str.erase();
		syntax_array(pop[best_no], &center_point_str);

		// 世代交代
		for(int i = 0;i < POP_SIZE;i++){
			pop[i] = free_tree(pop[i]);
			pop[i] = tree_copy(new_pop[i]);
			new_pop[i] = free_tree(new_pop[i]);
		}

		// 最小分散半径
		for(int i = 0;i < POP_SIZE;i++){
			string st;
			syntax_array(pop[i], &st);
			dis[i] = levenshtein_distance(center_point_str, st);
		}

		//cout << endl;
	}

	// メモリの解放
	for(int i = 0;i < POP_SIZE;i++){
		pop[i] = free_tree(pop[i]);
		new_pop[i] = free_tree(new_pop[i]);
	}

	if(g <= MAX_GEN){
		for(int i = g;i < MAX_GEN;i++){
			cout << i << ":" << gsd << ":" << gdr << ":" << best_fitness << ":" << gstr << endl;
		}
	}

	return 0;
}
