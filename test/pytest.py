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
    var = f.fmi1_getVariableByIndex(i)
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
    var = f.fmi1_getVariableByIndex(i);
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
    var = f.fmi1_getVariableByIndex(i)
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
    var = f.fmi1_getVariableByIndex(i);
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

getDerivativesSuccess = f.fmi1_getDerivatives([1],1)
verify("getDerivativesSuccess", getDerivativesSuccess)

getStateValueReferencesSuccess = f.fmi1_getStateValueReferences(1)
verify("getStateValueReferencesSuccess", getStateValueReferencesSuccess)

getNominalContinuousStatesSuccess = f.fmi1_getNominalContinuousStates([1],1)
verify("getNominalContinuousStatesSuccess", getNominalContinuousStatesSuccess)

setContinuousStatesSuccess = f.fmi1_setContinuousStates([42], 1)
verify("setContinuousStatesSuccess", setContinuousStatesSuccess)

getContinuousStatesSuccess = f.fmi1_getContinuousStates([1],1)
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
    "fmiType": "fmi1ModelExchange",
    "modelName": "fmi2",
    "modelIdentifier": "fmi2",
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
    "variableCausalities": ["fmi2CausalityInput", "fmi2CausalityOutput"],
    "variableVariabilities": ["fmi2VariabilityContinuous", "fmi2VariabilityContinuous"],
    "variableInitials": ["fmi2InitialUnknown", "fmi2InitialExact"],
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
    "numberOfBaseUnits": 2,
    "units": ['rad/s', 'm/s'],
    "numberOfDisplayUnits": [2, 1],
    "displayUnitUnits": [['deg/s', 'r/min'], ['km/h']],
    "numberOfBaseUnits": 2,
    "units": ['rad/s', 'm/s'],
    "numberOfDisplayUnits": [2, 1],
    "displayUnitUnits": [['deg/s', 'r/min'], ['km/h']],
    "variableStartValues": [0.0, 1.0],
    "typesPlatform": "default",
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

author = f.fmi2_getAuthor().decode()
verify("author", author)

generationTool = f.fmi2_getGenerationTool().decode()
verify("generationTool", generationTool)

generationDateAndTime = f.fmi2_getGenerationDateAndTime().decode()
verify("generationDateAndTime", generationDateAndTime)
#
modelName = f.fmi2_getModelName().decode()
verify("modelName", modelName)

description = f.fmi2_getModelDescription().decode()
verify("description", description)

modelIdentifier = f.fmi2_getModelIdentifier().decode()
verify("modelIdentifier", modelIdentifier)

copyright = f.fmi2_getCopyright().decode()
verify("copyright", copyright)

license = f.fmi2_getLicense().decode()
verify("license", license)

variableNamingConvention = f.fmi2_getVariableNamingConvention().decode()
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
    var = f.fmi2_getVariableByIndex(i)
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

instantiateSuccess = f.fmi2_instantiate(0, False, False)    # 0 = model exchange
verify("instantiateSuccess", instantiateSuccess)

typesPlatform = f.fmi2_getTypesPlatform()
verify("typesPlatform", typesPlatform)


f.fmi4c_freeFmu()
     
print("")      
print("All tests were successful!")
             
exit(0)      