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
