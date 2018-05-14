#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <stack>
using std::string;
using std::vector;
using std::stack;
using std::map;
using std::make_shared;


struct Null_struct {
    bool operator==(Null_struct) const { return true; }
    bool operator<(Null_struct) const { return false; }
};


class Json_value;
class Json final{
public:
	enum Type {
        NUL,STRING, ARRAY, OBJECT
    };
    typedef std::vector<Json> array;
    typedef std::map<std::string, Json> object;
    Json() noexcept;
    Json(std::nullptr_t) noexcept;
    Json(const std::string& value);
    Json(std::string &&value);
    Json(const char* value);
    Json(const array &values);
    Json(array &&values);
    Json(const object &values);
    Json(object &&values);
    Type type() const;
	bool is_null()   const { return type() == NUL; }
	bool is_string() const { return type() == STRING; }
	bool is_array()  const { return type() == ARRAY; }
	bool is_object() const { return type() == OBJECT; }
	const string & string_value () const;
	const std::vector<Json> & array_items() const;
	const std::map<std::string, Json> & object_items() const;
	const Json & operator[](size_t) const;
	const Json & operator[](const string& key) const; 

	// Implicit constructor: map-like objects (std::map, std::unordered_map, etc)
	template <class M, typename std::enable_if<
	    std::is_constructible<std::string, decltype(std::declval<M>().begin()->first)>::value
	    && std::is_constructible<Json, decltype(std::declval<M>().begin()->second)>::value,
	        int>::type = 0>
	Json(const M & m) : Json(object(m.begin(), m.end())) {}

	Json(void *) = delete;


private:
	std::shared_ptr<Json_value> _ptr;
};
class Json_value{
	friend class Json;
protected:
	virtual Json::Type type() const=0;
	virtual const string &string_value() const;
	virtual const Json::array &array_items() const;
	virtual const Json::object &object_items() const;
	virtual const Json &operator[](size_t i) const;
	virtual const Json &operator[](const string &key) const;
	virtual ~Json_value(){};
};

static const Json & static_null(){
	static const Json json_null;
	return json_null;
}
template<Json::Type tag,typename T>
class Value:public Json_value{
protected:
	explicit Value(const T &value):_value(value){}
	explicit Value(T &&value):_value(std::move(value)){}
	Json::Type type() const override{
		return tag;
	}
	const T _value;
};
class Json_null final:public Value<Json::NUL,Null_struct>{
public:
	Json_null():Value({}){}
};

class Json_string final:public Value<Json::STRING,string>{
	const string &string_value() const override{return _value;}
public:
	explicit Json_string(const string &value):Value(value){}
	explicit Json_string(string &&value):Value(std::move(value)){}
};

class Json_array final:public Value<Json::ARRAY,Json::array>{
	const Json::array &array_items() const override{return _value;}
	const Json &operator[](size_t i) const override;
public:
	explicit Json_array(const Json::array &value):Value(value){}
	explicit Json_array(Json::array &&value):Value(std::move(value)){}
};

class Json_object final:public Value<Json::OBJECT,Json::object>{
	const Json::object &object_items() const override{return _value;}
	const Json &operator[](const string &key) const override;
public:
	explicit Json_object(const Json::object &value):Value(value){}
	explicit Json_object(Json_object &&value):Value(std::move(value)){}
};

struct Statics {
    const std::shared_ptr<Json_value> null = make_shared<Json_null>();
    const string empty_string;
    const vector<Json> empty_vector;
    const map<string, Json> empty_map;
    Statics() {}
};

static const Statics & statics() {
    static const Statics s {};
    return s;
}
class Json_parser{
public:
	static string encode(const Json& json);
	static Json decode(string str);
};


