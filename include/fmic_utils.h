#ifndef FMIC_UTILS_H
#define FMIC_UTILS_H

#include "tree.h"
#include <stdbool.h>

char* parseStringAttribute(xmlAttr *attr);
bool parseBooleanAttribute(xmlAttr *attr);
double parseDoubleAttribute(xmlAttr *attr);
int parseIntegerAttribute(xmlAttr *attr);
int parseLongAttribute(xmlAttr *attr);
const char* getFunctionName(const char* modelName, const char* functionName);

#endif // FMIC_UTILS_H
