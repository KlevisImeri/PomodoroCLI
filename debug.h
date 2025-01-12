// clang-format off
void print(char x){cerr<<'\''<<x<<'\'';}
void print(string &x){cerr<<'"'<<x<<'\"';}
void print(const char* x){cerr<<'"'<<x<<'"';};
void print(bool x){cerr<<(x?"true":"false");}

template <typename T>
void print(T& x){
  if constexpr(is_arithmetic_v<T>) cerr<<x;
	else if constexpr(requires{x.print();}) x.print();
	else{
		int f=0;
		cerr<<'{';
		for(auto &i:x)
			cerr<<(f++ ?",":""),print(i);
		cerr<<"}";
	}
}

void _print() {}
template <typename T, typename... V> 
void _print(T t, V... v) {
  print(t);
  if (sizeof...(v)) cerr<<", ";
  _print(v...);
}

#define d(x...) cerr<<"["<<#x<<"]: ";_print(x);cerr<<"\n";
#define out(x...) void print(){cerr<<'{';_print(x);cerr<<"}";}

