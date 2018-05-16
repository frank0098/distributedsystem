#include "simple_json.h"


Json::Json() noexcept:_ptr(statics().null){}
Json::Json(std::nullptr_t) noexcept:_ptr(statics().null){}
Json::Json(const std::string& value):_ptr(make_shared<Json_string>(value)){}
Json::Json(std::string &&value):_ptr(make_shared<Json_string>(std::move(value))){}
Json::Json(const char* value):_ptr(make_shared<Json_string>(value)){}
Json::Json(const array &values):_ptr(make_shared<Json_array>(values)){}
Json::Json(array &&values):_ptr(make_shared<Json_array>(std::move(values))){}
Json::Json(const object &values):_ptr(make_shared<Json_object>(values)){}
Json::Json(object &&values):_ptr(make_shared<Json_object>(std::move(values))){}


 
Json::Type Json::type() const{
	return _ptr->type();
}
const string & Json::string_value () const{
	return _ptr->string_value();
}
const vector<Json> & Json::array_items() const{
	return _ptr->array_items();
}
const map<string,Json> & Json::object_items() const{
	return _ptr->object_items();
}
const Json & Json::operator[](size_t i) const{
	return (*_ptr)[i];
}
const Json & Json::operator[](const string& key) const{
	return (*_ptr)[key];
}
const Json & Json_array::operator[](size_t i) const{
	if(i>=_value.size()) return static_null();
	return _value[i];
}
const Json & Json_object::operator[](const string &key) const{
	auto iter=_value.find(key);
	return iter==_value.end()?static_null():iter->second;
}
const string &Json_value::string_value() const {
	return statics().empty_string; 
}
const vector<Json> &Json_value::array_items() const { 
	return statics().empty_vector;
}
const map<string, Json> &Json_value::object_items() const {
	return statics().empty_map;
}
const Json &Json_value::operator[] (size_t) const{
	return static_null();
}
const Json &Json_value::operator[] (const string &) const{
	return static_null(); 
}

string Json_parser::encode(const Json& json){
	if(json.is_null()){
		return "null";
	}
	if(json.is_string()){
		return "\""+json.string_value()+"\"";
	}
	if(json.is_array()){
		string res="";
		res+="[";
		for(const auto& x:json.array_items()){
			res+=encode(x);
			res+=",";
		}
		if(res.back()==',') res.pop_back();
		res+="]";
		return res;
	}
	if(json.is_object()){
		string res="{";
		for(const auto& x:json.object_items()){
			res+="\"";
			res+=x.first;
			res+="\"";
			res+=":";
			res+=encode(x.second);
			res+=",";
		}
		if(res.back()==',') res.pop_back();
		res+="}";
		return res;
	}
	throw std::exception();
}
Json Json_parser::decode(string str){
	if(str.size()==0 or str=="null"){
		return Json();
	}
	if(str.size()>=2 and str[0]=='\"' and str.back()=='\"'){
		return Json(str.substr(1,str.size()-2));
	}
	if(str.size()>=2 and str[0]=='[' and str.back()==']'){
		vector<Json> v;
		int l=1;
		int r=l;
		while(l<str.size()){
			if(r<str.size() && (str[r]=='[' || str[r]=='{')){
				stack<char> stk;
				stk.push(str[r]);
				while(r<str.size() and !stk.empty()){
					r++;
					if(str[r]=='[' or str[r]=='{') stk.push(str[r]);
					else if(str[r]==']' and stk.top()=='[') stk.pop();
					else if(str[r]=='}' and stk.top()=='{')
						stk.pop();
				}
				v.push_back(decode(str.substr(l,r-l+1)));
				if(str[r]==']')break;
				l=r+2;
				r=l;
			}
			else{
				while(r<str.size() && !(str[r]==',' or str[r]==']')) r++;
				v.push_back(decode(str.substr(l,r-l)));
				if(str[r]==']')break;
				l=r+1;
				r=l;
			}
		}
		return Json(v);
	}
	if(str.size()>=2 and str[0]=='{' and str.back()=='}'){
		map<string,Json> m;
		int l=1;
		int r=l;
		while(l<str.size()){
			while(r<str.size() && str[r]!=':'){ 
				r++;
			}
			string key=str.substr(l+1,r-l-2);
			l=r+1;
			r=l;
			if(r>=str.size()) break;
			if(str[r]=='[' or str[r]=='{'){
				stack<char> stk;
				stk.push(str[r]);
				while(r<str.size() and !stk.empty()){
					r++;
					if(str[r]=='[' or str[r]=='{') stk.push(str[r]);
					else if(str[r]==']' and stk.top()=='[') stk.pop();
					else if(str[r]=='}' and stk.top()=='{')stk.pop();
				}
				m[key]=decode(str.substr(l,r-l+1));
				if(str[r]=='}') break;
				l=r+2;
				r=l;
			}	
			else{
				while(r<str.size() && !(str[r]==',' or str[r]=='}')){
					r++;
				}
				m[key]=decode(str.substr(l,r-l));
				if(str[r]=='}') break;
				l=r+1;
				r=l;			
			}
		}
		return Json(m);
	}
	throw std::exception();

}
