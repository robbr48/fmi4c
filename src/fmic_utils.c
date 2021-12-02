#include <stdlib.h>
#include <string.h>
#include "fmic_utils.h"

char* parseStringAttribute(xmlAttr *attr) {
    return strdup(xmlNodeListGetString(attr->doc, attr->children, 1));
}

bool parseBooleanAttribute(xmlAttr *attr)
{
    return !strcmp(xmlNodeListGetString(attr->doc, attr->children, 1), "true");
}

double parseDoubleAttribute(xmlAttr *attr)
{
    return atof(xmlNodeListGetString(attr->doc, attr->children, 1));
}

int parseIntegerAttribute(xmlAttr *attr)
{
    return atoi(xmlNodeListGetString(attr->doc, attr->children, 1));
}

int parseLongAttribute(xmlAttr *attr)
{
    return atol(xmlNodeListGetString(attr->doc, attr->children, 1));
}


//! @brief Concatenates model name and function name into "modelName_functionName" (for FMI 1)
//! @param modelName FMU model name
//! @param functionName Function name
//! @returns Full (concatenated) function name
const char* getFunctionName(const char* modelName, const char* functionName) {
    if(modelName == NULL || modelName[0] == '\0') {
        return functionName;    //!< Do not change function name if model name is empty
    }
    char* fullName = (char*)malloc(strlen(modelName)+strlen(functionName)+2);
    strcpy(fullName, modelName);
    strcat(fullName, "_");
    strcat(fullName, functionName);
    printf("Function name: %s\n",fullName); //!< @todo Remove debug output
    return fullName;
}
