#pragma once

extern "C" {
    #include <lua.h>
}

typedef int (*LuaCFunc)(lua_State*);

union Value {
    void *genericVal;
    const char *stringVal;
    float numericVal;
    LuaCFunc funcVal;
};

enum EVENT_TYPE {
   NUMBER,
   STRING,
   CFUNCTION,
};

struct KeyValuePair {
    enum EVENT_TYPE type;
    const char *key;
    Value value;
};

KeyValuePair kvPairWithNumber(const char *key, float number);
KeyValuePair kvPairWithString(const char *key, const char *string);
KeyValuePair kvPairWithCFunction(const char *key, LuaCFunc function);

void luaDict(lua_State *L, const char *tableName, int kvCount, ...);
