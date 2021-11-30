//#include "fmiquick.h"
#include "fmic_private.h"
#include "3rdparty/libxml2/include/libxml/tree.h"
#include "3rdparty/libxml2/include/libxml/parser.h"
#include "3rdparty/minizip/miniunz.h"

#include <sys/stat.h>
#include <stdlib.h>


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

    printf("CWD (2): %s\n",cwd);
    printf("Successfully unzipped FMU\n");
    return fmu;
}


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

    fmu->type = fmi1ModelExchange;

    char cwd[FILENAME_MAX];
    _getcwd(cwd, FILENAME_MAX);
    printf("Current working dir: %s\n", cwd);

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
            fmu->modelName = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "modelIdentifier")) {
            fmu->modelIdentifier = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "guid")) {
            fmu->guid = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "description")) {
            fmu->description = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "author")) {
            fmu->author = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "version")) {
            fmu->version = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "generationtool")) {
            fmu->generationTool = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "generationDateAndTime")) {
            fmu->generationDateAndTime = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "variableNamingConvention")) {
            fmu->variableNamingConvention = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "numberOfContinuousStates")) {
            fmu->numberOfContinuousStates = atoi(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "numberOfEventIndicators")) {
            fmu->numberOfEventIndicators = atoi(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
    }

    xmlNode *node = rootElement->children;
    for(; node != NULL; node = node->next) {

        if(!strcmp(node->name, "CoSimulation_Tool")) {
            fmu->type = fmi1CoSimulationTool;
        }
        else if(!strcmp(node->name, "CoSimulation_StandAlone")) {
            fmu->type = fmi1CoSimulationStandAlone;
        }
        if(!strcmp(node->name, "CoSimulation_StandAlone") || !strcmp(node->name, "CoSimulation_Tool")) {
            xmlNode *cosimNode = node->children;
            for(; cosimNode!= NULL; cosimNode = cosimNode->next) {
                if(!strcmp(node->name, "Capabilities")) {
                    for(xmlAttr *attr = node->properties; attr != NULL; attr = attr->next) {
                        if(!strcmp(attr->name, "canHandleVariableCommunicationStepSize")) {
                            fmu->canHandleVariableCommunicationStepSize = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true");
                        }
                        else if(!strcmp(attr->name, "canHandleEvents")) {
                            fmu->canHandleEvents = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true");
                        }
                        else if(!strcmp(attr->name, "canRejectSteps")) {
                            fmu->canRejectSteps = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true");
                        }
                        else if(!strcmp(attr->name, "canInterpolateInputs")) {
                            fmu->canInterpolateInputs = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true");
                        }
                        else if(!strcmp(attr->name, "maxOutputDerivativeOrder")) {
                            fmu->maxOutputDerivativeOrder = atoi(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                        }
                        else if(!strcmp(attr->name, "canRunAsynchronuously")) {
                            fmu->canRunAsynchronuously = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true");
                        }
                        else if(!strcmp(attr->name, "canSignalEvents")) {
                            fmu->canSignalEvents = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true");
                        }
                        else if(!strcmp(attr->name, "canBeInstantiatedOnlyOncePerProcess")) {
                            fmu->canBeInstantiatedOnlyOncePerProcess = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true");
                        }
                        else if(!strcmp(attr->name, "canNotUseMemoryManagementFunctions")) {
                            fmu->canNotUseMemoryManagementFunctions = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true");
                        }
                    }
                }
            }
        }
        //Parse arguments in <DefaultExperiment>
        if(!strcmp(node->name, "DefaultExperiment")) {
            for(xmlAttr *attr = node->properties; attr != NULL; attr = attr->next) {
                if(!strcmp(attr->name, "startTime")) {
                    fmu->defaultStartTime = atof(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    fmu->defaultStartTimeDefined = true;
                }
                if(!strcmp(attr->name, "stopTime")) {
                    fmu->defaultStartTime = atof(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    fmu->defaultStartTimeDefined = true;
                }
                if(!strcmp(attr->name, "tolerance")) {
                    fmu->defaultTolerance = atof(xmlNodeListGetString(rootElement->doc, attr->children, 1));
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
                        var.name = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    }
                    if(!strcmp(attr->name, "valueReference")) {
                        var.valueReference = atol(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    }
                    if(!strcmp(attr->name, "description")) {
                        var.description = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    }
                    if(!strcmp(attr->name, "causality")) {
                        xmlChar *value = xmlNodeListGetString(rootElement->doc, attr->children, 1);
                        if(!strcmp(value, "input")) {
                            var.causality = fmi1CausalityInput;
                        }
                        else if(!strcmp(value, "output")) {
                            var.causality = fmi1CausalityOutput;
                        }
                        else if(!strcmp(value, "internal")) {
                            var.causality = fmi1CausalityInternal;
                        }
                        else if(!strcmp(value, "none")) {
                            var.causality = fmi1CausalityNone;
                        }
                        else {
                            printf("Unknown causality: %s\n",value);
                            return false;
                        }
                    }
                    if(!strcmp(attr->name, "variability")) {
                        xmlChar *value = xmlNodeListGetString(rootElement->doc, attr->children, 1);
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
                        printf("Found a real variable!\n");

                        //Parse variable attributes
                        for(xmlAttr *attr = varNode->properties; attr != NULL; attr = attr->next) {
                            if(!strcmp(attr->name, "start")) {
                                var.startReal = atof(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                            }
                            if(!strcmp(attr->name, "start")) {
                                var.fixed = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
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
    printf("Current working dir: %s\n", cwd);

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
            fmu->modelName = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "guid")) {
            fmu->guid = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "description")) {
            fmu->description = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "author")) {
            fmu->author = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "version")) {
            fmu->version = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "copyright")) {
            fmu->copyright = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "license")) {
            fmu->license = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "generationtool")) {
            fmu->generationTool = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "generationDateAndTime")) {
            fmu->generationDateAndTime = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "variableNamingConvention")) {
            fmu->variableNamingConvention = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "numberOfEventIndicators")) {
            fmu->numberOfEventIndicators = atoi(xmlNodeListGetString(rootElement->doc, attr->children, 1));
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
                         fmu->modelIdentifier = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                     }
                     else if(!strcmp(attr->name, "needsExecutionTool")) {
                         fmu->needsExecutionTool = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true");
                     }
                     else if(!strcmp(attr->name, "canHandleVariableCommunicationStepSize")) {
                         fmu->canHandleVariableCommunicationStepSize = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true");
                     }
                     else if(!strcmp(attr->name, "canInterpolateInputs")) {
                         fmu->canInterpolateInputs = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true");
                     }
                     else if(!strcmp(attr->name, "maxOutputDerivativeOrder")) {
                         fmu->maxOutputDerivativeOrder = atoi(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                     }
                     else if(!strcmp(attr->name, "canRunAsynchronuously")) {
                         fmu->canRunAsynchronuously = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true");
                     }
                     else if(!strcmp(attr->name, "canBeInstantiatedOnlyOncePerProcess")) {
                         fmu->canBeInstantiatedOnlyOncePerProcess = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true");
                     }
                     else if(!strcmp(attr->name, "canNotUseMemoryManagementFunctions")) {
                         fmu->canNotUseMemoryManagementFunctions = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true");
                     }
                     else if(!strcmp(attr->name, "canGetAndSetFMUState")) {
                         fmu->canGetAndSetFMUState = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true");
                     }
                     else if(!strcmp(attr->name, "canSerializeFMUState")) {
                         fmu->canSerializeFMUState = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true");
                     }
                     else if(!strcmp(attr->name, "providesDirectionalDerivative")) {
                         fmu->providesDirectionalDerivative = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true");
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
                    fmu->defaultStartTime = atof(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    fmu->defaultStartTimeDefined = true;
                }
                if(!strcmp(attr->name, "stopTime")) {
                    fmu->defaultStartTime = atof(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    fmu->defaultStartTimeDefined = true;
                }
                if(!strcmp(attr->name, "tolerance")) {
                    fmu->defaultTolerance = atof(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    fmu->defaultToleranceDefined = true;
                }
                if(!strcmp(attr->name, "stepSize")) {
                    fmu->defaultStepSize = atof(xmlNodeListGetString(rootElement->doc, attr->children, 1));
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
                        var.instanceName = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    }
                    if(!strcmp(attr->name, "valueReference")) {
                        var.valueReference = atol(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    }
                    if(!strcmp(attr->name, "description")) {
                        var.description = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    }
                    if(!strcmp(attr->name, "causality")) {
                        xmlChar *value = xmlNodeListGetString(rootElement->doc, attr->children, 1);
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
                        xmlChar *value = xmlNodeListGetString(rootElement->doc, attr->children, 1);
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
                        xmlChar *value = xmlNodeListGetString(rootElement->doc, attr->children, 1);
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
                        var.canHandleMultipleSetPerTimeInstant = (strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true") == 0);
                    }

                    xmlNode *dataNode = varNode->children;
                    if(dataNode && !strcmp(dataNode->name, "Real")) {
                        printf("Found a real variable!\n");
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
    fmu-> maxOutputDerivativeOrder = 0;
    fmu->providesIntermediateUpdate = false;
    fmu->providesEvaluateDiscreteStates = false;
    fmu->recommendedIntermediateInputSmoothness = false;
    fmu->canHandleVariableCommunicationStepSize = false;
    fmu->canReturnEarlyAfterIntermediateUpdate = false;
    fmu->fixedInternalStepSize = 0;
    fmu->hasEventMode = false;
    fmu->completedIntegratorStepNotNeeded = false;

    char cwd[FILENAME_MAX];
    _getcwd(cwd, FILENAME_MAX);
    printf("Current working dir: %s\n", cwd);

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
            fmu->modelName = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "instantiationToken")) {
            fmu->instantiationToken = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "description")) {
            fmu->description = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "author")) {
            fmu->author = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "version")) {
            fmu->version = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "copyright")) {
            fmu->copyright = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "license")) {
            fmu->license = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "generationtool")) {
            fmu->generationTool = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "generationDateAndTime")) {
            fmu->generationDateAndTime = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
        }
        if(!strcmp(attr->name, "variableNamingConvention")) {
            fmu->variableNamingConvention = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
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
                    fmu->modelIdentifier = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                }
                else if(!strcmp(attr->name, "needsExecutionTool")) {
                    fmu->needsExecutionTool = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "canBeInstantiatedOnlyOncePerProcess")) {
                    fmu->canBeInstantiatedOnlyOncePerProcess = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "canGetAndSetFMUState")) {
                    fmu->canGetAndSetFMUState = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "canSerializeFMUState")) {
                    fmu->canSerializeFMUState = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "providesDirectionalDerivative")) {
                    fmu->providesDirectionalDerivative = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "providesAdjointDerivatives")) {
                    fmu->providesAdjointDerivatives = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "providesPerElementDependencies")) {
                    fmu->providesPerElementDependencies = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "maxOutputDerivativeOrder")) {
                    fmu->maxOutputDerivativeOrder = atoi(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                }
                else if(!strcmp(attr->name, "providesIntermediateUpdate")) {
                    fmu->providesIntermediateUpdate = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "providesEvaluateDiscreteStates")) {
                    fmu->providesEvaluateDiscreteStates = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "recommendedIntermediateInputSmoothness")) {
                    fmu->recommendedIntermediateInputSmoothness = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "canHandleVariableCommunicationStepSize")) {
                    fmu->canHandleVariableCommunicationStepSize = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "canReturnEarlyAfterIntermediateUpdate")) {
                    fmu->canReturnEarlyAfterIntermediateUpdate = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "fixedInternalStepSize")) {
                    fmu->fixedInternalStepSize = atof(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                }
                else if(!strcmp(attr->name, "hasEventMode")) {
                    fmu->hasEventMode = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
            }
        }

        if(!strcmp(node->name, "ModelExchange")) {
            fmu->supportsModelExchange = true;
            for(xmlAttr *attr = node->properties; attr != NULL; attr = attr->next) {
                if(!strcmp(attr->name, "modelIdentifier")) {
                    fmu->modelIdentifier = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                }
                else if(!strcmp(attr->name, "needsExecutionTool")) {
                    fmu->needsExecutionTool = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "canBeInstantiatedOnlyOncePerProcess")) {
                    fmu->canBeInstantiatedOnlyOncePerProcess = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "canGetAndSetFMUState")) {
                    fmu->canGetAndSetFMUState = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "canSerializeFMUState")) {
                    fmu->canSerializeFMUState = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "providesDirectionalDerivative")) {
                    fmu->providesDirectionalDerivative = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "providesAdjointDerivatives")) {
                    fmu->providesAdjointDerivatives = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "providesPerElementDependencies")) {
                    fmu->providesPerElementDependencies = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "maxOutputDerivativeOrder")) {
                    fmu->maxOutputDerivativeOrder = atoi(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                }
                else if(!strcmp(attr->name, "providesIntermediateUpdate")) {
                    fmu->providesIntermediateUpdate = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "providesEvaluateDiscreteStates")) {
                    fmu->providesEvaluateDiscreteStates = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "recommendedIntermediateInputSmoothness")) {
                    fmu->recommendedIntermediateInputSmoothness = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "completedIntegratorStepNotNeeded")) {
                    fmu->completedIntegratorStepNotNeeded = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
            }
        }

        if(!strcmp(node->name, "ScheduledExecution")) {
            fmu->supportsScheduledExecution = true;
            for(xmlAttr *attr = node->properties; attr != NULL; attr = attr->next) {
                if(!strcmp(attr->name, "modelIdentifier")) {
                    fmu->modelIdentifier = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                }
                else if(!strcmp(attr->name, "needsExecutionTool")) {
                    fmu->needsExecutionTool = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "canBeInstantiatedOnlyOncePerProcess")) {
                    fmu->canBeInstantiatedOnlyOncePerProcess = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "canGetAndSetFMUState")) {
                    fmu->canGetAndSetFMUState = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "canSerializeFMUState")) {
                    fmu->canSerializeFMUState = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "providesDirectionalDerivative")) {
                    fmu->providesDirectionalDerivative = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "providesAdjointDerivatives")) {
                    fmu->providesAdjointDerivatives = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "providesPerElementDependencies")) {
                    fmu->providesPerElementDependencies = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "maxOutputDerivativeOrder")) {
                    fmu->maxOutputDerivativeOrder = atoi(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                }
                else if(!strcmp(attr->name, "providesIntermediateUpdate")) {
                    fmu->providesIntermediateUpdate = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "providesEvaluateDiscreteStates")) {
                    fmu->providesEvaluateDiscreteStates = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
                else if(!strcmp(attr->name, "recommendedIntermediateInputSmoothness")) {
                    fmu->recommendedIntermediateInputSmoothness = !strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1), "true");
                }
            }
        }

        //Parse arguments in <DefaultExperiment>
        if(!strcmp(node->name, "DefaultExperiment")) {
            for(xmlAttr *attr = node->properties; attr != NULL; attr = attr->next) {
                if(!strcmp(attr->name, "startTime")) {
                    fmu->defaultStartTime = atof(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    fmu->defaultStartTimeDefined = true;
                }
                if(!strcmp(attr->name, "stopTime")) {
                    fmu->defaultStartTime = atof(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    fmu->defaultStartTimeDefined = true;
                }
                if(!strcmp(attr->name, "tolerance")) {
                    fmu->defaultTolerance = atof(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    fmu->defaultToleranceDefined = true;
                }
                if(!strcmp(attr->name, "stepSize")) {
                    fmu->defaultStepSize = atof(xmlNodeListGetString(rootElement->doc, attr->children, 1));
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
                        var.instanceName = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    }
                    if(!strcmp(attr->name, "description")) {
                        var.description = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    }
                    if(!strcmp(attr->name, "quantity")) {
                        var.quantity = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    }
                    if(!strcmp(attr->name, "unit")) {
                        var.unit = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    }
                    if(!strcmp(attr->name, "displayUnit")) {
                        var.displayUnit = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    }
                    if(!strcmp(attr->name, "start") && !strcmp(varNode->name, "Float64")) {
                        const char* temp = strdup(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                        var.startFloat64 = atof(temp);
                        printf("start(%s): %f\n",var.instanceName, var.startFloat64);
                    }
                    if(!strcmp(attr->name, "valueReference")) {
                        var.valueReference = atol(xmlNodeListGetString(rootElement->doc, attr->children, 1));
                    }
                     if(!strcmp(attr->name, "causality")) {
                        xmlChar *value = xmlNodeListGetString(rootElement->doc, attr->children, 1);
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
                        xmlChar *value = xmlNodeListGetString(rootElement->doc, attr->children, 1);
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
                        var.intermediateUpdate = (strcmp(xmlNodeListGetString(rootElement->doc, attr->children, 1),"true") == 0);
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


//! @brief Loads all DLL functions for FMI 2
bool loadFunctionsFmi1(fmi1Handle *fmu)
{
    TRACEFUNC

    char *dllPath = (char*)malloc(sizeof(char)*FILENAME_MAX);
    _getcwd(dllPath, sizeof(char)*FILENAME_MAX);
    chdir(dllPath);
    strcat(dllPath,"\\binaries\\win64\\");
    strcat(dllPath,fmu->modelIdentifier);
    strcat(dllPath,".dll");
    HINSTANCE dll = LoadLibraryA(dllPath);
    if(NULL == dll) {
        printf("Loading DLL failed: %s\n",dllPath);
        return false;
    }
    printf("Load successful!\n",dllPath);

    fmu->dll = dll;

    fmu->fmi1GetTypesPlatform = LOADFUNCTION(fmi1GetTypesPlatform);
    fmu->fmi1GetVersion = LOADFUNCTION(fmi1GetVersion);
    fmu->fmi1SetDebugLogging = LOADFUNCTION(fmi1SetDebugLogging);
    fmu->fmi1GetReal = LOADFUNCTION(fmi1GetReal);
    fmu->fmi1GetInteger = LOADFUNCTION(fmi1GetInteger);
    fmu->fmi1GetBoolean = LOADFUNCTION(fmi1GetBoolean);
    fmu->fmi1GetString = LOADFUNCTION(fmi1GetString);
    fmu->fmi1SetReal = LOADFUNCTION(fmi1SetReal);
    fmu->fmi1SetInteger = LOADFUNCTION(fmi1SetInteger);
    fmu->fmi1SetBoolean = LOADFUNCTION(fmi1SetBoolean);
    fmu->fmi1SetString = LOADFUNCTION(fmi1SetString);
    fmu->fmi1InstantiateSlave = LOADFUNCTION(fmi1InstantiateSlave);
    fmu->fmi1InitializeSlave = LOADFUNCTION(fmi1InitializeSlave);
    fmu->fmi1TerminateSlave = LOADFUNCTION(fmi1TerminateSlave);
    fmu->fmi1ResetSlave = LOADFUNCTION(fmi1ResetSlave);
    fmu->fmi1FreeSlaveInstance = LOADFUNCTION(fmi1FreeSlaveInstance);
    fmu->fmi1SetRealInputDerivatives = LOADFUNCTION(fmi1SetRealInputDerivatives);
    fmu->fmi1GetRealOutputDerivatives = LOADFUNCTION(fmi1GetRealOutputDerivatives);
    fmu->fmi1CancelStep = LOADFUNCTION(fmi1CancelStep);
    fmu->fmi1DoStep = LOADFUNCTION(fmi1DoStep);
    fmu->fmi1GetStatus = LOADFUNCTION(fmi1GetStatus);
    fmu->fmi1GetRealStatus = LOADFUNCTION(fmi1GetRealStatus);
    fmu->fmi1GetIntegerStatus = LOADFUNCTION(fmi1GetIntegerStatus);
    fmu->fmi1GetBooleanStatus = LOADFUNCTION(fmi1GetBooleanStatus);
    fmu->fmi1GetStringStatus = LOADFUNCTION(fmi1GetStringStatus);
    fmu->fmi1GetModelTypesPlatform = LOADFUNCTION(fmi1GetModelTypesPlatform);
    fmu->fmi1InstantiateModel = LOADFUNCTION(fmi1InstantiateModel);
    fmu->fmi1FreeModelInstance = LOADFUNCTION(fmi1FreeModelInstance);
    fmu->fmi1SetTime = LOADFUNCTION(fmi1SetTime);
    fmu->fmi1SetContinuousStates = LOADFUNCTION(fmi1SetContinuousStates);
    fmu->fmi1CompletedIntegratorStep = LOADFUNCTION(fmi1CompletedIntegratorStep);
    fmu->fmi1Initialize = LOADFUNCTION(fmi1Initialize);
    fmu->fmi1GetDerivatives = LOADFUNCTION(fmi1GetDerivatives);
    fmu->fmi1GetEventIndicators = LOADFUNCTION(fmi1GetEventIndicators);
    fmu->fmi1EventUpdate = LOADFUNCTION(fmi1EventUpdate);
    fmu->fmi1GetContinuousStates = LOADFUNCTION(fmi1GetContinuousStates);
    fmu->fmi1GetNominalContinuousStates = LOADFUNCTION(fmi1GetNominalContinuousStates);
    fmu->fmi1GetStateValueReferences = LOADFUNCTION(fmi1GetStateValueReferences);
    fmu->fmi1Terminate = LOADFUNCTION(fmi1Terminate);

    CHECKFUNCTION(fmi1GetVersion);
    CHECKFUNCTION(fmi1SetDebugLogging);
    CHECKFUNCTION(fmi1GetReal);
    CHECKFUNCTION(fmi1GetInteger);
    CHECKFUNCTION(fmi1GetBoolean);
    CHECKFUNCTION(fmi1GetString);
    CHECKFUNCTION(fmi1SetReal);
    CHECKFUNCTION(fmi1SetInteger);
    CHECKFUNCTION(fmi1SetBoolean);
    CHECKFUNCTION(fmi1SetString);

    if(fmu->type == fmi1ModelExchange) {
        CHECKFUNCTION(fmi1InstantiateModel);
        CHECKFUNCTION(fmi1FreeModelInstance);
        CHECKFUNCTION(fmi1Initialize);
        CHECKFUNCTION(fmi1GetDerivatives);
        CHECKFUNCTION(fmi1Terminate);
        CHECKFUNCTION(fmi1SetTime);
        CHECKFUNCTION(fmi1GetModelTypesPlatform);
        CHECKFUNCTION(fmi1SetContinuousStates);
        CHECKFUNCTION(fmi1CompletedIntegratorStep);
        CHECKFUNCTION(fmi1GetEventIndicators);
        CHECKFUNCTION(fmi1EventUpdate);
        CHECKFUNCTION(fmi1GetContinuousStates);
        CHECKFUNCTION(fmi1GetNominalContinuousStates);
        CHECKFUNCTION(fmi1GetStateValueReferences);
    }

    if(fmu->type == fmi1CoSimulationStandAlone || fmu->type == fmi1CoSimulationTool) {
        CHECKFUNCTION(fmi1GetTypesPlatform);
        CHECKFUNCTION(fmi1InstantiateSlave);
        CHECKFUNCTION(fmi1InitializeSlave);
        CHECKFUNCTION(fmi1TerminateSlave);
        CHECKFUNCTION(fmi1ResetSlave);
        CHECKFUNCTION(fmi1FreeSlaveInstance);
        CHECKFUNCTION(fmi1SetRealInputDerivatives);
        CHECKFUNCTION(fmi1GetRealOutputDerivatives);
        CHECKFUNCTION(fmi1DoStep);
        CHECKFUNCTION(fmi1CancelStep);
        CHECKFUNCTION(fmi1GetStatus);
        CHECKFUNCTION(fmi1GetRealStatus);
        CHECKFUNCTION(fmi1GetIntegerStatus);
        CHECKFUNCTION(fmi1GetBooleanStatus);
        CHECKFUNCTION(fmi1GetStringStatus);
    }

    return true;
}

//! @brief Loads all DLL functions for FMI 2
bool loadFunctionsFmi2(fmi2Handle *fmu)
{
    TRACEFUNC

    char *dllPath = (char*)malloc(sizeof(char)*FILENAME_MAX);
    _getcwd(dllPath, sizeof(char)*FILENAME_MAX);
    chdir(dllPath);
    strcat(dllPath,"\\binaries\\win64\\");
    strcat(dllPath,fmu->modelIdentifier);
    strcat(dllPath,".dll");
    HINSTANCE dll = LoadLibraryA(dllPath);
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
bool loadFunctionsFmi3(fmi3Handle *fmu)
{
    TRACEFUNC
    char *dllPath = (char*)malloc(sizeof(char)*FILENAME_MAX);
    _getcwd(dllPath, sizeof(char)*FILENAME_MAX);
    chdir(dllPath);
    strcat(dllPath,"\\binaries\\x86_64-windows\\");
    strcat(dllPath,fmu->modelIdentifier);
    strcat(dllPath,".dll");
    HINSTANCE dll = LoadLibraryA(dllPath);
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
fmiVersion_t getFmiVersion(fmiHandle *fmu)
{
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
    printf("Debug 1\n");

    //Figure out FMI version
    xmlAttr *attr = rootElement->properties;
    for(; attr != NULL; attr = attr->next) {
        if(!strcmp(attr->name,"fmiVersion")) {
            xmlChar *version = xmlNodeListGetString(rootElement->doc, attr->children, 1);
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


//! @brief Loads the FMU as version 2
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


//! @brief Loads the FMU as version 3
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


void fmi3InstantiateCoSimulation(fmi3Handle *fmu,
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
    return var->instanceName;
}


//! @brief Free FMU dll for FMI version 2
void *freeFmu2(fmi2Handle *fmu)
{
    TRACEFUNC
    FreeLibrary(fmu->dll);
}


//! @brief Free FMU dll for FMI version 3
void *freeFmu3(fmi3Handle *fmu)
{
    TRACEFUNC
    FreeLibrary(fmu->dll);
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
    return var->instanceName;
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
