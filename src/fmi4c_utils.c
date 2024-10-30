#include "fmi4c_utils.h"
#include "fmi4c_common.h"

#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
#include "dirent/dirent.h"
#else
#include <dirent.h>
#include <sys/stat.h>
#endif
#ifndef _WIN32
#include <unistd.h>
#endif

//! @brief Concatenates model name and function name into "modelName_functionName" (for FMI 1)
//! @param modelName FMU model name
//! @param functionName Function name
//! @param concatBuffer A buffer in which to concatenate modelName_functionName, this buffer must be FILENAME_MAX big
//! @returns Full (concatenated) function name
const char* getFunctionName(const char* modelName, const char* functionName, char* concatBuffer) {
    if(modelName == NULL || strlen(modelName) == 0) {
        // Do not change function name if model name is empty, in this case there is no need to use the buffer
        return functionName;
    }
    strncpy(concatBuffer, modelName, FILENAME_MAX-1);
    strncat(concatBuffer,  "_", FILENAME_MAX-strlen(concatBuffer)-1);
    strncat(concatBuffer, functionName, FILENAME_MAX-strlen(concatBuffer)-1);
    return concatBuffer;
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

        if(dependencies == NULL || dependencies[0] == '\0') {
            //If dependencies is empty, no need to parse further
            return true;
        }

        //Duplicate the dependencies string to make it mutable
        char* nonConstDependencies = _strdup(dependencies);
        free((char*)dependencies);

        if (nonConstDependencies == NULL) {
            return false; //strdup failed, handle as an error
        }

        //Count the number of dependencies based on space-delimited tokens
        output->numberOfDependencies = 1;
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

//! @brief Remove a directory (including all files and sub directories)
//! @param rootDirPath The path to the directory to remove
//! @param expectedDirNamePrefix Optional directory name prefix to avoid removing unintended root dir. Set to Null to ignore.
//! @returns 0 if removed OK else a system error code or -1
int removeDirectoryRecursively(const char* rootDirPath, const char *expectedDirNamePrefix) {
    // If expectedDirNamePrefix is set, ensure that the name of the directory being removed starts with this prefix
    // This is just an optional sanity check to prevent unexpected removal of the wrong directory
    if (expectedDirNamePrefix != NULL) {
        int dsp; // Dir separator position
        for (dsp=(int)strlen(rootDirPath); dsp>-1; dsp--) {
            if ( rootDirPath[dsp] == '/' || rootDirPath[dsp] == '\\' ) {
                break;
            }
        }
        dsp++; // Advance to first char after separator (or first char if no separator found)

        if (strncmp(expectedDirNamePrefix, rootDirPath+dsp, strlen(expectedDirNamePrefix)) != 0) {
            printf("Directory name prefix '%s' mismatch, refusing to remove directory '%s'\n", expectedDirNamePrefix, rootDirPath);
            return 1;
        }
    }

    DIR* dir = opendir(rootDirPath);
    int rc = -1;
    if (dir) {
        struct dirent *entry = readdir(dir);
        while(entry) {
            // Avoid recursing upwards
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
                entry = readdir(dir);
                continue;
            }

            // Determine full path to file or directory for current entry
            size_t fullPathLength = strlen(rootDirPath)+ strlen(entry->d_name) + 2;
            char* fullPath = malloc(fullPathLength);
            snprintf(fullPath, fullPathLength, "%s/%s", rootDirPath, entry->d_name);

            // Figure out if entry represents a directory or not
            bool entryIsDir;
            struct stat statbuf;
#ifdef _DIRENT_HAVE_D_TYPE
            entryIsDir = (entry->d_type == DT_DIR);
            // On some filesystems d_type is not set, then use stat to check the type
            if (entry->d_type == DT_UNKNOWN) {
                if (!stat(fullPath, &statbuf)) {
                    entryIsDir = S_ISDIR(statbuf.st_mode);
                }
            }
#else
            // On some systems d_type is note present, in which case stats is used
            if (!stat(fullPath, &statbuf)) {
                entryIsDir = S_ISDIR(statbuf.st_mode);
            }
#endif
            // Recurse on directory, else unlink file
            if (entryIsDir) {
                rc = removeDirectoryRecursively(fullPath, NULL);
            }
            else {
                // --- For debug ---
                //printf("Debug: Would unlink: %s\n", fullPath);
                //rc = 0;
                // -----------------
#ifdef _WIN32
                rc = _unlink(fullPath);
#else
                rc = unlink(fullPath);
#endif
                if (rc != 0) {
                    perror("Error");
                    printf("Could not remove '%s'\n", fullPath);
                }
            }
            free(fullPath);
            if (rc != 0) {
                break;
            }
            entry = readdir(dir);
        }
        closedir(dir);
        if (rc == 0) {
            // --- For debug ---
            //printf("Debug: Would rmdir: %s\n", rootDirPath);
            //rc = 0;
            // -----------------
#ifdef _WIN32
            rc = RemoveDirectoryA(rootDirPath);
            // RemoveDirectoryA returns non-zero on success so we swap it so that rest of the code works as expected (return code 0 = OK)
            rc = (rc == 0) ? 1 : 0;
#else
            rc = rmdir(rootDirPath);
#endif
            if (rc != 0) {
                perror("Error");
            }
        }
        if (rc != 0) {
            printf("Could not remove '%s'\n", rootDirPath);
        }
    }
    else {
        perror("Error");
        printf("Could not access '%s' for removal\n", rootDirPath);
    }
    return rc;
}
