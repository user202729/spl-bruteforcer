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
#include<queue>
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

int constexpr BOUND=10000;

std::unordered_map<int,String> names,fixed_names;
struct state{
	unsigned len;int val;
	state(unsigned len,int val):len(len),val(val){}
	bool operator<(state x)const{return len>x.len;}
};
std::priority_queue<state> pq; // for Dijkstra's algorithm (variant)
// to find the shortest representation of each number

// Return (true) if insert successful, (false) otherwise.
bool insert_(int val,String new_name){
	if(
		val>BOUND||val<-BOUND or
		fixed_names.count(val) or
		new_name.length()>900
	)return false;

	auto it=names.find(val);
	if(it==names.end()){
		names.emplace(val,new_name);
		return true;
	}else{
		String& name=it->second;
		if(new_name.length()<name.length()){
			name=new_name;
			return true;
		}
	}
	return false;
}

// also do (manual) GC and update (pq).
void insert(int val,String new_name,int n_pop){
	if(insert_(val,new_name) /* successful */)
		pq.emplace(new_name.length(),val);
	else
		String::__pop(n_pop);
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
	insert(0,String("zero "),1);

	String st("a ");
	String const BIG("big "), CAT("cat "), PIG("pig ");

	for(int i=1;i;i<<=1,st=st+BIG){
		insert( i,st+CAT,1);
		insert(-i,st+PIG,1);
	}

	int charv; // value of characters
	std::deque<std::string> charn; // name of characters
	// used to store the char* pointers safely

	while(std::cin>>charv){ // read characters (8 I, 100 you, ...)
		charn.emplace_back();
		std::string& st=charn.back();

		std::cin>>std::ws;
		std::getline(std::cin,st);

		// remove all trailing spaces
		st.erase(st.find_last_not_of(' ')+1+st.begin(),st.end());

		// append one if there isn't any space inside
		if(st.find(' ')==std::string::npos)
			st+=' ';

		insert(charv,String(st.c_str()),1);
	}

	std::cin.ignore(1);
	std::cin.clear();
	int queryv;
	while(std::cin>>queryv){

		auto it=fixed_names.find(queryv);
		if(it==fixed_names.end()){

			while(true){

				state st{0,0};
				do{
					if(pq.empty()){
						goto break_outer;
						// can't do anything more
					}
					st=pq.top();pq.pop();
				}while( fixed_names.count(st.val) or (
					it=names.find(st.val),st.len!=it->second.length()
				));

				fixed_names.insert(*it); // the (val, name) pair
				String const ni=it->second;
				names.erase(it);

				int const vi=st.val;
				if(vi==0)continue;

				for(auto [vj,nj]:fixed_names)if(
					vj!=0 // a-0=a+0=a, a*0=0
					and vj!=vi // a-a=0, a+a='twice a', a*a='the square of a',
					// a/a='a cat',a%a=0
				){
					insert(vi-vj,DIFF+ni+nj,2);
					insert(vj-vi,DIFF+nj+ni,2);
					
					insert(vi*vj,PROD+ni+nj,2);
					
					insert(vi/vj,QUOT+ni+nj,2);
					insert(vj/vi,QUOT+nj+ni,2);

					insert(vi%vj,REM+ni+nj,2);
					insert(vj%vi,REM+nj+ni,2);

					insert(vi+vj,SUM+ni+nj,2);
				}

				insert(vi*vi*vi,CUBE+ni,1);
				if(vi>=0) insert(factorial(vi),FACT+ni,1);
				insert(vi*vi,SQR+ni,1);
				if(vi>=0) insert((int)std::sqrt(vi),SQRT+ni,1);
				insert(vi<<1,TWICE+ni,1);

				if(st.val==queryv)
					break;

			}
			break_outer:;
		}

		it=fixed_names.find(queryv);
		if(it==fixed_names.end()){
			std::cout<<queryv<<": not found\n";
		}else{
			std::cout<<queryv<<": "<<it->second<<'\n';
		}

	}

}
