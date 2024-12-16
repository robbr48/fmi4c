import ctypes as ct

print("")     
print("###################################")
print("## Testing FMI 1 (co-simulation) ##")           
print("###################################")
print("")

verificationDict =	{
    "fmiVersion": "fmiVersion1",
    "fmiType": "fmi1CoSimulationStandAlone",
    "fmiTypeME": "fmi1ModelExchange",
    "modelName": "fmi1cs",
    "modelNameME": "fmi1me",
    "modelIdenetifier": "fmi1cs",
    "numberOfContinuousStates": 1,
    "guid": "123",
    "description": "Integrator (dx = der(x))",
    "author": "Robert Braun",
    "generationTool": "None",
    "generationDateAndTime": "2009-12-08T14:33:22Z",
    "numberOfEventIndicators": 0,
    "defaultStartTimeDefined": False,
    "defaultStopTimeDefined": False,
    "defaultToleranceDefined": False,
    "defaultStartTime": 0.0,
    "defaultStopTime": 0.0,
    "defaultTolerance": 0.0,
    "numberOfVariables": 2,
    "variableNames": ['dx', 'x'],
    "variableValueReferences": [1, 2],
    "variableDescriptions": ['Derivative of x', 'x'],
    "variableQuantities": ['', ''],
    "variableUnits": ['m/s', 'm'],
    "variableDisplayUnits": ['', ''],
    "variableRelativeQuantities": [False, False],
    "variableMinValues": [-1.7976931348623157e+308, -1.7976931348623157e+308],
    "variableMaxValues": [1.7976931348623157e+308, 1.7976931348623157e+308],
    "variableNominals": [1.0, 1.0],
    "variableDataTypes": ["fmi1DataTypeReal", "fmi1DataTypeReal"],
    "numberOfBaseUnits": 2,
    "units": ['rad/s', 'm/s'],
    "numberOfDisplayUnits": [2, 1],
    "displayUnitUnits": [['deg/s', 'r/min'], ['km/h']],
    "numberOfBaseUnits": 2,
    "units": ['rad/s', 'm/s'],
    "numberOfDisplayUnits": [2, 1],
    "displayUnitUnits": [['deg/s', 'r/min'], ['km/h']],
    "variableStartValues": [0.0, 1.0],
    "typesPlatform": "standard32",
    "version": "1.0",
    "instantiateSuccess": True,
    "setDebugLoggingSuccess": 0,
    "initializeSuccess": 0,
    "setRealSuccess": 0,
    "doStepSuccess": 0,
    "getRealResults": [0, [5.0, 0.25]],
    "terminateSuccess": 0,
    "resetSuccess": 0
}

def verify(name, value):
    print(name+": "+str(value)+" -", end=" ")
    if value != verificationDict[name]:
        print("ERROR! Expected: "+str(verificationDict[name]))
        print(f.fmi4c_getErrorMessages())
        exit(1)
    print("ok!")

import fmi4c
f = fmi4c.fmi4c()
import os

#Test FMI1 CS
success = f.fmi4c_loadFmu(os.path.dirname(os.path.abspath(__file__))+"/fmi1cs.fmu", "testfmu")
if not success:
    print("Failed to load fmi1cs.fmu")
    print(f.fmi4c_getErrorMessages())
    exit(1)
else:
    print("Successfully loaded fmi1cs.fmu")

fmiVersion = f.fmi4c_getFmiVersion()
verify("fmiVersion", fmiVersion)
    
fmiType = f.fmi1_getType()
verify("fmiType", fmiType)    

modelName = f.fmi1_getModelName()
verify("modelName", modelName) 

modelIdentifier= f.fmi1_getModelIdentifier()
verify("modelIdenetifier", modelIdentifier)

guid = f.fmi1_getGuid()
verify("guid", guid)

description = f.fmi1_getDescription()
verify("description", description)

author = f.fmi1_getAuthor()
verify("author", author)

generationTool = f.fmi1_getGenerationTool()
verify("generationTool", generationTool)

generationDateAndTime = f.fmi1_getGenerationDateAndTime()
verify("generationDateAndTime", generationDateAndTime)

numberOfContinuousStates = f.fmi1_getNumberOfContinuousStates()
verify("numberOfContinuousStates", numberOfContinuousStates)

numberOfEventIndicators = f.fmi1_getNumberOfEventIndicators()    
verify("numberOfEventIndicators", numberOfEventIndicators)

defaultStartTimeDefined = f.fmi1_defaultStartTimeDefined()
verify("defaultStartTimeDefined", defaultStartTimeDefined)

defaultStopTimeDefined = f.fmi1_defaultStopTimeDefined()    
verify("defaultStopTimeDefined", defaultStopTimeDefined)

defaultToleranceDefined = f.fmi1_defaultToleranceDefined()    
verify("defaultToleranceDefined", defaultToleranceDefined)

defaultStartTime = f.fmi1_getDefaultStartTime()    
verify("defaultStartTime", defaultStartTime)

defaultStopTime = f.fmi1_getDefaultStopTime()    
verify("defaultStopTime", defaultStopTime)

defaultTolerance = f.fmi1_getDefaultTolerance()  
verify("defaultTolerance", defaultTolerance)

numberOfVariables = f.fmi1_getNumberOfVariables()
verify("numberOfVariables", numberOfVariables)

variableNames = []
variableValueReferences = []
variableDescriptions = []
variableQuantities = []
variableUnits = []
variableDisplayUnits = []
variableRelativeQuantities = []
variableMinValues = []
variableMaxValues = []
variableNominals = []
variableDataTypes = []
for i in range(numberOfVariables):
    var = f.fmi1_getVariableByIndex(i+1)
    variableNames.append(f.fmi1_getVariableName(var))
    variableValueReferences.append(f.fmi1_getVariableValueReference(var))
    variableDescriptions.append(f.fmi1_getVariableDescription(var))
    variableQuantities.append(f.fmi1_getVariableQuantity(var))
    variableUnits.append(f.fmi1_getVariableUnit(var))
    variableDisplayUnits.append(f.fmi1_getVariableDisplayUnit(var))
    variableRelativeQuantities.append(f.fmi1_getVariableRelativeQuantity(var))
    variableMinValues.append(f.fmi1_getVariableMin(var))
    variableMaxValues.append(f.fmi1_getVariableMax(var))
    variableNominals.append(f.fmi1_getVariableNominal(var))
    variableDataTypes.append(f.fmi1_getVariableDataType(var))
    
verify("variableNames", variableNames)
verify("variableValueReferences", variableValueReferences)
verify("variableDescriptions", variableDescriptions)
verify("variableQuantities", variableQuantities)
verify("variableUnits", variableUnits)
verify("variableDisplayUnits", variableDisplayUnits)
verify("variableRelativeQuantities", variableRelativeQuantities)
verify("variableMinValues", variableMinValues)
verify("variableMaxValues", variableMaxValues) 
verify("variableNominals", variableNominals)
verify("variableDataTypes", variableDataTypes)

numberOfBaseUnits = f.fmi1_getNumberOfBaseUnits()   
verify("numberOfBaseUnits", numberOfBaseUnits)
 
numberOfDisplayUnits = []
units = []
displayUnitUnits = []
for i in range(numberOfBaseUnits):
    unit = f.fmi1_getBaseUnitByIndex(i)
    units.append(f.fmi1_getBaseUnitUnit(unit))
    numberOfDisplayUnits.append(f.fmi1_getNumberOfDisplayUnits(unit))  
    tempUnits = []
    for j in range(numberOfDisplayUnits[-1]):    
        tempUnits.append(f.fmi1_getDisplayUnitUnitByIndex(unit, j))
    displayUnitUnits.append(tempUnits)
verify("units", units)
verify("numberOfDisplayUnits", numberOfDisplayUnits)
verify("displayUnitUnits", displayUnitUnits)

variableStartValues = []
for i in range(numberOfVariables):
    var = f.fmi1_getVariableByIndex(i+1);
    if f.fmi1_getVariableHasStartValue(var):
        if f.fmi1_getVariableDataType(var) == "fmi1DataTypeReal":
            variableStartValues.append(f.fmi1_getVariableStartReal(var))
        if f.fmi1_getVariableDataType(var) == "fmi1DataTypeInteger":
            variableStartValues.append(f.fmi1_getVariableStartInteger(var))
        if f.fmi1_getVariableDataType(var) == "fmi1DataTypeBoolean":
            variableStartValues.append(f.fmi1_getVariableStartBoolean(var))
        if f.fmi1_getVariableDataType(var) == "fmi1DataTypeString":
            variableStartValues.append(f.fmi1_getVariableStartString(var))
verify("variableStartValues", variableStartValues)

typesPlatform = (f.fmi1_getTypesPlatform())
verify("typesPlatform", typesPlatform)

version = f.fmi1_getVersion()
verify("version", version)

instantiateSuccess = f.fmi1_instantiateSlave(b"application/x-fmu-sharedlibrary", 1.0, False, False, False)
verify("instantiateSuccess", instantiateSuccess)

setDebugLoggingSuccess = f.fmi1_setDebugLogging(True)
verify("setDebugLoggingSuccess", setDebugLoggingSuccess) 

initializeSuccess = f.fmi1_initializeSlave(0, True, 1)
verify("initializeSuccess", initializeSuccess)
    
setRealSuccess = f.fmi1_setReal([1], 1, [5])
verify("setRealSuccess", setRealSuccess)

doStepSuccess = f.fmi1_doStep(0,0.1,True)
verify("doStepSuccess", doStepSuccess)

getRealResults = f.fmi1_getReal([1, 2],2)
verify("getRealResults", getRealResults)
    
terminateSuccess = f.fmi1_terminateSlave()
verify("terminateSuccess", terminateSuccess)

resetSuccess = f.fmi1_resetSlave()
verify("resetSuccess", resetSuccess)

f.fmi1_freeSlaveInstance()

f.fmi4c_freeFmu()

#Test FMI1 ME

print("")
print("####################################")
print("## Testing FMI 1 (model exchange) ##")           
print("####################################")
print("")
  
eventInfo = f.fmi1EventInfo(False, False, False, True, 0.0)

verificationDict =	{
    "fmiVersion": "fmiVersion1",
    "fmiType": "fmi1ModelExchange",
    "modelName": "fmi1me",
    "modelIdenetifier": "fmi1me",
    "numberOfContinuousStates": 1,
    "guid": "123",
    "description": "Integrator (dx = der(x))",
    "author": "Robert Braun",
    "generationTool": "None",
    "generationDateAndTime": "2009-12-08T14:33:22Z",
    "numberOfEventIndicators": 0,
    "defaultStartTimeDefined": False,
    "defaultStopTimeDefined": False,
    "defaultToleranceDefined": False,
    "defaultStartTime": 0.0,
    "defaultStopTime": 0.0,
    "defaultTolerance": 0.0,
    "numberOfVariables": 2,
    "variableNames": ['dx', 'x'],
    "variableValueReferences": [1, 2],
    "variableDescriptions": ['Derivative of x', 'x'],
    "variableQuantities": ['', ''],
    "variableUnits": ['m/s', 'm'],
    "variableDisplayUnits": ['', ''],
    "variableRelativeQuantities": [False, False],
    "variableMinValues": [-1.7976931348623157e+308, -1.7976931348623157e+308],
    "variableMaxValues": [1.7976931348623157e+308, 1.7976931348623157e+308],
    "variableNominals": [1.0, 1.0],
    "variableDataTypes": ["fmi1DataTypeReal", "fmi1DataTypeReal"],
    "numberOfBaseUnits": 2,
    "units": ['rad/s', 'm/s'],
    "numberOfDisplayUnits": [2, 1],
    "displayUnitUnits": [['deg/s', 'r/min'], ['km/h']],
    "numberOfBaseUnits": 2,
    "units": ['rad/s', 'm/s'],
    "numberOfDisplayUnits": [2, 1],
    "displayUnitUnits": [['deg/s', 'r/min'], ['km/h']],
    "variableStartValues": [0.0, 1.0],
    "typesPlatform": "standard32",
    "version": "1.0",
    "instantiateSuccess": True,
    "setDebugLoggingSuccess": 0,
    "initializeSuccess": 0,
    "setRealSuccess": 0,
    "doStepSuccess": 0,
    "getRealResults": [0, [5.0, 0.25]],
    "getDerivativesSuccess": [0, [0.0]],
    "getStateValueReferencesSuccess": [0, [2]],
    "getNominalContinuousStatesSuccess": [0, [1.0]],
    "setContinuousStatesSuccess": 0,
    "getContinuousStatesSuccess": [0, [42.0]],
    "completedIntegratorStepSuccess": 0,
    "terminateSuccess": 0,
    "resetSuccess": 0,
    "setTimeSuccess": 0
}
success = f.fmi4c_loadFmu(os.path.dirname(os.path.abspath(__file__))+"/fmi1me.fmu", "testfmu")
if not success:
    print("Failed to load fmi1me.fmu")
    print(f.fmi4c_getErrorMessages())
    exit(1)
else:
    print("Successfully loaded fmi1me.fmu")

fmiVersion = f.fmi4c_getFmiVersion()
verify("fmiVersion", fmiVersion)
    
fmiType = f.fmi1_getType()
verify("fmiType", fmiType)    

modelName = f.fmi1_getModelName()
verify("modelName", modelName) 

modelIdentifier= f.fmi1_getModelIdentifier()
verify("modelIdenetifier", modelIdentifier)

guid = f.fmi1_getGuid()
verify("guid", guid)

description = f.fmi1_getDescription()
verify("description", description)

author = f.fmi1_getAuthor()
verify("author", author)

generationTool = f.fmi1_getGenerationTool()
verify("generationTool", generationTool)

generationDateAndTime = f.fmi1_getGenerationDateAndTime()
verify("generationDateAndTime", generationDateAndTime)

numberOfContinuousStates = f.fmi1_getNumberOfContinuousStates()
verify("numberOfContinuousStates", numberOfContinuousStates)

numberOfEventIndicators = f.fmi1_getNumberOfEventIndicators()    
verify("numberOfEventIndicators", numberOfEventIndicators)

defaultStartTimeDefined = f.fmi1_defaultStartTimeDefined()
verify("defaultStartTimeDefined", defaultStartTimeDefined)

defaultStopTimeDefined = f.fmi1_defaultStopTimeDefined()    
verify("defaultStopTimeDefined", defaultStopTimeDefined)

defaultToleranceDefined = f.fmi1_defaultToleranceDefined()    
verify("defaultToleranceDefined", defaultToleranceDefined)

defaultStartTime = f.fmi1_getDefaultStartTime()    
verify("defaultStartTime", defaultStartTime)

defaultStopTime = f.fmi1_getDefaultStopTime()    
verify("defaultStopTime", defaultStopTime)

defaultTolerance = f.fmi1_getDefaultTolerance()  
verify("defaultTolerance", defaultTolerance)

numberOfVariables = f.fmi1_getNumberOfVariables()
verify("numberOfVariables", numberOfVariables)

variableNames = []
variableDescriptions = []
variableQuantities = []
variableUnits = []
variableDisplayUnits = []
variableRelativeQuantities = []
variableMinValues = []
variableMaxValues = []
variableNominals = []
variableDataTypes = []
variableValueReferences = []
for i in range(numberOfVariables):
    var = f.fmi1_getVariableByIndex(i+1)
    variableNames.append(f.fmi1_getVariableName(var))
    variableValueReferences.append(f.fmi1_getVariableValueReference(var))
    variableDescriptions.append(f.fmi1_getVariableDescription(var))
    variableQuantities.append(f.fmi1_getVariableQuantity(var))
    variableUnits.append(f.fmi1_getVariableUnit(var))
    variableDisplayUnits.append(f.fmi1_getVariableDisplayUnit(var))
    variableRelativeQuantities.append(f.fmi1_getVariableRelativeQuantity(var))
    variableMinValues.append(f.fmi1_getVariableMin(var))
    variableMaxValues.append(f.fmi1_getVariableMax(var))
    variableNominals.append(f.fmi1_getVariableNominal(var))
    variableDataTypes.append(f.fmi1_getVariableDataType(var))
    
verify("variableNames", variableNames)
verify("variableValueReferences", variableValueReferences)
verify("variableDescriptions", variableDescriptions)
verify("variableQuantities", variableQuantities)
verify("variableUnits", variableUnits)
verify("variableDisplayUnits", variableDisplayUnits)
verify("variableRelativeQuantities", variableRelativeQuantities)
verify("variableMinValues", variableMinValues)
verify("variableMaxValues", variableMaxValues) 
verify("variableNominals", variableNominals)
verify("variableDataTypes", variableDataTypes)

numberOfBaseUnits = f.fmi1_getNumberOfBaseUnits()   
verify("numberOfBaseUnits", numberOfBaseUnits)
 
numberOfDisplayUnits = []
units = []
displayUnitUnits = []
for i in range(numberOfBaseUnits):
    unit = f.fmi1_getBaseUnitByIndex(i)
    units.append(f.fmi1_getBaseUnitUnit(unit))
    numberOfDisplayUnits.append(f.fmi1_getNumberOfDisplayUnits(unit))  
    tempUnits = []
    for j in range(numberOfDisplayUnits[-1]):    
        tempUnits.append(f.fmi1_getDisplayUnitUnitByIndex(unit, j))
    displayUnitUnits.append(tempUnits)
verify("units", units)
verify("numberOfDisplayUnits", numberOfDisplayUnits)
verify("displayUnitUnits", displayUnitUnits)

variableStartValues = []
for i in range(numberOfVariables):
    var = f.fmi1_getVariableByIndex(i+1);
    if f.fmi1_getVariableHasStartValue(var):
        if f.fmi1_getVariableDataType(var) == "fmi1DataTypeReal":
            variableStartValues.append(f.fmi1_getVariableStartReal(var))
        if f.fmi1_getVariableDataType(var) == "fmi1DataTypeInteger":
            variableStartValues.append(f.fmi1_getVariableStartInteger(var))
        if f.fmi1_getVariableDataType(var) == "fmi1DataTypeBoolean":
            variableStartValues.append(f.fmi1_getVariableStartBoolean(var))
        if f.fmi1_getVariableDataType(var) == "fmi1DataTypeString":
            variableStartValues.append(f.fmi1_getVariableStartString(var))
            
verify("variableStartValues", variableStartValues)

typesPlatform = (f.fmi1_getModelTypesPlatform())
verify("typesPlatform", typesPlatform)

version = f.fmi1_getVersion()
verify("version", version)

instantiateSuccess = f.fmi1_instantiateModel(False)
verify("instantiateSuccess", instantiateSuccess)

setDebugLoggingSuccess = f.fmi1_setDebugLogging(False)
verify("setDebugLoggingSuccess", setDebugLoggingSuccess) 

initializeSuccess = f.fmi1_initialize(False, 0, ct.pointer(eventInfo))
verify("initializeSuccess", initializeSuccess)

#

setTimeSuccess = f.fmi1_setTime(0.5)
verify("setTimeSuccess", setTimeSuccess)

getDerivativesSuccess = f.fmi1_getDerivatives(1)
verify("getDerivativesSuccess", getDerivativesSuccess)

getStateValueReferencesSuccess = f.fmi1_getStateValueReferences(1)
verify("getStateValueReferencesSuccess", getStateValueReferencesSuccess)

getNominalContinuousStatesSuccess = f.fmi1_getNominalContinuousStates(1)
verify("getNominalContinuousStatesSuccess", getNominalContinuousStatesSuccess)

setContinuousStatesSuccess = f.fmi1_setContinuousStates([42], 1)
verify("setContinuousStatesSuccess", setContinuousStatesSuccess)

getContinuousStatesSuccess = f.fmi1_getContinuousStates(1)
verify("getContinuousStatesSuccess", getContinuousStatesSuccess)

completedIntegratorStepSuccess = f.fmi1_completedIntegratorStep(False)
verify("completedIntegratorStepSuccess", completedIntegratorStepSuccess)

terminateSuccess = f.fmi1_terminate()
verify("terminateSuccess", terminateSuccess)

f.fmi1_freeModelInstance()

f.fmi4c_freeFmu()
     
#Test FMI2
     
print("")     
print("###################")
print("## Testing FMI 2 ##")           
print("###################")
print("")
        
eventInfo = f.fmi1EventInfo(False, False, False, True, 0.0)

verificationDict =	{
    "fmiVersion": "fmiVersion2",
    "fmiVersion2": "2.0",
    "modelName": "fmi2",
    "modelIdentifierCs": "fmi2",
    "modelIdentifierMe": "fmi2",
    "numberOfContinuousStates": 1,
    "guid": "123",
    "description": "Integrator (dx = der(x))",
    "author": "Robert Braun",
    "copyright": "N/A",
    "license": "N/A",
    "variableNamingConvention": "flat",
    "generationTool": "None",
    "generationDateAndTime": "2009-12-08T14:33:22Z",
    "numberOfEventIndicators": 0,
    "defaultStartTimeDefined": True,
    "defaultStopTimeDefined": True,
    "defaultToleranceDefined": True,
    "defaultStepSizeDefined": True,
    "defaultStartTime": 0.0,
    "defaultStopTime": 3.0,
    "defaultTolerance": 0.0001,
    "defaultStepSize": 0.002,
    "numberOfVariables": 2,
    "variableNames": ['dx', 'x'],
    "variableValueReferences": [1, 2],
    "variableCausalities": ["fmi2CausalityLocal", "fmi2CausalityOutput"],
    "variableVariabilities": ["fmi2VariabilityContinuous", "fmi2VariabilityContinuous"],
    "variableInitials": ["fmi2InitialApprox", "fmi2InitialExact"],
    "variableDescriptions": ['Derivative of x', 'x'],
    "variableQuantities": ['', ''],
    "variableUnits": ['m/s', 'm'],
    "variableDisplayUnits": ['', ''],
    "variableRelativeQuantities": [False, False],
    "variableMinValues": [-1.7976931348623157e+308, -1.7976931348623157e+308],
    "variableMaxValues": [1.7976931348623157e+308, 1.7976931348623157e+308],
    "variableNominals": [1.0, 1.0],
    "variableUnbounded": [False, False],
    "variableDerivativeIndexes": [2, 0],
    "variablesHaveStartValues": [True, True],
    "variableDataTypes": ["fmi2DataTypeReal", "fmi2DataTypeReal"],
    "variablesCanHandleMultipleSetPerTimeInstant": [False, False],
    "numberOfDisplayUnits": [2, 1],
    "displayUnits": [[('mm', 0.001, 0.0), ('km', 1000.0, 0.0)], [('km/h', 3.6, 0.0)]],
    "numberOfUnits": 2,
    "unitNames": ['rad/s', 'm/s'],
    "baseUnits": [(1.0, 0.0, 0, 1, 0, 0, 0, 0, 0, 0), (1.0, 0.0, 0, 1, -1, 0, 0, 0, 0, 0)],
    "displayUnitUnits": [['deg/s', 'r/min'], ['km/h']],
    "variableStartValues": [0.0, 1.0],
    "typesPlatform": "default",
    "version": "0.1",
    "instantiateSuccess": True,
    "setDebugLoggingSuccess": 0,
    "setupExperimentSuccess": 0,
    "enterInitializationModeSuccess": 0,
    "exitInitializationModeSuccess": 0,
    "setRealSuccess": 0,
    "doStepSuccess": 0,
    "getRealResults": [0, [5.0, 0.25]],
    "getDerivativesSuccess": [0, [0.0]],
    "getStateValueReferencesSuccess": [0, [2]],
    "getNominalsOfContinuousStatesSuccess": [0, [1.0]],
    "setContinuousStatesSuccess": 0,
    "getContinuousStatesSuccess": [0, [42.0]],
    "completedIntegratorStepSuccess": (0, False, False),
    "terminateSuccess": 0,
    "resetSuccess": 0,
    "setTimeSuccess": 0,
    "canHandleVariableCommunicationStepSize": True,
    "canInterpolateInputs": False,
    "maxOutputDerivativeOrder": 0,
    "canRunAsynchronously": False,
    "needsExecutionTool": False,
    "canBeInstantiatedOnlyOncePerProcess": False,
    "canNotUseMemoryManagementFunctions": False,
    "canGetAndSetFMUState": False,
    "canSerializeFMUState": False,
    "providesDirectionalDerivative": False,
    "completedIntegratorStepNotNeeded": False,
    "supportsCoSimulation": True,
    "supportsModelExchange": True,
    "numberOfModelStructureOutputs": 1,
    "numberOfModelStructureDerivatives": 1,
    "numberOfModelStructureInitialUnknowns": 1,    
    "modelStructureIndexes": [2, 1, 1],
    "modelStructureNumberOfDependencies": [0, 0, 1],
    "modelStructureDependencies": [[], [], [2]],
    "modelStructureDependencyKindsDefined": [False, False, True],
    "modelStructureDependencyKinds": [[], [], [0]],
}
success = f.fmi4c_loadFmu(os.path.dirname(os.path.abspath(__file__))+"/fmi2.fmu", "testfmu")
if not success:
    print("Failed to load fmi2.fmu")
    print(f.fmi4c_getErrorMessages())
    exit(1)
else:
    print("Successfully loaded fmi2.fmu")

fmiVersion = f.fmi4c_getFmiVersion()
verify("fmiVersion", fmiVersion)

fmiVersion2 = f.fmi2_getFmiVersion()
verify("fmiVersion2", fmiVersion2)

guid = f.fmi2_getGuid()
verify("guid", guid)

author = f.fmi2_getAuthor()
verify("author", author)

generationTool = f.fmi2_getGenerationTool()
verify("generationTool", generationTool)

generationDateAndTime = f.fmi2_getGenerationDateAndTime()
verify("generationDateAndTime", generationDateAndTime)

modelName = f.fmi2_getModelName()
verify("modelName", modelName)

description = f.fmi2_getModelDescription()
verify("description", description)

copyright = f.fmi2_getCopyright()
verify("copyright", copyright)

license = f.fmi2_getLicense()
verify("license", license)

variableNamingConvention = f.fmi2_getVariableNamingConvention()
verify("variableNamingConvention", variableNamingConvention)

defaultStartTimeDefined = f.fmi2_defaultStartTimeDefined()
verify("defaultStartTimeDefined", defaultStartTimeDefined)

defaultStopTimeDefined = f.fmi2_defaultStopTimeDefined()
verify("defaultStopTimeDefined", defaultStopTimeDefined)

defaultToleranceDefined = f.fmi2_defaultToleranceDefined()
verify("defaultToleranceDefined", defaultToleranceDefined)

defaultStepSizeDefined = f.fmi2_defaultStepSizeDefined()
verify("defaultStepSizeDefined", defaultStepSizeDefined)

defaultStartTime = f.fmi2_getDefaultStartTime()
verify("defaultStartTime", defaultStartTime)

defaultStopTime = f.fmi2_getDefaultStopTime()
verify("defaultStopTime", defaultStopTime)

defaultTolerance = f.fmi2_getDefaultTolerance()
verify("defaultTolerance", defaultTolerance)

defaultStepSize = f.fmi2_getDefaultStepSize()
verify("defaultStepSize", defaultStepSize)

numberOfVariables = f.fmi2_getNumberOfVariables()
verify("numberOfVariables", numberOfVariables)

variableNames = []
variableValueReferences = []
variableCausalities = []
variableVariabilities = []
variableInitials = []
variablesCanHandleMultipleSetPerTimeInstant = []
variableDescriptions = []
variableQuantities = []
variableUnits = []
variableDisplayUnits = []
variableRelativeQuantities = []
variableMinValues = []
variableMaxValues = []
variableNominals = []
variableUnbounded = []
variableDerivativeIndexes = []
variablesHaveStartValues = []
variableDataTypes = []
for i in range(numberOfVariables):
    var = f.fmi2_getVariableByIndex(i+1)
    variableNames.append(f.fmi2_getVariableName(var))
    variableCausalities.append(f.fmi2_getVariableCausality(var))
    variableVariabilities.append(f.fmi2_getVariableVariability(var))
    variableInitials.append(f.fmi2_getVariableInitial(var))
    variableValueReferences.append(f.fmi2_getVariableValueReference(var))
    variableDescriptions.append(f.fmi2_getVariableDescription(var))
    variableQuantities.append(f.fmi2_getVariableQuantity(var))
    variableUnits.append(f.fmi2_getVariableUnit(var))
    variableDisplayUnits.append(f.fmi2_getVariableDisplayUnit(var))
    variableRelativeQuantities.append(f.fmi2_getVariableRelativeQuantity(var))
    variableMinValues.append(f.fmi2_getVariableMin(var))
    variableMaxValues.append(f.fmi2_getVariableMax(var))
    variableNominals.append(f.fmi2_getVariableNominal(var))
    variableUnbounded.append(f.fmi2_getVariableUnbounded(var))
    variableDerivativeIndexes.append(f.fmi2_getVariableDerivativeIndex(var))
    variablesHaveStartValues.append(f.fmi2_getVariableHasStartValue(var))
    variableDataTypes.append(f.fmi2_getVariableDataType(var))
    variablesCanHandleMultipleSetPerTimeInstant.append(f.fmi2_getVariableCanHandleMultipleSetPerTimeInstant(var))
        
verify("variableNames", variableNames)
verify("variableValueReferences", variableValueReferences)
verify("variableCausalities", variableCausalities)
verify("variableVariabilities", variableVariabilities)
verify("variableInitials", variableInitials)
verify("variableDescriptions", variableDescriptions)
verify("variableQuantities", variableQuantities)
verify("variableUnits", variableUnits)
verify("variableDisplayUnits", variableDisplayUnits)
verify("variableRelativeQuantities", variableRelativeQuantities)
verify("variableMinValues", variableMinValues)
verify("variableMaxValues", variableMaxValues)
verify("variableNominals", variableNominals)
verify("variableUnbounded", variableUnbounded)
verify("variableDerivativeIndexes", variableDerivativeIndexes)
verify("variablesHaveStartValues", variablesHaveStartValues)
verify("variableDataTypes", variableDataTypes)
verify("variablesCanHandleMultipleSetPerTimeInstant", variablesCanHandleMultipleSetPerTimeInstant)

variableStartValues = []
for i in range(numberOfVariables):
    var = f.fmi2_getVariableByIndex(i+1)
    if f.fmi2_getVariableHasStartValue(var):
        if f.fmi2_getVariableDataType(var) == "fmi2DataTypeReal":
            variableStartValues.append(f.fmi2_getVariableStartReal(var))
        if f.fmi2_getVariableDataType(var) == "fmi2DataTypeInteger":
            variableStartValues.append(f.fmi2_getVariableStartInteger(var))
        if f.fmi2_getVariableDataType(var) == "fmi2DataTypeBoolean":
            variableStartValues.append(f.fmi2_getVariableStartBoolean(var))
        if f.fmi2_getVariableDataType(var) == "fmi2DataTypeString":
            variableStartValues.append(f.fmi2_getVariableStartString(var))
verify("variableStartValues", variableStartValues)

numberOfUnits = f.fmi2_getNumberOfUnits()   
verify("numberOfUnits", numberOfUnits)
 
unitNames = []
baseUnits = []
numberOfDisplayUnits = []
displayUnits = []
for i in range(numberOfUnits):
    unit = f.fmi2_getUnitByIndex(i)
    unitNames.append(f.fmi2_getUnitName(unit))
    if(f.fmi2_hasBaseUnit(unit)):
        baseUnits.append(f.fmi2_getBaseUnit(unit))
    numberOfDisplayUnits.append(f.fmi2_getNumberOfDisplayUnits(unit))  
    tempUnits = []
    for j in range(numberOfDisplayUnits[-1]):    
        tempUnits.append(f.fmi2_getDisplayUnitByIndex(unit, j))
    displayUnits.append(tempUnits)
verify("unitNames", units)
verify("baseUnits", baseUnits)
verify("numberOfDisplayUnits", numberOfDisplayUnits)
verify("displayUnits", displayUnits)

verify("modelIdentifierCs", f.fmi2cs_getModelIdentifier())

verify("canHandleVariableCommunicationStepSize", f.fmi2cs_getCanHandleVariableCommunicationStepSize())
verify("canInterpolateInputs", f.fmi2cs_getCanInterpolateInputs())
verify("maxOutputDerivativeOrder", f.fmi2cs_getMaxOutputDerivativeOrder())
verify("canRunAsynchronously", f.fmi2cs_getCanRunAsynchronuously())
verify("needsExecutionTool", f.fmi2cs_getNeedsExecutionTool())
verify("canBeInstantiatedOnlyOncePerProcess", f.fmi2cs_getCanBeInstantiatedOnlyOncePerProcess())
verify("canNotUseMemoryManagementFunctions", f.fmi2cs_getCanNotUseMemoryManagementFunctions())
verify("canGetAndSetFMUState", f.fmi2cs_getCanGetAndSetFMUState())
verify("canSerializeFMUState", f.fmi2cs_getCanSerializeFMUState())
verify("providesDirectionalDerivative", f.fmi2cs_getProvidesDirectionalDerivative())



verify("modelIdentifierMe", f.fmi2me_getModelIdentifier())

verify("completedIntegratorStepNotNeeded", f.fmi2me_getCompletedIntegratorStepNotNeeded())
verify("needsExecutionTool", f.fmi2me_getNeedsExecutionTool())
verify("canBeInstantiatedOnlyOncePerProcess", f.fmi2me_getCanBeInstantiatedOnlyOncePerProcess())
verify("canNotUseMemoryManagementFunctions", f.fmi2me_getCanNotUseMemoryManagementFunctions()) 
verify("canGetAndSetFMUState", f.fmi2me_getCanGetAndSetFMUState())
verify("canSerializeFMUState", f.fmi2me_getCanSerializeFMUState())
verify("providesDirectionalDerivative", f.fmi2me_getProvidesDirectionalDerivative())

verify("numberOfContinuousStates", f.fmi2_getNumberOfContinuousStates())
verify("numberOfEventIndicators", f.fmi2_getNumberOfEventIndicators())
verify("supportsCoSimulation", f.fmi2_getSupportsCoSimulation())
verify("supportsModelExchange", f.fmi2_getSupportsModelExchange())

numberOfModelStructureOutputs = f.fmi2_getNumberOfModelStructureOutputs()
numberOfModelStructureDerivatives = f.fmi2_getNumberOfModelStructureDerivatives()
numberOfModelStructureInitialUnknowns = f.fmi2_getNumberOfModelStructureInitialUnknowns()


modelStructureIndexes = []
modelStructureNumberOfDependencies = []
modelStructureDependencies = []
modelStructureDependencyKindsDefined = []
modelStructureDependencyKinds = []
for i in range(numberOfModelStructureOutputs):
    unknown = f.fmi2_getModelStructureOutput(i)
    modelStructureIndexes.append(f.fmi2_getModelStructureIndex(unknown))
    nDependencies = f.fmi2_getModelStructureNumberOfDependencies(unknown)
    modelStructureNumberOfDependencies.append(nDependencies)
    modelStructureDependencies.append(f.fmi2_getModelStructureDependencies(unknown, nDependencies))
    dependencyKindsDefined = f.fmi2_getModelStructureDependencyKindsDefined(unknown)
    modelStructureDependencyKindsDefined.append(dependencyKindsDefined)
    if dependencyKindsDefined:
        modelStructureDependencyKinds.append(f.fmi2_getModelStructureDependencyKinds(unknown, nDependencies))
    else:
        modelStructureDependencyKinds.append([])
    
for i in range(numberOfModelStructureDerivatives):
    unknown = f.fmi2_getModelStructureDerivative(i)
    modelStructureIndexes.append(f.fmi2_getModelStructureIndex(unknown))
    nDependencies = f.fmi2_getModelStructureNumberOfDependencies(unknown)
    modelStructureNumberOfDependencies.append(nDependencies)
    modelStructureDependencies.append(f.fmi2_getModelStructureDependencies(unknown, nDependencies))
    dependencyKindsDefined = f.fmi2_getModelStructureDependencyKindsDefined(unknown)
    modelStructureDependencyKindsDefined.append(dependencyKindsDefined)
    if dependencyKindsDefined:
        modelStructureDependencyKinds.append(f.fmi2_getModelStructureDependencyKinds(unknown, nDependencies))
    else:
        modelStructureDependencyKinds.append([])

for i in range(numberOfModelStructureInitialUnknowns):
    unknown = f.fmi2_getModelStructureInitialUnknown(i)
    modelStructureIndexes.append(f.fmi2_getModelStructureIndex(unknown))
    nDependencies = f.fmi2_getModelStructureNumberOfDependencies(unknown)
    modelStructureNumberOfDependencies.append(nDependencies)
    modelStructureDependencies.append(f.fmi2_getModelStructureDependencies(unknown, nDependencies))
    dependencyKindsDefined = f.fmi2_getModelStructureDependencyKindsDefined(unknown)
    modelStructureDependencyKindsDefined.append(dependencyKindsDefined)
    if dependencyKindsDefined:
        modelStructureDependencyKinds.append(f.fmi2_getModelStructureDependencyKinds(unknown, nDependencies))
    else:
        modelStructureDependencyKinds.append([])
    
verify("numberOfModelStructureOutputs", numberOfModelStructureOutputs)
verify("numberOfModelStructureDerivatives", numberOfModelStructureDerivatives)
verify("numberOfModelStructureInitialUnknowns", numberOfModelStructureInitialUnknowns)
verify("modelStructureIndexes", modelStructureIndexes)
verify("modelStructureNumberOfDependencies", modelStructureNumberOfDependencies)
verify("modelStructureDependencies", modelStructureDependencies)
verify("modelStructureDependencyKindsDefined", modelStructureDependencyKindsDefined)
verify("modelStructureDependencyKinds", modelStructureDependencyKinds)
   
instantiateSuccess = f.fmi2_instantiate(1, False, False)    # 1 = co-simulation
verify("instantiateSuccess", instantiateSuccess)

typesPlatform = f.fmi2_getTypesPlatform()
verify("typesPlatform", typesPlatform)

version = f.fmi2_getVersion()
verify("version", version)

setDebugLoggingSuccess = f.fmi2_setDebugLogging(False, 0, None)
verify("setDebugLoggingSuccess", setDebugLoggingSuccess) 

verify("setupExperimentSuccess", f.fmi2_setupExperiment(True, 1e-5, 0, True, 2))

verify("enterInitializationModeSuccess", f.fmi2_enterInitializationMode())
verify("exitInitializationModeSuccess", f.fmi2_exitInitializationMode())

verify("setRealSuccess", f.fmi2_setReal([1], 1, [5]))

verify("doStepSuccess", f.fmi2_doStep(0,0.1,True))

verify("getRealResults", f.fmi2_getReal([1, 2],2))

verify("resetSuccess", f.fmi2_reset())

instantiateSuccess = f.fmi2_instantiate(0, False, False)    #1 = model exchange
verify("instantiateSuccess", instantiateSuccess)

verify("enterInitializationModeSuccess", f.fmi2_enterInitializationMode())
verify("exitInitializationModeSuccess", f.fmi2_exitInitializationMode())

setTimeSuccess = f.fmi2_setTime(0.5)
verify("setTimeSuccess", setTimeSuccess)

getDerivativesSuccess = f.fmi2_getDerivatives(1)
verify("getDerivativesSuccess", getDerivativesSuccess)

#getStateValueReferencesSuccess = f.fmi2_getStateValueReferences(1)
#verify("getStateValueReferencesSuccess", getStateValueReferencesSuccess)

getNominalsOfContinuousStatesSuccess = f.fmi2_getNominalsOfContinuousStates(1)
verify("getNominalsOfContinuousStatesSuccess", getNominalsOfContinuousStatesSuccess)

setContinuousStatesSuccess = f.fmi2_setContinuousStates([42], 1)
verify("setContinuousStatesSuccess", setContinuousStatesSuccess)

getContinuousStatesSuccess = f.fmi2_getContinuousStates(1)
verify("getContinuousStatesSuccess", getContinuousStatesSuccess)

completedIntegratorStepSuccess = f.fmi2_completedIntegratorStep(False)
verify("completedIntegratorStepSuccess", completedIntegratorStepSuccess)

terminateSuccess = f.fmi2_terminate()
verify("terminateSuccess", terminateSuccess)

f.fmi2_freeInstance()

f.fmi4c_freeFmu()

#Untested FMI2 functions:
#fmi2_getVariableByValueReference
#fmi2_getVariableByName
#fmi2_getVariableStartReal
#fmi2_getVariableStartInteger
#fmi2_getVariableStartBoolean
#fmi2_getVariableStartString
#fmi2_getInteger
#fmi2_getBoolean
#fmi2_getString
#fmi2_setInteger
#fmi2_setBoolean
#fmi2_setString
#fmi2_getFMUstate
#fmi2_setFMUstate
#fmi2_freeFMUstate
#fmi2_serializedFMUstateSize
#fmi2_serializeFMUstate
#fmi2_deSerializeFMUstate
#fmi2_getDirectionalDerivative
#fmi2_enterEventMode
#fmi2_newDiscreteStates
#fmi2_enterContinuousTimeMode
#fmi2_getEventIndicators
#fmi2_setRealInputDerivatives
#fmi2_getRealOutputDerivatives
#fmi2_cancelStep
#fmi2_getStatus
#fmi2_getRealStatus
#fmi2_getIntegerStatus
#fmi2_getBooleanStatus
#fmi2_getStringStatus

    
print("")     
print("###################")
print("## Testing FMI 3 ##")           
print("###################")
print("")
        
#eventInfo = f.fmi3EventInfo(False, False, False, True, 0.0)

verificationDict =	{
    "fmiVersion": "fmiVersion3",
    "fmiVersionNumber": "3.0",
    "modelName": "fmi3",
    "instantiationToken": "123",
    "description": "Integrator (dx = der(x))",
    "author": "Robert Braun",
    "copyright": "N/A",
    "license": "N/A",
    "variableNamingConvention": "flat",
    "generationTool": "None",
    "generationDateAndTime": "2009-12-08T14:33:22Z",
    "defaultStartTimeDefined": True,
    "defaultStopTimeDefined": True,
    "defaultToleranceDefined": True,
    "defaultStepSizeDefined": True,
    "defaultStartTime": 0.0,
    "defaultStopTime": 3.0,
    "defaultTolerance": 0.0001,
    "defaultStepSize": 0.002,
    "version": "0.1",
    "supportsCoSimulation": True,
    "supportsModelExchange": True,
    "supportsScheduledExecution": False,
    "numberOfVariables": 2,
    "variableNames": ['dx', 'x'],
    "variableValueReferences": [1, 2],
    "variableDescriptions": ['Derivative of x', 'x'],
    "variableCausalities": ['fmi3CausalityInput', 'fmi3CausalityOutput'],
    "variableVariabilities": ['fmi3VariabilityContinuous', 'fmi3VariabilityContinuous'],
    "variableInitials": ['fmi3InitialExact', 'fmi3InitialCalculated'],
    "variablesSupportsIntermediateUpdate": [False, False],
    "variableDataTypes": ["fmi3DataTypeFloat64", "fmi3DataTypeFloat64"],
    "variableQuantities": ['Velocity', 'Position'],
    "variableUnits": ['m/s', 'm'],
    "variableDisplayUnits": ['km/h', 'km'],
    "variablesHaveStartValues": [True, True],
    "variableStartValues": [0.0, 1.0],
    "numberOfUnits": 2,
    "unitNames": ['rad/s', 'm/s'],
    "baseUnits": [(1.0, 0.0, 0, 1, 0, 0, 0, 0, 0, 0), (1.0, 0.0, 0, 1, -1, 0, 0, 0, 0, 0)],
    "numberOfDisplayUnits": [2, 1],
    "displayUnits": [[('mm', 0.001, 0.0, False), ('km', 1000.0, 0.0, False)], [('km/h', 3.6, 0.0, False)]],
    "displayUnitUnits": [['deg/s', 'r/min'], ['km/h']],
    "float64type": ('Speed that can only be positive', 'Velocity', 'm/s', '', False, False, 0.0, 1.7976931348623157e+308, 1.0),
    "numberOfLogCategories": 2,
    "logCategories": [("logStatusError", "Log error messages"), ("logStatusWarning", "Log warning messages")],
    "numberOfModelStructureOutputs": 1,
    "numberOfModelStructureContinuousStateDerivatives": 1,
    "numberOfModelStructureClockedStates": 0,
    "numberOfModelStructureEventIndicators": 0,
    "numberOfModelStructureInitialUnknowns": 1,
    "modelStructureValueReferences": [2, 1, 1],
    "modelStructureNumberOfDependencies": [0, 0, 1],
    "modelStructureDependencies": [[], [], [2]],
    "modelStructureDependencyKindsDefined": [False, False, True],
    "modelStructureDependencyKinds": [[], [], [5]],
    "modelIdentifier": "fmi3",
    "needsExecutionTool": False,
    "canBeInstantiatedOnlyOncePerProcess": False,
    "canGetAndSetFMUState": False,
    "canSerializeFMUState": False,
    "providesDirectionalDerivative": False,
    "providesAdjointDerivatives": False,
    "providesPerElementDependencies": False,
    "providesIntermediateUpdate": False,
    "providesEvaluateDiscreteStates": False,
    "hasEventMode": False,
    "recommendedIntermediateInputSmoothness": 0,
    "maxOutputDerivativeOrder": 0,
    "canHandleVariableCommunicationStepSize": True,
    "canReturnEarlyAfterIntermediateUpdate": False,
    "fixedInternalStepSize": 0,
    "needsCompletedIntegratorStep": False,
    "modelIdentifierSE": "",
    "instantiateSuccess": True,
    "setDebugLoggingSuccess": 0,
    "enterInitializationModeSuccess": 0,
    "exitInitializationModeSuccess": 0,
    "setFloat64Success": 0,
    "doStepSuccess": 0,
    "getFloat64Results": [0, [5.0, 0.0025]],
    "terminateSuccess": 0,
    "doStepResults": (0, False, False, False, 0.001),
    "resetSuccess": 0,
    "setTimeSuccess": 0,
    "getContinousStateDerivativesSuccess": (0, [0.0]),
    "getNominalsOfContinuousStatesSuccess": (0, [1.0]),
    "setContinuousStatesSuccess": 0,
    "getContinuousStatesSuccess": (0, [42.0]),
    "completedIntegratorStepSuccess": (0, False, False),
    "enterStepModeSuccess": 0,
    "enterContinuousTimeModeSuccess": 0,
    "numberOfEventIndicators": 0,
    "numberOfContinuousStates": 1,
}   
success = f.fmi4c_loadFmu(os.path.dirname(os.path.abspath(__file__))+"/fmi3.fmu", "testfmu")
if not success:
    print("Failed to load fmi3.fmu")
    print(f.fmi4c_getErrorMessages())
    exit(1)
else:
    print("Successfully loaded fmi3.fmu")

verify("fmiVersion", f.fmi4c_getFmiVersion())
verify("modelName", f.fmi3_modelName())
verify("instantiationToken", f.fmi3_instantiationToken())
verify("description", f.fmi3_description())
verify("author", f.fmi3_author())

verify("copyright", f.fmi3_copyright())
verify("license", f.fmi3_license())
verify("generationTool", f.fmi3_generationTool())
verify("generationDateAndTime", f.fmi3_generationDateAndTime())
verify("variableNamingConvention", f.fmi3_variableNamingConvention())
verify("supportsModelExchange", f.fmi3_supportsModelExchange())
verify("supportsScheduledExecution", f.fmi3_supportsScheduledExecution())
verify("supportsCoSimulation", f.fmi3_supportsCoSimulation())
verify("defaultStartTimeDefined", f.fmi3_defaultStartTimeDefined())
verify("defaultStopTimeDefined", f.fmi3_defaultStopTimeDefined())
verify("defaultToleranceDefined", f.fmi3_defaultToleranceDefined())
verify("defaultStepSizeDefined", f.fmi3_defaultStepSizeDefined())
verify("defaultStartTime", f.fmi3_getDefaultStartTime())
verify("defaultStopTime", f.fmi3_getDefaultStopTime())
verify("defaultTolerance", f.fmi3_getDefaultTolerance())
verify("defaultStepSize", f.fmi3_getDefaultStepSize())

#Test variables functions
verify("numberOfVariables", f.fmi3_getNumberOfVariables())
variableNames = []
variableDescriptions = []
variableValueReferences = []
variableCausalities = []
variableVariabilities = []
variableInitials = []
variablesSupportsIntermediateUpdate = []
variableDataTypes = []
variableQuantities = []
variableUnits = []
variableDisplayUnits = []
variablesHaveStartValues = []
variableStartValues = []
for i in range(numberOfVariables):
    var = f.fmi3_getVariableByIndex(i+1)
    variableNames.append(f.fmi3_getVariableName(var))
    var = f.fmi3_getVariableByName(variableNames[-1])
    variableValueReferences.append(f.fmi3_getVariableValueReference(var))
    var = f.fmi3_getVariableByValueReference(variableValueReferences[-1])
    variableDescriptions.append(f.fmi3_getVariableDescription(var))
    variableCausalities.append(f.fmi3_getVariableCausality(var))
    variableVariabilities.append(f.fmi3_getVariableVariability(var))
    variableInitials.append(f.fmi3_getVariableInitial(var))
    variablesSupportsIntermediateUpdate.append(f.fmi3_getVariableSupportsIntermediateUpdate(var))
    variableDataTypes.append(f.fmi3_getVariableDataType(var))
    variableQuantities.append(f.fmi3_getVariableQuantity(var))
    variableUnits.append(f.fmi3_getVariableUnit(var))
    variableDisplayUnits.append(f.fmi3_getVariableDisplayUnit(var))
    variablesHaveStartValues.append(f.fmi3_getVariableHasStartValue(var))
    if f.fmi3_getVariableHasStartValue(var):
        if f.fmi3_getVariableDataType(var) == "fmi3DataTypeFloat64":
            variableStartValues.append(f.fmi3_getVariableStartFloat64(var))
        if f.fmi3_getVariableDataType(var) == "fmi3DataTypeFloat32":
            variableStartValues.append(f.fmi3_getVariableStartFloat32(var))
        if f.fmi3_getVariableDataType(var) == "fmi3DataTypeInt64":
            variableStartValues.append(f.fmi3_getVariableStartInt64(var))
        if f.fmi3_getVariableDataType(var) == "fmi3DataTypeInt32":
            variableStartValues.append(f.fmi3_getVariableStartInt32(var))
        if f.fmi3_getVariableDataType(var) == "fmi3DataTypeInt16":
            variableStartValues.append(f.fmi3_getVariableStartInt16(var))
        if f.fmi3_getVariableDataType(var) == "fmi3DataTypeInt8":
            variableStartValues.append(f.fmi3_getVariableStartInt8(var))
        if f.fmi3_getVariableDataType(var) == "fmi3DataTypeUInt64":
            variableStartValues.append(f.fmi3_getVariableStartUInt64(var))
        if f.fmi3_getVariableDataType(var) == "fmi3DataTypeUInt32":
            variableStartValues.append(f.fmi3_getVariableStartUInt32(var))
        if f.fmi3_getVariableDataType(var) == "fmi3DataTypeUInt16":
            variableStartValues.append(f.fmi3_getVariableStartUInt16(var))
        if f.fmi3_getVariableDataType(var) == "fmi3DataTypeUInt8":
            variableStartValues.append(f.fmi3_getVariableStartUInt8(var))
        if f.fmi3_getVariableDataType(var) == "fmi3DataTypeBoolean":
            variableStartValues.append(f.fmi3_getVariableStartBoolean(var))
        if f.fmi3_getVariableDataType(var) == "fmi3DataTypeString":
            variableStartValues.append(f.fmi3_getVariableStartString(var))
        if f.fmi3_getVariableDataType(var) == "fmi3DataTypeBinary":
            variableStartValues.append(f.fmi3_getVariableStartBinary(var))
verify("variableNames", variableNames)
verify("variableValueReferences", variableValueReferences)
verify("variableDescriptions", variableDescriptions)
verify("variableCausalities", variableCausalities)
verify("variableVariabilities", variableVariabilities)
verify("variableInitials", variableInitials)
verify("variablesSupportsIntermediateUpdate", variablesSupportsIntermediateUpdate)
verify("variableDataTypes", variableDataTypes)
verify("variableQuantities", variableQuantities)
verify("variableUnits", variableUnits)
verify("variableDisplayUnits", variableDisplayUnits)
verify("variableStartValues", variableStartValues)

numberOfModelStructureOutputs = f.fmi3_getNumberOfModelStructureOutputs()
numberOfModelStructureContinuousStateDerivatives = f.fmi3_getNumberOfModelStructureContinuousStateDerivatives()
numberOfModelStructureClockedStates = f.fmi3_getNumberOfModelStructureClockedStates()
numberOfModelStructureEventIndicators= f.fmi3_getNumberOfModelStructureEventIndicators()
numberOfModelStructureInitialUnknowns = f.fmi3_getNumberOfModelStructureInitialUnknowns()

modelStructureValueReferences = []
modelStructureNumberOfDependencies = []
modelStructureDependencies = []
modelStructureDependencyKindsDefined = []
modelStructureDependencyKinds = []
for i in range(numberOfModelStructureOutputs):
    unknown = f.fmi3_getModelStructureOutput(i)
    modelStructureValueReferences.append(f.fmi3_getModelStructureValueReference(unknown))
    nDependencies = f.fmi3_getModelStructureNumberOfDependencies(unknown)
    modelStructureNumberOfDependencies.append(nDependencies)
    modelStructureDependencies.append(f.fmi3_getModelStructureDependencies(unknown, nDependencies))
    dependencyKindsDefined = f.fmi3_getModelStructureDependencyKindsDefined(unknown)
    modelStructureDependencyKindsDefined.append(dependencyKindsDefined)
    if dependencyKindsDefined:
        modelStructureDependencyKinds.append(f.fmi3_getModelStructureDependencyKinds(unknown, nDependencies))
    else:
        modelStructureDependencyKinds.append([])
    
for i in range(numberOfModelStructureContinuousStateDerivatives):
    unknown = f.fmi3_getModelStructureContinuousStateDerivative(i)
    modelStructureValueReferences.append(f.fmi3_getModelStructureValueReference(unknown))
    nDependencies = f.fmi3_getModelStructureNumberOfDependencies(unknown)
    modelStructureNumberOfDependencies.append(nDependencies)
    modelStructureDependencies.append(f.fmi3_getModelStructureDependencies(unknown, nDependencies))
    dependencyKindsDefined = f.fmi3_getModelStructureDependencyKindsDefined(unknown)
    modelStructureDependencyKindsDefined.append(dependencyKindsDefined)
    if dependencyKindsDefined:
        modelStructureDependencyKinds.append(f.fmi3_getModelStructureDependencyKinds(unknown, nDependencies))
    else:
        modelStructureDependencyKinds.append([])

for i in range(numberOfModelStructureClockedStates):
    unknown = f.fmi3_getModelStructureClockedState(i)
    modelStructureValueReferences.append(f.fmi3_getModelStructureValueReference(unknown))
    nDependencies = f.fmi3_getModelStructureNumberOfDependencies(unknown)
    modelStructureNumberOfDependencies.append(nDependencies)
    modelStructureDependencies.append(f.fmi3_getModelStructureDependencies(unknown, nDependencies))
    dependencyKindsDefined = f.fmi3_getModelStructureDependencyKindsDefined(unknown)
    modelStructureDependencyKindsDefined.append(dependencyKindsDefined)
    if dependencyKindsDefined:
        modelStructureDependencyKinds.append(f.fmi3_getModelStructureDependencyKinds(unknown, nDependencies))
    else:
        modelStructureDependencyKinds.append([])

for i in range(numberOfModelStructureEventIndicators):
    unknown = f.fmi3_getModelStructureEventIndicator(i)
    modelStructureValueReferences.append(f.fmi3_getModelStructureValueReference(unknown))
    nDependencies = f.fmi3_getModelStructureNumberOfDependencies(unknown)
    modelStructureNumberOfDependencies.append(nDependencies)
    modelStructureDependencies.append(f.fmi3_getModelStructureDependencies(unknown, nDependencies))
    dependencyKindsDefined = f.fmi3_getModelStructureDependencyKindsDefined(unknown)
    modelStructureDependencyKindsDefined.append(dependencyKindsDefined)
    if dependencyKindsDefined:
        modelStructureDependencyKinds.append(f.fmi3_getModelStructureDependencyKinds(unknown, nDependencies))
    else:
        modelStructureDependencyKinds.append([])

for i in range(numberOfModelStructureInitialUnknowns):
    unknown = f.fmi3_getModelStructureInitialUnknown(i)
    modelStructureValueReferences.append(f.fmi3_getModelStructureValueReference(unknown))
    nDependencies = f.fmi3_getModelStructureNumberOfDependencies(unknown)
    modelStructureNumberOfDependencies.append(nDependencies)
    modelStructureDependencies.append(f.fmi3_getModelStructureDependencies(unknown, nDependencies))
    dependencyKindsDefined = f.fmi3_getModelStructureDependencyKindsDefined(unknown)
    modelStructureDependencyKindsDefined.append(dependencyKindsDefined)
    if dependencyKindsDefined:
        modelStructureDependencyKinds.append(f.fmi3_getModelStructureDependencyKinds(unknown, nDependencies))
    else:
        modelStructureDependencyKinds.append([])
    
verify("numberOfModelStructureOutputs", numberOfModelStructureOutputs)
verify("numberOfModelStructureContinuousStateDerivatives", numberOfModelStructureContinuousStateDerivatives)
verify("numberOfModelStructureClockedStates", numberOfModelStructureClockedStates)
verify("numberOfModelStructureEventIndicators", numberOfModelStructureEventIndicators)
verify("numberOfModelStructureInitialUnknowns", numberOfModelStructureInitialUnknowns)
verify("modelStructureValueReferences", modelStructureValueReferences)
verify("modelStructureNumberOfDependencies", modelStructureNumberOfDependencies)
verify("modelStructureDependencies", modelStructureDependencies)
verify("modelStructureDependencyKindsDefined", modelStructureDependencyKindsDefined)
verify("modelStructureDependencyKinds", modelStructureDependencyKinds)

numberOfUnits = f.fmi3_getNumberOfUnits()   
verify("numberOfUnits", numberOfUnits)
 
unitNames = []
baseUnits = []
numberOfDisplayUnits = []
displayUnits = []
for i in range(numberOfUnits):
    unit = f.fmi3_getUnitByIndex(i)
    unitNames.append(f.fmi3_getUnitName(unit))
    if(f.fmi3_hasBaseUnit(unit)):
        baseUnits.append(f.fmi3_getBaseUnit(unit))
    numberOfDisplayUnits.append(f.fmi3_getNumberOfDisplayUnits(unit))  
    tempUnits = []
    for j in range(numberOfDisplayUnits[-1]):    
        tempUnits.append(f.fmi3_getDisplayUnitByIndex(unit, j))
    displayUnits.append(tempUnits)
verify("unitNames", units)
verify("baseUnits", baseUnits)
verify("numberOfDisplayUnits", numberOfDisplayUnits)
verify("displayUnits", displayUnits)

verify("float64type", f.fmi3_getFloat64Type("PositiveSpeed"))

numberOfLogCategories = f.fmi3_getNumberOfLogCategories()
verify("numberOfLogCategories", numberOfLogCategories)
logCategories = []
for i in range(numberOfLogCategories):
    logCategories.append(f.fmi3_getLogCategory(i))
verify("logCategories", logCategories);

#Test co-simulation capability flags
verify("modelIdentifier", f.fmi3cs_getModelIdentifier())
verify("needsExecutionTool", f.fmi3cs_getNeedsExecutionTool())
verify("canBeInstantiatedOnlyOncePerProcess", f.fmi3cs_getCanBeInstantiatedOnlyOncePerProcess())
verify("canGetAndSetFMUState", f.fmi3cs_getCanGetAndSetFMUState())
verify("canSerializeFMUState", f.fmi3cs_getCanSerializeFMUState())
verify("providesDirectionalDerivative", f.fmi3cs_getProvidesDirectionalDerivative())
verify("providesAdjointDerivatives", f.fmi3cs_getProvidesAdjointDerivatives())
verify("providesPerElementDependencies", f.fmi3cs_getProvidesPerElementDependencies())
verify("providesIntermediateUpdate", f.fmi3cs_getProvidesIntermediateUpdate())
verify("providesEvaluateDiscreteStates", f.fmi3cs_getProvidesEvaluateDiscreteStates())
verify("hasEventMode", f.fmi3cs_getHasEventMode())
verify("recommendedIntermediateInputSmoothness", f.fmi3cs_getRecommendedIntermediateInputSmoothness())
verify("maxOutputDerivativeOrder", f.fmi3cs_getMaxOutputDerivativeOrder())
verify("canHandleVariableCommunicationStepSize", f.fmi3cs_getCanHandleVariableCommunicationStepSize())
verify("canReturnEarlyAfterIntermediateUpdate", f.fmi3cs_getCanReturnEarlyAfterIntermediateUpdate())
verify("fixedInternalStepSize", f.fmi3cs_getFixedInternalStepSize()),

#Test model exchange capability flags
verify("modelIdentifier", f.fmi3me_getModelIdentifier())
verify("needsExecutionTool", f.fmi3me_getNeedsExecutionTool())
verify("canBeInstantiatedOnlyOncePerProcess", f.fmi3me_getCanBeInstantiatedOnlyOncePerProcess())
verify("canGetAndSetFMUState", f.fmi3me_getCanGetAndSetFMUState())
verify("canSerializeFMUState", f.fmi3me_getCanSerializeFMUState())
verify("providesDirectionalDerivative", f.fmi3me_getProvidesDirectionalDerivative())
verify("providesAdjointDerivatives", f.fmi3me_getProvidesAdjointDerivatives())
verify("providesPerElementDependencies", f.fmi3me_getProvidesPerElementDependencies())
verify("providesEvaluateDiscreteStates", f.fmi3me_getProvidesEvaluateDiscreteStates())
verify("needsCompletedIntegratorStep", f.fmi3me_getNeedsCompletedIntegratorStep())

#Test scheduled execution capability flags
verify("modelIdentifierSE", f.fmi3se_getModelIdentifier())
verify("needsExecutionTool", f.fmi3se_getNeedsExecutionTool())
verify("canBeInstantiatedOnlyOncePerProcess", f.fmi3se_getCanBeInstantiatedOnlyOncePerProcess())
verify("canGetAndSetFMUState", f.fmi3se_getCanGetAndSetFMUState())
verify("canSerializeFMUState", f.fmi3se_getCanSerializeFMUState())
verify("providesDirectionalDerivative", f.fmi3se_getProvidesDirectionalDerivative())
verify("providesAdjointDerivatives", f.fmi3se_getProvidesAdjointDerivatives())
verify("providesPerElementDependencies", f.fmi3se_getProvidesPerElementDependencies())

#Test co-simualtion
verify("instantiateSuccess", f.fmi3_instantiateCoSimulation(False, False, False, False, [], 0))
verify("fmiVersionNumber", f.fmi3_getVersion())
verify("version", f.fmi3_version())
verify("setDebugLoggingSuccess", f.fmi3_setDebugLogging(True, 0, None))
verify("enterInitializationModeSuccess", f.fmi3_enterInitializationMode(True, 1e-5, 0, True, 2))
verify("exitInitializationModeSuccess", f.fmi3_exitInitializationMode())
verify("setFloat64Success", f.fmi3_setFloat64([1], 1, [5], 1))
verify("doStepResults", f.fmi3_doStep(0, 0.001, True))
verify("getFloat64Results", f.fmi3_getFloat64([1, 2],2,2))
verify("resetSuccess", f.fmi3_reset())

#Test model exchange
verify("instantiateSuccess", f.fmi3_instantiateModelExchange(False, False))
verify("setTimeSuccess", f.fmi3_setTime(0.5))
verify("numberOfEventIndicators", f.fmi3_getNumberOfEventIndicators()[1])
numberOfContinuousStates = f.fmi3_getNumberOfContinuousStates()[1]
verify("numberOfContinuousStates", numberOfContinuousStates)
verify("getContinousStateDerivativesSuccess", f.fmi3_getContinuousStateDerivatives(numberOfContinuousStates))
verify("getNominalsOfContinuousStatesSuccess", f.fmi3_getNominalsOfContinuousStates(numberOfContinuousStates))
verify("setContinuousStatesSuccess", f.fmi3_setContinuousStates([42], numberOfContinuousStates))
verify("getContinuousStatesSuccess", f.fmi3_getContinuousStates(numberOfContinuousStates))
verify("completedIntegratorStepSuccess", f.fmi3_completedIntegratorStep(False))
verify("enterStepModeSuccess", f.fmi3_enterStepMode())
verify("enterContinuousTimeModeSuccess", f.fmi3_enterContinuousTimeMode())

verify("terminateSuccess", f.fmi3_terminate())

f.fmi3_freeInstance()

f.fmi4c_freeFmu()

#Untested FMI3 functions;
#FMI4C_DLLAPI void fmi3_getFloat32Type(fmiHandle* fmu, const char* name, const char** description, const char** quantity, const char** unit, const char** displayUnit, bool *relativeQuantity, bool *unbounded, float *min, float *max, float *nominal);
#FMI4C_DLLAPI void fmi3_getInt64Type(fmiHandle *fmu, const char *name, const char** description, const char** quantity, int64_t* min, int64_t* max);
#FMI4C_DLLAPI void fmi3_getInt32Type(fmiHandle *fmu, const char *name, const char** description, const char** quantity, int32_t* min, int32_t* max);
#FMI4C_DLLAPI void fmi3_getInt16Type(fmiHandle *fmu, const char *name, const char** description, const char** quantity, int16_t* min, int16_t* max);
#FMI4C_DLLAPI void fmi3_getInt8Type(fmiHandle *fmu, const char *name, const char** description, const char** quantity, int8_t* min, int8_t* max);
#FMI4C_DLLAPI void fmi3_getUInt64Type(fmiHandle *fmu, const char *name, const char** description, const char** quantity, uint64_t* min, uint64_t* max);
#FMI4C_DLLAPI void fmi3_getUInt32Type(fmiHandle *fmu, const char *name, const char** description, const char** quantity, uint32_t* min, uint32_t* max);
#FMI4C_DLLAPI void fmi3_getUInt16Type(fmiHandle *fmu, const char *name, const char** description, const char** quantity, uint16_t* min, uint16_t* max);
#FMI4C_DLLAPI void fmi3_getUInt8Type(fmiHandle *fmu, const char *name, const char** description, const char** quantity, uint8_t* min, uint8_t* max);
#FMI4C_DLLAPI void fmi3_getBooleanType(fmiHandle *fmu, const char *name, const char **description);
#FMI4C_DLLAPI void fmi3_getStringType(fmiHandle *fmu, const char *name, const char **description);
#FMI4C_DLLAPI void fmi3_getBinaryType(fmiHandle *fmu, const char *name, const char **description, const char **mimeType, uint32_t *maxSize);
#FMI4C_DLLAPI void fmi3_getEnumerationType(fmiHandle *fmu, const char *name, const char **description, const char **quantity, int64_t *min, int64_t *max, int *numberOfItems);
#FMI4C_DLLAPI void fmi3_getEnumerationItem(fmiHandle *fmu, const char *typeName, int itemId, const char **itemName, int64_t *value, const char **description);
#FMI4C_DLLAPI void fmi3_getClockType(fmiHandle *fmu, const char *name, const char **description, bool *canBeDeactivated, uint32_t *priority, fmi3IntervalVariability *intervalVariability, float *intervalDecimal, float *shiftDecimal, bool *supportsFraction, uint64_t *resolution, uint64_t *intervalCounter, uint64_t *shiftCounter);
#FMI4C_DLLAPI fmi3ValueReference fmi3_getModelStructureOutputDependency(fmiHandle *fmu, int outputId, int dependencyId, bool *ok);
#FMI4C_DLLAPI fmi3ValueReference fmi3_getModelStructureOutputDependencyKind(fmiHandle *fmu, int outputId, int dependencyId, bool *ok);
#FMI4C_DLLAPI fmi3ValueReference fmi3_getModelStructureContinuousStateDerivativeDependency(fmiHandle *fmu, int derId, int dependencyId, bool *ok);
#FMI4C_DLLAPI fmi3ValueReference fmi3_getModelStructureContinuousStateDerivativeDependencyKind(fmiHandle *fmu, int derId, int dependencyId, bool *ok);
#FMI4C_DLLAPI void fmi3_getModelStructureClockedState(fmiHandle *fmu, int id, fmi3ValueReference *vr, int *numberOfDependencies, bool *dependencyKindsDefined);
#FMI4C_DLLAPI fmi3ValueReference fmi3_getModelStructureClockedStateDependency(fmiHandle *fmu, int clockId, int dependencyId, bool *ok);
#FMI4C_DLLAPI fmi3ValueReference fmi3_getModelStructureClockedStateDependencyKind(fmiHandle *fmu, int clockId, int dependencyId, bool *ok);
#FMI4C_DLLAPI fmi3ValueReference fmi3_modelStructureGetInitialUnknownDependencyKind(fmiHandle *fmu, int unknownId, int dependencyId, bool *ok);
#FMI4C_DLLAPI void fmi3_getModelStructureEventIndicator(fmiHandle *fmu, int id, fmi3ValueReference *vr, int *numberOfDependencies, bool *dependencyKindsDefined);
#FMI4C_DLLAPI fmi3ValueReference fmi3_getModelStructureEventIndicatorDependency(fmiHandle *fmu, int indicatorId, int dependencyId, bool *ok);
#FMI4C_DLLAPI fmi3ValueReference fmi3_getModelStructureEventIndicatorDependencyKind(fmiHandle *fmu, int indicatorId, int dependencyId, bool *ok);
#FMI4C_DLLAPI fmi3Status fmi3_enterEventMode(fmiHandle *fmu);
#FMI4C_DLLAPI fmi3Status fmi3_getFloat32(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,fmi3Float32 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_getInt8(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,fmi3Int8 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_getUInt8(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,fmi3UInt8 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_getInt16(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,fmi3Int16 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_getUInt16(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,fmi3UInt16 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_getInt32(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,fmi3Int32 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_getUInt32(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,fmi3UInt32 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_getInt64(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,fmi3Int64 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_getUInt64(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,fmi3UInt64 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_getBoolean(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,fmi3Boolean values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_getString(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,fmi3String values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_getBinary(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,size_t valueSizes[],fmi3Binary values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_getClock(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,fmi3Clock values[]);
#FMI4C_DLLAPI fmi3Status fmi3_setFloat32(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,const fmi3Float32 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_setInt8(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,const fmi3Int8 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_setUInt8(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,const fmi3UInt8 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_setInt16(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,const fmi3Int16 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_setUInt16(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,const fmi3UInt16 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_setInt32(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,const fmi3Int32 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_setUInt32(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,const fmi3UInt32 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_setInt64(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,const fmi3Int64 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_setUInt64(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,const fmi3UInt64 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_setBoolean(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,const fmi3Boolean values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_setString(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,const fmi3String values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_setBinary(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,const size_t valueSizes[],const fmi3Binary values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_setClock(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,const fmi3Clock values[]);
#FMI4C_DLLAPI fmi3Status fmi3_getNumberOfVariableDependencies(fmiHandle *fmu, fmi3ValueReference valueReference, size_t* nDependencies);
#FMI4C_DLLAPI fmi3Status fmi3_getVariableDependencies(fmiHandle *fmu, fmi3ValueReference dependent, size_t elementIndicesOfDependent[], fmi3ValueReference independents[], size_t elementIndicesOfIndependents[], fmi3DependencyKind dependencyKinds[], size_t nDependencies);
#FMI4C_DLLAPI fmi3Status fmi3_getFMUState(fmiHandle *fmu, fmi3FMUState* FMUState);
#FMI4C_DLLAPI fmi3Status fmi3_setFMUState(fmiHandle *fmu, fmi3FMUState  FMUState);
#FMI4C_DLLAPI fmi3Status fmi3_freeFMUState(fmiHandle *fmu, fmi3FMUState* FMUState);
#FMI4C_DLLAPI fmi3Status fmi3_serializedFMUStateSize(fmiHandle *fmu,fmi3FMUState  FMUState,size_t* size);
#FMI4C_DLLAPI fmi3Status fmi3_serializeFMUState(fmiHandle *fmu,fmi3FMUState  FMUState,fmi3Byte serializedState[],size_t size);
#FMI4C_DLLAPI fmi3Status fmi3_deserializeFMUState(fmiHandle *fmu,const fmi3Byte serializedState[],size_t size,fmi3FMUState* FMUState);
#FMI4C_DLLAPI fmi3Status fmi3_getDirectionalDerivative(fmiHandle *fmu,const fmi3ValueReference unknowns[],size_t nUnknowns,const fmi3ValueReference knowns[],size_t nKnowns,const fmi3Float64 seed[],size_t nSeed,fmi3Float64 sensitivity[],size_t nSensitivity);
#FMI4C_DLLAPI fmi3Status fmi3_getAdjointDerivative(fmiHandle *fmu,const fmi3ValueReference unknowns[],size_t nUnknowns,const fmi3ValueReference knowns[],size_t nKnowns,const fmi3Float64 seed[],size_t nSeed,fmi3Float64 sensitivity[],size_t nSensitivity);
#FMI4C_DLLAPI fmi3Status fmi3_enterConfigurationMode(fmiHandle *fmu);
#FMI4C_DLLAPI fmi3Status fmi3_exitConfigurationMode(fmiHandle *fmu);
#FMI4C_DLLAPI fmi3Status fmi3_getIntervalDecimal(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,fmi3Float64 intervals[],fmi3IntervalQualifier qualifiers[]);
#FMI4C_DLLAPI fmi3Status fmi3_getIntervalFraction(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,fmi3UInt64 intervalCounters[],fmi3UInt64 resolutions[],fmi3IntervalQualifier qualifiers[]);
#FMI4C_DLLAPI fmi3Status fmi3_getShiftDecimal(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,fmi3Float64 shifts[]);
#FMI4C_DLLAPI fmi3Status fmi3_getShiftFraction(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,fmi3UInt64 shiftCounters[],fmi3UInt64 resolutions[]);
#FMI4C_DLLAPI fmi3Status fmi3_setIntervalDecimal(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,const fmi3Float64 intervals[]);
#FMI4C_DLLAPI fmi3Status fmi3_setIntervalFraction(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,const fmi3UInt64 intervalCounters[],const fmi3UInt64 resolutions[]);
#FMI4C_DLLAPI fmi3Status fmi3_evaluateDiscreteStates(fmiHandle *fmu);
#FMI4C_DLLAPI fmi3Status fmi3_updateDiscreteStates(fmiHandle *fmu,fmi3Boolean* discreteStatesNeedUpdate,fmi3Boolean* terminateSimulation,fmi3Boolean* nominalsOfContinuousStatesChanged,fmi3Boolean* valuesOfContinuousStatesChanged,fmi3Boolean* nextEventTimeDefined,fmi3Float64* nextEventTime);
#FMI4C_DLLAPI fmi3Status fmi3_getEventIndicators(fmiHandle *fmu,fmi3Float64 eventIndicators[],size_t nEventIndicators);
#FMI4C_DLLAPI fmi3Status fmi3_getOutputDerivatives(fmiHandle *fmu,const fmi3ValueReference valueReferences[],size_t nValueReferences,const fmi3Int32 orders[],fmi3Float64 values[],size_t nValues);
#FMI4C_DLLAPI fmi3Status fmi3_activateModelPartition(fmiHandle *fmu,fmi3ValueReference clockReference,fmi3Float64 activationTime);

# # #
print("")      
print("All tests were successful!")
             
exit(0)