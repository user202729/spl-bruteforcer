// Must be compiled with -fwrapv

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
#include<string>
#include<cmath>
#include<array>

static_assert(4==sizeof(int),"");

int constexpr BOUND=1000;

std::unordered_map<int,unsigned> index;
std::vector<std::string> names;
std::vector<int> value;

bool doneat;
void insert(int val,std::string st){
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
		std::string& name=names[it->second];
		if(st.length()<name.length()){
			doneat=1;
			name=std::move(st);
			return;
		}
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
	insert(0,"zero");

	std::string st;
	st="a ";
	for(int i=1;i;i<<=1,st+="big "){
		insert(i,st+"cat");
		insert(-i,st+"pig");
	}

	int x;
	#ifndef Sublime
	while(std::cin>>x){
		std::cin>>std::ws;
		std::getline(std::cin,st);
		std::cout<<'"'<<st<<"\" -> "<<x<<'\n';
		insert(x,std::move(st));
		st.clear();
	}
	#endif

	do{
		doneat=false;
		for(unsigned i=value.size();i-->0;)
		for(unsigned j=value.size();j-->0;){
			int vi=value[i],vj=value[j];
			std::string ni=names[i], nj=names[j];
			insert(vi-vj,"the difference between"+ni+' '+nj);
			insert(vi*vj,"the product of"+ni+' '+nj);
			if(vj){
				insert(vi/vj,"the quotient between"+ni+' '+nj);
				insert(vi%vj,"the remainder of the quotient between"+ni+' '+nj);
			}
			insert(vi+vj,"the sum of"+ni+' '+nj);
		}

		for(unsigned i=value.size();i-->0;){
			int v=value[i];
			std::string n=names[i];

			if(n.empty()){
				std::cout<<i<<" empty"<<'\n';
				return 1;
			}

			insert(v*v*v,"the cube of"+n);
			if(v>=0) insert(factorial(v),"the factorial of"+n);
			insert(v*v,"the square of"+n);
			if(v>=0) insert((int)std::sqrt(v),"the square root of"+n);
			insert(v<<1,"twice "+n);
		}

	}while(doneat);

	std::cin>>st;
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
