// Must be compiled with -std=c++17 -fwrapv 

/* Input format:

<value> <name>
<value> <name>
-
<query_value>
<query_value>

Example:

12 I
34 you
56 Romeo

Names should not contain spaces.
*/

#include<iostream>
#include<unordered_map>
#include<vector>
#include<cmath>
#include<deque>
#include<string>


static unsigned constexpr strlen_(char const* st){
	return *st?1+strlen_(1+st):0;
}

struct String{

private:
	struct data;
	static std::vector<data> pool;
	struct data{
		union{
			char const* str;
			struct {unsigned left,right;};
		};
		bool leaf;
		unsigned length;

		constexpr data(char const* str)
		:str(str),leaf(true),length(strlen_(str)) {}
		constexpr data(unsigned left,unsigned right,unsigned length)
		:left(left),right(right),leaf(false),length(length) {}

		void print(std::ostream& stream){
			if(leaf)
				stream<<str;
			else{
				pool[left].print(stream);
				pool[right].print(stream);
			}
		}
	};
	// store the actual data.

	// the actual "String" consists of a sole index to (pool)
	unsigned index;

public:
	String(char const* str):index(pool.size()){
		pool.emplace_back(str);
	}
	String(String left,String right):index(pool.size()){
		pool.emplace_back(left.index,right.index,left.length()+right.length());
	}

	unsigned length()const{return pool[index].length;}
	friend std::ostream& operator<<(std::ostream&, String);

	// Hacky workaround because there are no garbage collector.
	static void __pop(int count){
		while(count-->0)pool.pop_back();
	}
};
std::vector<String::data> String::pool {};

std::ostream& operator<<(std::ostream& stream, String st){
	String::pool[st.index].print(stream);
	return stream;
}

String operator+(String x,String y){ return String(x,y); }


String const
	DIFF("the difference between"),
	PROD("the product of"),
	QUOT("the quotient between"),
	REM("the remainder of the quotient between"),
	SUM("the sum of"),

	CUBE("the cube of"),
	FACT("the factorial of"),
	SQR("the square of"),
	SQRT("the square root of"),
	TWICE("twice ");

// =============

static_assert(4==sizeof(int),"");

int constexpr BOUND=1000;

std::unordered_map<int,unsigned> index;
std::vector<String> names;
std::vector<int> value;

bool doneat;
void insert(int val,String st){
	if(val>BOUND||val<-BOUND)return;
	if(st.length()>900)return;

	auto it=index.find(val);
	if(it==index.end()){
		index.emplace(val,names.size());
		value.push_back(val);
		doneat=1;
		names.push_back(std::move(st));
		return;
	}else{
		String& name=names[it->second];
		if(st.length()<name.length()){
			doneat=1;
			name=std::move(st);
			return;
		}
	}
}

void insert(int val,String st,int n_pop){
	bool doneat1=doneat;
	doneat=false;
	insert(val,st);
	if(!doneat){
		doneat=doneat1;
		String::__pop(n_pop);
	}
}


auto const factorial_table=[](){
	int constexpr SIZE=34;
	std::array<int,SIZE> factorial_table;
	factorial_table[0]=1;
	for(int i=1;i<SIZE;++i){
		factorial_table[i]=int(i*(long long)factorial_table[i-1]);
		if(factorial_table[i]==0)std::cout<<i<<'\n';
	}
	return factorial_table;
}();

int factorial(int x){
	return x<(int)factorial_table.size()?factorial_table[x]:0;
}

int main(){
	insert(0,String("zero "));

	String st("a ");
	String const BIG("big "), CAT("cat "), PIG("pig ");

	for(int i=1;i;i<<=1,st=st+BIG){
		insert(i,st+CAT,1);
		insert(-i,st+PIG,1);
	}

	int x;
	std::deque<std::string> characters;
	while(std::cin>>x){
		characters.emplace_back();
		std::string& st=characters.back();

		std::cin>>std::ws;
		std::getline(std::cin,st);

		// remove all trailing spaces
		st.erase(st.find_last_not_of(' ')+1+st.begin(),st.end());

		// append one if there isn't any space inside
		if(st.find(' ')==std::string::npos)
			st+=' ';
		
		insert(x,String(st.c_str()),1);
	}

	do{
		doneat=false;
		for(unsigned i=value.size();i-->0;)
		for(unsigned j=value.size();j-->0;){
			int vi=value[i],vj=value[j];
			String ni=names[i], nj=names[j];
			insert(vi-vj,DIFF+ni+nj,2);
			insert(vi*vj,PROD+ni+nj,2);
			if(vj){
				insert(vi/vj,QUOT+ni+nj,2);
				insert(vi%vj,REM+ni+nj,2);
			}
			insert(vi+vj,SUM+ni+nj,2);
		}

		for(unsigned i=value.size();i-->0;){
			int v=value[i];
			String n=names[i];

			insert(v*v*v,CUBE+n,1);
			if(v>=0) insert(factorial(v),FACT+n,1);
			insert(v*v,SQR+n,1);
			if(v>=0) insert((int)std::sqrt(v),SQRT+n,1);
			insert(v<<1,TWICE+n,1);
		}

	}while(doneat);

	std::cin.ignore(1);
	std::cin.clear();
	while(std::cin>>x){
		auto it=index.find(x);
		if(it==index.end()){
			std::cout<<x<<": not found\n";
			continue;
		}
		std::cout<<x<<": "<<names[it->second]<<'\n';
	}
}
