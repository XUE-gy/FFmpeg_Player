// CMakeProject_simple.cpp: 定义应用程序的入口点。
//

#include "CMakeProject_simple.h"

using namespace std;
void test1();
void test2();
void print(int[]);
void print(int(*)[2]);
void print1(int(&)[2]);
void print(char[]);
void print(const int *beg,const int *end);
void error_msg(initializer_list<string> ls);
int(*func(int a))[10];
constexpr size_t scale(size_t c);

bool lengthCompare(const string& s1, const string& s2);

typedef bool(*lengthCompare2)(const string& s1, const string& s2);

bool useBigger(const string& s1, const string& s2,lengthCompare2);

using PF = int(*)(int*,int);

struct Salas_data {
	std::string isbn() const { return bookNo; }
	std::string bookNo;
	std::string isbn2() const { return this->bookNo; }
};


int main(int argc, char* argv[])
{
	//test1();
	test2();
	return 0;

}

void test2() {

	Salas_data total;
	total.bookNo = "23";
	cout << total.isbn() << " " << total.isbn2()<<endl;




}

void test1() {
	//cout << "Hello CMake_simple." << endl;

//int a[2] = { 1,2 };
//char b[3] = { '1','2' };
//int(*p1)[2] = &a;
//int(&p2)[2] = a;
//print(p1);
//print1(p2);
//print(b);
//print(begin(a),end(a));

//if (argc < 2) {
//	cout << "太短" << endl;

//}
//else {
//	cout << "第一个字符串是： " << argv[0] << endl;
//	cout << "第二个字符串是： " << argv[1] << endl;
//}

	error_msg({ "dfd","aa","bb" });
	error_msg({ "dfds","aa" });
	cout << endl;

	int k1[10] = { 1,2,3,4 };
	//auto cc = func(15);
	//for (auto elem : *cc) {
	//	cout << elem << " ";

	//}

	//const string s1 = "sss";
	//string &s2 = const_cast<string&>(s1);
	//const string& s3 = s1;
	//const string& s4 = s2;
	//cout << s4 << endl;

	//int a1 = 5;
	//const int  a2 = 6;
	//int arr[a1] = { 1 };

	//int arr[scale(1)];
	//arr[0] = 1;
	//cout << arr[0] << endl;

	//constexpr int a6 = 24;
	//auto a5 = scale(1);
	//auto a7 = a6;
	//int arr[a6];
	//int arr[a7];
	const string s1 = "ssgggg", s2 = "sss0";
	auto pf = lengthCompare;
	cout << pf(s1, s2) << endl;

	auto pf2 = pf(s1, s2);


	//auto pf2 = lengthCompare2;

	//auto c = lengthCompare2(s1, s2);

	cout << "useBigger " << useBigger(s1, s2, pf);

	PF f1(int);
	//auto c = f1(2);
	//cout << c;





}

bool useBigger(const string& s1, const string& s2, lengthCompare2) {
	//auto kk= (*lengthCompare2)(s1,s2);
	//lengthCompare2 c(const string & s1, const string & s2);
	//
	//return c(s1, s2);
	return true;

}


bool lengthCompare(const string& s1, const string& s2) {
	return (s1.length() > s2.length());
}

constexpr size_t scale(size_t c) {
	const int a3 = 1;
	//return  a3 * c;
	return 1;
}

static int k2_su[10] = {0,1,2};
//返回数组长度为10的数组的指针
int(*func(int a))[10]{
	//int k2[10] = {a,0};
	int(*p1)[10] = &k2_su;
	
	return p1;
}

void error_msg(initializer_list<string> ls) {
	for (auto beg = ls.begin(); beg != ls.end(); ++beg) {
		cout << *beg << "  ";
	}
	cout << endl;
}



void print(int a[]) {
	cout << "print1"<<endl;
	//cout << a[3];
	if (a)
		while (*a)
			cout << *a++;
}
void print(int (*a)[2]) {
	cout << "print1" << endl;
	//cout << a[3];
	//if (a) {
	//	int a1 = 1;
	//	while (*a && a1 != 5) {
	//		cout << *a++;
	//		a1++;
	//	}
	//}
	for (auto elem : *a) {
		cout << elem << endl;

	}
		
}

void print1(int (&a)[2]) {
	cout << "print1" << endl;

	for (auto elem : a) {
		cout << elem << endl;

	}


}

void print(char b[]) {
	cout << "print2" << endl;
	//cout << a[3];
	if (b)
		while (*b)
			cout << *b++;
}

void print(const int* beg, const int* end){
	cout << "print3" << endl;
	//cout << a[3];
	while (beg != end) {
		cout << *beg++;
	}
}
