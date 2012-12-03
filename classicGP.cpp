#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <map>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>

static const int 		DEPTH			= 10;
static const int 		N_FUNCTION		= 8;
static const int 		N_TERMINAL		= 2;
static const int 		LINE_WIDE		= 100;
static const int 		MAXGEN			= 1000;
static const int 		MAX_PROG_SIZE	= LINE_WIDE * 2;
static const int		MAXNUM			= 100;
static const double 	THRESHOLD		= 0.5;
static const double 	PCROSS			= 0.7;
static const double 	PMUT			= 0.01;

using namespace std;

struct Node{
	Node *parent;
	int no;
	std::string string;
	int depth;
	Node *left;
	Node *right;
};

struct cont{		// 単語情報格納構造体
	int wordid;
	int tf;
	int df;
};

struct Category{
	map< int, map<int, int> > docs;
	map< int, vector<int> > kywd;
	map< Node*, vector<double> > recalls;
	vector<Node*> trees;
};

// typedef 定義
typedef map< string, Node* > MIN;			// 単語IDと構文木のペア
typedef vector<string> VS;
typedef map< int, map<int, int> > MVM;
typedef map< int, vector<int> > MV;

/*
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

/*/
class Random_number
{
private:
	Random_number()
	{
		//srand(1024);
		srand( (unsigned int)time(NULL) );
	}
	Random_number( const Random_number& rhs);
	Random_number& operator=(const Random_number& rhs);

public:
	static Random_number* getInstance()
	{
		static Random_number r;
		return &r;
	}

	double get()
	{
		//double r = (double)rand()/RAND_MAX;
		double d = -1.0;
		while( (d <= 0) || (1 <= d) ){
			d = (double)rand() / RAND_MAX;
		}
		return d;
	}
};/**/

Random_number *run = Random_number::getInstance();

/*
// stringからintへ変換
int stoi(string str)
{
	istringstream iss( str.data() );
	int i;
	iss >> i;
	return i;
}
*/

// stringからdoubleへ変換
double stod(string s)
{
	double d;
	istringstream iss;
	iss.str(s);
	iss >> d;
	return d;
}

// intからstringへ変換
string itos(int i)
{
	stringstream iss;
	iss << i;
	return iss.str();
}

// 構文木情報
void _tree_info(Node* p)
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

void tree_info(Node* p)
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
void node_info(Node* p)
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

// 後置記法
void _show_tree(Node* p)
{
	cout << p->string << " ";
	if(p->left != NULL){
		_show_tree(p->left);
	}
	if(p->right != NULL){
		_show_tree(p->right);
	}
}

void show_tree(Node* p)
{
	_show_tree(p);
	cout << endl;
}

// 構文木の消去
Node* free_tree(Node* p)
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

// 演算子の選択
int random_function()
{
	static const int function[8] = { '+', '-', '*', '/', 's', 'c', 'l', 'e' };
	return ( function[(int) (run->get() * N_FUNCTION)]);
}
// 端末の選択
int random_terminal()
{
	static const int terminal[3] = { 'T', 'D', 'N' };
	return ( terminal[(int) (run->get() * N_TERMINAL)] );
}

// 番号の再設定
long int _number_reset(Node* p, long int* i, long int d)
{
	p->no = *i;
	*i += 1;
	p->depth = d++;
	if(p->left != NULL){
		*i = _number_reset(p->left, i, d);
	}
	if(p->right != NULL){
		*i = _number_reset(p->right, i, d);
	}
	return *i;
}

void number_reset(Node *p)
{
	long int i = 0, d = 0;
	p->no = i++;
	p->depth = d++;
	if(p->left != NULL){
		i = _number_reset(p->left, &i, d);
	}
	if(p->right != NULL){
		i = _number_reset(p->right, &i, d);
	}
}

// ランダムに構文木の生成
Node* _build_tree(Node* parent, int num, int d)
{
	string buff;

	if(d > DEPTH){
		return NULL;
	}
	else if( (int)(run->get()+0.5) >= 1 ){
		buff += random_function();
	}
	else{
		buff += random_terminal();
	}

	Node* p = new Node();
	assert(p);

	p->parent = parent;
	p->left   = NULL;
	p->right  = NULL;
	p->string = buff;
	p->depth  = d;
	p->no     = num;

	if( (p->string == "T") || (p->string == "D") ){
		return p;
	}

	p->left = _build_tree(p, num*2+1, ++d);
	if( (p->string != "s") && (p->string != "c") && (p->string != "l") && (p->string != "e") ){
		p->right = _build_tree(p, num*2+2, d);
	}
	return p;
}

Node* build_tree()
{
	Node *root = _build_tree(NULL, 0, 0);
	number_reset(root);
	return ( root );
}

// 構文木からstringへ変換
void _syntax_array(Node* p, string* str)
{
	str[p->no] = p->string;
	if(p->left != NULL){
		_syntax_array(p->left, str);
	}
	if(p->right != NULL){
		_syntax_array(p->right, str);
	}
}

string syntax_array(Node* p)
{
	string str[LINE_WIDE], re;
	_syntax_array(p, str);

	for(int i = 0;i < LINE_WIDE;i++){
			re += str[i];
	}

	return re;
}

// 木のコピー
Node* _tree_copy(Node* p, Node* parent)
{
	if(p == NULL){
		return NULL;
	}
	else{
		Node *a = new Node();
		assert(a);

		a->parent = parent;
		a->depth  = p->depth;
		a->no	  = p->no;
		a->string = p->string;

		if(p->left != NULL){
			a->left = _tree_copy(p->left, a);
		}
		if(p->right != NULL){
			a->right = _tree_copy(p->right, a);
		}
		return a;
	}
}

Node* tree_copy(Node* p)
{
	return ( _tree_copy(p, NULL) );
}

// 構文木長さ
void _tree_length(Node *p, int *count)
{
	*count += 1;
	if(p->left != NULL){
		_tree_length(p->left, count);
	}
	if(p->right != NULL){
		_tree_length(p->right, count);
	}
}

int tree_length(Node *p)
{
	if(p == NULL){
		return 0;
	}

	int count = 1;
	if(p->left != NULL){
		_tree_length(p->left, &count);
	}
	if(p->right != NULL){
		_tree_length(p->right, &count);
	}
	return count;
}

// 交叉点のポインタの走査
Node* _key_serch(Node* p, const int x, int* count)
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

Node* key_serch(Node* p, const int x)
{
	Node *q = NULL;
	int count = 1;

	if( (x == 0) || (x >= tree_length(p)) ){
		return p;
	}

	if(p->left != NULL){
		q = _key_serch(p->left, x, &count);
	}
	if((p->right != NULL) && (q == NULL)){
		q = _key_serch(p->right, x, &count);
	}

	return q;
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

	number_reset(*c1);
	number_reset(*c2);
}

//突然変異
void mutate(Node *p, Node **c, long int size){
	int cp;
	Node *q, *mt = NULL;

	do{
		//突然変異点の決定
		*c = free_tree(*c);
		*c = tree_copy(p);
		cp = run->get() * tree_length(*c);
		q = key_serch(*c, cp);

		//突然変異木の作成
		mt = free_tree(mt);
		mt = build_tree();

		if(cp == 0){
			node_swap3(c, &mt);
		}
		else{
			node_swap2(&mt, q);
		}
	}while(tree_length(*c) > size);		//size以下に調整
	//}while(1);

	number_reset(*c);
}

double mach_string(string str, double tf, double df)
{
	if( str == "T" ){
		return tf;
	}
	else if( str == "D" ){
		return df;
	}
	else{
		return (double)atof(str.c_str());
	}
}

/*---木の計算---*/
double prog_value(Node *p, double tf, double df)
{
	double l = 0.0, r = 0.0;

	if(p->left != NULL){
		l = prog_value(p->left, tf, df);
	}
	if(p->right != NULL){
		r = prog_value(p->right, tf, df);
	}
	if( (p->left == NULL) && (p->right == NULL) ){
		return mach_string(p->string, tf, df);
	}

	if( p->string == "+" ){
		return (l + r);
	}
	else if( p->string == "-" ){
		return (l - r);
	}
	else if( p->string == "*" ){
		return (l * r);
	}
	else if( p->string == "/" ){
		if(r == 0.0){
			return 0.0;
		}
		else{
			return (l / r);
		}
	}
	else if( p->string == "s"){
		return (sin(l));
	}
	else if( p->string == "c" ){
		return (cos(l));
	}
	else if( p->string == "e" ){
		return (exp(l));
	}
	else if( p->string == "l" ){
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

double test_fitness(Node *p, double tf, double df)
{
	double calc = prog_value(p, tf, df);

	if(__isnan(calc) || __isinf(calc)){
		return 0.0;
	}
	else{
		return calc;
	}
}

// 文字列の分割
VS split(string str, string delim) {
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

// mapソート関数
bool compare(const map<int, double>::const_iterator& lhs, const map<int, double>::const_iterator& rhs)
{
	return (lhs->second > rhs->second);
}

bool compareN(const map<Node*, double>::const_iterator& lhs, const map<Node*, double>::const_iterator& rhs){
	return (lhs->second > rhs->second);
}

/*
template <typename T>
struct strmap{
	map<T, double> mtd;
};

template <typename T>
bool compare(const strmap<T>& lhs, const strmap<T>& rhs){
	return NULL;
}
*/

// ランキングルーレット
void ranking_roulet(Category c, Node** pop1, Node** pop2)
{
	map<Node*, double> sum;		// 木毎のリコール値の総数
	map<Node*, double> p;		// 選択確率
	map< Node*, vector<double> >::iterator it = c.recalls.begin();
	while(it != c.recalls.end()){
		double s = 0.0;

		vector<double>::iterator v = (*it).second.begin();
		while(v != (*it).second.end()){
			s += *v;
			v++;
		}
		sum.insert( make_pair((*it).first, s) );
		//cout << s << " ";
		//show_tree((*it).first);

		it++;
	}

	// 選択確率
	int plus = 0;
	double ds = 0.0, r;
	size_t si;

	// ソート
	vector< map<Node*, double>::const_iterator > sorted;
	for(map<Node*, double>::const_iterator itf = sum.begin(); itf != sum.end(); ++itf){
		sorted.push_back(itf);
	}
	sort(sorted.begin(), sorted.end(), compareN);

	for(int j = c.recalls.size();j > 0;j--){
		plus += j;
	}

	// 親１
	si = sorted.size() - 1;
	for(vector< map<Node*, double>::const_iterator >::reverse_iterator it1 = sorted.rbegin(); it1 != sorted.rend(); it1++){
		ds += (double)(sorted.size() - si) / plus;
		p.insert( make_pair((*it1)->first, ds) );
		//cout << ds << " " << sum[(*it1)->first] << " ";
		//show_tree((*it1)->first);
		si--;
	}

	r = run->get();
	sorted.clear();
	//cout << "rand > " << r << endl;
	for(map<Node*, double>::const_iterator itf = p.begin(); itf != p.end(); ++itf){
		sorted.push_back(itf);
	}
	sort(sorted.begin(), sorted.end(), compareN);

	for(vector< map<Node*, double>::const_iterator >::reverse_iterator it2 = sorted.rbegin(); it2 != sorted.rend(); it2++){
		//cout << (*it2)->second << " ";
		//show_tree((*it2)->first);
		if(r < (*it2)->second){
			*pop1 = (*it2)->first;
			sum.erase( *pop1 );
			break;
		}
	}

	// 親２
	si = sorted.size() - 1;
	for(vector< map<Node*, double>::const_iterator >::reverse_iterator it1 = sorted.rbegin(); it1 != sorted.rend(); it1++){
		ds += (double)(sorted.size() - si) / plus;
		p.insert( make_pair((*it1)->first, ds) );
		//cout << ds << " " << sum[(*it1)->first] << " ";
		//show_tree((*it1)->first);
		si--;
	}

	r = run->get();
	sorted.clear();
	//cout << "rand > " << r << endl;
	for(map<Node*, double>::const_iterator itf = p.begin(); itf != p.end(); ++itf){
		sorted.push_back(itf);
	}
	sort(sorted.begin(), sorted.end(), compareN);

	for(vector< map<Node*, double>::const_iterator >::reverse_iterator it2 = sorted.rbegin(); it2 != sorted.rend(); it2++){
		//cout << (*it2)->second << " ";
		//show_tree((*it2)->first);
		if(r < (*it2)->second){
			*pop2 = (*it2)->first;
			break;
		}
	}

	//show_tree(*pop1);
	//show_tree(*pop2);
}

// エリート保存
Node* save_elite(Category c)
{
	double max = 0.0;
	Node *p = NULL;
	map< Node*, vector<double> >::iterator it = c.recalls.begin();
	while(it != c.recalls.end()){
		double sum = 0.0;
		vector<double>::iterator it1 = (*it).second.begin();
		while(it1 != (*it).second.end()){
			sum += *it1;
			it1++;
		}
		if(max <= sum){
			max = sum;
			p = free_tree(p);
			p = tree_copy( (*it).first );
		}
		it++;
	}
	return p;
}

int main()
{
	ifstream ifs;
	string str;

	MVM tfdata;								// TFデータ
	map<int, int> dfdata;					// DFデータ
	MV kywddata;							// キーワードデータ
	map< string, vector<int> > kind;		// 単語種類データ
	map<int, string> rekind;				// 種類相対データ
	vector<Node*> pop;
	vector<Node*> new_pop;

	// 全単語数読み込み
	ifs.open( "./WordData/worddata.txt" );
	if( !(getline(ifs, str)) ){
		cerr << "File Don't Open!!\n";
		exit(0);
	}
	str.erase();
	ifs.close();

	// 単語種類の読み込み
	// 種類相対表の作成
	ifs.open( "./WordData/kind.txt" );
	if( ifs.fail() ){
		cerr << "File Don't Open!!\n";
		exit(0);
	}
	int no = 0;
	while( getline(ifs, str) ){
		VS vs = split( str, " " );
		string s = vs[0];
		vs.erase( vs.begin(), vs.begin()+1 );
		vector<int> vi;

		VS::iterator it = vs.begin();
		while( it != vs.end() ){
			vi.push_back( stoi(*it) );
			it++;
		}

		kind.insert( make_pair(s, vi) );
		rekind.insert( make_pair(no, s) );
		no++;
	}
	str.erase();
	ifs.close();

	// 実験
	// TFファイル読み込み
	ifs.open( "./WordData/experiment/tfdata.txt" );
	if( !(getline(ifs, str)) ){
		cerr << "File Don't Open!!\n";
		exit(0);
	}
	while( getline(ifs, str) ){
		VS t1 = split( str, " " );
		map<int, int> tmp1;

		VS::iterator it = t1.begin();
		it++;
		while( it != t1.end() ){
			VS t2 = split( (*it), ":" );
			tmp1.insert( make_pair(stoi(t2[0]), stoi(t2[1])) );		// ID:回数配列に挿入
			it++;
		}
		tfdata.insert( make_pair(stoi(t1[0]), tmp1) );				// 文書ID、TFデータを挿入
	}
	str.erase();
	ifs.close();



	// DFファイル読み込み
	ifs.open( "./WordData/experiment/dfdata.txt" );
	if( !(getline(ifs, str)) ){
		cerr << "File Don't Open!!\n";
		exit(0);
	}
	while( getline(ifs, str) ){
		VS tmp = split( str, ":" );
		dfdata.insert( make_pair(stoi(tmp[0]), stoi(tmp[1])) );		// 単語ID、DFデータを挿入
	}
	str.erase();
	ifs.close();

	// キーワードファイル読み込み
	ifs.open( "./WordData/experiment/kywddata.txt" );
	if( !(getline(ifs, str)) ){
		cerr << "File Don't Open!!\n";
		exit(0);
	}
	while( getline(ifs, str) ){
		VS tmp = split( str, ":" );
		VS t = split(tmp[1], " ");
		vector<int> v;
		VS::iterator ti = t.begin();
		while(ti != t.end()){
			v.push_back( stoi(*ti) );
			ti++;
		}
		kywddata.insert( make_pair(stoi(tmp[0]), v));				// 文書ID、キーワードデータを挿入
	}
	str.erase();
	ifs.close();

	// 構造体の初期化
	Category cates;
	for(int i = 0;i < MAXNUM;i++){
		cates.trees.push_back( build_tree() );
	}
	cates.docs = tfdata;
	cates.kywd = kywddata;

	// 遺伝的進化
	for(int gene = 1;gene <= MAXGEN;gene++){
		cout << gene << " ";

		auto it1 = cates.docs.begin();
		while(it1 != cates.docs.end()){
			int doc = (*it1).first;
			//cout << "DocID > " << doc << endl;

			vector<cont> tmp;
			vector<int>  tmp1 = cates.kywd[doc];	// キーワードをコピー

			// 文書の単語をコピー
			auto ii = (*it1).second.begin();
			while(ii != (*it1).second.end()){
				cont c;
				c.wordid = (*ii).first;
				c.tf = (*ii).second;
				c. df = dfdata[(*ii).first];
				//cout << " WordID > " << c.wordid << "  TF > " << c.tf << " DF > " << c.df << endl;
				//cout << (*ii).first << " " << c.wordid << endl;
				tmp.push_back(c);
				ii++;
			}

			auto it3 = cates.trees.begin();
			while(it3 != cates.trees.end()){
				map<int, double> msd;

				auto it = tmp.begin();
				while(it != tmp.end()){
					// Fitness計算
					msd.insert( make_pair((*it).wordid, test_fitness(*it3, (*it).tf, (*it).df)) );
					it++;
				}

				// Fitnessのソート
				vector< map<int, double>::const_iterator > sorted;
				for(auto it = msd.begin();it != msd.end();it++){
					sorted.push_back(it);
				}
				sort(sorted.begin(), sorted.end(), compare);

				// キーワードと比較
				int count = 0;
				size_t num = tmp1.size();
				if(sorted.size() < num){
					num = sorted.size();
				}

				auto it1 = sorted.begin();
				for(size_t i = 0;i < num;i++){
					auto it2 = find(tmp1.begin(), tmp1.end(), (*it1)->first);
					if(it2 != tmp1.end()){
						count++;
					}
					it1++;
				}

				// リコール計算
				double recall;
				if(num == 0){
					recall = 0.0;
				}
				else{
					recall = (double)count / num;
				}

				cates.recalls[*it3].push_back(recall);
				it3++;
			}

			it1++;
		}

		// 木毎の平均リコールの計算
		map<Node*, double> reave;
		vector<Node*>::iterator vi = cates.trees.begin();
		while(vi != cates.trees.end()){
			double sum = 0.0;
			vector<double>::iterator v = cates.recalls[*vi].begin();
			while(v != cates.recalls[*vi].end()){
				sum += *v;
				v++;
			}
			//printf("%3f ", sum);
			//show_tree(*vi);
			reave.insert( make_pair(*vi, (sum/cates.recalls[*vi].size())) );

			vi++;
		}

		// 最も平均リコールの高い構文木の選出
		vector< map<Node*, double>::const_iterator > sorted;
		for(map<Node*, double>::const_iterator itt = reave.begin(); itt != reave.end(); ++itt){
			sorted.push_back(itt);
		}
		sort(sorted.begin(), sorted.end(), compareN);
		vector< map<Node*, double>::const_iterator >::iterator itt = sorted.begin();
		cout << " " << (*itt)->second << endl;
		//printf(" %3.2f ", (*itt)->second);
		//show_tree((*itt)->first);

		//cout << "GENE\n";
		// セレクション、進化
		// 世代交代
		// ランキングルーレット選択
		vector<Node*> nv;
		nv.push_back( tree_copy((*itt)->first) );		// エリート保存

		int num = MAXNUM - nv.size();
		for(int j = 0;j < num;j++){

			Node *pop1 = NULL, *pop2 = NULL;
			Node *a = NULL, *b = NULL;

			ranking_roulet(cates, &pop1, &pop2);		// ランキングルーレット

			// 突然変異、交叉、コピー
			double r = run->get();
			if(r < PMUT){
				mutate( pop1, &a, MAX_PROG_SIZE );
			}
			else if(r < PCROSS + PMUT){
				cross_over( pop1, pop2, &a, &b, MAX_PROG_SIZE );
				b = free_tree( b );
			}
			else{
				a = tree_copy( pop1 );
			}

			nv.push_back( a );
		}

		//　世代交代
		vector<Node*>::iterator ii = cates.trees.begin();
		while(ii != cates.trees.end()){
			free_tree(*ii);
			ii++;
		}
		cates.trees.clear();
		cates.recalls.clear();

		cates.trees = nv;
	}

	// メモリの解放
	auto vv = pop.begin();
	while(vv != pop.end()){
		free_tree(*vv);
		vv++;
	}

	return 0;
}
