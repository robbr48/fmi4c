#include <stdlib.h>
#include <string.h>
#include "fmi4c_utils.h"
#include "fmi4c_common.h"


//! @brief Concatenates model name and function name into "modelName_functionName" (for FMI 1)
//! @param modelName FMU model name
//! @param functionName Function name
//! @returns Full (concatenated) function name
const char* getFunctionName(const char* modelName, const char* functionName) {
    if(modelName == NULL || modelName[0] == '\0') {
        return functionName;    //!< Do not change function name if model name is empty
    }
    char* fullName = (char*)malloc(strlen(modelName)+strlen(functionName)+2);
    strncpy(fullName, modelName, strlen(modelName)+strlen(functionName)+2);
    fullName[strlen(modelName)] = '\0';
    strncat(fullName,  "_", strlen(modelName)+strlen(functionName)+2);
    strncat(fullName, functionName, strlen(modelName)+strlen(functionName)+2);
    return fullName;
}

//! @brief Parses specified XML attribute and assigns it to target
//! @param element XML element
//! @param attributeName Attribute name
//! @param target Pointer to target variable
//! @returns True if attribute was found, else false
bool parseStringAttributeEzXml(ezxml_t element, const char *attributeName, const char **target)
{
    if(ezxml_attr(element, attributeName)) {
        (*target) = _strdup(ezxml_attr(element, attributeName));
        return true;
    }
    return false;
}

//! @brief Parses specified XML attribute and assigns it to target
//! @param element XML element
//! @param attributeName Attribute name
//! @param target Pointer to target variable
//! @returns True if attribute was found, else false
bool parseBooleanAttributeEzXml(ezxml_t element, const char *attributeName, bool *target)
{
    if(ezxml_attr(element, attributeName)) {
        (*target) = !strcmp(ezxml_attr(element, attributeName), "true");
        return true;
    }
    return false;
}

//! @brief Parses specified XML attribute and assigns it to target
//! @param element XML element
//! @param attributeName Attribute name
//! @param target Pointer to target variable
//! @returns True if attribute was found, else false
bool parseFloat64AttributeEzXml(ezxml_t element, const char *attributeName, double *target)
{
    if(ezxml_attr(element, attributeName)) {
        (*target) = atof(ezxml_attr(element, attributeName));
        return true;
    }
    return false;
}

//! @brief Parses specified XML attribute and assigns it to target
//! @param element XML element
//! @param attributeName Attribute name
//! @param target Pointer to target variable
//! @returns True if attribute was found, else false
bool parseInt16AttributeEzXml(ezxml_t element, const char *attributeName, short *target)
{
    if(ezxml_attr(element, attributeName)) {
        (*target) = (short)atoi(ezxml_attr(element, attributeName));
        return true;
    }
    return false;
}

//! @brief Parses specified XML attribute and assigns it to target
//! @param element XML element
//! @param attributeName Attribute name
//! @param target Pointer to target variable
//! @returns True if attribute was found, else false
bool parseInt64AttributeEzXml(ezxml_t element, const char *attributeName, int64_t* target)
{
    if(ezxml_attr(element, attributeName)) {
        (*target) = atol(ezxml_attr(element, attributeName));
        return true;
    }
    return false;
}

//! @brief Parses specified XML attribute and assigns it to target
//! @param element XML element
//! @param attributeName Attribute name
//! @param target Pointer to target variable
//! @returns True if attribute was found, else false
bool parseInt32AttributeEzXml(ezxml_t element, const char *attributeName, int32_t *target)
{
    if(ezxml_attr(element, attributeName)) {
        (*target) = atoi(ezxml_attr(element, attributeName));
        return true;
    }
    return false;
}

//! @brief Parses specified XML attribute and assigns it to target
//! @param element XML element
//! @param attributeName Attribute name
//! @param target Pointer to target variable
//! @returns True if attribute was found, else false
bool parseFloat32AttributeEzXml(ezxml_t element, const char *attributeName, float *target)
{
    if(ezxml_attr(element, attributeName)) {
        (*target) = (float)atof(ezxml_attr(element, attributeName));
        return true;
    }
    return false;
}

//! @brief Parses specified XML attribute and assigns it to target
//! @param element XML element
//! @param attributeName Attribute name
//! @param target Pointer to target variable
//! @returns True if attribute was found, else false
bool parseInt8AttributeEzXml(ezxml_t element, const char *attributeName, int8_t *target)
{
    if(ezxml_attr(element, attributeName)) {
        (*target) = (int8_t)atoi(ezxml_attr(element, attributeName));
        return true;
    }
    return false;
}

//! @brief Parses specified XML attribute and assigns it to target
//! @param element XML element
//! @param attributeName Attribute name
//! @param target Pointer to target variable
//! @returns True if attribute was found, else false
bool parseUInt64AttributeEzXml(ezxml_t element, const char *attributeName, uint64_t *target)
{
    if(ezxml_attr(element, attributeName)) {
        (*target) = strtoul(ezxml_attr(element, attributeName), NULL, 10);
        return true;
    }
    return false;
}

//! @brief Parses specified XML attribute and assigns it to target
//! @param element XML element
//! @param attributeName Attribute name
//! @param target Pointer to target variable
//! @returns True if attribute was found, else false
bool parseUInt32AttributeEzXml(ezxml_t element, const char *attributeName, uint32_t *target)
{
    if(ezxml_attr(element, attributeName)) {
        (*target) = (uint32_t)strtoul(ezxml_attr(element, attributeName), NULL, 10);
        return true;
    }
    return false;
}

//! @brief Parses specified XML attribute and assigns it to target
//! @param element XML element
//! @param attributeName Attribute name
//! @param target Pointer to target variable
//! @returns True if attribute was found, else false
bool parseUInt16AttributeEzXml(ezxml_t element, const char *attributeName, uint16_t *target)
{
    if(ezxml_attr(element, attributeName)) {
        (*target) = (uint16_t)strtoul(ezxml_attr(element, attributeName), NULL, 10);
        return true;
    }
    return false;
}

//! @brief Parses specified XML attribute and assigns it to target
//! @param element XML element
//! @param attributeName Attribute name
//! @param target Pointer to target variable
//! @returns True if attribute was found, else false
bool parseUInt8AttributeEzXml(ezxml_t element, const char *attributeName, uint8_t *target)
{
    if(ezxml_attr(element, attributeName)) {
        (*target) = (uint8_t)strtoul(ezxml_attr(element, attributeName), NULL, 10);
        return true;
    }
    return false;
}

bool parseModelStructureElement(fmi3ModelStructureElement *output, ezxml_t *element)
{
    //Count number of dependencies
    output->numberOfDependencies = 0;
    const char* dependencies = NULL;
    if(parseStringAttributeEzXml(*element, "dependencies", &dependencies)) {
        char* nonConstDependencies = _strdup(dependencies);
        free((char*)dependencies);

        //Count number of dependencies
        if(nonConstDependencies != NULL) {
            output->numberOfDependencies = 1;
        }
        else {
            return true;
        }
        for(int i=0; nonConstDependencies[i]; ++i) {
            if(nonConstDependencies[i] == ' ') {
                ++output->numberOfDependencies;
            }
        }


        //Allocate memory for dependencies
        output->dependencies = malloc(output->numberOfDependencies*sizeof(fmi3ValueReference));

        //Read dependencies
        const char* delim = " ";
        for(int j=0; j<output->numberOfDependencies; ++j) {
            if(j == 0) {
                output->dependencies[j] = atoi(strtok(nonConstDependencies, delim));
            }
            else {
                output->dependencies[j] =  atoi(strtok(NULL, delim));
            }
        }

        //Parse depenendency kinds element if present
        const char* dependencyKinds = NULL;
        parseStringAttributeEzXml(*element, "dependencyKinds", &dependencyKinds);
        if(dependencyKinds) {
            char* nonConstDependencyKinds = _strdup(dependencyKinds);
            free((char*)dependencyKinds);

            //Allocate memory for dependencies (assume same number as dependencies, according to FMI3 specification)
            output->dependencies = malloc(output->numberOfDependencies*sizeof(fmi3ValueReference));

            //Read dependency kinds
            for(int j=0; j<output->numberOfDependencies; ++j) {
                const char* kind;
                if(j == 0) {
                    kind = strtok(nonConstDependencyKinds, delim);
                }
                else {
                    kind = strtok(NULL, delim);
                }

                if(!strcmp(kind, "independent")) {
                    fmi4cErrorMessage = _strdup("Dependency kind = \"independent\" is not allowed for output dependencies.");
                    free(nonConstDependencyKinds);
                    free(nonConstDependencies);
                    return false;
                }
                else if(!strcmp(kind, "constant")) {
                    output->dependencyKinds[j] = fmi3Constant;
                }
                else if(!strcmp(kind, "fixed")) {
                    output->dependencyKinds[j] = fmi3Fixed;
                }
                else if(!strcmp(kind, "tunable")) {
                    output->dependencyKinds[j] = fmi3Tunable;
                }
                else if(!strcmp(kind, "discrete")) {
                    output->dependencyKinds[j] = fmi3Discrete;
                }
                else if(!strcmp(kind, "dependent")) {
                    output->dependencyKinds[j] = fmi3Dependent;
                }
                else {
                    fmi4cErrorMessage = _strdup("Unknown dependency kind for output dependency.");
                    free(nonConstDependencyKinds);
                    free(nonConstDependencies);
                    return false;
                }
            }
            free(nonConstDependencyKinds);
        }
        free(nonConstDependencies);
    }

    return true;
}
