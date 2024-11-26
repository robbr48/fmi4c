#ifndef FMIC_UTILS_H
#define FMIC_UTILS_H

#include <stdbool.h>
#include <stdint.h>
#include "ezxml/ezxml.h"
#include "fmi4c_private.h"

void rememberPointer(fmiHandle *fmu, void* ptr);
void* mallocAndRememberPointer(fmiHandle *fmu, size_t size);
void* reallocAndRememberPointer(fmiHandle *fmu, void* org, size_t size);
char* duplicateAndRememberString(fmiHandle *fmu, const char* str);

const char* getFunctionName(const char* modelName, const char* functionName, char* concatBuffer);

int removeDirectoryRecursively(const char* rootDirPath, const char* expectedDirNamePrefix);

bool parseStringAttributeEzXmlAndRememberPointer(ezxml_t element, const char* attributeName, const char** target, fmiHandle *fmu);
bool parseStringAttributeEzXml(ezxml_t element, const char* attributeName, const char** target);
bool parseBooleanAttributeEzXml(ezxml_t element, const char* attributeName, bool* target);
bool parseFloat64AttributeEzXml(ezxml_t element, const char* attributeName, double* target);
bool parseFloat32AttributeEzXml(ezxml_t element, const char* attributeName, float *target);
bool parseInt64AttributeEzXml(ezxml_t element, const char* attributeName, int64_t* target);
bool parseInt32AttributeEzXml(ezxml_t element, const char* attributeName, int32_t* target);
bool parseInt16AttributeEzXml(ezxml_t element, const char *attributeName, int16_t* target);
bool parseInt8AttributeEzXml(ezxml_t element, const char *attributeName, int8_t* target);
bool parseUInt64AttributeEzXml(ezxml_t element, const char* attributeName, uint64_t *target);
bool parseUInt32AttributeEzXml(ezxml_t element, const char* attributeName, uint32_t* target);
bool parseUInt16AttributeEzXml(ezxml_t element, const char *attributeName, uint16_t* target);
bool parseUInt8AttributeEzXml(ezxml_t element, const char *attributeName, uint8_t *target);

bool parseModelStructureElement(fmiHandle *fmu, fmi3ModelStructureElement *output, ezxml_t *element);

#endif // FMIC_UTILS_H
