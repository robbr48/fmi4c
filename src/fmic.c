#include "fmic_private.h"
#include "3rdparty/libxml2/include/libxml/tree.h"
#include "3rdparty/libxml2/include/libxml/parser.h"
#include "3rdparty/minizip/miniunz.h"

#include <sys/stat.h>
#include <stdlib.h>

#include "fmic_utils.h"

//! @brief Unzips an FMU file
//! @param fmuFile Path to zip file
//! @param instanceName Instance name for FMU
fmiHandle* unzipFmu(const char* fmuFile, const char* instanceName)
{
    printf("Entering unzip()\n");
    char *cwd = (char*)malloc(sizeof(char)*FILENAME_MAX);
    _getcwd(cwd, sizeof(char)*FILENAME_MAX);
    printf("CWD (1): %s\n",cwd);

    int argc = 6;
    const char *argv[6];

    mkdir(instanceName);

    argv[0]="miniunz";
    argv[1]="-x";
    argv[2]="-o";
    argv[3]=fmuFile;
    argv[4]="-d";
    argv[5]=instanceName;

    int status = miniunz(argc, (char**)argv);

    if (status != 0) {
        printf("Failed to unzip FMU: status = %i\n",status);
        return NULL;
    }

    fmiHandle *fmu = malloc(sizeof(fmiHandle));

    //Decide location for where to unzip
    //! @todo Change to temp folder
    char tempPath[FILENAME_MAX];
    _getcwd(tempPath, FILENAME_MAX);
    fmu->unzippedLocation = strdup(tempPath);

    strcat(tempPath, "/resources");
    fmu->resourcesLocation = strdup(tempPath);

    fmu->instanceName = instanceName;

    chdir(cwd);
    printf("CWD (2): %s\n",cwd);
    printf("Successfully unzipped FMU\n");
    return fmu;
}


//! @brief Parses modelDescription.xml for FMI 1
//! @param fmu FMU handle
//! @returns True if parsing was successful
bool parseModelDescriptionFmi1(fmi1Handle *fmu)
{
    fmu->defaultStartTimeDefined = false;
    fmu->defaultStopTimeDefined = false;
    fmu->defaultToleranceDefined = false;

    fmu->defaultStartTimeDefined = false;
    fmu->defaultStopTimeDefined = false;
    fmu->defaultToleranceDefined = false;

    fmu->canHandleVariableCommunicationStepSize = false;
    fmu->canInterpolateInputs = false;
    fmu->canRunAsynchronuously = false;
    fmu->canBeInstantiatedOnlyOncePerProcess = false;
    fmu->canNotUseMemoryManagementFunctions = false;
    fmu->maxOutputDerivativeOrder = 0;

    fmu->type = fmi1ModelExchange;

    char cwd[FILENAME_MAX];
    _getcwd(cwd, FILENAME_MAX);

    chdir(fmu->unzippedLocation);

    xmlKeepBlanksDefault(0);

    xmlDoc *doc = NULL;
    xmlNode *rootElement = NULL;

    doc = xmlReadFile("modelDescription.xml", NULL, 0);
    if(NULL == doc){
       printf("Failed to read modelDescription.xml");
       return false;
    }

    rootElement = xmlDocGetRootElement(doc);
    if(!strcmp(rootElement->name, "fmiModelDescription")) {
        printf("Correct root element: %s\n", rootElement->name);
    }
    else {
        printf("Wrong root element: %s\n", rootElement->name);
    }

    //Parse attributes in <fmiModelDescription>
    for(xmlAttr *attr = rootElement->properties; attr != NULL; attr = attr->next) {
        if(!strcmp(attr->name, "modelName")) {
            fmu->modelName = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "modelIdentifier")) {
            fmu->modelIdentifier = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "guid")) {
            fmu->guid = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "description")) {
            fmu->description = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "author")) {
            fmu->author = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "version")) {
            fmu->version = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "generationtool")) {
            fmu->generationTool = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "generationDateAndTime")) {
            fmu->generationDateAndTime = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "variableNamingConvention")) {
            fmu->variableNamingConvention = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "numberOfContinuousStates")) {
            fmu->numberOfContinuousStates = parseIntegerAttribute(attr);
        }
        if(!strcmp(attr->name, "numberOfEventIndicators")) {
            fmu->numberOfEventIndicators = parseIntegerAttribute(attr);
        }
    }

    xmlNode *node = rootElement->children;
    for(; node != NULL; node = node->next) {

        if(!strcmp(node->name, "Implementation")) {
            xmlNode *cosimNode = node->children;
            if(!strcmp(cosimNode->name, "CoSimulation_Tool")) {
                fmu->type = fmi1CoSimulationTool;
            }
            else if(!strcmp(cosimNode->name, "CoSimulation_StandAlone")) {
                fmu->type = fmi1CoSimulationStandAlone;
            }
            if(!strcmp(node->name, "CoSimulation_StandAlone") || !strcmp(node->name, "CoSimulation_Tool")) {
                for(; cosimNode!= NULL; cosimNode = cosimNode->next) {
                    if(!strcmp(node->name, "Capabilities")) {
                        for(xmlAttr *attr = node->properties; attr != NULL; attr = attr->next) {
                            if(!strcmp(attr->name, "canHandleVariableCommunicationStepSize")) {
                                fmu->canHandleVariableCommunicationStepSize = parseBooleanAttribute(attr);
                            }
                            else if(!strcmp(attr->name, "canHandleEvents")) {
                                fmu->canHandleEvents = parseBooleanAttribute(attr);
                            }
                            else if(!strcmp(attr->name, "canRejectSteps")) {
                                fmu->canRejectSteps = parseBooleanAttribute(attr);
                            }
                            else if(!strcmp(attr->name, "canInterpolateInputs")) {
                                fmu->canInterpolateInputs = parseBooleanAttribute(attr);
                            }
                            else if(!strcmp(attr->name, "maxOutputDerivativeOrder")) {
                                fmu->maxOutputDerivativeOrder = parseIntegerAttribute(attr);
                            }
                            else if(!strcmp(attr->name, "canRunAsynchronuously")) {
                                fmu->canRunAsynchronuously = parseBooleanAttribute(attr);
                            }
                            else if(!strcmp(attr->name, "canSignalEvents")) {
                                fmu->canSignalEvents = parseBooleanAttribute(attr);
                            }
                            else if(!strcmp(attr->name, "canBeInstantiatedOnlyOncePerProcess")) {
                                fmu->canBeInstantiatedOnlyOncePerProcess = parseBooleanAttribute(attr);
                            }
                            else if(!strcmp(attr->name, "canNotUseMemoryManagementFunctions")) {
                                fmu->canNotUseMemoryManagementFunctions = parseBooleanAttribute(attr);
                            }
                        }
                    }
                }
            }
        }

        //Parse attributes in <DefaultExperiment>
        if(!strcmp(node->name, "DefaultExperiment")) {
            for(xmlAttr *attr = node->properties; attr != NULL; attr = attr->next) {
                if(!strcmp(attr->name, "startTime")) {
                    fmu->defaultStartTime = parseDoubleAttribute(attr);
                    fmu->defaultStartTimeDefined = true;
                }
                if(!strcmp(attr->name, "stopTime")) {
                    fmu->defaultStopTime = parseDoubleAttribute(attr);
                    fmu->defaultStopTimeDefined = true;
                }
                if(!strcmp(attr->name, "tolerance")) {
                    fmu->defaultTolerance = parseDoubleAttribute(attr);
                    fmu->defaultToleranceDefined = true;
                }
            }
        }

        //Parse model variables
        if(!strcmp(node->name, "ModelVariables")) {
            xmlNode *varNode = node->children;
            for(; varNode != NULL; varNode = varNode->next) {
                printf("Variable Element: %s\n",varNode->name);
                fmi1VariableHandle var;

                //Parse variable attributes
                for(xmlAttr *attr = varNode->properties; attr != NULL; attr = attr->next) {
                    if(!strcmp(attr->name, "name")) {
                        var.name = parseStringAttribute(attr);
                    }
                    if(!strcmp(attr->name, "valueReference")) {
                        var.valueReference = parseLongAttribute(attr);
                    }
                    if(!strcmp(attr->name, "description")) {
                        var.description = parseStringAttribute(attr);
                    }
                    if(!strcmp(attr->name, "causality")) {
                        xmlChar *value = parseStringAttribute(attr);
                        if(!strcmp(value, "input")) {
                            var.causality = fmi1CausalityInput;
                            printf("Input!\n");
                        }
                        else if(!strcmp(value, "output")) {
                            var.causality = fmi1CausalityOutput;
                            printf("Output!\n");
                        }
                        else if(!strcmp(value, "internal")) {
                            var.causality = fmi1CausalityInternal;
                            printf("Internal!\n");
                        }
                        else if(!strcmp(value, "none")) {
                            var.causality = fmi1CausalityNone;
                            printf("None!\n");
                        }
                        else {
                            printf("Unknown causality: %s\n",value);
                            return false;
                        }
                    }
                    if(!strcmp(attr->name, "variability")) {
                        xmlChar *value = parseStringAttribute(attr);
                        if(!strcmp(value, "parameter")) {
                            var.variability = fmi1VariabilityParameter;
                        }
                        else if(!strcmp(value, "constant")) {
                            var.variability = fmi1VariabilityConstant;
                        }
                        else if(!strcmp(value, "discrete")) {
                            var.variability = fmi1VariabilityDiscrete;
                        }
                        else if(!strcmp(value, "continuous")) {
                            var.variability = fmi1VariabilityContinuous;
                        }
                         else {
                          printf("Unknown variability: %s\n", value);
                            return false;
                        }
                    }
                    xmlNode *dataNode = varNode->children;
                    if(dataNode && !strcmp(dataNode->name, "Real")) {
                        printf("Found a real variable!\n");  //!< @todo Remove debug output
                        var.datatype = fmi1DataTypeReal;

                        //Parse variable attributes
                        for(xmlAttr *attr = varNode->properties; attr != NULL; attr = attr->next) {
                            if(!strcmp(attr->name, "start")) {
                                var.startReal = parseDoubleAttribute(attr);
                            }
                            if(!strcmp(attr->name, "start")) {
                                var.fixed = parseBooleanAttribute(attr);
                            }
                        }
                    }
                    else if(dataNode && !strcmp(dataNode->name, "Integer")) {
                        printf("Found an integer variable!\n");  //!< @todo Remove debug output
                        var.datatype = fmi1DataTypeInteger;

                        //Parse variable attributes
                        for(xmlAttr *attr = varNode->properties; attr != NULL; attr = attr->next) {
                            if(!strcmp(attr->name, "start")) {
                                var.startInteger = parseIntegerAttribute(attr);
                            }
                            if(!strcmp(attr->name, "start")) {
                                var.fixed = parseBooleanAttribute(attr);
                            }
                        }
                    }
                    else if(dataNode && !strcmp(dataNode->name, "Boolean")) {
                        printf("Found a boolean variable!\n");  //!< @todo Remove debug output
                        var.datatype = fmi1DataTypeBoolean;

                        //Parse variable attributes
                        for(xmlAttr *attr = varNode->properties; attr != NULL; attr = attr->next) {
                            if(!strcmp(attr->name, "start")) {
                                var.startBoolean = parseBooleanAttribute(attr);
                            }
                            if(!strcmp(attr->name, "start")) {
                                var.fixed = parseBooleanAttribute(attr);
                            }
                        }
                    }
                    else if(dataNode && !strcmp(dataNode->name, "String")) {
                        printf("Found a string variable!\n");  //!< @todo Remove debug output
                        var.datatype = fmi1DataTypeString;

                        //Parse variable attributes
                        for(xmlAttr *attr = varNode->properties; attr != NULL; attr = attr->next) {
                            if(!strcmp(attr->name, "start")) {
                                var.startString = parseStringAttribute(attr);
                            }
                            if(!strcmp(attr->name, "start")) {
                                var.fixed = parseBooleanAttribute(attr);
                            }
                        }
                    }
                }

                if(fmu->numberOfVariables >= fmu->variablesSize) {
                    fmu->variablesSize *= 2;
                    fmu->variables = realloc(fmu->variables, fmu->variablesSize*sizeof(fmi1VariableHandle));
                }

                fmu->variables[fmu->numberOfVariables] = var;
                fmu->numberOfVariables++;
            }
        }
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();

    chdir(cwd);

    return true;
}


//! @brief Parses modelDescription.xml for FMI 2
//! @param fmu FMU handle
//! @returns True if parsing was successful
bool parseModelDescriptionFmi2(fmi2Handle *fmu)
{
    fmu->defaultStartTimeDefined = false;
    fmu->defaultStopTimeDefined = false;
    fmu->defaultToleranceDefined = false;
    fmu->defaultStepSizeDefined = false;

    fmu->defaultStartTimeDefined = false;
    fmu->defaultStopTimeDefined = false;
    fmu->defaultToleranceDefined = false;
    fmu->defaultStepSizeDefined = false;

    fmu->supportsCoSimulation = false;
    fmu->supportsModelExchange = false;

    fmu->needsExecutionTool = false;
    fmu->canHandleVariableCommunicationStepSize = false;
    fmu->canInterpolateInputs = false;
    fmu->canRunAsynchronuously = false;
    fmu->canBeInstantiatedOnlyOncePerProcess = false;
    fmu->canNotUseMemoryManagementFunctions = false;
    fmu->canGetAndSetFMUState = false;
    fmu->canSerializeFMUState = false;
    fmu->providesDirectionalDerivative = false;

    char cwd[FILENAME_MAX];
    _getcwd(cwd, FILENAME_MAX);

    xmlKeepBlanksDefault(0);

    xmlDoc *doc = NULL;
    xmlNode *rootElement = NULL;

    doc = xmlReadFile("modelDescription.xml", NULL, 0);
    if(NULL == doc){
       printf("Failed to read modelDescription.xml");
       return false;
    }

    rootElement = xmlDocGetRootElement(doc);
    if(!strcmp(rootElement->name, "fmiModelDescription")) {
        printf("Correct root element: %s\n", rootElement->name);
    }
    else {
        printf("Wrong root element: %s\n", rootElement->name);
    }

    //Parse attributes in <fmiModelDescription>
    for(xmlAttr *attr = rootElement->properties; attr != NULL; attr = attr->next) {
        if(!strcmp(attr->name, "modelName")) {
            fmu->modelName = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "guid")) {
            fmu->guid = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "description")) {
            fmu->description = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "author")) {
            fmu->author = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "version")) {
            fmu->version = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "copyright")) {
            fmu->copyright = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "license")) {
            fmu->license = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "generationtool")) {
            fmu->generationTool = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "generationDateAndTime")) {
            fmu->generationDateAndTime = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "variableNamingConvention")) {
            fmu->variableNamingConvention = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "numberOfEventIndicators")) {
            fmu->numberOfEventIndicators = parseIntegerAttribute(attr);
        }
    }

    xmlNode *node = rootElement->children;
    for(; node != NULL; node = node->next) {
        printf("Element: %s\n",node->name);
        //Parse arguments in <DefaultExperiment>
         if(!strcmp(node->name, "CoSimulation")) {
             fmu->supportsCoSimulation = true;

             if(!strcmp(node->name, "CoSimulation")) {
                 for(xmlAttr *attr = node->properties; attr != NULL; attr = attr->next) {
                     if(!strcmp(attr->name, "modelIdentifier")) {
                         fmu->modelIdentifier = parseStringAttribute(attr);
                     }
                     else if(!strcmp(attr->name, "needsExecutionTool")) {
                         fmu->needsExecutionTool = parseBooleanAttribute(attr);
                     }
                     else if(!strcmp(attr->name, "canHandleVariableCommunicationStepSize")) {
                         fmu->canHandleVariableCommunicationStepSize = parseBooleanAttribute(attr);
                     }
                     else if(!strcmp(attr->name, "canInterpolateInputs")) {
                         fmu->canInterpolateInputs = parseBooleanAttribute(attr);
                     }
                     else if(!strcmp(attr->name, "maxOutputDerivativeOrder")) {
                         fmu->maxOutputDerivativeOrder = parseIntegerAttribute(attr);
                     }
                     else if(!strcmp(attr->name, "canRunAsynchronuously")) {
                         fmu->canRunAsynchronuously = parseBooleanAttribute(attr);
                     }
                     else if(!strcmp(attr->name, "canBeInstantiatedOnlyOncePerProcess")) {
                         fmu->canBeInstantiatedOnlyOncePerProcess = parseBooleanAttribute(attr);
                     }
                     else if(!strcmp(attr->name, "canNotUseMemoryManagementFunctions")) {
                         fmu->canNotUseMemoryManagementFunctions = parseBooleanAttribute(attr);
                     }
                     else if(!strcmp(attr->name, "canGetAndSetFMUState")) {
                         fmu->canGetAndSetFMUState = parseBooleanAttribute(attr);
                     }
                     else if(!strcmp(attr->name, "canSerializeFMUState")) {
                         fmu->canSerializeFMUState = parseBooleanAttribute(attr);
                     }
                     else if(!strcmp(attr->name, "providesDirectionalDerivative")) {
                         fmu->providesDirectionalDerivative = parseBooleanAttribute(attr);
                     }
                 }
             }
         }
         if(!strcmp(node->name, "ModelExchange")) {
             fmu->supportsModelExchange = true;

             //! @todo Read model exchange data
         }
        //Parse arguments in <DefaultExperiment>
        if(!strcmp(node->name, "DefaultExperiment")) {
            for(xmlAttr *attr = node->properties; attr != NULL; attr = attr->next) {
                if(!strcmp(attr->name, "startTime")) {
                    fmu->defaultStartTime = parseDoubleAttribute(attr);
                    fmu->defaultStartTimeDefined = true;
                }
                if(!strcmp(attr->name, "stopTime")) {
                    fmu->defaultStopTime = parseDoubleAttribute(attr);
                    fmu->defaultStopTimeDefined = true;
                }
                if(!strcmp(attr->name, "tolerance")) {
                    fmu->defaultTolerance = parseDoubleAttribute(attr);
                    fmu->defaultToleranceDefined = true;
                }
                if(!strcmp(attr->name, "stepSize")) {
                    fmu->defaultStepSize = parseDoubleAttribute(attr);
                    fmu->defaultStepSizeDefined = true;
                }
            }
        }

        //Parse model variables
        if(!strcmp(node->name, "ModelVariables")) {
            xmlNode *varNode = node->children;
            for(; varNode != NULL; varNode = varNode->next) {
                printf("Variable Element: %s\n",varNode->name);
                fmi2VariableHandle var;
                var.canHandleMultipleSetPerTimeInstant = false; //Default value if attribute not defined

                //Parse variable attributes
                for(xmlAttr *attr = varNode->properties; attr != NULL; attr = attr->next) {
                    if(!strcmp(attr->name, "name")) {
                        var.name = parseStringAttribute(attr);
                    }
                    if(!strcmp(attr->name, "valueReference")) {
                        var.valueReference = parseLongAttribute(attr);
                    }
                    if(!strcmp(attr->name, "description")) {
                        var.description = parseStringAttribute(attr);
                    }
                    if(!strcmp(attr->name, "causality")) {
                        xmlChar *value = parseStringAttribute(attr);
                        if(!strcmp(value, "input")) {
                            var.causality = fmi2CausalityInput;
                        }
                        else if(!strcmp(value, "output")) {
                            var.causality = fmi2CausalityOutput;
                        }
                        else if(!strcmp(value, "parameter")) {
                            var.causality = fmi2CausalityParameter;
                        }
                        else if(!strcmp(value, "calculatedparameter")) {
                            var.causality = fmi2CausalityCalculatedParameter;
                        }
                        else if(!strcmp(value, "local")) {
                            var.causality = fmi2CausalityLocal;
                        }
                        else if(!strcmp(value, "independent")) {
                            var.causality = fmi2CausalityIndependent;
                        }
                        else {
                            printf("Unknown causality: %s\n",value);
                            return false;
                        }
                    }
                    if(!strcmp(attr->name, "variability")) {
                        xmlChar *value = parseStringAttribute(attr);
                        if(!strcmp(value, "fixed")) {
                            var.variability = fmi2VariabilityFixed;
                        }
                        else if(!strcmp(value, "tunable")) {
                            var.variability = fmi2VariabilityTunable;
                        }
                        else if(!strcmp(value, "constant")) {
                            var.variability = fmi2VariabilityConstant;
                        }
                        else if(!strcmp(value, "discrete")) {
                            var.variability = fmi2VariabilityDiscrete;
                        }
                        else if(!strcmp(value, "continuous")) {
                            var.variability = fmi2VariabilityContinuous;
                        }
                         else {
                            printf("Unknown variability: %s\n", value);
                            return false;
                        }
                    }
                    if(!strcmp(attr->name, "initial")) {
                        xmlChar *value = parseStringAttribute(attr);
                        if(!strcmp(value, "approx")) {
                            var.variability = fmi2InitialApprox;
                        }
                        else if(!strcmp(value, "calculated")) {
                            var.variability = fmi2InitialCalculated;
                        }
                        else if(!strcmp(value, "exact")) {
                            var.variability = fmi2InitialExact;
                        }
                        else {
                            printf("Unknown initial: %s\n", value);
                            return false;
                        }
                    }
                    if(!strcmp(attr->name, "canHandleMultipleSetPerTimeInstant")) {
                        var.canHandleMultipleSetPerTimeInstant = parseBooleanAttribute(attr);
                    }

                    xmlNode *dataNode = varNode->children;
                    if(dataNode && !strcmp(dataNode->name, "Real")) {
                        var.datatype = fmi2DataTypeReal;
                        printf("Found a real variable!\n");  //!< @todo Remove debug output
                    }
                    else if(dataNode && !strcmp(dataNode->name, "Integer")) {
                        var.datatype = fmi2DataTypeInteger;
                        printf("Found an integer variable!\n");  //!< @todo Remove debug output
                    }
                    else if(dataNode && !strcmp(dataNode->name, "Boolean")) {
                        var.datatype = fmi2DataTypeBoolean;
                        printf("Found a boolean variable!\n");  //!< @todo Remove debug output
                    }
                    else if(dataNode && !strcmp(dataNode->name, "String")) {
                        var.datatype = fmi2DataTypeString;
                        printf("Found a string variable!\n");  //!< @todo Remove debug output
                    }
                }

                if(fmu->numberOfVariables >= fmu->variablesSize) {
                    fmu->variablesSize *= 2;
                    fmu->variables = realloc(fmu->variables, fmu->variablesSize*sizeof(fmi2VariableHandle));
                }

                fmu->variables[fmu->numberOfVariables] = var;
                fmu->numberOfVariables++;
            }
        }
    }


    xmlFreeDoc(doc);
    xmlCleanupParser();

    chdir(cwd);

    return true;
}


//! @brief Parses modelDescription.xml for FMI 3
//! @param fmu FMU handle
//! @returns True if parsing was successful
bool parseModelDescriptionFmi3(fmi3Handle *fmu)
{
    fmu->supportsCoSimulation = false;
    fmu->supportsModelExchange = false;
    fmu->supportsScheduledExecution = false;

    fmu->defaultStartTimeDefined = false;
    fmu->defaultStopTimeDefined = false;
    fmu->defaultToleranceDefined = false;
    fmu->defaultStepSizeDefined = false;

    fmu->needsExecutionTool = false;
    fmu->canBeInstantiatedOnlyOncePerProcess = false;
    fmu->canGetAndSetFMUState = false;
    fmu->canSerializeFMUState = false;
    fmu->providesDirectionalDerivative = false;
    fmu->providesAdjointDerivatives = false;
    fmu->providesPerElementDependencies = false;
    fmu->maxOutputDerivativeOrder = 0;
    fmu->providesIntermediateUpdate = false;
    fmu->mightReturnEarlyFromDoStep = false;
    fmu->providesEvaluateDiscreteStates = false;
    fmu->recommendedIntermediateInputSmoothness = false;
    fmu->canHandleVariableCommunicationStepSize = false;
    fmu->canReturnEarlyAfterIntermediateUpdate = false;
    fmu->fixedInternalStepSize = 0;
    fmu->hasEventMode = false;
    fmu->completedIntegratorStepNotNeeded = false;

    char cwd[FILENAME_MAX];
    _getcwd(cwd, FILENAME_MAX);

    chdir(fmu->unzippedLocation);
    chdir(fmu->instanceName);

    char tempPath[FILENAME_MAX];
    _getcwd(tempPath, FILENAME_MAX);
    fmu->unzippedLocation = strdup(tempPath);

    strcat(tempPath, "/resources");
    fmu->resourcesLocation = strdup(tempPath);

    xmlKeepBlanksDefault(0);

    xmlDoc *doc = NULL;
    xmlNode *rootElement = NULL;

    doc = xmlReadFile("modelDescription.xml", NULL, 0);
    if(NULL == doc){
       printf("Failed to read modelDescription.xml");
       return false;
    }

    rootElement = xmlDocGetRootElement(doc);
    if(!strcmp(rootElement->name, "fmiModelDescription")) {
        printf("Correct root element: %s\n", rootElement->name);
    }
    else {
        printf("Wrong root element: %s\n", rootElement->name);
    }


    printf("FMI version = 3\n");

    //Parse attributes in <fmiModelDescription>
    for(xmlAttr *attr = rootElement->properties; attr != NULL; attr = attr->next) {
        if(!strcmp(attr->name, "modelName")) {
            fmu->modelName = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "instantiationToken")) {
            fmu->instantiationToken = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "description")) {
            fmu->description = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "author")) {
            fmu->author = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "version")) {
            fmu->version = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "copyright")) {
            fmu->copyright = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "license")) {
            fmu->license = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "generationtool")) {
            fmu->generationTool = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "generationDateAndTime")) {
            fmu->generationDateAndTime = parseStringAttribute(attr);
        }
        if(!strcmp(attr->name, "variableNamingConvention")) {
            fmu->variableNamingConvention = parseStringAttribute(attr);
        }
    }

    xmlNode *node = rootElement->children;
    for(; node != NULL; node = node->next) {
        printf("Element: %s\n",node->name);

        //Parse arguments in <CoSimulation>
        if(!strcmp(node->name, "CoSimulation")) {
            fmu->supportsCoSimulation = true;
            for(xmlAttr *attr = node->properties; attr != NULL; attr = attr->next) {
                if(!strcmp(attr->name, "modelIdentifier")) {
                    fmu->modelIdentifier = parseStringAttribute(attr);
                }
                else if(!strcmp(attr->name, "needsExecutionTool")) {
                    fmu->needsExecutionTool = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "canBeInstantiatedOnlyOncePerProcess")) {
                    fmu->canBeInstantiatedOnlyOncePerProcess = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "canGetAndSetFMUState")) {
                    fmu->canGetAndSetFMUState = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "canSerializeFMUState")) {
                    fmu->canSerializeFMUState = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "providesDirectionalDerivative")) {
                    fmu->providesDirectionalDerivative = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "providesAdjointDerivatives")) {
                    fmu->providesAdjointDerivatives = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "providesPerElementDependencies")) {
                    fmu->providesPerElementDependencies = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "maxOutputDerivativeOrder")) {
                    fmu->maxOutputDerivativeOrder = parseIntegerAttribute(attr);
                }
                else if(!strcmp(attr->name, "providesIntermediateUpdate")) {
                    fmu->providesIntermediateUpdate = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "mightReturnEarlyFromDoStep")) {
                    fmu->mightReturnEarlyFromDoStep = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "providesEvaluateDiscreteStates")) {
                    fmu->providesEvaluateDiscreteStates = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "recommendedIntermediateInputSmoothness")) {
                    fmu->recommendedIntermediateInputSmoothness = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "canHandleVariableCommunicationStepSize")) {
                    fmu->canHandleVariableCommunicationStepSize = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "canReturnEarlyAfterIntermediateUpdate")) {
                    fmu->canReturnEarlyAfterIntermediateUpdate = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "fixedInternalStepSize")) {
                    fmu->fixedInternalStepSize = parseDoubleAttribute(attr);
                }
                else if(!strcmp(attr->name, "hasEventMode")) {
                    fmu->hasEventMode = parseBooleanAttribute(attr);
                }
            }
        }

        if(!strcmp(node->name, "ModelExchange")) {
            fmu->supportsModelExchange = true;
            for(xmlAttr *attr = node->properties; attr != NULL; attr = attr->next) {
                if(!strcmp(attr->name, "modelIdentifier")) {
                    fmu->modelIdentifier = parseStringAttribute(attr);
                }
                else if(!strcmp(attr->name, "needsExecutionTool")) {
                    fmu->needsExecutionTool = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "canBeInstantiatedOnlyOncePerProcess")) {
                    fmu->canBeInstantiatedOnlyOncePerProcess = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "canGetAndSetFMUState")) {
                    fmu->canGetAndSetFMUState = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "canSerializeFMUState")) {
                    fmu->canSerializeFMUState = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "providesDirectionalDerivative")) {
                    fmu->providesDirectionalDerivative = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "providesAdjointDerivatives")) {
                    fmu->providesAdjointDerivatives = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "providesPerElementDependencies")) {
                    fmu->providesPerElementDependencies = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "maxOutputDerivativeOrder")) {
                    fmu->maxOutputDerivativeOrder = parseIntegerAttribute(attr);
                }
                else if(!strcmp(attr->name, "providesIntermediateUpdate")) {
                    fmu->providesIntermediateUpdate = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "mightReturnEarlyFromDoStep")) {
                    fmu->mightReturnEarlyFromDoStep = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "providesEvaluateDiscreteStates")) {
                    fmu->providesEvaluateDiscreteStates = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "recommendedIntermediateInputSmoothness")) {
                    fmu->recommendedIntermediateInputSmoothness = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "completedIntegratorStepNotNeeded")) {
                    fmu->completedIntegratorStepNotNeeded = parseBooleanAttribute(attr);
                }
            }
        }

        if(!strcmp(node->name, "ScheduledExecution")) {
            fmu->supportsScheduledExecution = true;
            for(xmlAttr *attr = node->properties; attr != NULL; attr = attr->next) {
                if(!strcmp(attr->name, "modelIdentifier")) {
                    fmu->modelIdentifier = parseStringAttribute(attr);
                }
                else if(!strcmp(attr->name, "needsExecutionTool")) {
                    fmu->needsExecutionTool = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "canBeInstantiatedOnlyOncePerProcess")) {
                    fmu->canBeInstantiatedOnlyOncePerProcess = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "canGetAndSetFMUState")) {
                    fmu->canGetAndSetFMUState = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "canSerializeFMUState")) {
                    fmu->canSerializeFMUState = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "providesDirectionalDerivative")) {
                    fmu->providesDirectionalDerivative = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "providesAdjointDerivatives")) {
                    fmu->providesAdjointDerivatives = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "mightReturnEarlyFromDoStep")) {
                    fmu->mightReturnEarlyFromDoStep = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "providesPerElementDependencies")) {
                    fmu->providesPerElementDependencies = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "maxOutputDerivativeOrder")) {
                    fmu->maxOutputDerivativeOrder = parseIntegerAttribute(attr);
                }
                else if(!strcmp(attr->name, "providesIntermediateUpdate")) {
                    fmu->providesIntermediateUpdate = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "providesEvaluateDiscreteStates")) {
                    fmu->providesEvaluateDiscreteStates = parseBooleanAttribute(attr);
                }
                else if(!strcmp(attr->name, "recommendedIntermediateInputSmoothness")) {
                    fmu->recommendedIntermediateInputSmoothness = parseBooleanAttribute(attr);
                }
            }
        }

        //Parse arguments in <DefaultExperiment>
        if(!strcmp(node->name, "DefaultExperiment")) {
            for(xmlAttr *attr = node->properties; attr != NULL; attr = attr->next) {
                if(!strcmp(attr->name, "startTime")) {
                    fmu->defaultStartTime = parseDoubleAttribute(attr);
                    fmu->defaultStartTimeDefined = true;
                }
                if(!strcmp(attr->name, "stopTime")) {
                    fmu->defaultStopTime = parseDoubleAttribute(attr);
                    fmu->defaultStopTimeDefined = true;
                }
                if(!strcmp(attr->name, "tolerance")) {
                    fmu->defaultTolerance = parseDoubleAttribute(attr);
                    fmu->defaultToleranceDefined = true;
                }
                if(!strcmp(attr->name, "stepSize")) {
                    fmu->defaultStepSize = parseDoubleAttribute(attr);
                    fmu->defaultStepSizeDefined = true;
                }
            }
        }

        //Parse model variables
        if(!strcmp(node->name, "ModelVariables")) {
            xmlNode *varNode = node->children;
            for(; varNode != NULL; varNode = varNode->next) {
                printf("Variable Element: %s\n",varNode->name);
                fmi3VariableHandle var;
                var.intermediateUpdate = false; //Default value if attribute not defined
                if(!strcmp(varNode->name,"Float64")) {
                    var.datatype = fmi3DataTypeFloat64;
                }
                else if(!strcmp(varNode->name,"Float32")) {
                    var.datatype = fmi3DataTypeFloat32;
                }
                else if(!strcmp(varNode->name,"Int64")) {
                    var.datatype = fmi3DataTypeInt64;
                }
                else if(!strcmp(varNode->name,"Int32")) {
                    var.datatype = fmi3DataTypeInt32;
                }
                else if(!strcmp(varNode->name,"Int16")) {
                    var.datatype = fmi3DataTypeInt16;
                }
                else if(!strcmp(varNode->name,"Int8")) {
                    var.datatype = fmi3DataTypeInt8;
                }
                else if(!strcmp(varNode->name,"Uint64")) {
                    var.datatype = fmi3DataTypeUint64;
                }
                else if(!strcmp(varNode->name,"Boolean")) {
                    var.datatype = fmi3DataTypeBoolean;
                }
                else if(!strcmp(varNode->name,"String")) {
                    var.datatype = fmi3DataTypeString;
                }
                else if(!strcmp(varNode->name,"Binary")) {
                    var.datatype = fmi3DataTypeBinary;
                }
                else if(!strcmp(varNode->name,"Clock")) {
                    var.datatype = fmi3DataTypeClock;
                }

                //Parse variable attributes
                for(xmlAttr *attr = varNode->properties; attr != NULL; attr = attr->next) {
                    if(!strcmp(attr->name, "name")) {
                        var.name = parseStringAttribute(attr);
                    }
                    if(!strcmp(attr->name, "description")) {
                        var.description = parseStringAttribute(attr);
                    }
                    if(!strcmp(attr->name, "quantity")) {
                        var.quantity = parseStringAttribute(attr);
                    }
                    if(!strcmp(attr->name, "unit")) {
                        var.unit = parseStringAttribute(attr);
                    }
                    if(!strcmp(attr->name, "displayUnit")) {
                        var.displayUnit = parseStringAttribute(attr);
                    }
                    if(!strcmp(attr->name, "start") && !strcmp(varNode->name, "Float64")) {
                        var.startFloat64 = parseDoubleAttribute(attr);
                    }
                    if(!strcmp(attr->name, "valueReference")) {
                        var.valueReference = parseLongAttribute(attr);
                    }
                     if(!strcmp(attr->name, "causality")) {
                        xmlChar *value = parseStringAttribute(attr);
                        if(!strcmp(value, "input")) {
                            var.causality = fmi3CausalityInput;
                        }
                        else if(!strcmp(value, "output")) {
                            var.causality = fmi3CausalityOutput;
                        }
                        else if(!strcmp(value, "parameter")) {
                            var.causality = fmi3CausalityParameter;
                        }
                        else if(!strcmp(value, "calculatedparameter")) {
                            var.causality = fmi3CausalityCalculatedParameter;
                        }
                        else if(!strcmp(value, "local")) {
                            var.causality = fmi3CausalityLocal;
                        }
                        else if(!strcmp(value, "independent")) {
                            var.causality = fmi3CausalityIndependent;
                        }
                        else if(!strcmp(value, "structuralparameter")) {
                            var.causality = fmi3CausalityStructuralParameter;
                        }
                        else {
                            printf("Unknown causality: %s\n",value);
                            return false;
                        }
                    }
                    if(!strcmp(attr->name, "variability")) {
                        xmlChar *value = parseStringAttribute(attr);
                        if(!strcmp(value, "fixed")) {
                            var.variability = fmi3VariabilityFixed;
                        }
                        else if(!strcmp(value, "tunable")) {
                            var.variability = fmi3VariabilityTunable;
                        }
                        else if(!strcmp(value, "constant")) {
                            var.variability = fmi3VariabilityConstant;
                        }
                        else if(!strcmp(value, "discrete")) {
                            var.variability = fmi3VariabilityDiscrete;
                        }
                        else if(!strcmp(value, "continuous")) {
                            var.variability = fmi3VariabilityContinuous;
                        }
                         else {
                            printf("Unknown variability: %s\n", value);
                            return false;
                        }
                    }
                    if(!strcmp(attr->name, "intermediateUpdate")) {
                        var.intermediateUpdate = parseBooleanAttribute(attr);
                    }
                }

                if(fmu->numberOfVariables >= fmu->variablesSize) {
                    fmu->variablesSize *= 2;
                    fmu->variables = realloc(fmu->variables, fmu->variablesSize*sizeof(fmi3VariableHandle));
                }

                fmu->variables[fmu->numberOfVariables] = var;
                fmu->numberOfVariables++;
            }
        }
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();

    chdir(cwd);

    return true;
}


//! @brief Loads all DLL functions for FMI 1
//! @param fmu FMU handle
//! @returns True if load was successful
bool loadFunctionsFmi1(fmi1Handle *fmu)
{
    TRACEFUNC

    char *dllPath = (char*)malloc(sizeof(char)*FILENAME_MAX);
    _getcwd(dllPath, sizeof(char)*FILENAME_MAX);
    chdir(dllPath);
    strcat(dllPath,"\\binaries\\win64\\");
    strcat(dllPath,fmu->modelIdentifier);
    strcat(dllPath,".dll");
#ifdef _WIN32
    HINSTANCE dll = LoadLibraryA(dllPath);
#else
    void* dll = dlopen(dll_file_path, RTLD_NOW|RTLD_LOCAL);
#endif
    if(NULL == dll) {
        printf("Loading DLL failed: %s\n",dllPath);
        return false;
    }
    printf("Load successful!\n",dllPath);

    fmu->dll = dll;

    fmu->fmiGetTypesPlatform = LOADFUNCTION2(fmiGetTypesPlatform);
    fmu->fmiGetVersion = LOADFUNCTION2(fmiGetVersion);
    fmu->fmiSetDebugLogging = LOADFUNCTION2(fmiSetDebugLogging);
    fmu->fmiGetReal = LOADFUNCTION2(fmiGetReal);
    fmu->fmiGetInteger = LOADFUNCTION2(fmiGetInteger);
    fmu->fmiGetBoolean = LOADFUNCTION2(fmiGetBoolean);
    fmu->fmiGetString = LOADFUNCTION2(fmiGetString);
    fmu->fmiSetReal = LOADFUNCTION2(fmiSetReal);
    fmu->fmiSetInteger = LOADFUNCTION2(fmiSetInteger);
    fmu->fmiSetBoolean = LOADFUNCTION2(fmiSetBoolean);
    fmu->fmiSetString = LOADFUNCTION2(fmiSetString);
    fmu->fmiInstantiateSlave = LOADFUNCTION2(fmiInstantiateSlave);
    fmu->fmiInitializeSlave = LOADFUNCTION2(fmiInitializeSlave);
    fmu->fmiTerminateSlave = LOADFUNCTION2(fmiTerminateSlave);
    fmu->fmiResetSlave = LOADFUNCTION2(fmiResetSlave);
    fmu->fmiFreeSlaveInstance = LOADFUNCTION2(fmiFreeSlaveInstance);
    fmu->fmiSetRealInputDerivatives = LOADFUNCTION2(fmiSetRealInputDerivatives);
    fmu->fmiGetRealOutputDerivatives = LOADFUNCTION2(fmiGetRealOutputDerivatives);
    fmu->fmiCancelStep = LOADFUNCTION2(fmiCancelStep);
    fmu->fmiDoStep = LOADFUNCTION2(fmiDoStep);
    fmu->fmiGetStatus = LOADFUNCTION2(fmiGetStatus);
    fmu->fmiGetRealStatus = LOADFUNCTION2(fmiGetRealStatus);
    fmu->fmiGetIntegerStatus = LOADFUNCTION2(fmiGetIntegerStatus);
    fmu->fmiGetBooleanStatus = LOADFUNCTION2(fmiGetBooleanStatus);
    fmu->fmiGetStringStatus = LOADFUNCTION2(fmiGetStringStatus);
    fmu->fmiGetModelTypesPlatform = LOADFUNCTION2(fmiGetModelTypesPlatform);
    fmu->fmiInstantiateModel = LOADFUNCTION2(fmiInstantiateModel);
    fmu->fmiFreeModelInstance = LOADFUNCTION2(fmiFreeModelInstance);
    fmu->fmiSetTime = LOADFUNCTION2(fmiSetTime);
    fmu->fmiSetContinuousStates = LOADFUNCTION2(fmiSetContinuousStates);
    fmu->fmiCompletedIntegratorStep = LOADFUNCTION2(fmiCompletedIntegratorStep);
    fmu->fmiInitialize = LOADFUNCTION2(fmiInitialize);
    fmu->fmiGetDerivatives = LOADFUNCTION2(fmiGetDerivatives);
    fmu->fmiGetEventIndicators = LOADFUNCTION2(fmiGetEventIndicators);
    fmu->fmiEventUpdate = LOADFUNCTION2(fmiEventUpdate);
    fmu->fmiGetContinuousStates = LOADFUNCTION2(fmiGetContinuousStates);
    fmu->fmiGetNominalContinuousStates = LOADFUNCTION2(fmiGetNominalContinuousStates);
    fmu->fmiGetStateValueReferences = LOADFUNCTION2(fmiGetStateValueReferences);
    fmu->fmiTerminate = LOADFUNCTION2(fmiTerminate);

    CHECKFUNCTION(fmiGetVersion);
    CHECKFUNCTION(fmiSetDebugLogging);
    CHECKFUNCTION(fmiGetReal);
    CHECKFUNCTION(fmiGetInteger);
    CHECKFUNCTION(fmiGetBoolean);
    CHECKFUNCTION(fmiGetString);
    CHECKFUNCTION(fmiSetReal);
    CHECKFUNCTION(fmiSetInteger);
    CHECKFUNCTION(fmiSetBoolean);
    CHECKFUNCTION(fmiSetString);

    if(fmu->type == fmi1ModelExchange) {
        CHECKFUNCTION(fmiInstantiateModel);
        CHECKFUNCTION(fmiFreeModelInstance);
        CHECKFUNCTION(fmiInitialize);
        CHECKFUNCTION(fmiGetDerivatives);
        CHECKFUNCTION(fmiTerminate);
        CHECKFUNCTION(fmiSetTime);
        CHECKFUNCTION(fmiGetModelTypesPlatform);
        CHECKFUNCTION(fmiSetContinuousStates);
        CHECKFUNCTION(fmiCompletedIntegratorStep);
        CHECKFUNCTION(fmiGetEventIndicators);
        CHECKFUNCTION(fmiEventUpdate);
        CHECKFUNCTION(fmiGetContinuousStates);
        CHECKFUNCTION(fmiGetNominalContinuousStates);
        CHECKFUNCTION(fmiGetStateValueReferences);
    }

    if(fmu->type == fmi1CoSimulationStandAlone || fmu->type == fmi1CoSimulationTool) {
        CHECKFUNCTION(fmiGetTypesPlatform);
        CHECKFUNCTION(fmiInstantiateSlave);
        CHECKFUNCTION(fmiInitializeSlave);
        CHECKFUNCTION(fmiTerminateSlave);
        CHECKFUNCTION(fmiResetSlave);
        CHECKFUNCTION(fmiFreeSlaveInstance);
        if(fmu->canInterpolateInputs) {
            CHECKFUNCTION(fmiSetRealInputDerivatives);
        }
        if(fmu->maxOutputDerivativeOrder > 0) {
            CHECKFUNCTION(fmiGetRealOutputDerivatives);
        }
        CHECKFUNCTION(fmiDoStep);
        if(fmu->canRunAsynchronuously) {
            CHECKFUNCTION(fmiCancelStep);
            CHECKFUNCTION(fmiGetStatus);
            CHECKFUNCTION(fmiGetRealStatus);
            CHECKFUNCTION(fmiGetIntegerStatus);
            CHECKFUNCTION(fmiGetBooleanStatus);
            CHECKFUNCTION(fmiGetStringStatus);
        }
    }

    return true;
}


//! @brief Loads all DLL functions for FMI 2
//! @param fmu FMU handle
//! @returns True if load was successful
bool loadFunctionsFmi2(fmi2Handle *fmu)
{
    TRACEFUNC

    char *dllPath = (char*)malloc(sizeof(char)*FILENAME_MAX);
    _getcwd(dllPath, sizeof(char)*FILENAME_MAX);
    chdir(dllPath);
    strcat(dllPath,"\\binaries\\win64\\");
    strcat(dllPath,fmu->modelIdentifier);
    strcat(dllPath,".dll");
#ifdef _WIN32
    HINSTANCE dll = LoadLibraryA(dllPath);
#else
    void* dll = dlopen(dll_file_path, RTLD_NOW|RTLD_LOCAL);
#endif
    if(NULL == dll) {
        printf("Loading DLL failed: %s\n",dllPath);
        return false;
    }
    printf("Load successful!\n",dllPath);

    fmu->dll = dll;

    fmu->fmi2GetTypesPlatform = LOADFUNCTION(fmi2GetTypesPlatform);
    fmu->fmi2GetVersion = LOADFUNCTION(fmi2GetVersion);
    fmu->fmi2SetDebugLogging= LOADFUNCTION(fmi2SetDebugLogging);
    fmu->fmi2Instantiate= LOADFUNCTION(fmi2Instantiate);
    fmu->fmi2FreeInstance= LOADFUNCTION(fmi2FreeInstance);
    fmu->fmi2SetupExperiment = LOADFUNCTION(fmi2SetupExperiment);
    fmu->fmi2EnterInitializationMode= LOADFUNCTION(fmi2EnterInitializationMode);
    fmu->fmi2ExitInitializationMode= LOADFUNCTION(fmi2ExitInitializationMode);
    fmu->fmi2Terminate= LOADFUNCTION(fmi2Terminate);
    fmu->fmi2Reset= LOADFUNCTION(fmi2Reset);
    fmu->fmi2GetReal= LOADFUNCTION(fmi2GetReal);
    fmu->fmi2GetInteger= LOADFUNCTION(fmi2GetInteger);
    fmu->fmi2GetBoolean= LOADFUNCTION(fmi2GetBoolean);
    fmu->fmi2GetString= LOADFUNCTION(fmi2GetString);
    fmu->fmi2SetReal= LOADFUNCTION(fmi2SetReal);
    fmu->fmi2SetInteger= LOADFUNCTION(fmi2SetInteger);
    fmu->fmi2SetBoolean= LOADFUNCTION(fmi2SetBoolean);
    fmu->fmi2SetString= LOADFUNCTION(fmi2SetString);
    fmu->fmi2GetFMUstate= LOADFUNCTION(fmi2GetFMUstate);
    fmu->fmi2SetFMUstate= LOADFUNCTION(fmi2SetFMUstate);
    fmu->fmi2FreeFMUstate= LOADFUNCTION(fmi2FreeFMUstate);
    fmu->fmi2SerializedFMUstateSize= LOADFUNCTION(fmi2SerializedFMUstateSize);
    fmu->fmi2SerializeFMUstate= LOADFUNCTION(fmi2SerializeFMUstate);
    fmu->fmi2DeSerializeFMUstate= LOADFUNCTION(fmi2DeSerializeFMUstate);
    fmu->fmi2GetDirectionalDerivative= LOADFUNCTION(fmi2GetDirectionalDerivative);
    fmu->fmi2EnterEventMode= LOADFUNCTION(fmi2EnterEventMode);
    fmu->fmi2NewDiscreteStates= LOADFUNCTION(fmi2NewDiscreteStates);
    fmu->fmi2EnterContinuousTimeMode= LOADFUNCTION(fmi2EnterContinuousTimeMode);
    fmu->fmi2CompletedIntegratorStep= LOADFUNCTION(fmi2CompletedIntegratorStep);
    fmu->fmi2SetTime= LOADFUNCTION(fmi2SetTime);
    fmu->fmi2SetContinuousStates= LOADFUNCTION(fmi2SetContinuousStates);
    fmu->fmi2GetDerivatives= LOADFUNCTION(fmi2GetDerivatives);
    fmu->fmi2GetEventIndicators= LOADFUNCTION(fmi2GetEventIndicators);
    fmu->fmi2GetContinuousStates= LOADFUNCTION(fmi2GetContinuousStates);
    fmu->fmi2GetNominalsOfContinuousStates= LOADFUNCTION(fmi2GetNominalsOfContinuousStates);
    fmu->fmi2SetRealInputDerivatives= LOADFUNCTION(fmi2SetRealInputDerivatives);
    fmu->fmi2GetRealOutputDerivatives= LOADFUNCTION(fmi2GetRealOutputDerivatives);
    fmu->fmi2DoStep= LOADFUNCTION(fmi2DoStep);
    fmu->fmi2CancelStep= LOADFUNCTION(fmi2CancelStep);
    fmu->fmi2GetStatus= LOADFUNCTION(fmi2GetStatus);
    fmu->fmi2GetRealStatus= LOADFUNCTION(fmi2GetRealStatus);
    fmu->fmi2GetIntegerStatus= LOADFUNCTION(fmi2GetIntegerStatus);
    fmu->fmi2GetBooleanStatus= LOADFUNCTION(fmi2GetBooleanStatus);
    fmu->fmi2GetStringStatus= LOADFUNCTION(fmi2GetStringStatus);

    CHECKFUNCTION(fmi2GetTypesPlatform);
    CHECKFUNCTION(fmi2GetVersion);
    CHECKFUNCTION(fmi2SetDebugLogging);
    CHECKFUNCTION(fmi2Instantiate);
    CHECKFUNCTION(fmi2FreeInstance);
    CHECKFUNCTION(fmi2SetupExperiment);
    CHECKFUNCTION(fmi2EnterInitializationMode);
    CHECKFUNCTION(fmi2ExitInitializationMode);
    CHECKFUNCTION(fmi2Terminate);
    CHECKFUNCTION(fmi2Reset);
    CHECKFUNCTION(fmi2GetReal);
    CHECKFUNCTION(fmi2GetInteger);
    CHECKFUNCTION(fmi2GetBoolean);
    CHECKFUNCTION(fmi2GetString);
    CHECKFUNCTION(fmi2SetReal);
    CHECKFUNCTION(fmi2SetInteger);
    CHECKFUNCTION(fmi2SetBoolean);
    CHECKFUNCTION(fmi2SetString);

    if(fmu->supportsModelExchange) {
        CHECKFUNCTION(fmi2EnterEventMode);
        CHECKFUNCTION(fmi2NewDiscreteStates);
        CHECKFUNCTION(fmi2EnterContinuousTimeMode);
        CHECKFUNCTION(fmi2CompletedIntegratorStep);
        CHECKFUNCTION(fmi2SetTime);
        CHECKFUNCTION(fmi2SetContinuousStates);
        CHECKFUNCTION(fmi2GetEventIndicators);
        CHECKFUNCTION(fmi2GetContinuousStates);
        CHECKFUNCTION(fmi2GetDerivatives);
        CHECKFUNCTION(fmi2GetNominalsOfContinuousStates);
    }

    if(fmu->supportsCoSimulation) {
        if(fmu->canInterpolateInputs) {
            CHECKFUNCTION(fmi2SetRealInputDerivatives);
            CHECKFUNCTION(fmi2GetRealOutputDerivatives);
        }
        CHECKFUNCTION(fmi2DoStep);
        CHECKFUNCTION(fmi2CancelStep);
        CHECKFUNCTION(fmi2GetStatus);
        CHECKFUNCTION(fmi2GetRealStatus);
        CHECKFUNCTION(fmi2GetIntegerStatus);
        CHECKFUNCTION(fmi2GetBooleanStatus);
        CHECKFUNCTION(fmi2GetStringStatus);
    }

    if(fmu->canGetAndSetFMUState) {
        CHECKFUNCTION(fmi2GetFMUstate);
        CHECKFUNCTION(fmi2SetFMUstate);
    }

    if(fmu->canSerializeFMUState) {
        CHECKFUNCTION(fmi2SerializedFMUstateSize);
        CHECKFUNCTION(fmi2SerializeFMUstate);
        CHECKFUNCTION(fmi2DeSerializeFMUstate);
        CHECKFUNCTION(fmi2FreeFMUstate);
    }

    if(fmu->providesDirectionalDerivative) {
        CHECKFUNCTION(fmi2GetDirectionalDerivative);
    }

    return true;
}


//! @brief Loads all DLL functions for FMI 3
//! @param fmu FMU handle
//! @returns True if load was successful
bool loadFunctionsFmi3(fmi3Handle *fmu)
{
    TRACEFUNC
    char *dllPath = (char*)malloc(sizeof(char)*FILENAME_MAX);
    _getcwd(dllPath, sizeof(char)*FILENAME_MAX);
    chdir(dllPath);
    strcat(dllPath,"\\binaries\\x86_64-windows\\");
    strcat(dllPath,fmu->modelIdentifier);
    strcat(dllPath,".dll");
#ifdef _WIN32
    HINSTANCE dll = LoadLibraryA(dllPath);
#else
    void* dll = dlopen(dll_file_path, RTLD_NOW|RTLD_LOCAL);
#endif
    if(NULL == dll) {
        printf("Loading DLL failed!\n");
        return false;
    }
    printf("Load successful!\n",dllPath);

    fmu->dll = dll;


    printf("Loading FMI version 3...\n");
    fmu->fmi3GetVersion = LOADFUNCTION(fmi3GetVersion);
    fmu->fmi3SetDebugLogging = LOADFUNCTION(fmi3SetDebugLogging);
    fmu->fmi3InstantiateModelExchange = LOADFUNCTION(fmi3InstantiateModelExchange);
    fmu->fmi3InstantiateCoSimulation = LOADFUNCTION(fmi3InstantiateCoSimulation);
    fmu->fmi3InstantiateScheduledExecution = LOADFUNCTION(fmi3InstantiateScheduledExecution);
    fmu->fmi3FreeInstance = LOADFUNCTION(fmi3FreeInstance);
    fmu->fmi3EnterInitializationMode = LOADFUNCTION(fmi3EnterInitializationMode);
    fmu->fmi3ExitInitializationMode = LOADFUNCTION(fmi3ExitInitializationMode);
    fmu->fmi3Terminate = LOADFUNCTION(fmi3Terminate);
    fmu->fmi3SetFloat64 = LOADFUNCTION(fmi3SetFloat64);
    fmu->fmi3GetFloat64 = LOADFUNCTION(fmi3GetFloat64);
    fmu->fmi3DoStep = LOADFUNCTION(fmi3DoStep);
    fmu->fmi3EnterEventMode = LOADFUNCTION(fmi3EnterEventMode);
    fmu->fmi3Reset = LOADFUNCTION(fmi3Reset);
    fmu->fmi3GetFloat32 = LOADFUNCTION(fmi3GetFloat32);
    fmu->fmi3GetInt8 = LOADFUNCTION(fmi3GetInt8);
    fmu->fmi3GetUInt8 = LOADFUNCTION(fmi3GetUInt8);
    fmu->fmi3GetInt16 = LOADFUNCTION(fmi3GetInt16);
    fmu->fmi3GetUInt16 = LOADFUNCTION(fmi3GetUInt16);
    fmu->fmi3GetInt32 = LOADFUNCTION(fmi3GetInt32);
    fmu->fmi3GetUInt32 = LOADFUNCTION(fmi3GetUInt32);
    fmu->fmi3GetInt64 = LOADFUNCTION(fmi3GetInt64);
    fmu->fmi3GetUInt64 = LOADFUNCTION(fmi3GetUInt64);
    fmu->fmi3GetBoolean = LOADFUNCTION(fmi3GetBoolean);
    fmu->fmi3GetString = LOADFUNCTION(fmi3GetString);
    fmu->fmi3GetBinary = LOADFUNCTION(fmi3GetBinary);
    fmu->fmi3GetClock = LOADFUNCTION(fmi3GetClock);
    fmu->fmi3SetFloat32 = LOADFUNCTION(fmi3SetFloat32);
    fmu->fmi3SetInt8 = LOADFUNCTION(fmi3SetInt8);
    fmu->fmi3SetUInt8 = LOADFUNCTION(fmi3SetUInt8);
    fmu->fmi3SetInt16 = LOADFUNCTION(fmi3SetInt16);
    fmu->fmi3SetUInt16 = LOADFUNCTION(fmi3SetUInt16);
    fmu->fmi3SetInt32 = LOADFUNCTION(fmi3SetInt32);
    fmu->fmi3SetUInt32 = LOADFUNCTION(fmi3SetUInt32);
    fmu->fmi3SetInt64 = LOADFUNCTION(fmi3SetInt64);
    fmu->fmi3SetUInt64 = LOADFUNCTION(fmi3SetUInt64);
    fmu->fmi3SetBoolean = LOADFUNCTION(fmi3SetBoolean);
    fmu->fmi3SetString = LOADFUNCTION(fmi3SetString);
    fmu->fmi3SetBinary = LOADFUNCTION(fmi3SetBinary);
    fmu->fmi3SetClock = LOADFUNCTION(fmi3SetClock);
    fmu->fmi3GetNumberOfVariableDependencies = LOADFUNCTION(fmi3GetNumberOfVariableDependencies);
    fmu->fmi3GetVariableDependencies = LOADFUNCTION(fmi3GetVariableDependencies);
    fmu->fmi3GetFMUState = LOADFUNCTION(fmi3GetFMUState);
    fmu->fmi3SetFMUState = LOADFUNCTION(fmi3SetFMUState);
    fmu->fmi3FreeFMUState = LOADFUNCTION(fmi3FreeFMUState);
    fmu->fmi3SerializedFMUStateSize = LOADFUNCTION(fmi3SerializedFMUStateSize);
    fmu->fmi3SerializeFMUState = LOADFUNCTION(fmi3SerializeFMUState);
    fmu->fmi3DeSerializeFMUState = LOADFUNCTION(fmi3DeSerializeFMUState);
    fmu->fmi3GetDirectionalDerivative = LOADFUNCTION(fmi3GetDirectionalDerivative);
    fmu->fmi3GetAdjointDerivative = LOADFUNCTION(fmi3GetAdjointDerivative);
    fmu->fmi3EnterConfigurationMode = LOADFUNCTION(fmi3EnterConfigurationMode);
    fmu->fmi3ExitConfigurationMode = LOADFUNCTION(fmi3ExitConfigurationMode);
    fmu->fmi3GetIntervalDecimal = LOADFUNCTION(fmi3GetIntervalDecimal);
    fmu->fmi3GetIntervalFraction = LOADFUNCTION(fmi3GetIntervalFraction);
    fmu->fmi3GetShiftDecimal = LOADFUNCTION(fmi3GetShiftDecimal);
    fmu->fmi3GetShiftFraction = LOADFUNCTION(fmi3GetShiftFraction);
    fmu->fmi3SetIntervalDecimal = LOADFUNCTION(fmi3SetIntervalDecimal);
    fmu->fmi3SetIntervalFraction = LOADFUNCTION(fmi3SetIntervalFraction);
    fmu->fmi3EvaluateDiscreteStates = LOADFUNCTION(fmi3EvaluateDiscreteStates);
    fmu->fmi3UpdateDiscreteStates = LOADFUNCTION(fmi3UpdateDiscreteStates);
    fmu->fmi3EnterContinuousTimeMode = LOADFUNCTION(fmi3EnterContinuousTimeMode);
    fmu->fmi3CompletedIntegratorStep = LOADFUNCTION(fmi3CompletedIntegratorStep);
    fmu->fmi3SetTime = LOADFUNCTION(fmi3SetTime);
    fmu->fmi3SetContinuousStates = LOADFUNCTION(fmi3SetContinuousStates);
    fmu->fmi3GetContinuousStateDerivatives = LOADFUNCTION(fmi3GetContinuousStateDerivatives);
    fmu->fmi3GetEventIndicators = LOADFUNCTION(fmi3GetEventIndicators);
    fmu->fmi3GetContinuousStates = LOADFUNCTION(fmi3GetContinuousStates);
    fmu->fmi3GetNominalsOfContinuousStates = LOADFUNCTION(fmi3GetNominalsOfContinuousStates);
    fmu->fmi3GetNumberOfEventIndicators = LOADFUNCTION(fmi3GetNumberOfEventIndicators);
    fmu->fmi3GetNumberOfContinuousStates = LOADFUNCTION(fmi3GetNumberOfContinuousStates);
    fmu->fmi3EnterStepMode = LOADFUNCTION(fmi3EnterStepMode);
    fmu->fmi3GetOutputDerivatives = LOADFUNCTION(fmi3GetOutputDerivatives);
    fmu->fmi3ActivateModelPartition = LOADFUNCTION(fmi3ActivateModelPartition);
    printf("Checking functions...\n");
    CHECKFUNCTION(fmi3GetVersion);
    CHECKFUNCTION(fmi3SetDebugLogging);
    CHECKFUNCTION(fmi3FreeInstance);
    CHECKFUNCTION(fmi3EnterInitializationMode);
    CHECKFUNCTION(fmi3ExitInitializationMode);
    CHECKFUNCTION(fmi3Terminate);
    CHECKFUNCTION(fmi3SetFloat64);
    CHECKFUNCTION(fmi3GetFloat64);
    CHECKFUNCTION(fmi3EnterEventMode);
    CHECKFUNCTION(fmi3Reset);
    CHECKFUNCTION(fmi3GetFloat32);
    CHECKFUNCTION(fmi3GetInt8);
    CHECKFUNCTION(fmi3GetUInt8);
    CHECKFUNCTION(fmi3GetInt16);
    CHECKFUNCTION(fmi3GetUInt16);
    CHECKFUNCTION(fmi3GetInt32);
    CHECKFUNCTION(fmi3GetUInt32);
    CHECKFUNCTION(fmi3GetInt64);
    CHECKFUNCTION(fmi3GetUInt64);
    CHECKFUNCTION(fmi3GetBoolean);
    CHECKFUNCTION(fmi3GetString);
    CHECKFUNCTION(fmi3GetBinary);
    CHECKFUNCTION(fmi3GetClock);
    CHECKFUNCTION(fmi3SetFloat32);
    CHECKFUNCTION(fmi3SetInt8);
    CHECKFUNCTION(fmi3SetUInt8);
    CHECKFUNCTION(fmi3SetInt16);
    CHECKFUNCTION(fmi3SetUInt16);
    CHECKFUNCTION(fmi3SetInt32);
    CHECKFUNCTION(fmi3SetUInt32);
    CHECKFUNCTION(fmi3SetInt64);
    CHECKFUNCTION(fmi3SetUInt64);
    CHECKFUNCTION(fmi3SetBoolean);
    CHECKFUNCTION(fmi3SetString);
    CHECKFUNCTION(fmi3SetBinary);
    CHECKFUNCTION(fmi3SetClock);
    CHECKFUNCTION(fmi3GetNumberOfVariableDependencies);
    CHECKFUNCTION(fmi3GetVariableDependencies);
    CHECKFUNCTION(fmi3EnterConfigurationMode);
    CHECKFUNCTION(fmi3ExitConfigurationMode);
    CHECKFUNCTION(fmi3GetIntervalDecimal);
    CHECKFUNCTION(fmi3GetIntervalFraction);
    CHECKFUNCTION(fmi3GetShiftDecimal);
    CHECKFUNCTION(fmi3GetShiftFraction);
    CHECKFUNCTION(fmi3SetIntervalDecimal);
    CHECKFUNCTION(fmi3SetIntervalFraction);



    CHECKFUNCTION(fmi3GetOutputDerivatives);


    if(fmu->supportsCoSimulation) {
        CHECKFUNCTION(fmi3InstantiateCoSimulation);
        CHECKFUNCTION(fmi3EnterStepMode);
        CHECKFUNCTION(fmi3DoStep);
        CHECKFUNCTION(fmi3EvaluateDiscreteStates);
        CHECKFUNCTION(fmi3UpdateDiscreteStates);
        CHECKFUNCTION(fmi3GetDirectionalDerivative);
        CHECKFUNCTION(fmi3GetAdjointDerivative);
    }

    if(fmu->supportsModelExchange) {
        CHECKFUNCTION(fmi3InstantiateModelExchange);
        CHECKFUNCTION(fmi3SetTime);
        CHECKFUNCTION(fmi3EnterContinuousTimeMode);
        CHECKFUNCTION(fmi3CompletedIntegratorStep);
        CHECKFUNCTION(fmi3SetContinuousStates);
        CHECKFUNCTION(fmi3GetContinuousStateDerivatives);
        CHECKFUNCTION(fmi3GetEventIndicators);
        CHECKFUNCTION(fmi3GetContinuousStates);
        CHECKFUNCTION(fmi3GetNominalsOfContinuousStates);
        CHECKFUNCTION(fmi3GetNumberOfEventIndicators);
        CHECKFUNCTION(fmi3GetNumberOfContinuousStates);
        CHECKFUNCTION(fmi3EvaluateDiscreteStates);
        CHECKFUNCTION(fmi3UpdateDiscreteStates);
        CHECKFUNCTION(fmi3GetDirectionalDerivative);
        CHECKFUNCTION(fmi3GetAdjointDerivative);
    }

    if(fmu->supportsScheduledExecution) {
        CHECKFUNCTION(fmi3InstantiateScheduledExecution);
        CHECKFUNCTION(fmi3ActivateModelPartition);
    }

    if(fmu->canGetAndSetFMUState) {
        CHECKFUNCTION(fmi3GetFMUState);
        CHECKFUNCTION(fmi3SetFMUState);
        CHECKFUNCTION(fmi3FreeFMUState);
    }

    if(fmu->canSerializeFMUState) {
        CHECKFUNCTION(fmi3SerializedFMUStateSize);
        CHECKFUNCTION(fmi3SerializeFMUState);
        CHECKFUNCTION(fmi3DeSerializeFMUState);
    }

    return true;
}


//! @brief Returns FMI version of FMU
//! @param fmu FMU handle
//! @returns Version of the FMU
fmiVersion_t getFmiVersion(fmiHandle *fmu)
{
    char cwd[FILENAME_MAX];
    _getcwd(cwd, FILENAME_MAX);

    chdir(fmu->unzippedLocation);


    xmlKeepBlanksDefault(0);

    xmlDoc *doc = NULL;
    xmlNode *rootElement = NULL;

    doc = xmlReadFile("modelDescription.xml", NULL, 0);
    chdir(cwd);
    if(NULL == doc){
       printf("Failed to read modelDescription.xml");
       return false;
    }

    rootElement = xmlDocGetRootElement(doc);
    if(!strcmp(rootElement->name, "fmiModelDescription")) {
        printf("Correct root element: %s\n", rootElement->name);
    }
    else {
        printf("Wrong root element: %s\n", rootElement->name);
    }
    printf("Debug 1\n");

    //Figure out FMI version
    xmlAttr *attr = rootElement->properties;
    for(; attr != NULL; attr = attr->next) {
        if(!strcmp(attr->name,"fmiVersion")) {
            xmlChar *version = parseStringAttribute(attr);
            if(version[0] == '1') {
                return fmiVersion1;
            }
            else if(version[0] == '2') {
                return fmiVersion2;
            }
            else if(version[0] == '3') {
                return fmiVersion3;
            }
            else {
                return fmiVersionUnknown;
            }
        }
    }
}


//! @brief Loads the FMU as version 2.
//! First parses modelDescription.xml, then loads all required FMI functions.
//! @param fmu FMU handle
//! @returns Handle to FMU with FMI version 2
fmi2Handle *loadFmu2(fmiHandle *fmu)
{
    fmi2Handle *fmu2 = malloc(sizeof(fmi2Handle));
    fmu2->instanceName = fmu->instanceName;
    fmu2->unzippedLocation = fmu->unzippedLocation;
    fmu2->resourcesLocation = fmu->resourcesLocation;
    fmu2->variables = malloc(100*sizeof(fmi2VariableHandle));
    fmu2->variablesSize = 100;
    fmu2->numberOfVariables = 0;
    if(!parseModelDescriptionFmi2(fmu2)) {
        printf("Failed to parse modelDescription.xml\n");
        return NULL;
    }
    if(!loadFunctionsFmi2(fmu2)) {
        return NULL;
    }

    printf("Returning FMU handle\n");

    return fmu2;
}


//! @brief Loads the FMU as version 3.
//! First parses modelDescription.xml, then loads all required FMI functions.
//! @param fmu FMU handle
//! @returns Handle to FMU with FMI version 3
fmi3Handle *loadFmu3(fmiHandle *fmu)
{
    TRACEFUNC
    fmi3Handle *fmu3 = malloc(sizeof(fmi3Handle));
    fmu3->instanceName = fmu->instanceName;
    fmu3->unzippedLocation = fmu->unzippedLocation;
    fmu3->resourcesLocation = fmu->resourcesLocation;
    fmu3->variables = malloc(100*sizeof(fmi3VariableHandle));
    fmu3->variablesSize = 100;
    fmu3->numberOfVariables = 0;
    if(!parseModelDescriptionFmi3(fmu3)) {
        printf("Failed to parse modelDescription.xml\n");
        return NULL;
    }
    if(!loadFunctionsFmi3(fmu3)) {
        return NULL;
    }

    printf("Returning FMU handle\n");

    return fmu3;
}


bool fmi3InstantiateCoSimulation(fmi3Handle *fmu,
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

    fmu->_fmi3Instance = fmu->fmi3InstantiateCoSimulation(fmu->instanceName,
                                                      fmu->instantiationToken,
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

    return (fmu->_fmi3Instance != NULL);
}

const char* fmi3GetVersion(fmi3Handle *fmu) {

    return fmu->fmi3GetVersion(fmu->_fmi3Instance);
}

fmi3Status fmi3SetDebugLogging(fmi3Handle *fmu,
                                fmi3Boolean loggingOn,
                                size_t nCategories,
                                const fmi3String categories[])
{

    return fmu->fmi3SetDebugLogging(fmu->_fmi3Instance, loggingOn, nCategories, categories);
}

fmi3Status fmi3GetFloat64(fmi3Handle *fmu,
                           const fmi3ValueReference valueReferences[],
                           size_t nValueReferences,
                           fmi3Float64 values[],
                           size_t nValues) {

    return fmu->fmi3GetFloat64(fmu->_fmi3Instance,
                                valueReferences,
                                nValueReferences,
                                values,
                                nValues);
}


fmi3Status fmi3SetFloat64(fmi3Handle *fmu,
                           const fmi3ValueReference valueReferences[],
                           size_t nValueReferences,
                           fmi3Float64 values[],
                           size_t nValues) {

    return fmu->fmi3SetFloat64(fmu->_fmi3Instance,
                                valueReferences,
                                nValueReferences,
                                values,
                                nValues);
}

fmi3Status fmi3EnterInitializationMode(fmi3Handle *fmu,
                                        fmi3Boolean toleranceDefined,
                                        fmi3Float64 tolerance,
                                        fmi3Float64 startTime,
                                        fmi3Boolean stopTimeDefined,
                                        fmi3Float64 stopTime)
{

    return fmu->fmi3EnterInitializationMode(fmu->_fmi3Instance,
                                             toleranceDefined,
                                             tolerance,
                                             startTime,
                                             stopTimeDefined,
                                             stopTime);
}

fmi3Status fmi3ExitInitializationMode(fmi3Handle *fmu)
{
    TRACEFUNC

    return fmu->fmi3ExitInitializationMode(fmu->_fmi3Instance);
}

fmi3Status fmi3Terminate(fmi3Handle *fmu)
{
    TRACEFUNC

    return fmu->fmi3Terminate(fmu->_fmi3Instance);
}

void fmi3FreeInstance(fmi3Handle *fmu)
{
    TRACEFUNC

    fmu->fmi3FreeInstance(fmu->_fmi3Instance);
}

fmi3Status fmi3DoStep(fmi3Handle *fmu,
                       fmi3Float64 currentCommunicationPoint,
                       fmi3Float64 communicationStepSize,
                       fmi3Boolean noSetFMUStatePriorToCurrentPoint,
                       fmi3Boolean *eventEncountered,
                       fmi3Boolean *terminateSimulation,
                       fmi3Boolean *earlyReturn,
                       fmi3Float64 *lastSuccessfulTime)
{

    return fmu->fmi3DoStep(fmu->_fmi3Instance,
                            currentCommunicationPoint,
                            communicationStepSize,
                            noSetFMUStatePriorToCurrentPoint,
                            eventEncountered,
                            terminateSimulation,
                            earlyReturn,
                            lastSuccessfulTime);
}

const char *fmi3ModelName(fmi3Handle *fmu)
{

    return fmu->modelName;
}

const char* fmi3InstantiationToken(fmi3Handle *fmu)
{

    return fmu->instantiationToken;
}

const char* fmi3Description(fmi3Handle *fmu)
{

    return fmu->description;
}

const char* fmi3Author(fmi3Handle *fmu)
{

    return fmu->author;
}

const char* fmi3Version(fmi3Handle *fmu)
{
    return fmu->version;
}

const char* fmi3Copyright(fmi3Handle *fmu)
{

    return fmu->copyright;
}

const char* fmi3License(fmi3Handle *fmu)
{

    return fmu->license;
}

const char* fmi3GenerationTool(fmi3Handle *fmu)
{

    return fmu->generationTool;
}

const char* fmi3GenerationDateAndTime(fmi3Handle *fmu)
{

    return fmu->generationDateAndTime;
}

const char* fmi3VariableNamingConvention(fmi3Handle *fmu)
{

    return fmu->variableNamingConvention;
}

const char *fmi2GetTypesPlatform(fmi2Handle *fmu)
{
    TRACEFUNC

    return fmu->fmi2GetTypesPlatform();
}

const char *fmi2GetVersion(fmi2Handle *fmu)
{
    TRACEFUNC

    return fmu->fmi2GetVersion();
}

fmi2Status fmi2SetDebugLogging(fmi2Handle *fmu, fmi2Boolean loggingOn, size_t nCategories, const fmi2String categories[])
{
    TRACEFUNC

    return fmu->fmi2SetDebugLogging(fmu->_fmi2Component, loggingOn, nCategories, categories);
}

bool fmi2Instantiate(fmi2Handle *fmu, fmi2Type type, fmi2CallbackLogger logger, fmi2CallbackAllocateMemory allocateMemory, fmi2CallbackFreeMemory freeMemory, fmi2StepFinished stepFinished, fmi2ComponentEnvironment componentEnvironment, fmi2Boolean visible, fmi2Boolean loggingOn)
{
    TRACEFUNC
    printf("Entering fmi2Instantiate()\n");
    fmi2Type fmuType;
    if(type == fmi2CoSimulation && !fmu->supportsCoSimulation) {
        printf("FMI for co-simulation is not supported by this FMU.");
        return false;
    }
    else if(type == fmi2ModelExchange && !fmu->supportsModelExchange) {
        printf("FMI for model exchange is not supported by this FMU.");
        return false;
    }

    fmu->callbacks.logger = logger;
    fmu->callbacks.allocateMemory = allocateMemory;
    fmu->callbacks.freeMemory = freeMemory;
    fmu->callbacks.stepFinished = stepFinished;
    fmu->callbacks.componentEnvironment = componentEnvironment;

    printf("Debug kanin\n");
    printf("Calling API function fmi2Instantiate():\n");
    printf("unzippedLocation = %s\n", fmu->unzippedLocation);
    printf("resourcesLocation = %s\n", fmu->resourcesLocation);
    printf("instanceName = %s\n", fmu->instanceName);
    printf("GUID = %s\n", fmu->guid);
    printf("FMU type = %i\n", fmuType);
    printf("Debug katt\n");
    fmu->_fmi2Component = fmu->fmi2Instantiate(fmu->instanceName, type, fmu->guid, fmu->resourcesLocation, &fmu->callbacks, visible, loggingOn);
    printf("Exiting fmi2Instantiate()\n");
    return (fmu->_fmi2Component != NULL);
}

void fmi2FreeInstance(fmi2Handle *fmu)
{
    TRACEFUNC

    fmu->fmi2FreeInstance(fmu->_fmi2Component);
}

fmi2Status fmi2SetupExperiment(fmi2Handle *fmu, fmi2Boolean toleranceDefined, fmi2Real tolerance, fmi2Real startTime, fmi2Boolean stopTimeDefined, fmi2Real stopTime)
{
    TRACEFUNC

    printf("fmi2SetupExperiment()");
    return fmu->fmi2SetupExperiment(fmu->_fmi2Component, toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime);
}

fmi2Status fmi2EnterInitializationMode(fmi2Handle *fmu)
{
    TRACEFUNC

    printf("fmi2EnterInitializationMode()");
    return fmu->fmi2EnterInitializationMode(fmu->_fmi2Component);
}

fmi2Status fmi2ExitInitializationMode(fmi2Handle *fmu)
{
    TRACEFUNC

    printf("fmi2ExitInitializationMode()");
    return fmu->fmi2ExitInitializationMode(fmu->_fmi2Component);
}

fmi2Status fmi2Terminate(fmi2Handle *fmu)
{
    TRACEFUNC

    return fmu->fmi2Terminate(fmu->_fmi2Component);
}

fmi2Status fmi2Reset(fmi2Handle *fmu)
{
    TRACEFUNC

    return fmu->fmi2Reset(fmu->_fmi2Component);
}

int fmi3GetNumberOfVariables(fmi3Handle *fmu)
{
    TRACEFUNC

    return fmu->numberOfVariables;
}

const char *fmi3GetVariableName(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->name;
}


//! @brief Free FMU dll for FMI version 2
//! @param fmu FMU handle
void *freeFmu2(fmi2Handle *fmu)
{
    TRACEFUNC
#ifdef _WIN32
    FreeLibrary(fmu->dll);
#else
    dlclose(fmu->dll);
#endif
}


//! @brief Free FMU dll for FMI version 3
//! @param fmu FMU handle
void *freeFmu3(fmi3Handle *fmu)
{
    TRACEFUNC
#ifdef _WIN32
    FreeLibrary(fmu->dll);
#else
    dlclose(fmu->dll);
#endif
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

char *fmi3GetVariableDescription(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->description;
}

char *fmi3GetVariableQuantity(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->quantity;
}

char *fmi3GetVariableUnit(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->unit;
}

char *fmi3GetVariableDisplayUnit(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->displayUnit;
}

fmi3Float64 fmi3GetVariableStartFloat64(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->startFloat64;
}

fmi3ValueReference fmi3GetVariableValueReference(fmi3VariableHandle *var)
{
    TRACEFUNC
    return var->valueReference;
}

fmi3Status fmi3EnterEventMode(fmi3Handle *fmu, fmi3Boolean stepEvent, fmi3Boolean stateEvent, const fmi3Int32 rootsFound[], size_t nEventIndicators, fmi3Boolean timeEvent)
{

    return fmu->fmi3EnterEventMode(fmu->_fmi3Instance, stepEvent, stateEvent, rootsFound, nEventIndicators, timeEvent);
}

fmi3Status fmi3Reset(fmi3Handle *fmu)
{
    TRACEFUNC

    return fmu->fmi3Reset(fmu->_fmi3Instance);
}

fmi3Status fmi3GetFloat32(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Float32 values[], size_t nValues)
{

    return fmu->fmi3GetFloat32(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetInt8(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int8 values[], size_t nValues)
{

    return fmu->fmi3GetInt8(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetUInt8(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt8 values[], size_t nValues)
{

    return fmu->fmi3GetUInt8(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetInt16(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int16 values[], size_t nValues)
{

    return fmu->fmi3GetInt16(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetUInt16(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt16 values[], size_t nValues)
{

    return fmu->fmi3GetUInt16(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetInt32(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int32 values[], size_t nValues)
{

    return fmu->fmi3GetInt32(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetUInt32(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt32 values[], size_t nValues)
{

    return fmu->fmi3GetUInt32(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetInt64(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int64 values[], size_t nValues)
{

    return fmu->fmi3GetInt64(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetUInt64(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt64 values[], size_t nValues)
{

    return fmu->fmi3GetUInt64(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetBoolean(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Boolean values[], size_t nValues)
{

    return fmu->fmi3GetBoolean(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetString(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3String values[], size_t nValues)
{

    return fmu->fmi3GetString(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetBinary(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, size_t valueSizes[], fmi3Binary values[], size_t nValues)
{

    return fmu->fmi3GetBinary(fmu->_fmi3Instance, valueReferences, nValueReferences, valueSizes, values, nValues);
}

fmi3Status fmi3GetClock(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Clock values[], size_t nValues)
{

    return fmu->fmi3GetClock(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetFloat32(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Float32 values[], size_t nValues)
{

    return fmu->fmi3SetFloat32(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetInt8(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int8 values[], size_t nValues)
{

    return fmu->fmi3SetInt8(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetUInt8(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt8 values[], size_t nValues)
{

    return fmu->fmi3SetUInt8(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetInt16(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int16 values[], size_t nValues)
{

    return fmu->fmi3SetInt16(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetUInt16(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt16 values[], size_t nValues)
{

    return fmu->fmi3SetUInt16(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetInt32(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int32 values[], size_t nValues)
{

    return fmu->fmi3SetInt32(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetUInt32(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt32 values[], size_t nValues)
{

    return fmu->fmi3SetUInt32(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetInt64(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int64 values[], size_t nValues)
{

    return fmu->fmi3SetInt64(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetUInt64(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt64 values[], size_t nValues)
{

    return fmu->fmi3SetUInt64(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetBoolean(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Boolean values[], size_t nValues)
{

    return fmu->fmi3SetBoolean(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetString(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3String values[], size_t nValues)
{

    return fmu->fmi3SetString(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3SetBinary(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const size_t valueSizes[], const fmi3Binary values[], size_t nValues)
{

    return fmu->fmi3SetBinary(fmu->_fmi3Instance, valueReferences, nValueReferences, valueSizes, values, nValues);
}

fmi3Status fmi3SetClock(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Clock values[], size_t nValues)
{

    return fmu->fmi3SetClock(fmu->_fmi3Instance, valueReferences, nValueReferences, values, nValues);
}

fmi3Status fmi3GetNumberOfVariableDependencies(fmi3Handle *fmu, fmi3ValueReference valueReference, size_t *nDependencies)
{

    return fmu->fmi3GetNumberOfVariableDependencies(fmu->_fmi3Instance, valueReference, nDependencies);
}

fmi3Status fmi3GetVariableDependencies(fmi3Handle *fmu, fmi3ValueReference dependent, size_t elementIndicesOfDependent[], fmi3ValueReference independents[], size_t elementIndicesOfIndependents[], fmi3DependencyKind dependencyKinds[], size_t nDependencies)
{

    return fmu->fmi3GetVariableDependencies(fmu->_fmi3Instance, dependent, elementIndicesOfDependent, independents, elementIndicesOfIndependents, dependencyKinds, nDependencies);
}

fmi3Status fmi3GetFMUState(fmi3Handle *fmu, fmi3FMUState *FMUState)
{

    return fmu->fmi3GetFMUState(fmu->_fmi3Instance, FMUState);
}

fmi3Status fmi3SetFMUState(fmi3Handle *fmu, fmi3FMUState FMUState)
{

    return fmu->fmi3SetFMUState(fmu->_fmi3Instance, FMUState);
}

fmi3Status fmi3FreeFMUState(fmi3Handle *fmu, fmi3FMUState *FMUState)
{

    return fmu->fmi3FreeFMUState(fmu->_fmi3Instance, FMUState);
}

fmi3Status fmi3SerializedFMUStateSize(fmi3Handle *fmu, fmi3FMUState FMUState, size_t *size)
{

    return fmu->fmi3SerializedFMUStateSize(fmu->_fmi3Instance, FMUState, size);
}

fmi3Status fmi3SerializeFMUState(fmi3Handle *fmu, fmi3FMUState FMUState, fmi3Byte serializedState[], size_t size)
{

    return fmu->fmi3SerializeFMUState(fmu->_fmi3Instance, FMUState, serializedState, size);
}

fmi3Status fmi3DeSerializeFMUState(fmi3Handle *fmu, const fmi3Byte serializedState[], size_t size, fmi3FMUState *FMUState)
{

    return fmu->fmi3DeSerializeFMUState(fmu->_fmi3Instance, serializedState, size, FMUState);
}

fmi3Status fmi3GetDirectionalDerivative(fmi3Handle *fmu, const fmi3ValueReference unknowns[], size_t nUnknowns, const fmi3ValueReference knowns[], size_t nKnowns, const fmi3Float64 seed[], size_t nSeed, fmi3Float64 sensitivity[], size_t nSensitivity)
{

    return fmu->fmi3GetDirectionalDerivative(fmu->_fmi3Instance, unknowns, nUnknowns, knowns, nKnowns, seed, nSeed, sensitivity, nSensitivity);
}

fmi3Status fmi3GetAdjointDerivative(fmi3Handle *fmu, const fmi3ValueReference unknowns[], size_t nUnknowns, const fmi3ValueReference knowns[], size_t nKnowns, const fmi3Float64 seed[], size_t nSeed, fmi3Float64 sensitivity[], size_t nSensitivity)
{

    return fmu->fmi3GetAdjointDerivative(fmu->_fmi3Instance, unknowns, nUnknowns, knowns, nKnowns, seed, nSeed, sensitivity, nSensitivity);
}

fmi3Status fmi3EnterConfigurationMode(fmi3Handle *fmu)
{

    return fmu->fmi3EnterConfigurationMode(fmu->_fmi3Instance);
}

fmi3Status fmi3ExitConfigurationMode(fmi3Handle *fmu)
{

    return fmu->fmi3ExitConfigurationMode(fmu->_fmi3Instance);
}

fmi3Status fmi3GetIntervalDecimal(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Float64 intervals[], fmi3IntervalQualifier qualifiers[], size_t nIntervals)
{

    return fmu->fmi3GetIntervalDecimal(fmu->_fmi3Instance, valueReferences, nValueReferences, intervals, qualifiers, nIntervals);
}

fmi3Status fmi3GetIntervalFraction(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt64 intervalCounters[], fmi3UInt64 resolutions[], fmi3IntervalQualifier qualifiers[], size_t nIntervals)
{

    return fmu->fmi3GetIntervalFraction(fmu->_fmi3Instance, valueReferences, nValueReferences, intervalCounters, resolutions, qualifiers, nIntervals);
}

fmi3Status fmi3GetShiftDecimal(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Float64 shifts[], size_t nShifts)
{

    return fmu->fmi3GetShiftDecimal(fmu->_fmi3Instance, valueReferences, nValueReferences, shifts, nShifts);
}

fmi3Status fmi3GetShiftFraction(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt64 shiftCounters[], fmi3UInt64 resolutions[], size_t nShifts)
{

    return fmu->fmi3GetShiftFraction(fmu->_fmi3Instance, valueReferences, nValueReferences, shiftCounters, resolutions, nShifts);
}

fmi3Status fmi3SetIntervalDecimal(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Float64 intervals[], size_t nIntervals)
{

    return fmu->fmi3SetIntervalDecimal(fmu->_fmi3Instance, valueReferences, nValueReferences, intervals, nIntervals);
}

fmi3Status fmi3SetIntervalFraction(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt64 intervalCounters[], const fmi3UInt64 resolutions[], size_t nIntervals)
{

    return fmu->fmi3SetIntervalFraction(fmu->_fmi3Instance, valueReferences, nValueReferences, intervalCounters, resolutions, nIntervals);
}

fmi3Status fmi3EvaluateDiscreteStates(fmi3Handle *fmu)
{

    return fmu->fmi3EvaluateDiscreteStates(fmu->_fmi3Instance);
}

fmi3Status fmi3UpdateDiscreteStates(fmi3Handle *fmu, fmi3Boolean *discreteStatesNeedUpdate, fmi3Boolean *terminateSimulation, fmi3Boolean *nominalsOfContinuousStatesChanged, fmi3Boolean *valuesOfContinuousStatesChanged, fmi3Boolean *nextEventTimeDefined, fmi3Float64 *nextEventTime)
{

    return fmu->fmi3UpdateDiscreteStates(fmu->_fmi3Instance, discreteStatesNeedUpdate, terminateSimulation, nominalsOfContinuousStatesChanged, valuesOfContinuousStatesChanged, nextEventTimeDefined, nextEventTime);
}

fmi3Status fmi3EnterContinuousTimeMode(fmi3Handle *fmu)
{

    return fmu->fmi3EnterContinuousTimeMode(fmu->_fmi3Instance);
}

fmi3Status fmi3CompletedIntegratorStep(fmi3Handle *fmu, fmi3Boolean noSetFMUStatePriorToCurrentPoint, fmi3Boolean *enterEventMode, fmi3Boolean *terminateSimulation)
{

    return fmu->fmi3CompletedIntegratorStep(fmu->_fmi3Instance, noSetFMUStatePriorToCurrentPoint, enterEventMode, terminateSimulation);
}

fmi3Status fmi3SetTime(fmi3Handle *fmu, fmi3Float64 time)
{

    return fmu->fmi3SetTime(fmu->_fmi3Instance, time);
}

fmi3Status fmi3SetContinuousStates(fmi3Handle *fmu, const fmi3Float64 continuousStates[], size_t nContinuousStates)
{

    return fmu->fmi3SetContinuousStates(fmu->_fmi3Instance, continuousStates, nContinuousStates);
}

fmi3Status fmi3GetContinuousStateDerivatives(fmi3Handle *fmu, fmi3Float64 derivatives[], size_t nContinuousStates)
{

    return fmu->fmi3GetContinuousStateDerivatives(fmu->_fmi3Instance, derivatives, nContinuousStates);
}

fmi3Status fmi3GetEventIndicators(fmi3Handle *fmu, fmi3Float64 eventIndicators[], size_t nEventIndicators)
{

    return fmu->fmi3GetEventIndicators(fmu->_fmi3Instance, eventIndicators, nEventIndicators);
}

fmi3Status fmi3GetContinuousStates(fmi3Handle *fmu, fmi3Float64 continuousStates[], size_t nContinuousStates)
{

    return fmu->fmi3GetContinuousStates(fmu->_fmi3Instance, continuousStates, nContinuousStates);
}

fmi3Status fmi3GetNominalsOfContinuousStates(fmi3Handle *fmu, fmi3Float64 nominals[], size_t nContinuousStates)
{

    return fmu->fmi3GetNominalsOfContinuousStates(fmu->_fmi3Instance, nominals, nContinuousStates);
}

fmi3Status fmi3GetNumberOfEventIndicators(fmi3Handle *fmu, size_t *nEventIndicators)
{

    return fmu->fmi3GetNumberOfEventIndicators(fmu->_fmi3Instance, nEventIndicators);
}

fmi3Status fmi3GetNumberOfContinuousStates(fmi3Handle *fmu, size_t *nContinuousStates)
{

    return fmu->fmi3GetNumberOfContinuousStates(fmu->_fmi3Instance, nContinuousStates);
}

fmi3Status fmi3EnterStepMode(fmi3Handle *fmu)
{

    return fmu->fmi3EnterStepMode(fmu->_fmi3Instance);
}

fmi3Status fmi3GetOutputDerivatives(fmi3Handle *fmu, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int32 orders[], fmi3Float64 values[], size_t nValues)
{

    return fmu->fmi3GetOutputDerivatives(fmu->_fmi3Instance, valueReferences, nValueReferences, orders, values, nValues);
}

fmi3Status fmi3ActivateModelPartition(fmi3Handle *fmu, fmi3ValueReference clockReference, size_t clockElementIndex, fmi3Float64 activationTime)
{

    return fmu->fmi3ActivateModelPartition(fmu->_fmi3Instance, clockReference, clockElementIndex, activationTime);
}

bool fmi3DefaultStartTimeDefined(fmi3Handle *fmu)
{

    return fmu->defaultStartTimeDefined;
}

bool fmi3DefaultStopTimeDefined(fmi3Handle *fmu)
{

    return fmu->defaultStopTimeDefined;
}

bool fmi3DefaultToleranceDefined(fmi3Handle *fmu)
{

    return fmu->defaultToleranceDefined;
}

bool fmi3DefaultStepSizeDefined(fmi3Handle *fmu)
{

    return fmu->defaultStepSizeDefined;
}

double fmi3GetDefaultStartTime(fmi3Handle *fmu)
{

    return fmu->defaultStartTime;
}

double fmi3GetDefaultStopTime(fmi3Handle *fmu)
{

    return fmu->defaultStopTime;
}

double fmi3GetDefaultTolerance(fmi3Handle *fmu)
{

    return fmu->defaultTolerance;
}

double fmi3GetDefaultStepSize(fmi3Handle *fmu)
{

    return fmu->defaultStepSize;
}

bool fmi2DefaultStartTimeDefined(fmi2Handle *fmu)
{
    TRACEFUNC

    return fmu->defaultStartTimeDefined;
}

bool fmi2DefaultStopTimeDefined(fmi2Handle *fmu)
{
    TRACEFUNC

    return fmu->defaultStopTimeDefined;
}

bool fmi2DefaultToleranceDefined(fmi2Handle *fmu)
{
    TRACEFUNC

    return fmu->defaultToleranceDefined;
}

bool fmi2DefaultStepSizeDefined(fmi2Handle *fmu)
{
    TRACEFUNC

    return fmu->defaultStepSizeDefined;
}

double fmi2GetDefaultStartTime(fmi2Handle *fmu)
{
    TRACEFUNC

    return fmu->defaultStartTime;
}

double fmi2GetDefaultStopTime(fmi2Handle *fmu)
{
    TRACEFUNC

    return fmu->defaultStopTime;
}

double fmi2GetDefaultTolerance(fmi2Handle *fmu)
{
    TRACEFUNC

    return fmu->defaultTolerance;
}

double fmi2GetDefaultStepSize(fmi2Handle *fmu)
{
    TRACEFUNC

    return fmu->defaultStepSize;
}

void *fmi3GetVariableByName(fmi3Handle *fmu, fmi3String name)
{
    for(int i=0; i<fmu->numberOfVariables; ++i) {
        if(fmu->variables[i].name == name) {
            return &fmu->variables[i];
        }
    }
    printf("Variable with name %s not found.\n", name);
    return NULL;
}

void *fmi3GetVariableByIndex(fmi3Handle *fmu, int i)
{

    if(i >= fmu->numberOfVariables) {
        printf("Variable index out of bounds: %i\n",i);
        return NULL;
    }
    return &fmu->variables[i];
}

void *fmi3GetVariableByValueReference(fmi3Handle *fmu, fmi3ValueReference vr)
{

    for(int i=0; i<fmu->numberOfVariables; ++i) {
        if(fmu->variables[i].valueReference == vr) {
            return &fmu->variables[i];
        }
    }
    printf("Variable with value reference %i not found.\n", vr);
    return NULL;
}

int fmi2GetNumberOfVariables(fmi2Handle *fmu)
{
    TRACEFUNC

    return fmu->numberOfVariables;
}

void *fmi2GetVariableByIndex(fmi2Handle *fmu, int i)
{
    TRACEFUNC

    if(i >= fmu->numberOfVariables) {
        printf("Variable index out of bounds: %i\n",i);
        return NULL;
    }
    return &fmu->variables[i];
}

void *fmi2GetVariableByValueReference(fmi2Handle *fmu, fmi3ValueReference vr)
{
    TRACEFUNC

    for(int i=0; i<fmu->numberOfVariables; ++i) {
        if(fmu->variables[i].valueReference == vr) {
            return &fmu->variables[i];
        }
    }
    printf("Variable with value reference %i not found.\n", vr);
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

fmi2Status fmi2GetReal(fmi2Handle *fmu,
                       const fmi2ValueReference valueReferences[],
                       size_t nValueReferences,
                       fmi2Real values[])
{
    return fmu->fmi2GetReal(fmu->_fmi2Component,
                               valueReferences,
                               nValueReferences,
                               values);
}

fmi2Status fmi2GetInteger(fmi2Handle *fmu,
                          const fmi2ValueReference valueReferences[],
                          size_t nValueReferences,
                          fmi2Integer values[])
{
    TRACEFUNC

    return fmu->fmi2GetInteger(fmu->_fmi2Component,
                                  valueReferences,
                                  nValueReferences,
                                  values);
}

fmi2Status fmi2GetBoolean(fmi2Handle *fmu,
                          const fmi2ValueReference valueReferences[],
                          size_t nValueReferences,
                          fmi2Boolean values[])
{
    TRACEFUNC

    return fmu->fmi2GetBoolean(fmu->_fmi2Component,
                                  valueReferences,
                                  nValueReferences,
                                  values);
}

fmi2Status fmi2GetString(fmi2Handle *fmu,
                         const fmi2ValueReference valueReferences[],
                         size_t nValueReferences,
                         fmi2String values[])
{
    TRACEFUNC

    return fmu->fmi2GetString(fmu->_fmi2Component,
                                 valueReferences,
                                 nValueReferences,
                                 values);
}

fmi2Status fmi2SetReal(fmi2Handle *fmu,
                       const fmi2ValueReference valueReferences[],
                       size_t nValueReferences,
                       const fmi2Real values[])
{
    return fmu->fmi2SetReal(fmu->_fmi2Component,
                               valueReferences,
                               nValueReferences,
                               values);
}

fmi2Status fmi2SetInteger(fmi2Handle *fmu,
                          const fmi2ValueReference valueReferences[],
                          size_t nValueReferences,
                          const fmi2Integer values[])
{
    TRACEFUNC

    return fmu->fmi2SetInteger(fmu->_fmi2Component,
                               valueReferences,
                               nValueReferences,
                               values);
}

fmi2Status fmi2SetBoolean(fmi2Handle *fmu,
                          const fmi2ValueReference valueReferences[],
                          size_t nValueReferences,
                          const fmi2Boolean values[])
{
    TRACEFUNC

    return fmu->fmi2SetBoolean(fmu->_fmi2Component,
                               valueReferences,
                               nValueReferences,
                               values);
}

fmi2Status fmi2SetString(fmi2Handle *fmu,
                         const fmi2ValueReference valueReferences[],
                         size_t nValueReferences,
                         const fmi2String values[])
{
    TRACEFUNC

    return fmu->fmi2SetString(fmu->_fmi2Component,
                               valueReferences,
                               nValueReferences,
                               values);
}

fmi2Status fmi2DoStep(fmi2Handle *fmu, fmi2Real currentCommunicationPoint, fmi2Real communicationStepSize, fmi2Boolean noSetFMUStatePriorToCurrentPoint)
{
    return fmu->fmi2DoStep(fmu->_fmi2Component,
                              currentCommunicationPoint,
                              communicationStepSize,
                              noSetFMUStatePriorToCurrentPoint);
}

const char *fmi2GetGuid(fmi2Handle *fmu)
{
    TRACEFUNC

    return fmu->guid;
}

const char *fmi2GetModelIdentifier(fmi2Handle *fmu)
{
    TRACEFUNC
    return fmu->modelIdentifier;
}

bool fmi2GetNeedsExecutionTool(fmi2Handle *fmu)
{
    TRACEFUNC
    return fmu->needsExecutionTool;
}

bool fmi2GetCanHandleVariableCommunicationStepSize(fmi2Handle *fmu)
{
    TRACEFUNC
    return fmu->canHandleVariableCommunicationStepSize;
}

bool fmi2GetCanInterpolateInputs(fmi2Handle *fmu)
{
    TRACEFUNC
    return fmu->canInterpolateInputs;
}

int fmi2GetMaxOutputDerivativeOrder(fmi2Handle *fmu)
{
    TRACEFUNC
    return fmu->maxOutputDerivativeOrder;
}

bool fmi2GetCanRunAsynchronuously(fmi2Handle *fmu)
{
    TRACEFUNC
    return fmu->canRunAsynchronuously;
}

bool fmi2GetCanBeInstantiatedOnlyOncePerProcess(fmi2Handle *fmu)
{
    TRACEFUNC
    return fmu->canBeInstantiatedOnlyOncePerProcess;
}

bool fmi2GetCanNotUseMemoryManagementFunctions(fmi2Handle *fmu)
{
    TRACEFUNC
    return fmu->canNotUseMemoryManagementFunctions;
}

bool fmi2GetCanGetAndSetFMUState(fmi2Handle *fmu)
{
    TRACEFUNC
    return fmu->canGetAndSetFMUState;
}

bool fmi2GetCanSerializeFMUState(fmi2Handle *fmu)
{
    TRACEFUNC
    return fmu->canSerializeFMUState;
}

bool fmi2GetProvidesDirectionalDerivative(fmi2Handle *fmu)
{
    TRACEFUNC
    return fmu->providesDirectionalDerivative;
}

const char *fmi3GetModelIdentifier(fmi3Handle *fmu)
{
    TRACEFUNC
    return fmu->modelIdentifier;
}

bool fmi3GetCanHandleVariableCommunicationStepSize(fmi3Handle *fmu)
{
    TRACEFUNC
    return fmu->canHandleVariableCommunicationStepSize;
}

bool fmi3GetCanReturnEarlyAfterIntermediateUpdate(fmi3Handle *fmu)
{
    TRACEFUNC
    return fmu->canReturnEarlyAfterIntermediateUpdate;
}

double fmi3GetFixedInternalStepSize(fmi3Handle *fmu)
{
    TRACEFUNC
    return fmu->fixedInternalStepSize;
}

bool fmi3GetHasEventMode(fmi3Handle *fmu)
{
    TRACEFUNC
    return fmu->hasEventMode;
}

bool fmi3GetNeedsExecutionTool(fmi3Handle *fmu)
{
    TRACEFUNC
    return fmu->needsExecutionTool;
}

bool fmi3GetCanBeInstantiatedOnlyOncePerProcess(fmi3Handle *fmu)
{
    TRACEFUNC
    return fmu->canBeInstantiatedOnlyOncePerProcess;
}

bool fmi3GetCanGetAndSetFMUState(fmi3Handle *fmu)
{
    TRACEFUNC
    return fmu->canGetAndSetFMUState;
}

bool fmi3GetCanSerializeFMUState(fmi3Handle *fmu)
{
    TRACEFUNC
    return fmu->canSerializeFMUState;
}

bool fmi3GetProvidesDirectionalDerivative(fmi3Handle *fmu)
{
    TRACEFUNC
    return fmu->providesDirectionalDerivative;
}

bool fmi3GetProvidesAdjointDerivatives(fmi3Handle *fmu)
{
    TRACEFUNC
    return fmu->providesAdjointDerivatives;
}

bool fmi3GetProvidesPerElementDependencies(fmi3Handle *fmu)
{
    TRACEFUNC
    return fmu->providesPerElementDependencies;
}

int fmi3GetMaxOutputDerivativeOrder(fmi3Handle *fmu)
{
    TRACEFUNC
    return fmu->maxOutputDerivativeOrder;
}

bool fmi3GetProvidesIntermediateUpdate(fmi3Handle *fmu)
{
    TRACEFUNC
    return fmu->providesIntermediateUpdate;
}

bool fmi3GetProvidesEvaluateDiscreteStates(fmi3Handle *fmu)
{
    TRACEFUNC
    return fmu->providesEvaluateDiscreteStates;
}

bool fmi3GetRecommendedIntermediateInputSmoothness(fmi3Handle *fmu)
{
    TRACEFUNC
    return fmu->recommendedIntermediateInputSmoothness;
}

bool fmi3GetCompletedIntegratorStepNotNeeded(fmi3Handle *fmu)
{
    TRACEFUNC
    return fmu->completedIntegratorStepNotNeeded;
}


//! @brief Loads the FMU as version 1.
//! First parses modelDescription.xml, then loads all required FMI functions.
//! @param fmu FMU handle
//! @returns Handle to FMU with FMI version 1
fmi1Handle *loadFmu1(fmiHandle *fmu)
{
    fmi1Handle *fmu1 = malloc(sizeof(fmi1Handle));
    fmu1->instanceName = fmu->instanceName;
    fmu1->unzippedLocation = fmu->unzippedLocation;
    fmu1->resourcesLocation = fmu->resourcesLocation;
    fmu1->variables = malloc(100*sizeof(fmi1VariableHandle));
    fmu1->variablesSize = 100;
    fmu1->numberOfVariables = 0;
    if(!parseModelDescriptionFmi1(fmu1)) {
        printf("Failed to parse modelDescription.xml\n");
        return NULL;
    }
    if(!loadFunctionsFmi1(fmu1)) {
        return NULL;
    }

    printf("Returning FMU handle\n");

    return fmu1;
}


//! @brief Free FMU dll for FMI version 1
//! @param fmu FMU handle
void *freeFmu1(fmi1Handle *fmu)
{
    TRACEFUNC
#ifdef _WIN32
    FreeLibrary(fmu->dll);
#else
    dlclose(fmu->dll);
#endif
}

bool fmi1DefaultStartTimeDefined(fmi1Handle *fmu)
{
    TRACEFUNC
    return fmu->defaultStartTimeDefined;
}

bool fmi1DefaultStopTimeDefined(fmi1Handle *fmu)
{
    TRACEFUNC
    return fmu->defaultStopTimeDefined;
}

bool fmi1DefaultToleranceDefined(fmi1Handle *fmu)
{
    TRACEFUNC
    return fmu->defaultToleranceDefined;
}

double fmi1GetDefaultStartTime(fmi1Handle *fmu)
{
    TRACEFUNC
    return fmu->defaultStartTime;
}

double fmi1GetDefaultStopTime(fmi1Handle *fmu)
{
    TRACEFUNC
    return fmu->defaultStopTime;
}

double fmi1GetDefaultTolerance(fmi1Handle *fmu)
{
    TRACEFUNC
    return fmu->defaultTolerance;
}

int fmi1GetNumberOfVariables(fmi1Handle *fmu)
{
    TRACEFUNC
    return fmu->numberOfVariables;
}

fmi1VariableHandle *fmi1GetVariableByIndex(fmi1Handle *fmu, int i)
{
    TRACEFUNC

    if(i >= fmu->numberOfVariables) {
        printf("Variable index out of bounds: %i\n",i);
        return NULL;
    }
    return &fmu->variables[i];
}

fmi1VariableHandle *fmi1GetVariableByValueReference(fmi1Handle *fmu, fmi1ValueReference vr)
{
    TRACEFUNC

    for(int i=0; i<fmu->numberOfVariables; ++i) {
        if(fmu->variables[i].valueReference == vr) {
            return &fmu->variables[i];
        }
    }
    printf("Variable with value reference %i not found.\n", vr);
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

bool fmi1GetVariableIsFixed(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->fixed;
}

const char *fmi1GetTypesPlatform(fmi1Handle *fmu)
{
    TRACEFUNC
    return fmu->fmiGetTypesPlatform();
}

const char *fmi1GetVersion(fmi1Handle *fmu)
{
    TRACEFUNC
    return fmu->fmiGetVersion();
}

fmi1Status fmi1SetDebugLogging(fmi1Handle *fmu, fmi1Boolean loggingOn)
{
    TRACEFUNC
    return fmu->fmiSetDebugLogging(fmu->_fmi1Component, loggingOn);
}

fmi1Status fmi1GetReal(fmi1Handle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1Real values[])
{
    TRACEFUNC
    return fmu->fmiGetReal(fmu->_fmi1Component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1GetInteger(fmi1Handle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1Integer values[])
{
    TRACEFUNC
    return fmu->fmiGetInteger(fmu->_fmi1Component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1GetBoolean(fmi1Handle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1Boolean values[])
{
    TRACEFUNC
    return fmu->fmiGetBoolean(fmu->_fmi1Component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1GetString(fmi1Handle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1String values[])
{
    TRACEFUNC
    return fmu->fmiGetString(fmu->_fmi1Component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1SetReal(fmi1Handle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Real values[])
{
    TRACEFUNC
    return fmu->fmiSetReal(fmu->_fmi1Component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1SetInteger(fmi1Handle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Integer values[])
{
    TRACEFUNC
    return fmu->fmiSetInteger(fmu->_fmi1Component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1SetBoolean(fmi1Handle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Boolean values[])
{
    TRACEFUNC
    return fmu->fmiSetBoolean(fmu->_fmi1Component, valueReferences, nValueReferences, values);
}

fmi1Status fmi1SetString(fmi1Handle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1String values[])
{
    TRACEFUNC
    return fmu->fmiSetString(fmu->_fmi1Component, valueReferences, nValueReferences, values);
}

bool fmi1InstantiateSlave(fmi1Handle *fmu, fmi1String mimeType, fmi1Real timeOut, fmi1Boolean visible, fmi1Boolean interactive, fmi1CallbackLogger_t logger, fmi1CallbackAllocateMemory_t allocateMemory, fmi1CallbackFreeMemory_t freeMemory, fmi1StepFinished_t stepFinished, fmi3Boolean loggingOn)
{
    TRACEFUNC
    fmu->callbacksCoSimulation.logger = logger;
    fmu->callbacksCoSimulation.allocateMemory = allocateMemory;
    fmu->callbacksCoSimulation.freeMemory = freeMemory;
    fmu->callbacksCoSimulation.stepFinished = stepFinished;

    fmu->_fmi1Component = fmu->fmiInstantiateSlave(fmu->instanceName, fmu->guid, fmu->unzippedLocation, mimeType, timeOut, visible, interactive, fmu->callbacksCoSimulation, loggingOn);

    return (fmu->_fmi1Component != NULL);
}

fmi1Status fmi1InitializeSlave(fmi1Handle *fmu, fmi1Real startTime, fmi1Boolean stopTimeDefined, fmi1Real stopTime)
{
    TRACEFUNC
    return fmu->fmiInitializeSlave(fmu->_fmi1Component, startTime, stopTimeDefined, stopTime);
}

fmi1Status fmi1TerminateSlave(fmi1Handle *fmu)
{
    TRACEFUNC
    return fmu->fmiTerminateSlave(fmu->_fmi1Component);
}

fmi1Status fmi1ResetSlave(fmi1Handle *fmu)
{
    TRACEFUNC
    return fmu->fmiResetSlave(fmu->_fmi1Component);
}

void fmi1FreeSlaveInstance(fmi1Handle *fmu)
{
    TRACEFUNC
    return fmu->fmiFreeSlaveInstance(fmu->_fmi1Component);
}

fmi1Status fmi1SetRealInputDerivatives(fmi1Handle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Integer orders[], const fmi1Real values[])
{
    TRACEFUNC
    return fmu->fmiSetRealInputDerivatives(fmu->_fmi1Component, valueReferences, nValueReferences, orders, values);
}

fmi1Status fmi1GetRealOutputDerivatives(fmi1Handle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Integer orders[], fmi1Real values[])
{
    TRACEFUNC
    return fmu->fmiGetRealOutputDerivatives(fmu->_fmi1Component, valueReferences, nValueReferences, orders, values);
}

fmi1Status fmi1CancelStep(fmi1Handle *fmu)
{
    TRACEFUNC
    return fmu->fmiCancelStep(fmu->_fmi1Component);
}

fmi1Status fmi1DoStep(fmi1Handle *fmu, fmi1Real currentCommunicationPoint, fmi1Real communicationStepSize, fmi1Boolean newStep)
{
    TRACEFUNC
    return fmu->fmiDoStep(fmu->_fmi1Component, currentCommunicationPoint, communicationStepSize, newStep);
}

fmi1Status fmi1GetStatus(fmi1Handle *fmu, const fmi1StatusKind statusKind, fmi1Status *value)
{
    TRACEFUNC
    return fmu->fmiGetStatus(fmu->_fmi1Component, statusKind, value);
}

fmi1Status fmi1GetRealStatus(fmi1Handle *fmu, const fmi1StatusKind statusKind, fmi1Real *value)
{
    TRACEFUNC
    return fmu->fmiGetRealStatus(fmu->_fmi1Component, statusKind, value);
}

fmi1Status fmi1GetIntegerStatus(fmi1Handle *fmu, const fmi1StatusKind statusKind, fmi1Integer *value)
{
    TRACEFUNC
    return fmu->fmiGetIntegerStatus(fmu->_fmi1Component, statusKind, value);
}

fmi1Status fmi1GetBooleanStatus(fmi1Handle *fmu, const fmi1StatusKind statusKind, fmi1Boolean *value)
{
    TRACEFUNC
    return fmu->fmiGetBooleanStatus(fmu->_fmi1Component, statusKind, value);
}

fmi1Status fmi1GetStringStatus(fmi1Handle *fmu, const fmi1StatusKind statusKind, fmi1String *value)
{
    TRACEFUNC
    return fmu->fmiGetStringStatus(fmu->_fmi1Component, statusKind, value);
}

const char *fmi1GetModelTypesPlatform(fmi1Handle *fmu)
{
    TRACEFUNC
    return fmu->fmiGetModelTypesPlatform();
}


bool fmi1InstantiateModel(fmi1Handle *fmu, fmi1Type type, fmi1CallbackLogger_t logger, fmi1CallbackAllocateMemory_t allocateMemory, fmi1CallbackFreeMemory_t freeMemory, fmi1Boolean loggingOn)
{
    TRACEFUNC
    fmu->callbacksModelExchange.logger = logger;
    fmu->callbacksModelExchange.allocateMemory = allocateMemory;
    fmu->callbacksModelExchange.freeMemory = freeMemory;
    fmu->_fmi1Component = fmu->fmiInstantiateModel(fmu->instanceName, fmu->guid, fmu->callbacksModelExchange, loggingOn);

    return (fmu->_fmi1Component != NULL);
}

void fmi1FreeModelInstance(fmi1Handle *fmu)
{
    TRACEFUNC
    return fmu->fmiFreeModelInstance(fmu->_fmi1Component);
}

fmi1Status fmi1SetTime(fmi1Handle *fmu, fmi1Real time)
{
    TRACEFUNC
    return fmu->fmiSetTime(fmu->_fmi1Component, time);
}

fmi1Status fmi1SetContinuousStates(fmi1Handle *fmu, const fmi1Real values[], size_t nStates)
{
    TRACEFUNC
    return fmu->fmiSetContinuousStates(fmu->_fmi1Component, values, nStates);
}

fmi1Status fmi1CompletedIntegratorStep(fmi1Handle *fmu, fmi1Boolean *callEventUpdate)
{
    TRACEFUNC
    return fmu->fmiCompletedIntegratorStep(fmu->_fmi1Component, callEventUpdate);
}

fmi1Status fmi1Initialize(fmi1Handle *fmu, fmi1Boolean toleranceControlled, fmi1Real relativeTolerance, fmi1EventInfo *eventInfo)
{
    TRACEFUNC
    return fmu->fmiInitialize(fmu->_fmi1Component, toleranceControlled, relativeTolerance, eventInfo);
}

fmi1Status fmi1GetDerivatives(fmi1Handle *fmu, fmi1Real derivatives[], size_t nDerivatives)
{
    TRACEFUNC
    return fmu->fmiGetDerivatives(fmu->_fmi1Component, derivatives, nDerivatives);
}

fmi1Status fmi1GetEventIndicators(fmi1Handle *fmu, fmi1Real indicators[], size_t nIndicators)
{
    TRACEFUNC
    return fmu->fmiGetEventIndicators(fmu->_fmi1Component, indicators, nIndicators);
}

fmi1Status fmi1EventUpdate(fmi1Handle *fmu, fmi1Boolean intermediateResults, fmi1EventInfo *eventInfo)
{
    TRACEFUNC
    return fmu->fmiEventUpdate(fmu->_fmi1Component, intermediateResults, eventInfo);
}

fmi1Status fmi1GetContinuousStates(fmi1Handle *fmu, fmi1Real states[], size_t nStates)
{
    TRACEFUNC
    return fmu->fmiGetContinuousStates(fmu, states, nStates);
}

fmi1Status fmi1GetNominalContinuousStates(fmi1Handle *fmu, fmi1Real nominals[], size_t nNominals)
{
    TRACEFUNC
    return fmu->fmiGetNominalContinuousStates(fmu->_fmi1Component, nominals, nNominals);
}

fmi1Status fmi1GetStateValueReferences(fmi1Handle *fmu, fmi1ValueReference valueReferences[], size_t nValueReferences)
{
    TRACEFUNC
    return fmu->fmiGetStateValueReferences(fmu, valueReferences, nValueReferences);
}

fmi1Status fmi1Terminate(fmi1Handle *fmu)
{
    TRACEFUNC
    return fmu->fmiTerminate(fmu->_fmi1Component);
}

fmi1DataType fmi1GetVariableDataType(fmi1VariableHandle *var)
{
    TRACEFUNC
    return var->datatype;
}
