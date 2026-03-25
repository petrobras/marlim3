/*
 * JSONDataModel.cpp
 *
 *  Created on: Dec 27, 2019
 *      Author: cx3d
 */

#include "JSONDataModel.h"
#include "rapidjson/filereadstream.h"

JSONInstance::JSONInstance() {
	;
	bIsNull = true;
	_fpin = NULL;
}

JSONInstance::~JSONInstance() {
	if (_fpin)
		fclose(_fpin);
}

bool JSONInstance::isNull() {
	return bIsNull;
}

bool JSONInstance::exists() {
	return !bIsNull;
}

bool JSONObject::HasParseError() {
	return this->bHasParseError;
}

size_t JSONObject::GetErrorOffset() {
	return this->errorOffset;
}

ParseErrorCode JSONObject::GetParseError() {
	return this->parseError;
}

void JSONObject::load(Value& v) {
	map<string, shared_ptr<JSONInstance>>::iterator iter = contents.begin();
	map<string, shared_ptr<JSONInstance>>::iterator end = contents.end();
	for (; iter != end; iter++) {
		string key = iter->first;
		shared_ptr<JSONInstance>& o = iter->second;
		bool exists = v.HasMember(key.c_str());
		bool isRequired = required[key];
		if (!exists && isRequired) {
			cout << "Erro!!!" << endl;

		} else if (exists) {
			Value& nv = v[key.c_str()];
			o->load(nv);
			if (bIsNull)
				bIsNull = false;
		}
	}
}

void JSONObject::toString(ostream& os) const {
	bool printComma = false;
	os << "{" << endl;
	map<string, shared_ptr<JSONInstance>>::const_iterator iter =
			contents.begin();
	map<string, shared_ptr<JSONInstance>>::const_iterator end = contents.end();
	for (; iter != end; iter++) {
		string key = iter->first;
		const shared_ptr<JSONInstance>& o = iter->second;
		if (!o->isNull()) {
			if (printComma)
				os << ", ";
			else
				printComma = true;
			os << "\"" << key << "\": ";
			o->toString(os);
		}
	}
	os << endl << "}" << endl;
}

void JSONRootObject::load(const char* path) {
	JSONObject* o = dynamic_cast<JSONObject*>(this);
	_fpin = fopen(path, "r");
	char _inputbuffer[128];

	FileReadStream _fin(_fpin, _inputbuffer, 128);
	_d.ParseStream(_fin);

	bHasParseError = _d.HasParseError();

	if (!bHasParseError) {
		o->load(_d);
	} else {
		errorOffset = _d.GetErrorOffset();
		parseError = _d.GetParseError();
	}
}

Document* JSONRootObject::getDocument(){
	return &_d;
}

JSONString::JSONString() {
	bIsNull = true;
}

JSONString::JSONString(string val) {
	this->val = val;
	bIsNull = false;
}

void JSONString::load(Value& v) {
	val = v.GetString();
	bIsNull = false;
}

JSONString::operator string() {
	return val;
}

void JSONString::toString(ostream& os) const {
	os << "\"" << val << "\"";
}

JSONInteger::JSONInteger() :
		val(0) {
	bIsNull = true;
}

JSONInteger::JSONInteger(int val) {
	this->val = val;
	bIsNull = false;
}

void JSONInteger::load(Value& v) {
	val = v.GetInt();
	bIsNull = false;
}

JSONInteger::operator int&() {
	return val;
}

void JSONInteger::toString(ostream& os) const {
	os << val;
}

JSONNumber::JSONNumber() :
		val(0) {
	bIsNull = true;

}

JSONNumber::JSONNumber(double val) {
	this->val = val;
	bIsNull = false;
}

void JSONNumber::load(Value& v) {
	val = v.GetDouble();
	bIsNull = false;
}

JSONNumber::operator double&() {
	return val;
}

void JSONNumber::toString(ostream& os) const {
	os << val;
}

JSONBoolean::JSONBoolean() :
		val(false) {
	bIsNull = true;
}

JSONBoolean::JSONBoolean(bool val) {
	this->val = val;
	bIsNull = false;
}

void JSONBoolean::load(Value& v) {
	val = v.GetBool();
	bIsNull = false;
}

JSONBoolean::operator bool&() {
	return val;
}

void JSONBoolean::toString(ostream& os) const {
	os << (val ? "true" : "false");
}

ostream& operator<<(ostream& os, const JSONInstance& obj) {
	obj.toString(os);
	return os;
}

