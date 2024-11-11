import ctypes as ct

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
    "variableDescriptions": ['Derivative of x', 'x'],
    "variableQuantities": ['', ''],
    "variableUnits": ['m/s', 'm'],
    "variableDisplayUnits": ['', ''],
    "variableRelativeQuantities": ['', ''],
    "variableMinValues": [-1.7976931348623157e+308, -1.7976931348623157e+308],
    "variableMaxValues": [1.7976931348623157e+308, 1.7976931348623157e+308],
    "variableNominals": [1.0, 1.0],
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
variableDescriptions = []
variableQuantities = []
variableUnits = []
variableDisplayUnits = []
variableRelativeQuantities = []
variableMinValues = []
variableMaxValues = []
variableNominals = []
for i in range(numberOfVariables):
    var = f.fmi1_getVariableByIndex(i)
    variableNames.append(f.fmi1_getVariableName(var))
    variableDescriptions.append(f.fmi1_getVariableDescription(var))
    variableQuantities.append(f.fmi1_getVariableQuantity(var))
    variableUnits.append(f.fmi1_getVariableUnit(var))
    variableDisplayUnits.append(f.fmi1_getVariableDisplayUnit(var))
    variableRelativeQuantities.append(f.fmi1_getVariableQuantity(var))
    variableMinValues.append(f.fmi1_getVariableMin(var))
    variableMaxValues.append(f.fmi1_getVariableMax(var))
    variableNominals.append(f.fmi1_getVariableNominal(var))
    
verify("variableNames", variableNames)
verify("variableDescriptions", variableDescriptions)
verify("variableQuantities", variableQuantities)
verify("variableUnits", variableUnits)
verify("variableDisplayUnits", variableDisplayUnits)
verify("variableRelativeQuantities", variableRelativeQuantities)
verify("variableMinValues", variableMinValues)
verify("variableMaxValues", variableMaxValues) 
verify("variableNominals", variableNominals)

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
        if f.fmi1_getVariableDataType(var) == 0:
            variableStartValues.append(f.fmi1_getVariableStartReal(var))
        if f.fmi1_getVariableDataType(var) == 1:
            variableStartValues.append(f.fmi1_getVariableStartInteger(var))
        if f.fmi1_getVariableDataType(var) == 2:
            variableStartValues.append(f.fmi1_getVariableStartBoolean(var))
        if f.fmi1_getVariableDataType(var) == 3:
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

#Test FMI1 ME

             
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
    "variableDescriptions": ['Derivative of x', 'x'],
    "variableQuantities": ['', ''],
    "variableUnits": ['m/s', 'm'],
    "variableDisplayUnits": ['', ''],
    "variableRelativeQuantities": ['', ''],
    "variableMinValues": [-1.7976931348623157e+308, -1.7976931348623157e+308],
    "variableMaxValues": [1.7976931348623157e+308, 1.7976931348623157e+308],
    "variableNominals": [1.0, 1.0],
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
variableDescriptions = []
variableQuantities = []
variableUnits = []
variableDisplayUnits = []
variableRelativeQuantities = []
variableMinValues = []
variableMaxValues = []
variableNominals = []
for i in range(numberOfVariables):
    var = f.fmi1_getVariableByIndex(i)
    variableNames.append(f.fmi1_getVariableName(var))
    variableDescriptions.append(f.fmi1_getVariableDescription(var))
    variableQuantities.append(f.fmi1_getVariableQuantity(var))
    variableUnits.append(f.fmi1_getVariableUnit(var))
    variableDisplayUnits.append(f.fmi1_getVariableDisplayUnit(var))
    variableRelativeQuantities.append(f.fmi1_getVariableQuantity(var))
    variableMinValues.append(f.fmi1_getVariableMin(var))
    variableMaxValues.append(f.fmi1_getVariableMax(var))
    variableNominals.append(f.fmi1_getVariableNominal(var))
    
verify("variableNames", variableNames)
verify("variableDescriptions", variableDescriptions)
verify("variableQuantities", variableQuantities)
verify("variableUnits", variableUnits)
verify("variableDisplayUnits", variableDisplayUnits)
verify("variableRelativeQuantities", variableRelativeQuantities)
verify("variableMinValues", variableMinValues)
verify("variableMaxValues", variableMaxValues) 
verify("variableNominals", variableNominals)

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
        if f.fmi1_getVariableDataType(var) == 0:
            variableStartValues.append(f.fmi1_getVariableStartReal(var))
        if f.fmi1_getVariableDataType(var) == 1:
            variableStartValues.append(f.fmi1_getVariableStartInteger(var))
        if f.fmi1_getVariableDataType(var) == 2:
            variableStartValues.append(f.fmi1_getVariableStartBoolean(var))
        if f.fmi1_getVariableDataType(var) == 3:
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
     
print("")      
print("All tests were successful!")
             
exit(0)      