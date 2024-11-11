import ctypes as ct

class fmi4c:

    class fmi1EventInfo(ct.Structure):
        _fields_=[("upcomingTimeEvent",ct.c_bool),
              ("stateValuesChanged",ct.c_bool),
              ("terminateSimulation",ct.c_bool),
              ("iterationConverged",ct.c_bool),
              ("nextEventTime",ct.c_double)]

    def __init__(self):
        import ctypes
        import os
        if os.name == "posix":
            self.hdll = ctypes.cdll.LoadLibrary(os.path.dirname(os.path.abspath(__file__)) + "/../libfmi4c.so")
        elif os.name == "nt":
            self.hdll = ctypes.cdll.LoadLibrary(os.path.dirname(os.path.abspath(__file__)) + "/../libfmi4c.dll")
        self.hdll.fmi4c_getFmiVersion.argtypes = ct.c_void_p,
        self.hdll.fmi4c_getFmiVersion.restype = ct.c_int
        self.hdll.fmi4c_loadFmu.argtypes = ct.c_char_p, ct.c_char_p,
        self.hdll.fmi4c_loadFmu.restype = ct.c_void_p
        
        #FMI1 metadata functions
        self.hdll.fmi1_getType.restype = ct.c_int
        self.hdll.fmi1_getType.argtypes = ct.c_void_p,
        self.hdll.fmi1_getModelName.restype = ct.c_char_p
        self.hdll.fmi1_getModelName.argtypes = ct.c_void_p,
        self.hdll.fmi1_getModelIdentifier.restype = ct.c_char_p
        self.hdll.fmi1_getModelIdentifier.argtypes = ct.c_void_p,
        self.hdll.fmi1_getGuid.restype = ct.c_char_p
        self.hdll.fmi1_getGuid.argtypes = ct.c_void_p,
        self.hdll.fmi1_getDescription.restype = ct.c_char_p
        self.hdll.fmi1_getDescription.argtypes = ct.c_void_p,
        self.hdll.fmi1_getAuthor.restype = ct.c_char_p
        self.hdll.fmi1_getAuthor.argtypes = ct.c_void_p,
        self.hdll.fmi1_getGenerationTool.restype = ct.c_char_p
        self.hdll.fmi1_getGenerationTool.argtypes = ct.c_void_p,
        self.hdll.fmi1_getGenerationDateAndTime.restype = ct.c_char_p
        self.hdll.fmi1_getGenerationDateAndTime.argtypes = ct.c_void_p,
        self.hdll.fmi1_getNumberOfContinuousStates.restype = ct.c_int
        self.hdll.fmi1_getNumberOfContinuousStates.argtypes = ct.c_void_p,
        self.hdll.fmi1_getNumberOfEventIndicators.restype = ct.c_int
        self.hdll.fmi1_getNumberOfEventIndicators.argtypes = ct.c_void_p,
        self.hdll.fmi1_defaultStartTimeDefined.restype = ct.c_bool
        self.hdll.fmi1_defaultStartTimeDefined.argtypes = ct.c_void_p,
        self.hdll.fmi1_defaultStopTimeDefined.restype = ct.c_bool
        self.hdll.fmi1_defaultStopTimeDefined.argtypes = ct.c_void_p,
        self.hdll.fmi1_defaultToleranceDefined.restype = ct.c_bool
        self.hdll.fmi1_defaultToleranceDefined.argtypes = ct.c_void_p,
        self.hdll.fmi1_getDefaultStartTime.restype = ct.c_double
        self.hdll.fmi1_getDefaultStartTime.argtypes = ct.c_void_p,
        self.hdll.fmi1_getDefaultStopTime.restype = ct.c_double
        self.hdll.fmi1_getDefaultStopTime.argtypes = ct.c_void_p,
        self.hdll.fmi1_getDefaultTolerance.restype = ct.c_double
        self.hdll.fmi1_getDefaultTolerance.argtypes = ct.c_void_p,
        self.hdll.fmi1_getNumberOfVariables.restype = ct.c_int
        self.hdll.fmi1_getNumberOfVariables.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVariableByIndex.restype = ct.c_void_p
        self.hdll.fmi1_getVariableByIndex.argtypes = ct.c_void_p,ct.c_int
        self.hdll.fmi1_getVariableByValueReference.restype = ct.c_void_p
        self.hdll.fmi1_getVariableByValueReference.argtypes = ct.c_void_p,ct.c_int
        self.hdll.fmi1_getVariableByName.restype = ct.c_void_p
        self.hdll.fmi1_getVariableByName.argtypes = ct.c_void_p,ct.c_char_p
        self.hdll.fmi1_getVariableName.restype = ct.c_char_p
        self.hdll.fmi1_getVariableName.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVariableDescription.restype = ct.c_char_p
        self.hdll.fmi1_getVariableDescription.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVariableQuantity.restype = ct.c_char_p
        self.hdll.fmi1_getVariableQuantity.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVariableUnit.restype = ct.c_char_p
        self.hdll.fmi1_getVariableUnit.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVariableDisplayUnit.restype = ct.c_char_p
        self.hdll.fmi1_getVariableDisplayUnit.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVariableRelativeQuantity.restype = ct.c_bool
        self.hdll.fmi1_getVariableRelativeQuantity.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVariableMin.restype = ct.c_double
        self.hdll.fmi1_getVariableMin.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVariableMax.restype = ct.c_double
        self.hdll.fmi1_getVariableMax.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVariableNominal.restype = ct.c_double
        self.hdll.fmi1_getVariableNominal.argtypes = ct.c_void_p,
        self.hdll.fmi1_getNumberOfBaseUnits.restype = ct.c_int
        self.hdll.fmi1_getNumberOfBaseUnits.argtypes = ct.c_void_p,
        self.hdll.fmi1_getBaseUnitByIndex.restype = ct.c_void_p
        self.hdll.fmi1_getBaseUnitByIndex.argtypes = ct.c_void_p,ct.c_int,
        self.hdll.fmi1_getBaseUnitUnit.restype = ct.c_char_p
        self.hdll.fmi1_getBaseUnitUnit.argtypes = ct.c_void_p,
        self.hdll.fmi1_getNumberOfDisplayUnits.restype = ct.c_int
        self.hdll.fmi1_getNumberOfDisplayUnits.argtypes = ct.c_void_p,
        self.hdll.fmi1_getDisplayUnitByIndex.argtypes = ct.c_void_p,ct.c_int,ct.POINTER(ct.c_char_p),ct.POINTER(ct.c_double),ct.POINTER(ct.c_double),
        self.hdll.fmi1_getVariableHasStartValue.restype = ct.c_bool
        self.hdll.fmi1_getVariableHasStartValue.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVariableStartReal.restype = ct.c_double
        self.hdll.fmi1_getVariableStartReal.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVariableStartInteger.restype = ct.c_int
        self.hdll.fmi1_getVariableStartInteger.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVariableStartBoolean.restype = ct.c_bool
        self.hdll.fmi1_getVariableStartBoolean.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVariableStartString.restype = ct.c_char_p
        self.hdll.fmi1_getVariableStartString.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVariableValueReference.restype = ct.c_uint
        self.hdll.fmi1_getVariableValueReference.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVariableCausality.restype = ct.c_int
        self.hdll.fmi1_getVariableCausality.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVariableVariability.restype = ct.c_int
        self.hdll.fmi1_getVariableVariability.argtypes = ct.c_void_p,
        self.hdll.fmi1_getAlias.restype = ct.c_int
        self.hdll.fmi1_getAlias.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVariableIsFixed.restype = ct.c_bool
        self.hdll.fmi1_getVariableIsFixed.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVariableDataType.restype = ct.c_int
        self.hdll.fmi1_getVariableDataType.argtypes = ct.c_void_p,
        
        #FMI1 functions
        self.hdll.fmi1_getTypesPlatform.restype = ct.c_char_p
        self.hdll.fmi1_getTypesPlatform.argtypes = ct.c_void_p,
        self.hdll.fmi1_getVersion.restype = ct.c_char_p
        self.hdll.fmi1_getVersion.argtypes = ct.c_void_p,
        self.hdll.fmi1_setDebugLogging.restype = ct.c_int
        self.hdll.fmi1_setDebugLogging.argtypes = ct.c_void_p,ct.c_bool,
        self.hdll.fmi1_getReal.restype = ct.c_int
        self.hdll.fmi1_getReal.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint),ct.c_size_t,ct.POINTER(ct.c_double),
        self.hdll.fmi1_getInteger.restype = ct.c_int
        self.hdll.fmi1_getInteger.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint),ct.c_size_t,ct.POINTER(ct.c_int),
        self.hdll.fmi1_getBoolean.restype = ct.c_int
        self.hdll.fmi1_getBoolean.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint),ct.c_size_t,ct.POINTER(ct.c_bool),
        self.hdll.fmi1_getString.restype = ct.c_int
        self.hdll.fmi1_getString.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint),ct.c_size_t,ct.POINTER(ct.c_char_p),
        self.hdll.fmi1_setReal.restype = ct.c_int
        self.hdll.fmi1_setReal.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint),ct.c_size_t,ct.POINTER(ct.c_double),
        self.hdll.fmi1_setInteger.restype = ct.c_int
        self.hdll.fmi1_setInteger.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint),ct.c_size_t,ct.POINTER(ct.c_int),
        self.hdll.fmi1_setBoolean.restype = ct.c_int
        self.hdll.fmi1_setBoolean.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint),ct.c_size_t,ct.POINTER(ct.c_bool),
        self.hdll.fmi1_setString.restype = ct.c_int
        self.hdll.fmi1_setString.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint),ct.c_size_t,ct.POINTER(ct.c_char_p),
        self.hdll.fmi1_instantiateSlave.restype = ct.c_bool
        self.hdll.fmi1_instantiateSlave.argtypes = ct.c_void_p,ct.c_char_p,ct.c_double,ct.c_bool,ct.c_bool,ct.c_void_p,ct.c_void_p,ct.c_void_p,ct.c_void_p, ct.c_bool,
        self.hdll.fmi1_initializeSlave.restype = ct.c_int
        self.hdll.fmi1_initializeSlave.argtypes = ct.c_void_p,ct.c_double,ct.c_bool,ct.c_double,
        self.hdll.fmi1_terminateSlave.restype = ct.c_int
        self.hdll.fmi1_terminateSlave.argtypes = ct.c_void_p,
        self.hdll.fmi1_resetSlave.restype = ct.c_int
        self.hdll.fmi1_resetSlave.argtypes = ct.c_void_p,
        self.hdll.fmi1_freeSlaveInstance.restype = None
        self.hdll.fmi1_freeSlaveInstance.argtypes = ct.c_void_p,
        self.hdll.fmi1_setRealInputDerivatives.restype = ct.c_int
        self.hdll.fmi1_setRealInputDerivatives.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint),ct.c_size_t,ct.POINTER(ct.c_int),ct.POINTER(ct.c_double),
        self.hdll.fmi1_getRealOutputDerivatives.restype = ct.c_int
        self.hdll.fmi1_getRealOutputDerivatives.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint),ct.c_size_t,ct.POINTER(ct.c_int),ct.POINTER(ct.c_double),
        self.hdll.fmi1_cancelStep.restype = ct.c_int
        self.hdll.fmi1_cancelStep.argtypes = ct.c_void_p,
        self.hdll.fmi1_doStep.restype = ct.c_int
        self.hdll.fmi1_doStep.argtypes = ct.c_void_p,ct.c_double,ct.c_double,ct.c_bool,
        self.hdll.fmi1_getStatus.restype = ct.c_int
        self.hdll.fmi1_getStatus.argtypes = ct.c_void_p,ct.c_int,ct.POINTER(ct.c_int),
        self.hdll.fmi1_getRealStatus.restype = ct.c_int
        self.hdll.fmi1_getRealStatus.argtypes = ct.c_void_p,ct.c_int,ct.POINTER(ct.c_double),
        self.hdll.fmi1_getIntegerStatus.restype = ct.c_int
        self.hdll.fmi1_getIntegerStatus.argtypes = ct.c_void_p,ct.c_int,ct.POINTER(ct.c_int),
        self.hdll.fmi1_getBooleanStatus.restype = ct.c_int
        self.hdll.fmi1_getBooleanStatus.argtypes = ct.c_void_p,ct.c_int,ct.POINTER(ct.c_bool),
        self.hdll.fmi1_getStringStatus.restype = ct.c_int
        self.hdll.fmi1_getStringStatus.argtypes = ct.c_void_p,ct.c_int,ct.POINTER(ct.c_char_p),
        self.hdll.fmi1_getModelTypesPlatform.restype = ct.c_char_p
        self.hdll.fmi1_getModelTypesPlatform.argtypes = ct.c_void_p,
        self.hdll.fmi1_instantiateModel.restype = ct.c_bool
        self.hdll.fmi1_instantiateModel.argtypes = ct.c_void_p,ct.c_void_p,ct.c_void_p,ct.c_bool,
        self.hdll.fmi1_freeModelInstance.restype = None
        self.hdll.fmi1_freeModelInstance.argtypes = ct.c_void_p,
        self.hdll.fmi1_setTime.restype = ct.c_int
        self.hdll.fmi1_setTime.argtypes = ct.c_void_p,ct.c_double,
        self.hdll.fmi1_setContinuousStates.restype = ct.c_int
        self.hdll.fmi1_setContinuousStates.argtypes = ct.c_void_p,ct.POINTER(ct.c_double),ct.c_size_t,
        self.hdll.fmi1_completedIntegratorStep.restype = ct.c_int
        self.hdll.fmi1_completedIntegratorStep.argtypes = ct.c_void_p,ct.c_bool,
        self.hdll.fmi1_initialize.restype = ct.c_int
        self.hdll.fmi1_initialize.argtypes = ct.c_void_p,ct.c_bool,ct.c_double,ct.POINTER(self.fmi1EventInfo),
        self.hdll.fmi1_getDerivatives.restype = ct.c_int
        self.hdll.fmi1_getDerivatives.argtypes = ct.c_void_p,ct.POINTER(ct.c_double),ct.c_int,
        self.hdll.fmi1_getEventIndicators.restype = ct.c_int
        self.hdll.fmi1_getEventIndicators.argtypes = ct.c_void_p,ct.POINTER(ct.c_double),ct.c_int,
        self.hdll.fmi1_eventUpdate.restype = ct.c_int
        self.hdll.fmi1_eventUpdate.argtypes = ct.c_void_p,ct.c_bool,ct.c_void_p,
        self.hdll.fmi1_getContinuousStates.restype = ct.c_int
        self.hdll.fmi1_getContinuousStates.argtypes = ct.c_void_p,ct.POINTER(ct.c_double),ct.c_int,
        self.hdll.fmi1_getNominalContinuousStates.restype = ct.c_int
        self.hdll.fmi1_getNominalContinuousStates.argtypes = ct.c_void_p,ct.POINTER(ct.c_double),ct.c_int,
        self.hdll.fmi1_getStateValueReferences.restype = ct.c_int
        self.hdll.fmi1_getStateValueReferences.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint),ct.c_int,
        self.hdll.fmi1_terminate.restype = ct.c_int
        self.hdll.fmi1_terminate.argtypes = ct.c_void_p,
    
    def translateFmiVersion(self, version):
        match version:
            case 0:
                return "fmiVersionUnknown"
            case 1:
                return "fmiVersion1"
            case 2:
                return "fmiVersion2"
            case 3:
                return "fmiVersion3"
        
    def translateFmi1Type(self, type):
        match type:
            case 0:
                return "fmi1ModelExchange"
            case 1:
                return "fmi1CoSimulationStandAlone"
            case 2:
                return "fmi1CoSimulationTool"
            case _:
                return ""    
        
    def fmi4c_getFmiVersion(self):
        return self.translateFmiVersion(self.hdll.fmi4c_getFmiVersion(self.fmu))
        
    def fmi4c_getErrorMessages(self):
        return self.hdll.fmi4c_getErrorMessages()
    
    def fmi4c_loadFmu(self, path, instanceName):
        self.fmu = self.hdll.fmi4c_loadFmu(path.encode(), instanceName.encode())
        return self.fmu != 0

    def fmi1_getType(self):
        return self.translateFmi1Type(self.hdll.fmi1_getType(self.fmu))
            
    def fmi1_getModelName(self):
        return self.hdll.fmi1_getModelName(self.fmu).decode()
        
    def fmi1_getModelIdentifier(self):
        return self.hdll.fmi1_getModelIdentifier(self.fmu).decode()

    def fmi1_getGuid(self):
        return self.hdll.fmi1_getGuid(self.fmu).decode()
        
    def fmi1_getDescription(self):
        ret = self.hdll.fmi1_getDescription(self.fmu)
        if ret is None:
            return ""
        else:
            return ret.decode()
        
        
    def fmi1_getAuthor(self):
        ret = self.hdll.fmi1_getAuthor(self.fmu)
        if ret is None:
            return ""
        else:
            return ret.decode()
        
    def fmi1_getGenerationTool(self):
        ret = self.hdll.fmi1_getGenerationTool(self.fmu)
        if ret is None:
            return ""
        else:
            return ret.decode()
        
    def fmi1_getGenerationDateAndTime(self):
        ret = self.hdll.fmi1_getGenerationDateAndTime(self.fmu)
        if ret is None:
            return ""
        else:
            return ret.decode()
        
    def fmi1_getNumberOfContinuousStates(self):
        return self.hdll.fmi1_getNumberOfContinuousStates(self.fmu)
    
    def fmi1_getNumberOfEventIndicators(self):
        return self.hdll.fmi1_getNumberOfEventIndicators(self.fmu)    

    def fmi1_defaultStartTimeDefined(self):
        return self.hdll.fmi1_defaultStartTimeDefined(self.fmu)    

    def fmi1_defaultStopTimeDefined(self):
        return self.hdll.fmi1_defaultStopTimeDefined(self.fmu)    

    def fmi1_defaultToleranceDefined(self):
        return self.hdll.fmi1_defaultToleranceDefined(self.fmu)    

    def fmi1_getDefaultStartTime(self):
        return self.hdll.fmi1_getDefaultStartTime(self.fmu)    
    
    def fmi1_getDefaultStopTime(self):
        return self.hdll.fmi1_getDefaultStopTime(self.fmu)    
 
    def fmi1_getDefaultTolerance(self):
        return self.hdll.fmi1_getDefaultTolerance(self.fmu)    
       
    def fmi1_getNumberOfVariables(self):
        return self.hdll.fmi1_getNumberOfVariables(self.fmu)  
        
    def fmi1_getVariableByIndex(self, i):
        return self.hdll.fmi1_getVariableByIndex(self.fmu, i)    
        
    def fmi1_getVariableByValueReference(self, vr):
        return self.hdll.fmi1_getVariableByValueReference(self.fmu, vr)    
        
    def fmi1_getVariableByName(self, name):
        return self.hdll.fmi1_getVariableByName(self.fmu, name)    

    def fmi1_getVariableName(self, var):
        return self.hdll.fmi1_getVariableName(var).decode()
        
    def fmi1_getVariableDescription(self, var):
        ret = self.hdll.fmi1_getVariableDescription(var)
        if ret is None:
            return ""
        else:
            return ret.decode()
         
    def fmi1_getVariableQuantity(self, var):
        ret = self.hdll.fmi1_getVariableQuantity(var)
        if ret is None:
            return ""
        else:
            return ret.decode()
       
    def fmi1_getVariableUnit(self, var):
        ret = self.hdll.fmi1_getVariableUnit(var)
        if ret is None:
            return ""
        else:
            return ret.decode()
        
    def fmi1_getVariableDisplayUnit(self, var):
        ret = self.hdll.fmi1_getVariableDisplayUnit(var)
        if ret is None:
            return ""
        else:
            return ret.decode()
    
    def fmi1_getVariableRelativeQUantity(self, var):
        return self.hdll.fmi1_getVariableRelativeQUantity(var)

    def fmi1_getVariableMin(self, var):
        return self.hdll.fmi1_getVariableMin(var)
        
    def fmi1_getVariableMax(self, var):
        return self.hdll.fmi1_getVariableMax(var)
        
    def fmi1_getVariableNominal(self, var):
        return self.hdll.fmi1_getVariableNominal(var)
        
    def fmi1_getNumberOfBaseUnits(self):
        return self.hdll.fmi1_getNumberOfBaseUnits(self.fmu)
        
    def fmi1_getBaseUnitByIndex(self, id):
        return self.hdll.fmi1_getBaseUnitByIndex(self.fmu, id)
        
    def fmi1_getBaseUnitUnit(self, unit):
        return self.hdll.fmi1_getBaseUnitUnit(unit).decode()
        
    def fmi1_getNumberOfDisplayUnits(self, unit):
        return self.hdll.fmi1_getNumberOfDisplayUnits(unit)
        
    def fmi1_getDisplayUnitUnitByIndex(self, unit, id):
        res = ct.c_char_p()
        dummy1 = ct.c_double()
        dummy2 = ct.c_double()
        self.hdll.fmi1_getDisplayUnitByIndex(unit, id, ct.byref(res), ct.byref(dummy1), ct.byref(dummy2))
        return res.value.decode()

    def fmi1_getVariableHasStartValue(self, var):
        return self.hdll.fmi1_getVariableHasStartValue(var);
    
    def fmi1_getVariableStartReal(self, var):
        return self.hdll.fmi1_getVariableStartReal(var);
    
    def fmi1_getVariableStartInteger(self, var):
        return self.hdll.fmi1_getVariableStartInteger(var);

    def fmi1_getVariableStartBoolean(self, var):
        return self.hdll.fmi1_getVariableStartBoolean(var);
        
    def fmi1_getVariableStartString(self, var):
        return self.hdll.fmi1_getVariableStartString(var);
        
    def fmi1_getVariableValueReference(self, var):
        return self.hdll.fmi1_getVariableValueReference(var);
        
    def fmi1_getVariableCausality(self, var):
        return self.hdll.fmi1_getVariableCausality(var);
    
    def fmi1_getVariableVariability(self, var):
        return self.hdll.fmi1_getVariableVariability(var);
    
    def fmi1_getAlias(self, var):
        return self.hdll.fmi1_getAlias(var);

    def fmi1_getVariableIsFixed(self, var):
        return self.hdll.fmi1_getAlias(var);
        
    def fmi1_getVariableDataType(self, var):
        return self.hdll.fmi1_getAlias(var);

    def fmi1_getTypesPlatform(self):
        return self.hdll.fmi1_getTypesPlatform(self.fmu).decode();

    def fmi1_getVersion(self):
        return self.hdll.fmi1_getVersion(self.fmu).decode();

    def fmi1_setDebugLogging(self, loggingOn):
        return self.hdll.fmi1_setDebugLogging(self.fmu, loggingOn);

    def fmi1_getReal(self, valueReferences, nValueReferences):
        uint_array_type = ct.c_uint * nValueReferences
        double_array_type = ct.c_double * nValueReferences
        valueReferencesArray = uint_array_type(1,2)
        valuesArray = double_array_type()
        success = self.hdll.fmi1_getReal(self.fmu, valueReferencesArray, nValueReferences, valuesArray)
        return [success, list(valuesArray)]
        
    def fmi1_getInteger(self, valueReferences, nValueReferences):
        uint_array_type = ct.c_uint * nValueReferences
        double_array_type = ct.c_int * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = double_array_type(*[1]*nValueReferences)
        success = self.hdll.fmi1_getInteger(self.fmu, valueReferencesArray, nValueReferences, valuesArray)
        return [success, list(valuesArray)]   
         
    def fmi1_getBoolean(self, valueReferences, nValueReferences):
        uint_array_type = ct.c_uint * nValueReferences
        double_array_type = ct.c_bool * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = double_array_type(*[1]*nValueReferences)
        success = self.hdll.fmi1_getBoolean(self.fmu, valueReferencesArray, nValueReferences, valuesArray)
        return [success, list(valuesArray)]     

    def fmi1_getString(self, valueReferences, nValueReferences):
        uint_array_type = ct.c_uint * nValueReferences
        double_array_type = ct.c_char_p * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = double_array_type(*[1]*nValueReferences)
        success = self.hdll.fmi1_getString(self.fmu, valueReferencesArray, nValueReferences, valuesArray)
        return [success, list(valuesArray)]     

    def fmi1_setReal(self, valueReferences, nValueReferences, values):
        uint_array_type = ct.c_uint * nValueReferences
        double_array_type = ct.c_double * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = double_array_type(*values)
        return self.hdll.fmi1_setReal(self.fmu, valueReferencesArray, nValueReferences, valuesArray)

    def fmi1_setInteger(self, valueReferences, nValueReferences, values):
        uint_array_type = ct.c_uint * nValueReferences
        double_array_type = ct.c_int * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = double_array_type(*values)
        return self.hdll.fmi1_setInteger(self.fmu, valueReferencesArray, nValueReferences, valuesArray)
        
    def fmi1_setBoolean(self, valueReferences, nValueReferences, values):
        uint_array_type = ct.c_uint * nValueReferences
        double_array_type = ct.c_bool * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = double_array_type(*values)
        return self.hdll.fmi1_setBoolean(self.fmu, valueReferencesArray, nValueReferences, valuesArray)
        
    def fmi1_setString(self, valueReferences, nValueReferences, values):
        uint_array_type = ct.c_uint * nValueReferences
        double_array_type = ct.c_char_p * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = double_array_type(*values)
        return self.hdll.fmi1_setString(self.fmu, valueReferencesArray, nValueReferences, valuesArray)

    def fmi1_instantiateSlave(self, mimeType, timeOut, visible, interactive, loggingOn):
        return self.hdll.fmi1_instantiateSlave(self.fmu, mimeType, timeOut, visible, interactive, None, None, None, None, loggingOn)
        
    def fmi1_initializeSlave(self, startTime, stopTimeDefined, stopTime):
        return self.hdll.fmi1_initializeSlave(self.fmu, startTime, stopTimeDefined, stopTime)
    
    def fmi1_resetSlave(self):
        return self.hdll.fmi1_resetSlave(self.fmu)
        
    def fmi1_terminateSlave(self):
        return self.hdll.fmi1_terminateSlave(self.fmu)

    def fmi1_freeSlaveInstance(self):
        self.hdll.fmi1_freeSlaveInstance(self.fmu)    

    def fmi1_setRealInputDerivatives(self, valueReferences, nValueReferences, orders, values):
        uint_array_type = ct.c_uint * nValueReferences
        int_array_type = ct.c_int * nValueReferences
        double_array_type = ct.c_double * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        ordersArray = int_array_type(*orders)
        valuesArray = double_array_type(*values)
        return self.hdll.fmi1_setRealInputDerivatives(self.fmu, valueReferencesArray, nValueReferences, ordersArray, valuesArray)
 
    def fmi1_getRealOutputDerivatives(self, valueReferences, nValueReferences, orders):
        uint_array_type = ct.c_uint * nValueReferences
        int_array_type = ct.c_int * nValueReferences
        double_array_type = ct.c_double * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        ordersArray = int_array_type(*orders)
        valuesArray = double_array_type(*[1]*nValueReferences)
        success = self.hdll.fmi1_getRealOutputDerivatives(self.fmu, valueReferencesArray, nValueReferences, ordersArray, valuesArray)
        return [success, list(valuesArray)]       

    def fmi1_cancelStep(self):
        return self.hdll.fmi1_cancelStep(self.fmu)

    def fmi1_doStep(self, currentCommunicationPoint, communicationStepSize, newStep):
        return self.hdll.fmi1_doStep(self.fmu, currentCommunicationPoint, communicationStepSize, newStep)

    def fmi1_getStatus(self, statusKind):
        value = ct.c_int()
        success = self.hdll.fmi1_getStatus(self.fmu, statusKind, ct.byref(value))
        return [success, value]
        
    def fmi1_getRealStatus(self, statusKind):
        value = ct.c_double()
        success = self.hdll.fmi1_getRealStatus(self.fmu, statusKind, ct.byref(value))
        return [success, value]
        
    def fmi1_getIntegerStatus(self, statusKind):
        value = ct.c_int()
        success = self.hdll.fmi1_getIntegerStatus(self.fmu, statusKind, ct.byref(value))
        return [success, value]
        
    def fmi1_getBooleanStatus(self, statusKind):
        value = ct.c_bool()
        success = self.hdll.fmi1_getBooleanStatus(self.fmu, statusKind, ct.byref(value))
        return [success, value]
        
    def fmi1_getStringStatus(self, statusKind):
        value = ct.c_char_p()
        success = self.hdll.fmi1_getStringStatus(self.fmu, statusKind, ct.byref(value))
        return [success, value.decode()]

    def fmi1_getModelTypesPlatform(self):
        return self.hdll.fmi1_getModelTypesPlatform(self.fmu).decode()
        
    def fmi1_instantiateModel(self, loggingOn):
        return self.hdll.fmi1_instantiateModel(self.fmu, None, None, None, loggingOn)
        
    def fmi1_freeModelInstance(self):
        return self.hdll.fmi1_freeModelInstance(self.fmu)

    def fmi1_setTime(self, time):
        return self.hdll.fmi1_setTime(self.fmu, time)

    def fmi1_setContinuousStates(self, continuousStates, nContinuousStates):
        double_array_type = ct.c_double * nContinuousStates
        continuousStatesArray = double_array_type(*continuousStates)
        return self.hdll.fmi1_setContinuousStates(self.fmu, continuousStatesArray, nContinuousStates)
        
    def fmi1_completedIntegratorStep(self, callEventUpdate):
        return self.hdll.fmi1_completedIntegratorStep(self.fmu, callEventUpdate)
       
    def fmi1_initialize(self, toleranceControlled, relativeTolerance, eventInfo):
        return self.hdll.fmi1_initialize(self.fmu, toleranceControlled, relativeTolerance, eventInfo)

    def fmi1_getDerivatives(self, derivatives, nDerivatives):
        double_array_type = ct.c_double * nDerivatives
        derivativesArray = double_array_type(*derivatives)
        success = self.hdll.fmi1_getDerivatives(self.fmu, derivativesArray, nDerivatives)
        return [success, list(derivativesArray)]  
         
    def fmi1_getEventIndicators(self, indicators, nIndicators):
        double_array_type = ct.c_double * nIndicators
        indicatorsArray = double_array_type(*indicators)
        success = self.hdll.fmi1_getEventIndicators(self.fmu, indicatorsArray, nIndicators)       
        return [success, list(indicatorsArray)]

    def fmi1_eventUpdate(self, intermediateResults, eventInfo):
        return self.hdll.fmi1_eventUpdate(self.fmu, intermediateResults, eventInfo)
          
    def fmi1_getContinuousStates(self, states, nStates):
        double_array_type = ct.c_double * nStates
        statesArray = double_array_type(*states)
        success = self.hdll.fmi1_getContinuousStates(self.fmu, statesArray, nStates)       
        return [success, list(statesArray)]       
          
    def fmi1_getNominalContinuousStates(self, nominals, nNominals):
        double_array_type = ct.c_double * nNominals
        nominalsArray = double_array_type(*nominals)
        success = self.hdll.fmi1_getNominalContinuousStates(self.fmu, nominalsArray, nNominals)       
        return [success, list(nominalsArray)]

          
    def fmi1_getStateValueReferences(self, nValueReferences):
        uint_array_type = ct.c_uint * nValueReferences
        valueReferencesArray = uint_array_type(*[1]*nValueReferences)
        success = self.hdll.fmi1_getStateValueReferences(self.fmu, valueReferencesArray, nValueReferences)       
        return [success, list(valueReferencesArray)]

    def fmi1_terminate(self):
        return self.hdll.fmi1_terminate(self.fmu)