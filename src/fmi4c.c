#include "fmi4c_private.h"
#include "fmi4c_placeholders.h"
#include "fmi4c_utils.h"

#include "minizip/miniunz.h"
#include "ezxml/ezxml.h"

#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#ifndef _WIN32
    #include <dlfcn.h>
    #include <unistd.h>
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


//! @brief Parses modelDescription.xml for FMI 1
//! @param fmu FMU handle
//! @returns True if parsing was successful
bool parseModelDescriptionFmi1(fmiHandle *fmu)
{
    fmu->fmi1.defaultStartTimeDefined = false;
    fmu->fmi1.defaultStopTimeDefined = false;
    fmu->fmi1.defaultToleranceDefined = false;

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
    parseStringAttributeEzXml(rootElement, "modelName",                 &fmu->fmi1.modelName);
    parseStringAttributeEzXml(rootElement, "modelIdentifier",           &fmu->fmi1.modelIdentifier);
    parseStringAttributeEzXml(rootElement, "guid",                      &fmu->fmi1.guid);
    parseStringAttributeEzXml(rootElement, "description",               &fmu->fmi1.description);
    parseStringAttributeEzXml(rootElement, "author",                    &fmu->fmi1.author);
    parseStringAttributeEzXml(rootElement, "version",                   &fmu->fmi1.version);
    parseStringAttributeEzXml(rootElement, "generationtool",            &fmu->fmi1.generationTool);
    parseStringAttributeEzXml(rootElement, "generationDateAndTime",     &fmu->fmi1.generationDateAndTime);
    parseStringAttributeEzXml(rootElement, "variableNamingConvention",  &fmu->fmi1.variableNamingConvention);
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

            parseStringAttributeEzXml(varElement, "name", &var.name);
            parseInt64AttributeEzXml(varElement, "valueReference", &var.valueReference);
            parseStringAttributeEzXml(varElement, "description", &var.description);

            const char* causality = "internal";
            parseStringAttributeEzXml(varElement, "causality", &causality);
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
                return false;
            }

            const char* variability = "continuous";
            parseStringAttributeEzXml(varElement, "variability", &variability);
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
                printf("Unknown variability: %s\n", causality);
                return false;
            }

            const char* alias = "noAlias";
            parseStringAttributeEzXml(varElement, "alias", &alias);
            if(!strcmp(alias, "alias")) {
                var.alias = fmi1AliasAlias;
            }
            else if(!strcmp(alias, "negatedAlias")) {
                var.alias = fmi1AliasNegatedAlias;
            }
            else if(!strcmp(alias, "noAlias")) {
                var.alias = fmi1AliasNoAlias;
            }

            ezxml_t realElement = ezxml_child(varElement, "Real");
            if(realElement) {
                fmu->fmi1.hasRealVariables = true;
                var.datatype = fmi1DataTypeReal;
                parseFloat64AttributeEzXml(realElement, "start", &var.startReal);
                parseBooleanAttributeEzXml(realElement, "fixed", &var.fixed);
            }

            ezxml_t integerElement = ezxml_child(varElement, "Integer");
            if(integerElement) {
                fmu->fmi1.hasIntegerVariables = true;
                var.datatype = fmi1DataTypeInteger;
                parseInt32AttributeEzXml(integerElement, "start", &var.startInteger);
                parseBooleanAttributeEzXml(integerElement, "fixed", &var.fixed);
            }

            ezxml_t booleanElement = ezxml_child(varElement, "Boolean");
            if(booleanElement) {
                fmu->fmi1.hasBooleanVariables = true;
                var.datatype = fmi1DataTypeBoolean;
                bool startBoolean;
                parseBooleanAttributeEzXml(booleanElement, "start", &startBoolean);
                var.startBoolean = startBoolean;
                parseBooleanAttributeEzXml(booleanElement, "fixed", &var.fixed);
            }

            ezxml_t stringElement = ezxml_child(varElement, "String");
            if(stringElement) {
                fmu->fmi1.hasStringVariables = true;
                var.datatype = fmi1DataTypeString;
                parseStringAttributeEzXml(stringElement, "start", &var.startString);
                parseBooleanAttributeEzXml(stringElement, "fixed", &var.fixed);
            }

            if(fmu->fmi1.numberOfVariables >= fmu->fmi1.variablesSize) {
                fmu->fmi1.variablesSize *= 2;
                fmu->fmi1.variables = realloc(fmu->fmi1.variables, fmu->fmi1.variablesSize*sizeof(fmi1VariableHandle));
            }

            fmu->fmi1.variables[fmu->fmi1.numberOfVariables] = var;
            fmu->fmi1.numberOfVariables++;
        }
    }

    chdir(cwd);

    return true;
}


//! @brief Parses modelDescription.xml for FMI 2
//! @param fmu FMU handle
//! @returns True if parsing was successful
bool parseModelDescriptionFmi2(fmiHandle *fmu)
{
    fmu->fmi2.defaultStartTimeDefined = false;
    fmu->fmi2.defaultStopTimeDefined = false;
    fmu->fmi2.defaultToleranceDefined = false;
    fmu->fmi2.defaultStepSizeDefined = false;

    fmu->fmi2.defaultStartTimeDefined = false;
    fmu->fmi2.defaultStopTimeDefined = false;
    fmu->fmi2.defaultToleranceDefined = false;
    fmu->fmi2.defaultStepSizeDefined = false;

    fmu->fmi2.numberOfContinuousStates = 0;

    fmu->fmi2.supportsCoSimulation = false;
    fmu->fmi2.supportsModelExchange = false;

    fmu->fmi2.needsExecutionTool = false;
    fmu->fmi2.canHandleVariableCommunicationStepSize = false;
    fmu->fmi2.canInterpolateInputs = false;
    fmu->fmi2.canRunAsynchronuously = false;
    fmu->fmi2.canBeInstantiatedOnlyOncePerProcess = false;
    fmu->fmi2.canNotUseMemoryManagementFunctions = false;
    fmu->fmi2.canGetAndSetFMUState = false;
    fmu->fmi2.canSerializeFMUState = false;
    fmu->fmi2.providesDirectionalDerivative = false;
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
    chdir(fmu->instanceName);

    ezxml_t rootElement = ezxml_parse_file("modelDescription.xml");
    if(strcmp(rootElement->name, "fmiModelDescription")) {
        printf("Wrong root tag name: %s\n", rootElement->name);
        return false;
    }

    //Parse attributes in <fmiModelDescription>
    parseStringAttributeEzXml(rootElement, "modelName",                 &fmu->fmi2.modelName);
    parseStringAttributeEzXml(rootElement, "guid",                      &fmu->fmi2.guid);
    parseStringAttributeEzXml(rootElement, "description",               &fmu->fmi2.description);
    parseStringAttributeEzXml(rootElement, "author",                    &fmu->fmi2.author);
    parseStringAttributeEzXml(rootElement, "version",                   &fmu->fmi2.version);
    parseStringAttributeEzXml(rootElement, "copyright",                 &fmu->fmi2.copyright);
    parseStringAttributeEzXml(rootElement, "license",                   &fmu->fmi2.license);
    parseStringAttributeEzXml(rootElement, "generationtool",            &fmu->fmi2.generationTool);
    parseStringAttributeEzXml(rootElement, "generationDateAndTime",     &fmu->fmi2.generationDateAndTime);
    parseStringAttributeEzXml(rootElement, "variableNamingConvention",  &fmu->fmi2.variableNamingConvention);
    parseInt32AttributeEzXml(rootElement, "numberOfEventIndicators",    &fmu->fmi2.numberOfEventIndicators);

    ezxml_t cosimElement = ezxml_child(rootElement, "CoSimulation");
    if(cosimElement) {
        fmu->fmi2.supportsCoSimulation = true;
        parseStringAttributeEzXml(cosimElement, "modelIdentifier",                          &fmu->fmi2.modelIdentifier);
        parseBooleanAttributeEzXml(cosimElement, "needsExecutionTool",                      &fmu->fmi2.needsExecutionTool);
        parseBooleanAttributeEzXml(cosimElement, "canHandleVariableCommunicationStepSize",  &fmu->fmi2.canHandleVariableCommunicationStepSize);
        parseBooleanAttributeEzXml(cosimElement, "canInterpolateInputs",                    &fmu->fmi2.canInterpolateInputs);
        parseInt32AttributeEzXml(cosimElement, "maxOutputDerivativeOrder",                  &fmu->fmi2.maxOutputDerivativeOrder);
        parseBooleanAttributeEzXml(cosimElement, "canRunAsynchronuously",                   &fmu->fmi2.canRunAsynchronuously);
        parseBooleanAttributeEzXml(cosimElement, "canBeInstantiatedOnlyOncePerProcess",     &fmu->fmi2.canBeInstantiatedOnlyOncePerProcess);
        parseBooleanAttributeEzXml(cosimElement, "canNotUseMemoryManagementFunctions",      &fmu->fmi2.canNotUseMemoryManagementFunctions);
        parseBooleanAttributeEzXml(cosimElement, "canGetAndSetFMUState",                    &fmu->fmi2.canGetAndSetFMUState);
        parseBooleanAttributeEzXml(cosimElement, "canSerializeFMUState",                    &fmu->fmi2.canSerializeFMUState);
        parseBooleanAttributeEzXml(cosimElement, "providesDirectionalDerivative",           &fmu->fmi2.providesDirectionalDerivative);
    }

    ezxml_t modelExchangeElement = ezxml_child(rootElement, "ModelExchange");
    if(modelExchangeElement) {
        fmu->fmi2.supportsModelExchange = true;
        parseStringAttributeEzXml(cosimElement, "modelIdentifier",                          &fmu->fmi2.modelIdentifier);
        parseBooleanAttributeEzXml(cosimElement, "needsExecutionTool",                      &fmu->fmi2.needsExecutionTool);
        parseBooleanAttributeEzXml(cosimElement, "completedIntegratorStepNotNeeded",        &fmu->fmi2.completedIntegratorStepNotNeeded);
        parseBooleanAttributeEzXml(cosimElement, "canBeInstantiatedOnlyOncePerProcess",     &fmu->fmi2.canBeInstantiatedOnlyOncePerProcess);
        parseBooleanAttributeEzXml(cosimElement, "canNotUseMemoryManagementFunctions",      &fmu->fmi2.canNotUseMemoryManagementFunctions);
        parseBooleanAttributeEzXml(cosimElement, "canGetAndSetFMUState",                    &fmu->fmi2.canGetAndSetFMUState);
        parseBooleanAttributeEzXml(cosimElement, "canSerializeFMUState",                    &fmu->fmi2.canSerializeFMUState);
        parseBooleanAttributeEzXml(cosimElement, "providesDirectionalDerivative",           &fmu->fmi2.providesDirectionalDerivative);
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

            parseStringAttributeEzXml(varElement, "name", &var.name);
            parseInt64AttributeEzXml(varElement, "valueReference", &var.valueReference);
            parseStringAttributeEzXml(varElement, "description", &var.description);
            parseBooleanAttributeEzXml(varElement, "canHandleMultipleSetPerTimeInstant", &var.canHandleMultipleSetPerTimeInstant);

            const char* causality = "local";
            parseStringAttributeEzXml(varElement, "causality", &causality);
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
                return false;
            }

            const char* variability = "continuous";
            parseStringAttributeEzXml(varElement, "variability", &variability);
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
                return false;
            }

            const char* initial = NULL;
            parseStringAttributeEzXml(varElement, "initial", &initial);
            if(initial && !strcmp(initial, "approx")) {
                var.initial = fmi2InitialApprox;
            }
            else if(initial && !strcmp(initial, "calculated")) {
                var.initial = fmi2InitialCalculated;
            }
            else if(initial && !strcmp(initial, "exact")) {
                var.initial = fmi2InitialExact;
            }
            else if(initial) {
                printf("Unknown initial: %s\n", initial);
                return false;
            }

            ezxml_t realElement = ezxml_child(varElement, "Real");
            if(realElement) {
                fmu->fmi2.hasRealVariables = true;
                var.datatype = fmi2DataTypeReal;
                parseFloat64AttributeEzXml(realElement, "start", &var.startReal);
                if(parseInt32AttributeEzXml(realElement, "derivative", &var.derivative)) {
                    fmu->fmi2.numberOfContinuousStates++;
                }
            }

            ezxml_t integerElement = ezxml_child(varElement, "Integer");
            if(integerElement) {
                fmu->fmi2.hasIntegerVariables = true;
                var.datatype = fmi2DataTypeInteger;
                parseInt32AttributeEzXml(integerElement, "start", &var.startInteger);
            }

            ezxml_t booleanElement = ezxml_child(varElement, "Boolean");
            if(booleanElement) {
                fmu->fmi2.hasBooleanVariables = true;
                var.datatype = fmi2DataTypeBoolean;
                bool startBoolean;
                parseBooleanAttributeEzXml(booleanElement, "start", &startBoolean);
                var.startBoolean = startBoolean;
            }

            ezxml_t stringElement = ezxml_child(varElement, "String");
            if(stringElement) {
                fmu->fmi2.hasStringVariables = true;
                var.datatype = fmi2DataTypeString;
                parseStringAttributeEzXml(stringElement, "start", &var.startString);
            }

            if(fmu->fmi2.numberOfVariables >= fmu->fmi2.variablesSize) {
                fmu->fmi2.variablesSize *= 2;
                fmu->fmi2.variables = realloc(fmu->fmi2.variables, fmu->fmi2.variablesSize*sizeof(fmi2VariableHandle));
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
    fmu->fmi3.variableNamingConvention = "flat";

    fmu->fmi3.supportsCoSimulation = false;
    fmu->fmi3.supportsModelExchange = false;
    fmu->fmi3.supportsScheduledExecution = false;

    fmu->fmi3.defaultStartTimeDefined = false;
    fmu->fmi3.defaultStopTimeDefined = false;
    fmu->fmi3.defaultToleranceDefined = false;
    fmu->fmi3.defaultStepSizeDefined = false;

    fmu->fmi3.needsExecutionTool = false;
    fmu->fmi3.canBeInstantiatedOnlyOncePerProcess = false;
    fmu->fmi3.canGetAndSetFMUState = false;
    fmu->fmi3.canSerializeFMUState = false;
    fmu->fmi3.providesDirectionalDerivative = false;
    fmu->fmi3.providesAdjointDerivatives = false;
    fmu->fmi3.providesPerElementDependencies = false;
    fmu->fmi3.maxOutputDerivativeOrder = 0;
    fmu->fmi3.providesIntermediateUpdate = false;
    fmu->fmi3.mightReturnEarlyFromDoStep = false;
    fmu->fmi3.providesEvaluateDiscreteStates = false;
    fmu->fmi3.recommendedIntermediateInputSmoothness = 0;
    fmu->fmi3.canHandleVariableCommunicationStepSize = false;
    fmu->fmi3.canReturnEarlyAfterIntermediateUpdate = false;
    fmu->fmi3.fixedInternalStepSize = 0;
    fmu->fmi3.hasEventMode = false;
    fmu->fmi3.needsCompletedIntegratorStep = false;
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
    chdir(fmu->instanceName);

    char tempPath[FILENAME_MAX];
#ifdef _WIN32
    _getcwd(tempPath, sizeof(char)*FILENAME_MAX);
#else
    getcwd(tempPath, sizeof(char)*FILENAME_MAX);
#endif
    fmu->unzippedLocation = strdup(tempPath);


    strcat(tempPath, "/resources");
    fmu->resourcesLocation = strdup(tempPath);

    ezxml_t rootElement = ezxml_parse_file("modelDescription.xml");
    if(strcmp(rootElement->name, "fmiModelDescription")) {
        printf("Wrong root tag name: %s\n", rootElement->name);
        return false;
    }

    parseStringAttributeEzXml(rootElement, "modelName",                 &fmu->fmi3.modelName);
    parseStringAttributeEzXml(rootElement, "instantiationToken",        &fmu->fmi3.instantiationToken);
    parseStringAttributeEzXml(rootElement, "description",               &fmu->fmi3.description);
    parseStringAttributeEzXml(rootElement, "author",                    &fmu->fmi3.author);
    parseStringAttributeEzXml(rootElement, "version",                   &fmu->fmi3.version);
    parseStringAttributeEzXml(rootElement, "copyright",                 &fmu->fmi3.copyright);
    parseStringAttributeEzXml(rootElement, "license",                   &fmu->fmi3.license);
    parseStringAttributeEzXml(rootElement, "generationtool",            &fmu->fmi3.generationTool);
    parseStringAttributeEzXml(rootElement, "generationDateAndTime",     &fmu->fmi3.generationDateAndTime);
    parseStringAttributeEzXml(rootElement, "variableNamingConvention",  &fmu->fmi3.variableNamingConvention);

    ezxml_t cosimElement = ezxml_child(rootElement, "CoSimulation");
    if(cosimElement) {
        fmu->fmi3.supportsCoSimulation = true;
        parseStringAttributeEzXml(cosimElement,  "modelIdentifier",                         &fmu->fmi3.modelIdentifier);
        parseBooleanAttributeEzXml(cosimElement, "needsExecutionTool",                      &fmu->fmi3.needsExecutionTool);
        parseBooleanAttributeEzXml(cosimElement, "canBeInstantiatedOnlyOncePerProcess",     &fmu->fmi3.canBeInstantiatedOnlyOncePerProcess);
        parseBooleanAttributeEzXml(cosimElement, "canGetAndSetFMUState",                    &fmu->fmi3.canGetAndSetFMUState);
        parseBooleanAttributeEzXml(cosimElement, "canSerializeFMUState",                    &fmu->fmi3.canSerializeFMUState);
        parseBooleanAttributeEzXml(cosimElement, "providesDirectionalDerivative",           &fmu->fmi3.providesDirectionalDerivative);
        parseBooleanAttributeEzXml(cosimElement, "providesAdjointDerivatives",              &fmu->fmi3.providesAdjointDerivatives);
        parseBooleanAttributeEzXml(cosimElement, "providesPerElementDependencies",          &fmu->fmi3.providesPerElementDependencies);
        parseInt32AttributeEzXml(cosimElement,   "maxOutputDerivativeOrder",                &fmu->fmi3.maxOutputDerivativeOrder);
        parseBooleanAttributeEzXml(cosimElement, "providesIntermediateUpdate",              &fmu->fmi3.providesIntermediateUpdate);
        parseBooleanAttributeEzXml(cosimElement, "mightReturnEarlyFromDoStep",              &fmu->fmi3.mightReturnEarlyFromDoStep);
        parseBooleanAttributeEzXml(cosimElement, "providesEvaluateDiscreteStates",          &fmu->fmi3.providesEvaluateDiscreteStates);
        parseInt32AttributeEzXml(cosimElement,   "recommendedIntermediateInputSmoothness",  &fmu->fmi3.recommendedIntermediateInputSmoothness);
        parseBooleanAttributeEzXml(cosimElement, "canHandleVariableCommunicationStepSize",  &fmu->fmi3.canHandleVariableCommunicationStepSize);
        parseBooleanAttributeEzXml(cosimElement, "canReturnEarlyAfterIntermediateUpdate",   &fmu->fmi3.canReturnEarlyAfterIntermediateUpdate);
        parseFloat64AttributeEzXml(cosimElement, "fixedInternalStepSize",                   &fmu->fmi3.fixedInternalStepSize);
        parseBooleanAttributeEzXml(cosimElement, "hasEventMode",                            &fmu->fmi3.hasEventMode);
    }

    ezxml_t modelExchangeElement = ezxml_child(rootElement, "ModelExchange");
    if(modelExchangeElement) {
        fmu->fmi3.supportsModelExchange = true;
        parseStringAttributeEzXml(modelExchangeElement,  "modelIdentifier",                     &fmu->fmi3.modelIdentifier);
        parseBooleanAttributeEzXml(modelExchangeElement, "needsExecutionTool",                  &fmu->fmi3.needsExecutionTool);
        parseBooleanAttributeEzXml(modelExchangeElement, "canBeInstantiatedOnlyOncePerProcess", &fmu->fmi3.canBeInstantiatedOnlyOncePerProcess);
        parseBooleanAttributeEzXml(modelExchangeElement, "canGetAndSetFMUState",                &fmu->fmi3.canGetAndSetFMUState);
        parseBooleanAttributeEzXml(modelExchangeElement, "canSerializeFMUState",                &fmu->fmi3.canSerializeFMUState);
        parseBooleanAttributeEzXml(modelExchangeElement, "providesDirectionalDerivative",       &fmu->fmi3.providesDirectionalDerivative);
        parseBooleanAttributeEzXml(modelExchangeElement, "providesAdjointDerivatives",          &fmu->fmi3.providesAdjointDerivatives);
        parseBooleanAttributeEzXml(modelExchangeElement, "providesPerElementDependencies",      &fmu->fmi3.providesPerElementDependencies);
        parseBooleanAttributeEzXml(modelExchangeElement, "needsCompletedIntegratorStep",        &fmu->fmi3.needsCompletedIntegratorStep);
        parseBooleanAttributeEzXml(modelExchangeElement, "providesEvaluateDiscreteStates",      &fmu->fmi3.providesEvaluateDiscreteStates);
    }

    ezxml_t scheduledExecutionElement = ezxml_child(rootElement, "ScheduledExecution");
    if(scheduledExecutionElement) {
        fmu->fmi3.supportsScheduledExecution = true;
        parseStringAttributeEzXml(scheduledExecutionElement,  "modelIdentifier",                        &fmu->fmi3.modelIdentifier);
        parseBooleanAttributeEzXml(scheduledExecutionElement, "needsExecutionTool",                     &fmu->fmi3.needsExecutionTool);
        parseBooleanAttributeEzXml(scheduledExecutionElement, "canBeInstantiatedOnlyOncePerProcess",    &fmu->fmi3.canBeInstantiatedOnlyOncePerProcess);
        parseBooleanAttributeEzXml(scheduledExecutionElement, "canGetAndSetFMUState",                   &fmu->fmi3.canGetAndSetFMUState);
        parseBooleanAttributeEzXml(scheduledExecutionElement, "canSerializeFMUState",                   &fmu->fmi3.canSerializeFMUState);
        parseBooleanAttributeEzXml(scheduledExecutionElement, "providesDirectionalDerivative",          &fmu->fmi3.providesDirectionalDerivative);
        parseBooleanAttributeEzXml(scheduledExecutionElement, "providesAdjointDerivatives",             &fmu->fmi3.providesAdjointDerivatives);
        parseBooleanAttributeEzXml(scheduledExecutionElement, "providesPerElementDependencies",         &fmu->fmi3.providesPerElementDependencies);
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
        fmu->fmi3.units = malloc(fmu->fmi3.numberOfUnits*sizeof(fmi3UnitHandle));
        int i=0;
        for(ezxml_t unitElement = unitDefinitionsElement->child; unitElement; unitElement = unitElement->next) {
            if(strcmp(unitElement->name, "Unit")) {
                continue;   //Wrong element name
            }
            fmi3UnitHandle unit;
            unit.baseUnit = NULL;
            unit.displayUnits = NULL;
            parseStringAttributeEzXml(unitElement, "name", &unit.name);
            unit.numberOfDisplayUnits = 0;
            for(ezxml_t unitSubElement = unitElement->child; unitSubElement; unitSubElement = unitSubElement->next) {
                if(!strcmp(unitSubElement->name, "BaseUnit")) {
                    unit.baseUnit = malloc(sizeof(fmi3BaseUnit));
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
            unit.displayUnits = malloc(unit.numberOfDisplayUnits*sizeof(fmi3DisplayUnitHandle));
            int j=0;
            for(ezxml_t unitSubElement = unitElement->child; unitSubElement; unitSubElement = unitSubElement->next) {
                if(!strcmp(unitSubElement->name, "DisplayUnit")) {
                    unit.displayUnits[j].factor = 1;
                    unit.displayUnits[j].offset = 0;
                    unit.displayUnits[j].inverse = false;
                    parseStringAttributeEzXml(unitSubElement,  "name",      &unit.displayUnits[j].name);
                    parseFloat64AttributeEzXml(unitSubElement, "factor",    &unit.displayUnits[j].factor);
                    parseFloat64AttributeEzXml(unitSubElement, "offset",    &unit.displayUnits[j].offset);
                    parseBooleanAttributeEzXml(unitSubElement, "inverse",   &unit.displayUnits[j].inverse);
                }
                ++j;
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
        for(ezxml_t typeElement = typeDefinitionsElement->child; typeElement; typeElement = typeElement->next) {
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
        fmu->fmi3.float64Types = malloc(fmu->fmi3.numberOfFloat64Types*sizeof(fmi3Float64Type));
        fmu->fmi3.float32Types = malloc(fmu->fmi3.numberOfFloat32Types*sizeof(fmi3Float32Type));
        fmu->fmi3.int64Types = malloc(fmu->fmi3.numberOfInt64Types*sizeof(fmi3Int64Type));
        fmu->fmi3.int32Types = malloc(fmu->fmi3.numberOfInt32Types*sizeof(fmi3Int32Type));
        fmu->fmi3.int16Types = malloc(fmu->fmi3.numberOfInt16Types*sizeof(fmi3Int16Type));
        fmu->fmi3.int8Types = malloc(fmu->fmi3.numberOfInt8Types*sizeof(fmi3Int8Type));
        fmu->fmi3.uint64Types = malloc(fmu->fmi3.numberOfUInt64Types*sizeof(fmi3UInt64Type));
        fmu->fmi3.uint32Types = malloc(fmu->fmi3.numberOfUInt32Types*sizeof(fmi3UInt32Type));
        fmu->fmi3.uint16Types = malloc(fmu->fmi3.numberOfUInt16Types*sizeof(fmi3UInt16Type));
        fmu->fmi3.uint8Types = malloc(fmu->fmi3.numberOfUInt8Types*sizeof(fmi3UInt8Type));
        fmu->fmi3.booleanTypes = malloc(fmu->fmi3.numberOfBooleanTypes*sizeof(fmi3BooleanType));
        fmu->fmi3.stringTypes = malloc(fmu->fmi3.numberOfStringTypes*sizeof(fmi3StringType));
        fmu->fmi3.binaryTypes = malloc(fmu->fmi3.numberOfBinaryTypes*sizeof(fmi3BinaryType));
        fmu->fmi3.enumTypes = malloc(fmu->fmi3.numberOfEnumerationTypes*sizeof(fmi3EnumerationType));
        fmu->fmi3.clockTypes = malloc(fmu->fmi3.numberOfClockTypes*sizeof(fmi3ClockType));

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
        for(ezxml_t typeElement = typeDefinitionsElement->child; typeElement; typeElement = typeElement->next) {
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
                parseStringAttributeEzXml(typeElement, "name", &fmu->fmi3.float64Types[iFloat64].name);
                parseStringAttributeEzXml(typeElement, "description", &fmu->fmi3.float64Types[iFloat64].description);
                parseStringAttributeEzXml(typeElement, "quantity", &fmu->fmi3.float64Types[iFloat64].quantity);
                parseStringAttributeEzXml(typeElement, "unit", &fmu->fmi3.float64Types[iFloat64].unit);
                parseStringAttributeEzXml(typeElement, "displayUnit", &fmu->fmi3.float64Types[iFloat64].displayUnit);
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
                parseStringAttributeEzXml(typeElement, "name", &fmu->fmi3.float32Types[iFloat32].name);
                parseStringAttributeEzXml(typeElement, "description", &fmu->fmi3.float32Types[iFloat32].description);
                parseStringAttributeEzXml(typeElement, "quantity", &fmu->fmi3.float32Types[iFloat32].quantity);
                parseStringAttributeEzXml(typeElement, "unit", &fmu->fmi3.float32Types[iFloat32].unit);
                parseStringAttributeEzXml(typeElement, "displayUnit", &fmu->fmi3.float32Types[iFloat32].displayUnit);
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
                parseStringAttributeEzXml(typeElement, "name", &fmu->fmi3.int64Types[iInt64].name);
                parseInt64AttributeEzXml(typeElement, "min", &fmu->fmi3.int64Types[iInt64].min);
                parseInt64AttributeEzXml(typeElement, "max", &fmu->fmi3.int64Types[iInt64].max);
                ++iInt64;
            }
            else if(!strcmp(typeElement->name, "Int32Type")) {
                fmu->fmi3.int32Types[iInt32].name = "";
                fmu->fmi3.int32Types[iInt32].min = -INT32_MAX;
                fmu->fmi3.int32Types[iInt32].max = INT32_MAX;
                parseStringAttributeEzXml(typeElement, "name", &fmu->fmi3.int32Types[iInt32].name);
                parseInt32AttributeEzXml(typeElement, "min", &fmu->fmi3.int32Types[iInt32].min);
                parseInt32AttributeEzXml(typeElement, "max", &fmu->fmi3.int32Types[iInt32].max);
                ++iInt32;
            }
            else if(!strcmp(typeElement->name, "Int16Type")) {
                fmu->fmi3.int16Types[iInt16].name = "";
                fmu->fmi3.int16Types[iInt16].min = -INT16_MAX;
                fmu->fmi3.int16Types[iInt16].max = INT16_MAX;
                parseStringAttributeEzXml(typeElement, "name", &fmu->fmi3.int16Types[iInt16].name);
                parseInt16AttributeEzXml(typeElement, "min", &fmu->fmi3.int16Types[iInt16].min);
                parseInt16AttributeEzXml(typeElement, "max", &fmu->fmi3.int16Types[iInt16].max);
                ++iInt16;
            }
            else if(!strcmp(typeElement->name, "Int8Type")) {
                fmu->fmi3.int8Types[iInt8].name = "";
                fmu->fmi3.int8Types[iInt8].min = -INT8_MAX;
                fmu->fmi3.int8Types[iInt8].max = INT8_MAX;
                parseStringAttributeEzXml(typeElement, "name", &fmu->fmi3.int8Types[iInt8].name);
                parseInt8AttributeEzXml(typeElement, "min", &fmu->fmi3.int8Types[iInt8].min);
                parseInt8AttributeEzXml(typeElement, "max", &fmu->fmi3.int8Types[iInt8].max);
                ++iInt8;
            }
            else if(!strcmp(typeElement->name, "UInt64Type")) {
                fmu->fmi3.uint64Types[iUInt64].name = "";
                fmu->fmi3.uint64Types[iUInt64].min = 0;
                fmu->fmi3.uint64Types[iUInt64].max = UINT64_MAX;
                parseStringAttributeEzXml(typeElement, "name", &fmu->fmi3.uint64Types[iUInt64].name);
                parseUInt64AttributeEzXml(typeElement, "min", &fmu->fmi3.uint64Types[iUInt64].min);
                parseUInt64AttributeEzXml(typeElement, "max", &fmu->fmi3.uint64Types[iUInt64].max);
                ++iUInt64;
            }
            else if(!strcmp(typeElement->name, "UInt32Type")) {
                fmu->fmi3.uint32Types[iUInt32].name = "";
                fmu->fmi3.uint32Types[iUInt32].min = 0;
                fmu->fmi3.uint32Types[iUInt32].max = UINT32_MAX;
                parseStringAttributeEzXml(typeElement, "name", &fmu->fmi3.uint32Types[iUInt32].name);
                parseUInt32AttributeEzXml(typeElement, "min", &fmu->fmi3.uint32Types[iUInt32].min);
                parseUInt32AttributeEzXml(typeElement, "max", &fmu->fmi3.uint32Types[iUInt32].max);
                ++iUInt32;
            }
            else if(!strcmp(typeElement->name, "UInt16Type")) {
                fmu->fmi3.uint16Types[iUInt16].name = "";
                fmu->fmi3.uint16Types[iUInt16].min = 0;
                fmu->fmi3.uint16Types[iUInt16].max = UINT16_MAX;
                parseStringAttributeEzXml(typeElement, "name", &fmu->fmi3.uint16Types[iUInt16].name);
                parseUInt16AttributeEzXml(typeElement, "min", &fmu->fmi3.uint16Types[iUInt16].min);
                parseUInt16AttributeEzXml(typeElement, "max", &fmu->fmi3.uint16Types[iUInt16].max);
                ++iUInt16;
            }
            else if(!strcmp(typeElement->name, "UInt8Type")) {
                fmu->fmi3.uint8Types[iUInt8].name = "";
                fmu->fmi3.uint8Types[iUInt8].min = 0;
                fmu->fmi3.uint8Types[iUInt8].max = UINT8_MAX;
                parseStringAttributeEzXml(typeElement, "name", &fmu->fmi3.uint8Types[iUInt8].name);
                parseUInt8AttributeEzXml(typeElement, "min", &fmu->fmi3.uint8Types[iUInt8].min);
                parseUInt8AttributeEzXml(typeElement, "max", &fmu->fmi3.uint8Types[iUInt8].max);
                ++iUInt8;
            }
            else if(!strcmp(typeElement->name, "BooleanType")) {
                fmu->fmi3.booleanTypes[iBoolean].name = "";
                fmu->fmi3.booleanTypes[iBoolean].description = "";
                parseStringAttributeEzXml(typeElement, "name", &fmu->fmi3.booleanTypes[iBoolean].name);
                parseStringAttributeEzXml(typeElement, "description", &fmu->fmi3.booleanTypes[iBoolean].description);
                ++iBoolean;
            }
            else if(!strcmp(typeElement->name, "StringType")) {
                fmu->fmi3.stringTypes[iString].name = "";
                fmu->fmi3.stringTypes[iString].description = "";
                parseStringAttributeEzXml(typeElement, "name", &fmu->fmi3.stringTypes[iString].name);
                parseStringAttributeEzXml(typeElement, "description", &fmu->fmi3.stringTypes[iString].description);
                ++iString;
            }
            else if(!strcmp(typeElement->name, "BinaryType")) {
                fmu->fmi3.binaryTypes[iBinary].name = "";
                fmu->fmi3.binaryTypes[iBinary].description = "";
                fmu->fmi3.binaryTypes[iBinary].mimeType = "application/octet-stream";
                fmu->fmi3.binaryTypes[iBinary].maxSize = UINT32_MAX;
                parseStringAttributeEzXml(typeElement, "name", &fmu->fmi3.binaryTypes[iBinary].name);
                parseStringAttributeEzXml(typeElement, "description", &fmu->fmi3.binaryTypes[iBinary].description);
                parseStringAttributeEzXml(typeElement, "mimeType", &fmu->fmi3.binaryTypes[iBinary].mimeType);
                parseUInt32AttributeEzXml(typeElement, "maxSize", &fmu->fmi3.binaryTypes[iBinary].maxSize);
                ++iBinary;
            }
            else if(!strcmp(typeElement->name, "EnumerationType")) {
                fmu->fmi3.enumTypes[iEnum].name = "";
                fmu->fmi3.enumTypes[iEnum].description = "";
                fmu->fmi3.enumTypes[iEnum].quantity = "";
                fmu->fmi3.enumTypes[iEnum].min = -INT64_MAX;
                fmu->fmi3.enumTypes[iEnum].max = INT64_MAX;
                parseStringAttributeEzXml(typeElement, "name", &fmu->fmi3.enumTypes[iEnum].name);
                parseStringAttributeEzXml(typeElement, "description", &fmu->fmi3.enumTypes[iEnum].description);
                parseStringAttributeEzXml(typeElement, "quantity", &fmu->fmi3.enumTypes[iEnum].quantity);
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
                fmu->fmi3.enumTypes[iEnum].items = malloc(fmu->fmi3.enumTypes[iEnum].numberOfItems*sizeof(fmi3EnumerationItem));

                //Read data for enumeration items
                int iItem = 0;
                for(ezxml_t itemElement = typeElement->child; itemElement; itemElement = itemElement->next) {
                    if(!strcmp(itemElement->name, "Item")) {
                        parseStringAttributeEzXml(itemElement, "name", &fmu->fmi3.enumTypes[iEnum].items[iItem].name);
                        parseInt64AttributeEzXml(itemElement, "value", &fmu->fmi3.enumTypes[iEnum].items[iItem].value);
                        parseStringAttributeEzXml(itemElement, "description", &fmu->fmi3.enumTypes[iEnum].items[iItem].description);
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
                parseStringAttributeEzXml(typeElement, "name", &fmu->fmi3.clockTypes[iClock].name);
                parseStringAttributeEzXml(typeElement, "description", &fmu->fmi3.clockTypes[iClock].description);
                parseBooleanAttributeEzXml(typeElement, "canBeDeactivated", &fmu->fmi3.clockTypes[iClock].canBeDeactivated);
                parseUInt32AttributeEzXml(typeElement, "priority", &fmu->fmi3.clockTypes[iClock].priority);
                const char* intervalVariability;
                parseStringAttributeEzXml(typeElement, "intervalVariability", &intervalVariability);
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
                    return false;
                }
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
        fmu->fmi3.logCategories = malloc(fmu->fmi3.numberOfLogCategories*sizeof(fmi3LogCategory));

        //Read log categories
        int i=0;
        for(ezxml_t logCategoryElement = logCategoriesElement->child; logCategoryElement; logCategoryElement = logCategoryElement->next) {
            parseStringAttributeEzXml(logCategoryElement, "name", &fmu->fmi3.logCategories[i].name);
            parseStringAttributeEzXml(logCategoryElement, "description", &fmu->fmi3.logCategories[i].description);
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
        for(ezxml_t varElement = modelVariablesElement->child; varElement; varElement = varElement->next) {
            fmi3VariableHandle var;
            var.name = NULL;
            var.description = NULL;
            var.quantity = NULL;
            var.unit = NULL;
            var.displayUnit = NULL;
            var.canHandleMultipleSetPerTimeInstant = false; //Default value if attribute not defined

            parseStringAttributeEzXml(varElement, "name", &var.name);
            parseInt64AttributeEzXml(varElement, "valueReference", &var.valueReference);
            parseStringAttributeEzXml(varElement, "description", &var.description);
            parseBooleanAttributeEzXml(varElement, "canHandleMultipleSetPerTimeInstant", &var.canHandleMultipleSetPerTimeInstant);
            parseBooleanAttributeEzXml(varElement, "intermediateUpdate", &var.intermediateUpdate);
            parseInt32AttributeEzXml(varElement, "previous", &var.previous);
            parseStringAttributeEzXml(varElement, "declaredType", &var.declaredType);
            const char* clocks = "";
            parseStringAttributeEzXml(varElement, "clocks", &clocks);
            char* nonConstClocks = strdup(clocks);

            //Count number of clocks
            var.numberOfClocks = 0;
            if(nonConstClocks[0]) {
                var.numberOfClocks = 1;
            }
            for(int i=0; nonConstClocks[i]; ++i) {
                if(nonConstClocks[i] == ' ') {
                    ++var.numberOfClocks;
                }
            }

            //Allocate memory for clocks
            var.clocks = malloc(var.numberOfClocks*sizeof(int));

            //Read clocks
            const char* delim = " ";
            for(int i=0; i<var.numberOfClocks; ++i) {
                if(i == 0) {
                    var.clocks[i] = atoi(strtok(nonConstClocks, delim));
                }
                else {
                    var.clocks[i] = atoi(strtok(NULL, delim));
                }
            }

            //Figure out data type
            if(!strcmp(varElement->name, "Float64")) {
                var.datatype = fmi3DataTypeFloat64;
                fmu->fmi3.hasFloat64Variables = true;
                parseFloat64AttributeEzXml(varElement, "start", &var.startFloat64);
            }
            else if(!strcmp(varElement->name, "Float32")) {
                var.datatype = fmi3DataTypeFloat32;
                fmu->fmi3.hasFloat32Variables = true;
                parseFloat32AttributeEzXml(varElement, "start", &var.startFloat32);
            }
            else if(!strcmp(varElement->name, "Int64")) {
                var.datatype = fmi3DataTypeInt64;
                fmu->fmi3.hasInt64Variables = true;
                parseInt64AttributeEzXml(varElement, "start", &var.startInt64);
            }
            else if(!strcmp(varElement->name, "Int32")) {
                var.datatype = fmi3DataTypeInt32;
                fmu->fmi3.hasInt32Variables = true;
                parseInt32AttributeEzXml(varElement, "start", &var.startInt32);
            }
            else if(!strcmp(varElement->name, "Int16")) {
                var.datatype = fmi3DataTypeInt16;
                fmu->fmi3.hasInt16Variables = true;
                parseInt16AttributeEzXml(varElement, "start", &var.startInt16);
            }
            else if(!strcmp(varElement->name, "Int8")) {
                var.datatype = fmi3DataTypeInt8;
                fmu->fmi3.hasInt8Variables = true;
                parseInt8AttributeEzXml(varElement, "start", &var.startInt8);
            }
            else if(!strcmp(varElement->name, "UInt64")) {
                var.datatype = fmi3DataTypeUInt64;
                fmu->fmi3.hasUInt64Variables = true;
                parseUInt64AttributeEzXml(varElement, "start", &var.startUInt64);
            }
            else if(!strcmp(varElement->name, "UInt32")) {
                var.datatype = fmi3DataTypeUInt32;
                fmu->fmi3.hasUInt32Variables = true;
                parseUInt32AttributeEzXml(varElement, "start", &var.startUInt32);
            }
            else if(!strcmp(varElement->name, "UInt16")) {
                var.datatype = fmi3DataTypeUInt16;
                fmu->fmi3.hasUInt16Variables = true;
                parseUInt16AttributeEzXml(varElement, "start", &var.startUInt16);
            }
            else if(!strcmp(varElement->name, "UInt8")) {
                var.datatype = fmi3DataTypeUInt8;
                fmu->fmi3.hasUInt8Variables = true;
                parseUInt8AttributeEzXml(varElement, "start", &var.startUInt8);
            }
            else if(!strcmp(varElement->name, "Boolean")) {
                var.datatype = fmi3DataTypeBoolean;
                fmu->fmi3.hasBooleanVariables = true;
                parseBooleanAttributeEzXml(varElement, "start", &var.startBoolean);
            }
            else if(!strcmp(varElement->name, "String")) {
                var.datatype = fmi3DataTypeString;
                fmu->fmi3.hasStringVariables = true;
                parseStringAttributeEzXml(varElement, "start", &var.startString);
            }
            else if(!strcmp(varElement->name, "Binary")) {
                var.datatype = fmi3DataTypeBinary;
                fmu->fmi3.hasBinaryVariables = true;
                parseUInt8AttributeEzXml(varElement, "start", var.startBinary);
            }
            else if(!strcmp(varElement->name, "Enumeration")) {
                var.datatype = fmi3DataTypeEnumeration;
                fmu->fmi3.hasEnumerationVariables = true;
                parseInt64AttributeEzXml(varElement, "start", &var.startEnumeration);
            }
            else if(!strcmp(varElement->name, "Clock")) {
                var.datatype = fmi3DataTypeClock;
                fmu->fmi3.hasClockVariables = true;
                parseBooleanAttributeEzXml(varElement, "start", &var.startClock);
            }

            const char* causality = "local";
            parseStringAttributeEzXml(varElement, "causality", &causality);
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
                return false;
            }

            const char* variability;
            if(var.datatype == fmi3DataTypeFloat64 || var.datatype == fmi3DataTypeFloat32) {
                variability = "continuous";
            }
            else {
                variability = "discrete";
            }
            parseStringAttributeEzXml(varElement, "variability", &variability);
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
                return false;
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
                    return false;
                }
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
                parseStringAttributeEzXml(varElement,  "quantity", &var.quantity);
                parseFloat64AttributeEzXml(varElement,  "min", &var.min);
                parseFloat64AttributeEzXml(varElement,  "max", &var.max);
            }

            //Parse arguments only in float type
            if(var.datatype == fmi3DataTypeFloat64 ||
               var.datatype == fmi3DataTypeFloat32) {
                parseStringAttributeEzXml(varElement,  "unit", &var.unit);
                parseStringAttributeEzXml(varElement,  "displayUnit", &var.displayUnit);
                parseBooleanAttributeEzXml(varElement, "relativeQuantity", &var.relativeQuantity);
                parseBooleanAttributeEzXml(varElement, "unbounded", &var.unbounded);
                parseFloat64AttributeEzXml(varElement,  "nominal", &var.nominal);
                parseInt32AttributeEzXml(varElement, "derivative", &var.derivative);
                parseBooleanAttributeEzXml(varElement, "reinit", &var.reInit);
            }

            //Parse arguments only in binary type
            if(var.datatype == fmi3DataTypeBinary) {
                parseStringAttributeEzXml(varElement, "mimeType", &var.mimeType);
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
                const char* intervalVariability;
                parseStringAttributeEzXml(varElement, "intervalVariability", &intervalVariability);
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
                    return false;
                }
            }

            if(fmu->fmi3.numberOfVariables >= fmu->fmi3.variablesSize) {
                fmu->fmi3.variablesSize *= 2;
                fmu->fmi3.variables = realloc(fmu->fmi3.variables, fmu->fmi3.variablesSize*sizeof(fmi3VariableHandle));
            }

            fmu->fmi3.variables[fmu->fmi3.numberOfVariables] = var;
            fmu->fmi3.numberOfVariables++;
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
        fmu->fmi3.outputs = malloc(fmu->fmi3.numberOfOutputs*sizeof(fmi3ModelStructureElement));
        fmu->fmi3.continuousStateDerivatives = malloc(fmu->fmi3.numberOfContinuousStateDerivatives*sizeof(fmi3ModelStructureElement));
        fmu->fmi3.clockedStates = malloc(fmu->fmi3.numberOfClockedStates*sizeof(fmi3ModelStructureElement));
        fmu->fmi3.initialUnknowns = malloc(fmu->fmi3.numberOfInitialUnknowns*sizeof(fmi3ModelStructureElement));
        fmu->fmi3.eventIndicators = malloc(fmu->fmi3.numberOfEventIndicators*sizeof(fmi3ModelStructureElement));

        //Read outputs
        int i=0;
        outputElement = ezxml_child(modelStructureElement, "Output");
        for(;outputElement;outputElement = outputElement->next) {
            if(!parseModelStructureElement(&fmu->fmi3.outputs[i], &outputElement)) {
                return false;
            }
            ++i;
        }

        //Read continuous state derivatives
        i=0;
        continuousStateDerElement = ezxml_child(modelStructureElement, "ContinuousStateDerivative");
        for(;continuousStateDerElement;continuousStateDerElement = continuousStateDerElement->next) {
            if(!parseModelStructureElement(&fmu->fmi3.continuousStateDerivatives[i], &continuousStateDerElement)) {
                return false;
            }
            ++i;
        }

        //Read clocked states
        i=0;
        clockedStateElement = ezxml_child(modelStructureElement, "ClockedState");
        for(;clockedStateElement;clockedStateElement = clockedStateElement->next) {
            if(!parseModelStructureElement(&fmu->fmi3.clockedStates[i], &clockedStateElement)) {
                return false;
            }
            ++i;
        }

        //Read initial unknowns
        i=0;
        initialUnknownElement = ezxml_child(modelStructureElement, "IninitalUnknown");
        for(;initialUnknownElement;initialUnknownElement = initialUnknownElement->next) {
            if(!parseModelStructureElement(&fmu->fmi3.initialUnknowns[i], &initialUnknownElement)) {
                return false;
            }
            ++i;
        }

        //Read event indicators
        i=0;
        eventIndicatorElement = ezxml_child(modelStructureElement, "EventIndicator");
        for(;eventIndicatorElement;eventIndicatorElement = eventIndicatorElement->next) {
            if(!parseModelStructureElement(&fmu->fmi3.eventIndicators[i], &eventIndicatorElement)) {
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

    char cwd[FILENAME_MAX];
#ifdef _WIN32
    _getcwd(cwd, sizeof(char)*FILENAME_MAX);
#else
    getcwd(cwd, sizeof(char)*FILENAME_MAX);
#endif

    char dllPath[FILENAME_MAX];
    dllPath[0] = '\0';
    strcat(dllPath, fmu->unzippedLocation);
#ifdef _WIN32
    strcat(dllPath,"\\binaries\\win64\\");
#else
    strcat(dllPath,"/binaries/linux64/");
#endif
    strcat(dllPath,fmu->fmi1.modelIdentifier);
#ifdef _WIN32
    strcat(dllPath,".dll");
#else
    strcat(dllPath,".so");
#endif
#ifdef _WIN32
    HINSTANCE dll = LoadLibraryA(dllPath);
    if(NULL == dll) {
        printf("Loading DLL failed: %s\n",dllPath);
        return false;
    }
#else
    void* dll = dlopen(dllPath, RTLD_NOW|RTLD_LOCAL);
    if(NULL == dll) {
        printf("Loading shared object failed: %s (%s)\n",dllPath, dlerror());
        return false;
    }
#endif

    fmu->dll = dll;

    bool ok = true;

    //Load all common functions
    fmu->fmi1.getVersion = (fmiGetVersion_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiGetVersion"), &ok);
    fmu->fmi1.setDebugLogging = (fmiSetDebugLogging_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiSetDebugLogging"), &ok);
    fmu->fmi1.getReal = (fmiGetReal_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiGetReal"), &ok);
    fmu->fmi1.setReal = (fmiSetReal_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiSetReal"), &ok);
    fmu->fmi1.getInteger = (fmiGetInteger_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiGetInteger"), &ok);
    fmu->fmi1.setInteger = (fmiSetInteger_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiSetInteger"), &ok);
    fmu->fmi1.getBoolean = (fmiGetBoolean_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiGetBoolean"), &ok);
    fmu->fmi1.setBoolean = (fmiSetBoolean_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiSetBoolean"), &ok);
    fmu->fmi1.getString = (fmiGetString_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiGetString"), &ok);
    fmu->fmi1.setString = (fmiSetString_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiSetString"), &ok);

    if(fmu->fmi1.type == fmi1ModelExchange) {
        //Load model exchange functions
        fmu->fmi1.instantiateModel = (fmiInstantiateModel_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelName, "fmiInstantiateModel"), &ok);
        fmu->fmi1.freeModelInstance = (fmiFreeModelInstance_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelName, "fmiFreeModelInstance"), &ok);
        fmu->fmi1.initialize = (fmiInitialize_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelName, "fmiInitialize"), &ok);
        fmu->fmi1.getDerivatives = (fmiGetDerivatives_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelName, "fmiGetDerivatives"), &ok);
        fmu->fmi1.terminate = (fmiTerminate_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelName, "fmiTerminate"), &ok);
        fmu->fmi1.setTime = (fmiSetTime_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelName, "fmiSetTime"), &ok);
        fmu->fmi1.getModelTypesPlatform = (fmiGetModelTypesPlatform_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelName, "fmiGetModelTypesPlatform"), &ok);
        fmu->fmi1.setContinuousStates = (fmiSetContinuousStates_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelName, "fmiSetContinuousStates"), &ok);
        fmu->fmi1.completedIntegratorStep = (fmiCompletedIntegratorStep_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelName, "fmiCompletedIntegratorStep"), &ok);
        fmu->fmi1.getEventIndicators = (fmiGetEventIndicators_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelName, "fmiGetEventIndicators"), &ok);
        fmu->fmi1.eventUpdate = (fmiEventUpdate_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelName, "fmiEventUpdate"), &ok);
        fmu->fmi1.getContinuousStates = (fmiGetContinuousStates_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelName, "fmiGetContinuousStates"), &ok);
        fmu->fmi1.getNominalContinuousStates = (fmiGetNominalContinuousStates_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelName, "fmiGetNominalContinuousStates"), &ok);
        fmu->fmi1.getStateValueReferences = (fmiGetStateValueReferences_t)loadDllFunction(dll,  getFunctionName(fmu->fmi1.modelName, "fmiGetStateValueReferences"), &ok);
    }

    if(fmu->fmi1.type == fmi1CoSimulationStandAlone || fmu->fmi1.type == fmi1CoSimulationTool) {
        //Load all co-simulation functions
        fmu->fmi1.getTypesPlatform = (fmiGetTypesPlatform_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiGetTypesPlatform"), &ok);
        fmu->fmi1.instantiateSlave = (fmiInstantiateSlave_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiInstantiateSlave"), &ok);
        fmu->fmi1.initializeSlave = (fmiInitializeSlave_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiInitializeSlave"), &ok);
        fmu->fmi1.terminateSlave = (fmiTerminateSlave_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiTerminateSlave"), &ok);
        fmu->fmi1.resetSlave =  (fmiResetSlave_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiResetSlave"), &ok);
        fmu->fmi1.freeSlaveInstance = (fmiFreeSlaveInstance_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiFreeSlaveInstance"), &ok);
        fmu->fmi1.setRealInputDerivatives = (fmiSetRealInputDerivatives_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiSetRealInputDerivatives"), &ok);
        fmu->fmi1.getRealOutputDerivatives = (fmiGetRealOutputDerivatives_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiGetRealOutputDerivatives"), &ok);
        fmu->fmi1.doStep = (fmiDoStep_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiDoStep"), &ok);
        fmu->fmi1.cancelStep = (fmiCancelStep_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiCancelStep"), &ok);
        fmu->fmi1.getStatus = (fmiGetStatus_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiGetStatus"), &ok);
        fmu->fmi1.getRealStatus = (fmiGetRealStatus_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiGetRealStatus"), &ok);
        fmu->fmi1.getIntegerStatus = (fmiGetIntegerStatus_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiGetIntegerStatus"), &ok);
        fmu->fmi1.getBooleanStatus = (fmiGetBooleanStatus_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiGetBooleanStatus"), &ok);
        fmu->fmi1.getStringStatus = (fmiGetStringStatus_t)loadDllFunction(dll, getFunctionName(fmu->fmi1.modelName, "fmiGetStringStatus"), &ok);
    }

    chdir(cwd);

    return ok;
}


//! @brief Loads all DLL functions for FMI 2
//! @param fmu FMU handle
//! @returns True if load was successful
bool loadFunctionsFmi2(fmiHandle *fmu)
{
    TRACEFUNC

    char cwd[FILENAME_MAX];
#ifdef _WIN32
    _getcwd(cwd, sizeof(char)*FILENAME_MAX);
#else
    getcwd(cwd, sizeof(char)*FILENAME_MAX);
#endif

    char dllPath[FILENAME_MAX];
    dllPath[0] = '\0';
    strcat(dllPath, fmu->unzippedLocation);
#ifdef _WIN32
    strcat(dllPath,"\\binaries\\win64\\");
#else
    strcat(dllPath,"/binaries/linux64/");
#endif
    strcat(dllPath,fmu->fmi2.modelIdentifier);
#ifdef _WIN32
    strcat(dllPath,".dll");
#else
    strcat(dllPath,".so");
#endif
#ifdef _WIN32
    HINSTANCE dll = LoadLibraryA(dllPath);
    if(NULL == dll) {
        printf("Loading DLL failed: %s\n", dllPath);
        return false;
    }
#else
    void* dll = dlopen(dllPath, RTLD_NOW|RTLD_LOCAL);
    if(NULL == dll) {
        printf("Loading shared object failed: %s (%s)\n", dllPath, dlerror());
        return false;
    }
#endif

    fmu->dll = dll;

    bool ok = true;

    //Load all common functions
    fmu->fmi2.getVersion = (fmi2GetVersion_t)loadDllFunction(dll, "fmi2GetVersion", &ok);
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

    if(fmu->fmi2.supportsCoSimulation) {
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

    if(fmu->fmi2.supportsModelExchange) {
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
bool loadFunctionsFmi3(fmiHandle *fmu)
{
    TRACEFUNC

    char cwd[FILENAME_MAX];
#ifdef _WIN32
    _getcwd(cwd, sizeof(char)*FILENAME_MAX);
#else
    getcwd(cwd, sizeof(char)*FILENAME_MAX);
#endif

    char dllPath[FILENAME_MAX];
    dllPath[0] = '\0';
    strcat(dllPath, fmu->unzippedLocation);
#ifdef _WIN32
    strcat(dllPath,"\\binaries\\x86_64-windows\\");
#else
    strcat(dllPath,"/binaries/x86_64-linux/");
#endif
    strcat(dllPath,fmu->fmi3.modelIdentifier);
#ifdef _WIN32
    strcat(dllPath,".dll");
#else
    strcat(dllPath,".so");
#endif
#ifdef _WIN32
    HINSTANCE dll = LoadLibraryA(dllPath);
    if(NULL == dll) {
        printf("Loading DLL failed: %s\n", dllPath);
        return false;
    }
#else
    void* dll = dlopen(dllPath, RTLD_NOW|RTLD_LOCAL);
    if(NULL == dll) {
        printf("Loading shared object failed: %s (%s)\n", dllPath, dlerror());
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
fmiVersion_t getFmiVersion(fmiHandle *fmu)
{
    return fmu->version;
}




bool fmi3InstantiateCoSimulation(fmiHandle *fmu,
                             fmi3Boolean                    visible,
                             fmi3Boolean                    loggingOn,
                             fmi3Boolean                    eventModeUsed,
                             fmi3Boolean                    earlyReturnAllowed,
                             const fmi3ValueReference       requiredIntermediateVariables[],
                             size_t                         nRequiredIntermediateVariables,
                             fmi3InstanceEnvironment        instanceEnvironment,
                             fmi3CallbackLogMessage         logMessage,
                             fmi3CallbackIntermediateUpdate intermediateUpdate)
{

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

bool fmi3InstantiateModelExchange(fmiHandle *fmu,
                                  fmi3Boolean               visible,
                                  fmi3Boolean                loggingOn,
                                  fmi3InstanceEnvironment    instanceEnvironment,
                                  fmi3CallbackLogMessage     logMessage)
{
    fmu->fmi3.fmi3Instance = fmu->fmi3.instantiateModelExchange(fmu->instanceName,
                                                           fmu->fmi3.instantiationToken,
                                                           fmu->resourcesLocation,
                                                           visible,
                                                           loggingOn,
                                                           instanceEnvironment,
                                                           logMessage);

    return (fmu->fmi3.fmi3Instance != NULL);
}

const char* fmi3GetVersion(fmiHandle *fmu) {

    return fmu->fmi3.getVersion(fmu->fmi3.fmi3Instance);
}

fmi3Status fmi3SetDebugLogging(fmiHandle *fmu,
                                fmi3Boolean loggingOn,
                                size_t nCategories,
                                const fmi3String categories[])
{

    return fmu->fmi3.setDebugLogging(fmu->fmi3.fmi3Instance, loggingOn, nCategories, categories);
}

fmi3Status fmi3GetFloat64(fmiHandle *fmu,
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


fmi3Status fmi3SetFloat64(fmiHandle *fmu,
                           const fmi3ValueReference valueReferences[],
                           size_t nValueReferences,
                           fmi3Float64 values[],
                           size_t nValues) {

    return fmu->fmi3.setFloat64(fmu->fmi3.fmi3Instance,
                                valueReferences,
                                nValueReferences,
                                values,
                                nValues);
}

fmi3Status fmi3EnterInitializationMode(fmiHandle *fmu,
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

fmi3Status fmi3ExitInitializationMode(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.exitInitializationMode(fmu->fmi3.fmi3Instance);
}

fmi3Status fmi3Terminate(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.terminate(fmu->fmi3.fmi3Instance);
}

void fmi3FreeInstance(fmiHandle *fmu)
{
    TRACEFUNC

    fmu->fmi3.freeInstance(fmu->fmi3.fmi3Instance);
}

fmi3Status fmi3DoStep(fmiHandle *fmu,
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

const char *fmi3ModelName(fmiHandle *fmu)
{
    printf("fmu pointer: %p\n", fmu);
    return fmu->fmi3.modelName;
}

const char* fmi3InstantiationToken(fmiHandle *fmu)
{

    return fmu->fmi3.instantiationToken;
}

const char* fmi3Description(fmiHandle *fmu)
{

    return fmu->fmi3.description;
}

const char* fmi3Author(fmiHandle *fmu)
{

    return fmu->fmi3.author;
}

const char* fmi3Version(fmiHandle *fmu)
{
    return fmu->fmi3.version;
}

const char* fmi3Copyright(fmiHandle *fmu)
{

    return fmu->fmi3.copyright;
}

const char* fmi3License(fmiHandle *fmu)
{

    return fmu->fmi3.license;
}

const char* fmi3GenerationTool(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.generationTool;
}

const char* fmi3GenerationDateAndTime(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.generationDateAndTime;
}

const char* fmi3VariableNamingConvention(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.variableNamingConvention;
}

bool fmi3SupportsCoSimulation(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.supportsCoSimulation;
}

bool fmi3SupportsModelExchange(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.supportsModelExchange;
}

bool fmi3SupportsScheduledExecution(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.supportsScheduledExecution;
}

const char *fmi2GetTypesPlatform(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.getTypesPlatform();
}

const char *fmi2GetVersion(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.getVersion();
}

fmi2Status fmi2SetDebugLogging(fmiHandle *fmu, fmi2Boolean loggingOn, size_t nCategories, const fmi2String categories[])
{
    TRACEFUNC

    return fmu->fmi2.setDebugLogging(fmu->fmi2.component, loggingOn, nCategories, categories);
}

bool fmi2Instantiate(fmiHandle *fmu, fmi2Type type, fmi2CallbackLogger logger, fmi2CallbackAllocateMemory allocateMemory, fmi2CallbackFreeMemory freeMemory, fmi2StepFinished stepFinished, fmi2ComponentEnvironment componentEnvironment, fmi2Boolean visible, fmi2Boolean loggingOn)
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

    fmu->fmi2.callbacks.logger = logger;
    fmu->fmi2.callbacks.allocateMemory = allocateMemory;
    fmu->fmi2.callbacks.freeMemory = freeMemory;
    fmu->fmi2.callbacks.stepFinished = stepFinished;
    fmu->fmi2.callbacks.componentEnvironment = componentEnvironment;

    printf("  instanceName:       %s\n", fmu->instanceName);
    printf("  GUID:               %s\n", fmu->fmi2.guid);
    printf("  unzipped location:  %s\n", fmu->unzippedLocation);
    printf("  resources location: %s\n", fmu->resourcesLocation);

    fmu->fmi2.component = fmu->fmi2.instantiate(fmu->instanceName, type, fmu->fmi2.guid, fmu->resourcesLocation, &fmu->fmi2.callbacks, visible, loggingOn);
    return (fmu->fmi2.component != NULL);
}

void fmi2FreeInstance(fmiHandle *fmu)
{
    TRACEFUNC

    fmu->fmi2.freeInstance(fmu->fmi2.component);
}

fmi2Status fmi2SetupExperiment(fmiHandle *fmu, fmi2Boolean toleranceDefined, fmi2Real tolerance, fmi2Real startTime, fmi2Boolean stopTimeDefined, fmi2Real stopTime)
{
    TRACEFUNC

    return fmu->fmi2.setupExperiment(fmu->fmi2.component, toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime);
}

fmi2Status fmi2EnterInitializationMode(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.enterInitializationMode(fmu->fmi2.component);
}

fmi2Status fmi2ExitInitializationMode(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.exitInitializationMode(fmu->fmi2.component);
}

fmi2Status fmi2Terminate(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.terminate(fmu->fmi2.component);
}

fmi2Status fmi2Reset(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.reset(fmu->fmi2.component);
}

int fmi3GetNumberOfVariables(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.numberOfVariables;
}

const char *fmi3GetVariableName(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->name;
}

fmi3Initial fmi3GetVariableInitial(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->initial;
}

fmi3Variability fmi3GetVariableVariability(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->variability;
}

fmi3Causality fmi3GetVariableCausality(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->causality;
}


bool fmi3GetVariableSupportsIntermediateUpdate(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->intermediateUpdate;
}

fmi3DataType fmi3GetVariableDataType(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->datatype;
}

const char *fmi3GetVariableDescription(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->description;
}

const char *fmi3GetVariableQuantity(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->quantity;
}

const char *fmi3GetVariableUnit(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->unit;
}

const char *fmi3GetVariableDisplayUnit(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->displayUnit;
}

fmi3Float64 fmi3GetVariableStartFloat64(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startFloat64;
}

fmi3Float32 fmi3GetVariableStartFloat32(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startFloat32;
}

fmi3Int64 fmi3GetVariableStartInt64(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startInt64;
}

fmi3Int32 fmi3GetVariableStartInt32(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startInt32;
}

fmi3Int16 fmi3GetVariableStartInt16(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startInt16;
}

fmi3Int8 fmi3GetVariableStartInt8(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startInt8;
}

fmi3UInt64 fmi3GetVariableStartUInt64(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startUInt64;
}

fmi3UInt32 fmi3GetVariableStartUInt32(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startUInt32;
}

fmi3UInt16 fmi3GetVariableStartUInt16(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startUInt16;
}

fmi3UInt8 fmi3GetVariableStartUInt8(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startUInt8;
}

fmi3Boolean fmi3GetVariableStartBoolean(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startBoolean;
}

fmi3String fmi3GetVariableStartString(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startString;
}

fmi3Binary fmi3GetVariableStartBinary(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startBinary;
}

fmi3Boolean fmi3GetVariableStartEnumeration(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startEnumeration;
}

fmi3ValueReference fmi3GetVariableValueReference(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->valueReference;
}

fmi3Status fmi3EnterEventMode(fmiHandle *fmu, fmi3Boolean stepEvent, fmi3Boolean stateEvent, const fmi3Int32 rootsFound[], size_t nEventIndicators, fmi3Boolean timeEvent)
{
    return fmu->fmi3.enterEventMode(fmu->fmi3.fmi3Instance, stepEvent, stateEvent, rootsFound, nEventIndicators, timeEvent);
}

fmi3Status fmi3Reset(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi3.reset(fmu->fmi3.fmi3Instance);
}

fmi3Status fmi3GetFloat32(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Float32 values[], size_t nValues)
{

    return fmu->fmi3.getFloat32(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetInt8(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int8 values[], size_t nValues)
{

    return fmu->fmi3.getInt8(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetUInt8(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt8 values[], size_t nValues)
{

    return fmu->fmi3.getUInt8(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetInt16(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int16 values[], size_t nValues)
{

    return fmu->fmi3.getInt16(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetUInt16(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt16 values[], size_t nValues)
{

    return fmu->fmi3.getUInt16(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetInt32(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int32 values[], size_t nValues)
{

    return fmu->fmi3.getInt32(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetUInt32(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt32 values[], size_t nValues)
{

    return fmu->fmi3.getUInt32(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetInt64(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int64 values[], size_t nValues)
{

    return fmu->fmi3.getInt64(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetUInt64(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt64 values[], size_t nValues)
{

    return fmu->fmi3.getUInt64(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetBoolean(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Boolean values[], size_t nValues)
{

    return fmu->fmi3.getBoolean(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetString(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3String values[], size_t nValues)
{

    return fmu->fmi3.getString(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetBinary(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, size_t valueSizes[], fmi3Binary values[], size_t nValues)
{

    return fmu->fmi3.getBinary(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, valueSizes, values, nValues);
}

fmi3Status fmi3GetClock(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Clock values[])
{

    return fmu->fmi3.getClock(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values);
}

fmi3Status fmi3SetFloat32(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Float32 values[], size_t nValues)
{

    return fmu->fmi3.setFloat32(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetInt8(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int8 values[], size_t nValues)
{

    return fmu->fmi3.setInt8(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetUInt8(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt8 values[], size_t nValues)
{

    return fmu->fmi3.setUInt8(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetInt16(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int16 values[], size_t nValues)
{

    return fmu->fmi3.setInt16(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetUInt16(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt16 values[], size_t nValues)
{

    return fmu->fmi3.setUInt16(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetInt32(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int32 values[], size_t nValues)
{

    return fmu->fmi3.setInt32(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetUInt32(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt32 values[], size_t nValues)
{

    return fmu->fmi3.setUInt32(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetInt64(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int64 values[], size_t nValues)
{

    return fmu->fmi3.setInt64(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetUInt64(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt64 values[], size_t nValues)
{

    return fmu->fmi3.setUInt64(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetBoolean(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Boolean values[], size_t nValues)
{

    return fmu->fmi3.setBoolean(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetString(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3String values[], size_t nValues)
{

    return fmu->fmi3.setString(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetBinary(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const size_t valueSizes[], const fmi3Binary values[], size_t nValues)
{

    return fmu->fmi3.setBinary(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, valueSizes, values, nValues);
}

fmi3Status fmi3SetClock(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Clock values[], size_t nValues)
{

    return fmu->fmi3.setClock(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetNumberOfVariableDependencies(fmiHandle *fmu, fmi3ValueReference valueReference, size_t *nDependencies)
{

    return fmu->fmi3.getNumberOfVariableDependencies(fmu->fmi3.fmi3Instance, valueReference, nDependencies);
}

fmi3Status fmi3GetVariableDependencies(fmiHandle *fmu, fmi3ValueReference dependent, size_t elementIndicesOfDependent[], fmi3ValueReference independents[], size_t elementIndicesOfIndependents[], fmi3DependencyKind dependencyKinds[], size_t nDependencies)
{

    return fmu->fmi3.getVariableDependencies(fmu->fmi3.fmi3Instance, dependent, elementIndicesOfDependent, independents, elementIndicesOfIndependents, dependencyKinds, nDependencies);
}

fmi3Status fmi3GetFMUState(fmiHandle *fmu, fmi3FMUState *FMUState)
{

    return fmu->fmi3.getFMUState(fmu->fmi3.fmi3Instance, FMUState);
}

fmi3Status fmi3SetFMUState(fmiHandle *fmu, fmi3FMUState FMUState)
{

    return fmu->fmi3.setFMUState(fmu->fmi3.fmi3Instance, FMUState);
}

fmi3Status fmi3FreeFMUState(fmiHandle *fmu, fmi3FMUState *FMUState)
{

    return fmu->fmi3.freeFMUState(fmu->fmi3.fmi3Instance, FMUState);
}

fmi3Status fmi3SerializedFMUStateSize(fmiHandle *fmu, fmi3FMUState FMUState, size_t *size)
{

    return fmu->fmi3.serializedFMUStateSize(fmu->fmi3.fmi3Instance, FMUState, size);
}

fmi3Status fmi3SerializeFMUState(fmiHandle *fmu, fmi3FMUState FMUState, fmi3Byte serializedState[], size_t size)
{

    return fmu->fmi3.serializeFMUState(fmu->fmi3.fmi3Instance, FMUState, serializedState, size);
}

fmi3Status fmi3DeserializeFMUState(fmiHandle *fmu, const fmi3Byte serializedState[], size_t size, fmi3FMUState *FMUState)
{

    return fmu->fmi3.deserializeFMUState(fmu->fmi3.fmi3Instance, serializedState, size, FMUState);
}

fmi3Status fmi3GetDirectionalDerivative(fmiHandle *fmu, const fmi3ValueReference unknowns[], size_t nUnknowns, const fmi3ValueReference knowns[], size_t nKnowns, const fmi3Float64 seed[], size_t nSeed, fmi3Float64 sensitivity[], size_t nSensitivity)
{

    return fmu->fmi3.getDirectionalDerivative(fmu->fmi3.fmi3Instance, unknowns, nUnknowns, knowns, nKnowns, seed, nSeed, sensitivity, nSensitivity);
}

fmi3Status fmi3GetAdjointDerivative(fmiHandle *fmu, const fmi3ValueReference unknowns[], size_t nUnknowns, const fmi3ValueReference knowns[], size_t nKnowns, const fmi3Float64 seed[], size_t nSeed, fmi3Float64 sensitivity[], size_t nSensitivity)
{

    return fmu->fmi3.getAdjointDerivative(fmu->fmi3.fmi3Instance, unknowns, nUnknowns, knowns, nKnowns, seed, nSeed, sensitivity, nSensitivity);
}

fmi3Status fmi3EnterConfigurationMode(fmiHandle *fmu)
{

    return fmu->fmi3.enterConfigurationMode(fmu->fmi3.fmi3Instance);
}

fmi3Status fmi3ExitConfigurationMode(fmiHandle *fmu)
{

    return fmu->fmi3.exitConfigurationMode(fmu->fmi3.fmi3Instance);
}

fmi3Status fmi3GetIntervalDecimal(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Float64 intervals[], fmi3IntervalQualifier qualifiers[])
{

    return fmu->fmi3.getIntervalDecimal(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, intervals, qualifiers);
}

fmi3Status fmi3GetIntervalFraction(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt64 intervalCounters[], fmi3UInt64 resolutions[], fmi3IntervalQualifier qualifiers[])
{

    return fmu->fmi3.getIntervalFraction(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, intervalCounters, resolutions, qualifiers);
}

fmi3Status fmi3GetShiftDecimal(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Float64 shifts[])
{

    return fmu->fmi3.getShiftDecimal(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, shifts);
}

fmi3Status fmi3GetShiftFraction(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt64 shiftCounters[], fmi3UInt64 resolutions[])
{

    return fmu->fmi3.getShiftFraction(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, shiftCounters, resolutions);
}

fmi3Status fmi3SetIntervalDecimal(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Float64 intervals[])
{

    return fmu->fmi3.setIntervalDecimal(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, intervals);
}

fmi3Status fmi3SetIntervalFraction(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt64 intervalCounters[], const fmi3UInt64 resolutions[])
{

    return fmu->fmi3.setIntervalFraction(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, intervalCounters, resolutions);
}

fmi3Status fmi3EvaluateDiscreteStates(fmiHandle *fmu)
{

    return fmu->fmi3.evaluateDiscreteStates(fmu->fmi3.fmi3Instance);
}

fmi3Status fmi3UpdateDiscreteStates(fmiHandle *fmu, fmi3Boolean *discreteStatesNeedUpdate, fmi3Boolean *terminateSimulation, fmi3Boolean *nominalsOfContinuousStatesChanged, fmi3Boolean *valuesOfContinuousStatesChanged, fmi3Boolean *nextEventTimeDefined, fmi3Float64 *nextEventTime)
{

    return fmu->fmi3.updateDiscreteStates(fmu->fmi3.fmi3Instance, discreteStatesNeedUpdate, terminateSimulation, nominalsOfContinuousStatesChanged, valuesOfContinuousStatesChanged, nextEventTimeDefined, nextEventTime);
}

fmi3Status fmi3EnterContinuousTimeMode(fmiHandle *fmu)
{

    return fmu->fmi3.enterContinuousTimeMode(fmu->fmi3.fmi3Instance);
}

fmi3Status fmi3CompletedIntegratorStep(fmiHandle *fmu, fmi3Boolean noSetFMUStatePriorToCurrentPoint, fmi3Boolean *enterEventMode, fmi3Boolean *terminateSimulation)
{

    return fmu->fmi3.completedIntegratorStep(fmu->fmi3.fmi3Instance, noSetFMUStatePriorToCurrentPoint, enterEventMode, terminateSimulation);
}

fmi3Status fmi3SetTime(fmiHandle *fmu, fmi3Float64 time)
{

    return fmu->fmi3.setTime(fmu->fmi3.fmi3Instance, time);
}

fmi3Status fmi3SetContinuousStates(fmiHandle *fmu, const fmi3Float64 continuousStates[], size_t nContinuousStates)
{

    return fmu->fmi3.setContinuousStates(fmu->fmi3.fmi3Instance, continuousStates, nContinuousStates);
}

fmi3Status fmi3GetContinuousStateDerivatives(fmiHandle *fmu, fmi3Float64 derivatives[], size_t nContinuousStates)
{

    return fmu->fmi3.getContinuousStateDerivatives(fmu->fmi3.fmi3Instance, derivatives, nContinuousStates);
}

fmi3Status fmi3GetEventIndicators(fmiHandle *fmu, fmi3Float64 eventIndicators[], size_t nEventIndicators)
{

    return fmu->fmi3.getEventIndicators(fmu->fmi3.fmi3Instance, eventIndicators, nEventIndicators);
}

fmi3Status fmi3GetContinuousStates(fmiHandle *fmu, fmi3Float64 continuousStates[], size_t nContinuousStates)
{

    return fmu->fmi3.getContinuousStates(fmu->fmi3.fmi3Instance, continuousStates, nContinuousStates);
}

fmi3Status fmi3GetNominalsOfContinuousStates(fmiHandle *fmu, fmi3Float64 nominals[], size_t nContinuousStates)
{

    return fmu->fmi3.getNominalsOfContinuousStates(fmu->fmi3.fmi3Instance, nominals, nContinuousStates);
}

fmi3Status fmi3GetNumberOfEventIndicators(fmiHandle *fmu, size_t *nEventIndicators)
{

    return fmu->fmi3.getNumberOfEventIndicators(fmu->fmi3.fmi3Instance, nEventIndicators);
}

fmi3Status fmi3GetNumberOfContinuousStates(fmiHandle *fmu, size_t *nContinuousStates)
{

    return fmu->fmi3.getNumberOfContinuousStates(fmu->fmi3.fmi3Instance, nContinuousStates);
}

fmi3Status fmi3EnterStepMode(fmiHandle *fmu)
{

    return fmu->fmi3.enterStepMode(fmu->fmi3.fmi3Instance);
}

fmi3Status fmi3GetOutputDerivatives(fmiHandle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int32 orders[], fmi3Float64 values[], size_t nValues)
{

    return fmu->fmi3.getOutputDerivatives(fmu->fmi3.fmi3Instance, valueReferences, nValueReferences, orders, values, nValues);
}

fmi3Status fmi3ActivateModelPartition(fmiHandle *fmu, fmi3ValueReference clockReference, fmi3Float64 activationTime)
{

    return fmu->fmi3.activateModelPartition(fmu->fmi3.fmi3Instance, clockReference, activationTime);
}

bool fmi3DefaultStartTimeDefined(fmiHandle *fmu)
{

    return fmu->fmi3.defaultStartTimeDefined;
}

bool fmi3DefaultStopTimeDefined(fmiHandle *fmu)
{

    return fmu->fmi3.defaultStopTimeDefined;
}

bool fmi3DefaultToleranceDefined(fmiHandle *fmu)
{

    return fmu->fmi3.defaultToleranceDefined;
}

bool fmi3DefaultStepSizeDefined(fmiHandle *fmu)
{

    return fmu->fmi3.defaultStepSizeDefined;
}

double fmi3GetDefaultStartTime(fmiHandle *fmu)
{

    return fmu->fmi3.defaultStartTime;
}

double fmi3GetDefaultStopTime(fmiHandle *fmu)
{

    return fmu->fmi3.defaultStopTime;
}

double fmi3GetDefaultTolerance(fmiHandle *fmu)
{

    return fmu->fmi3.defaultTolerance;
}

double fmi3GetDefaultStepSize(fmiHandle *fmu)
{

    return fmu->fmi3.defaultStepSize;
}

bool fmi2DefaultStartTimeDefined(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.defaultStartTimeDefined;
}

bool fmi2DefaultStopTimeDefined(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.defaultStopTimeDefined;
}

bool fmi2DefaultToleranceDefined(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.defaultToleranceDefined;
}

bool fmi2DefaultStepSizeDefined(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.defaultStepSizeDefined;
}

double fmi2GetDefaultStartTime(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.defaultStartTime;
}

double fmi2GetDefaultStopTime(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.defaultStopTime;
}

double fmi2GetDefaultTolerance(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.defaultTolerance;
}

double fmi2GetDefaultStepSize(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.defaultStepSize;
}

fmi3VariableHandle *fmi3GetVariableByName(fmiHandle *fmu, fmi3String name)
{
    for(int i=0; i<fmu->fmi3.numberOfVariables; ++i) {
        if(!strcmp(fmu->fmi3.variables[i].name, name)) {
            return &fmu->fmi3.variables[i];
        }
    }
    printf("Variable with name %s not found.\n", name);
    return NULL;
}

fmi3VariableHandle *fmi3GetVariableByIndex(fmiHandle *fmu, int i)
{

    if(i >= fmu->fmi3.numberOfVariables) {
        printf("Variable index out of bounds: %i\n",i);
        return NULL;
    }
    return &fmu->fmi3.variables[i];
}

fmi3VariableHandle *fmi3GetVariableByValueReference(fmiHandle *fmu, fmi3ValueReference vr)
{

    for(int i=0; i<fmu->fmi3.numberOfVariables; ++i) {
        if(fmu->fmi3.variables[i].valueReference == vr) {
            return &fmu->fmi3.variables[i];
        }
    }
    printf("Variable with value reference %i not found.\n", vr);
    return NULL;
}

int fmi2GetNumberOfVariables(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.numberOfVariables;
}

fmi2VariableHandle *fmi2GetVariableByIndex(fmiHandle *fmu, int i)
{
    TRACEFUNC

    if(i >= fmu->fmi2.numberOfVariables) {
        printf("Variable index out of bounds: %i\n",i);
        return NULL;
    }
    return &fmu->fmi2.variables[i];
}

fmi2VariableHandle *fmi2GetVariableByValueReference(fmiHandle *fmu, fmi2ValueReference vr)
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

fmi2VariableHandle *fmi2GetVariableByName(fmiHandle *fmu, fmi2String name)
{
    for(int i=0; i<fmu->fmi2.numberOfVariables; ++i) {
        if(!strcmp(fmu->fmi2.variables[i].name, name)) {
            return &fmu->fmi2.variables[i];
        }
    }
    printf("Variable with name %s not found.\n", name);
    return NULL;
}

const char *fmi2GetVariableName(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->name;
}

const char *fmi2GetVariableDescription(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->description;
}

const char *fmi2GetVariableQuantity(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->quantity;
}

const char *fmi2GetVariableUnit(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->unit;
}

const char *fmi2GetVariableDisplayUnit(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->displayUnit;
}

fmi2Real fmi2GetVariableStartReal(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->startReal;
}


fmi2Integer fmi2GetVariableStartInteger(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->startInteger;
}

fmi2Boolean fmi2GetVariableStartBoolean(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->startBoolean;
}

fmi2String fmi2GetVariableStartString(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->startString;
}

long fmi2GetVariableValueReference(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->valueReference;
}

fmi2Causality fmi2GetVariableCausality(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->causality;
}

fmi2Variability fmi2GetVariableVariability(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->variability;
}

fmi2Initial fmi2GetVariableInitial(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->initial;
}

bool fmi2GetVariableCanHandleMultipleSetPerTimeInstant(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->canHandleMultipleSetPerTimeInstant;
}

fmi2DataType fmi2GetVariableDataType(fmi2VariableHandle *var)
{
    TRACEFUNC
    return var->datatype;
}

fmi2Status fmi2GetReal(fmiHandle *fmu,
                       const fmi2ValueReference valueReferences[],
                       size_t nValueReferences,
                       fmi2Real values[])
{
    return fmu->fmi2.getReal(fmu->fmi2.component,
                            valueReferences,
                            nValueReferences,
                            values);
}

fmi2Status fmi2GetInteger(fmiHandle *fmu,
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

fmi2Status fmi2GetBoolean(fmiHandle *fmu,
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

fmi2Status fmi2GetString(fmiHandle *fmu,
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

fmi2Status fmi2SetReal(fmiHandle *fmu,
                       const fmi2ValueReference valueReferences[],
                       size_t nValueReferences,
                       const fmi2Real values[])
{
    return fmu->fmi2.setReal(fmu->fmi2.component,
                               valueReferences,
                               nValueReferences,
                               values);
}

fmi2Status fmi2SetInteger(fmiHandle *fmu,
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

fmi2Status fmi2SetBoolean(fmiHandle *fmu,
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

fmi2Status fmi2SetString(fmiHandle *fmu,
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

fmi2Status fmi2GetFMUstate(fmiHandle* fmu, fmi2FMUstate* FMUstate)
{
    TRACEFUNC
    return fmu->fmi2.getFMUstate(fmu->fmi2.component, FMUstate);
}

fmi2Status fmi2SetFMUstate(fmiHandle* fmu, fmi2FMUstate FMUstate)
{
    TRACEFUNC
    return fmu->fmi2.setFMUstate(fmu->fmi2.component, FMUstate);
}

fmi2Status fmi2FreeFMUstate(fmiHandle* fmu, fmi2FMUstate* FMUstate)
{
    TRACEFUNC
    return fmu->fmi2.freeFMUstate(fmu->fmi2.component, FMUstate);
}

fmi2Status fmi2SerializedFMUstateSize(fmiHandle* fmu, fmi2FMUstate FMUstate, size_t* size)
{
    TRACEFUNC
    return fmu->fmi2.serializedFMUstateSize(fmu->fmi2.component, FMUstate, size);
}

fmi2Status fmi2SerializeFMUstate(fmiHandle* fmu, fmi2FMUstate FMUstate, fmi2Byte serializedState[], size_t size)
{
    TRACEFUNC
    return fmu->fmi2.serializeFMUstate(fmu->fmi2.component, FMUstate, serializedState, size);
}

fmi2Status fmi2DeSerializeFMUstate(fmiHandle* fmu, const fmi2Byte serializedState[], size_t size, fmi2FMUstate* FMUstate)
{
    TRACEFUNC
    return fmu->fmi2.deSerializeFMUstate(fmu->fmi2.component, serializedState, size, FMUstate);
}

fmi2Status fmi2GetDirectionalDerivative(fmiHandle* fmu,
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

fmi2Status fmi2EnterEventMode(fmiHandle* fmu)
{
    TRACEFUNC
    return fmu->fmi2.enterEventMode(fmu->fmi2.component);
}

fmi2Status fmi2NewDiscreteStates(fmiHandle* fmu, fmi2EventInfo* eventInfo)
{
    TRACEFUNC
            return fmu->fmi2.newDiscreteStates(fmu->fmi2.component, eventInfo);
}

fmi2Status fmi2EnterContinuousTimeMode(fmiHandle* fmu)
{
    TRACEFUNC
    return fmu->fmi2.enterContinuousTimeMode(fmu->fmi2.component);
}

fmi2Status fmi2CompletedIntegratorStep(fmiHandle* fmu,
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

fmi2Status fmi2SetTime(fmiHandle* fmu, fmi2Real time)
{
    TRACEFUNC
    return fmu->fmi2.setTime(fmu->fmi2.component, time);
}

fmi2Status fmi2SetContinuousStates(fmiHandle* fmu,
                                   const fmi2Real x[],
                                   size_t nx)
{
    TRACEFUNC
    return fmu->fmi2.setContinuousStates(fmu->fmi2.component, x, nx);
}

fmi2Status fmi2GetDerivatives(fmiHandle* fmu, fmi2Real derivatives[], size_t nx)
{
    TRACEFUNC
    return fmu->fmi2.getDerivatives(fmu->fmi2.component, derivatives, nx);
}

fmi2Status fmi2GetEventIndicators(fmiHandle* fmu, fmi2Real eventIndicators[], size_t ni)
{
    TRACEFUNC
    return fmu->fmi2.getEventIndicators(fmu->fmi2.component, eventIndicators, ni);
}

fmi2Status fmi2GetContinuousStates(fmiHandle* fmu, fmi2Real x[], size_t nx)
{
    TRACEFUNC
    return fmu->fmi2.getContinuousStates(fmu->fmi2.component, x, nx);
}

fmi2Status fmi2GetNominalsOfContinuousStates(fmiHandle* fmu, fmi2Real x_nominal[], size_t nx)
{
    TRACEFUNC
    return fmu->fmi2.getNominalsOfContinuousStates(fmu->fmi2.component, x_nominal, nx);
}

fmi2Status fmi2SetRealInputDerivatives(fmiHandle* fmu, const fmi2ValueReference [], size_t, const fmi2Integer [], const fmi2Real []);
fmi2Status fmi2GetRealOutputDerivatives(fmiHandle* fmu, const fmi2ValueReference [], size_t, const fmi2Integer [], fmi2Real []);

fmi2Status fmi2SetRealInputDerivatives(fmiHandle* fmu,
                                       const fmi2ValueReference vr[],
                                       size_t nvr,
                                       const fmi2Integer order[],
                                       const fmi2Real value[])
{
    TRACEFUNC
    return fmu->fmi2.setRealInputDerivatives(fmu->fmi2.component, vr, nvr, order, value);
}

fmi2Status fmi2GetRealOutputDerivatives (fmiHandle* fmu,
                                         const fmi2ValueReference vr[],
                                         size_t nvr,
                                         const fmi2Integer order[],
                                         fmi2Real value[])
{
    TRACEFUNC
    return fmu->fmi2.getRealOutputDerivatives(fmu->fmi2.component, vr, nvr, order, value);
}

fmi2Status fmi2DoStep(fmiHandle *fmu, fmi2Real currentCommunicationPoint, fmi2Real communicationStepSize, fmi2Boolean noSetFMUStatePriorToCurrentPoint)
{
    return fmu->fmi2.doStep(fmu->fmi2.component,
                              currentCommunicationPoint,
                              communicationStepSize,
                              noSetFMUStatePriorToCurrentPoint);
}

const char *fmi2GetGuid(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi2.guid;
}

const char *fmi2GetModelIdentifier(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.modelIdentifier;
}

bool fmi2GetNeedsExecutionTool(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.needsExecutionTool;
}

bool fmi2GetCanHandleVariableCommunicationStepSize(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.canHandleVariableCommunicationStepSize;
}

bool fmi2GetCanInterpolateInputs(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.canInterpolateInputs;
}

int fmi2GetMaxOutputDerivativeOrder(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.maxOutputDerivativeOrder;
}

bool fmi2GetCanRunAsynchronuously(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.canRunAsynchronuously;
}

bool fmi2GetCanBeInstantiatedOnlyOncePerProcess(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.canBeInstantiatedOnlyOncePerProcess;
}

bool fmi2GetCanNotUseMemoryManagementFunctions(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.canNotUseMemoryManagementFunctions;
}

bool fmi2GetCanGetAndSetFMUState(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.canGetAndSetFMUState;
}

bool fmi2GetCanSerializeFMUState(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.canSerializeFMUState;
}

bool fmi2GetProvidesDirectionalDerivative(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.providesDirectionalDerivative;
}

int fmi2GetNumberOfContinuousStates(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.numberOfContinuousStates;
}

int fmi2GetNumberOfEventIndicators(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.numberOfEventIndicators;
}

bool fmi2GetSupportsCoSimulation(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.supportsCoSimulation;
}

bool fmi2GetSupportsModelExchange(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi2.supportsModelExchange;
}

const char *fmi3GetModelIdentifier(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.modelIdentifier;
}

bool fmi3GetCanHandleVariableCommunicationStepSize(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.canHandleVariableCommunicationStepSize;
}

bool fmi3GetCanReturnEarlyAfterIntermediateUpdate(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.canReturnEarlyAfterIntermediateUpdate;
}

double fmi3GetFixedInternalStepSize(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.fixedInternalStepSize;
}

bool fmi3GetHasEventMode(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.hasEventMode;
}

bool fmi3GetNeedsExecutionTool(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.needsExecutionTool;
}

bool fmi3GetCanBeInstantiatedOnlyOncePerProcess(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.canBeInstantiatedOnlyOncePerProcess;
}

bool fmi3GetCanGetAndSetFMUState(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.canGetAndSetFMUState;
}

bool fmi3GetCanSerializeFMUState(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.canSerializeFMUState;
}

bool fmi3GetProvidesDirectionalDerivative(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.providesDirectionalDerivative;
}

bool fmi3GetProvidesAdjointDerivatives(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.providesAdjointDerivatives;
}

bool fmi3GetProvidesPerElementDependencies(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.providesPerElementDependencies;
}

int fmi3GetMaxOutputDerivativeOrder(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.maxOutputDerivativeOrder;
}

bool fmi3GetProvidesIntermediateUpdate(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.providesIntermediateUpdate;
}

bool fmi3GetProvidesEvaluateDiscreteStates(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.providesEvaluateDiscreteStates;
}

bool fmi3GetRecommendedIntermediateInputSmoothness(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.recommendedIntermediateInputSmoothness;
}

bool fmi3GetNeedsCompletedIntegratorStep(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi3.needsCompletedIntegratorStep;
}


//! @brief Loads the FMU as version 1.
//! First parses modelDescription.xml, then loads all required FMI functions.
//! @param fmu FMU handle
//! @returns Handle to FMU with FMI version 1
fmiHandle *loadFmu(const char *fmufile, const char* instanceName)
{
   char cwd[FILENAME_MAX];
#ifdef _WIN32
    _getcwd(cwd, sizeof(char)*FILENAME_MAX);
#else
    getcwd(cwd, sizeof(char)*FILENAME_MAX);
#endif

    int argc = 6;
    const char *argv[6];

#ifdef _WIN32
    mkdir(instanceName);
#else
    mkdir(instanceName, S_IRWXU | S_IRWXG | S_IRWXO);
#endif

    argv[0] = "miniunz";
    argv[1] = "-x";
    argv[2] = "-o";
    argv[3] = fmufile;
    argv[4] = "-d";
    argv[5] = instanceName;

    int status = miniunz(argc, (char**)argv);

    if (status != 0) {
        printf("Failed to unzip FMU: status = %i\n",status);
        return NULL;
    }

    fmiHandle *fmu = malloc(sizeof(fmiHandle));


    //Decide location for where to unzip
    //! @todo Change to temp folder
    char tempPath[FILENAME_MAX];
#ifdef _WIN32
    _getcwd(tempPath, sizeof(char)*FILENAME_MAX);
#else
    getcwd(tempPath, sizeof(char)*FILENAME_MAX);
#endif
    fmu->unzippedLocation = strdup(tempPath);

    strcat(tempPath, "/resources");
    fmu->resourcesLocation = strdup(tempPath);

    fmu->instanceName = instanceName;

    chdir(cwd);

#ifdef _WIN32
    _getcwd(cwd, sizeof(char)*FILENAME_MAX);
#else
    getcwd(cwd, sizeof(char)*FILENAME_MAX);
#endif
    chdir(fmu->unzippedLocation);

    ezxml_t rootElement = ezxml_parse_file("modelDescription.xml");

    if(strcmp(rootElement->name, "fmiModelDescription")) {
        printf("Wrong root tag name: %s\n", rootElement->name);
        free(fmu);
        return false;
    }

    chdir(cwd);

    //Figure out FMI version
    const char* version;
    parseStringAttributeEzXml(rootElement, "fmiVersion", &version);
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
        free(fmu);
        return NULL;
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

    fmu->fmi2.getTypesPlatform = placeholder_fmi2GetTypesPlatform;
    fmu->fmi2.getVersion = placeholder_fmi2GetVersion;
    fmu->fmi2.setDebugLogging = placeholder_fmi2SetDebugLogging;
    fmu->fmi2.instantiate = placeholder_fmi2Instantiate;
    fmu->fmi2.freeInstance = placeholder_fmi2FreeInstance;
    fmu->fmi2.setupExperiment = placeholder_fmi2SetupExperiment;
    fmu->fmi2.enterInitializationMode = placeholder_fmi2EnterInitializationMode;
    fmu->fmi2.exitInitializationMode = placeholder_fmi2ExitInitializationMode;
    fmu->fmi2.terminate = placeholder_fmi2Terminate;
    fmu->fmi2.reset = placeholder_fmi2Reset;
    fmu->fmi2.getReal = placeholder_fmi2GetReal;
    fmu->fmi2.getInteger = placeholder_fmi2GetInteger;
    fmu->fmi2.getBoolean = placeholder_fmi2GetBoolean;
    fmu->fmi2.getString = placeholder_fmi2GetString;
    fmu->fmi2.setReal = placeholder_fmi2SetReal;
    fmu->fmi2.setInteger = placeholder_fmi2SetInteger;
    fmu->fmi2.setBoolean = placeholder_fmi2SetBoolean;
    fmu->fmi2.setString = placeholder_fmi2SetString;
    fmu->fmi2.getFMUstate = placeholder_fmi2GetFMUstate;
    fmu->fmi2.setFMUstate = placeholder_fmi2SetFMUstate;
    fmu->fmi2.freeFMUstate = placeholder_fmi2FreeFMUstate;
    fmu->fmi2.serializedFMUstateSize = placeholder_fmi2SerializedFMUstateSize;
    fmu->fmi2.serializeFMUstate = placeholder_fmi2SerializeFMUstate;
    fmu->fmi2.deSerializeFMUstate = placeholder_fmi2DeSerializeFMUstate;
    fmu->fmi2.getDirectionalDerivative = placeholder_fmi2GetDirectionalDerivative;
    fmu->fmi2.enterEventMode = placeholder_fmi2EnterEventMode;
    fmu->fmi2.newDiscreteStates = placeholder_fmi2NewDiscreteStates;
    fmu->fmi2.enterContinuousTimeMode = placeholder_fmi2EnterContinuousTimeMode;
    fmu->fmi2.completedIntegratorStep = placeholder_fmi2CompletedIntegratorStep;
    fmu->fmi2.setTime = placeholder_fmi2SetTime;
    fmu->fmi2.setContinuousStates = placeholder_fmi2SetContinuousStates;
    fmu->fmi2.getDerivatives = placeholder_fmi2GetDerivatives;
    fmu->fmi2.getEventIndicators = placeholder_fmi2GetEventIndicators;
    fmu->fmi2.getContinuousStates = placeholder_fmi2GetContinuousStates;
    fmu->fmi2.getNominalsOfContinuousStates = placeholder_fmi2GetNominalsOfContinuousStates;
    fmu->fmi2.setRealInputDerivatives = placeholder_fmi2SetRealInputDerivatives;
    fmu->fmi2.getRealOutputDerivatives = placeholder_fmi2GetRealOutputDerivatives;
    fmu->fmi2.doStep = placeholder_fmi2DoStep;
    fmu->fmi2.cancelStep = placeholder_fmi2CancelStep;
    fmu->fmi2.getStatus = placeholder_fmi2GetStatus;
    fmu->fmi2.getRealStatus = placeholder_fmi2GetRealStatus;
    fmu->fmi2.getIntegerStatus = placeholder_fmi2GetIntegerStatus;
    fmu->fmi2.getBooleanStatus = placeholder_fmi2GetBooleanStatus;
    fmu->fmi2.getStringStatus = placeholder_fmi2GetStringStatus;

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
        fmu->fmi1.variables = malloc(100*sizeof(fmi1VariableHandle));
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
        fmu->fmi2.variables = malloc(100*sizeof(fmi2VariableHandle));
        fmu->fmi2.variablesSize = 100;
        fmu->fmi2.numberOfVariables = 0;
        if(!parseModelDescriptionFmi2(fmu)) {
            printf("Failed to parse modelDescription.xml\n");
            free(fmu);
            return NULL;
        }
        if(!loadFunctionsFmi2(fmu)) {
            free(fmu);
            return NULL;    //Error message should already have been printed
        }
    }
    else if(fmu->version == fmiVersion3) {
        fmu->fmi3.variables = malloc(100*sizeof(fmi3VariableHandle));
        fmu->fmi3.variablesSize = 100;
        fmu->fmi3.numberOfVariables = 0;
        if(!parseModelDescriptionFmi3(fmu)) {
            printf("Failed to parse modelDescription.xml\n");
            free(fmu);
            return NULL;
        }
        if(!loadFunctionsFmi3(fmu)) {
            free(fmu);
            return NULL;
        }
    }

    return fmu;
}


//! @brief Free FMU dll
//! @param fmu FMU handle
void freeFmu(fmiHandle *fmu)
{
    TRACEFUNC
#ifdef _WIN32
    FreeLibrary(fmu->dll);
#else
    dlclose(fmu->dll);
#endif
}

fmi1Type fmi1GetType(fmiHandle *fmu)
{
    TRACEFUNC

    return fmu->fmi1.type;
}

int fmi1GetNumberOfContinuousStates(fmiHandle *fmu)
{
    return fmu->fmi1.numberOfContinuousStates;
}

int fmi1GetNumberOfEventIndicators(fmiHandle *fmu)
{
    return fmu->fmi1.numberOfEventIndicators;
}

bool fmi1DefaultStartTimeDefined(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.defaultStartTimeDefined;
}

bool fmi1DefaultStopTimeDefined(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.defaultStopTimeDefined;
}

bool fmi1DefaultToleranceDefined(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.defaultToleranceDefined;
}

double fmi1GetDefaultStartTime(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.defaultStartTime;
}

double fmi1GetDefaultStopTime(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.defaultStopTime;
}

double fmi1GetDefaultTolerance(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.defaultTolerance;
}

int fmi1GetNumberOfVariables(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.numberOfVariables;
}

fmi1VariableHandle *fmi1GetVariableByIndex(fmiHandle *fmu, int i)
{
    TRACEFUNC

    if(i >= fmu->fmi1.numberOfVariables) {
        printf("Variable index out of bounds: %i\n",i);
        return NULL;
    }
    return &fmu->fmi1.variables[i];
}

fmi1VariableHandle *fmi1GetVariableByValueReference(fmiHandle *fmu, fmi1ValueReference vr)
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

fmi1VariableHandle *fmi1GetVariableByName(fmiHandle *fmu, fmi1String name)
{
    for(int i=0; i<fmu->fmi1.numberOfVariables; ++i) {
        if(!strcmp(fmu->fmi1.variables[i].name, name)) {
            return &fmu->fmi1.variables[i];
        }
    }
    printf("Variable with name %s not found.\n", name);
    return NULL;
}

const char *fmi1GetVariableName(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->name;
}

const char *fmi1GetVariableDescription(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->description;
}

fmi1Real fmi1GetVariableStartReal(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->startReal;
}

fmi1Integer fmi1GetVariableStartInteger(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->startInteger;
}

fmi1Boolean fmi1GetVariableStartBoolean(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->startBoolean;
}

fmi1String fmi1GetVariableStartString(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->startString;
}

long fmi1GetVariableValueReference(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->valueReference;
}

fmi1Causality fmi1GetVariableCausality(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->causality;
}

fmi1Variability fmi1GetVariableVariability(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->variability;
}

fmi1Alias fmi1GetAlias(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->alias;
}

bool fmi1GetVariableIsFixed(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->fixed;
}

const char *fmi1GetTypesPlatform(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.getTypesPlatform();
}

const char *fmi1GetVersion(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.getVersion();
}

fmi1Status fmi1SetDebugLogging(fmiHandle *fmu, fmi1Boolean loggingOn)
{
    TRACEFUNC
    return fmu->fmi1.setDebugLogging(fmu->fmi1.component, loggingOn);
}

fmi1Status fmi1GetReal(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1Real values[])
{
    TRACEFUNC
    return fmu->fmi1.getReal(fmu->fmi1.component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1GetInteger(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1Integer values[])
{
    TRACEFUNC
    return fmu->fmi1.getInteger(fmu->fmi1.component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1GetBoolean(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1Boolean values[])
{
    TRACEFUNC
    return fmu->fmi1.getBoolean(fmu->fmi1.component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1GetString(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1String values[])
{
    TRACEFUNC
    return fmu->fmi1.getString(fmu->fmi1.component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1SetReal(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Real values[])
{
    TRACEFUNC
    return fmu->fmi1.setReal(fmu->fmi1.component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1SetInteger(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Integer values[])
{
    TRACEFUNC
    return fmu->fmi1.setInteger(fmu->fmi1.component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1SetBoolean(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Boolean values[])
{
    TRACEFUNC
    return fmu->fmi1.setBoolean(fmu->fmi1.component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1SetString(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1String values[])
{
    TRACEFUNC
    return fmu->fmi1.setString(fmu->fmi1.component, valueReferences, nValueReferences, values);
}

bool fmi1InstantiateSlave(fmiHandle *fmu, fmi1String mimeType, fmi1Real timeOut, fmi1Boolean visible, fmi1Boolean interactive, fmi1CallbackLogger_t logger, fmi1CallbackAllocateMemory_t allocateMemory, fmi1CallbackFreeMemory_t freeMemory, fmi1StepFinished_t stepFinished, fmi3Boolean loggingOn)
{
    TRACEFUNC
    fmu->fmi1.callbacksCoSimulation.logger = logger;
    fmu->fmi1.callbacksCoSimulation.allocateMemory = allocateMemory;
    fmu->fmi1.callbacksCoSimulation.freeMemory = freeMemory;
    fmu->fmi1.callbacksCoSimulation.stepFinished = stepFinished;

    fmu->fmi1.component = fmu->fmi1.instantiateSlave(fmu->instanceName, fmu->fmi1.guid, fmu->unzippedLocation, mimeType, timeOut, visible, interactive, fmu->fmi1.callbacksCoSimulation, loggingOn);

    return (fmu->fmi1.component != NULL);
}

fmi1Status fmi1InitializeSlave(fmiHandle *fmu, fmi1Real startTime, fmi1Boolean stopTimeDefined, fmi1Real stopTime)
{
    TRACEFUNC
    return fmu->fmi1.initializeSlave(fmu->fmi1.component, startTime, stopTimeDefined, stopTime);
}

fmi1Status fmi1TerminateSlave(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.terminateSlave(fmu->fmi1.component);
}

fmi1Status fmi1ResetSlave(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.resetSlave(fmu->fmi1.component);
}

void fmi1FreeSlaveInstance(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.freeSlaveInstance(fmu->fmi1.component);
}

fmi1Status fmi1SetRealInputDerivatives(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Integer orders[], const fmi1Real values[])
{
    TRACEFUNC
    return fmu->fmi1.setRealInputDerivatives(fmu->fmi1.component, valueReferences, nValueReferences, orders, values);
}

fmi1Status fmi1GetRealOutputDerivatives(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Integer orders[], fmi1Real values[])
{
    TRACEFUNC
    return fmu->fmi1.getRealOutputDerivatives(fmu->fmi1.component, valueReferences, nValueReferences, orders, values);
}

fmi1Status fmi1CancelStep(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.cancelStep(fmu->fmi1.component);
}

fmi1Status fmi1DoStep(fmiHandle *fmu, fmi1Real currentCommunicationPoint, fmi1Real communicationStepSize, fmi1Boolean newStep)
{
    TRACEFUNC
    return fmu->fmi1.doStep(fmu->fmi1.component, currentCommunicationPoint, communicationStepSize, newStep);
}

fmi1Status fmi1GetStatus(fmiHandle *fmu, const fmi1StatusKind statusKind, fmi1Status *value)
{
    TRACEFUNC
    return fmu->fmi1.getStatus(fmu->fmi1.component, statusKind, value);
}

fmi1Status fmi1GetRealStatus(fmiHandle *fmu, const fmi1StatusKind statusKind, fmi1Real *value)
{
    TRACEFUNC
    return fmu->fmi1.getRealStatus(fmu->fmi1.component, statusKind, value);
}

fmi1Status fmi1GetIntegerStatus(fmiHandle *fmu, const fmi1StatusKind statusKind, fmi1Integer *value)
{
    TRACEFUNC
    return fmu->fmi1.getIntegerStatus(fmu->fmi1.component, statusKind, value);
}

fmi1Status fmi1GetBooleanStatus(fmiHandle *fmu, const fmi1StatusKind statusKind, fmi1Boolean *value)
{
    TRACEFUNC
    return fmu->fmi1.getBooleanStatus(fmu->fmi1.component, statusKind, value);
}

fmi1Status fmi1GetStringStatus(fmiHandle *fmu, const fmi1StatusKind statusKind, fmi1String *value)
{
    TRACEFUNC
    return fmu->fmi1.getStringStatus(fmu->fmi1.component, statusKind, value);
}

const char *fmi1GetModelTypesPlatform(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.getModelTypesPlatform();
}


bool fmi1InstantiateModel(fmiHandle *fmu, fmi1CallbackLogger_t logger, fmi1CallbackAllocateMemory_t allocateMemory, fmi1CallbackFreeMemory_t freeMemory, fmi1Boolean loggingOn)
{
    TRACEFUNC
    fmu->fmi1.callbacksModelExchange.logger = logger;
    fmu->fmi1.callbacksModelExchange.allocateMemory = allocateMemory;
    fmu->fmi1.callbacksModelExchange.freeMemory = freeMemory;
    fmu->fmi1.component = fmu->fmi1.instantiateModel(fmu->instanceName, fmu->fmi1.guid, fmu->fmi1.callbacksModelExchange, loggingOn);

    return (fmu->fmi1.component != NULL);
}

void fmi1FreeModelInstance(fmiHandle *fmu)
{
    TRACEFUNC
    fmu->fmi1.freeModelInstance(fmu->fmi1.component);
}

fmi1Status fmi1SetTime(fmiHandle *fmu, fmi1Real time)
{
    TRACEFUNC
    return fmu->fmi1.setTime(fmu->fmi1.component, time);
}

fmi1Status fmi1SetContinuousStates(fmiHandle *fmu, const fmi1Real values[], size_t nStates)
{
    TRACEFUNC
    return fmu->fmi1.setContinuousStates(fmu->fmi1.component, values, nStates);
}

fmi1Status fmi1CompletedIntegratorStep(fmiHandle *fmu, fmi1Boolean *callEventUpdate)
{
    TRACEFUNC
    return fmu->fmi1.completedIntegratorStep(fmu->fmi1.component, callEventUpdate);
}

fmi1Status fmi1Initialize(fmiHandle *fmu, fmi1Boolean toleranceControlled, fmi1Real relativeTolerance, fmi1EventInfo *eventInfo)
{
    TRACEFUNC
    return fmu->fmi1.initialize(fmu->fmi1.component, toleranceControlled, relativeTolerance, eventInfo);
}

fmi1Status fmi1GetDerivatives(fmiHandle *fmu, fmi1Real derivatives[], size_t nDerivatives)
{
    TRACEFUNC
    return fmu->fmi1.getDerivatives(fmu->fmi1.component, derivatives, nDerivatives);
}

fmi1Status fmi1GetEventIndicators(fmiHandle *fmu, fmi1Real indicators[], size_t nIndicators)
{
    TRACEFUNC
    return fmu->fmi1.getEventIndicators(fmu->fmi1.component, indicators, nIndicators);
}

fmi1Status fmi1EventUpdate(fmiHandle *fmu, fmi1Boolean intermediateResults, fmi1EventInfo *eventInfo)
{
    TRACEFUNC
    return fmu->fmi1.eventUpdate(fmu->fmi1.component, intermediateResults, eventInfo);
}

fmi1Status fmi1GetContinuousStates(fmiHandle *fmu, fmi1Real states[], size_t nStates)
{
    TRACEFUNC
    return fmu->fmi1.getContinuousStates(fmu->fmi1.component, states, nStates);
}

fmi1Status fmi1GetNominalContinuousStates(fmiHandle *fmu, fmi1Real nominals[], size_t nNominals)
{
    TRACEFUNC
    return fmu->fmi1.getNominalContinuousStates(fmu->fmi1.component, nominals, nNominals);
}

fmi1Status fmi1GetStateValueReferences(fmiHandle *fmu, fmi1ValueReference valueReferences[], size_t nValueReferences)
{
    TRACEFUNC
    return fmu->fmi1.getStateValueReferences(fmu->fmi1.component, valueReferences, nValueReferences);
}

fmi1Status fmi1Terminate(fmiHandle *fmu)
{
    TRACEFUNC
    return fmu->fmi1.terminate(fmu->fmi1.component);
}

fmi1DataType fmi1GetVariableDataType(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->datatype;
}

int fmi3GetNumberOfUnits(fmiHandle *fmu)
{
    return fmu->fmi3.numberOfUnits;
}

fmi3UnitHandle *fmi3GetUnitByIndex(fmiHandle *fmu, int i)
{
    return &fmu->fmi3.units[i];
}

const char *fmi3GetUnitName(fmi3UnitHandle *unit)
{
    return unit->name;
}

bool fmi3HasBaseUnit(fmi3UnitHandle *unit)
{
    return (unit->baseUnit != NULL);
}

void fmi3GetBaseUnit(fmi3UnitHandle *unit, double *factor, double *offset,
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

int fmi3GetNumberOfDisplayUnits(fmi3UnitHandle *unit)
{
    return unit->numberOfDisplayUnits;
}

void fmi3GetDisplayUnitByIndex(fmi3UnitHandle *unit, int id, const char **name, double *factor, double *offset, bool *inverse)
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

void fmi3GetFloat64Type(fmiHandle *fmu,
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

void fmi3GetFloat32Type(fmiHandle *fmu,
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

void fmi3GetInt64Type(fmiHandle *fmu,
                      const char *name,
                      const char **description,
                      const char **quantity,
                      double *min,
                      double *max)
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

void fmi3GetInt32Type(fmiHandle *fmu,
                      const char *name,
                      const char **description,
                      const char **quantity,
                      double *min,
                      double *max)
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

void fmi3GetInt16Type(fmiHandle *fmu,
                      const char *name,
                      const char **description,
                      const char **quantity,
                      double *min,
                      double *max)
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

void fmi3GetInt8Type(fmiHandle *fmu,
                      const char *name,
                      const char **description,
                      const char **quantity,
                      double *min,
                      double *max)
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

void fmi3GetUInt64Type(fmiHandle *fmu,
                      const char *name,
                      const char **description,
                      const char **quantity,
                      double *min,
                      double *max)
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

void fmi3GetUInt32Type(fmiHandle *fmu,
                      const char *name,
                      const char **description,
                      const char **quantity,
                      double *min,
                      double *max)
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

void fmi3GetUInt16Type(fmiHandle *fmu,
                      const char *name,
                      const char **description,
                      const char **quantity,
                      double *min,
                      double *max)
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

void fmi3GetUInt8Type(fmiHandle *fmu,
                      const char *name,
                      const char **description,
                      const char **quantity,
                      double *min,
                      double *max)
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

void fmi3GetBooleanType(fmiHandle *fmu,
                      const char *name,
                      const char **description)
{
    for(int i=0; i<fmu->fmi3.numberOfBooleanTypes; ++i) {
        if(!strcmp(fmu->fmi3.booleanTypes[i].name, name)) {
            *description = fmu->fmi3.booleanTypes[i].description;
        }
    }
}

void fmi3GetStringType(fmiHandle *fmu,
                      const char *name,
                      const char **description)
{
    for(int i=0; i<fmu->fmi3.numberOfStringTypes; ++i) {
        if(!strcmp(fmu->fmi3.stringTypes[i].name, name)) {
            *description = fmu->fmi3.stringTypes[i].description;
        }
    }
}

void fmi3GetBinaryType(fmiHandle *fmu,
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

void fmi3GetEnumerationType(fmiHandle *fmu,
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

void fmi3GetClockType(fmiHandle *fmu,
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

void fmi3GetEnumerationItem(fmiHandle *fmu, const char *typeName, int itemId, const char **itemName, int64_t *value, const char **description)
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


int fmi3GetNumberOfLogCategories(fmiHandle *fmu)
{
    return fmu->fmi3.numberOfLogCategories;
}

void fmi3GetLogCategory(fmiHandle *fmu, int id, const char **name, const char **description)
{
    if(id < fmu->fmi3.numberOfLogCategories) {
        *name = fmu->fmi3.logCategories[id].name;
        *description = fmu->fmi3.logCategories[id].description;
    }
}

int fmi3GetNumberOfModelStructureOutputs(fmiHandle *fmu)
{
    return fmu->fmi3.numberOfOutputs;
}

void fmi3GetModelStructureOutput(fmiHandle *fmu,
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

fmi3ValueReference fmi3GetModelStructureOutputDependency(fmiHandle *fmu, int outputId, int dependencyId)
{
    if(outputId < fmu->fmi3.numberOfOutputs &&
       dependencyId < fmu->fmi3.outputs[outputId].numberOfDependencies) {
        return fmu->fmi3.outputs[outputId].dependencies[dependencyId];
    }
    // TODO return value if failure ?
}

fmi3ValueReference fmi3GetModelStructureOutputDependencyKind(fmiHandle *fmu, int outputId, int dependencyId)
{
    if(outputId < fmu->fmi3.numberOfOutputs &&
       dependencyId < fmu->fmi3.outputs[outputId].numberOfDependencies &&
       fmu->fmi3.outputs[outputId].dependencyKindsDefined) {
        return fmu->fmi3.outputs[outputId].dependencyKinds[dependencyId];
    }
    // TODO return value if failure ?
}

int fmi3GetNumberOfModelStructureContinuousStateDerivatives(fmiHandle *fmu)
{
    return fmu->fmi3.numberOfContinuousStateDerivatives;
}

void fmi3GetModelStructureContinuousStateDerivative(fmiHandle *fmu,
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

fmi3ValueReference fmi3GetModelStructureContinuousStateDerivativeDependency(fmiHandle *fmu,
                                                          int derId,
                                                          int dependencyId)
{
    if(derId < fmu->fmi3.numberOfContinuousStateDerivatives &&
       dependencyId < fmu->fmi3.continuousStateDerivatives[derId].numberOfDependencies) {
        return fmu->fmi3.continuousStateDerivatives[derId].dependencies[dependencyId];
    }
    // TODO return value if failure ?
}

fmi3ValueReference fmi3GetModelStructureContinuousStateDerivativeDependencyKind(fmiHandle *fmu,
                                                              int derId,
                                                              int dependencyId)
{
    if(derId < fmu->fmi3.numberOfContinuousStateDerivatives &&
       dependencyId < fmu->fmi3.continuousStateDerivatives[derId].numberOfDependencies &&
       fmu->fmi3.continuousStateDerivatives[derId].dependencyKindsDefined) {
        return fmu->fmi3.continuousStateDerivatives[derId].dependencyKinds[dependencyId];
    }
    // TODO return value if failure ?
}

int fmi3GetNumberOfModelStructureClockedStates(fmiHandle *fmu)
{
    return fmu->fmi3.numberOfClockedStates;
}

void fmi3GetModelStructureClockedState(fmiHandle *fmu,
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

fmi3ValueReference fmi3GetModelStructureClockedStateDependency(fmiHandle *fmu,
                                                          int clockId,
                                                          int dependencyId)
{
    if(clockId < fmu->fmi3.numberOfClockedStates &&
       dependencyId < fmu->fmi3.clockedStates[clockId].numberOfDependencies) {
        return fmu->fmi3.clockedStates[clockId].dependencies[dependencyId];
    }
    // TODO return value if failure ?
}

fmi3ValueReference fmi3GetModelStructureClockedStateDependencyKind(fmiHandle *fmu,
                                                              int clockId,
                                                              int dependencyId)
{
    if(clockId < fmu->fmi3.numberOfClockedStates &&
       dependencyId < fmu->fmi3.clockedStates[clockId].numberOfDependencies &&
       fmu->fmi3.clockedStates[clockId].dependencyKindsDefined) {
        return fmu->fmi3.clockedStates[clockId].dependencyKinds[dependencyId];
    }
    // TODO return value if failure ?
}

int fmi3GetNumberOfModelStructureInitialUnknowns(fmiHandle *fmu)
{
    return fmu->fmi3.numberOfInitialUnknowns;
}

void fmi3GetModelStructureInitialUnknown(fmiHandle *fmu,
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

fmi3ValueReference fmi3GetModelStructureInitialUnknownDependency(fmiHandle *fmu,
                                                          int unknownId,
                                                          int dependencyId)
{
    if(unknownId < fmu->fmi3.numberOfInitialUnknowns &&
       dependencyId < fmu->fmi3.initialUnknowns[unknownId].numberOfDependencies) {
        return fmu->fmi3.initialUnknowns[unknownId].dependencies[dependencyId];
    }
    // TODO return value if failure ?
}

fmi3ValueReference fmi3ModelStructureGetInitialUnknownDependencyKind(fmiHandle *fmu,
                                                              int unknownId,
                                                              int dependencyId)
{
    if(unknownId < fmu->fmi3.numberOfInitialUnknowns &&
       dependencyId < fmu->fmi3.initialUnknowns[unknownId].numberOfDependencies &&
       fmu->fmi3.initialUnknowns[unknownId].dependencyKindsDefined) {
        return fmu->fmi3.initialUnknowns[unknownId].dependencyKinds[dependencyId];
    }
    // TODO return value if failure ?
}

int fmi3GetNumberOfModelStructureEventIndicators(fmiHandle *fmu)
{
    return fmu->fmi3.numberOfEventIndicators;
}

void fmi3GetModelStructureEventIndicator(fmiHandle *fmu,
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

fmi3ValueReference fmi3GetModelStructureEventIndicatorDependency(fmiHandle *fmu,
                                                   int indicatorId,
                                                   int dependencyId)
{
    if(indicatorId < fmu->fmi3.numberOfEventIndicators &&
       dependencyId < fmu->fmi3.eventIndicators[indicatorId].numberOfDependencies) {
        return fmu->fmi3.eventIndicators[indicatorId].dependencies[dependencyId];
    }
    // TODO return value if failure ?
}

fmi3ValueReference fmi3GetModelStructureEventIndicatorDependencyKind(fmiHandle *fmu,
                                                       int indicatorId,
                                                       int dependencyId)
{
    if(indicatorId < fmu->fmi3.numberOfEventIndicators &&
       dependencyId < fmu->fmi3.eventIndicators[indicatorId].numberOfDependencies &&
       fmu->fmi3.eventIndicators[indicatorId].dependencyKindsDefined) {
        return fmu->fmi3.eventIndicators[indicatorId].dependencyKinds[dependencyId];
    }
    // TODO return value if failure ?
}
