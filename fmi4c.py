import ctypes as ct

class fmi4c:

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
        self.hdll.fmi1_getVariableValueReference.restype = ct.c_long
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
