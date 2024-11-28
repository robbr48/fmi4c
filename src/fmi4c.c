#include "fmi4c_private.h"
#define FMI4C_H_INTERNAL_INCLUDE
#include "fmi4c.h"
#include "fmi4c_placeholders.h"
#include "fmi4c_utils.h"

#ifdef FMI4C_WITH_MINIZIP
#include "miniunz.h"
#endif
#include "ezxml/ezxml.h"

#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <ctype.h>
#ifdef _WIN32
#include <process.h>
#endif
#ifndef _WIN32
#include "fmi4c_common.h"
#include <dlfcn.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

#if defined(__x86_64__) || defined(_M_X64)
    #define arch_str "x86_64"
    #define bits_str "64"
#else
    #define arch_str "x86"
    #define bits_str "32"
#endif

#if defined(__CYGWIN__)
    #define dirsep_str "/"
#elif defined(_WIN32)
    #define dirsep_str "\\"
#else
    #define dirsep_str "/"
#endif

#if defined(__CYGWIN__) || defined(_WIN32)
    #define dllext_str ".dll"
    #define fmi12_system_str "win"
    #define fmi3_system_str "windows"
#else
    #define dllext_str ".so"
    #define fmi12_system_str "linux"
    #define fmi3_system_str "linux"
#endif

#ifdef _WIN32
FARPROC loadDllFunction(HINSTANCE dll, const char *name, bool *ok) {
    FARPROC fnc = GetProcAddress(dll, name);
    if(fnc == NULL) {
        printf("Failed to load function \"%s\"\n", name);
        (*ok) = false;
    }
    return fnc;
}
#else
void *loadDllFunction(void *dll, const char *name, bool *ok) {
    void* fnc = dlsym(dll, name);
    if(fnc == NULL) {
        printf("Failed to load function \"%s\"\n", name);
        (*ok) = false;
    }
    return fnc;
}
#endif

const char* fmi4cErrorMessage = "";

const char* fmi4c_getErrorMessages()
{
    return fmi4cErrorMessage;
}

void freeDuplicatedConstChar(const char* ptr) {
  if(ptr != NULL) {
      free((void*)ptr);
  }
}

//! @brief Parses modelDescription.xml for FMI 1
//! @param fmu FMU handle
//! @returns True if parsing was successful
bool parseModelDescriptionFmi1(fmiHandle *fmu)
{
    fmu->fmi1.modelName = NULL;
    fmu->fmi1.modelIdentifier = NULL;
    fmu->fmi1.guid = NULL;
    fmu->fmi1.description = NULL;
    fmu->fmi1.author = NULL;
    fmu->fmi1.version = NULL;
    fmu->fmi1.generationTool = NULL;
    fmu->fmi1.generationDateAndTime = NULL;
    fmu->fmi1.variableNamingConvention = NULL;

    fmu->fmi1.defaultStartTimeDefined = false;
    fmu->fmi1.defaultStopTimeDefined = false;
    fmu->fmi1.defaultToleranceDefined = false;

    fmu->fmi1.canHandleVariableCommunicationStepSize = false;
    fmu->fmi1.canInterpolateInputs = false;
    fmu->fmi1.canRunAsynchronuously = false;
    fmu->fmi1.canBeInstantiatedOnlyOncePerProcess = false;
    fmu->fmi1.canNotUseMemoryManagementFunctions = false;
    fmu->fmi1.maxOutputDerivativeOrder = 0;

    fmu->fmi1.hasRealVariables = false;
    fmu->fmi1.hasIntegerVariables = false;
    fmu->fmi1.hasStringVariables = false;
    fmu->fmi1.hasBooleanVariables = false;

    fmu->fmi1.type = fmi1ModelExchange;

    char cwd[FILENAME_MAX];
#ifdef _WIN32
    _getcwd(cwd, sizeof(char)*FILENAME_MAX);
#else
    getcwd(cwd, sizeof(char)*FILENAME_MAX);
#endif
    chdir(fmu->unzippedLocation);

    ezxml_t rootElement = ezxml_parse_file("modelDescription.xml");
    if(strcmp(rootElement->name, "fmiModelDescription")) {
        printf("Wrong root tag name: %s\n", rootElement->name);
        return false;
    }

    //Parse attributes in <fmiModelDescription>
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "modelName",                 &fmu->fmi1.modelName,                  fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "modelIdentifier",           &fmu->fmi1.modelIdentifier,            fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "guid",                      &fmu->fmi1.guid,                       fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "description",               &fmu->fmi1.description,                fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "author",                    &fmu->fmi1.author,                     fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "version",                   &fmu->fmi1.version,                    fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "generationTool",            &fmu->fmi1.generationTool,             fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "generationDateAndTime",     &fmu->fmi1.generationDateAndTime,      fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "variableNamingConvention",  &fmu->fmi1.variableNamingConvention,   fmu);
    parseInt32AttributeEzXml(rootElement, "numberOfContinuousStates",   &fmu->fmi1.numberOfContinuousStates);
    parseInt32AttributeEzXml(rootElement, "numberOfEventIndicators",    &fmu->fmi1.numberOfEventIndicators);

    ezxml_t implementationElement = ezxml_child(rootElement, "Implementation");
    if(implementationElement) {
        ezxml_t capabilitiesElement = NULL;
        ezxml_t cosimToolElement = ezxml_child(implementationElement, "CoSimulation_Tool");
        if(cosimToolElement) {
            fmu->fmi1.type = fmi1CoSimulationTool;
            capabilitiesElement = ezxml_child(cosimToolElement, "Capabilities");
        }
        ezxml_t cosimStandAloneElement = ezxml_child(implementationElement, "CoSimulation_StandAlone");
        if(cosimStandAloneElement) {
            fmu->fmi1.type = fmi1CoSimulationStandAlone;
            capabilitiesElement = ezxml_child(cosimStandAloneElement, "Capabilities");
        }
        if(capabilitiesElement) {
            parseBooleanAttributeEzXml(capabilitiesElement, "canHandleVariableCommunicationStepSize",   &fmu->fmi1.canHandleVariableCommunicationStepSize);
            parseBooleanAttributeEzXml(capabilitiesElement, "canHandleEvents",                          &fmu->fmi1.canHandleEvents);
            parseBooleanAttributeEzXml(capabilitiesElement, "canRejectSteps",                           &fmu->fmi1.canRejectSteps);
            parseBooleanAttributeEzXml(capabilitiesElement, "canInterpolateInputs",                     &fmu->fmi1.canInterpolateInputs);
            parseInt32AttributeEzXml(capabilitiesElement, "maxOutputDerivativeOrder",                   &fmu->fmi1.maxOutputDerivativeOrder);
            parseBooleanAttributeEzXml(capabilitiesElement, "canRunAsynchronuously",                    &fmu->fmi1.canRunAsynchronuously);
            parseBooleanAttributeEzXml(capabilitiesElement, "canSignalEvents",                          &fmu->fmi1.canSignalEvents);
            parseBooleanAttributeEzXml(capabilitiesElement, "canBeInstantiatedOnlyOncePerProcess",      &fmu->fmi1.canBeInstantiatedOnlyOncePerProcess);
            parseBooleanAttributeEzXml(capabilitiesElement, "canNotUseMemoryManagementFunctions",       &fmu->fmi1.canNotUseMemoryManagementFunctions);
        }
    }

    ezxml_t unitDefinitionsElement = ezxml_child(rootElement, "UnitDefinitions");
    if(unitDefinitionsElement) {
        //First count number of units
        fmu->fmi1.numberOfBaseUnits = 0;
        for(ezxml_t baseUnitElement = unitDefinitionsElement->child; baseUnitElement; baseUnitElement = baseUnitElement->ordered) {
            if(!strcmp(baseUnitElement->name, "BaseUnit")) {
                ++fmu->fmi1.numberOfBaseUnits;
            }
        }
        if(fmu->fmi1.numberOfBaseUnits > 0) {
            fmu->fmi1.baseUnits = mallocAndRememberPointer(fmu, fmu->fmi1.numberOfBaseUnits*sizeof(fmi1BaseUnitHandle));
        }
        int i=0;
        for(ezxml_t baseUnitElement = unitDefinitionsElement->child; baseUnitElement; baseUnitElement = baseUnitElement->ordered) {
            if(strcmp(baseUnitElement->name, "BaseUnit")) {
                continue;   //Wrong element name
            }
            fmi1BaseUnitHandle baseUnit;
            baseUnit.unit = NULL;
            baseUnit.displayUnits = NULL;
            parseStringAttributeEzXmlAndRememberPointer(baseUnitElement, "unit", &baseUnit.unit, fmu);
            baseUnit.numberOfDisplayUnits = 0;
            for(ezxml_t unitSubElement = baseUnitElement->child; unitSubElement; unitSubElement = unitSubElement->ordered) {
                if(!strcmp(unitSubElement->name, "DisplayUnitDefinition")) {
                    ++baseUnit.numberOfDisplayUnits;  //Just count them for now, so we can allocate memory before loading them
                }
            }
            if(baseUnit.numberOfDisplayUnits > 0) {
                baseUnit.displayUnits = mallocAndRememberPointer(fmu, baseUnit.numberOfDisplayUnits*sizeof(fmi1DisplayUnitHandle));
            }
            int j=0;
            for(ezxml_t unitSubElement = baseUnitElement->child; unitSubElement; unitSubElement = unitSubElement->ordered) {
                if(!strcmp(unitSubElement->name, "DisplayUnitDefinition")) {
                    baseUnit.displayUnits[j].gain = 1;
                    baseUnit.displayUnits[j].offset = 0;
                    parseStringAttributeEzXmlAndRememberPointer(unitSubElement,  "displayUnit",      &baseUnit.displayUnits[j].displayUnit, fmu);
                    parseFloat64AttributeEzXml(unitSubElement, "factor",    &baseUnit.displayUnits[j].gain);
                    parseFloat64AttributeEzXml(unitSubElement, "offset",    &baseUnit.displayUnits[j].offset);
                }
                ++j;
            }
            fmu->fmi1.baseUnits[i] = baseUnit;
            ++i;
        }
    }

    ezxml_t defaultExperimentElement = ezxml_child(rootElement, "DefaultExperiment");
    if(defaultExperimentElement) {
        fmu->fmi1.defaultStartTimeDefined = parseFloat64AttributeEzXml(defaultExperimentElement, "startTime", &fmu->fmi1.defaultStartTime);
        fmu->fmi1.defaultStopTimeDefined =  parseFloat64AttributeEzXml(defaultExperimentElement, "stopTime",  &fmu->fmi1.defaultStopTime);
        fmu->fmi1.defaultToleranceDefined = parseFloat64AttributeEzXml(defaultExperimentElement, "tolerance", &fmu->fmi1.defaultTolerance);
    }

    ezxml_t modelVariablesElement = ezxml_child(rootElement, "ModelVariables");
    if(modelVariablesElement) {
        for(ezxml_t varElement = ezxml_child(modelVariablesElement, "ScalarVariable"); varElement; varElement = varElement->next) {

            fmi1VariableHandle var;
            var.name = NULL;
            var.description = NULL;
            var.quantity = NULL;
            var.unit = NULL;
            var.displayUnit = NULL;
            var.relativeQuantity = false;
            var.min = -DBL_MAX;
            var.max = DBL_MAX;
            var.nominal = 1;
            var.startReal = 0;
            var.startInteger = 0;
            var.startBoolean = 0;
            var.startString = "";

            parseStringAttributeEzXmlAndRememberPointer(varElement, "name", &var.name, fmu);
            parseInt64AttributeEzXml(varElement, "valueReference", &var.valueReference);
            parseStringAttributeEzXmlAndRememberPointer(varElement, "description", &var.description, fmu);

            var.causality = fmi1CausalityInternal;
            const char* causality = NULL;
            if(parseStringAttributeEzXml(varElement, "causality", &causality)) {
                if(!strcmp(causality, "input")) {
                    var.causality = fmi1CausalityInput;
                }
                else if(!strcmp(causality, "output")) {
                    var.causality = fmi1CausalityOutput;
                }
                else if(!strcmp(causality, "internal")) {
                    var.causality = fmi1CausalityInternal;
                }
                else if(!strcmp(causality, "none")) {
                    var.causality = fmi1CausalityNone;
                }
                else {
                    printf("Unknown causality: %s\n", causality);
                    freeDuplicatedConstChar(causality);
                    return false;
                }
                freeDuplicatedConstChar(causality);
            }

            var.variability = fmi1VariabilityContinuous;
            const char* variability = NULL;
            if(parseStringAttributeEzXml(varElement, "variability", &variability)) {
                if(!strcmp(variability, "parameter")) {
                    var.variability = fmi1VariabilityParameter;
                }
                else if(!strcmp(variability, "constant")) {
                    var.variability = fmi1VariabilityConstant;
                }
                else if(!strcmp(variability, "discrete")) {
                    var.variability = fmi1VariabilityDiscrete;
                }
                else if(!strcmp(variability, "continuous")) {
                    var.variability = fmi1VariabilityContinuous;
                }
                else {
                    printf("Unknown variability: %s\n", variability);
                    freeDuplicatedConstChar(variability);
                    return false;
                }
                freeDuplicatedConstChar(variability);
            }

            var.alias = fmi1AliasNoAlias;
            const char* alias = NULL;
            if(parseStringAttributeEzXml(varElement, "alias", &alias)) {
                if(!strcmp(alias, "alias")) {
                    var.alias = fmi1AliasAlias;
                }
                else if(!strcmp(alias, "negatedAlias")) {
                    var.alias = fmi1AliasNegatedAlias;
                }
                else if(!strcmp(alias, "noAlias")) {
                    var.alias = fmi1AliasNoAlias;
                }
                else {
                    printf("Unknown alias: %s\n", alias);
                    freeDuplicatedConstChar(alias);
                    return false;
                }
                freeDuplicatedConstChar(alias);
            }

            var.hasStartValue = false;
            ezxml_t realElement = ezxml_child(varElement, "Real");
            if(realElement) {
                fmu->fmi1.hasRealVariables = true;
                var.datatype = fmi1DataTypeReal;
                if(parseFloat64AttributeEzXml(realElement, "start", &var.startReal)) {
                    var.hasStartValue = true;
                }
                parseBooleanAttributeEzXml(realElement, "fixed", &var.fixed);
                parseStringAttributeEzXmlAndRememberPointer(realElement, "quantity", &var.quantity, fmu);
                parseStringAttributeEzXmlAndRememberPointer(realElement, "unit", &var.unit, fmu);
                parseStringAttributeEzXmlAndRememberPointer(realElement, "displayUnit", &var.displayUnit, fmu);
                parseBooleanAttributeEzXml(realElement, "relativeQuantity", &var.relativeQuantity);
                parseFloat64AttributeEzXml(realElement, "min", &var.min);
                parseFloat64AttributeEzXml(realElement, "max", &var.max);
                parseFloat64AttributeEzXml(realElement, "nominal", &var.nominal);
            }

            ezxml_t integerElement = ezxml_child(varElement, "Integer");
            if(integerElement) {
                fmu->fmi1.hasIntegerVariables = true;
                var.datatype = fmi1DataTypeInteger;
                if(parseInt32AttributeEzXml(integerElement, "start", &var.startInteger)) {
                    var.hasStartValue = true;
                }
                parseBooleanAttributeEzXml(integerElement, "fixed", &var.fixed);
            }

            ezxml_t booleanElement = ezxml_child(varElement, "Boolean");
            if(booleanElement) {
                fmu->fmi1.hasBooleanVariables = true;
                var.datatype = fmi1DataTypeBoolean;
                bool startBoolean;
                if(parseBooleanAttributeEzXml(booleanElement, "start", &startBoolean)) {
                    var.startBoolean = startBoolean;
                    var.hasStartValue = true;
                }
                parseBooleanAttributeEzXml(booleanElement, "fixed", &var.fixed);
            }

            ezxml_t stringElement = ezxml_child(varElement, "String");
            if(stringElement) {
                fmu->fmi1.hasStringVariables = true;
                var.datatype = fmi1DataTypeString;
                if(parseStringAttributeEzXmlAndRememberPointer(stringElement, "start", &var.startString, fmu)) {
                    var.hasStartValue = true;
                }
                parseBooleanAttributeEzXml(stringElement, "fixed", &var.fixed);
            }

            if(fmu->fmi1.numberOfVariables >= fmu->fmi1.variablesSize) {
                fmu->fmi1.variablesSize *= 2;
                fmu->fmi1.variables = reallocAndRememberPointer(fmu, fmu->fmi1.variables, fmu->fmi1.variablesSize*sizeof(fmi1VariableHandle));
            }

            fmu->fmi1.variables[fmu->fmi1.numberOfVariables] = var;
            fmu->fmi1.numberOfVariables++;
        }
    }

    ezxml_free(rootElement);

    chdir(cwd);

    return true;
}


// table according to fmi-specification 2.2 page 51
fmi2Initial initialDefaultTable[5][6] = {
    /*              parameter                  calculated parameter,        input                     output                 local                  independent */
    /* constant */  {fmi2InitialUnknown,       fmi2InitialUnknown,          fmi2InitialUnknown,       fmi2InitialExact,      fmi2InitialExact,      fmi2InitialUnknown},
    /* fixed   */   {fmi2InitialExact,         fmi2InitialCalculated,       fmi2InitialUnknown,       fmi2InitialUnknown,    fmi2InitialCalculated, fmi2InitialUnknown},
    /* tunable */   {fmi2InitialExact,         fmi2InitialCalculated,       fmi2InitialUnknown,       fmi2InitialUnknown,    fmi2InitialCalculated, fmi2InitialUnknown},
    /* discrete */  {fmi2InitialUnknown,       fmi2InitialUnknown,          fmi2InitialUnknown,       fmi2InitialCalculated, fmi2InitialCalculated, fmi2InitialUnknown},
    /* continuous */{fmi2InitialUnknown,       fmi2InitialUnknown,          fmi2InitialUnknown,       fmi2InitialCalculated, fmi2InitialCalculated, fmi2InitialUnknown}
};

// mapping index for variabilty and causality for the above table
fmi2Variability mapVariabilityIndex[5] = {fmi2VariabilityConstant,fmi2VariabilityFixed,fmi2VariabilityTunable,fmi2VariabilityDiscrete,fmi2VariabilityContinuous};
fmi2Causality mapCausalityIndex[6] = {fmi2CausalityParameter, fmi2CausalityCalculatedParameter, fmi2CausalityInput, fmi2CausalityOutput, fmi2CausalityLocal, fmi2CausalityIndependent};


//! @brief Parses modelDescription.xml for FMI 2
//! @param fmu FMU handle
//! @returns True if parsing was successful
bool parseModelDescriptionFmi2(fmiHandle *fmu)
{
    fmu->fmi2.fmiVersion_ = NULL;
    fmu->fmi2.modelName = NULL;
    fmu->fmi2.guid = NULL;
    fmu->fmi2.description = NULL;
    fmu->fmi2.author = NULL;
    fmu->fmi2.version = NULL;
    fmu->fmi2.copyright = NULL;
    fmu->fmi2.license = NULL;
    fmu->fmi2.generationTool = NULL;
    fmu->fmi2.generationDateAndTime = NULL;
    fmu->fmi2.variableNamingConvention = NULL;

    fmu->fmi2.cs.modelIdentifier = NULL;
    fmu->fmi2.cs.needsExecutionTool = false;
    fmu->fmi2.cs.canHandleVariableCommunicationStepSize = false;
    fmu->fmi2.cs.canInterpolateInputs = false;
    fmu->fmi2.cs.canRunAsynchronuously = false;
    fmu->fmi2.cs.maxOutputDerivativeOrder = 0;
    fmu->fmi2.cs.canBeInstantiatedOnlyOncePerProcess = false;
    fmu->fmi2.cs.canNotUseMemoryManagementFunctions = false;
    fmu->fmi2.cs.canGetAndSetFMUState = false;
    fmu->fmi2.cs.canSerializeFMUState = false;
    fmu->fmi2.cs.providesDirectionalDerivative = false;

    fmu->fmi2.me.modelIdentifier = NULL;
    fmu->fmi2.me.canBeInstantiatedOnlyOncePerProcess = false;
    fmu->fmi2.me.canNotUseMemoryManagementFunctions = false;
    fmu->fmi2.me.canGetAndSetFMUState = false;
    fmu->fmi2.me.canSerializeFMUState = false;
    fmu->fmi2.me.providesDirectionalDerivative = false;

    fmu->fmi2.defaultStartTimeDefined = false;
    fmu->fmi2.defaultStopTimeDefined = false;
    fmu->fmi2.defaultToleranceDefined = false;
    fmu->fmi2.defaultStepSizeDefined = false;

    fmu->fmi2.numberOfContinuousStates = 0;

    fmu->fmi2.supportsCoSimulation = false;
    fmu->fmi2.supportsModelExchange = false;

    fmu->fmi2.hasRealVariables = false;
    fmu->fmi2.hasIntegerVariables = false;
    fmu->fmi2.hasStringVariables = false;
    fmu->fmi2.hasBooleanVariables = false;


    char cwd[FILENAME_MAX];
#ifdef _WIN32
    _getcwd(cwd, sizeof(char)*FILENAME_MAX);
#else
    getcwd(cwd, sizeof(char)*FILENAME_MAX);
#endif
    chdir(fmu->unzippedLocation);

    ezxml_t rootElement = ezxml_parse_file("modelDescription.xml");
    if(strcmp(rootElement->name, "fmiModelDescription")) {
        printf("Wrong root tag name: %s\n", rootElement->name);
        return false;
    }

    //Parse attributes in <fmiModelDescription>
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "fmiVersion",                &fmu->fmi2.fmiVersion_,                fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "modelName",                 &fmu->fmi2.modelName,                  fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "guid",                      &fmu->fmi2.guid,                       fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "description",               &fmu->fmi2.description,                fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "author",                    &fmu->fmi2.author,                     fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "version",                   &fmu->fmi2.version,                    fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "copyright",                 &fmu->fmi2.copyright,                  fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "license",                   &fmu->fmi2.license,                    fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "generationTool",            &fmu->fmi2.generationTool,             fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "generationDateAndTime",     &fmu->fmi2.generationDateAndTime,      fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "variableNamingConvention",  &fmu->fmi2.variableNamingConvention,   fmu);
    parseInt32AttributeEzXml(rootElement, "numberOfEventIndicators",    &fmu->fmi2.numberOfEventIndicators);

    ezxml_t cosimElement = ezxml_child(rootElement, "CoSimulation");
    if(cosimElement) {
        fmu->fmi2.supportsCoSimulation = true;
        parseStringAttributeEzXmlAndRememberPointer(cosimElement, "modelIdentifier",                         &fmu->fmi2.cs.modelIdentifier,                          fmu);
        parseBooleanAttributeEzXml(cosimElement, "needsExecutionTool",                      &fmu->fmi2.cs.needsExecutionTool);
        parseBooleanAttributeEzXml(cosimElement, "canHandleVariableCommunicationStepSize",  &fmu->fmi2.cs.canHandleVariableCommunicationStepSize);
        parseBooleanAttributeEzXml(cosimElement, "canInterpolateInputs",                    &fmu->fmi2.cs.canInterpolateInputs);
        parseInt32AttributeEzXml(cosimElement, "maxOutputDerivativeOrder",                  &fmu->fmi2.cs.maxOutputDerivativeOrder);
        parseBooleanAttributeEzXml(cosimElement, "canRunAsynchronuously",                   &fmu->fmi2.cs.canRunAsynchronuously);
        parseBooleanAttributeEzXml(cosimElement, "canBeInstantiatedOnlyOncePerProcess",     &fmu->fmi2.cs.canBeInstantiatedOnlyOncePerProcess);
        parseBooleanAttributeEzXml(cosimElement, "canNotUseMemoryManagementFunctions",      &fmu->fmi2.cs.canNotUseMemoryManagementFunctions);
        parseBooleanAttributeEzXml(cosimElement, "canGetAndSetFMUstate",                    &fmu->fmi2.cs.canGetAndSetFMUState);
        parseBooleanAttributeEzXml(cosimElement, "canSerializeFMUstate",                    &fmu->fmi2.cs.canSerializeFMUState);
        parseBooleanAttributeEzXml(cosimElement, "providesDirectionalDerivative",           &fmu->fmi2.cs.providesDirectionalDerivative);
    }

    ezxml_t modelExchangeElement = ezxml_child(rootElement, "ModelExchange");
    if(modelExchangeElement) {
        fmu->fmi2.supportsModelExchange = true;
        parseStringAttributeEzXmlAndRememberPointer(modelExchangeElement, "modelIdentifier",                         &fmu->fmi2.me.modelIdentifier,                      fmu);
        parseBooleanAttributeEzXml(modelExchangeElement, "needsExecutionTool",                      &fmu->fmi2.me.needsExecutionTool);
        parseBooleanAttributeEzXml(modelExchangeElement, "completedIntegratorStepNotNeeded",        &fmu->fmi2.me.completedIntegratorStepNotNeeded);
        parseBooleanAttributeEzXml(modelExchangeElement, "canBeInstantiatedOnlyOncePerProcess",     &fmu->fmi2.me.canBeInstantiatedOnlyOncePerProcess);
        parseBooleanAttributeEzXml(modelExchangeElement, "canNotUseMemoryManagementFunctions",      &fmu->fmi2.me.canNotUseMemoryManagementFunctions);
        parseBooleanAttributeEzXml(modelExchangeElement, "canGetAndSetFMUstate",                    &fmu->fmi2.me.canGetAndSetFMUState);
        parseBooleanAttributeEzXml(modelExchangeElement, "canSerializeFMUstate",                    &fmu->fmi2.me.canSerializeFMUState);
        parseBooleanAttributeEzXml(modelExchangeElement, "providesDirectionalDerivative",           &fmu->fmi2.me.providesDirectionalDerivative);
    }

    ezxml_t unitDefinitionsElement = ezxml_child(rootElement, "UnitDefinitions");
    if(unitDefinitionsElement) {
        //First count number of units
        fmu->fmi2.numberOfUnits = 0;
        for(ezxml_t unitElement = unitDefinitionsElement->child; unitElement; unitElement = unitElement->next) {
            if(!strcmp(unitElement->name, "Unit")) {
                ++fmu->fmi2.numberOfUnits;
            }
        }
        if(fmu->fmi2.numberOfUnits > 0) {
            fmu->fmi2.units = mallocAndRememberPointer(fmu, fmu->fmi2.numberOfUnits*sizeof(fmi2UnitHandle));
        }
        int i=0;
        for(ezxml_t unitElement = unitDefinitionsElement->child; unitElement; unitElement = unitElement->next) {
            if(strcmp(unitElement->name, "Unit")) {
                continue;   //Wrong element name
            }
            fmi2UnitHandle unit;
            unit.baseUnit = NULL;
            unit.displayUnits = NULL;
            parseStringAttributeEzXmlAndRememberPointer(unitElement, "name", &unit.name, fmu);
            unit.numberOfDisplayUnits = 0;
            for(ezxml_t unitSubElement = unitElement->child; unitSubElement; unitSubElement = unitSubElement->ordered) {
                if(!strcmp(unitSubElement->name, "BaseUnit")) {
                    unit.baseUnit = mallocAndRememberPointer(fmu, sizeof(fmi2BaseUnitHandle));
                    unit.baseUnit->kg = 0;
                    unit.baseUnit->m = 0;
                    unit.baseUnit->s = 0;
                    unit.baseUnit->A = 0;
                    unit.baseUnit->K = 0;
                    unit.baseUnit->mol = 0;
                    unit.baseUnit->cd = 0;
                    unit.baseUnit->rad = 0;
                    unit.baseUnit->factor = 1;
                    unit.baseUnit->offset = 0;
                    parseInt32AttributeEzXml(unitSubElement,    "kg",       &unit.baseUnit->kg);
                    parseInt32AttributeEzXml(unitSubElement,    "m",        &unit.baseUnit->m);
                    parseInt32AttributeEzXml(unitSubElement,    "s",        &unit.baseUnit->s);
                    parseInt32AttributeEzXml(unitSubElement,    "A",        &unit.baseUnit->A);
                    parseInt32AttributeEzXml(unitSubElement,    "K",        &unit.baseUnit->K);
                    parseInt32AttributeEzXml(unitSubElement,    "mol",      &unit.baseUnit->mol);
                    parseInt32AttributeEzXml(unitSubElement,    "cd",       &unit.baseUnit->cd);
                    parseInt32AttributeEzXml(unitSubElement,    "rad",      &unit.baseUnit->rad);
                    parseFloat64AttributeEzXml(unitSubElement,  "factor",   &unit.baseUnit->factor);
                    parseFloat64AttributeEzXml(unitSubElement,  "offset",   &unit.baseUnit->offset);
                }
                else if(!strcmp(unitSubElement->name, "DisplayUnit")) {
                    ++unit.numberOfDisplayUnits;  //Just count them for now, so we can allocate memory before loading them
                }
            }
            if(unit.numberOfDisplayUnits > 0) {
                unit.displayUnits = mallocAndRememberPointer(fmu, unit.numberOfDisplayUnits*sizeof(fmi2DisplayUnitHandle));
            }
            int j=0;
            for(ezxml_t unitSubElement = unitElement->child; unitSubElement; unitSubElement = unitSubElement->ordered) {
                if(!strcmp(unitSubElement->name, "DisplayUnit")) {
                    unit.displayUnits[j].factor = 1;
                    unit.displayUnits[j].offset = 0;
                    parseStringAttributeEzXmlAndRememberPointer(unitSubElement,  "name",      &unit.displayUnits[j].name, fmu);
                    parseFloat64AttributeEzXml(unitSubElement, "factor",    &unit.displayUnits[j].factor);
                    parseFloat64AttributeEzXml(unitSubElement, "offset",    &unit.displayUnits[j].offset);
                    ++j;
                }
            }
            fmu->fmi2.units[i] = unit;
            ++i;
        }
    }

    ezxml_t defaultExperimentElement = ezxml_child(rootElement, "DefaultExperiment");
    if(defaultExperimentElement) {
        fmu->fmi2.defaultStartTimeDefined = parseFloat64AttributeEzXml(defaultExperimentElement, "startTime", &fmu->fmi2.defaultStartTime);
        fmu->fmi2.defaultStopTimeDefined =  parseFloat64AttributeEzXml(defaultExperimentElement, "stopTime",  &fmu->fmi2.defaultStopTime);
        fmu->fmi2.defaultToleranceDefined = parseFloat64AttributeEzXml(defaultExperimentElement, "tolerance", &fmu->fmi2.defaultTolerance);
        fmu->fmi2.defaultStepSizeDefined =  parseFloat64AttributeEzXml(defaultExperimentElement, "stepSize",  &fmu->fmi2.defaultStepSize);
    }

    ezxml_t modelVariablesElement = ezxml_child(rootElement, "ModelVariables");
    if(modelVariablesElement) {
        for(ezxml_t varElement = ezxml_child(modelVariablesElement, "ScalarVariable"); varElement; varElement = varElement->next) {
            fmi2VariableHandle var;
            var.canHandleMultipleSetPerTimeInstant = false; //Default value if attribute not defined
            var.name = NULL;
            var.description = NULL;
            var.quantity = NULL;
            var.unit = NULL;
            var.displayUnit = NULL;
            var.relativeQuantity = false;
            var.min = -DBL_MAX;
            var.max = DBL_MAX;
            var.nominal = 1;
            var.unbounded = false;
            var.derivative = 0;

            parseStringAttributeEzXmlAndRememberPointer(varElement, "name", &var.name, fmu);
            parseInt64AttributeEzXml(varElement, "valueReference", &var.valueReference);
            parseStringAttributeEzXmlAndRememberPointer(varElement, "description", &var.description, fmu);
            parseBooleanAttributeEzXml(varElement, "canHandleMultipleSetPerTimeInstant", &var.canHandleMultipleSetPerTimeInstant);

            var.causality = fmi2CausalityLocal;
            const char* causality = NULL;
            if(parseStringAttributeEzXml(varElement, "causality", &causality)) {
                if(!strcmp(causality, "input")) {
                    var.causality = fmi2CausalityInput;
                }
                else if(!strcmp(causality, "output")) {
                    var.causality = fmi2CausalityOutput;
                }
                else if(!strcmp(causality, "parameter")) {
                    var.causality = fmi2CausalityParameter;
                }
                else if(!strcmp(causality, "calculatedParameter")) {
                    var.causality = fmi2CausalityCalculatedParameter;
                }
                else if(!strcmp(causality, "local")) {
                    var.causality = fmi2CausalityLocal;
                }
                else if(!strcmp(causality, "independent")) {
                    var.causality = fmi2CausalityIndependent;
                }
                else {
                    printf("Unknown causality: %s\n", causality);
                    freeDuplicatedConstChar(causality);
                    return false;
                }
                freeDuplicatedConstChar(causality);
            }

            var.variability = fmi2VariabilityContinuous;
            const char* variability = NULL;
            if(parseStringAttributeEzXml(varElement, "variability", &variability)) {
                if(variability && !strcmp(variability, "fixed")) {
                    var.variability = fmi2VariabilityFixed;
                }
                else if(variability && !strcmp(variability, "tunable")) {
                    var.variability = fmi2VariabilityTunable;
                }
                else if(variability && !strcmp(variability, "constant")) {
                    var.variability = fmi2VariabilityConstant;
                }
                else if(variability && !strcmp(variability, "discrete")) {
                    var.variability = fmi2VariabilityDiscrete;
                }
                else if(variability && !strcmp(variability, "continuous")) {
                    var.variability = fmi2VariabilityContinuous;
                }
                else if(variability) {
                    printf("Unknown variability: %s\n", variability);
                    freeDuplicatedConstChar(variability);
                    return false;
                }
                freeDuplicatedConstChar(variability);
            }

            var.initial = fmi2InitialUnknown;
            const char* initial = NULL;
            if(parseStringAttributeEzXml(varElement, "initial", &initial)) {
                if(initial && !strcmp(initial, "approx")) {
                    var.initial = fmi2InitialApprox;
                }
                else if(initial && !strcmp(initial, "calculated")) {
                    var.initial = fmi2InitialCalculated;
                }
                else if(initial && !strcmp(initial, "exact")) {
                    var.initial = fmi2InitialExact;
                }
                else {
                    printf("Unknown intial: %s\n", initial);
                    freeDuplicatedConstChar(initial);
                    return false;
                }
                freeDuplicatedConstChar(initial);
            }
            else {
                // calculate the initial value according to fmi specification 2.2 table page 51
                var.initial = initialDefaultTable[mapVariabilityIndex[var.variability]][mapCausalityIndex[var.causality]];
            }

            var.hasStartValue = false;
            var.relativeQuantity = false;
            var.min = -DBL_MAX;
            var.max = DBL_MAX;
            var.nominal = 1;
            var.unbounded = false;

            ezxml_t realElement = ezxml_child(varElement, "Real");
            if(realElement) {
                fmu->fmi2.hasRealVariables = true;
                var.datatype = fmi2DataTypeReal;
                if(parseFloat64AttributeEzXml(realElement, "start", &var.startReal)) {
                    var.hasStartValue = true;
                }
                if(parseUInt32AttributeEzXml(realElement, "derivative", &var.derivative)) {
                    fmu->fmi2.numberOfContinuousStates++;
                }
                parseStringAttributeEzXmlAndRememberPointer(realElement, "quantity", &var.quantity, fmu);
                parseStringAttributeEzXmlAndRememberPointer(realElement, "unit", &var.unit, fmu);
                parseStringAttributeEzXmlAndRememberPointer(realElement, "displayUnit", &var.displayUnit, fmu);
                parseBooleanAttributeEzXml(realElement, "relativeQuantity", &var.relativeQuantity);
                parseFloat64AttributeEzXml(realElement, "min", &var.min);
                parseFloat64AttributeEzXml(realElement, "max", &var.max);
                parseFloat64AttributeEzXml(realElement, "nominal", &var.nominal);
                parseBooleanAttributeEzXml(realElement, "unbounded", &var.unbounded);
            }

            ezxml_t integerElement = ezxml_child(varElement, "Integer");
            if(integerElement) {
                fmu->fmi2.hasIntegerVariables = true;
                var.datatype = fmi2DataTypeInteger;
                if(parseInt32AttributeEzXml(integerElement, "start", &var.startInteger)) {
                    var.hasStartValue = true;
                }
                parseStringAttributeEzXmlAndRememberPointer(integerElement, "quantity", &var.quantity, fmu);
                parseFloat64AttributeEzXml(integerElement, "min", &var.min);
                parseFloat64AttributeEzXml(integerElement, "max", &var.max);
            }

            ezxml_t booleanElement = ezxml_child(varElement, "Boolean");
            if(booleanElement) {
                fmu->fmi2.hasBooleanVariables = true;
                var.datatype = fmi2DataTypeBoolean;
                bool startBoolean;
                if(parseBooleanAttributeEzXml(booleanElement, "start", &startBoolean)) {
                    var.hasStartValue = true;
                }
                var.startBoolean = startBoolean;
            }

            ezxml_t stringElement = ezxml_child(varElement, "String");
            if(stringElement) {
                fmu->fmi2.hasStringVariables = true;
                var.datatype = fmi2DataTypeString;
                if(parseStringAttributeEzXmlAndRememberPointer(stringElement, "start", &var.startString, fmu)) {
                    var.hasStartValue = true;
                }
            }

            ezxml_t enumerationElement = ezxml_child(varElement, "Enumeration");
            if(enumerationElement) {
                fmu->fmi2.hasEnumerationVariables = true;
                var.datatype = fmi2DataTypeEnumeration;
                if(parseInt32AttributeEzXml(enumerationElement, "start", &var.startEnumeration)) {
                    var.hasStartValue = true;
                }
                parseStringAttributeEzXmlAndRememberPointer(enumerationElement, "quantity", &var.quantity, fmu);
            }

            if(fmu->fmi2.numberOfVariables >= fmu->fmi2.variablesSize) {
                fmu->fmi2.variablesSize *= 2;
                fmu->fmi2.variables = reallocAndRememberPointer(fmu, fmu->fmi2.variables, fmu->fmi2.variablesSize*sizeof(fmi2VariableHandle));
            }

            fmu->fmi2.variables[fmu->fmi2.numberOfVariables] = var;
            fmu->fmi2.numberOfVariables++;
        }
    }

    ezxml_free(rootElement);

    chdir(cwd);

    return true;
}


//! @brief Parses modelDescription.xml for FMI 3
//! @param fmu FMU handle
//! @returns True if parsing was successful
bool parseModelDescriptionFmi3(fmiHandle *fmu)
{
    fmu->fmi3.modelName = NULL;
    fmu->fmi3.instantiationToken = NULL;
    fmu->fmi3.description = NULL;
    fmu->fmi3.author = NULL;
    fmu->fmi3.version = NULL;
    fmu->fmi3.copyright = NULL;
    fmu->fmi3.license = NULL;
    fmu->fmi3.generationTool = NULL;
    fmu->fmi3.generationDateAndTime = NULL;

    fmu->fmi3.cs.modelIdentifier = NULL;
    fmu->fmi3.cs.needsExecutionTool = false;
    fmu->fmi3.cs.canBeInstantiatedOnlyOncePerProcess = false;
    fmu->fmi3.cs.canGetAndSetFMUState = false;
    fmu->fmi3.cs.canSerializeFMUState = false;
    fmu->fmi3.cs.providesDirectionalDerivative = false;
    fmu->fmi3.cs.providesAdjointDerivatives = false;
    fmu->fmi3.cs.providesPerElementDependencies = false;
    fmu->fmi3.cs.maxOutputDerivativeOrder = 0;
    fmu->fmi3.cs.providesIntermediateUpdate = false;
    fmu->fmi3.cs.mightReturnEarlyFromDoStep = false;
    fmu->fmi3.cs.providesEvaluateDiscreteStates = false;
    fmu->fmi3.cs.recommendedIntermediateInputSmoothness = 0;
    fmu->fmi3.cs.canHandleVariableCommunicationStepSize = false;
    fmu->fmi3.cs.canReturnEarlyAfterIntermediateUpdate = false;
    fmu->fmi3.cs.fixedInternalStepSize = 0;
    fmu->fmi3.cs.hasEventMode = false;

    fmu->fmi3.me.modelIdentifier = NULL;
    fmu->fmi3.me.needsExecutionTool = false;
    fmu->fmi3.me.canBeInstantiatedOnlyOncePerProcess = false;
    fmu->fmi3.me.canGetAndSetFMUState = false;
    fmu->fmi3.me.canSerializeFMUState = false;
    fmu->fmi3.me.providesDirectionalDerivative = false;
    fmu->fmi3.me.providesAdjointDerivatives = false;
    fmu->fmi3.me.providesPerElementDependencies = false;
    fmu->fmi3.me.providesEvaluateDiscreteStates = false;
    fmu->fmi3.me.needsCompletedIntegratorStep = false;

    fmu->fmi3.se.modelIdentifier = NULL;
    fmu->fmi3.se.needsExecutionTool = false;
    fmu->fmi3.se.canBeInstantiatedOnlyOncePerProcess = false;
    fmu->fmi3.se.canGetAndSetFMUState = false;
    fmu->fmi3.se.canSerializeFMUState = false;
    fmu->fmi3.se.providesDirectionalDerivative = false;
    fmu->fmi3.se.providesAdjointDerivatives = false;
    fmu->fmi3.se.providesPerElementDependencies = false;

    fmu->fmi3.variableNamingConvention = "flat";

    fmu->fmi3.supportsCoSimulation = false;
    fmu->fmi3.supportsModelExchange = false;
    fmu->fmi3.supportsScheduledExecution = false;

    fmu->fmi3.defaultStartTimeDefined = false;
    fmu->fmi3.defaultStopTimeDefined = false;
    fmu->fmi3.defaultToleranceDefined = false;
    fmu->fmi3.defaultStepSizeDefined = false;

    fmu->fmi3.hasClockVariables = false;
    fmu->fmi3.hasFloat64Variables = false;
    fmu->fmi3.hasFloat32Variables = false;
    fmu->fmi3.hasInt64Variables = false;
    fmu->fmi3.hasInt32Variables = false;
    fmu->fmi3.hasInt16Variables = false;
    fmu->fmi3.hasInt8Variables = false;
    fmu->fmi3.hasUInt64Variables = false;
    fmu->fmi3.hasUInt32Variables = false;
    fmu->fmi3.hasUInt16Variables = false;
    fmu->fmi3.hasUInt8Variables = false;
    fmu->fmi3.hasBooleanVariables = false;
    fmu->fmi3.hasStringVariables = false;
    fmu->fmi3.hasBinaryVariables = false;
    fmu->fmi3.hasClockVariables = false;
    fmu->fmi3.hasStructuralParameters = false;

    char cwd[FILENAME_MAX];
#ifdef _WIN32
    _getcwd(cwd, sizeof(char)*FILENAME_MAX);
#else
    getcwd(cwd, sizeof(char)*FILENAME_MAX);
#endif
    chdir(fmu->unzippedLocation);

    ezxml_t rootElement = ezxml_parse_file("modelDescription.xml");
    if(strcmp(rootElement->name, "fmiModelDescription")) {
        printf("Wrong root tag name: %s\n", rootElement->name);
        return false;
    }

    parseStringAttributeEzXmlAndRememberPointer(rootElement, "modelName",                 &fmu->fmi3.modelName,                  fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "instantiationToken",        &fmu->fmi3.instantiationToken,         fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "description",               &fmu->fmi3.description,                fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "author",                    &fmu->fmi3.author,                     fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "version",                   &fmu->fmi3.version,                    fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "copyright",                 &fmu->fmi3.copyright,                  fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "license",                   &fmu->fmi3.license,                    fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "generationTool",            &fmu->fmi3.generationTool,             fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "generationDateAndTime",     &fmu->fmi3.generationDateAndTime,      fmu);
    parseStringAttributeEzXmlAndRememberPointer(rootElement, "variableNamingConvention",  &fmu->fmi3.variableNamingConvention,   fmu);

    ezxml_t cosimElement = ezxml_child(rootElement, "CoSimulation");
    if(cosimElement) {
        fmu->fmi3.supportsCoSimulation = true;
        parseStringAttributeEzXmlAndRememberPointer(cosimElement, "modelIdentifier",                         &fmu->fmi3.cs.modelIdentifier,                          fmu);
        parseBooleanAttributeEzXml(cosimElement, "needsExecutionTool",                      &fmu->fmi3.cs.needsExecutionTool);
        parseBooleanAttributeEzXml(cosimElement, "canBeInstantiatedOnlyOncePerProcess",     &fmu->fmi3.cs.canBeInstantiatedOnlyOncePerProcess);
        parseBooleanAttributeEzXml(cosimElement, "canGetAndSetFMUState",                    &fmu->fmi3.cs.canGetAndSetFMUState);
        parseBooleanAttributeEzXml(cosimElement, "canSerializeFMUState",                    &fmu->fmi3.cs.canSerializeFMUState);
        parseBooleanAttributeEzXml(cosimElement, "providesDirectionalDerivative",           &fmu->fmi3.cs.providesDirectionalDerivative);
        parseBooleanAttributeEzXml(cosimElement, "providesAdjointDerivatives",              &fmu->fmi3.cs.providesAdjointDerivatives);
        parseBooleanAttributeEzXml(cosimElement, "providesPerElementDependencies",          &fmu->fmi3.cs.providesPerElementDependencies);
        parseInt32AttributeEzXml(cosimElement,   "maxOutputDerivativeOrder",                &fmu->fmi3.cs.maxOutputDerivativeOrder);
        parseBooleanAttributeEzXml(cosimElement, "providesIntermediateUpdate",              &fmu->fmi3.cs.providesIntermediateUpdate);
        parseBooleanAttributeEzXml(cosimElement, "mightReturnEarlyFromDoStep",              &fmu->fmi3.cs.mightReturnEarlyFromDoStep);
        parseBooleanAttributeEzXml(cosimElement, "providesEvaluateDiscreteStates",          &fmu->fmi3.cs.providesEvaluateDiscreteStates);
        parseInt32AttributeEzXml(cosimElement,   "recommendedIntermediateInputSmoothness",  &fmu->fmi3.cs.recommendedIntermediateInputSmoothness);
        parseBooleanAttributeEzXml(cosimElement, "canHandleVariableCommunicationStepSize",  &fmu->fmi3.cs.canHandleVariableCommunicationStepSize);
        parseBooleanAttributeEzXml(cosimElement, "canReturnEarlyAfterIntermediateUpdate",   &fmu->fmi3.cs.canReturnEarlyAfterIntermediateUpdate);
        parseFloat64AttributeEzXml(cosimElement, "fixedInternalStepSize",                   &fmu->fmi3.cs.fixedInternalStepSize);
        parseBooleanAttributeEzXml(cosimElement, "hasEventMode",                            &fmu->fmi3.cs.hasEventMode);
    }

    ezxml_t modelExchangeElement = ezxml_child(rootElement, "ModelExchange");
    if(modelExchangeElement) {
        fmu->fmi3.supportsModelExchange = true;
        parseStringAttributeEzXmlAndRememberPointer(modelExchangeElement, "modelIdentifier",                     &fmu->fmi3.me.modelIdentifier,                      fmu);
        parseBooleanAttributeEzXml(modelExchangeElement, "needsExecutionTool",                  &fmu->fmi3.me.needsExecutionTool);
        parseBooleanAttributeEzXml(modelExchangeElement, "canBeInstantiatedOnlyOncePerProcess", &fmu->fmi3.me.canBeInstantiatedOnlyOncePerProcess);
        parseBooleanAttributeEzXml(modelExchangeElement, "canGetAndSetFMUState",                &fmu->fmi3.me.canGetAndSetFMUState);
        parseBooleanAttributeEzXml(modelExchangeElement, "canSerializeFMUState",                &fmu->fmi3.me.canSerializeFMUState);
        parseBooleanAttributeEzXml(modelExchangeElement, "providesDirectionalDerivative",       &fmu->fmi3.me.providesDirectionalDerivative);
        parseBooleanAttributeEzXml(modelExchangeElement, "providesAdjointDerivatives",          &fmu->fmi3.me.providesAdjointDerivatives);
        parseBooleanAttributeEzXml(modelExchangeElement, "providesPerElementDependencies",      &fmu->fmi3.me.providesPerElementDependencies);
        parseBooleanAttributeEzXml(modelExchangeElement, "needsCompletedIntegratorStep",        &fmu->fmi3.me.needsCompletedIntegratorStep);
        parseBooleanAttributeEzXml(modelExchangeElement, "providesEvaluateDiscreteStates",      &fmu->fmi3.me.providesEvaluateDiscreteStates);
    }

    ezxml_t scheduledExecutionElement = ezxml_child(rootElement, "ScheduledExecution");
    if(scheduledExecutionElement) {
        fmu->fmi3.supportsScheduledExecution = true;
        parseStringAttributeEzXmlAndRememberPointer(scheduledExecutionElement, "modelIdentifier",                        &fmu->fmi3.se.modelIdentifier,                      fmu);
        parseBooleanAttributeEzXml(scheduledExecutionElement, "needsExecutionTool",                     &fmu->fmi3.se.needsExecutionTool);
        parseBooleanAttributeEzXml(scheduledExecutionElement, "canBeInstantiatedOnlyOncePerProcess",    &fmu->fmi3.se.canBeInstantiatedOnlyOncePerProcess);
        parseBooleanAttributeEzXml(scheduledExecutionElement, "canGetAndSetFMUState",                   &fmu->fmi3.se.canGetAndSetFMUState);
        parseBooleanAttributeEzXml(scheduledExecutionElement, "canSerializeFMUState",                   &fmu->fmi3.se.canSerializeFMUState);
        parseBooleanAttributeEzXml(scheduledExecutionElement, "providesDirectionalDerivative",          &fmu->fmi3.se.providesDirectionalDerivative);
        parseBooleanAttributeEzXml(scheduledExecutionElement, "providesAdjointDerivatives",             &fmu->fmi3.se.providesAdjointDerivatives);
        parseBooleanAttributeEzXml(scheduledExecutionElement, "providesPerElementDependencies",         &fmu->fmi3.se.providesPerElementDependencies);
    }

    ezxml_t unitDefinitionsElement = ezxml_child(rootElement, "UnitDefinitions");
    if(unitDefinitionsElement) {
        //First count number of units
        fmu->fmi3.numberOfUnits = 0;
        for(ezxml_t unitElement = unitDefinitionsElement->child; unitElement; unitElement = unitElement->next) {
            if(!strcmp(unitElement->name, "Unit")) {
                ++fmu->fmi3.numberOfUnits;
            }
        }
        if(fmu->fmi3.numberOfUnits > 0) {
            fmu->fmi3.units = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfUnits*sizeof(fmi3UnitHandle));
        }
        int i=0;
        for(ezxml_t unitElement = unitDefinitionsElement->child; unitElement; unitElement = unitElement->next) {
            if(strcmp(unitElement->name, "Unit")) {
                continue;   //Wrong element name
            }
            fmi3UnitHandle unit;
            unit.baseUnit = NULL;
            unit.displayUnits = NULL;
            parseStringAttributeEzXmlAndRememberPointer(unitElement, "name", &unit.name, fmu);
            unit.numberOfDisplayUnits = 0;
            for(ezxml_t unitSubElement = unitElement->child; unitSubElement; unitSubElement = unitSubElement->ordered) {
                if(!strcmp(unitSubElement->name, "BaseUnit")) {
                    unit.baseUnit = mallocAndRememberPointer(fmu, sizeof(fmi3BaseUnit));
                    unit.baseUnit->kg = 0;
                    unit.baseUnit->m = 0;
                    unit.baseUnit->s = 0;
                    unit.baseUnit->A = 0;
                    unit.baseUnit->K = 0;
                    unit.baseUnit->mol = 0;
                    unit.baseUnit->cd = 0;
                    unit.baseUnit->rad = 0;
                    unit.baseUnit->factor = 1;
                    unit.baseUnit->offset = 0;
                    parseInt32AttributeEzXml(unitSubElement,    "kg",       &unit.baseUnit->kg);
                    parseInt32AttributeEzXml(unitSubElement,    "m",        &unit.baseUnit->m);
                    parseInt32AttributeEzXml(unitSubElement,    "s",        &unit.baseUnit->s);
                    parseInt32AttributeEzXml(unitSubElement,    "A",        &unit.baseUnit->A);
                    parseInt32AttributeEzXml(unitSubElement,    "K",        &unit.baseUnit->K);
                    parseInt32AttributeEzXml(unitSubElement,    "mol",      &unit.baseUnit->mol);
                    parseInt32AttributeEzXml(unitSubElement,    "cd",       &unit.baseUnit->cd);
                    parseInt32AttributeEzXml(unitSubElement,    "rad",      &unit.baseUnit->rad);
                    parseFloat64AttributeEzXml(unitSubElement,  "factor",   &unit.baseUnit->factor);
                    parseFloat64AttributeEzXml(unitSubElement,  "offset",   &unit.baseUnit->offset);
                }
                else if(!strcmp(unitSubElement->name, "DisplayUnit")) {
                    ++unit.numberOfDisplayUnits;  //Just count them for now, so we can allocate memory before loading them
                }
            }
            if(unit.numberOfDisplayUnits > 0) {
                unit.displayUnits = mallocAndRememberPointer(fmu, unit.numberOfDisplayUnits*sizeof(fmi3DisplayUnitHandle));
            }
            int j=0;
            for(ezxml_t unitSubElement = unitElement->child; unitSubElement; unitSubElement = unitSubElement->ordered) {
                if(!strcmp(unitSubElement->name, "DisplayUnit")) {
                    unit.displayUnits[j].factor = 1;
                    unit.displayUnits[j].offset = 0;
                    unit.displayUnits[j].inverse = false;
                    parseStringAttributeEzXmlAndRememberPointer(unitSubElement,  "name",      &unit.displayUnits[j].name,    fmu);
                    parseFloat64AttributeEzXml(unitSubElement, "factor",    &unit.displayUnits[j].factor);
                    parseFloat64AttributeEzXml(unitSubElement, "offset",    &unit.displayUnits[j].offset);
                    parseBooleanAttributeEzXml(unitSubElement, "inverse",   &unit.displayUnits[j].inverse);
                    ++j;
                }
            }
            fmu->fmi3.units[i] = unit;
            ++i;
        }
    }

    ezxml_t typeDefinitionsElement = ezxml_child(rootElement, "TypeDefinitions");
    if(typeDefinitionsElement) {
        //Count all elements by type
        fmu->fmi3.numberOfFloat64Types = 0;
        fmu->fmi3.numberOfFloat32Types = 0;
        fmu->fmi3.numberOfInt64Types = 0;
        fmu->fmi3.numberOfInt32Types = 0;
        fmu->fmi3.numberOfInt16Types = 0;
        fmu->fmi3.numberOfInt8Types = 0;
        fmu->fmi3.numberOfUInt64Types = 0;
        fmu->fmi3.numberOfUInt32Types = 0;
        fmu->fmi3.numberOfUInt16Types = 0;
        fmu->fmi3.numberOfUInt8Types = 0;
        fmu->fmi3.numberOfBooleanTypes = 0;
        fmu->fmi3.numberOfStringTypes = 0;
        fmu->fmi3.numberOfBinaryTypes = 0;
        fmu->fmi3.numberOfEnumerationTypes = 0;
        fmu->fmi3.numberOfClockTypes = 0;
        for(ezxml_t typeElement = typeDefinitionsElement->child; typeElement; typeElement = typeElement->ordered) {
            if(!strcmp(typeElement->name, "Float64Type")) {
                ++fmu->fmi3.numberOfFloat64Types;
            }
            else if(!strcmp(typeElement->name, "Float32Type")) {
                ++fmu->fmi3.numberOfFloat32Types;
            }
            else if(!strcmp(typeElement->name, "Int64Type")) {
                ++fmu->fmi3.numberOfInt64Types;
            }
            else if(!strcmp(typeElement->name, "Int32Type")) {
                ++fmu->fmi3.numberOfInt32Types;
            }
            else if(!strcmp(typeElement->name, "Int16Type")) {
                ++fmu->fmi3.numberOfInt16Types;
            }
            else if(!strcmp(typeElement->name, "Int8Type")) {
                ++fmu->fmi3.numberOfInt8Types;
            }
            else if(!strcmp(typeElement->name, "UInt64Type")) {
                ++fmu->fmi3.numberOfUInt64Types;
            }
            else if(!strcmp(typeElement->name, "UInt32Type")) {
                ++fmu->fmi3.numberOfUInt32Types;
            }
            else if(!strcmp(typeElement->name, "UInt16Type")) {
                ++fmu->fmi3.numberOfUInt16Types;
            }
            else if(!strcmp(typeElement->name, "UInt8Type")) {
                ++fmu->fmi3.numberOfUInt8Types;
            }
            else if(!strcmp(typeElement->name, "BooleanType")) {
                ++fmu->fmi3.numberOfBooleanTypes;
            }
            else if(!strcmp(typeElement->name, "StringType")) {
                ++fmu->fmi3.numberOfStringTypes;
            }
            else if(!strcmp(typeElement->name, "BinaryType")) {
                ++fmu->fmi3.numberOfBinaryTypes;
            }
            else if(!strcmp(typeElement->name, "EnumerationType")) {
                ++fmu->fmi3.numberOfEnumerationTypes;
            }
            else if(!strcmp(typeElement->name, "ClockType")) {
                ++fmu->fmi3.numberOfClockTypes;
            }
        }

        //Allocate memory
        if(fmu->fmi3.numberOfFloat64Types > 0) {
            fmu->fmi3.float64Types = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfFloat64Types*sizeof(fmi3Float64Type));
        }
        if(fmu->fmi3.numberOfFloat32Types > 0) {
            fmu->fmi3.float32Types = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfFloat32Types*sizeof(fmi3Float32Type));
        }
        if(fmu->fmi3.numberOfInt64Types > 0) {
            fmu->fmi3.int64Types = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfInt64Types*sizeof(fmi3Int64Type));
        }
        if(fmu->fmi3.numberOfInt32Types > 0) {
            fmu->fmi3.int32Types = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfInt32Types*sizeof(fmi3Int32Type));
        }
        if(fmu->fmi3.numberOfInt16Types > 0) {
            fmu->fmi3.int16Types = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfInt16Types*sizeof(fmi3Int16Type));
        }
        if(fmu->fmi3.numberOfInt8Types > 0) {
            fmu->fmi3.int8Types = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfInt8Types*sizeof(fmi3Int8Type));
        }
        if(fmu->fmi3.numberOfUInt64Types > 0) {
            fmu->fmi3.uint64Types = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfUInt64Types*sizeof(fmi3UInt64Type));
        }
        if(fmu->fmi3.numberOfUInt32Types > 0) {
            fmu->fmi3.uint32Types = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfUInt32Types*sizeof(fmi3UInt32Type));
        }
        if(fmu->fmi3.numberOfUInt16Types > 0) {
            fmu->fmi3.uint16Types = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfUInt16Types*sizeof(fmi3UInt16Type));
        }
        if(fmu->fmi3.numberOfUInt8Types > 0) {
            fmu->fmi3.uint8Types = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfUInt8Types*sizeof(fmi3UInt8Type));
        }
        if(fmu->fmi3.numberOfBooleanTypes > 0) {
            fmu->fmi3.booleanTypes = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfBooleanTypes*sizeof(fmi3BooleanType));
        }
        if(fmu->fmi3.numberOfStringTypes > 0) {
            fmu->fmi3.stringTypes = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfStringTypes*sizeof(fmi3StringType));
        }
        if(fmu->fmi3.numberOfBinaryTypes > 0) {
            fmu->fmi3.binaryTypes = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfBinaryTypes*sizeof(fmi3BinaryType));
        }
        if(fmu->fmi3.numberOfEnumerationTypes > 0) {
            fmu->fmi3.enumTypes = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfEnumerationTypes*sizeof(fmi3EnumerationType));
        }
        if(fmu->fmi3.numberOfClockTypes > 0) {
            fmu->fmi3.clockTypes = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfClockTypes*sizeof(fmi3ClockType));
        }

        //Read all elements
        int iFloat64 = 0;
        int iFloat32 = 0;
        int iInt64 = 0;
        int iInt32 = 0;
        int iInt16 = 0;
        int iInt8 = 0;
        int iUInt64 = 0;
        int iUInt32 = 0;
        int iUInt16 = 0;
        int iUInt8 = 0;
        int iBoolean = 0;
        int iString = 0;
        int iBinary = 0;
        int iEnum = 0;
        int iClock = 0;
        for(ezxml_t typeElement = typeDefinitionsElement->child; typeElement; typeElement = typeElement->ordered) {
            if(!strcmp(typeElement->name, "Float64Type")) {
                fmu->fmi3.float64Types[iFloat64].name = "";
                fmu->fmi3.float64Types[iFloat64].description = "";
                fmu->fmi3.float64Types[iFloat64].quantity = "";
                fmu->fmi3.float64Types[iFloat64].unit = "";
                fmu->fmi3.float64Types[iFloat64].displayUnit = "";
                fmu->fmi3.float64Types[iFloat64].relativeQuantity = false;
                fmu->fmi3.float64Types[iFloat64].unbounded = false;
                fmu->fmi3.float64Types[iFloat64].min = -DBL_MAX;
                fmu->fmi3.float64Types[iFloat64].max = DBL_MAX;
                fmu->fmi3.float64Types[iFloat64].nominal = 1;
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "name", &fmu->fmi3.float64Types[iFloat64].name, fmu);
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "description", &fmu->fmi3.float64Types[iFloat64].description, fmu);
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "quantity", &fmu->fmi3.float64Types[iFloat64].quantity, fmu);
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "unit", &fmu->fmi3.float64Types[iFloat64].unit, fmu);
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "displayUnit", &fmu->fmi3.float64Types[iFloat64].displayUnit, fmu);
                parseBooleanAttributeEzXml(typeElement, "relativeQuantity", &fmu->fmi3.float64Types[iFloat64].relativeQuantity);
                parseBooleanAttributeEzXml(typeElement, "unbounded", &fmu->fmi3.float64Types[iFloat64].unbounded);
                parseFloat64AttributeEzXml(typeElement, "min", &fmu->fmi3.float64Types[iFloat64].min);
                parseFloat64AttributeEzXml(typeElement, "max", &fmu->fmi3.float64Types[iFloat64].max);
                parseFloat64AttributeEzXml(typeElement, "nominal", &fmu->fmi3.float64Types[iFloat64].nominal);
                ++iFloat64;
            }
            else if(!strcmp(typeElement->name, "Float32Type")) {
                fmu->fmi3.float32Types[iFloat32].name = "";
                fmu->fmi3.float32Types[iFloat32].description = "";
                fmu->fmi3.float32Types[iFloat32].quantity = "";
                fmu->fmi3.float32Types[iFloat32].unit = "";
                fmu->fmi3.float32Types[iFloat32].displayUnit = "";
                fmu->fmi3.float32Types[iFloat32].relativeQuantity = false;
                fmu->fmi3.float32Types[iFloat32].unbounded = false;
                fmu->fmi3.float32Types[iFloat32].min = -FLT_MAX;
                fmu->fmi3.float32Types[iFloat32].max = FLT_MAX;
                fmu->fmi3.float32Types[iFloat32].nominal = 1;
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "name", &fmu->fmi3.float32Types[iFloat32].name, fmu);
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "description", &fmu->fmi3.float32Types[iFloat32].description, fmu);
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "quantity", &fmu->fmi3.float32Types[iFloat32].quantity, fmu);
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "unit", &fmu->fmi3.float32Types[iFloat32].unit, fmu);
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "displayUnit", &fmu->fmi3.float32Types[iFloat32].displayUnit, fmu);
                parseBooleanAttributeEzXml(typeElement, "relativeQuantity", &fmu->fmi3.float32Types[iFloat32].relativeQuantity);
                parseBooleanAttributeEzXml(typeElement, "unbounded", &fmu->fmi3.float32Types[iFloat32].unbounded);
                parseFloat32AttributeEzXml(typeElement, "min", &fmu->fmi3.float32Types[iFloat32].min);
                parseFloat32AttributeEzXml(typeElement, "max", &fmu->fmi3.float32Types[iFloat32].max);
                parseFloat32AttributeEzXml(typeElement, "nominal", &fmu->fmi3.float32Types[iFloat32].nominal);
                ++iFloat32;
            }
            else if(!strcmp(typeElement->name, "Int64Type")) {
                fmu->fmi3.int64Types[iInt64].name = "";
                fmu->fmi3.int64Types[iInt64].min = -INT64_MAX;
                fmu->fmi3.int64Types[iInt64].max = INT64_MAX;
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "name", &fmu->fmi3.int64Types[iInt64].name, fmu);
                parseInt64AttributeEzXml(typeElement, "min", &fmu->fmi3.int64Types[iInt64].min);
                parseInt64AttributeEzXml(typeElement, "max", &fmu->fmi3.int64Types[iInt64].max);
                ++iInt64;
            }
            else if(!strcmp(typeElement->name, "Int32Type")) {
                fmu->fmi3.int32Types[iInt32].name = "";
                fmu->fmi3.int32Types[iInt32].min = -INT32_MAX;
                fmu->fmi3.int32Types[iInt32].max = INT32_MAX;
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "name", &fmu->fmi3.int32Types[iInt32].name, fmu);
                parseInt32AttributeEzXml(typeElement, "min", &fmu->fmi3.int32Types[iInt32].min);
                parseInt32AttributeEzXml(typeElement, "max", &fmu->fmi3.int32Types[iInt32].max);
                ++iInt32;
            }
            else if(!strcmp(typeElement->name, "Int16Type")) {
                fmu->fmi3.int16Types[iInt16].name = "";
                fmu->fmi3.int16Types[iInt16].min = -INT16_MAX;
                fmu->fmi3.int16Types[iInt16].max = INT16_MAX;
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "name", &fmu->fmi3.int16Types[iInt16].name, fmu);
                parseInt16AttributeEzXml(typeElement, "min", &fmu->fmi3.int16Types[iInt16].min);
                parseInt16AttributeEzXml(typeElement, "max", &fmu->fmi3.int16Types[iInt16].max);
                ++iInt16;
            }
            else if(!strcmp(typeElement->name, "Int8Type")) {
                fmu->fmi3.int8Types[iInt8].name = "";
                fmu->fmi3.int8Types[iInt8].min = -INT8_MAX;
                fmu->fmi3.int8Types[iInt8].max = INT8_MAX;
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "name", &fmu->fmi3.int8Types[iInt8].name, fmu);
                parseInt8AttributeEzXml(typeElement, "min", &fmu->fmi3.int8Types[iInt8].min);
                parseInt8AttributeEzXml(typeElement, "max", &fmu->fmi3.int8Types[iInt8].max);
                ++iInt8;
            }
            else if(!strcmp(typeElement->name, "UInt64Type")) {
                fmu->fmi3.uint64Types[iUInt64].name = "";
                fmu->fmi3.uint64Types[iUInt64].min = 0;
                fmu->fmi3.uint64Types[iUInt64].max = UINT64_MAX;
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "name", &fmu->fmi3.uint64Types[iUInt64].name, fmu);
                parseUInt64AttributeEzXml(typeElement, "min", &fmu->fmi3.uint64Types[iUInt64].min);
                parseUInt64AttributeEzXml(typeElement, "max", &fmu->fmi3.uint64Types[iUInt64].max);
                ++iUInt64;
            }
            else if(!strcmp(typeElement->name, "UInt32Type")) {
                fmu->fmi3.uint32Types[iUInt32].name = "";
                fmu->fmi3.uint32Types[iUInt32].min = 0;
                fmu->fmi3.uint32Types[iUInt32].max = UINT32_MAX;
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "name", &fmu->fmi3.uint32Types[iUInt32].name, fmu);
                parseUInt32AttributeEzXml(typeElement, "min", &fmu->fmi3.uint32Types[iUInt32].min);
                parseUInt32AttributeEzXml(typeElement, "max", &fmu->fmi3.uint32Types[iUInt32].max);
                ++iUInt32;
            }
            else if(!strcmp(typeElement->name, "UInt16Type")) {
                fmu->fmi3.uint16Types[iUInt16].name = "";
                fmu->fmi3.uint16Types[iUInt16].min = 0;
                fmu->fmi3.uint16Types[iUInt16].max = UINT16_MAX;
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "name", &fmu->fmi3.uint16Types[iUInt16].name, fmu);
                parseUInt16AttributeEzXml(typeElement, "min", &fmu->fmi3.uint16Types[iUInt16].min);
                parseUInt16AttributeEzXml(typeElement, "max", &fmu->fmi3.uint16Types[iUInt16].max);
                ++iUInt16;
            }
            else if(!strcmp(typeElement->name, "UInt8Type")) {
                fmu->fmi3.uint8Types[iUInt8].name = "";
                fmu->fmi3.uint8Types[iUInt8].min = 0;
                fmu->fmi3.uint8Types[iUInt8].max = UINT8_MAX;
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "name", &fmu->fmi3.uint8Types[iUInt8].name, fmu);
                parseUInt8AttributeEzXml(typeElement, "min", &fmu->fmi3.uint8Types[iUInt8].min);
                parseUInt8AttributeEzXml(typeElement, "max", &fmu->fmi3.uint8Types[iUInt8].max);
                ++iUInt8;
            }
            else if(!strcmp(typeElement->name, "BooleanType")) {
                fmu->fmi3.booleanTypes[iBoolean].name = "";
                fmu->fmi3.booleanTypes[iBoolean].description = "";
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "name", &fmu->fmi3.booleanTypes[iBoolean].name, fmu);
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "description", &fmu->fmi3.booleanTypes[iBoolean].description, fmu);
                ++iBoolean;
            }
            else if(!strcmp(typeElement->name, "StringType")) {
                fmu->fmi3.stringTypes[iString].name = "";
                fmu->fmi3.stringTypes[iString].description = "";
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "name", &fmu->fmi3.stringTypes[iString].name, fmu);
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "description", &fmu->fmi3.stringTypes[iString].description, fmu);
                ++iString;
            }
            else if(!strcmp(typeElement->name, "BinaryType")) {
                fmu->fmi3.binaryTypes[iBinary].name = "";
                fmu->fmi3.binaryTypes[iBinary].description = "";
                fmu->fmi3.binaryTypes[iBinary].mimeType = "application/octet-stream";
                fmu->fmi3.binaryTypes[iBinary].maxSize = UINT32_MAX;
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "name", &fmu->fmi3.binaryTypes[iBinary].name, fmu);
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "description", &fmu->fmi3.binaryTypes[iBinary].description, fmu);
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "mimeType", &fmu->fmi3.binaryTypes[iBinary].mimeType, fmu);
                parseUInt32AttributeEzXml(typeElement, "maxSize", &fmu->fmi3.binaryTypes[iBinary].maxSize);
                ++iBinary;
            }
            else if(!strcmp(typeElement->name, "EnumerationType")) {
                fmu->fmi3.enumTypes[iEnum].name = "";
                fmu->fmi3.enumTypes[iEnum].description = "";
                fmu->fmi3.enumTypes[iEnum].quantity = "";
                fmu->fmi3.enumTypes[iEnum].min = -INT64_MAX;
                fmu->fmi3.enumTypes[iEnum].max = INT64_MAX;
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "name", &fmu->fmi3.enumTypes[iEnum].name, fmu);
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "description", &fmu->fmi3.enumTypes[iEnum].description, fmu);
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "quantity", &fmu->fmi3.enumTypes[iEnum].quantity, fmu);
                parseInt64AttributeEzXml(typeElement, "min", &fmu->fmi3.enumTypes[iEnum].min);
                parseInt64AttributeEzXml(typeElement, "max", &fmu->fmi3.enumTypes[iEnum].max);

                //Count number of enumeration items
                fmu->fmi3.enumTypes[iEnum].numberOfItems = 0;
                for(ezxml_t itemElement = typeElement->child; itemElement; itemElement = itemElement->next) {
                    if(!strcmp(itemElement->name, "Item")) {
                        ++fmu->fmi3.enumTypes[iEnum].numberOfItems;
                    }
                }

                //Allocate memory for enumeration items
                if(fmu->fmi3.enumTypes[iEnum].numberOfItems > 0) {
                    fmu->fmi3.enumTypes[iEnum].items = mallocAndRememberPointer(fmu, fmu->fmi3.enumTypes[iEnum].numberOfItems*sizeof(fmi3EnumerationItem));
                }

                //Read data for enumeration items
                int iItem = 0;
                for(ezxml_t itemElement = typeElement->child; itemElement; itemElement = itemElement->next) {
                    if(!strcmp(itemElement->name, "Item")) {
                        parseStringAttributeEzXmlAndRememberPointer(itemElement, "name", &fmu->fmi3.enumTypes[iEnum].items[iItem].name, fmu);
                        parseInt64AttributeEzXml(itemElement, "value", &fmu->fmi3.enumTypes[iEnum].items[iItem].value);
                        parseStringAttributeEzXmlAndRememberPointer(itemElement, "description", &fmu->fmi3.enumTypes[iEnum].items[iItem].description, fmu);
                    }
                    ++iItem;
                }
                ++iEnum;
            }
            else if(!strcmp(typeElement->name, "ClockType")) {
                fmu->fmi3.clockTypes[iClock].name = "";
                fmu->fmi3.clockTypes[iClock].description = "";
                fmu->fmi3.clockTypes[iClock].canBeDeactivated = false;
                fmu->fmi3.clockTypes[iClock].priority = 0;
                fmu->fmi3.clockTypes[iClock].intervalVariability = fmi3IntervalVariabilityFixed;
                fmu->fmi3.clockTypes[iClock].intervalDecimal = FLT_MAX;
                fmu->fmi3.clockTypes[iClock].shiftDecimal = 0;
                fmu->fmi3.clockTypes[iClock].supportsFraction = false;
                fmu->fmi3.clockTypes[iClock].resolution = UINT64_MAX;
                fmu->fmi3.clockTypes[iClock].intervalCounter = UINT64_MAX;
                fmu->fmi3.clockTypes[iClock].shiftCounter = 0;
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "name", &fmu->fmi3.clockTypes[iClock].name, fmu);
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "description", &fmu->fmi3.clockTypes[iClock].description, fmu);
                parseBooleanAttributeEzXml(typeElement, "canBeDeactivated", &fmu->fmi3.clockTypes[iClock].canBeDeactivated);
                parseUInt32AttributeEzXml(typeElement, "priority", &fmu->fmi3.clockTypes[iClock].priority);
                const char* intervalVariability = NULL;
                parseStringAttributeEzXmlAndRememberPointer(typeElement, "intervalVariability", &intervalVariability, fmu);
                if(intervalVariability && !strcmp(intervalVariability, "calculated")) {
                    fmu->fmi3.clockTypes[iClock].intervalVariability = fmi3IntervalVariabilityCalculated;
                }
                else if(intervalVariability && !strcmp(intervalVariability, "changing")) {
                    fmu->fmi3.clockTypes[iClock].intervalVariability = fmi3IntervalVariabilityChanging;
                }
                else if(intervalVariability && !strcmp(intervalVariability, "constant")) {
                    fmu->fmi3.clockTypes[iClock].intervalVariability = fmi3IntervalVariabilityConstant;
                }
                else if(intervalVariability && !strcmp(intervalVariability, "countdown")) {
                    fmu->fmi3.clockTypes[iClock].intervalVariability = fmi3IntervalVariabilityCountdown;
                }
                else if(intervalVariability && !strcmp(intervalVariability, "fixed")) {
                    fmu->fmi3.clockTypes[iClock].intervalVariability = fmi3IntervalVariabilityFixed;
                }
                else if(intervalVariability && !strcmp(intervalVariability, "triggered")) {
                    fmu->fmi3.clockTypes[iClock].intervalVariability = fmi3IntervalVariabilityTriggered;
                }
                else if(intervalVariability && !strcmp(intervalVariability, "tunable")) {
                    fmu->fmi3.clockTypes[iClock].intervalVariability = fmi3IntervalVariabilityTunable;
                }
                else if(intervalVariability) {
                    printf("Unknown interval variability: %s\n", intervalVariability);
                    freeDuplicatedConstChar(intervalVariability);
                    return false;
                }
                freeDuplicatedConstChar(intervalVariability);
                parseFloat32AttributeEzXml(typeElement, "intervalDecimal", &fmu->fmi3.clockTypes[iClock].intervalDecimal);
                parseFloat32AttributeEzXml(typeElement, "shiftDecimal", &fmu->fmi3.clockTypes[iClock].shiftDecimal);
                parseBooleanAttributeEzXml(typeElement, "supportsFraction", &fmu->fmi3.clockTypes[iClock].supportsFraction);
                parseUInt64AttributeEzXml(typeElement, "resolution", &fmu->fmi3.clockTypes[iClock].resolution);
                parseUInt64AttributeEzXml(typeElement, "intervalCounter", &fmu->fmi3.clockTypes[iClock].intervalCounter);
                parseUInt64AttributeEzXml(typeElement, "shiftCounter", &fmu->fmi3.clockTypes[iClock].shiftCounter);
                ++iClock;
            }
        }
    }

    ezxml_t logCategoriesElement = ezxml_child(rootElement, "LogCategories");
    fmu->fmi3.numberOfLogCategories = 0;
    if(logCategoriesElement) {
        //Count log categories
        for(ezxml_t logCategoryElement = logCategoriesElement->child; logCategoryElement; logCategoryElement = logCategoryElement->next) {
            ++fmu->fmi3.numberOfLogCategories;
        }

        //Allocate memory for log categories
        if(fmu->fmi3.numberOfLogCategories > 0) {
            fmu->fmi3.logCategories = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfLogCategories*sizeof(fmi3LogCategory));
        }

        //Read log categories
        int i=0;
        for(ezxml_t logCategoryElement = logCategoriesElement->child; logCategoryElement; logCategoryElement = logCategoryElement->next) {
            parseStringAttributeEzXmlAndRememberPointer(logCategoryElement, "name", &fmu->fmi3.logCategories[i].name, fmu);
            parseStringAttributeEzXmlAndRememberPointer(logCategoryElement, "description", &fmu->fmi3.logCategories[i].description, fmu);
            ++i;
        }
    }

    ezxml_t defaultExperimentElement = ezxml_child(rootElement, "DefaultExperiment");
    if(defaultExperimentElement) {
        fmu->fmi3.defaultStartTimeDefined = parseFloat64AttributeEzXml(defaultExperimentElement, "startTime", &fmu->fmi3.defaultStartTime);
        fmu->fmi3.defaultStopTimeDefined =  parseFloat64AttributeEzXml(defaultExperimentElement, "stopTime",  &fmu->fmi3.defaultStopTime);
        fmu->fmi3.defaultToleranceDefined = parseFloat64AttributeEzXml(defaultExperimentElement, "tolerance", &fmu->fmi3.defaultTolerance);
        fmu->fmi3.defaultStepSizeDefined =  parseFloat64AttributeEzXml(defaultExperimentElement, "stepSize",  &fmu->fmi3.defaultStepSize);
    }

    ezxml_t modelVariablesElement = ezxml_child(rootElement, "ModelVariables");
    if(modelVariablesElement) {
        for(ezxml_t varElement = modelVariablesElement->child; varElement; varElement = varElement->ordered) {
            fmi3VariableHandle var;
            var.name = NULL;
            var.description = NULL;
            var.quantity = NULL;
            var.unit = NULL;
            var.displayUnit = NULL;
            var.canHandleMultipleSetPerTimeInstant = false; //Default value if attribute not defined
            var.startBinary = NULL;
            var.intermediateUpdate = false;

            parseStringAttributeEzXmlAndRememberPointer(varElement, "name", &var.name, fmu);
            parseInt64AttributeEzXml(varElement, "valueReference", &var.valueReference);
            parseStringAttributeEzXmlAndRememberPointer(varElement, "description", &var.description, fmu);
            parseBooleanAttributeEzXml(varElement, "canHandleMultipleSetPerTimeInstant", &var.canHandleMultipleSetPerTimeInstant);
            parseBooleanAttributeEzXml(varElement, "intermediateUpdate", &var.intermediateUpdate);
            parseUInt32AttributeEzXml(varElement, "previous", &var.previous);
            parseStringAttributeEzXmlAndRememberPointer(varElement, "declaredType", &var.declaredType, fmu);

            var.numberOfClocks = 0;
            const char* clocks = NULL;
            if (parseStringAttributeEzXmlAndRememberPointer(varElement, "clocks", &clocks, fmu)) {
                // Count number of clocks
                if(clocks[0]) {
                    var.numberOfClocks = 1;
                }
                for(int i=0; clocks[i]; ++i) {
                    if(clocks[i] == ' ') {
                        ++var.numberOfClocks;
                    }
                }


                //Allocate memory for clocks
                if(var.numberOfClocks > 0) {
                    var.clocks = mallocAndRememberPointer(fmu, var.numberOfClocks*sizeof(int));
                }

                //Read clocks
                char* mutable_clocks = _strdup(clocks);
                const char* delim = " ";
                for(int i=0; i<var.numberOfClocks; ++i) {
                    if(i == 0) {
                        var.clocks[i] = atoi(strtok(mutable_clocks, delim));
                    }
                    else {
                        var.clocks[i] = atoi(strtok(NULL, delim));
                    }
                }

                freeDuplicatedConstChar(mutable_clocks);
                freeDuplicatedConstChar(clocks);
            }

            var.hasStartValue = false;

            //Figure out data type
            if(!strcmp(varElement->name, "Float64")) {
                var.datatype = fmi3DataTypeFloat64;
                fmu->fmi3.hasFloat64Variables = true;
                if(parseFloat64AttributeEzXml(varElement, "start", &var.startFloat64)) {
                    var.hasStartValue = true;
                }
            }
            else if(!strcmp(varElement->name, "Float32")) {
                var.datatype = fmi3DataTypeFloat32;
                fmu->fmi3.hasFloat32Variables = true;
                if(parseFloat32AttributeEzXml(varElement, "start", &var.startFloat32)) {
                    var.hasStartValue = true;
                }
            }
            else if(!strcmp(varElement->name, "Int64")) {
                var.datatype = fmi3DataTypeInt64;
                fmu->fmi3.hasInt64Variables = true;
                if(parseInt64AttributeEzXml(varElement, "start", &var.startInt64)) {
                    var.hasStartValue = true;
                }
            }
            else if(!strcmp(varElement->name, "Int32")) {
                var.datatype = fmi3DataTypeInt32;
                fmu->fmi3.hasInt32Variables = true;
                if(parseInt32AttributeEzXml(varElement, "start", &var.startInt32)) {
                    var.hasStartValue = true;
                }
            }
            else if(!strcmp(varElement->name, "Int16")) {
                var.datatype = fmi3DataTypeInt16;
                fmu->fmi3.hasInt16Variables = true;
                if(parseInt16AttributeEzXml(varElement, "start", &var.startInt16)) {
                    var.hasStartValue = true;
                }
            }
            else if(!strcmp(varElement->name, "Int8")) {
                var.datatype = fmi3DataTypeInt8;
                fmu->fmi3.hasInt8Variables = true;
                if(parseInt8AttributeEzXml(varElement, "start", &var.startInt8)) {
                    var.hasStartValue = true;
                }
            }
            else if(!strcmp(varElement->name, "UInt64")) {
                var.datatype = fmi3DataTypeUInt64;
                fmu->fmi3.hasUInt64Variables = true;
                if(parseUInt64AttributeEzXml(varElement, "start", &var.startUInt64)) {
                    var.hasStartValue = true;
                }
            }
            else if(!strcmp(varElement->name, "UInt32")) {
                var.datatype = fmi3DataTypeUInt32;
                fmu->fmi3.hasUInt32Variables = true;
                if(parseUInt32AttributeEzXml(varElement, "start", &var.startUInt32)) {
                    var.hasStartValue = true;
                }
            }
            else if(!strcmp(varElement->name, "UInt16")) {
                var.datatype = fmi3DataTypeUInt16;
                fmu->fmi3.hasUInt16Variables = true;
                if(parseUInt16AttributeEzXml(varElement, "start", &var.startUInt16)) {
                    var.hasStartValue = true;
                }
            }
            else if(!strcmp(varElement->name, "UInt8")) {
                var.datatype = fmi3DataTypeUInt8;
                fmu->fmi3.hasUInt8Variables = true;
                if(parseUInt8AttributeEzXml(varElement, "start", &var.startUInt8)) {
                    var.hasStartValue = true;
                }
            }
            else if(!strcmp(varElement->name, "Boolean")) {
                var.datatype = fmi3DataTypeBoolean;
                fmu->fmi3.hasBooleanVariables = true;
                if(parseBooleanAttributeEzXml(varElement, "start", &var.startBoolean)) {
                    var.hasStartValue = true;
                }
            }
            else if(!strcmp(varElement->name, "String")) {
                var.datatype = fmi3DataTypeString;
                fmu->fmi3.hasStringVariables = true;
                var.startString = "";
                if(parseStringAttributeEzXmlAndRememberPointer(varElement, "start", &var.startString, fmu)) {
                    var.hasStartValue = true;
                }
            }
            else if(!strcmp(varElement->name, "Binary")) {
                var.datatype = fmi3DataTypeBinary;
                fmu->fmi3.hasBinaryVariables = true;
                if(parseUInt8AttributeEzXml(varElement, "start", var.startBinary)) {
                    var.hasStartValue = true;
                }
            }
            else if(!strcmp(varElement->name, "Enumeration")) {
                var.datatype = fmi3DataTypeEnumeration;
                fmu->fmi3.hasEnumerationVariables = true;
                if(parseInt64AttributeEzXml(varElement, "start", &var.startEnumeration)) {
                    var.hasStartValue = true;
                }
            }
            else if(!strcmp(varElement->name, "Clock")) {
                var.datatype = fmi3DataTypeClock;
                fmu->fmi3.hasClockVariables = true;
                if(parseBooleanAttributeEzXml(varElement, "start", &var.startClock)) {
                    var.hasStartValue = true;
                }
            }

            var.causality = fmi3CausalityLocal;
            const char* causality = NULL;
            if(parseStringAttributeEzXml(varElement, "causality", &causality)) {
                if(!strcmp(causality, "parameter")) {
                    var.causality = fmi3CausalityParameter;
                }
                else if(!strcmp(causality, "calculatedParameter")) {
                    var.causality = fmi3CausalityCalculatedParameter;
                }
                else if(!strcmp(causality, "input")) {
                    var.causality = fmi3CausalityInput;
                }
                else if(!strcmp(causality, "output")) {
                    var.causality = fmi3CausalityOutput;
                }
                else if(!strcmp(causality, "local")) {
                    var.causality = fmi3CausalityLocal;
                }
                else if(!strcmp(causality, "independent")) {
                    var.causality = fmi3CausalityIndependent;
                }
                else if(!strcmp(causality, "structuralParameter")) {
                    var.causality = fmi3CausalityStructuralParameter;
                }
                else {
                    printf("Unknown causality: %s\n", causality);
                    if(var.numberOfClocks > 0) {
                        free(var.clocks);
                    }
                    freeDuplicatedConstChar(causality);
                    return false;
                }
                freeDuplicatedConstChar(causality);
            }


            const char* variability = NULL;
            if(var.datatype == fmi3DataTypeFloat64 || var.datatype == fmi3DataTypeFloat32) {
                var.variability = fmi3VariabilityContinuous;
            }
            else {
                var.variability = fmi3VariabilityDiscrete;
            }
            if(parseStringAttributeEzXml(varElement, "variability", &variability)) {
                if(variability && !strcmp(variability, "constant")) {
                    var.variability = fmi3VariabilityConstant;
                }
                else if(variability && !strcmp(variability, "fixed")) {
                    var.variability = fmi3VariabilityFixed;
                }
                else if(variability && !strcmp(variability, "tunable")) {
                    var.variability = fmi3VariabilityTunable;
                }
                else if(variability && !strcmp(variability, "discrete")) {
                    var.variability = fmi3VariabilityDiscrete;
                }
                else if(variability && !strcmp(variability, "continuous")) {
                    var.variability = fmi3VariabilityContinuous;
                }
                else if(variability) {
                    printf("Unknown variability: %s\n", variability);
                    freeDuplicatedConstChar(variability);
                    return false;
                }
                freeDuplicatedConstChar(variability);
            }

            //Parse arguments common to all except clock type
            if(var.datatype == fmi3DataTypeFloat64 ||
               var.datatype == fmi3DataTypeFloat32 ||
               var.datatype == fmi3DataTypeInt64 ||
               var.datatype == fmi3DataTypeInt32 ||
               var.datatype == fmi3DataTypeInt16 ||
               var.datatype == fmi3DataTypeInt8 ||
               var.datatype == fmi3DataTypeUInt64 ||
               var.datatype == fmi3DataTypeUInt32 ||
               var.datatype == fmi3DataTypeUInt16 ||
               var.datatype == fmi3DataTypeUInt8 ||
               var.datatype == fmi3DataTypeBoolean ||
               var.datatype == fmi3DataTypeBinary ||
               var.datatype == fmi3DataTypeEnumeration) {
                const char* initial = NULL;
                var.initial = fmi3InitialUndefined;
                if(var.variability == fmi3VariabilityConstant && (var.causality == fmi3CausalityOutput || var.causality == fmi3CausalityLocal)) {
                    var.initial = fmi3InitialExact;
                }
                else if(var.causality == fmi3CausalityOutput || var.causality == fmi3CausalityLocal) {
                    var.initial = fmi3InitialCalculated;
                }
                else if (var.causality == fmi3CausalityStructuralParameter || var.causality == fmi3CausalityParameter) {
                    var.initial = fmi3InitialExact;
                }
                else if(var.causality == fmi3CausalityCalculatedParameter) {
                    var.initial = fmi3InitialCalculated;
                }
                else if(var.causality == fmi3CausalityInput) {
                    var.initial = fmi3InitialExact;
                }
                parseStringAttributeEzXml(varElement, "initial", &initial);
                if(initial && !strcmp(initial, "approx")) {
                    var.initial = fmi3InitialApprox;
                }
                else if(initial && !strcmp(initial, "exact")) {
                    var.initial = fmi3InitialExact;
                }
                else if(initial && !strcmp(initial, "calculated")) {
                    var.initial = fmi3InitialCalculated;
                }
                else if(initial) {
                    printf("Unknown initial: %s\n", initial);
                    freeDuplicatedConstChar(initial);
                    return false;
                }
                freeDuplicatedConstChar(initial);
            }

            //Parse arguments common to float, int and enumeration
            if(var.datatype == fmi3DataTypeFloat64 ||
               var.datatype == fmi3DataTypeFloat32 ||
               var.datatype == fmi3DataTypeInt64 ||
               var.datatype == fmi3DataTypeInt32 ||
               var.datatype == fmi3DataTypeInt16 ||
               var.datatype == fmi3DataTypeInt8 ||
               var.datatype == fmi3DataTypeUInt64 ||
               var.datatype == fmi3DataTypeUInt32 ||
               var.datatype == fmi3DataTypeUInt16 ||
               var.datatype == fmi3DataTypeUInt8 ||
               var.datatype == fmi3DataTypeEnumeration) {
                parseStringAttributeEzXmlAndRememberPointer(varElement,  "quantity", &var.quantity, fmu);
                parseFloat64AttributeEzXml(varElement,  "min", &var.min);
                parseFloat64AttributeEzXml(varElement,  "max", &var.max);
            }

            //Parse arguments only in float type
            if(var.datatype == fmi3DataTypeFloat64 ||
               var.datatype == fmi3DataTypeFloat32) {
                parseStringAttributeEzXmlAndRememberPointer(varElement,  "unit", &var.unit, fmu);
                parseStringAttributeEzXmlAndRememberPointer(varElement,  "displayUnit", &var.displayUnit, fmu);
                parseBooleanAttributeEzXml(varElement, "relativeQuantity", &var.relativeQuantity);
                parseBooleanAttributeEzXml(varElement, "unbounded", &var.unbounded);
                parseFloat64AttributeEzXml(varElement,  "nominal", &var.nominal);
                parseUInt32AttributeEzXml(varElement, "derivative", &var.derivative);
                parseBooleanAttributeEzXml(varElement, "reinit", &var.reInit);
            }

            //Parse arguments only in binary type
            if(var.datatype == fmi3DataTypeBinary) {
                parseStringAttributeEzXmlAndRememberPointer(varElement, "mimeType", &var.mimeType, fmu);
                parseInt32AttributeEzXml(varElement, "maxSize", &var.maxSize);
            }

            if(var.datatype == fmi3DataTypeClock) {
                parseBooleanAttributeEzXml(varElement, "canBeDeactivated", &var.canBeDeactivated);
                parseInt32AttributeEzXml(varElement, "priority", &var.priority);
                parseFloat64AttributeEzXml(varElement, "intervalDecimal", &var.intervalDecimal);
                parseFloat64AttributeEzXml(varElement, "shiftDecimal", &var.shiftDecimal);
                parseBooleanAttributeEzXml(varElement, "supportsFraction", &var.supportsFraction);
                parseInt64AttributeEzXml(varElement, "resolution", &var.resolution);
                parseInt64AttributeEzXml(varElement, "intervalCounter", &var.intervalCounter);
                parseInt64AttributeEzXml(varElement, "shiftCounter", &var.shiftCounter);
                const char* intervalVariability = NULL;
                parseStringAttributeEzXmlAndRememberPointer(varElement, "intervalVariability", &intervalVariability, fmu);
                if(intervalVariability && !strcmp(intervalVariability, "calculated")) {
                    var.intervalVariability = fmi3IntervalVariabilityCalculated;
                }
                else if(intervalVariability && !strcmp(intervalVariability, "changing")) {
                    var.intervalVariability = fmi3IntervalVariabilityChanging;
                }
                else if(intervalVariability && !strcmp(intervalVariability, "constant")) {
                    var.intervalVariability = fmi3IntervalVariabilityConstant;
                }
                else if(intervalVariability && !strcmp(intervalVariability, "countdown")) {
                    var.intervalVariability = fmi3IntervalVariabilityCountdown;
                }
                else if(intervalVariability && !strcmp(intervalVariability, "fixed")) {
                    var.intervalVariability = fmi3IntervalVariabilityFixed;
                }
                else if(intervalVariability && !strcmp(intervalVariability, "triggered")) {
                    var.intervalVariability = fmi3IntervalVariabilityTriggered;
                }
                else if(intervalVariability && !strcmp(intervalVariability, "tunable")) {
                    var.intervalVariability = fmi3IntervalVariabilityTunable;
                }
                else if(intervalVariability) {
                    printf("Unknown interval variability: %s\n", intervalVariability);
                    freeDuplicatedConstChar(intervalVariability);
                    return false;
                }
                freeDuplicatedConstChar(intervalVariability);
            }

            if(fmu->fmi3.numberOfVariables >= fmu->fmi3.variablesSize) {
                fmu->fmi3.variablesSize *= 2;
                fmu->fmi3.variables = reallocAndRememberPointer(fmu, fmu->fmi3.variables, fmu->fmi3.variablesSize*sizeof(fmi3VariableHandle));
            }



            fmu->fmi3.variables[fmu->fmi3.numberOfVariables] = var;
            fmu->fmi3.numberOfVariables++;
            if(var.numberOfClocks > 0) {
                free(var.clocks);
            }
        }
    }

    ezxml_t modelStructureElement = ezxml_child(rootElement, "ModelStructure");
    fmu->fmi3.numberOfOutputs = 0;
    if(modelStructureElement) {
        //Count each element type
        ezxml_t outputElement = ezxml_child(modelStructureElement, "Output");
        for(;outputElement;outputElement = outputElement->next) {
            ++fmu->fmi3.numberOfOutputs;
        }
        ezxml_t continuousStateDerElement = ezxml_child(modelStructureElement, "ContinuousStateDerivative");
        for(;continuousStateDerElement;continuousStateDerElement = continuousStateDerElement->next) {
            ++fmu->fmi3.numberOfContinuousStateDerivatives;
        }
        ezxml_t clockedStateElement = ezxml_child(modelStructureElement, "ClockedState");
        for(;clockedStateElement;clockedStateElement = clockedStateElement->next) {
            ++fmu->fmi3.numberOfClockedStates;
        }
        ezxml_t initialUnknownElement = ezxml_child(modelStructureElement, "InitialUnknown");
        for(;initialUnknownElement;initialUnknownElement = initialUnknownElement->next) {
            ++fmu->fmi3.numberOfInitialUnknowns;
        }
        ezxml_t eventIndicatorElement = ezxml_child(modelStructureElement, "EventIndicator");
        for(;eventIndicatorElement;eventIndicatorElement = eventIndicatorElement->next) {
            ++fmu->fmi3.numberOfEventIndicators;
        }

        //Allocate memory for each element type
        fmu->fmi3.outputs = NULL;
        fmu->fmi3.continuousStateDerivatives = NULL;
        fmu->fmi3.clockedStates = NULL;
        fmu->fmi3.initialUnknowns = NULL;
        fmu->fmi3.eventIndicators = NULL;
        if(fmu->fmi3.numberOfOutputs > 0) {
            fmu->fmi3.outputs = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfOutputs*sizeof(fmi3ModelStructureElement));
        }
        fmu->fmi3.continuousStateDerivatives = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfContinuousStateDerivatives*sizeof(fmi3ModelStructureElement));
        fmu->fmi3.clockedStates = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfClockedStates*sizeof(fmi3ModelStructureElement));
        fmu->fmi3.initialUnknowns = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfInitialUnknowns*sizeof(fmi3ModelStructureElement));
        fmu->fmi3.eventIndicators = mallocAndRememberPointer(fmu, fmu->fmi3.numberOfEventIndicators*sizeof(fmi3ModelStructureElement));

        //Read outputs
        int i=0;
        outputElement = ezxml_child(modelStructureElement, "Output");
        for(;outputElement;outputElement = outputElement->next) {
            if(!parseModelStructureElement(fmu, &fmu->fmi3.outputs[i], &outputElement)) {
                return false;
            }
            ++i;
        }

        //Read continuous state derivatives
        i=0;
        continuousStateDerElement = ezxml_child(modelStructureElement, "ContinuousStateDerivative");
        for(;continuousStateDerElement;continuousStateDerElement = continuousStateDerElement->next) {
            if(!parseModelStructureElement(fmu, &fmu->fmi3.continuousStateDerivatives[i], &continuousStateDerElement)) {
                return false;
            }
            ++i;
        }

        //Read clocked states
        i=0;
        clockedStateElement = ezxml_child(modelStructureElement, "ClockedState");
        for(;clockedStateElement;clockedStateElement = clockedStateElement->next) {
            if(!parseModelStructureElement(fmu, &fmu->fmi3.clockedStates[i], &clockedStateElement)) {
                return false;
            }
            ++i;
        }

        //Read initial unknowns
        i=0;
        initialUnknownElement = ezxml_child(modelStructureElement, "InitialUnknown");
        for(;initialUnknownElement;initialUnknownElement = initialUnknownElement->next) {
            if(!parseModelStructureElement(fmu, &fmu->fmi3.initialUnknowns[i], &initialUnknownElement)) {
                return false;
            }
            ++i;
        }

        //Read event indicators
        i=0;
        eventIndicatorElement = ezxml_child(modelStructureElement, "EventIndicator");
        for(;eventIndicatorElement;eventIndicatorElement = eventIndicatorElement->next) {
            if(!parseModelStructureElement(fmu, &fmu->fmi3.eventIndicators[i], &eventIndicatorElement)) {
                return false;
            }
            ++i;
        }
    }

    ezxml_free(rootElement);

    chdir(cwd);

    return true;
}


//! @brief Loads all DLL functions for FMI 1
//! @param fmu FMU handle
//! @returns True if load was successful
bool loadFunctionsFmi1(fmiHandle *fmu)
{
    TRACEFUNC

    if (fmu->dll != NULL) {
#ifdef _WIN32
        FreeLibrary(fmu->dll);
#else
        dlclose(fmu->dll);
#endif
    }

    char cwd[FILENAME_MAX];
#ifdef _WIN32
    _getcwd(cwd, sizeof(char)*FILENAME_MAX);
#else
    getcwd(cwd, sizeof(char)*FILENAME_MAX);
#endif

    char dllPath[FILENAME_MAX] = {0};
    size_t max_num_chars_to_append = sizeof(dllPath)-1;
    strncat(dllPath, fmu->unzippedLocation, max_num_chars_to_append);

    // Append on the form: /binaries/win64/
    max_num_chars_to_append = sizeof(dllPath)-strlen(dllPath)-1;
    strncat(dllPath, dirsep_str "binaries" dirsep_str fmi12_system_str bits_str dirsep_str, max_num_chars_to_append);

    max_num_chars_to_append = sizeof(dllPath)-strlen(dllPath)-1;
    strncat(dllPath, fmu->fmi1.modelIdentifier, max_num_chars_to_append);
    max_num_chars_to_append = sizeof(dllPath)-strlen(dllPath)-1;
    strncat(dllPath, dllext_str, max_num_chars_to_append);

#ifdef _WIN32
    char dllDirectory[FILENAME_MAX] = {0};
    max_num_chars_to_append = sizeof(dllDirectory)-1;
    strncat(dllDirectory, fmu->unzippedLocation, max_num_chars_to_append);
    max_num_chars_to_append = sizeof(dllDirectory)-strlen(dllDirectory)-1;
    strncat(dllDirectory, dirsep_str "binaries" dirsep_str fmi12_system_str bits_str dirsep_str, max_num_chars_to_append);
    BOOL success = SetDllDirectoryA(dllDirectory);
    if (!success) {
        fprintf(stderr, "Loading DLL %s failed:\nFailed to set DLL directory %s", dllPath, dllDirectory);
        return false;
    }

    HINSTANCE dll = LoadLibraryA(dllPath);
    if(NULL == dll) {
        DWORD error = GetLastError();
        LPSTR message = NULL;
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&message, 0, NULL);
        fprintf(stderr, "Failed to load DLL %s:\n%s", dllPath, message);
        LocalFree(message);
        return false;
    }
#else
    char cmd[FILENAME_MAX] = {0};
    strcat(cmd, "chmod +x ");
    strcat(cmd, dllPath);
    system(cmd);

    void* dll = dlopen(dllPath, RTLD_NOW|RTLD_LOCAL);
    if(NULL == dll) {
        printf("Loading shared object failed: %s (%s)\n",dllPath, dlerror());
        return false;
    }
#endif

    fmu->dll = dll;

    bool ok = true;

    // The temp buffer is used for concatenating modelnName_functionName to avoid memory if such a buffer would be allocated inside getFunctionName
    char tmpbuff[FILENAME_MAX];

    //Load all common functions
    fmu->fmi1.getVersion = (fmiGetVersion_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiGetVersion", tmpbuff), &ok);
    fmu->fmi1.setDebugLogging = (fmiSetDebugLogging_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiSetDebugLogging", tmpbuff), &ok);
    fmu->fmi1.getReal = (fmiGetReal_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiGetReal", tmpbuff), &ok);
    fmu->fmi1.setReal = (fmiSetReal_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiSetReal", tmpbuff), &ok);
    fmu->fmi1.getInteger = (fmiGetInteger_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiGetInteger", tmpbuff), &ok);
    fmu->fmi1.setInteger = (fmiSetInteger_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiSetInteger", tmpbuff), &ok);
    fmu->fmi1.getBoolean = (fmiGetBoolean_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiGetBoolean", tmpbuff), &ok);
    fmu->fmi1.setBoolean = (fmiSetBoolean_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiSetBoolean", tmpbuff), &ok);
    fmu->fmi1.getString = (fmiGetString_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiGetString", tmpbuff), &ok);
    fmu->fmi1.setString = (fmiSetString_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiSetString", tmpbuff), &ok);

    if(fmu->fmi1.type == fmi1ModelExchange) {
        //Load model exchange functions
        fmu->fmi1.instantiateModel = (fmiInstantiateModel_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelIdentifier, "fmiInstantiateModel", tmpbuff), &ok);
        fmu->fmi1.freeModelInstance = (fmiFreeModelInstance_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelIdentifier, "fmiFreeModelInstance", tmpbuff), &ok);
        fmu->fmi1.initialize = (fmiInitialize_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelIdentifier, "fmiInitialize", tmpbuff), &ok);
        fmu->fmi1.getDerivatives = (fmiGetDerivatives_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelIdentifier, "fmiGetDerivatives", tmpbuff), &ok);
        fmu->fmi1.terminate = (fmiTerminate_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelIdentifier, "fmiTerminate", tmpbuff), &ok);
        fmu->fmi1.setTime = (fmiSetTime_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelIdentifier, "fmiSetTime", tmpbuff), &ok);
        fmu->fmi1.getModelTypesPlatform = (fmiGetModelTypesPlatform_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelIdentifier, "fmiGetModelTypesPlatform", tmpbuff), &ok);
        fmu->fmi1.setContinuousStates = (fmiSetContinuousStates_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelIdentifier, "fmiSetContinuousStates", tmpbuff), &ok);
        fmu->fmi1.completedIntegratorStep = (fmiCompletedIntegratorStep_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelIdentifier, "fmiCompletedIntegratorStep", tmpbuff), &ok);
        fmu->fmi1.getEventIndicators = (fmiGetEventIndicators_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelIdentifier, "fmiGetEventIndicators", tmpbuff), &ok);
        fmu->fmi1.eventUpdate = (fmiEventUpdate_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelIdentifier, "fmiEventUpdate", tmpbuff), &ok);
        fmu->fmi1.getContinuousStates = (fmiGetContinuousStates_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelIdentifier, "fmiGetContinuousStates", tmpbuff), &ok);
        fmu->fmi1.getNominalContinuousStates = (fmiGetNominalContinuousStates_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelIdentifier, "fmiGetNominalContinuousStates", tmpbuff), &ok);
        fmu->fmi1.getStateValueReferences = (fmiGetStateValueReferences_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelIdentifier, "fmiGetStateValueReferences", tmpbuff), &ok);
    }

    if(fmu->fmi1.type == fmi1CoSimulationStandAlone || fmu->fmi1.type == fmi1CoSimulationTool) {
        //Load all co-simulation functions
        fmu->fmi1.getTypesPlatform = (fmiGetTypesPlatform_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiGetTypesPlatform", tmpbuff), &ok);
        fmu->fmi1.instantiateSlave = (fmiInstantiateSlave_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiInstantiateSlave", tmpbuff), &ok);
        fmu->fmi1.initializeSlave = (fmiInitializeSlave_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiInitializeSlave", tmpbuff), &ok);
        fmu->fmi1.terminateSlave = (fmiTerminateSlave_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiTerminateSlave", tmpbuff), &ok);
        fmu->fmi1.resetSlave =  (fmiResetSlave_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiResetSlave", tmpbuff), &ok);
        fmu->fmi1.freeSlaveInstance = (fmiFreeSlaveInstance_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiFreeSlaveInstance", tmpbuff), &ok);
        fmu->fmi1.setRealInputDerivatives = (fmiSetRealInputDerivatives_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiSetRealInputDerivatives", tmpbuff), &ok);
        fmu->fmi1.getRealOutputDerivatives = (fmiGetRealOutputDerivatives_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiGetRealOutputDerivatives", tmpbuff), &ok);
        fmu->fmi1.doStep = (fmiDoStep_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiDoStep", tmpbuff), &ok);
        fmu->fmi1.cancelStep = (fmiCancelStep_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiCancelStep", tmpbuff), &ok);
        fmu->fmi1.getStatus = (fmiGetStatus_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiGetStatus", tmpbuff), &ok);
        fmu->fmi1.getRealStatus = (fmiGetRealStatus_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiGetRealStatus", tmpbuff), &ok);
        fmu->fmi1.getIntegerStatus = (fmiGetIntegerStatus_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiGetIntegerStatus", tmpbuff), &ok);
        fmu->fmi1.getBooleanStatus = (fmiGetBooleanStatus_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiGetBooleanStatus", tmpbuff), &ok);
        fmu->fmi1.getStringStatus = (fmiGetStringStatus_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelIdentifier, "fmiGetStringStatus", tmpbuff), &ok);
    }

    chdir(cwd);

    return ok;
}


//! @brief Loads all DLL functions for FMI 2
//! @param fmu FMU handle
//! @returns True if load was successful
bool loadFunctionsFmi2(fmiHandle *fmu, fmi2Type fmuType)
{
    TRACEFUNC

    if (fmu->dll != NULL) {
#ifdef _WIN32
        FreeLibrary(fmu->dll);
#else
        dlclose(fmu->dll);
#endif
    }

    char cwd[FILENAME_MAX];
#ifdef _WIN32
    _getcwd(cwd, sizeof(char)*FILENAME_MAX);
#else
    getcwd(cwd, sizeof(char)*FILENAME_MAX);
#endif

    char dllPath[FILENAME_MAX] = {0};
    size_t max_num_chars_to_append = sizeof(dllPath)-1;
    strncat(dllPath, fmu->unzippedLocation, max_num_chars_to_append);

    // Append on the form: /binaries/win64/
    max_num_chars_to_append = sizeof(dllPath)-strlen(dllPath)-1;
    strncat(dllPath, dirsep_str "binaries" dirsep_str fmi12_system_str bits_str dirsep_str, max_num_chars_to_append);

    max_num_chars_to_append = sizeof(dllPath)-strlen(dllPath)-1;
    if(fmuType == fmi2CoSimulation) {
        strncat(dllPath, fmu->fmi2.cs.modelIdentifier, max_num_chars_to_append);
    }
    else {
        strncat(dllPath, fmu->fmi2.me.modelIdentifier, max_num_chars_to_append);
    }

    max_num_chars_to_append = sizeof(dllPath)-strlen(dllPath)-1;
    strncat(dllPath, dllext_str, max_num_chars_to_append);

#ifdef _WIN32
    char dllDirectory[FILENAME_MAX] = {0};
    max_num_chars_to_append = sizeof(dllDirectory)-1;
    strncat(dllDirectory, fmu->unzippedLocation, max_num_chars_to_append);
    max_num_chars_to_append = sizeof(dllDirectory)-strlen(dllDirectory)-1;
    strncat(dllDirectory, dirsep_str "binaries" dirsep_str fmi12_system_str bits_str dirsep_str, max_num_chars_to_append);
    BOOL success = SetDllDirectoryA(dllDirectory);
    if (!success) {
        fprintf(stderr, "Loading DLL %s failed:\nFailed to set DLL directory %s", dllPath, dllDirectory);
        return false;
    }

    HINSTANCE dll = LoadLibraryA(dllPath);
    if(NULL == dll) {
        DWORD error = GetLastError();
        LPSTR message = NULL;
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&message, 0, NULL);
        fprintf(stderr, "Failed to load DLL %s:\n%s", dllPath, message);
        LocalFree(message);
        return false;
    }
#else
    char cmd[FILENAME_MAX] = {0};
    strcat(cmd, "chmod +x ");
    strcat(cmd, dllPath);
    system(cmd);

    void *dll = dlopen(dllPath, RTLD_NOW | RTLD_LOCAL);
    if (NULL == dll) {
        printf("Loading shared object failed: %s (%s)\n", dllPath, dlerror());
        return false;
    }
#endif

    fmu->dll = dll;

    bool ok = true;

    //Load all common functions
    fmu->fmi2.getVersion = (fmi2GetVersion_t)loadDllFunction(dll, "fmi2GetVersion", &ok);
    fmu->fmi2.getTypesPlatform = (fmi2GetTypesPlatform_t)loadDllFunction(dll, "fmi2GetTypesPlatform", &ok);
    fmu->fmi2.setDebugLogging = (fmi2SetDebugLogging_t)loadDllFunction(dll, "fmi2SetDebugLogging", &ok);
    fmu->fmi2.instantiate = (fmi2Instantiate_t)loadDllFunction(dll, "fmi2Instantiate", &ok);
    fmu->fmi2.freeInstance = (fmi2FreeInstance_t)loadDllFunction(dll, "fmi2FreeInstance", &ok);
    fmu->fmi2.setupExperiment = (fmi2SetupExperiment_t)loadDllFunction(dll, "fmi2SetupExperiment", &ok);
    fmu->fmi2.enterInitializationMode = (fmi2EnterInitializationMode_t)loadDllFunction(dll, "fmi2EnterInitializationMode", &ok);
    fmu->fmi2.exitInitializationMode = (fmi2ExitInitializationMode_t)loadDllFunction(dll, "fmi2ExitInitializationMode", &ok);
    fmu->fmi2.terminate = (fmi2Terminate_t)loadDllFunction(dll, "fmi2Terminate", &ok);
    fmu->fmi2.reset = (fmi2Reset_t)loadDllFunction(dll,"fmi2Reset", &ok);
    fmu->fmi2.getReal = (fmi2GetReal_t)loadDllFunction(dll, "fmi2GetReal", &ok);
    fmu->fmi2.setReal = (fmi2SetReal_t)loadDllFunction(dll, "fmi2SetReal", &ok);
    fmu->fmi2.getInteger = (fmi2GetInteger_t)loadDllFunction(dll, "fmi2GetInteger", &ok);
    fmu->fmi2.setInteger = (fmi2SetInteger_t)loadDllFunction(dll, "fmi2SetInteger", &ok);
    fmu->fmi2.getBoolean = (fmi2GetBoolean_t)loadDllFunction(dll, "fmi2GetBoolean", &ok);
    fmu->fmi2.setBoolean = (fmi2SetBoolean_t)loadDllFunction(dll, "fmi2SetBoolean", &ok);
    fmu->fmi2.getString = (fmi2GetString_t)loadDllFunction(dll, "fmi2GetString", &ok);
    fmu->fmi2.setString = (fmi2SetString_t)loadDllFunction(dll, "fmi2SetString", &ok);
    fmu->fmi2.getFMUstate = (fmi2GetFMUstate_t)loadDllFunction(dll, "fmi2GetFMUstate", &ok);
    fmu->fmi2.setFMUstate = (fmi2SetFMUstate_t)loadDllFunction(dll, "fmi2SetFMUstate", &ok);
    fmu->fmi2.freeFMUstate = (fmi2FreeFMUstate_t)loadDllFunction(dll, "fmi2FreeFMUstate", &ok);
    fmu->fmi2.serializedFMUstateSize = (fmi2SerializedFMUstateSize_t)loadDllFunction(dll, "fmi2SerializedFMUstateSize", &ok);
    fmu->fmi2.serializeFMUstate = (fmi2SerializeFMUstate_t)loadDllFunction(dll, "fmi2SerializeFMUstate", &ok);
    fmu->fmi2.deSerializeFMUstate = (fmi2DeSerializeFMUstate_t)loadDllFunction(dll, "fmi2DeSerializeFMUstate", &ok);
    fmu->fmi2.getDirectionalDerivative = (fmi2GetDirectionalDerivative_t)loadDllFunction(dll, "fmi2GetDirectionalDerivative", &ok);

    if(fmuType == fmi2CoSimulation) {
        //Load co-simulation specific functions
        fmu->fmi2.setRealInputDerivatives = (fmi2SetRealInputDerivatives_t)loadDllFunction(dll, "fmi2SetRealInputDerivatives", &ok);
        fmu->fmi2.getRealOutputDerivatives = (fmi2GetRealOutputDerivatives_t)loadDllFunction(dll, "fmi2GetRealOutputDerivatives", &ok);
        fmu->fmi2.doStep = (fmi2DoStep_t)loadDllFunction(dll,"fmi2DoStep", &ok);
        fmu->fmi2.cancelStep = (fmi2CancelStep_t)loadDllFunction(dll, "fmi2CancelStep", &ok);
        fmu->fmi2.getStatus = (fmi2GetStatus_t)loadDllFunction(dll, "fmi2GetStatus", &ok);
        fmu->fmi2.getRealStatus = (fmi2GetRealStatus_t)loadDllFunction(dll, "fmi2GetRealStatus", &ok);
        fmu->fmi2.getIntegerStatus = (fmi2GetIntegerStatus_t)loadDllFunction(dll, "fmi2GetIntegerStatus", &ok);
        fmu->fmi2.getBooleanStatus = (fmi2GetBooleanStatus_t)loadDllFunction(dll, "fmi2GetBooleanStatus", &ok);
        fmu->fmi2.getStringStatus = (fmi2GetStringStatus_t)loadDllFunction(dll, "fmi2GetStringStatus", &ok);
    }

    if(fmuType == fmi2ModelExchange) {
        //Load model exchange specific functions
         fmu->fmi2.enterEventMode = (fmi2EnterEventMode_t)loadDllFunction(dll, "fmi2EnterEventMode", &ok);
         fmu->fmi2.newDiscreteStates = (fmi2NewDiscreteStates_t)loadDllFunction(dll, "fmi2NewDiscreteStates", &ok);
         fmu->fmi2.enterContinuousTimeMode = (fmi2EnterContinuousTimeMode_t)loadDllFunction(dll, "fmi2EnterContinuousTimeMode", &ok);
         fmu->fmi2.completedIntegratorStep = (fmi2CompletedIntegratorStep_t)loadDllFunction(dll, "fmi2CompletedIntegratorStep", &ok);
         fmu->fmi2.setTime = (fmi2SetTime_t)loadDllFunction(dll,"fmi2SetTime", &ok);
         fmu->fmi2.setContinuousStates = (fmi2SetContinuousStates_t)loadDllFunction(dll, "fmi2SetContinuousStates", &ok);
         fmu->fmi2.getEventIndicators = (fmi2GetEventIndicators_t)loadDllFunction(dll, "fmi2GetEventIndicators", &ok);
         fmu->fmi2.getContinuousStates = (fmi2GetContinuousStates_t)loadDllFunction(dll, "fmi2GetContinuousStates", &ok);
         fmu->fmi2.getDerivatives = (fmi2GetDerivatives_t)loadDllFunction(dll, "fmi2GetDerivatives", &ok);
         fmu->fmi2.getNominalsOfContinuousStates = (fmi2GetNominalsOfContinuousStates_t)loadDllFunction(dll, "fmi2GetNominalsOfContinuousStates", &ok);
    }

    chdir(cwd);

    return ok;
}


//! @brief Loads all DLL functions for FMI 3
//! @param fmu FMU handle
//! @returns True if load was successful
bool loadFunctionsFmi3(fmiHandle *fmu, fmi3Type fmuType)
{
    TRACEFUNC

    if (fmu->dll != NULL) {
#ifdef _WIN32
        FreeLibrary(fmu->dll);
#else
        dlclose(fmu->dll);
#endif
    }

    char cwd[FILENAME_MAX];
#ifdef _WIN32
    _getcwd(cwd, sizeof(char)*FILENAME_MAX);
#else
    getcwd(cwd, sizeof(char)*FILENAME_MAX);
#endif

    char dllPath[FILENAME_MAX] = {0};
    size_t max_num_chars_to_append = sizeof(dllPath)-1;
    strncat(dllPath, fmu->unzippedLocation, max_num_chars_to_append);

    // Append on the form: /binaries/x86_64-windows/
    max_num_chars_to_append = sizeof(dllPath)-strlen(dllPath)-1;
    strncat(dllPath, dirsep_str "binaries" dirsep_str arch_str "-" fmi3_system_str dirsep_str, max_num_chars_to_append);

    max_num_chars_to_append = sizeof(dllPath)-strlen(dllPath)-1;
    if(fmuType == fmi3CoSimulation) {
        strncat(dllPath, fmu->fmi3.cs.modelIdentifier, max_num_chars_to_append);
    }
    else if(fmuType == fmi3ModelExchange) {
        strncat(dllPath, fmu->fmi3.me.modelIdentifier, max_num_chars_to_append);
    }
    else {
        strncat(dllPath, fmu->fmi3.se.modelIdentifier, max_num_chars_to_append);
    }

    max_num_chars_to_append = sizeof(dllPath)-strlen(dllPath)-1;
    strncat(dllPath, dllext_str, max_num_chars_to_append);

#ifdef _WIN32
    char dllDirectory[FILENAME_MAX] = {0};
    max_num_chars_to_append = sizeof(dllDirectory)-1;
    strncat(dllDirectory, fmu->unzippedLocation, max_num_chars_to_append);
    max_num_chars_to_append = sizeof(dllDirectory)-strlen(dllDirectory)-1;
    strncat(dllDirectory, dirsep_str "binaries" dirsep_str arch_str "-" fmi3_system_str dirsep_str, max_num_chars_to_append);
    BOOL success = SetDllDirectoryA(dllDirectory);
    if (!success) {
        fprintf(stderr, "Loading DLL %s failed:\nFailed to set DLL directory %s", dllPath, dllDirectory);
        return false;
    }

    HINSTANCE dll = LoadLibraryA(dllPath);
    if(NULL == dll) {
        DWORD error = GetLastError();
        LPSTR message = NULL;
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&message, 0, NULL);
        fprintf(stderr, "Failed to load DLL %s:\n%s", dllPath, message);
        LocalFree(message);
        return false;
    }
#else
    char cmd[FILENAME_MAX] = {0};
    strcat(cmd, "chmod +x ");
    strcat(cmd, dllPath);
    system(cmd);

    void *dll = dlopen(dllPath, RTLD_NOW | RTLD_LOCAL);
    if (NULL == dll) {
        printf("Loading shared object fejlade: %s (%s)\n", dllPath, dlerror());
        return false;
    }
#endif

    fmu->dll = dll;

    printf("Loading FMI version 3...\n");

    //Load all common functions




//#define fmi3InstantiateModelExchange         fmi3FullName(fmi3InstantiateModelExchange)
//#define fmi3InstantiateCoSimulation          fmi3FullName(fmi3InstantiateCoSimulation)
//#define fmi3InstantiateScheduledExecution    fmi3FullName(fmi3InstantiateScheduledExecution)

///* Getting and setting variable values */



///* Getting Variable Dependency Information */
//#define fmi3GetNumberOfVariableDependencies fmi3FullName(fmi3GetNumberOfVariableDependencies)
//#define fmi3GetVariableDependencies         fmi3FullName(fmi3GetVariableDependencies)

    bool ok = true;

    //Load common functions
    fmu->fmi3.getVersion = (fmi3GetVersion_t)loadDllFunction(dll,"fmi3GetVersion", &ok);
    fmu->fmi3.setDebugLogging = (fmi3SetDebugLogging_t)loadDllFunction(dll,    "fmi3SetDebugLogging", &ok);
    fmu->fmi3.instantiateModelExchange = (fmi3InstantiateModelExchange_t)loadDllFunction(dll,    "fmi3InstantiateModelExchange", &ok);
    fmu->fmi3.instantiateCoSimulation = (fmi3InstantiateCoSimulation_t)loadDllFunction(dll,    "fmi3InstantiateCoSimulation", &ok);
    fmu->fmi3.instantiateScheduledExecution = (fmi3InstantiateScheduledExecution_t)loadDllFunction(dll,    "fmi3InstantiateScheduledExecution", &ok);
    fmu->fmi3.freeInstance = (fmi3FreeInstance_t)loadDllFunction(dll,"fmi3FreeInstance", &ok);
    fmu->fmi3.enterInitializationMode = (fmi3EnterInitializationMode_t)loadDllFunction(dll,    "fmi3EnterInitializationMode", &ok);
    fmu->fmi3.exitInitializationMode = (fmi3ExitInitializationMode_t)loadDllFunction(dll,    "fmi3ExitInitializationMode", &ok);
    fmu->fmi3.enterEventMode = (fmi3EnterEventMode_t)loadDllFunction(dll,    "fmi3EnterEventMode", &ok);
    fmu->fmi3.terminate = (fmi3Terminate_t)loadDllFunction(dll,"fmi3Terminate", &ok);
    fmu->fmi3.reset = (fmi3Reset_t)loadDllFunction(dll,"fmi3Reset", &ok);
    fmu->fmi3.setFloat64 = (fmi3SetFloat64_t)loadDllFunction(dll,"fmi3SetFloat64", &ok);
    fmu->fmi3.getFloat64 = (fmi3GetFloat64_t)loadDllFunction(dll,"fmi3GetFloat64", &ok);
    fmu->fmi3.getFloat32 = (fmi3GetFloat32_t)loadDllFunction(dll,"fmi3GetFloat32", &ok);
    fmu->fmi3.setFloat32 = (fmi3SetFloat32_t)loadDllFunction(dll,"fmi3SetFloat32", &ok);
    fmu->fmi3.setInt64 = (fmi3SetInt64_t)loadDllFunction(dll,"fmi3SetInt64", &ok);
    fmu->fmi3.getInt64 = (fmi3GetInt64_t)loadDllFunction(dll,"fmi3GetInt64", &ok);
    fmu->fmi3.setInt32 = (fmi3SetInt32_t)loadDllFunction(dll,"fmi3SetInt32", &ok);
    fmu->fmi3.getInt32 = (fmi3GetInt32_t)loadDllFunction(dll,"fmi3GetInt32", &ok);
    fmu->fmi3.setInt16 = (fmi3SetInt16_t)loadDllFunction(dll,"fmi3SetInt16", &ok);
    fmu->fmi3.getInt16 = (fmi3GetInt16_t)loadDllFunction(dll,"fmi3GetInt16", &ok);
    fmu->fmi3.getInt8 = (fmi3GetInt8_t)loadDllFunction(dll,"fmi3GetInt8", &ok);
    fmu->fmi3.setInt8 = (fmi3SetInt8_t)loadDllFunction(dll,"fmi3SetInt8", &ok);
    fmu->fmi3.getUInt64 = (fmi3GetUInt64_t)loadDllFunction(dll,"fmi3GetUInt64", &ok);
    fmu->fmi3.setUInt64 = (fmi3SetUInt64_t)loadDllFunction(dll,"fmi3SetUInt64", &ok);
    fmu->fmi3.getUInt32 = (fmi3GetUInt32_t)loadDllFunction(dll,"fmi3GetUInt32", &ok);
    fmu->fmi3.setUInt32 = (fmi3SetUInt32_t)loadDllFunction(dll,"fmi3SetUInt32", &ok);
    fmu->fmi3.getUInt16 = (fmi3GetUInt16_t)loadDllFunction(dll,"fmi3GetUInt16", &ok);
    fmu->fmi3.setUInt16 = (fmi3SetUInt16_t)loadDllFunction(dll,"fmi3SetUInt16", &ok);
    fmu->fmi3.setUInt8 = (fmi3SetUInt8_t)loadDllFunction(dll,"fmi3SetUInt8", &ok);
    fmu->fmi3.getUInt8 = (fmi3GetUInt8_t)loadDllFunction(dll,"fmi3GetUInt8", &ok);
    fmu->fmi3.setBoolean = (fmi3SetBoolean_t)loadDllFunction(dll,"fmi3SetBoolean", &ok);
    fmu->fmi3.getBoolean = (fmi3GetBoolean_t)loadDllFunction(dll,"fmi3GetBoolean", &ok);
    fmu->fmi3.getString = (fmi3GetString_t)loadDllFunction(dll,"fmi3GetString", &ok);
    fmu->fmi3.setString = (fmi3SetString_t)loadDllFunction(dll,"fmi3SetString", &ok);
    fmu->fmi3.getBinary = (fmi3GetBinary_t)loadDllFunction(dll,"fmi3GetBinary", &ok);
    fmu->fmi3.setBinary = (fmi3SetBinary_t)loadDllFunction(dll,"fmi3SetBinary", &ok);
    fmu->fmi3.getClock = (fmi3GetClock_t)loadDllFunction(dll,"fmi3GetClock", &ok);
    fmu->fmi3.setClock = (fmi3SetClock_t)loadDllFunction(dll,"fmi3SetClock", &ok);
    fmu->fmi3.getNumberOfVariableDependencies =(fmi3GetNumberOfVariableDependencies_t)loadDllFunction(dll,    "fmi3GetNumberOfVariableDependencies", &ok);
    fmu->fmi3.getVariableDependencies = (fmi3GetVariableDependencies_t)loadDllFunction(dll,    "fmi3GetVariableDependencies", &ok);
    fmu->fmi3.getFMUState = (fmi3GetFMUState_t)loadDllFunction(dll,"fmi3GetFMUState", &ok);
    fmu->fmi3.setFMUState = (fmi3SetFMUState_t)loadDllFunction(dll,"fmi3SetFMUState", &ok);
    fmu->fmi3.freeFMUState = (fmi3FreeFMUState_t)loadDllFunction(dll,"fmi3FreeFMUState", &ok);
    fmu->fmi3.serializedFMUStateSize = (fmi3SerializedFMUStateSize_t)loadDllFunction(dll,    "fmi3SerializedFMUStateSize", &ok);
    fmu->fmi3.serializeFMUState = (fmi3SerializeFMUState_t)loadDllFunction(dll,    "fmi3SerializeFMUState", &ok);
    fmu->fmi3.deserializeFMUState = (fmi3DeserializeFMUState_t)loadDllFunction(dll,    "fmi3DeserializeFMUState", &ok);
    fmu->fmi3.getDirectionalDerivative = (fmi3GetDirectionalDerivative_t)loadDllFunction(dll,    "fmi3GetDirectionalDerivative", &ok);
    fmu->fmi3.getAdjointDerivative = (fmi3GetAdjointDerivative_t)loadDllFunction(dll,    "fmi3GetAdjointDerivative", &ok);
    fmu->fmi3.enterConfigurationMode = (fmi3EnterConfigurationMode_t)loadDllFunction(dll,    "fmi3EnterConfigurationMode", &ok);
    fmu->fmi3.exitConfigurationMode = (fmi3ExitConfigurationMode_t)loadDllFunction(dll,    "fmi3ExitConfigurationMode", &ok);
    fmu->fmi3.getIntervalDecimal = (fmi3GetIntervalDecimal_t)loadDllFunction(dll,    "fmi3GetIntervalDecimal", &ok);
    fmu->fmi3.getIntervalFraction = (fmi3GetIntervalFraction_t)loadDllFunction(dll,    "fmi3GetIntervalFraction", &ok);
    fmu->fmi3.getShiftDecimal = (fmi3GetShiftDecimal_t)loadDllFunction(dll,    "fmi3GetShiftDecimal", &ok);
    fmu->fmi3.getShiftFraction = (fmi3GetShiftFraction_t)loadDllFunction(dll,    "fmi3GetShiftFraction", &ok);
    fmu->fmi3.setIntervalDecimal = (fmi3SetIntervalDecimal_t)loadDllFunction(dll,    "fmi3SetIntervalDecimal", &ok);
    fmu->fmi3.setIntervalFraction = (fmi3SetIntervalFraction_t)loadDllFunction(dll,    "fmi3SetIntervalFraction", &ok);
    fmu->fmi3.setShiftDecimal = (fmi3SetShiftDecimal_t)loadDllFunction(dll,    "fmi3SetShiftDecimal", &ok);
    fmu->fmi3.setShiftFraction = (fmi3SetShiftFraction_t)loadDllFunction(dll,    "fmi3SetShiftFraction", &ok);

    if(fmu->fmi3.supportsCoSimulation) {
        //Load co-simulation specific functions
        fmu->fmi3.enterStepMode = (fmi3EnterStepMode_t)loadDllFunction(dll, "fmi3EnterStepMode", &ok);
        fmu->fmi3.getOutputDerivatives = (fmi3GetOutputDerivatives_t)loadDllFunction(dll, "fmi3GetOutputDerivatives", &ok);
        fmu->fmi3.doStep = (fmi3DoStep_t)loadDllFunction(dll, "fmi3DoStep", &ok);
    }

    if(fmu->fmi3.supportsModelExchange) {
        //Load model exchange specific functions
        fmu->fmi3.enterContinuousTimeMode = (fmi3EnterContinuousTimeMode_t)loadDllFunction(dll, "fmi3EnterContinuousTimeMode", &ok);
        fmu->fmi3.completedIntegratorStep = (fmi3CompletedIntegratorStep_t)loadDllFunction(dll, "fmi3CompletedIntegratorStep", &ok);
        fmu->fmi3.setTime = (fmi3SetTime_t)loadDllFunction(dll,"fmi3SetTime", &ok);
        fmu->fmi3.setContinuousStates = (fmi3SetContinuousStates_t)loadDllFunction(dll, "fmi3SetContinuousStates", &ok);
        fmu->fmi3.getContinuousStateDerivatives = (fmi3GetContinuousStateDerivatives_t)loadDllFunction(dll, "fmi3GetContinuousStateDerivatives", &ok);
        fmu->fmi3.getEventIndicators = (fmi3GetEventIndicators_t)loadDllFunction(dll, "fmi3GetEventIndicators", &ok);
        fmu->fmi3.getContinuousStates = (fmi3GetContinuousStates_t)loadDllFunction(dll, "fmi3GetContinuousStates", &ok);
        fmu->fmi3.getNominalsOfContinuousStates = (fmi3GetNominalsOfContinuousStates_t)loadDllFunction(dll, "fmi3GetNominalsOfContinuousStates", &ok);
        fmu->fmi3.getNumberOfEventIndicators = (fmi3GetNumberOfEventIndicators_t)loadDllFunction(dll, "fmi3GetNumberOfEventIndicators", &ok);
        fmu->fmi3.getNumberOfContinuousStates = (fmi3GetNumberOfContinuousStates_t)loadDllFunction(dll, "fmi3GetNumberOfContinuousStates", &ok);
        fmu->fmi3.evaluateDiscreteStates = (fmi3EvaluateDiscreteStates_t)loadDllFunction(dll, "fmi3EvaluateDiscreteStates", &ok);
        fmu->fmi3.updateDiscreteStates = (fmi3UpdateDiscreteStates_t)loadDllFunction(dll, "fmi3UpdateDiscreteStates", &ok);
    }

    if(fmu->fmi3.supportsScheduledExecution) {
        //Load all scheduled execution specific functions
        fmu->fmi3.activateModelPartition = (fmi3ActivateModelPartition_t)loadDllFunction(dll, "fmi3ActivateModelPartition", &ok);
    }

    chdir(cwd);

    return ok;
}


//! @brief Returns FMI version of FMU
//! @param fmu FMU handle
//! @returns Version of the FMU
fmiVersion_t fmi4c_getFmiVersion(fmiHandle *fmu)
{
    return fmu->version;
}




bool fmi3_instantiateCoSimulation(fmiHandle *fmu,
                             fmi3Boolean                    visible,
                             fmi3Boolean                    loggingOn,
                             fmi3Boolean                    eventModeUsed,
                             fmi3Boolean                    earlyReturnAllowed,
                             const fmi3ValueReference       requiredIntermediateVariables[],
                             size_t                         nRequiredIntermediateVariables,
                             fmi3InstanceEnvironment        instanceEnvironment,
                             fmi3LogMessageCallback         logMessage,
                             fmi3IntermediateUpdateCallback intermediateUpdate)
{
    if(!loadFunctionsFmi3(fmu, fmi3CoSimulation)) {
        printf("Failed to load functions for FMI 3 CS.");
        return false;
    }

    fmu->fmi3.fmi3Instance = fmu->fmi3.instantiateCoSimulation(fmu->instanceName,
                                                      fmu->fmi3.instantiationToken,
                                                      fmu->resourcesLocation,
                                                      visible,
                                                      loggingOn,
                                                      eventModeUsed,
                                                      earlyReturnAllowed,
                                                      requiredIntermediateVariables,
                                                      nRequiredIntermediateVariables,
                                                      instanceEnvironment,
                                                      logMessage,
                                                      intermediateUpdate);

    return (fmu->fmi3.fmi3Instance != NULL);
}

bool fmi3_instantiateModelExchange(fmiHandle *fmu,
                                  fmi3Boolean               visible,
                                  fmi3Boolean                loggingOn,
                                  fmi3InstanceEnvironment    instanceEnvironment,
                                  fmi3LogMessageCallback     logMessage)
{
    if(!loadFunctionsFmi3(fmu, fmi3ModelExchange)) {
        printf("Failed to load functions for FMI 3 ME.");
        return false;
    }


    fmu->fmi3.fmi3Instance = fmu->fmi3.instantiateModelExchange(fmu->instanceName,
                                                           fmu->fmi3.instantiationToken,
                                                           fmu->resourcesLocation,
                                                           visible,
                                                           loggingOn,
                                                           instanceEnvironment,
                                                           logMessage);

    return (fmu->fmi3.fmi3Instance != NULL);
}

const char* fmi3_getVersion(fmiHandle *fmu) {

    return fmu->fmi3.getVersion();
}

fmi3Status fmi3_setDebugLogging(fmiHandle *fmu,
                                fmi3Boolean loggingOn,
                                size_t nCategories,
                                const fmi3String categories[])
{

    return fmu->fmi3.setDebugLogging(fmu->fmi3.fmi3Instance, loggingOn, nCategories, categories);
}

fmi3Status fmi3_getFloat64(fmiHandle *fmu,
                           const fmi3ValueReference valueReferences[],
                           size_t nValueReferences,
                           fmi3Float64 values[],
                           size_t nValues) {

    return fmu->fmi3.getFloat64(fmu->fmi3.fmi3Instance,
                                valueReferences,
                                nValueReferences,
                                values,
                                nValues);
}


fmi3Status fmi3_setFloat64(fmiHandle *fmu,
                           const fmi3ValueReference valueReferences[],
                           size_t nValueReferences,
                           const fmi3Float64 values[],
                           size_t nValues) {

    return fmu->fmi3.setFloat64(fmu->fmi3.fmi3Instance,
                                valueReferences,
                                nValueReferences,
                                values,
                                nValues);
}

fmi3Status fmi3_enterInitializationMode(fmiHandle *fmu,
                                       fmi3Boolean toleranceDefined,
                                       fmi3Float64 tolerance,
                                       fmi3Float64 startTime,
                                       fmi3Boolean stopTimeDefined,
                                       fmi3Float64 stopTime)
{
    return fmu->fmi3.enterInitializationMode(fmu->fmi3.fmi3Instance,
                                            toleranceDefined,
                                            tolerance,
                                            startTime,
                                            stopTimeDefined,
                                            stopTime);
}

fmi3Status fmi3_exitInitializationMode(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.exitInitializationMode(fmu->fmi3.fmi3Instance);
}

fmi3Status fmi3_terminate(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.terminate(fmu->fmi3.fmi3Instance);
}

void fmi3_freeInstance(fmiHandle *fmu)
{
    TRACEFUNC

    fmu->fmi3.freeInstance(fmu->fmi3.fmi3Instance);
}

fmi3Status fmi3_doStep(fmiHandle *fmu,
                       fmi3Float64 currentCommunicationPoint,
                       fmi3Float64 communicationStepSize,
                       fmi3Boolean noSetFMUStatePriorToCurrentPoint,
                       fmi3Boolean *eventEncountered,
                       fmi3Boolean *terminateSimulation,
                       fmi3Boolean *earlyReturn,
                       fmi3Float64 *lastSuccessfulTime)
{

    return fmu->fmi3.doStep(fmu->fmi3.fmi3Instance,
                            currentCommunicationPoint,
                            communicationStepSize,
                            noSetFMUStatePriorToCurrentPoint,
                            eventEncountered,
                            terminateSimulation,
                            earlyReturn,
                            lastSuccessfulTime);
}

const char *fmi3_modelName(fmiHandle *fmu)
{
    return fmu->fmi3.modelName;
}

const char* fmi3_instantiationToken(fmiHandle *fmu)
{

    return fmu->fmi3.instantiationToken;
}

const char* fmi3_description(fmiHandle *fmu)
{

    return fmu->fmi3.description;
}

const char* fmi3_author(fmiHandle *fmu)
{

    return fmu->fmi3.author;
}

const char* fmi3_version(fmiHandle *fmu)
{
    return fmu->fmi3.version;
}

const char* fmi3_copyright(fmiHandle *fmu)
{

    return fmu->fmi3.copyright;
}

const char* fmi3_license(fmiHandle *fmu)
{

    return fmu->fmi3.license;
}

const char* fmi3_generationTool(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.generationTool;
}

const char* fmi3_generationDateAndTime(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.generationDateAndTime;
}

const char* fmi3_variableNamingConvention(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.variableNamingConvention;
}

bool fmi3_supportsCoSimulation(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.supportsCoSimulation;
}

bool fmi3_supportsModelExchange(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.supportsModelExchange;
}

bool fmi3_supportsScheduledExecution(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.supportsScheduledExecution;
}

const char *fmi2_getTypesPlatform(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.getTypesPlatform();
}

// this function returns the fmiVersion (e.g) fmiVersion = 2.0
const char *fmi2_getFmiVersion(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.fmiVersion_;
}

// this function returns the optional model version from the <fmiModeldescription>
const char *fmi2_getVersion(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.version;
}

fmi2Status fmi2_setDebugLogging(fmiHandle *fmu, fmi2Boolean loggingOn, size_t nCategories, const fmi2String categories[])
{
    TRACEFUNC

    return fmu->fmi2.setDebugLogging(fmu->fmi2.component, loggingOn, nCategories, categories);
}

bool fmi2_instantiate(fmiHandle *fmu, fmi2Type type, fmi2CallbackLogger logger, fmi2CallbackAllocateMemory allocateMemory, fmi2CallbackFreeMemory freeMemory, fmi2StepFinished stepFinished, fmi2ComponentEnvironment componentEnvironment, fmi2Boolean visible, fmi2Boolean loggingOn)
{
    TRACEFUNC
    if(type == fmi2CoSimulation && !fmu->fmi2.supportsCoSimulation) {
        printf("FMI for co-simulation is not supported by this FMU.");
        return false;
    }
    else if(type == fmi2ModelExchange && !fmu->fmi2.supportsModelExchange) {
        printf("FMI for model exchange is not supported by this FMU.");
        return false;
    }

    if(!loadFunctionsFmi2(fmu, type)) {
        printf("Failed to load functions for FMI 2.");
        return false;
    }

    fmu->fmi2.callbacks.logger = logger;
    fmu->fmi2.callbacks.allocateMemory = allocateMemory;
    fmu->fmi2.callbacks.freeMemory = freeMemory;
    fmu->fmi2.callbacks.stepFinished = stepFinished;
    fmu->fmi2.callbacks.componentEnvironment = componentEnvironment;

    // printf("  FMIVersion:         %s\n", fmu->fmi2.fmiVersion_);
    // printf("  instanceName:       %s\n", fmu->instanceName);
    // printf("  GUID:               %s\n", fmu->fmi2.guid);
    // printf("  unzipped location:  %s\n", fmu->unzippedLocation);
    // printf("  resources location: %s\n", fmu->resourcesLocation);

    fmu->fmi2.component = fmu->fmi2.instantiate(fmu->instanceName, type, fmu->fmi2.guid, fmu->resourcesLocation, &fmu->fmi2.callbacks, visible, loggingOn);
    return (fmu->fmi2.component != NULL);
}

void fmi2_freeInstance(fmiHandle *fmu)
{
    TRACEFUNC

    fmu->fmi2.freeInstance(fmu->fmi2.component);
}

fmi2Status fmi2_setupExperiment(fmiHandle *fmu, fmi2Boolean toleranceDefined, fmi2Real tolerance, fmi2Real startTime, fmi2Boolean stopTimeDefined, fmi2Real stopTime)
{
    TRACEFUNC

    return fmu->fmi2.setupExperiment(fmu->fmi2.component, toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime);
}

fmi2Status fmi2_enterInitializationMode(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.enterInitializationMode(fmu->fmi2.component);
}

fmi2Status fmi2_exitInitializationMode(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.exitInitializationMode(fmu->fmi2.component);
}

fmi2Status fmi2_terminate(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.terminate(fmu->fmi2.component);
}

fmi2Status fmi2_reset(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.reset(fmu->fmi2.component);
}

int fmi2_getNumberOfUnits(fmiHandle *fmu)
{
    return fmu->fmi2.numberOfUnits;
}

fmi2UnitHandle *fmi2_getUnitByIndex(fmiHandle *fmu, int i)
{
    return &fmu->fmi2.units[i];
}

const char* fmi2_getUnitName(fmi2UnitHandle *unit)
{
    return unit->name;
}

bool fmi2_hasBaseUnit(fmi2UnitHandle *unit)
{
    return (unit->baseUnit != NULL);
}

void fmi2_getBaseUnit(fmi2UnitHandle *unit, double *factor, double *offset, int *kg, int *m, int *s, int *A, int *K, int *mol, int *cd, int *rad)
{
    if(unit->baseUnit != NULL) {
        *factor = unit->baseUnit->factor;
        *offset = unit->baseUnit->offset;
        *kg = unit->baseUnit->kg;
        *m= unit->baseUnit->m;
        *s= unit->baseUnit->s;
        *A= unit->baseUnit->A;
        *K= unit->baseUnit->K;
        *mol= unit->baseUnit->mol;
        *cd= unit->baseUnit->cd;
        *rad= unit->baseUnit->rad;
    }
}

double fmi2GetBaseUnitFactor(fmi2UnitHandle *unit)
{
if(unit->baseUnit != NULL) {
    return unit->baseUnit->factor;
}
return 0;
}

double fmi2GetBaseUnitOffset(fmi2UnitHandle *unit)
{
if(unit->baseUnit != NULL) {
    return unit->baseUnit->offset;
}
return 0;
}

int fmi2_getNumberOfDisplayUnits(fmi2UnitHandle *unit)
{
    return unit->numberOfDisplayUnits;
}

void fmi2_getDisplayUnitByIndex(fmi2UnitHandle *unit, int id, const char **name, double *factor, double *offset)
{
    *name = unit->displayUnits[id].name;
    *factor = unit->displayUnits[id].factor;
    *offset = unit->displayUnits[id].offset;
}

int fmi3_getNumberOfVariables(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.numberOfVariables;
}

const char *fmi3_getVariableName(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->name;
}

fmi3Initial fmi3_getVariableInitial(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->initial;
}

fmi3Variability fmi3_getVariableVariability(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->variability;
}

fmi3Causality fmi3_getVariableCausality(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->causality;
}


bool fmi3_getVariableSupportsIntermediateUpdate(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->intermediateUpdate;
}

fmi3DataType fmi3_getVariableDataType(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->datatype;
}

const char *fmi3_getVariableDescription(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->description;
}

const char *fmi3_getVariableQuantity(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->quantity;
}

const char *fmi3_getVariableUnit(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->unit;
}

const char *fmi3_getVariableDisplayUnit(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->displayUnit;
}

bool fmi3_getVariableHasStartValue(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->hasStartValue;
}

fmi3Float64 fmi3_getVariableStartFloat64(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startFloat64;
}

fmi3Float32 fmi3_getVariableStartFloat32(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startFloat32;
}

fmi3Int64 fmi3_getVariableStartInt64(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startInt64;
}

fmi3Int32 fmi3_getVariableStartInt32(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startInt32;
}

fmi3Int16 fmi3_getVariableStartInt16(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startInt16;
}

fmi3Int8 fmi3_getVariableStartInt8(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startInt8;
}

fmi3UInt64 fmi3_getVariableStartUInt64(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startUInt64;
}

fmi3UInt32 fmi3_getVariableStartUInt32(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startUInt32;
}

fmi3UInt16 fmi3_getVariableStartUInt16(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startUInt16;
}

fmi3UInt8 fmi3_getVariableStartUInt8(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startUInt8;
}

fmi3Boolean fmi3_getVariableStartBoolean(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startBoolean;
}

fmi3String fmi3_getVariableStartString(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startString;
}

fmi3Binary fmi3_getVariableStartBinary(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startBinary;
}

fmi3Boolean fmi3GetVariableStartEnumeration(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startEnumeration;
}

fmi3ValueReference fmi3_getVariableValueReference(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->valueReference;
}

fmi3Status fmi3_enterEventMode(fmiHandle *fmu)
{
    return fmu->fmi3.enterEventMode(fmu->fmi3.fmi3Instance);
}

fmi3Status fmi3_reset(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.reset(fmu->fmi3.fmi3Instance);
}

fmi3Status fmi3_getFloat32(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Float32 values[], size_t nValues)
{

    return fmu->fmi3.getFloat32(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_getInt8(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int8 values[], size_t nValues)
{

    return fmu->fmi3.getInt8(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_getUInt8(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt8 values[], size_t nValues)
{

    return fmu->fmi3.getUInt8(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_getInt16(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int16 values[], size_t nValues)
{

    return fmu->fmi3.getInt16(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_getUInt16(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt16 values[], size_t nValues)
{

    return fmu->fmi3.getUInt16(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_getInt32(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int32 values[], size_t nValues)
{

    return fmu->fmi3.getInt32(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_getUInt32(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt32 values[], size_t nValues)
{

    return fmu->fmi3.getUInt32(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_getInt64(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int64 values[], size_t nValues)
{

    return fmu->fmi3.getInt64(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_getUInt64(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt64 values[], size_t nValues)
{

    return fmu->fmi3.getUInt64(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_getBoolean(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Boolean values[], size_t nValues)
{

    return fmu->fmi3.getBoolean(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_getString(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3String values[], size_t nValues)
{

    return fmu->fmi3.getString(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_getBinary(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, size_t valueSizes[], fmi3Binary values[], size_t nValues)
{

    return fmu->fmi3.getBinary(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, valueSizes, values, nValues);
}

fmi3Status fmi3_getClock(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Clock values[])
{

    return fmu->fmi3.getClock(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values);
}

fmi3Status fmi3_setFloat32(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Float32 values[], size_t nValues)
{

    return fmu->fmi3.setFloat32(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_setInt8(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int8 values[], size_t nValues)
{

    return fmu->fmi3.setInt8(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_setUInt8(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt8 values[], size_t nValues)
{

    return fmu->fmi3.setUInt8(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_setInt16(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int16 values[], size_t nValues)
{

    return fmu->fmi3.setInt16(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_setUInt16(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt16 values[], size_t nValues)
{

    return fmu->fmi3.setUInt16(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_setInt32(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int32 values[], size_t nValues)
{

    return fmu->fmi3.setInt32(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_setUInt32(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt32 values[], size_t nValues)
{

    return fmu->fmi3.setUInt32(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_setInt64(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int64 values[], size_t nValues)
{

    return fmu->fmi3.setInt64(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_setUInt64(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt64 values[], size_t nValues)
{

    return fmu->fmi3.setUInt64(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_setBoolean(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Boolean values[], size_t nValues)
{

    return fmu->fmi3.setBoolean(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_setString(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3String values[], size_t nValues)
{

    return fmu->fmi3.setString(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3_setBinary(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const size_t valueSizes[], const fmi3Binary values[], size_t nValues)
{

    return fmu->fmi3.setBinary(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, valueSizes, values, nValues);
}

fmi3Status fmi3_setClock(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Clock values[])
{

    return fmu->fmi3.setClock(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values);
}

fmi3Status fmi3_getNumberOfVariableDependencies(fmiHandle *fmu, fmi3ValueReference valueReference, size_t *nDependencies)
{

    return fmu->fmi3.getNumberOfVariableDependencies(fmu->fmi3.fmi3Instance, valueReference, nDependencies);
}

fmi3Status fmi3_getVariableDependencies(fmiHandle *fmu, fmi3ValueReference dependent, size_t elementIndicesOfDependent[], fmi3ValueReference independents[], size_t elementIndicesOfIndependents[], fmi3DependencyKind dependencyKinds[], size_t nDependencies)
{

    return fmu->fmi3.getVariableDependencies(fmu->fmi3.fmi3Instance, dependent, elementIndicesOfDependent, independents, elementIndicesOfIndependents, dependencyKinds, nDependencies);
}

fmi3Status fmi3_getFMUState(fmiHandle *fmu, fmi3FMUState *FMUState)
{

    return fmu->fmi3.getFMUState(fmu->fmi3.fmi3Instance, FMUState);
}

fmi3Status fmi3_setFMUState(fmiHandle *fmu, fmi3FMUState FMUState)
{

    return fmu->fmi3.setFMUState(fmu->fmi3.fmi3Instance, FMUState);
}

fmi3Status fmi3_freeFMUState(fmiHandle *fmu, fmi3FMUState *FMUState)
{

    return fmu->fmi3.freeFMUState(fmu->fmi3.fmi3Instance, FMUState);
}

fmi3Status fmi3_serializedFMUStateSize(fmiHandle *fmu, fmi3FMUState FMUState, size_t *size)
{

    return fmu->fmi3.serializedFMUStateSize(fmu->fmi3.fmi3Instance, FMUState, size);
}

fmi3Status fmi3_serializeFMUState(fmiHandle *fmu, fmi3FMUState FMUState, fmi3Byte serializedState[], size_t size)
{

    return fmu->fmi3.serializeFMUState(fmu->fmi3.fmi3Instance, FMUState, serializedState, size);
}

fmi3Status fmi3_deserializeFMUState(fmiHandle *fmu, const fmi3Byte serializedState[], size_t size, fmi3FMUState *FMUState)
{

    return fmu->fmi3.deserializeFMUState(fmu->fmi3.fmi3Instance, serializedState, size, FMUState);
}

fmi3Status fmi3_getDirectionalDerivative(fmiHandle *fmu, const fmi3ValueReference unknowns[], size_t nUnknowns, const fmi3ValueReference knowns[], size_t nKnowns, const fmi3Float64 seed[], size_t nSeed, fmi3Float64 sensitivity[], size_t nSensitivity)
{

    return fmu->fmi3.getDirectionalDerivative(fmu->fmi3.fmi3Instance, unknowns, nUnknowns, knowns, nKnowns, seed, nSeed, sensitivity, nSensitivity);
}

fmi3Status fmi3_getAdjointDerivative(fmiHandle *fmu, const fmi3ValueReference unknowns[], size_t nUnknowns, const fmi3ValueReference knowns[], size_t nKnowns, const fmi3Float64 seed[], size_t nSeed, fmi3Float64 sensitivity[], size_t nSensitivity)
{

    return fmu->fmi3.getAdjointDerivative(fmu->fmi3.fmi3Instance, unknowns, nUnknowns, knowns, nKnowns, seed, nSeed, sensitivity, nSensitivity);
}

fmi3Status fmi3_enterConfigurationMode(fmiHandle *fmu)
{

    return fmu->fmi3.enterConfigurationMode(fmu->fmi3.fmi3Instance);
}

fmi3Status fmi3_exitConfigurationMode(fmiHandle *fmu)
{

    return fmu->fmi3.exitConfigurationMode(fmu->fmi3.fmi3Instance);
}

fmi3Status fmi3_getIntervalDecimal(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Float64 intervals[], fmi3IntervalQualifier qualifiers[])
{

    return fmu->fmi3.getIntervalDecimal(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, intervals, qualifiers);
}

fmi3Status fmi3_getIntervalFraction(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt64 intervalCounters[], fmi3UInt64 resolutions[], fmi3IntervalQualifier qualifiers[])
{

    return fmu->fmi3.getIntervalFraction(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, intervalCounters, resolutions, qualifiers);
}

fmi3Status fmi3_getShiftDecimal(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Float64 shifts[])
{

    return fmu->fmi3.getShiftDecimal(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, shifts);
}

fmi3Status fmi3_getShiftFraction(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt64 shiftCounters[], fmi3UInt64 resolutions[])
{

    return fmu->fmi3.getShiftFraction(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, shiftCounters, resolutions);
}

fmi3Status fmi3_setIntervalDecimal(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Float64 intervals[])
{

    return fmu->fmi3.setIntervalDecimal(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, intervals);
}

fmi3Status fmi3_setIntervalFraction(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt64 intervalCounters[], const fmi3UInt64 resolutions[])
{

    return fmu->fmi3.setIntervalFraction(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, intervalCounters, resolutions);
}

fmi3Status fmi3_evaluateDiscreteStates(fmiHandle *fmu)
{

    return fmu->fmi3.evaluateDiscreteStates(fmu->fmi3.fmi3Instance);
}

fmi3Status fmi3_updateDiscreteStates(fmiHandle *fmu, fmi3Boolean *discreteStatesNeedUpdate, fmi3Boolean *terminateSimulation, fmi3Boolean *nominalsOfContinuousStatesChanged, fmi3Boolean *valuesOfContinuousStatesChanged, fmi3Boolean *nextEventTimeDefined, fmi3Float64 *nextEventTime)
{

    return fmu->fmi3.updateDiscreteStates(fmu->fmi3.fmi3Instance, discreteStatesNeedUpdate, terminateSimulation, nominalsOfContinuousStatesChanged, valuesOfContinuousStatesChanged, nextEventTimeDefined, nextEventTime);
}

fmi3Status fmi3_enterContinuousTimeMode(fmiHandle *fmu)
{

    return fmu->fmi3.enterContinuousTimeMode(fmu->fmi3.fmi3Instance);
}

fmi3Status fmi3_completedIntegratorStep(fmiHandle *fmu, fmi3Boolean noSetFMUStatePriorToCurrentPoint, fmi3Boolean *enterEventMode, fmi3Boolean *terminateSimulation)
{

    return fmu->fmi3.completedIntegratorStep(fmu->fmi3.fmi3Instance, noSetFMUStatePriorToCurrentPoint, enterEventMode, terminateSimulation);
}

fmi3Status fmi3_setTime(fmiHandle *fmu, fmi3Float64 time)
{

    return fmu->fmi3.setTime(fmu->fmi3.fmi3Instance, time);
}

fmi3Status fmi3_setContinuousStates(fmiHandle *fmu, const fmi3Float64 continuousStates[], size_t nContinuousStates)
{

    return fmu->fmi3.setContinuousStates(fmu->fmi3.fmi3Instance, continuousStates, nContinuousStates);
}

fmi3Status fmi3_getContinuousStateDerivatives(fmiHandle *fmu, fmi3Float64 derivatives[], size_t nContinuousStates)
{

    return fmu->fmi3.getContinuousStateDerivatives(fmu->fmi3.fmi3Instance, derivatives, nContinuousStates);
}

fmi3Status fmi3_getEventIndicators(fmiHandle *fmu, fmi3Float64 eventIndicators[], size_t nEventIndicators)
{

    return fmu->fmi3.getEventIndicators(fmu->fmi3.fmi3Instance, eventIndicators, nEventIndicators);
}

fmi3Status fmi3_getContinuousStates(fmiHandle *fmu, fmi3Float64 continuousStates[], size_t nContinuousStates)
{

    return fmu->fmi3.getContinuousStates(fmu->fmi3.fmi3Instance, continuousStates, nContinuousStates);
}

fmi3Status fmi3_getNominalsOfContinuousStates(fmiHandle *fmu, fmi3Float64 nominals[], size_t nContinuousStates)
{

    return fmu->fmi3.getNominalsOfContinuousStates(fmu->fmi3.fmi3Instance, nominals, nContinuousStates);
}

fmi3Status fmi3_getNumberOfEventIndicators(fmiHandle *fmu, size_t *nEventIndicators)
{

    return fmu->fmi3.getNumberOfEventIndicators(fmu->fmi3.fmi3Instance, nEventIndicators);
}

fmi3Status fmi3_getNumberOfContinuousStates(fmiHandle *fmu, size_t *nContinuousStates)
{

    return fmu->fmi3.getNumberOfContinuousStates(fmu->fmi3.fmi3Instance, nContinuousStates);
}

fmi3Status fmi3_enterStepMode(fmiHandle *fmu)
{

    return fmu->fmi3.enterStepMode(fmu->fmi3.fmi3Instance);
}

fmi3Status fmi3_getOutputDerivatives(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int32 orders[], fmi3Float64 values[], size_t nValues)
{

    return fmu->fmi3.getOutputDerivatives(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, orders, values, nValues);
}

fmi3Status fmi3_activateModelPartition(fmiHandle *fmu, fmi3ValueReference clockReference, fmi3Float64 activationTime)
{

    return fmu->fmi3.activateModelPartition(fmu->fmi3.fmi3Instance, clockReference, activationTime);
}

bool fmi3_defaultStartTimeDefined(fmiHandle *fmu)
{

    return fmu->fmi3.defaultStartTimeDefined;
}

bool fmi3_defaultStopTimeDefined(fmiHandle *fmu)
{

    return fmu->fmi3.defaultStopTimeDefined;
}

bool fmi3_defaultToleranceDefined(fmiHandle *fmu)
{

    return fmu->fmi3.defaultToleranceDefined;
}

bool fmi3_defaultStepSizeDefined(fmiHandle *fmu)
{

    return fmu->fmi3.defaultStepSizeDefined;
}

double fmi3_getDefaultStartTime(fmiHandle *fmu)
{

    return fmu->fmi3.defaultStartTime;
}

double fmi3_getDefaultStopTime(fmiHandle *fmu)
{

    return fmu->fmi3.defaultStopTime;
}

double fmi3_getDefaultTolerance(fmiHandle *fmu)
{

    return fmu->fmi3.defaultTolerance;
}

double fmi3_getDefaultStepSize(fmiHandle *fmu)
{

    return fmu->fmi3.defaultStepSize;
}

bool fmi2_defaultStartTimeDefined(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.defaultStartTimeDefined;
}

bool fmi2_defaultStopTimeDefined(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.defaultStopTimeDefined;
}

bool fmi2_defaultToleranceDefined(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.defaultToleranceDefined;
}

bool fmi2_defaultStepSizeDefined(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.defaultStepSizeDefined;
}

double fmi2_getDefaultStartTime(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.defaultStartTime;
}

double fmi2_getDefaultStopTime(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.defaultStopTime;
}

double fmi2_getDefaultTolerance(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.defaultTolerance;
}

double fmi2_getDefaultStepSize(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.defaultStepSize;
}

fmi3VariableHandle *fmi3_getVariableByName(fmiHandle *fmu, fmi3String name)
{
    for(int i=0; i<fmu->fmi3.numberOfVariables; ++i) {
        if(!strcmp(fmu->fmi3.variables[i].name, name)) {
            return &fmu->fmi3.variables[i];
        }
    }
    printf("Variable with name %s not found.\n", name);
    return NULL;
}

fmi3VariableHandle *fmi3_getVariableByIndex(fmiHandle *fmu, int i)
{

    if(i >= fmu->fmi3.numberOfVariables) {
        printf("Variable index out of bounds: %i\n",i);
        return NULL;
    }
    return &fmu->fmi3.variables[i];
}

fmi3VariableHandle *fmi3_getVariableByValueReference(fmiHandle *fmu, fmi3ValueReference vr)
{

    for(int i=0; i<fmu->fmi3.numberOfVariables; ++i) {
        if(fmu->fmi3.variables[i].valueReference == vr) {
            return &fmu->fmi3.variables[i];
        }
    }
    printf("Variable with value reference %i not found.\n", vr);
    return NULL;
}

int fmi2_getNumberOfVariables(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.numberOfVariables;
}

fmi2VariableHandle *fmi2_getVariableByIndex(fmiHandle *fmu, int i)
{
    TRACEFUNC

    if(i >= fmu->fmi2.numberOfVariables) {
        printf("Variable index out of bounds: %i\n",i);
        return NULL;
    }
    return &fmu->fmi2.variables[i];
}

fmi2VariableHandle *fmi2_getVariableByValueReference(fmiHandle *fmu, fmi2ValueReference vr)
{
    TRACEFUNC

    for(int i=0; i<fmu->fmi2.numberOfVariables; ++i) {
        if(fmu->fmi2.variables[i].valueReference == vr) {
            return &fmu->fmi2.variables[i];
        }
    }
    printf("Variable with value reference %i not found.\n", vr);
    return NULL;
}

fmi2VariableHandle *fmi2_getVariableByName(fmiHandle *fmu, fmi2String name)
{
    for(int i=0; i<fmu->fmi2.numberOfVariables; ++i) {
        if(!strcmp(fmu->fmi2.variables[i].name, name)) {
            return &fmu->fmi2.variables[i];
        }
    }
    printf("Variable with name %s not found.\n", name);
    return NULL;
}

const char *fmi2_getVariableName(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->name;
}

const char *fmi2_getVariableDescription(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->description;
}

int fmi2_getVariableDerivativeIndex(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->derivative;
}

const char* fmi2_getAuthor(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.author;
}

const char* fmi2_getModelName(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.modelName;
}

const char* fmi2_getModelDescription(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.description;
}

const char* fmi2_getCopyright(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.copyright;
}

const char* fmi2_getLicense(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.license;
}

const char* fmi2_getGenerationTool(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.generationTool;
}

const char* fmi2_getGenerationDateAndTime(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.generationDateAndTime;
}

const char* fmi2_getVariableNamingConvention(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.variableNamingConvention;
}

const char *fmi2_getVariableQuantity(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->quantity;
}

const char *fmi2_getVariableUnit(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->unit;
}

const char *fmi2_getVariableDisplayUnit(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->displayUnit;
}

bool fmi2_getVariableRelativeQuantity(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->relativeQuantity;
}

fmi2Real fmi2_getVariableMin(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->min;
}

fmi2Real fmi2_getVariableMax(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->max;
}

fmi2Real fmi2_getVariableNominal(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->nominal;
}

bool fmi2_getVariableUnbounded(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->unbounded;
}

bool fmi2_getVariableHasStartValue(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->hasStartValue;
}

fmi2Real fmi2_getVariableStartReal(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->startReal;
}


fmi2Integer fmi2_getVariableStartInteger(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->startInteger;
}

fmi2Boolean fmi2_getVariableStartBoolean(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->startBoolean;
}

fmi2String fmi2_getVariableStartString(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->startString;
}

long fmi2_getVariableValueReference(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->valueReference;
}

fmi2Causality fmi2_getVariableCausality(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->causality;
}

fmi2Variability fmi2_getVariableVariability(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->variability;
}

fmi2Initial fmi2_getVariableInitial(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->initial;
}

bool fmi2_getVariableCanHandleMultipleSetPerTimeInstant(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->canHandleMultipleSetPerTimeInstant;
}

fmi2DataType fmi2_getVariableDataType(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->datatype;
}

fmi2Status fmi2_getReal(fmiHandle *fmu,
                       const fmi2ValueReference valueReferences[],
                       size_t nValueReferences,
                       fmi2Real values[])
{
    return fmu->fmi2.getReal(fmu->fmi2.component,
                            valueReferences,
                            nValueReferences,
                            values);
}

fmi2Status fmi2_getInteger(fmiHandle *fmu,
                          const fmi2ValueReference valueReferences[],
                          size_t nValueReferences,
                          fmi2Integer values[])
{
    TRACEFUNC

    return fmu->fmi2.getInteger(fmu->fmi2.component,
                                  valueReferences,
                                  nValueReferences,
                                  values);
}

fmi2Status fmi2_getBoolean(fmiHandle *fmu,
                          const fmi2ValueReference valueReferences[],
                          size_t nValueReferences,
                          fmi2Boolean values[])
{
    TRACEFUNC

    return fmu->fmi2.getBoolean(fmu->fmi2.component,
                                  valueReferences,
                                  nValueReferences,
                                  values);
}

fmi2Status fmi2_getString(fmiHandle *fmu,
                         const fmi2ValueReference valueReferences[],
                         size_t nValueReferences,
                         fmi2String values[])
{
    TRACEFUNC

    return fmu->fmi2.getString(fmu->fmi2.component,
                                 valueReferences,
                                 nValueReferences,
                                 values);
}

fmi2Status fmi2_setReal(fmiHandle *fmu,
                       const fmi2ValueReference valueReferences[],
                       size_t nValueReferences,
                       const fmi2Real values[])
{
    return fmu->fmi2.setReal(fmu->fmi2.component,
                               valueReferences,
                               nValueReferences,
                               values);
}

fmi2Status fmi2_setInteger(fmiHandle *fmu,
                          const fmi2ValueReference valueReferences[],
                          size_t nValueReferences,
                          const fmi2Integer values[])
{
    TRACEFUNC

    return fmu->fmi2.setInteger(fmu->fmi2.component,
                               valueReferences,
                               nValueReferences,
                               values);
}

fmi2Status fmi2_setBoolean(fmiHandle *fmu,
                          const fmi2ValueReference valueReferences[],
                          size_t nValueReferences,
                          const fmi2Boolean values[])
{
    TRACEFUNC

    return fmu->fmi2.setBoolean(fmu->fmi2.component,
                               valueReferences,
                               nValueReferences,
                               values);
}

fmi2Status fmi2_setString(fmiHandle *fmu,
                         const fmi2ValueReference valueReferences[],
                         size_t nValueReferences,
                         const fmi2String values[])
{
    TRACEFUNC

    return fmu->fmi2.setString(fmu->fmi2.component,
                               valueReferences,
                               nValueReferences,
                               values);
}

fmi2Status fmi2_getFMUstate(fmiHandle* fmu, fmi2FMUstate* FMUstate)
{
    TRACEFUNC
    return fmu->fmi2.getFMUstate(fmu->fmi2.component, FMUstate);
}

fmi2Status fmi2_setFMUstate(fmiHandle* fmu, fmi2FMUstate FMUstate)
{
    TRACEFUNC
    return fmu->fmi2.setFMUstate(fmu->fmi2.component, FMUstate);
}

fmi2Status fmi2_freeFMUstate(fmiHandle* fmu, fmi2FMUstate* FMUstate)
{
    TRACEFUNC
    return fmu->fmi2.freeFMUstate(fmu->fmi2.component, FMUstate);
}

fmi2Status fmi2_serializedFMUstateSize(fmiHandle* fmu, fmi2FMUstate FMUstate, size_t* size)
{
    TRACEFUNC
    return fmu->fmi2.serializedFMUstateSize(fmu->fmi2.component, FMUstate, size);
}

fmi2Status fmi2_serializeFMUstate(fmiHandle* fmu, fmi2FMUstate FMUstate, fmi2Byte serializedState[], size_t size)
{
    TRACEFUNC
    return fmu->fmi2.serializeFMUstate(fmu->fmi2.component, FMUstate, serializedState, size);
}

fmi2Status fmi2_deSerializeFMUstate(fmiHandle* fmu, const fmi2Byte serializedState[], size_t size, fmi2FMUstate* FMUstate)
{
    TRACEFUNC
    return fmu->fmi2.deSerializeFMUstate(fmu->fmi2.component, serializedState, size, FMUstate);
}

fmi2Status fmi2_getDirectionalDerivative(fmiHandle* fmu,
                                        const fmi2ValueReference unknownReferences[],
                                        size_t nUnknown,
                                        const fmi2ValueReference knownReferences[],
                                        size_t nKnown,
                                        const fmi2Real dvKnown[],
                                        fmi2Real dvUnknown[])
{
    TRACEFUNC
    return fmu->fmi2.getDirectionalDerivative(fmu->fmi2.component,
                                             unknownReferences,
                                             nUnknown,
                                             knownReferences,
                                             nKnown,
                                             dvKnown,
                                             dvUnknown);
}

fmi2Status fmi2_enterEventMode(fmiHandle* fmu)
{
    TRACEFUNC
    return fmu->fmi2.enterEventMode(fmu->fmi2.component);
}

fmi2Status fmi2_newDiscreteStates(fmiHandle* fmu, fmi2EventInfo* eventInfo)
{
    TRACEFUNC
            return fmu->fmi2.newDiscreteStates(fmu->fmi2.component, eventInfo);
}

fmi2Status fmi2_enterContinuousTimeMode(fmiHandle* fmu)
{
    TRACEFUNC
    return fmu->fmi2.enterContinuousTimeMode(fmu->fmi2.component);
}

fmi2Status fmi2_completedIntegratorStep(fmiHandle* fmu,
                                       fmi2Boolean noSetFMUStatePriorToCurrentPoint,
                                       fmi2Boolean* enterEventMode,
                                       fmi2Boolean* terminateSimulation)
{
    TRACEFUNC
    return fmu->fmi2.completedIntegratorStep(fmu->fmi2.component,
                                            noSetFMUStatePriorToCurrentPoint,
                                            enterEventMode,
                                            terminateSimulation);
}

fmi2Status fmi2_setTime(fmiHandle* fmu, fmi2Real time)
{
    TRACEFUNC
    return fmu->fmi2.setTime(fmu->fmi2.component, time);
}

fmi2Status fmi2_setContinuousStates(fmiHandle* fmu,
                                   const fmi2Real x[],
                                   size_t nx)
{
    TRACEFUNC
    return fmu->fmi2.setContinuousStates(fmu->fmi2.component, x, nx);
}

fmi2Status fmi2_getDerivatives(fmiHandle* fmu, fmi2Real derivatives[], size_t nx)
{
    TRACEFUNC
    return fmu->fmi2.getDerivatives(fmu->fmi2.component, derivatives, nx);
}

fmi2Status fmi2_getEventIndicators(fmiHandle* fmu, fmi2Real eventIndicators[], size_t ni)
{
    TRACEFUNC
    return fmu->fmi2.getEventIndicators(fmu->fmi2.component, eventIndicators, ni);
}

fmi2Status fmi2_getContinuousStates(fmiHandle* fmu, fmi2Real x[], size_t nx)
{
    TRACEFUNC
    return fmu->fmi2.getContinuousStates(fmu->fmi2.component, x, nx);
}

fmi2Status fmi2_getNominalsOfContinuousStates(fmiHandle* fmu, fmi2Real x_nominal[], size_t nx)
{
    TRACEFUNC
    return fmu->fmi2.getNominalsOfContinuousStates(fmu->fmi2.component, x_nominal, nx);
}

fmi2Status fmi2_setRealInputDerivatives(fmiHandle* fmu, const fmi2ValueReference [], size_t, const fmi2Integer [], const fmi2Real []);
fmi2Status fmi2_getRealOutputDerivatives(fmiHandle* fmu, const fmi2ValueReference [], size_t, const fmi2Integer [], fmi2Real []);

fmi2Status fmi2_setRealInputDerivatives(fmiHandle* fmu,
                                       const fmi2ValueReference vr[],
                                       size_t nvr,
                                       const fmi2Integer order[],
                                       const fmi2Real value[])
{
    TRACEFUNC
    return fmu->fmi2.setRealInputDerivatives(fmu->fmi2.component, vr, nvr, order, value);
}

fmi2Status fmi2_getRealOutputDerivatives (fmiHandle* fmu,
                                         const fmi2ValueReference vr[],
                                         size_t nvr,
                                         const fmi2Integer order[],
                                         fmi2Real value[])
{
    TRACEFUNC
    return fmu->fmi2.getRealOutputDerivatives(fmu->fmi2.component, vr, nvr, order, value);
}

fmi2Status fmi2_doStep(fmiHandle *fmu, fmi2Real currentCommunicationPoint, fmi2Real communicationStepSize, fmi2Boolean noSetFMUStatePriorToCurrentPoint)
{
    TRACEFUNC
    return fmu->fmi2.doStep(fmu->fmi2.component,
                              currentCommunicationPoint,
                              communicationStepSize,
                              noSetFMUStatePriorToCurrentPoint);
}

fmi2Status fmi2_cancelStep(fmiHandle* fmu)
{
    TRACEFUNC
    return fmu->fmi2.cancelStep(fmu);
}

fmi2Status fmi2_getStatus(fmiHandle* fmu, const fmi2StatusKind s, fmi2Status* value)
{
    TRACEFUNC
    return fmu->fmi2.getStatus(fmu, s, value);
}

fmi2Status fmi2_getRealStatus(fmiHandle* fmu, const fmi2StatusKind s, fmi2Real* value)
{
    TRACEFUNC
    return fmu->fmi2.getRealStatus(fmu, s, value);
}

fmi2Status fmi2_getIntegerStatus(fmiHandle* fmu, const fmi2StatusKind s, fmi2Integer* value)
{
    TRACEFUNC
    return fmu->fmi2.getIntegerStatus(fmu, s, value);
}

fmi2Status fmi2_getBooleanStatus(fmiHandle* fmu, const fmi2StatusKind s, fmi2Boolean* value)
{
    TRACEFUNC
    return fmu->fmi2.getBooleanStatus(fmu, s, value);
}

fmi2Status fmi2_getStringStatus(fmiHandle* fmu, const fmi2StatusKind s, fmi2String* value)
{
    TRACEFUNC
    return fmu->fmi2.getStringStatus(fmu, s, value);
}

const char *fmi2_getGuid(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.guid;
}

const char *fmi2cs_getModelIdentifier(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.cs.modelIdentifier;
}

const char *fmi2me_getModelIdentifier(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.me.modelIdentifier;
}

bool fmi2me_getCompletedIntegratorStepNotNeeded(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.me.completedIntegratorStepNotNeeded;
}

bool fmi2cs_getNeedsExecutionTool(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.cs.needsExecutionTool;
}

bool fmi2me_getNeedsExecutionTool(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.me.needsExecutionTool;
}

bool fmi2cs_getCanHandleVariableCommunicationStepSize(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.cs.canHandleVariableCommunicationStepSize;
}

bool fmi2cs_getCanInterpolateInputs(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.cs.canInterpolateInputs;
}

int fmi2cs_getMaxOutputDerivativeOrder(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.cs.maxOutputDerivativeOrder;
}

bool fmi2cs_getCanRunAsynchronuously(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.cs.canRunAsynchronuously;
}

bool fmi2cs_getCanBeInstantiatedOnlyOncePerProcess(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.cs.canBeInstantiatedOnlyOncePerProcess;
}

bool fmi2cs_getCanNotUseMemoryManagementFunctions(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.cs.canNotUseMemoryManagementFunctions;
}

bool fmi2cs_getCanGetAndSetFMUState(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.cs.canGetAndSetFMUState;
}

bool fmi2cs_getCanSerializeFMUState(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.cs.canSerializeFMUState;
}

bool fmi2cs_getProvidesDirectionalDerivative(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.cs.providesDirectionalDerivative;
}

bool fmi2me_getCanBeInstantiatedOnlyOncePerProcess(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi2.me.canBeInstantiatedOnlyOncePerProcess;
}

bool fmi2me_getCanNotUseMemoryManagementFunctions(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi2.me.canNotUseMemoryManagementFunctions;
}

bool fmi2me_getCanGetAndSetFMUState(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi2.me.canGetAndSetFMUState;
}

bool fmi2me_getCanSerializeFMUState(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi2.me.canSerializeFMUState;
}

bool fmi2me_getProvidesDirectionalDerivative(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi2.me.providesDirectionalDerivative;
}

int fmi2_getNumberOfContinuousStates(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.numberOfContinuousStates;
}

int fmi2_getNumberOfEventIndicators(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.numberOfEventIndicators;
}

bool fmi2_getSupportsCoSimulation(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.supportsCoSimulation;
}

bool fmi2_getSupportsModelExchange(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.supportsModelExchange;
}

const char *fmi3cs_getModelIdentifier(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.cs.modelIdentifier;
}

const char *fmi3me_getModelIdentifier(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi3.me.modelIdentifier;
}

const char *fmi3se_getModelIdentifier(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi3.se.modelIdentifier;
}

bool fmi3cs_getCanHandleVariableCommunicationStepSize(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.cs.canHandleVariableCommunicationStepSize;
}

bool fmi3cs_getCanReturnEarlyAfterIntermediateUpdate(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.cs.canReturnEarlyAfterIntermediateUpdate;
}

double fmi3cs_getFixedInternalStepSize(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.cs.fixedInternalStepSize;
}

bool fmi3cs_getHasEventMode(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.cs.hasEventMode;
}

bool fmi3cs_getNeedsExecutionTool(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.cs.needsExecutionTool;
}

bool fmi3cs_getCanBeInstantiatedOnlyOncePerProcess(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.cs.canBeInstantiatedOnlyOncePerProcess;
}

bool fmi3cs_getCanGetAndSetFMUState(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.cs.canGetAndSetFMUState;
}

bool fmi3cs_getCanSerializeFMUState(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.cs.canSerializeFMUState;
}

bool fmi3cs_getProvidesDirectionalDerivative(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.cs.providesDirectionalDerivative;
}

bool fmi3cs_getProvidesAdjointDerivatives(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.cs.providesAdjointDerivatives;
}

bool fmi3cs_getProvidesPerElementDependencies(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.cs.providesPerElementDependencies;
}

bool fmi3me_getNeedsExecutionTool(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi3.me.needsExecutionTool;
}

bool fmi3me_getCanBeInstantiatedOnlyOncePerProcess(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi3.me.canBeInstantiatedOnlyOncePerProcess;
}

bool fmi3me_getCanGetAndSetFMUState(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi3.me.canGetAndSetFMUState;
}

bool fmi3me_getCanSerializeFMUState(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi3.me.canSerializeFMUState;
}

bool fmi3me_getProvidesDirectionalDerivative(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi3.me.providesDirectionalDerivative;
}

bool fmi3me_getProvidesAdjointDerivatives(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi3.me.providesAdjointDerivatives;
}

bool fmi3me_getProvidesPerElementDependencies(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi3.me.providesPerElementDependencies;
}

bool fmi3se_getNeedsExecutionTool(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi3.se.needsExecutionTool;
}

bool fmi3se_getCanBeInstantiatedOnlyOncePerProcess(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi3.se.canBeInstantiatedOnlyOncePerProcess;
}

bool fmi3se_getCanGetAndSetFMUState(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi3.se.canGetAndSetFMUState;
}

bool fmi3se_getCanSerializeFMUState(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi3.se.canSerializeFMUState;
}

bool fmi3se_getProvidesDirectionalDerivative(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi3.se.providesDirectionalDerivative;
}

bool fmi3se_getProvidesAdjointDerivatives(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi3.se.providesAdjointDerivatives;
}

bool fmi3se_getProvidesPerElementDependencies(fmiHandle *fmu)
{
    TRACEFUNC
        return fmu->fmi3.se.providesPerElementDependencies;
}

int fmi3cs_getMaxOutputDerivativeOrder(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.cs.maxOutputDerivativeOrder;
}

bool fmi3cs_getProvidesIntermediateUpdate(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.cs.providesIntermediateUpdate;
}

bool fmi3cs_getProvidesEvaluateDiscreteStates(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.cs.providesEvaluateDiscreteStates;
}

bool fmi3me_getProvidesEvaluateDiscreteStates(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.me.providesEvaluateDiscreteStates;
}

bool fmi3cs_getRecommendedIntermediateInputSmoothness(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.cs.recommendedIntermediateInputSmoothness;
}

bool fmi3me_getNeedsCompletedIntegratorStep(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.me.needsCompletedIntegratorStep;
}


const char* generateTempPath(const char *instanceName)
{
    char cwd[FILENAME_MAX];
 #ifdef _WIN32
     _getcwd(cwd, sizeof(char)*FILENAME_MAX);
 #else
     getcwd(cwd, sizeof(char)*FILENAME_MAX);
 #endif

     // Decide location for where to unzip
     char unzipLocationTemp[FILENAME_MAX] = {0};

     bool instanceNameIsAlphaNumeric = true;
     for(int i=0; i<strlen(instanceName); ++i) {
         if(!isalnum(instanceName[i])) {
             instanceNameIsAlphaNumeric = false;
         }
     }
 #ifdef _WIN32
     DWORD len = GetTempPathA(FILENAME_MAX, unzipLocationTemp);
     if (len == 0) {
        printf("Cannot find temp path, using current directory\n");
     }

     // Create a unique name for the temp folder
     char tempFileName[11] = "\0\0\0\0\0\0\0\0\0\0\0";
     srand(getpid());
     for(int i=0; i<10; ++i) {
         tempFileName[i] = rand() % 26 + 65;
     }

     strncat(unzipLocationTemp, "fmi4c_", FILENAME_MAX-strlen(unzipLocationTemp)-1);
     if(instanceNameIsAlphaNumeric) {
         strncat(unzipLocationTemp, instanceName, FILENAME_MAX-strlen(unzipLocationTemp)-1);
         strncat(unzipLocationTemp, "_", FILENAME_MAX-strlen(unzipLocationTemp)-1);
     }
     char * ds = strrchr(tempFileName, '\\');
     if (ds) {
         strncat(unzipLocationTemp, ds+1, FILENAME_MAX-strlen(unzipLocationTemp)-1);
     }
     else {
         strncat(unzipLocationTemp, tempFileName, FILENAME_MAX-strlen(unzipLocationTemp)-1);
     }
     _mkdir(unzipLocationTemp);
#else
    const char* env_tmpdir = getenv("TMPDIR");
    const char* env_tmp = getenv("TMP");
    const char* env_temp = getenv("TEMP");
    if (env_tmpdir) {
        strncat(unzipLocationTemp, env_tmpdir, FILENAME_MAX-strlen(unzipLocationTemp)-1);
    }
    else if (env_tmp) {
        strncat(unzipLocationTemp, env_tmp, FILENAME_MAX-strlen(unzipLocationTemp)-1);
    }
    else if (env_temp) {
        strncat(unzipLocationTemp, env_temp, FILENAME_MAX-strlen(unzipLocationTemp)-1);
    }
    else if (access("/tmp/", W_OK) == 0) {
        strncat(unzipLocationTemp, "/tmp/", FILENAME_MAX-strlen(unzipLocationTemp)-1);
    }
    // If no suitable temp directory is found, the current working directory will be used

    // Append / if needed
    if (strlen(unzipLocationTemp) > 0 && unzipLocationTemp[strlen(unzipLocationTemp)-1] != '/') {
        strncat(unzipLocationTemp, "/", FILENAME_MAX-strlen(unzipLocationTemp)-1);
    }

    strncat(unzipLocationTemp, "fmi4c_", FILENAME_MAX-strlen(unzipLocationTemp)-1);
    if(instanceNameIsAlphaNumeric) {
        strncat(unzipLocationTemp, instanceName, FILENAME_MAX-strlen(unzipLocationTemp)-1);
        strncat(unzipLocationTemp, "_", FILENAME_MAX-strlen(unzipLocationTemp)-1);
    }
    strncat(unzipLocationTemp, "XXXXXX", FILENAME_MAX-strlen(unzipLocationTemp)-1); // XXXXXX is for unique name by mkdtemp
    mkdtemp(unzipLocationTemp);
#endif

     return _strdup(unzipLocationTemp); //Not freed automatically!
}

bool unzipFmu(const char* fmufile, const char* instanceName, const char* unzipLocation)
{
#ifdef FMI4C_WITH_MINIZIP
    int argc = 6;
    const char *argv[6];
    argv[0] = "miniunz";
    argv[1] = "-x";
    argv[2] = "-o";
    argv[3] = fmufile;
    argv[4] = "-d";
    argv[5] = unzipLocation;

    int status = miniunz(argc, (char**)argv);
    if (status != 0) {
     printf("Failed to unzip FMU: status = %i, to location %s\n",status, unzipLocation);
     return NULL;
    }
    // miniunzip will change dir to unzipLocation, lets change back
    chdir(cwd);
#else
#ifdef _WIN32
    const int commandLength = strlen("tar -xf \"") + strlen(fmufile) + strlen("\" -C \"") + strlen(unzipLocation) + 2;

    // Allocate memory for the command
    char *command = malloc(commandLength * sizeof(char));
    if (command == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return false;
    }
    // Build the command string
    snprintf(command, commandLength, "tar -xf \"%s\" -C \"%s\"", fmufile, unzipLocation);
#else
    const int commandLength = strlen("unzip -o \"") + strlen(fmufile) + strlen("\" -d \"") + strlen(unzipLocation) + 2;

    // Allocate memory for the command
    char *command = malloc(commandLength * sizeof(char));
    if (command == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    // Build the command string
    snprintf(command, commandLength, "unzip -o \"%s\" -d \"%s\"", fmufile, unzipLocation);
#endif
    const int status = system(command);
    free(command);
    if (status != 0) {
        printf("Failed to unzip FMU: status = %i, to location %s\n",status, unzipLocation);
        return false;
    }
#endif

    return true;
}

//! @brief Loads an already unzippedFMU file
//! Parses modelDescription.xml, and then loads all required FMI functions.
//! @param fmu FMU handle
//! @returns Handle to FMU
fmiHandle *fmi4c_loadUnzippedFmu(const char *instanceName, const char *unzipLocation)
{
    fmiHandle *fmu = calloc(1, sizeof(fmiHandle)); // Using calloc to ensure all member pointers (and data) are initialized to NULL (0)
    fmu->dll = NULL;
    fmu->numAllocatedPointers = 0;
    fmu->allocatedPointers = NULL;
    fmu->version = fmiVersionUnknown;
    fmu->instanceName = duplicateAndRememberString(fmu, instanceName);
    fmu->unzippedLocation = unzipLocation;  //Already duplicated
    rememberPointer(fmu, (void*)unzipLocation);

    char cwd[FILENAME_MAX];
 #ifdef _WIN32
     _getcwd(cwd, sizeof(char)*FILENAME_MAX);
 #else
     getcwd(cwd, sizeof(char)*FILENAME_MAX);
 #endif
    chdir(fmu->unzippedLocation);
    ezxml_t rootElement = ezxml_parse_file("modelDescription.xml");
    if (rootElement == NULL) {
        printf("Failed to read modelDescription.xml in %s\n", fmu->unzippedLocation);
        fmi4c_freeFmu(fmu);
        return NULL;
    }
    if(strcmp(rootElement->name, "fmiModelDescription")) {
        printf("Wrong root tag name: %s\n", rootElement->name);
        fmi4c_freeFmu(fmu);
        return NULL;
    }
    chdir(cwd);

    // Figure out FMI version
    const char* version = NULL;
    if(parseStringAttributeEzXml(rootElement, "fmiVersion", &version)) {
        if(version[0] == '1') {
            fmu->version = fmiVersion1;
        }
        else if(version[0] == '2') {
            fmu->version = fmiVersion2;
        }
        else if(version[0] == '3') {
            fmu->version = fmiVersion3;
        }
        else {
            printf("Unsupported FMI version: %s\n", version);
            freeDuplicatedConstChar(version);
            free(fmu);
            return NULL;
        }
        freeDuplicatedConstChar(version);
    }
    else {
        printf("FMI version not specified.");
        free(fmu);
        return NULL;
    }

    if(fmu->version == fmiVersion1) {
        char resourcesLocation[FILENAME_MAX] = "file:///";
        strncat(resourcesLocation, unzipLocation, FILENAME_MAX-8);
        fmu->resourcesLocation = duplicateAndRememberString(fmu, resourcesLocation);
    }
    else if(fmu->version == fmiVersion2) {
        char resourcesLocation[FILENAME_MAX] = "file:///";
        strncat(resourcesLocation, unzipLocation, FILENAME_MAX-8);
        strncat(resourcesLocation, "/resources", FILENAME_MAX-8-strlen(unzipLocation)-1);
        fmu->resourcesLocation = duplicateAndRememberString(fmu, resourcesLocation);
    }
    else {
        char resourcesLocation[FILENAME_MAX] = "";
        strncat(resourcesLocation, unzipLocation, FILENAME_MAX);
        strncat(resourcesLocation, "/resources/", FILENAME_MAX-strlen(unzipLocation)-1);
        fmu->resourcesLocation = duplicateAndRememberString(fmu, resourcesLocation);
    }

    ezxml_free(rootElement);

    fmu->fmi1.getVersion = placeholder_fmiGetVersion;
    fmu->fmi1.getTypesPlatform = placeholder_fmiGetTypesPlatform;
    fmu->fmi1.setDebugLogging = placeholder;
    fmu->fmi1.getReal = placeholder_fmiGetReal;
    fmu->fmi1.getInteger = placeholder_fmiGetInteger;
    fmu->fmi1.getBoolean = placeholder_fmiGetBoolean;
    fmu->fmi1.getString = placeholder_fmiGetString;
    fmu->fmi1.setReal = placeholder_fmiSetReal;
    fmu->fmi1.setInteger = placeholder_fmiSetInteger;
    fmu->fmi1.setBoolean = placeholder_fmiSetBoolean;
    fmu->fmi1.setString = placeholder_fmiSetString;
    fmu->fmi1.instantiateSlave = placeholder_fmiInstantiateSlave;
    fmu->fmi1.initializeSlave = placeholder_fmiInitializeSlave;
    fmu->fmi1.terminateSlave = placeholder_fmiTerminateSlave;
    fmu->fmi1.resetSlave = placeholder_fmiResetSlave;
    fmu->fmi1.freeSlaveInstance = placeholder_fmiFreeSlaveInstance;
    fmu->fmi1.setRealInputDerivatives = placeholder_fmiSetRealInputDerivatives;
    fmu->fmi1.getRealOutputDerivatives = placeholder_fmiGetRealOutputDerivatives;
    fmu->fmi1.cancelStep = placeholder_fmiCancelStep;
    fmu->fmi1.doStep = placeholder_fmiDoStep;
    fmu->fmi1.getStatus = placeholder_fmiGetStatus;
    fmu->fmi1.getRealStatus = placeholder_fmiGetRealStatus;
    fmu->fmi1.getIntegerStatus = placeholder_fmiGetIntegerStatus;
    fmu->fmi1.getBooleanStatus = placeholder_fmiGetBooleanStatus;
    fmu->fmi1.getStringStatus = placeholder_fmiGetStringStatus;
    fmu->fmi1.getModelTypesPlatform = placeholder_fmiGetModelTypesPlatform;
    fmu->fmi1.instantiateModel = placeholder_fmiInstantiateModel;
    fmu->fmi1.freeModelInstance = placeholder_fmiFreeModelInstance;
    fmu->fmi1.setTime = placeholder_fmiSetTime;
    fmu->fmi1.setContinuousStates = placeholder_fmiSetContinuousStates;
    fmu->fmi1.completedIntegratorStep = placeholder_fmiCompletedIntegratorStep;
    fmu->fmi1.initialize = placeholder_fmiInitialize;
    fmu->fmi1.getDerivatives = placeholder_fmiGetDerivatives;
    fmu->fmi1.getEventIndicators = placeholder_fmiGetEventIndicators;
    fmu->fmi1.eventUpdate = placeholder_fmiEventUpdate;
    fmu->fmi1.getContinuousStates = placeholder_fmiGetContinuousStates;
    fmu->fmi1.getNominalContinuousStates = placeholder_fmiGetNominalContinuousStates;
    fmu->fmi1.getStateValueReferences = placeholder_fmiGetStateValueReferences;
    fmu->fmi1.terminate = placeholder_fmiTerminate;

    fmu->fmi2.getTypesPlatform = placeholder_fmi2_getTypesPlatform;
    fmu->fmi2.getVersion = placeholder_fmi2_getVersion;
    fmu->fmi2.setDebugLogging = placeholder_fmi2_setDebugLogging;
    fmu->fmi2.instantiate = placeholder_fmi2Instantiate;
    fmu->fmi2.freeInstance = placeholder_fmi2FreeInstance;
    fmu->fmi2.setupExperiment = placeholder_fmi2_setupExperiment;
    fmu->fmi2.enterInitializationMode = placeholder_fmi2EnterInitializationMode;
    fmu->fmi2.exitInitializationMode = placeholder_fmi2ExitInitializationMode;
    fmu->fmi2.terminate = placeholder_fmi2Terminate;
    fmu->fmi2.reset = placeholder_fmi2Reset;
    fmu->fmi2.getReal = placeholder_fmi2_getReal;
    fmu->fmi2.getInteger = placeholder_fmi2_getInteger;
    fmu->fmi2.getBoolean = placeholder_fmi2_getBoolean;
    fmu->fmi2.getString = placeholder_fmi2_getString;
    fmu->fmi2.setReal = placeholder_fmi2_setReal;
    fmu->fmi2.setInteger = placeholder_fmi2_setInteger;
    fmu->fmi2.setBoolean = placeholder_fmi2_setBoolean;
    fmu->fmi2.setString = placeholder_fmi2_setString;
    fmu->fmi2.getFMUstate = placeholder_fmi2_getFMUstate;
    fmu->fmi2.setFMUstate = placeholder_fmi2_setFMUstate;
    fmu->fmi2.freeFMUstate = placeholder_fmi2FreeFMUstate;
    fmu->fmi2.serializedFMUstateSize = placeholder_fmi2SerializedFMUstateSize;
    fmu->fmi2.serializeFMUstate = placeholder_fmi2SerializeFMUstate;
    fmu->fmi2.deSerializeFMUstate = placeholder_fmi2DeSerializeFMUstate;
    fmu->fmi2.getDirectionalDerivative = placeholder_fmi2_getDirectionalDerivative;
    fmu->fmi2.enterEventMode = placeholder_fmi2EnterEventMode;
    fmu->fmi2.newDiscreteStates = placeholder_fmi2NewDiscreteStates;
    fmu->fmi2.enterContinuousTimeMode = placeholder_fmi2EnterContinuousTimeMode;
    fmu->fmi2.completedIntegratorStep = placeholder_fmi2CompletedIntegratorStep;
    fmu->fmi2.setTime = placeholder_fmi2_setTime;
    fmu->fmi2.setContinuousStates = placeholder_fmi2_setContinuousStates;
    fmu->fmi2.getDerivatives = placeholder_fmi2_getDerivatives;
    fmu->fmi2.getEventIndicators = placeholder_fmi2_getEventIndicators;
    fmu->fmi2.getContinuousStates = placeholder_fmi2_getContinuousStates;
    fmu->fmi2.getNominalsOfContinuousStates = placeholder_fmi2_getNominalsOfContinuousStates;
    fmu->fmi2.setRealInputDerivatives = placeholder_fmi2_setRealInputDerivatives;
    fmu->fmi2.getRealOutputDerivatives = placeholder_fmi2_getRealOutputDerivatives;
    fmu->fmi2.doStep = placeholder_fmi2DoStep;
    fmu->fmi2.cancelStep = placeholder_fmi2CancelStep;
    fmu->fmi2.getStatus = placeholder_fmi2_getStatus;
    fmu->fmi2.getRealStatus = placeholder_fmi2_getRealStatus;
    fmu->fmi2.getIntegerStatus = placeholder_fmi2_getIntegerStatus;
    fmu->fmi2.getBooleanStatus = placeholder_fmi2_getBooleanStatus;
    fmu->fmi2.getStringStatus = placeholder_fmi2_getStringStatus;

    fmu->fmi3.getVersion = placeholder_fmi3GetVersion;
    fmu->fmi3.setDebugLogging = placeholder_fmi3SetDebugLogging;
    fmu->fmi3.instantiateModelExchange = placeholder_fmi3InstantiateModelExchange;
    fmu->fmi3.instantiateCoSimulation = placeholder_fmi3InstantiateCoSimulation;
    fmu->fmi3.instantiateScheduledExecution = placeholder_fmi3InstantiateScheduledExecution;
    fmu->fmi3.freeInstance = placeholder_fmi3FreeInstance;
    fmu->fmi3.enterInitializationMode = placeholder_fmi3EnterInitializationMode;
    fmu->fmi3.exitInitializationMode = placeholder_fmi3ExitInitializationMode;
    fmu->fmi3.terminate = placeholder_fmi3Terminate;
    fmu->fmi3.setFloat64 = placeholder_fmi3SetFloat64;
    fmu->fmi3.getFloat64 = placeholder_fmi3GetFloat64;
    fmu->fmi3.doStep = placeholder_fmi3DoStep;
    fmu->fmi3.enterEventMode = placeholder_fmi3EnterEventMode;
    fmu->fmi3.reset = placeholder_fmi3Reset;
    fmu->fmi3.getFloat32 = placeholder_fmi3GetFloat32;
    fmu->fmi3.getInt8 = placeholder_fmi3GetInt8;
    fmu->fmi3.getUInt8 = placeholder_fmi3GetUInt8;
    fmu->fmi3.getInt16 = placeholder_fmi3GetInt16;
    fmu->fmi3.getUInt16 = placeholder_fmi3GetUInt16;
    fmu->fmi3.getInt32 = placeholder_fmi3GetInt32;
    fmu->fmi3.getUInt32 = placeholder_fmi3GetUInt32;
    fmu->fmi3.getInt64 = placeholder_fmi3GetInt64;
    fmu->fmi3.getUInt64 = placeholder_fmi3GetUInt64;
    fmu->fmi3.getBoolean = placeholder_fmi3GetBoolean;
    fmu->fmi3.getString = placeholder_fmi3GetString;
    fmu->fmi3.getBinary = placeholder_fmi3GetBinary;
    fmu->fmi3.getClock = placeholder_fmi3GetClock;
    fmu->fmi3.setFloat32 = placeholder_fmi3SetFloat32;
    fmu->fmi3.setInt8 = placeholder_fmi3SetInt8;
    fmu->fmi3.setUInt8 = placeholder_fmi3SetUInt8;
    fmu->fmi3.setInt16 = placeholder_fmi3SetInt16;
    fmu->fmi3.setUInt16 = placeholder_fmi3SetUInt16;
    fmu->fmi3.setInt32 = placeholder_fmi3SetInt32;
    fmu->fmi3.setUInt32 = placeholder_fmi3SetUInt32;
    fmu->fmi3.setInt64 = placeholder_fmi3SetInt64;
    fmu->fmi3.setUInt64 = placeholder_fmi3SetUInt64;
    fmu->fmi3.setBoolean = placeholder_fmi3SetBoolean;
    fmu->fmi3.setString = placeholder_fmi3SetString;
    fmu->fmi3.setBinary = placeholder_fmi3SetBinary;
    fmu->fmi3.setClock = placeholder_fmi3SetClock;
    fmu->fmi3.getNumberOfVariableDependencies = placeholder_fmi3GetNumberOfVariableDependencies;
    fmu->fmi3.getVariableDependencies = placeholder_fmi3GetVariableDependencies;
    fmu->fmi3.getFMUState = placeholder_fmi3GetFMUState;
    fmu->fmi3.setFMUState = placeholder_fmi3SetFMUState;
    fmu->fmi3.freeFMUState = placeholder_fmi3FreeFMUState;
    fmu->fmi3.serializedFMUStateSize = placeholder_fmi3SerializedFMUStateSize;
    fmu->fmi3.serializeFMUState = placeholder_fmi3SerializeFMUState;
    fmu->fmi3.deserializeFMUState = placeholder_fmi3DeserializeFMUState;
    fmu->fmi3.getDirectionalDerivative = placeholder_fmi3GetDirectionalDerivative;
    fmu->fmi3.getAdjointDerivative = placeholder_fmi3GetAdjointDerivative;
    fmu->fmi3.enterConfigurationMode = placeholder_fmi3EnterConfigurationMode;
    fmu->fmi3.exitConfigurationMode = placeholder_fmi3ExitConfigurationMode;
    fmu->fmi3.getIntervalDecimal = placeholder_fmi3GetIntervalDecimal;
    fmu->fmi3.getIntervalFraction = placeholder_fmi3GetIntervalFraction;
    fmu->fmi3.getShiftDecimal = placeholder_fmi3GetShiftDecimal;
    fmu->fmi3.getShiftFraction = placeholder_fmi3GetShiftFraction;
    fmu->fmi3.setIntervalDecimal = placeholder_fmi3SetIntervalDecimal;
    fmu->fmi3.setIntervalFraction = placeholder_fmi3SetIntervalFraction;
    fmu->fmi3.setShiftDecimal = placeholder_fmi3SetShiftDecimal;
    fmu->fmi3.setShiftFraction = placeholder_fmi3SetShiftFraction;
    fmu->fmi3.evaluateDiscreteStates = placeholder_fmi3EvaluateDiscreteStates;
    fmu->fmi3.updateDiscreteStates = placeholder_fmi3UpdateDiscreteStates;
    fmu->fmi3.enterContinuousTimeMode = placeholder_fmi3EnterContinuousTimeMode;
    fmu->fmi3.completedIntegratorStep = placeholder_fmi3CompletedIntegratorStep;
    fmu->fmi3.setTime = placeholder_fmi3SetTime;
    fmu->fmi3.setContinuousStates = placeholder_fmi3SetContinuousStates;
    fmu->fmi3.getContinuousStateDerivatives = placeholder_fmi3GetContinuousStateDerivatives;
    fmu->fmi3.getEventIndicators = placeholder_fmi3GetEventIndicators;
    fmu->fmi3.getContinuousStates = placeholder_fmi3GetContinuousStates;
    fmu->fmi3.getNominalsOfContinuousStates = placeholder_fmi3GetNominalsOfContinuousStates;
    fmu->fmi3.getNumberOfEventIndicators = placeholder_fmi3GetNumberOfEventIndicators;
    fmu->fmi3.getNumberOfContinuousStates = placeholder_fmi3GetNumberOfContinuousStates;
    fmu->fmi3.enterStepMode = placeholder_fmi3EnterStepMode;
    fmu->fmi3.getOutputDerivatives = placeholder_fmi3GetOutputDerivatives;
    fmu->fmi3.activateModelPartition = placeholder_fmi3ActivateModelPartition;

    if(fmu->version == fmiVersion1) {
        fmu->fmi1.variables = mallocAndRememberPointer(fmu, 100*sizeof(fmi1VariableHandle));
        fmu->fmi1.variablesSize = 100;
        fmu->fmi1.numberOfVariables = 0;
        if(!parseModelDescriptionFmi1(fmu)) {
            printf("Failed to parse modelDescription.xml\n");
            free(fmu);
            return NULL;
        }
        if(!loadFunctionsFmi1(fmu)) {
            free(fmu);
            return NULL;    //Error message should already have been printed
        }
    }
    else if(fmu->version == fmiVersion2) {
        fmu->fmi2.variables = mallocAndRememberPointer(fmu, 100*sizeof(fmi2VariableHandle));
        fmu->fmi2.variablesSize = 100;
        fmu->fmi2.numberOfVariables = 0;
        if(!parseModelDescriptionFmi2(fmu)) {
            printf("Failed to parse modelDescription.xml\n");
            free(fmu);
            return NULL;
        }
    }
    else if(fmu->version == fmiVersion3) {
        fmu->fmi3.variables = mallocAndRememberPointer(fmu, 100*sizeof(fmi3VariableHandle));
        fmu->fmi3.variablesSize = 100;
        fmu->fmi3.numberOfVariables = 0;
        if(!parseModelDescriptionFmi3(fmu)) {
            printf("Failed to parse modelDescription.xml\n");
            free(fmu);
            return NULL;
        }
    }

    fmu->unzippedLocationIsTemporary = false;

    return fmu;
}

//! @brief Loads the specified FMU file
//! First unzips the FMU, then parses modelDescription.xml, and then loads all required FMI functions.
//! @param fmu FMU handle
//! @returns Handle to FMU
fmiHandle *fmi4c_loadFmu(const char *fmufile, const char* instanceName)
{
    const char* unzipLocation = generateTempPath(instanceName);

    if(!unzipFmu(fmufile, instanceName, unzipLocation)) {
        return NULL;
    }

    fmiHandle *fmu = fmi4c_loadUnzippedFmu(instanceName, unzipLocation);
    fmu->unzippedLocationIsTemporary = true;
    return fmu;
}


//! @brief Free FMU dll
//! @param fmu FMU handle
void fmi4c_freeFmu(fmiHandle *fmu)
{
    TRACEFUNC

    if (fmu->dll) {
#ifdef _WIN32
        FreeLibrary(fmu->dll);
#else
        dlclose(fmu->dll);
#endif
    }

    if (fmu->unzippedLocation && fmu->unzippedLocationIsTemporary) {
        removeDirectoryRecursively(fmu->unzippedLocation, "fmi4c_");
    }

    //Free all allocated memory
    for(int i=0; i<fmu->numAllocatedPointers; ++i) {
        free(fmu->allocatedPointers[i]);
    }
    free(fmu->allocatedPointers);
    free(fmu);
}

fmi1Type fmi1_getType(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.type;
}

const char* fmi1_getModelName(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.modelName;
}

const char* fmi1_getModelIdentifier(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.modelIdentifier;
}

const char* fmi1_getGuid(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.guid;
}

const char* fmi1_getDescription(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.description;
}

const char* fmi1_getAuthor(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.author;
}

const char* fmi1_getGenerationTool(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.generationTool;
}

const char* fmi1_getGenerationDateAndTime(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.generationDateAndTime;
}

int fmi1_getNumberOfContinuousStates(fmiHandle *fmu)
{
    return fmu->fmi1.numberOfContinuousStates;
}

int fmi1_getNumberOfEventIndicators(fmiHandle *fmu)
{
    return fmu->fmi1.numberOfEventIndicators;
}

bool fmi1_defaultStartTimeDefined(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.defaultStartTimeDefined;
}

bool fmi1_defaultStopTimeDefined(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.defaultStopTimeDefined;
}

bool fmi1_defaultToleranceDefined(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.defaultToleranceDefined;
}

double fmi1_getDefaultStartTime(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.defaultStartTime;
}

double fmi1_getDefaultStopTime(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.defaultStopTime;
}

double fmi1_getDefaultTolerance(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.defaultTolerance;
}

int fmi1_getNumberOfVariables(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.numberOfVariables;
}

fmi1VariableHandle *fmi1_getVariableByIndex(fmiHandle *fmu, int i)
{
    TRACEFUNC

    if(i >= fmu->fmi1.numberOfVariables) {
        printf("Variable index out of bounds: %i\n",i);
        return NULL;
    }
    return &fmu->fmi1.variables[i];
}

fmi1VariableHandle *fmi1_getVariableByValueReference(fmiHandle *fmu, fmi1ValueReference vr)
{
    TRACEFUNC

    for(int i=0; i<fmu->fmi1.numberOfVariables; ++i) {
        if(fmu->fmi1.variables[i].valueReference == vr) {
            return &fmu->fmi1.variables[i];
        }
    }
    printf("Variable with value reference %i not found.\n", vr);
    return NULL;
}

fmi1VariableHandle *fmi1_getVariableByName(fmiHandle *fmu, fmi1String name)
{
    for(int i=0; i<fmu->fmi1.numberOfVariables; ++i) {
        if(!strcmp(fmu->fmi1.variables[i].name, name)) {
            return &fmu->fmi1.variables[i];
        }
    }
    printf("Variable with name %s not found.\n", name);
    return NULL;
}

const char *fmi1_getVariableName(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->name;
}

const char *fmi1_getVariableDescription(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->description;
}

const char *fmi1_getVariableQuantity(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->quantity;
}

const char *fmi1_getVariableUnit(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->unit;
}

const char *fmi1_getVariableDisplayUnit(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->displayUnit;
}

bool fmi1_getVariableRelativeQuantity(fmi1VariableHandle* var)
{
    TRACEFUNC
    return var->relativeQuantity;
}

fmi1Real fmi1_getVariableMin(fmi1VariableHandle* var)
{
    TRACEFUNC
    return var->min;
}

fmi1Real fmi1_getVariableMax(fmi1VariableHandle* var)
{
    TRACEFUNC
    return var->max;
}

fmi1Real fmi1_getVariableNominal(fmi1VariableHandle* var)
{
    TRACEFUNC
    return var->nominal;
}

bool fmi1_getVariableHasStartValue(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->hasStartValue;
}

fmi1Real fmi1_getVariableStartReal(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->startReal;
}

fmi1Integer fmi1_getVariableStartInteger(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->startInteger;
}

fmi1Boolean fmi1_getVariableStartBoolean(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->startBoolean;
}

fmi1String fmi1_getVariableStartString(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->startString;
}

long fmi1_getVariableValueReference(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->valueReference;
}

fmi1Causality fmi1_getVariableCausality(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->causality;
}

fmi1Variability fmi1_getVariableVariability(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->variability;
}

fmi1Alias fmi1_getAlias(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->alias;
}

bool fmi1_getVariableIsFixed(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->fixed;
}

const char *fmi1_getTypesPlatform(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.getTypesPlatform();
}

const char *fmi1_getVersion(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.getVersion();
}

fmi1Status fmi1_setDebugLogging(fmiHandle *fmu, fmi1Boolean loggingOn)
{
    TRACEFUNC
    return fmu->fmi1.setDebugLogging(fmu->fmi1.component, loggingOn);
}

int fmi1_getNumberOfBaseUnits(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.numberOfBaseUnits;
}

fmi1BaseUnitHandle *fmi1_getBaseUnitByIndex(fmiHandle *fmu, int i)
{
    TRACEFUNC
    return &fmu->fmi1.baseUnits[i];
}

const char* fmi1_getBaseUnitUnit(fmi1BaseUnitHandle *baseUnit)
{
    TRACEFUNC
    return baseUnit->unit;
}

int fmi1_getNumberOfDisplayUnits(fmi1BaseUnitHandle *baseUnit)
{
    TRACEFUNC
    return baseUnit->numberOfDisplayUnits;
}

void fmi1_getDisplayUnitByIndex(fmi1BaseUnitHandle *baseUnit, int id, const char **displayUnit, double *gain, double *offset)
{
    *displayUnit = baseUnit->displayUnits[id].displayUnit;
    *gain = baseUnit->displayUnits[id].gain;
    *offset = baseUnit->displayUnits[id].offset;
}

fmi1Status fmi1_getReal(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1Real values[])
{
    TRACEFUNC
    return fmu->fmi1.getReal(fmu->fmi1.component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1_getInteger(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1Integer values[])
{
    TRACEFUNC
    return fmu->fmi1.getInteger(fmu->fmi1.component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1_getBoolean(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1Boolean values[])
{
    TRACEFUNC
    return fmu->fmi1.getBoolean(fmu->fmi1.component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1_getString(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1String values[])
{
    TRACEFUNC
    return fmu->fmi1.getString(fmu->fmi1.component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1_setReal(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Real values[])
{
    TRACEFUNC
    return fmu->fmi1.setReal(fmu->fmi1.component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1_setInteger(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Integer values[])
{
    TRACEFUNC
    return fmu->fmi1.setInteger(fmu->fmi1.component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1_setBoolean(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Boolean values[])
{
    TRACEFUNC
    return fmu->fmi1.setBoolean(fmu->fmi1.component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1_setString(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1String values[])
{
    TRACEFUNC
    return fmu->fmi1.setString(fmu->fmi1.component, valueReferences, nValueReferences, values);
}

bool fmi1_instantiateSlave(fmiHandle *fmu, fmi1String mimeType, fmi1Real timeOut, fmi1Boolean visible, fmi1Boolean interactive, fmi1CallbackLogger_t logger, fmi1CallbackAllocateMemory_t allocateMemory, fmi1CallbackFreeMemory_t freeMemory, fmi1StepFinished_t stepFinished, fmi1Boolean loggingOn)
{
    TRACEFUNC
    fmu->fmi1.callbacksCoSimulation.logger = logger;
    fmu->fmi1.callbacksCoSimulation.allocateMemory = allocateMemory;
    fmu->fmi1.callbacksCoSimulation.freeMemory = freeMemory;
    fmu->fmi1.callbacksCoSimulation.stepFinished = stepFinished;

    fmu->fmi1.component = fmu->fmi1.instantiateSlave(fmu->instanceName, fmu->fmi1.guid, fmu->resourcesLocation, mimeType, timeOut, visible, interactive, fmu->fmi1.callbacksCoSimulation, loggingOn);

    return (fmu->fmi1.component != NULL);
}

fmi1Status fmi1_initializeSlave(fmiHandle *fmu, fmi1Real startTime, fmi1Boolean stopTimeDefined, fmi1Real stopTime)
{
    TRACEFUNC
    return fmu->fmi1.initializeSlave(fmu->fmi1.component, startTime, stopTimeDefined, stopTime);
}

fmi1Status fmi1_terminateSlave(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.terminateSlave(fmu->fmi1.component);
}

fmi1Status fmi1_resetSlave(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.resetSlave(fmu->fmi1.component);
}

void fmi1_freeSlaveInstance(fmiHandle *fmu)
{
    TRACEFUNC
    fmu->fmi1.freeSlaveInstance(fmu->fmi1.component);
}

fmi1Status fmi1_setRealInputDerivatives(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Integer orders[], const fmi1Real values[])
{
    TRACEFUNC
    return fmu->fmi1.setRealInputDerivatives(fmu->fmi1.component, valueReferences, nValueReferences, orders, values);
}

fmi1Status fmi1_getRealOutputDerivatives(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Integer orders[], fmi1Real values[])
{
    TRACEFUNC
    return fmu->fmi1.getRealOutputDerivatives(fmu->fmi1.component, valueReferences, nValueReferences, orders, values);
}

fmi1Status fmi1_cancelStep(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.cancelStep(fmu->fmi1.component);
}

fmi1Status fmi1_doStep(fmiHandle *fmu, fmi1Real currentCommunicationPoint, fmi1Real communicationStepSize, fmi1Boolean newStep)
{
    TRACEFUNC
    return fmu->fmi1.doStep(fmu->fmi1.component, currentCommunicationPoint, communicationStepSize, newStep);
}

fmi1Status fmi1_getStatus(fmiHandle *fmu, const fmi1StatusKind statusKind, fmi1Status *value)
{
    TRACEFUNC
    return fmu->fmi1.getStatus(fmu->fmi1.component, statusKind, value);
}

fmi1Status fmi1_getRealStatus(fmiHandle *fmu, const fmi1StatusKind statusKind, fmi1Real *value)
{
    TRACEFUNC
    return fmu->fmi1.getRealStatus(fmu->fmi1.component, statusKind, value);
}

fmi1Status fmi1_getIntegerStatus(fmiHandle *fmu, const fmi1StatusKind statusKind, fmi1Integer *value)
{
    TRACEFUNC
    return fmu->fmi1.getIntegerStatus(fmu->fmi1.component, statusKind, value);
}

fmi1Status fmi1_getBooleanStatus(fmiHandle *fmu, const fmi1StatusKind statusKind, fmi1Boolean *value)
{
    TRACEFUNC
    return fmu->fmi1.getBooleanStatus(fmu->fmi1.component, statusKind, value);
}

fmi1Status fmi1_getStringStatus(fmiHandle *fmu, const fmi1StatusKind statusKind, fmi1String *value)
{
    TRACEFUNC
    return fmu->fmi1.getStringStatus(fmu->fmi1.component, statusKind, value);
}

const char *fmi1_getModelTypesPlatform(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.getModelTypesPlatform();
}


bool fmi1_instantiateModel(fmiHandle *fmu, fmi1CallbackLogger_t logger, fmi1CallbackAllocateMemory_t allocateMemory, fmi1CallbackFreeMemory_t freeMemory, fmi1Boolean loggingOn)
{
    TRACEFUNC
    fmu->fmi1.callbacksModelExchange.logger = logger;
    fmu->fmi1.callbacksModelExchange.allocateMemory = allocateMemory;
    fmu->fmi1.callbacksModelExchange.freeMemory = freeMemory;
    fmu->fmi1.component = fmu->fmi1.instantiateModel(fmu->instanceName, fmu->fmi1.guid, fmu->fmi1.callbacksModelExchange, loggingOn);

    return (fmu->fmi1.component != NULL);
}

void fmi1_freeModelInstance(fmiHandle *fmu)
{
    TRACEFUNC
    fmu->fmi1.freeModelInstance(fmu->fmi1.component);
}

fmi1Status fmi1_setTime(fmiHandle *fmu, fmi1Real time)
{
    TRACEFUNC
    return fmu->fmi1.setTime(fmu->fmi1.component, time);
}

fmi1Status fmi1_setContinuousStates(fmiHandle *fmu, const fmi1Real values[], size_t nStates)
{
    TRACEFUNC
    return fmu->fmi1.setContinuousStates(fmu->fmi1.component, values, nStates);
}

fmi1Status fmi1_completedIntegratorStep(fmiHandle *fmu, fmi1Boolean *callEventUpdate)
{
    TRACEFUNC
    return fmu->fmi1.completedIntegratorStep(fmu->fmi1.component, callEventUpdate);
}

fmi1Status fmi1_initialize(fmiHandle *fmu, fmi1Boolean toleranceControlled, fmi1Real relativeTolerance, fmi1EventInfo *eventInfo)
{
    TRACEFUNC
    return fmu->fmi1.initialize(fmu->fmi1.component, toleranceControlled, relativeTolerance, eventInfo);
}

fmi1Status fmi1_getDerivatives(fmiHandle *fmu, fmi1Real derivatives[], size_t nDerivatives)
{
    TRACEFUNC
    return fmu->fmi1.getDerivatives(fmu->fmi1.component, derivatives, nDerivatives);
}

fmi1Status fmi1_getEventIndicators(fmiHandle *fmu, fmi1Real indicators[], size_t nIndicators)
{
    TRACEFUNC
    return fmu->fmi1.getEventIndicators(fmu->fmi1.component, indicators, nIndicators);
}

fmi1Status fmi1_eventUpdate(fmiHandle *fmu, fmi1Boolean intermediateResults, fmi1EventInfo *eventInfo)
{
    TRACEFUNC
    return fmu->fmi1.eventUpdate(fmu->fmi1.component, intermediateResults, eventInfo);
}

fmi1Status fmi1_getContinuousStates(fmiHandle *fmu, fmi1Real states[], size_t nStates)
{
    TRACEFUNC
    return fmu->fmi1.getContinuousStates(fmu->fmi1.component, states, nStates);
}

fmi1Status fmi1_getNominalContinuousStates(fmiHandle *fmu, fmi1Real nominals[], size_t nNominals)
{
    TRACEFUNC
    return fmu->fmi1.getNominalContinuousStates(fmu->fmi1.component, nominals, nNominals);
}

fmi1Status fmi1_getStateValueReferences(fmiHandle *fmu, fmi1ValueReference valueReferences[], size_t nValueReferences)
{
    TRACEFUNC
    return fmu->fmi1.getStateValueReferences(fmu->fmi1.component, valueReferences, nValueReferences);
}

fmi1Status fmi1_terminate(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.terminate(fmu->fmi1.component);
}

fmi1DataType fmi1_getVariableDataType(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->datatype;
}

int fmi3_getNumberOfUnits(fmiHandle *fmu)
{
    return fmu->fmi3.numberOfUnits;
}

fmi3UnitHandle *fmi3_getUnitByIndex(fmiHandle *fmu, int i)
{
    return &fmu->fmi3.units[i];
}

const char *fmi3_getUnitName(fmi3UnitHandle *unit)
{
    return unit->name;
}

bool fmi3_hasBaseUnit(fmi3UnitHandle *unit)
{
    return (unit->baseUnit != NULL);
}

void fmi3_getBaseUnit(fmi3UnitHandle *unit, double *factor, double *offset,
                     int *kg, int *m, int *s, int *A, int *K, int *mol, int *cd, int *rad)
{
    if(unit->baseUnit != NULL) {
        *factor = unit->baseUnit->factor;
        *offset = unit->baseUnit->offset;
        *kg = unit->baseUnit->kg;
        *m= unit->baseUnit->m;
        *s= unit->baseUnit->s;
        *A= unit->baseUnit->A;
        *K= unit->baseUnit->K;
        *mol= unit->baseUnit->mol;
        *cd= unit->baseUnit->cd;
        *rad= unit->baseUnit->rad;
    }
}

double fmi3GetBaseUnitFactor(fmi3UnitHandle *unit)
{
    if(unit->baseUnit != NULL) {
        return unit->baseUnit->factor;
    }
    return 0;
}

double fmi3GetBaseUnitOffset(fmi3UnitHandle *unit)
{
    if(unit->baseUnit != NULL) {
        return unit->baseUnit->offset;
    }
    return 0;
}

int fmi3GetBaseUnit_kg(fmi3UnitHandle *unit)
{
    if(unit->baseUnit != NULL) {
        return unit->baseUnit->kg;
    }
    return 0;
}

int fmi3GetBaseUnit_m(fmi3UnitHandle *unit)
{
    if(unit->baseUnit != NULL) {
        return unit->baseUnit->m;
    }
    return 0;
}

int fmi3GetBaseUnit_s(fmi3UnitHandle *unit)
{
    if(unit->baseUnit != NULL) {
        return unit->baseUnit->s;
    }
    return 0;
}

int fmi3GetBaseUnit_A(fmi3UnitHandle *unit)
{
    if(unit->baseUnit != NULL) {
        return unit->baseUnit->A;
    }
    return 0;
}

int fmi3GetBaseUnit_K(fmi3UnitHandle *unit)
{
    if(unit->baseUnit != NULL) {
        return unit->baseUnit->K;
    }
    return 0;
}

int fmi3GetBaseUnit_mol(fmi3UnitHandle *unit)
{
    if(unit->baseUnit != NULL) {
        return unit->baseUnit->mol;
    }
    return 0;
}

int fmi3GetBaseUnit_cd(fmi3UnitHandle *unit)
{
    if(unit->baseUnit != NULL) {
        return unit->baseUnit->cd;
    }
    return 0;
}

int fmi3GetBaseUnit_rad(fmi3UnitHandle *unit)
{
    if(unit->baseUnit != NULL) {
        return unit->baseUnit->rad;
    }
    return 0;
}

int fmi3_getNumberOfDisplayUnits(fmi3UnitHandle *unit)
{
    return unit->numberOfDisplayUnits;
}

void fmi3_getDisplayUnitByIndex(fmi3UnitHandle *unit, int id, const char **name, double *factor, double *offset, bool *inverse)
{
    *name = unit->displayUnits[id].name;
    *factor = unit->displayUnits[id].factor;
    *offset = unit->displayUnits[id].offset;
    *inverse = unit->displayUnits[id].inverse;
}

const char *fmi3GetDisplayUnitName(fmi3DisplayUnitHandle *displayUnit)
{
    return displayUnit->name;
}

double fmi3GetDisplayUnitFactor(fmi3DisplayUnitHandle *displayUnit)
{
    return displayUnit->factor;
}

double fmi3GetDisplayUnitOffset(fmi3DisplayUnitHandle *displayUnit)
{
    return displayUnit->offset;
}

bool fmi3GetDisplayUnitInverse(fmi3DisplayUnitHandle *displayUnit)
{
    return displayUnit->inverse;
}

void fmi3_getFloat64Type(fmiHandle *fmu,
                        const char *name,
                        const char **description,
                        const char **quantity,
                        const char **unit,
                        const char **displayUnit,
                        bool *relativeQuantity,
                        bool *unbounded,
                        double *min,
                        double *max,
                        double *nominal)
{
    for(int i=0; i<fmu->fmi3.numberOfFloat64Types; ++i) {
        if(!strcmp(fmu->fmi3.float64Types[i].name, name)) {
            *description = fmu->fmi3.float64Types[i].description;
            *quantity= fmu->fmi3.float64Types[i].quantity;
            *unit = fmu->fmi3.float64Types[i].unit;
            *displayUnit = fmu->fmi3.float64Types[i].displayUnit;
            *relativeQuantity = fmu->fmi3.float64Types[i].relativeQuantity;
            *unbounded = fmu->fmi3.float64Types[i].unbounded;
            *min = fmu->fmi3.float64Types[i].min;
            *max = fmu->fmi3.float64Types[i].max;
            *nominal = fmu->fmi3.float64Types[i].nominal;
        }
    }
}

void fmi3_getFloat32Type(fmiHandle *fmu,
                        const char *name,
                        const char **description,
                        const char **quantity,
                        const char **unit,
                        const char **displayUnit,
                        bool *relativeQuantity,
                        bool *unbounded,
                        float *min,
                        float *max,
                        float *nominal)
{
    for(int i=0; i<fmu->fmi3.numberOfFloat32Types; ++i) {
        if(!strcmp(fmu->fmi3.float32Types[i].name, name)) {
            *description = fmu->fmi3.float32Types[i].description;
            *quantity= fmu->fmi3.float32Types[i].quantity;
            *unit = fmu->fmi3.float32Types[i].unit;
            *displayUnit = fmu->fmi3.float32Types[i].displayUnit;
            *relativeQuantity = fmu->fmi3.float32Types[i].relativeQuantity;
            *unbounded = fmu->fmi3.float32Types[i].unbounded;
            *min = fmu->fmi3.float32Types[i].min;
            *max = fmu->fmi3.float32Types[i].max;
            *nominal = fmu->fmi3.float32Types[i].nominal;
        }
    }
}

void fmi3_getInt64Type(fmiHandle *fmu,
                      const char *name,
                      const char **description,
                      const char **quantity,
                      int64_t *min,
                      int64_t *max)
{
    for(int i=0; i<fmu->fmi3.numberOfInt64Types; ++i) {
        if(!strcmp(fmu->fmi3.int64Types[i].name, name)) {
            *description = fmu->fmi3.int64Types[i].description;
            *quantity= fmu->fmi3.int64Types[i].quantity;
            *min = fmu->fmi3.int64Types[i].min;
            *max = fmu->fmi3.int64Types[i].max;
        }
    }
}

void fmi3_getInt32Type(fmiHandle *fmu,
                      const char *name,
                      const char **description,
                      const char **quantity,
                      int32_t *min,
                      int32_t *max)
{
    for(int i=0; i<fmu->fmi3.numberOfInt32Types; ++i) {
        if(!strcmp(fmu->fmi3.int32Types[i].name, name)) {
            *description = fmu->fmi3.int32Types[i].description;
            *quantity= fmu->fmi3.int32Types[i].quantity;
            *min = fmu->fmi3.int32Types[i].min;
            *max = fmu->fmi3.int32Types[i].max;
        }
    }
}

void fmi3_getInt16Type(fmiHandle *fmu,
                      const char *name,
                      const char **description,
                      const char **quantity,
                      int16_t *min,
                      int16_t *max)
{
    for(int i=0; i<fmu->fmi3.numberOfInt16Types; ++i) {
        if(!strcmp(fmu->fmi3.int16Types[i].name, name)) {
            *description = fmu->fmi3.int16Types[i].description;
            *quantity= fmu->fmi3.int16Types[i].quantity;
            *min = fmu->fmi3.int16Types[i].min;
            *max = fmu->fmi3.int16Types[i].max;
        }
    }
}

void fmi3_getInt8Type(fmiHandle *fmu,
                      const char *name,
                      const char **description,
                      const char **quantity,
                      int8_t *min,
                      int8_t *max)
{
    for(int i=0; i<fmu->fmi3.numberOfInt8Types; ++i) {
        if(!strcmp(fmu->fmi3.int8Types[i].name, name)) {
            *description = fmu->fmi3.int8Types[i].description;
            *quantity= fmu->fmi3.int8Types[i].quantity;
            *min = fmu->fmi3.int8Types[i].min;
            *max = fmu->fmi3.int8Types[i].max;
        }
    }
}

void fmi3_getUInt64Type(fmiHandle *fmu,
                      const char *name,
                      const char **description,
                      const char **quantity,
                      uint64_t *min,
                      uint64_t *max)
{
    for(int i=0; i<fmu->fmi3.numberOfUInt64Types; ++i) {
        if(!strcmp(fmu->fmi3.uint64Types[i].name, name)) {
            *description = fmu->fmi3.uint64Types[i].description;
            *quantity= fmu->fmi3.uint64Types[i].quantity;
            *min = fmu->fmi3.uint64Types[i].min;
            *max = fmu->fmi3.uint64Types[i].max;
        }
    }
}

void fmi3_getUInt32Type(fmiHandle *fmu,
                      const char *name,
                      const char **description,
                      const char **quantity,
                      uint32_t *min,
                      uint32_t *max)
{
    for(int i=0; i<fmu->fmi3.numberOfUInt32Types; ++i) {
        if(!strcmp(fmu->fmi3.uint32Types[i].name, name)) {
            *description = fmu->fmi3.uint32Types[i].description;
            *quantity= fmu->fmi3.uint32Types[i].quantity;
            *min = fmu->fmi3.uint32Types[i].min;
            *max = fmu->fmi3.uint32Types[i].max;
        }
    }
}

void fmi3_getUInt16Type(fmiHandle *fmu,
                      const char *name,
                      const char **description,
                      const char **quantity,
                      uint16_t *min,
                      uint16_t *max)
{
    for(int i=0; i<fmu->fmi3.numberOfUInt16Types; ++i) {
        if(!strcmp(fmu->fmi3.uint16Types[i].name, name)) {
            *description = fmu->fmi3.uint16Types[i].description;
            *quantity= fmu->fmi3.uint16Types[i].quantity;
            *min = fmu->fmi3.uint16Types[i].min;
            *max = fmu->fmi3.uint16Types[i].max;
        }
    }
}

void fmi3_getUInt8Type(fmiHandle *fmu,
                      const char *name,
                      const char **description,
                      const char **quantity,
                      uint8_t *min,
                      uint8_t *max)
{
    for(int i=0; i<fmu->fmi3.numberOfUInt8Types; ++i) {
        if(!strcmp(fmu->fmi3.uint8Types[i].name, name)) {
            *description = fmu->fmi3.uint8Types[i].description;
            *quantity= fmu->fmi3.uint8Types[i].quantity;
            *min = fmu->fmi3.uint8Types[i].min;
            *max = fmu->fmi3.uint8Types[i].max;
        }
    }
}

void fmi3_getBooleanType(fmiHandle *fmu,
                      const char *name,
                      const char **description)
{
    for(int i=0; i<fmu->fmi3.numberOfBooleanTypes; ++i) {
        if(!strcmp(fmu->fmi3.booleanTypes[i].name, name)) {
            *description = fmu->fmi3.booleanTypes[i].description;
        }
    }
}

void fmi3_getStringType(fmiHandle *fmu,
                      const char *name,
                      const char **description)
{
    for(int i=0; i<fmu->fmi3.numberOfStringTypes; ++i) {
        if(!strcmp(fmu->fmi3.stringTypes[i].name, name)) {
            *description = fmu->fmi3.stringTypes[i].description;
        }
    }
}

void fmi3_getBinaryType(fmiHandle *fmu,
                       const char *name,
                       const char **description,
                       const char **mimeType,
                       uint32_t *maxSize)
{
    for(int i=0; i<fmu->fmi3.numberOfBinaryTypes; ++i) {
        if(!strcmp(fmu->fmi3.binaryTypes[i].name, name)) {
            *description = fmu->fmi3.binaryTypes[i].description;
            *mimeType = fmu->fmi3.binaryTypes[i].mimeType;
            *maxSize = fmu->fmi3.binaryTypes[i].maxSize;
        }
    }
}

void fmi3_getEnumerationType(fmiHandle *fmu,
                            const char *name,
                            const char **description,
                            const char **quantity,
                            int64_t *min,
                            int64_t *max,
                            int *numberOfItems)
{
    for(int i=0; i<fmu->fmi3.numberOfEnumerationTypes; ++i) {
        if(!strcmp(fmu->fmi3.enumTypes[i].name, name)) {
            *description = fmu->fmi3.enumTypes[i].description;
            *quantity = fmu->fmi3.enumTypes[i].quantity;
            *min = fmu->fmi3.enumTypes[i].min;
            *max = fmu->fmi3.enumTypes[i].max;
        }
    }
    // TODO numberOfItems
}

void fmi3_getClockType(fmiHandle *fmu,
                      const char *name,
                      const char **description,
                      bool *canBeDeactivated,
                      uint32_t *priority,
                      fmi3IntervalVariability *intervalVariability,
                      float *intervalDecimal,
                      float *shiftDecimal,
                      bool *supportsFraction,
                      uint64_t *resolution,
                      uint64_t *intervalCounter,
                      uint64_t *shiftCounter)
{
    for(int i=0; i<fmu->fmi3.numberOfClockTypes; ++i) {
        if(!strcmp(fmu->fmi3.clockTypes[i].name, name)) {
            *description = fmu->fmi3.clockTypes[i].description;
            *canBeDeactivated = fmu->fmi3.clockTypes[i].canBeDeactivated;
            *priority = fmu->fmi3.clockTypes[i].priority;
            *intervalVariability = fmu->fmi3.clockTypes[i].intervalVariability;
            *intervalDecimal = fmu->fmi3.clockTypes[i].intervalDecimal;
            *shiftDecimal = fmu->fmi3.clockTypes[i].shiftDecimal;
            *supportsFraction = fmu->fmi3.clockTypes[i].supportsFraction;
            *resolution = fmu->fmi3.clockTypes[i].resolution;
            *intervalCounter = fmu->fmi3.clockTypes[i].intervalCounter;
            *shiftCounter = fmu->fmi3.clockTypes[i].shiftCounter;
        }
    }
}

void fmi3_getEnumerationItem(fmiHandle *fmu, const char *typeName, int itemId, const char **itemName, int64_t *value, const char **description)
{
    for(int i=0; i<fmu->fmi3.numberOfEnumerationTypes; ++i) {
        if(!strcmp(fmu->fmi3.enumTypes[i].name, typeName)) {
            if(i < fmu->fmi3.enumTypes[i].numberOfItems) {
                *itemName = fmu->fmi3.enumTypes[i].items[itemId].name;
                *value = fmu->fmi3.enumTypes[i].items[itemId].value;
                *description = fmu->fmi3.enumTypes[i].items[itemId].description;
            }
        }
    }
}


int fmi3_getNumberOfLogCategories(fmiHandle *fmu)
{
    return fmu->fmi3.numberOfLogCategories;
}

void fmi3_getLogCategory(fmiHandle *fmu, int id, const char **name, const char **description)
{
    if(id < fmu->fmi3.numberOfLogCategories) {
        *name = fmu->fmi3.logCategories[id].name;
        *description = fmu->fmi3.logCategories[id].description;
    }
}

int fmi3_getNumberOfModelStructureOutputs(fmiHandle *fmu)
{
    return fmu->fmi3.numberOfOutputs;
}

void fmi3_getModelStructureOutput(fmiHandle *fmu,
                       int id,
                       fmi3ValueReference *vr,
                       int *numberOfDependencies,
                       bool *dependencyKindsDefined)
{
    if(id < fmu->fmi3.numberOfOutputs) {
        *vr = fmu->fmi3.outputs[id].valueReference;
        *numberOfDependencies = fmu->fmi3.outputs[id].numberOfDependencies;
        *dependencyKindsDefined = fmu->fmi3.outputs[id].dependencyKindsDefined;
    }
}

fmi3ValueReference fmi3_getModelStructureOutputDependency(fmiHandle *fmu, int outputId, int dependencyId, bool *ok)
{
    *ok = false;
    if(outputId < fmu->fmi3.numberOfOutputs &&
       dependencyId < fmu->fmi3.outputs[outputId].numberOfDependencies) {
        *ok = true;
        return fmu->fmi3.outputs[outputId].dependencies[dependencyId];
    }
    return 0;
}

fmi3ValueReference fmi3_getModelStructureOutputDependencyKind(fmiHandle *fmu, int outputId, int dependencyId, bool *ok)
{
    *ok = false;
    if(outputId < fmu->fmi3.numberOfOutputs &&
       dependencyId < fmu->fmi3.outputs[outputId].numberOfDependencies &&
       fmu->fmi3.outputs[outputId].dependencyKindsDefined) {
        *ok = true;
        return fmu->fmi3.outputs[outputId].dependencyKinds[dependencyId];
    }
    return 0;
}

int fmi3_getNumberOfModelStructureContinuousStateDerivatives(fmiHandle *fmu)
{
    return fmu->fmi3.numberOfContinuousStateDerivatives;
}

void fmi3_getModelStructureContinuousStateDerivative(fmiHandle *fmu,
                                                    int id,
                                                    fmi3ValueReference *vr,
                                                    int *numberOfDependencies,
                                                    bool *dependencyKindsDefined)
{
    if(id < fmu->fmi3.numberOfContinuousStateDerivatives) {
        *vr = fmu->fmi3.continuousStateDerivatives[id].valueReference;
        *numberOfDependencies = fmu->fmi3.continuousStateDerivatives[id].numberOfDependencies;
        *dependencyKindsDefined = fmu->fmi3.continuousStateDerivatives[id].dependencyKindsDefined;
    }
}

fmi3ValueReference fmi3_getModelStructureContinuousStateDerivativeDependency(fmiHandle *fmu,
                                                                            int derId,
                                                                            int dependencyId,
                                                                            bool *ok)
{
    *ok = false;
    if(derId < fmu->fmi3.numberOfContinuousStateDerivatives &&
       dependencyId < fmu->fmi3.continuousStateDerivatives[derId].numberOfDependencies) {
        *ok = true;
        return fmu->fmi3.continuousStateDerivatives[derId].dependencies[dependencyId];
    }
    return 0;
}

fmi3ValueReference fmi3_getModelStructureContinuousStateDerivativeDependencyKind(fmiHandle *fmu,
                                                                                int derId,
                                                                                int dependencyId,
                                                                                bool *ok)
{
    *ok = false;
    if(derId < fmu->fmi3.numberOfContinuousStateDerivatives &&
       dependencyId < fmu->fmi3.continuousStateDerivatives[derId].numberOfDependencies &&
       fmu->fmi3.continuousStateDerivatives[derId].dependencyKindsDefined) {
        *ok = true;
        return fmu->fmi3.continuousStateDerivatives[derId].dependencyKinds[dependencyId];
    }
    return 0;
}

int fmi3_getNumberOfModelStructureClockedStates(fmiHandle *fmu)
{
    return fmu->fmi3.numberOfClockedStates;
}

void fmi3_getModelStructureClockedState(fmiHandle *fmu,
                                      int id,
                                      fmi3ValueReference *vr,
                                      int *numberOfDependencies,
                                      bool *dependencyKindsDefined)
{
    if(id < fmu->fmi3.numberOfClockedStates) {
        *vr = fmu->fmi3.clockedStates[id].valueReference;
        *numberOfDependencies = fmu->fmi3.clockedStates[id].numberOfDependencies;
        *dependencyKindsDefined = fmu->fmi3.clockedStates[id].dependencyKindsDefined;
    }
}

fmi3ValueReference fmi3_getModelStructureClockedStateDependency(fmiHandle *fmu,
                                                               int clockId,
                                                               int dependencyId,
                                                               bool *ok)
{
    *ok = false;
    if(clockId < fmu->fmi3.numberOfClockedStates &&
       dependencyId < fmu->fmi3.clockedStates[clockId].numberOfDependencies) {
        *ok = true;
        return fmu->fmi3.clockedStates[clockId].dependencies[dependencyId];
    }
    return 0;
}

fmi3ValueReference fmi3_getModelStructureClockedStateDependencyKind(fmiHandle *fmu,
                                                                   int clockId,
                                                                   int dependencyId,
                                                                   bool *ok)
{
    *ok = false;
    if(clockId < fmu->fmi3.numberOfClockedStates &&
       dependencyId < fmu->fmi3.clockedStates[clockId].numberOfDependencies &&
       fmu->fmi3.clockedStates[clockId].dependencyKindsDefined) {
        *ok = true;
        return fmu->fmi3.clockedStates[clockId].dependencyKinds[dependencyId];
    }
    return 0;
}

int fmi3_getNumberOfModelStructureInitialUnknowns(fmiHandle *fmu)
{
    return fmu->fmi3.numberOfInitialUnknowns;
}

void fmi3_getModelStructureInitialUnknown(fmiHandle *fmu,
                                      int id,
                                      fmi3ValueReference *vr,
                                      int *numberOfDependencies,
                                      bool *dependencyKindsDefined)
{
    if(id < fmu->fmi3.numberOfInitialUnknowns) {
        *vr = fmu->fmi3.initialUnknowns[id].valueReference;
        *numberOfDependencies = fmu->fmi3.initialUnknowns[id].numberOfDependencies;
        *dependencyKindsDefined = fmu->fmi3.initialUnknowns[id].dependencyKindsDefined;
    }
}

fmi3ValueReference fmi3_getModelStructureInitialUnknownDependency(fmiHandle *fmu,
                                                                 int unknownId,
                                                                 int dependencyId,
                                                                 bool *ok)
{
    *ok = false;
    if(unknownId < fmu->fmi3.numberOfInitialUnknowns &&
       dependencyId < fmu->fmi3.initialUnknowns[unknownId].numberOfDependencies) {
        *ok = true;
        return fmu->fmi3.initialUnknowns[unknownId].dependencies[dependencyId];
    }
    return 0;
}

fmi3ValueReference fmi3_modelStructureGetInitialUnknownDependencyKind(fmiHandle *fmu,
                                                                     int unknownId,
                                                                     int dependencyId,
                                                                     bool *ok)
{
    *ok = false;
    if(unknownId < fmu->fmi3.numberOfInitialUnknowns &&
       dependencyId < fmu->fmi3.initialUnknowns[unknownId].numberOfDependencies &&
       fmu->fmi3.initialUnknowns[unknownId].dependencyKindsDefined) {
        *ok = true;
        return fmu->fmi3.initialUnknowns[unknownId].dependencyKinds[dependencyId];
    }
    return 0;
}

int fmi3_getNumberOfModelStructureEventIndicators(fmiHandle *fmu)
{
    return fmu->fmi3.numberOfEventIndicators;
}

void fmi3_getModelStructureEventIndicator(fmiHandle *fmu,
                           int id,
                           fmi3ValueReference *vr,
                           int *numberOfDependencies,
                           bool *dependencyKindsDefined)
{
    if(id < fmu->fmi3.numberOfEventIndicators) {
        *vr = fmu->fmi3.eventIndicators[id].valueReference;
        *numberOfDependencies = fmu->fmi3.eventIndicators[id].numberOfDependencies;
        *dependencyKindsDefined = fmu->fmi3.eventIndicators[id].dependencyKindsDefined;
    }
}

fmi3ValueReference fmi3_getModelStructureEventIndicatorDependency(fmiHandle *fmu,
                                                                 int indicatorId,
                                                                 int dependencyId,
                                                                 bool *ok)
{
    *ok = false;
    if(indicatorId < fmu->fmi3.numberOfEventIndicators &&
       dependencyId < fmu->fmi3.eventIndicators[indicatorId].numberOfDependencies) {
        *ok = true;
        return fmu->fmi3.eventIndicators[indicatorId].dependencies[dependencyId];
    }
    return 0;
}

fmi3ValueReference fmi3_getModelStructureEventIndicatorDependencyKind(fmiHandle *fmu,
                                                                     int indicatorId,
                                                                     int dependencyId,
                                                                     bool *ok)
{
    *ok = false;
    if(indicatorId < fmu->fmi3.numberOfEventIndicators &&
       dependencyId < fmu->fmi3.eventIndicators[indicatorId].numberOfDependencies &&
       fmu->fmi3.eventIndicators[indicatorId].dependencyKindsDefined) {
        *ok = true;
        return fmu->fmi3.eventIndicators[indicatorId].dependencyKinds[dependencyId];
    }
    return 0;
}
