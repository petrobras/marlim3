/*
 * JSONDataModel.h
 *
 *  Created on: Dec 27, 2019
 *      Author: cx3d
 */

#ifndef JSONDATAMODEL_H_
#define JSONDATAMODEL_H_

#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include "rapidjson/document.h"

using namespace std;
using namespace rapidjson;

template<class T> class JSONArray;

template<class T> class JSONComparable {
public:
	virtual ~JSONComparable() {

	}

	virtual bool operator==(T& o) = 0;
};

class JSONInstance {
protected:
	bool bIsNull;
	FILE* _fpin;
public:
	JSONInstance();

	virtual ~JSONInstance();

	bool isNull();

	bool exists();

	virtual void load(Value& v) = 0;

	virtual void toString(ostream& os) const = 0;
};

class JSONObject: public JSONInstance {
protected:
	map<string, shared_ptr<JSONInstance>> contents;
	map<string, bool> required;
	bool bHasParseError;
	int errorOffset;
	ParseErrorCode parseError;
public:
	void load(Value& v);

	bool HasParseError();

	size_t GetErrorOffset();

	ParseErrorCode GetParseError();

	void toString(ostream& os) const;
};

class JSONRootObject: public JSONObject{
private:
	Document _d;
public:
	void load(const char* path);

	Document* getDocument();
};

class JSONString: public JSONInstance {
protected:
	string val;
public:
	JSONString();

	JSONString(string val);

	operator string();

	void load(Value& v);

	void toString(ostream& os) const;

};

class JSONInteger: public JSONInstance {
protected:
	int val;
public:
	JSONInteger();

	JSONInteger(int val);

	operator int&();

	void load(Value& v);

	void toString(ostream& os) const;

};

class JSONNumber: public JSONInstance {
protected:
	double val;
public:
	JSONNumber();

	JSONNumber(double val);

	operator double&();

	void load(Value& v);

	void toString(ostream& os) const;
};

class JSONBoolean: public JSONInstance {
protected:
	bool val;
public:
	JSONBoolean();

	JSONBoolean(bool val);

	operator bool&();

	void load(Value& v);

	void toString(ostream& os) const;

};

template<class T> class JSONArray: public JSONInstance {
protected:
	vector<shared_ptr<T>> payload;
public:
	class JSONArrayIterator {
	protected:
		int idx;
		const vector<shared_ptr<T>>* payload;
	public:
		JSONArrayIterator(int idx, const vector<shared_ptr<T>>* payload) {
			this->idx = idx;
			this->payload = payload;
		}

		bool operator!=(const JSONArrayIterator& o) const {
			return this->idx != o.idx;
		}

		const JSONArrayIterator& operator++() {
			++this->idx;
			return *this;
		}

		T& operator*() const {
			shared_ptr<T> o = (*payload)[idx];
			T* ptr = o.get();
			return *ptr;
		}
	};

	JSONArrayIterator begin() const {
		return JSONArrayIterator(0, &payload);
	}

	JSONArrayIterator end() const {
		return JSONArrayIterator(payload.size(), &payload);
	}

	JSONArray() {
		bIsNull = true;
	}

	T& operator[](int idx) {
		return *payload[idx];
	}

	T& operator()(int idx) {
		return *payload[idx];
	}

	int size() {
		return payload.size();
	}

	void load(Value& v) {
		Value::Array array = v.GetArray();
		int sz = array.Size();
		for (int i = 0; i < sz; i++) {
			payload.push_back(make_shared<T>());
			shared_ptr<T>& item = payload.back();
			item->load(array[i]);
			if (bIsNull)
				bIsNull = false;
		}
	}

	void toString(ostream& os) const {
		bool printComma = false;
		os << "[";
		auto _begin = begin();
		auto _end = end();
		auto iter = _begin;
		while (iter != _end) {
			if (printComma)
				os << ", ";
			else
				printComma = true;
			(*iter).toString(os);
			++iter;
		}
		os << "]";
	}
};

ostream& operator<<(ostream& os, const JSONInstance& obj);
#endif /* JSONDATAMODEL_H_ */
