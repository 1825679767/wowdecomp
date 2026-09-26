#ifndef ENGINE_SOURCE_FRAMEXML_EXPAT_H
#define ENGINE_SOURCE_FRAMEXML_EXPAT_H

#include <cstddef>
#include <cstdint>

extern "C" {

struct XML_ParserStruct;

typedef XML_ParserStruct* XML_Parser;
typedef char XML_Char;
typedef unsigned char XML_Bool;

enum XML_Status {
    XML_STATUS_ERROR = 0,
    XML_STATUS_OK = 1,
};

typedef void (*XML_StartElementHandler)(void* userData, const XML_Char* name, const XML_Char** atts);
typedef void (*XML_EndElementHandler)(void* userData, const XML_Char* name);
typedef void (*XML_CharacterDataHandler)(void* userData, const XML_Char* s, int len);

XML_Parser XML_ParserCreate(const XML_Char* encoding);
void XML_ParserFree(XML_Parser parser);
XML_Bool XML_ParserReset(XML_Parser parser, const XML_Char* encoding);
void XML_SetUserData(XML_Parser parser, void* userData);
void XML_SetElementHandler(XML_Parser parser, XML_StartElementHandler start, XML_EndElementHandler end);
void XML_SetCharacterDataHandler(XML_Parser parser, XML_CharacterDataHandler handler);
XML_Status XML_Parse(XML_Parser parser, const char* s, int len, int isFinal);

}

#endif
