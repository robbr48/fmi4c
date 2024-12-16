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
        self.hdll.fmi4c_getFmiVersion.restype = ct.c_int
        self.hdll.fmi4c_getFmiVersion.argtypes = ct.c_void_p,
        self.hdll.fmi4c_loadFmu.restype = ct.c_void_p
        self.hdll.fmi4c_loadFmu.argtypes = ct.c_char_p, ct.c_char_p,
        self.hdll.fmi4c_freeFmu.restype = None
        self.hdll.fmi4c_freeFmu.argtypes = ct.c_void_p,
        
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

        self.hdll.fmi2_defaultStartTimeDefined.restype = ct.c_bool
        self.hdll.fmi2_defaultStartTimeDefined.argtypes =ct.c_void_p,
        self.hdll.fmi2_defaultStopTimeDefined.restype = ct.c_bool
        self.hdll.fmi2_defaultStopTimeDefined.argtypes =ct.c_void_p,
        self.hdll.fmi2_defaultToleranceDefined.restype = ct.c_bool
        self.hdll.fmi2_defaultToleranceDefined.argtypes =ct.c_void_p,
        self.hdll.fmi2_defaultStepSizeDefined.restype = ct.c_bool
        self.hdll.fmi2_defaultStepSizeDefined.argtypes =ct.c_void_p,
        self.hdll.fmi2_getDefaultStartTime.restype = ct.c_double
        self.hdll.fmi2_getDefaultStartTime.argtypes =ct.c_void_p,
        self.hdll.fmi2_getDefaultStopTime.restype = ct.c_double
        self.hdll.fmi2_getDefaultStopTime.argtypes =ct.c_void_p,
        self.hdll.fmi2_getDefaultTolerance.restype = ct.c_double
        self.hdll.fmi2_getDefaultTolerance.argtypes =ct.c_void_p,
        self.hdll.fmi2_getDefaultStepSize.restype = ct.c_double
        self.hdll.fmi2_getDefaultStepSize.argtypes =ct.c_void_p,

        self.hdll.fmi2_getNumberOfVariables.restype = ct.c_int
        self.hdll.fmi2_getNumberOfVariables.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableByIndex.restype =ct.c_void_p
        self.hdll.fmi2_getVariableByIndex.argtypes =ct.c_void_p,ct.c_int,
        self.hdll.fmi2_getVariableByValueReference.restype =ct.c_void_p
        self.hdll.fmi2_getVariableByValueReference.argtypes =ct.c_void_p, ct.c_long,
        self.hdll.fmi2_getVariableByName.restype =ct.c_void_p
        self.hdll.fmi2_getVariableByName.argtypes =ct.c_void_p, ct.c_char_p,
        self.hdll.fmi2_getVariableName.restype = ct.c_char_p
        self.hdll.fmi2_getVariableName.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableDescription.restype = ct.c_char_p
        self.hdll.fmi2_getVariableDescription.argtypes = ct.c_void_p,
        self.hdll.fmi2_getFmiVersion.restype = ct.c_char_p
        self.hdll.fmi2_getFmiVersion.argtypes = ct.c_void_p,
        self.hdll.fmi2_getAuthor.restype = ct.c_char_p
        self.hdll.fmi2_getAuthor.argtypes =ct.c_void_p,
        self.hdll.fmi2_getModelName.restype = ct.c_char_p
        self.hdll.fmi2_getModelName.argtypes =ct.c_void_p,
        self.hdll.fmi2_getModelDescription.restype = ct.c_char_p
        self.hdll.fmi2_getModelDescription.argtypes =ct.c_void_p,
        self.hdll.fmi2_getCopyright.restype = ct.c_char_p
        self.hdll.fmi2_getCopyright.argtypes =ct.c_void_p,
        self.hdll.fmi2_getLicense.restype = ct.c_char_p
        self.hdll.fmi2_getLicense.argtypes =ct.c_void_p,
        self.hdll.fmi2_getGenerationTool.restype = ct.c_char_p
        self.hdll.fmi2_getGenerationTool.argtypes =ct.c_void_p,
        self.hdll.fmi2_getGenerationDateAndTime.restype = ct.c_char_p
        self.hdll.fmi2_getGenerationDateAndTime.argtypes =ct.c_void_p,
        self.hdll.fmi2_getVariableNamingConvention.restype = ct.c_char_p
        self.hdll.fmi2_getVariableNamingConvention.argtypes =ct.c_void_p,
        self.hdll.fmi2_getVariableDerivativeIndex.restype = ct.c_int
        self.hdll.fmi2_getVariableDerivativeIndex.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableQuantity.restype = ct.c_char_p
        self.hdll.fmi2_getVariableQuantity.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableUnit.restype = ct.c_char_p
        self.hdll.fmi2_getVariableUnit.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableDisplayUnit.restype = ct.c_char_p
        self.hdll.fmi2_getVariableDisplayUnit.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableRelativeQuantity.restype = ct.c_bool
        self.hdll.fmi2_getVariableRelativeQuantity.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableMin.restype = ct.c_double
        self.hdll.fmi2_getVariableMin.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableMax.restype = ct.c_double
        self.hdll.fmi2_getVariableMax.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableNominal.restype = ct.c_double
        self.hdll.fmi2_getVariableNominal.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableUnbounded.restype = ct.c_bool
        self.hdll.fmi2_getVariableUnbounded.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableHasStartValue.restype = ct.c_bool
        self.hdll.fmi2_getVariableHasStartValue.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableStartReal.restype = ct.c_double
        self.hdll.fmi2_getVariableStartReal.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableStartInteger.restype = ct.c_int
        self.hdll.fmi2_getVariableStartInteger.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableStartBoolean.restype = ct.c_bool
        self.hdll.fmi2_getVariableStartBoolean.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableStartString.restype = ct.c_char_p
        self.hdll.fmi2_getVariableStartString.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableValueReference.restype = ct.c_long
        self.hdll.fmi2_getVariableValueReference.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableCausality.restype = ct.c_int
        self.hdll.fmi2_getVariableCausality.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableVariability.restype = ct.c_int
        self.hdll.fmi2_getVariableVariability.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableInitial.restype = ct.c_int
        self.hdll.fmi2_getVariableInitial.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableCanHandleMultipleSetPerTimeInstant.restype = ct.c_bool
        self.hdll.fmi2_getVariableCanHandleMultipleSetPerTimeInstant.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVariableDataType.restype = ct.c_int
        self.hdll.fmi2_getVariableDataType.argtypes = ct.c_void_p,

        self.hdll.fmi2_getTypesPlatform.restype = ct.c_char_p
        self.hdll.fmi2_getTypesPlatform.argtypes = ct.c_void_p,
        self.hdll.fmi2_getVersion.restype = ct.c_char_p
        self.hdll.fmi2_getVersion.argtypes = ct.c_void_p,
        self.hdll.fmi2_setDebugLogging.restype = ct.c_int
        self.hdll.fmi2_setDebugLogging.argtypes = ct.c_void_p, ct.c_bool, ct.c_size_t, ct.c_char_p
        self.hdll.fmi2_getGuid.restype = ct.c_char_p
        self.hdll.fmi2_getGuid.argtypes =ct.c_void_p,

        self.hdll.fmi2cs_getModelIdentifier.restype = ct.c_char_p
        self.hdll.fmi2cs_getModelIdentifier.argtypes = ct.c_void_p,
        self.hdll.fmi2cs_getCanHandleVariableCommunicationStepSize.restype = ct.c_bool
        self.hdll.fmi2cs_getCanHandleVariableCommunicationStepSize.argtypes = ct.c_void_p,
        self.hdll.fmi2cs_getCanInterpolateInputs.restype = ct.c_bool
        self.hdll.fmi2cs_getCanInterpolateInputs.argtypes = ct.c_void_p,
        self.hdll.fmi2cs_getMaxOutputDerivativeOrder.restype = ct.c_int
        self.hdll.fmi2cs_getMaxOutputDerivativeOrder.argtypes = ct.c_void_p,
        self.hdll.fmi2cs_getCanRunAsynchronuously.restype = ct.c_bool
        self.hdll.fmi2cs_getCanRunAsynchronuously.argtypes = ct.c_void_p,
        self.hdll.fmi2cs_getNeedsExecutionTool.restype = ct.c_bool
        self.hdll.fmi2cs_getNeedsExecutionTool.argtypes = ct.c_void_p,
        self.hdll.fmi2cs_getCanBeInstantiatedOnlyOncePerProcess.restype = ct.c_bool
        self.hdll.fmi2cs_getCanBeInstantiatedOnlyOncePerProcess.argtypes = ct.c_void_p,
        self.hdll.fmi2cs_getCanNotUseMemoryManagementFunctions.restype = ct.c_bool
        self.hdll.fmi2cs_getCanNotUseMemoryManagementFunctions.argtypes = ct.c_void_p,
        self.hdll.fmi2cs_getCanGetAndSetFMUState.restype = ct.c_bool
        self.hdll.fmi2cs_getCanGetAndSetFMUState.argtypes = ct.c_void_p,
        self.hdll.fmi2cs_getCanSerializeFMUState.restype = ct.c_bool
        self.hdll.fmi2cs_getCanSerializeFMUState.argtypes = ct.c_void_p,
        self.hdll.fmi2cs_getProvidesDirectionalDerivative.restype = ct.c_bool
        self.hdll.fmi2cs_getProvidesDirectionalDerivative.argtypes = ct.c_void_p,

        self.hdll.fmi2me_getModelIdentifier.restype = ct.c_char_p
        self.hdll.fmi2me_getModelIdentifier.argtypes = ct.c_void_p,
        self.hdll.fmi2me_getCompletedIntegratorStepNotNeeded.restype = ct.c_bool
        self.hdll.fmi2me_getCompletedIntegratorStepNotNeeded.argtypes = ct.c_void_p,
        self.hdll.fmi2me_getNeedsExecutionTool.restype = ct.c_bool
        self.hdll.fmi2me_getNeedsExecutionTool.argtypes = ct.c_void_p,
        self.hdll.fmi2me_getCanBeInstantiatedOnlyOncePerProcess.restype = ct.c_bool
        self.hdll.fmi2me_getCanBeInstantiatedOnlyOncePerProcess.argtypes = ct.c_void_p,
        self.hdll.fmi2me_getCanNotUseMemoryManagementFunctions.restype = ct.c_bool
        self.hdll.fmi2me_getCanNotUseMemoryManagementFunctions.argtypes = ct.c_void_p,
        self.hdll.fmi2me_getCanGetAndSetFMUState.restype = ct.c_bool
        self.hdll.fmi2me_getCanGetAndSetFMUState.argtypes = ct.c_void_p,
        self.hdll.fmi2me_getCanSerializeFMUState.restype = ct.c_bool
        self.hdll.fmi2me_getCanSerializeFMUState.argtypes = ct.c_void_p,
        self.hdll.fmi2me_getProvidesDirectionalDerivative.restype = ct.c_bool
        self.hdll.fmi2me_getProvidesDirectionalDerivative.argtypes = ct.c_void_p,

        self.hdll.fmi2_getNumberOfContinuousStates.restype = ct.c_int
        self.hdll.fmi2_getNumberOfContinuousStates.argtypes =ct.c_void_p,
        self.hdll.fmi2_getNumberOfEventIndicators.restype = ct.c_int
        self.hdll.fmi2_getNumberOfEventIndicators.argtypes =ct.c_void_p,
        self.hdll.fmi2_getSupportsCoSimulation.restype = ct.c_bool
        self.hdll.fmi2_getSupportsCoSimulation.argtypes =ct.c_void_p,
        self.hdll.fmi2_getSupportsModelExchange.restype = ct.c_bool
        self.hdll.fmi2_getSupportsModelExchange.argtypes =ct.c_void_p,

        self.hdll.fmi2_getNumberOfModelStructureOutputs.restype = ct.c_int;
        self.hdll.fmi2_getNumberOfModelStructureOutputs.argtypes = ct.c_void_p,
        self.hdll.fmi2_getNumberOfModelStructureDerivatives.restype = ct.c_int;
        self.hdll.fmi2_getNumberOfModelStructureDerivatives.argtypes = ct.c_void_p,
        self.hdll.fmi2_getNumberOfModelStructureInitialUnknowns.restype = ct.c_int;
        self.hdll.fmi2_getNumberOfModelStructureInitialUnknowns.argtypes = ct.c_void_p,
        self.hdll.fmi2_getModelStructureOutput.restype =  ct.c_void_p
        self.hdll.fmi2_getModelStructureOutput.argtypes = ct.c_void_p,ct.c_size_t,
        self.hdll.fmi2_getModelStructureDerivative.restype =  ct.c_void_p
        self.hdll.fmi2_getModelStructureDerivative.argtypes = ct.c_void_p,ct.c_size_t,
        self.hdll.fmi2_getModelStructureInitialUnknown.restype =  ct.c_void_p
        self.hdll.fmi2_getModelStructureInitialUnknown.argtypes = ct.c_void_p,ct.c_size_t,
        self.hdll.fmi2_getModelStructureIndex.restype = ct.c_int
        self.hdll.fmi2_getModelStructureIndex.argtypes = ct.c_void_p,
        self.hdll.fmi2_getModelStructureNumberOfDependencies.restype = ct.c_int
        self.hdll.fmi2_getModelStructureNumberOfDependencies.argtypes = ct.c_void_p,
        self.hdll.fmi2_getModelStructureDependencyKindsDefined.restype = ct.c_bool
        self.hdll.fmi2_getModelStructureDependencyKindsDefined.argtypes = ct.c_void_p,
        self.hdll.fmi2_getModelStructureDependencies.restype = None
        self.hdll.fmi2_getModelStructureDependencies.argtypes = ct.c_void_p, ct.POINTER(ct.c_int), ct.c_size_t,
        self.hdll.fmi2_getModelStructureDependencyKinds.restype = None
        self.hdll.fmi2_getModelStructureDependencyKinds.argtypes = ct.c_void_p, ct.POINTER(ct.c_int), ct.c_size_t,

        self.hdll.fmi2_instantiate.restype = ct.c_bool
        self.hdll.fmi2_instantiate.argtypes =ct.c_void_p, ct.c_int, ct.c_void_p, ct.c_void_p, ct.c_void_p, ct.c_void_p, ct.c_void_p, ct.c_bool, ct.c_bool,
        self.hdll.fmi2_freeInstance.restype = None
        self.hdll.fmi2_freeInstance.argtypes = ct.c_void_p,

        self.hdll.fmi2_setupExperiment.restype = ct.c_int
        self.hdll.fmi2_setupExperiment.argtypes = ct.c_void_p, ct.c_bool, ct.c_double, ct.c_double, ct.c_bool, ct.c_double,
        self.hdll.fmi2_enterInitializationMode.restype = ct.c_int
        self.hdll.fmi2_enterInitializationMode.argtypes = ct.c_void_p,
        self.hdll.fmi2_exitInitializationMode.restype = ct.c_int
        self.hdll.fmi2_exitInitializationMode.argtypes = ct.c_void_p,
        self.hdll.fmi2_terminate.restype = ct.c_int
        self.hdll.fmi2_terminate.argtypes = ct.c_void_p,
        self.hdll.fmi2_reset.restype = ct.c_int
        self.hdll.fmi2_reset.argtypes = ct.c_void_p,

        self.hdll.fmi2_getNumberOfUnits.restype = ct.c_int
        self.hdll.fmi2_getNumberOfUnits.argtypes =ct.c_void_p,

        self.hdll.fmi2_getUnitByIndex.restype = ct.c_void_p
        self.hdll.fmi2_getUnitByIndex.argtypes = ct.c_void_p, ct.c_int,
        self.hdll.fmi2_getUnitName.restype = ct.c_char_p
        self.hdll.fmi2_getUnitName.argtypes = ct.c_void_p,
        self.hdll.fmi2_hasBaseUnit.restype = ct.c_bool
        self.hdll.fmi2_hasBaseUnit.argtypes = ct.c_void_p,
        self.hdll.fmi2_getBaseUnit.restype = None
        self.hdll.fmi2_getBaseUnit.argtypes = ct.c_void_p,ct.POINTER(ct.c_double),ct.POINTER(ct.c_double),ct.POINTER(ct.c_int),ct.POINTER(ct.c_int),ct.POINTER(ct.c_int),ct.POINTER(ct.c_int),ct.POINTER(ct.c_int),ct.POINTER(ct.c_int),ct.POINTER(ct.c_int),ct.POINTER(ct.c_int),
        self.hdll.fmi2_getNumberOfDisplayUnits.restype = ct.c_int
        self.hdll.fmi2_getNumberOfDisplayUnits.argtypes = ct.c_void_p,
        self.hdll.fmi2_getDisplayUnitByIndex.restype = None
        self.hdll.fmi2_getDisplayUnitByIndex.argtypes = ct.c_void_p, ct.c_int, ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_double),ct.POINTER(ct.c_double),

        self.hdll.fmi2_getReal.restype = ct.c_int
        self.hdll.fmi2_getReal.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t,ct.POINTER(ct.c_double),
        self.hdll.fmi2_getInteger.restype = ct.c_int
        self.hdll.fmi2_getInteger.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t,ct.POINTER(ct.c_int),
        self.hdll.fmi2_getBoolean.restype = ct.c_int
        self.hdll.fmi2_getBoolean.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t,ct.POINTER(ct.c_bool),
        self.hdll.fmi2_getString.restype = ct.c_int
        self.hdll.fmi2_getString.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t,ct.POINTER(ct.c_char_p),

        self.hdll.fmi2_setReal.restype = ct.c_int
        self.hdll.fmi2_setReal.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t,ct.POINTER(ct.c_double),
        self.hdll.fmi2_setInteger.restype = ct.c_int
        self.hdll.fmi2_setInteger.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t,ct.POINTER(ct.c_int),
        self.hdll.fmi2_setBoolean.restype = ct.c_int
        self.hdll.fmi2_setBoolean.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t,ct.POINTER(ct.c_bool),
        self.hdll.fmi2_setString.restype = ct.c_int
        self.hdll.fmi2_setString.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t,ct.POINTER(ct.c_char_p),

        self.hdll.fmi2_getFMUstate.restype = ct.c_int
        self.hdll.fmi2_getFMUstate.argtypes = ct.c_void_p,ct.c_void_p,
        self.hdll.fmi2_setFMUstate.restype = ct.c_int
        self.hdll.fmi2_setFMUstate.argtypes = ct.c_void_p,ct.c_void_p,
        self.hdll.fmi2_freeFMUstate.restype = ct.c_int
        self.hdll.fmi2_freeFMUstate.argtypes = ct.c_void_p,ct.c_void_p,
        self.hdll.fmi2_serializedFMUstateSize.restype = ct.c_int
        self.hdll.fmi2_serializedFMUstateSize.argtypes = ct.c_void_p,ct.c_void_p,ct.POINTER(ct.c_size_t),
        self.hdll.fmi2_serializeFMUstate.restype = ct.c_int
        self.hdll.fmi2_serializeFMUstate.argtypes = ct.c_void_p,ct.c_void_p,ct.POINTER(ct.c_char),ct.c_size_t,
        self.hdll.fmi2_deSerializeFMUstate.restype = ct.c_int
        self.hdll.fmi2_deSerializeFMUstate.argtypes = ct.c_void_p,ct.POINTER(ct.c_char),ct.c_size_t,ct.c_void_p,

        self.hdll.fmi2_getDirectionalDerivative.restype = ct.c_int
        self.hdll.fmi2_getDirectionalDerivative.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint),ct.c_size_t,ct.POINTER(ct.c_uint),ct.c_size_t,ct.c_double,ct.c_double,

        self.hdll.fmi2_enterEventMode.restype = ct.c_int
        self.hdll.fmi2_enterEventMode.argtypes = ct.c_void_p,
        self.hdll.fmi2_newDiscreteStates.restype = ct.c_int
        self.hdll.fmi2_newDiscreteStates.argtypes = ct.c_void_p,ct.c_void_p,
        self.hdll.fmi2_enterContinuousTimeMode.restype = ct.c_int
        self.hdll.fmi2_enterContinuousTimeMode.argtypes = ct.c_void_p,
        self.hdll.fmi2_completedIntegratorStep.restype = ct.c_int
        self.hdll.fmi2_completedIntegratorStep.argtypes = ct.c_void_p,ct.c_bool,ct.POINTER(ct.c_bool),ct.POINTER(ct.c_bool),

        self.hdll.fmi2_setTime.restype = ct.c_int
        self.hdll.fmi2_setTime.argtypes = ct.c_void_p,ct.c_double,
        self.hdll.fmi2_setContinuousStates.restype = ct.c_int
        self.hdll.fmi2_setContinuousStates.argtypes = ct.c_void_p,ct.POINTER(ct.c_double),ct.c_size_t,

        self.hdll.fmi2_getDerivatives.restype = ct.c_int
        self.hdll.fmi2_getDerivatives.argtypes = ct.c_void_p,ct.POINTER(ct.c_double),ct.c_size_t,

        self.hdll.fmi2_getEventIndicators.restype = ct.c_int
        self.hdll.fmi2_getEventIndicators.argtypes = ct.c_void_p,ct.POINTER(ct.c_double),ct.c_size_t,

        self.hdll.fmi2_getContinuousStates.restype = ct.c_int
        self.hdll.fmi2_getContinuousStates.argtypes = ct.c_void_p,ct.POINTER(ct.c_double),ct.c_size_t,

        self.hdll.fmi2_getNominalsOfContinuousStates.restype = ct.c_int
        self.hdll.fmi2_getNominalsOfContinuousStates.argtypes = ct.c_void_p,ct.POINTER(ct.c_double),ct.c_size_t,

        self.hdll.fmi2_setRealInputDerivatives.restype = ct.c_int
        self.hdll.fmi2_setRealInputDerivatives.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint),ct.c_size_t,ct.POINTER(ct.c_int),ct.POINTER(ct.c_double),

        self.hdll.fmi2_getRealOutputDerivatives.restype = ct.c_int
        self.hdll.fmi2_getRealOutputDerivatives.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint),ct.c_size_t,ct.POINTER(ct.c_int),ct.POINTER(ct.c_double),

        self.hdll.fmi2_doStep.restype = ct.c_int
        self.hdll.fmi2_doStep.argtypes = ct.c_void_p,ct.c_double,ct.c_double,ct.c_bool,
        self.hdll.fmi2_cancelStep.restype = ct.c_int
        self.hdll.fmi2_cancelStep.argtypes = ct.c_void_p,

        self.hdll.fmi2_getStatus.restype = ct.c_int
        self.hdll.fmi2_getStatus.argtypes = ct.c_void_p, ct.c_int, ct.POINTER(ct.c_int)
        self.hdll.fmi2_getRealStatus.restype = ct.c_int
        self.hdll.fmi2_getRealStatus.argtypes = ct.c_void_p, ct.c_int, ct.POINTER(ct.c_double),
        self.hdll.fmi2_getIntegerStatus.restype = ct.c_int
        self.hdll.fmi2_getIntegerStatus.argtypes = ct.c_void_p, ct.c_int, ct.POINTER(ct.c_int)
        self.hdll.fmi2_getBooleanStatus.restype = ct.c_int
        self.hdll.fmi2_getBooleanStatus.argtypes = ct.c_void_p, ct.c_int, ct.POINTER(ct.c_bool)
        self.hdll.fmi2_getStringStatus.restype = ct.c_int
        self.hdll.fmi2_getStringStatus.argtypes = ct.c_void_p, ct.c_int,ct.POINTER(ct.c_char_p)

        self.hdll.fmi3_modelName.restype = ct.c_char_p 
        self.hdll.fmi3_modelName.argtypes = ct.c_void_p,
        self.hdll.fmi3_instantiationToken.restype = ct.c_char_p 
        self.hdll.fmi3_instantiationToken.argtypes = ct.c_void_p,
        self.hdll.fmi3_description.restype = ct.c_char_p 
        self.hdll.fmi3_description.argtypes = ct.c_void_p,
        self.hdll.fmi3_author.restype = ct.c_char_p 
        self.hdll.fmi3_author.argtypes = ct.c_void_p,
        self.hdll.fmi3_version.restype = ct.c_char_p 
        self.hdll.fmi3_version.argtypes = ct.c_void_p,
        self.hdll.fmi3_copyright.restype = ct.c_char_p 
        self.hdll.fmi3_copyright.argtypes = ct.c_void_p,
        self.hdll.fmi3_license.restype = ct.c_char_p 
        self.hdll.fmi3_license.argtypes = ct.c_void_p,
        self.hdll.fmi3_generationTool.restype = ct.c_char_p 
        self.hdll.fmi3_generationTool.argtypes = ct.c_void_p,
        self.hdll.fmi3_generationDateAndTime.restype = ct.c_char_p 
        self.hdll.fmi3_generationDateAndTime.argtypes = ct.c_void_p,
        self.hdll.fmi3_variableNamingConvention.restype = ct.c_char_p 
        self.hdll.fmi3_variableNamingConvention.argtypes = ct.c_void_p,
        self.hdll.fmi3_supportsModelExchange.restype = ct.c_bool 
        self.hdll.fmi3_supportsModelExchange.argtypes = ct.c_void_p,
        self.hdll.fmi3_supportsScheduledExecution.restype = ct.c_bool  
        self.hdll.fmi3_supportsScheduledExecution.argtypes = ct.c_void_p,
        self.hdll.fmi3_supportsCoSimulation.restype = ct.c_bool  
        self.hdll.fmi3_supportsCoSimulation.argtypes = ct.c_void_p,
        self.hdll.fmi3_defaultStartTimeDefined.restype = ct.c_bool  
        self.hdll.fmi3_defaultStartTimeDefined.argtypes = ct.c_void_p,
        self.hdll.fmi3_defaultStopTimeDefined.restype = ct.c_bool  
        self.hdll.fmi3_defaultStopTimeDefined.argtypes = ct.c_void_p,
        self.hdll.fmi3_defaultToleranceDefined.restype = ct.c_bool  
        self.hdll.fmi3_defaultToleranceDefined.argtypes = ct.c_void_p,
        self.hdll.fmi3_defaultStepSizeDefined.restype = ct.c_bool  
        self.hdll.fmi3_defaultStepSizeDefined.argtypes = ct.c_void_p,
        self.hdll.fmi3_getDefaultStartTime.restype = ct.c_double 
        self.hdll.fmi3_getDefaultStartTime.argtypes = ct.c_void_p,
        self.hdll.fmi3_getDefaultStopTime.restype = ct.c_double 
        self.hdll.fmi3_getDefaultStopTime.argtypes = ct.c_void_p,
        self.hdll.fmi3_getDefaultTolerance.restype = ct.c_double 
        self.hdll.fmi3_getDefaultTolerance.argtypes = ct.c_void_p,
        self.hdll.fmi3_getDefaultStepSize.restype = ct.c_double 
        self.hdll.fmi3_getDefaultStepSize.argtypes = ct.c_void_p,

        self.hdll.fmi3_getNumberOfVariables.restype = ct.c_int 
        self.hdll.fmi3_getNumberOfVariables.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableByName.restype = ct.c_void_p
        self.hdll.fmi3_getVariableByName.argtypes = ct.c_void_p,ct.c_char_p,
        self.hdll.fmi3_getVariableByIndex.restype = ct.c_void_p 
        self.hdll.fmi3_getVariableByIndex.argtypes = ct.c_void_p,ct.c_int,
        self.hdll.fmi3_getVariableByValueReference.restype = ct.c_void_p
        self.hdll.fmi3_getVariableByValueReference.argtypes = ct.c_void_p, ct.c_uint,
        self.hdll.fmi3_getVariableName.restype = ct.c_char_p 
        self.hdll.fmi3_getVariableName.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableCausality.restype = ct.c_int 
        self.hdll.fmi3_getVariableCausality.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableVariability.restype = ct.c_int
        self.hdll.fmi3_getVariableVariability.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableInitial.restype = ct.c_int
        self.hdll.fmi3_getVariableInitial.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableSupportsIntermediateUpdate.restype = ct.c_bool
        self.hdll.fmi3_getVariableSupportsIntermediateUpdate.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableDataType.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableDescription.restype = ct.c_char_p
        self.hdll.fmi3_getVariableDescription.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableQuantity.restype = ct.c_char_p
        self.hdll.fmi3_getVariableQuantity.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableUnit.restype = ct.c_char_p
        self.hdll.fmi3_getVariableUnit.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableDisplayUnit.restype = ct.c_char_p
        self.hdll.fmi3_getVariableDisplayUnit.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableHasStartValue.restype = ct.c_bool 
        self.hdll.fmi3_getVariableHasStartValue.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableStartFloat64.restype = ct.c_double 
        self.hdll.fmi3_getVariableStartFloat64.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableStartFloat32.restype = ct.c_float 
        self.hdll.fmi3_getVariableStartFloat32.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableStartInt64.restype = ct.c_int64
        self.hdll.fmi3_getVariableStartInt64.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableStartInt32.restype = ct.c_int32 
        self.hdll.fmi3_getVariableStartInt32.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableStartInt16.restype = ct.c_int16
        self.hdll.fmi3_getVariableStartInt16.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableStartInt8.restype = ct.c_int8 
        self.hdll.fmi3_getVariableStartInt8.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableStartUInt64.restype = ct.c_uint64
        self.hdll.fmi3_getVariableStartUInt64.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableStartUInt32.restype = ct.c_uint32 
        self.hdll.fmi3_getVariableStartUInt32.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableStartUInt16.restype = ct.c_uint16
        self.hdll.fmi3_getVariableStartUInt16.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableStartUInt8.restype = ct.c_uint8 
        self.hdll.fmi3_getVariableStartUInt8.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableStartBoolean.restype = ct.c_bool 
        self.hdll.fmi3_getVariableStartBoolean.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableStartString.restype = ct.c_char_p 
        self.hdll.fmi3_getVariableStartString.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableStartBinary.restype = ct.POINTER(ct.c_uint8)
        self.hdll.fmi3_getVariableStartBinary.argtypes = ct.c_void_p,
        self.hdll.fmi3_getVariableValueReference.restype = ct.c_uint 
        self.hdll.fmi3_getVariableValueReference.argtypes = ct.c_void_p,

        self.hdll.fmi3_getNumberOfUnits.restype = ct.c_int 
        self.hdll.fmi3_getNumberOfUnits.argtypes = ct.c_void_p,
        self.hdll.fmi3_getUnitByIndex.restype = ct.c_void_p
        self.hdll.fmi3_getUnitByIndex.argtypes = ct.c_void_p, ct.c_int,
        self.hdll.fmi3_getUnitName.restype = ct.c_char_p
        self.hdll.fmi3_getUnitName.argtypes = ct.c_void_p,
        self.hdll.fmi3_hasBaseUnit.restype = ct.c_bool
        self.hdll.fmi3_hasBaseUnit.argtypes = ct.c_void_p,
        self.hdll.fmi3_getBaseUnit.restype = None 
        self.hdll.fmi3_getBaseUnit.argtypes = ct.c_void_p,ct.POINTER(ct.c_double),ct.POINTER(ct.c_double),ct.POINTER(ct.c_int),ct.POINTER(ct.c_int),ct.POINTER(ct.c_int),ct.POINTER(ct.c_int),ct.POINTER(ct.c_int),ct.POINTER(ct.c_int),ct.POINTER(ct.c_int),ct.POINTER(ct.c_int),
        self.hdll.fmi3_getNumberOfDisplayUnits.restype = ct.c_int 
        self.hdll.fmi3_getNumberOfDisplayUnits.argtypes = ct.c_void_p,
        self.hdll.fmi3_getDisplayUnitByIndex.restype = None 
        self.hdll.fmi3_getDisplayUnitByIndex.argtypes = ct.c_void_p, ct.c_int, ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_double), ct.POINTER(ct.c_double), ct.POINTER(ct.c_bool),

        self.hdll.fmi3_getFloat64Type.restype = None 
        self.hdll.fmi3_getFloat64Type.argtypes = ct.c_void_p, ct.c_char_p, ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_bool), ct.POINTER(ct.c_bool), ct.POINTER(ct.c_double), ct.POINTER(ct.c_double), ct.POINTER(ct.c_double),
        self.hdll.fmi3_getFloat32Type.restype = None 
        self.hdll.fmi3_getFloat32Type.argtypes = ct.c_void_p, ct.c_char_p, ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_bool), ct.POINTER(ct.c_bool), ct.POINTER(ct.c_float), ct.POINTER(ct.c_float), ct.POINTER(ct.c_float),
        self.hdll.fmi3_getInt64Type.restype = None 
        self.hdll.fmi3_getInt64Type.argtypes = ct.c_void_p, ct.c_char_p, ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_int64), ct.POINTER(ct.c_int64),
        self.hdll.fmi3_getInt32Type.restype = None 
        self.hdll.fmi3_getInt32Type.argtypes = ct.c_void_p, ct.c_char_p, ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_int32), ct.POINTER(ct.c_int32),
        self.hdll.fmi3_getInt16Type.restype = None 
        self.hdll.fmi3_getInt16Type.argtypes = ct.c_void_p, ct.c_char_p, ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_int16), ct.POINTER(ct.c_int16),
        self.hdll.fmi3_getInt8Type.restype = None 
        self.hdll.fmi3_getInt8Type.argtypes = ct.c_void_p, ct.c_char_p, ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_int8), ct.POINTER(ct.c_int8),
        self.hdll.fmi3_getUInt64Type.restype = None 
        self.hdll.fmi3_getUInt64Type.argtypes = ct.c_void_p, ct.c_char_p, ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_uint64), ct.POINTER(ct.c_uint64),
        self.hdll.fmi3_getUInt32Type.restype = None 
        self.hdll.fmi3_getUInt32Type.argtypes = ct.c_void_p, ct.c_char_p, ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_uint32), ct.POINTER(ct.c_uint32),
        self.hdll.fmi3_getUInt16Type.restype = None 
        self.hdll.fmi3_getUInt16Type.argtypes = ct.c_void_p, ct.c_char_p, ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_uint16), ct.POINTER(ct.c_uint16),
        self.hdll.fmi3_getUInt8Type.restype = None 
        self.hdll.fmi3_getUInt8Type.argtypes = ct.c_void_p, ct.c_char_p, ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_uint8), ct.POINTER(ct.c_uint8),
        self.hdll.fmi3_getBooleanType.restype = None 
        self.hdll.fmi3_getBooleanType.argtypes = ct.c_void_p, ct.c_char_p, ct.c_char_p,
        self.hdll.fmi3_getStringType.restype = None 
        self.hdll.fmi3_getStringType.argtypes = ct.c_void_p, ct.c_char_p, ct.c_char_p,
        self.hdll.fmi3_getBinaryType.restype = None 
        self.hdll.fmi3_getBinaryType.argtypes = ct.c_void_p, ct.c_char_p, ct.c_char_p, ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_uint32),
        self.hdll.fmi3_getEnumerationType.restype = None 
        self.hdll.fmi3_getEnumerationType.argtypes = ct.c_void_p, ct.c_char_p, ct.c_char_p, ct.c_char_p, ct.POINTER(ct.c_int64), ct.POINTER(ct.c_int64), ct.POINTER(ct.c_int),
        self.hdll.fmi3_getEnumerationItem.restype = None 
        self.hdll.fmi3_getEnumerationItem.argtypes = ct.c_void_p, ct.c_char_p, ct.c_int, ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_int64), ct.POINTER(ct.c_char_p),
        self.hdll.fmi3_getClockType.restype = None 
        self.hdll.fmi3_getClockType.argtypes = ct.c_void_p, ct.c_char_p, ct.c_char_p, ct.POINTER(ct.c_bool), ct.POINTER(ct.c_uint32), ct.c_void_p, ct.POINTER(ct.c_float), ct.POINTER(ct.c_float), ct.POINTER(ct.c_bool), ct.POINTER(ct.c_uint64), ct.POINTER(ct.c_uint64), ct.POINTER(ct.c_uint64),

        self.hdll.fmi3_getNumberOfLogCategories.restype = ct.c_int 
        self.hdll.fmi3_getNumberOfLogCategories.argtypes = ct.c_void_p,
        self.hdll.fmi3_getLogCategory.restype = None 
        self.hdll.fmi3_getLogCategory.argtypes = ct.c_void_p, ct.c_int, ct.POINTER(ct.c_char_p), ct.POINTER(ct.c_char_p),
        
        self.hdll.fmi3_getNumberOfModelStructureOutputs.restype = ct.c_int;
        self.hdll.fmi3_getNumberOfModelStructureOutputs.argtypes = ct.c_void_p,
        self.hdll.fmi3_getNumberOfModelStructureContinuousStateDerivatives.restype = ct.c_int;
        self.hdll.fmi3_getNumberOfModelStructureContinuousStateDerivatives.argtypes = ct.c_void_p,
        self.hdll.fmi3_getNumberOfModelStructureClockedStates.restype = ct.c_int;
        self.hdll.fmi3_getNumberOfModelStructureClockedStates.argtypes = ct.c_void_p,
        self.hdll.fmi3_getNumberOfModelStructureEventIndicators.restype = ct.c_int;
        self.hdll.fmi3_getNumberOfModelStructureEventIndicators.argtypes = ct.c_void_p,
        self.hdll.fmi3_getNumberOfModelStructureInitialUnknowns.restype = ct.c_int;
        self.hdll.fmi3_getNumberOfModelStructureInitialUnknowns.argtypes = ct.c_void_p,
        self.hdll.fmi3_getModelStructureOutput.restype =  ct.c_void_p
        self.hdll.fmi3_getModelStructureOutput.argtypes = ct.c_void_p,ct.c_size_t,
        self.hdll.fmi3_getModelStructureContinuousStateDerivative.restype =  ct.c_void_p
        self.hdll.fmi3_getModelStructureContinuousStateDerivative.argtypes = ct.c_void_p,ct.c_size_t,
        self.hdll.fmi3_getModelStructureInitialUnknown.restype =  ct.c_void_p
        self.hdll.fmi3_getModelStructureInitialUnknown.argtypes = ct.c_void_p,ct.c_size_t,
        self.hdll.fmi3_getModelStructureValueReference.restype = ct.c_int
        self.hdll.fmi3_getModelStructureValueReference.argtypes = ct.c_void_p,
        self.hdll.fmi3_getModelStructureNumberOfDependencies.restype = ct.c_int
        self.hdll.fmi3_getModelStructureNumberOfDependencies.argtypes = ct.c_void_p,
        self.hdll.fmi3_getModelStructureDependencyKindsDefined.restype = ct.c_bool
        self.hdll.fmi3_getModelStructureDependencyKindsDefined.argtypes = ct.c_void_p,
        self.hdll.fmi3_getModelStructureDependencies.restype = None
        self.hdll.fmi3_getModelStructureDependencies.argtypes = ct.c_void_p, ct.POINTER(ct.c_int), ct.c_size_t,
        self.hdll.fmi3_getModelStructureDependencyKinds.restype = None
        self.hdll.fmi3_getModelStructureDependencyKinds.argtypes = ct.c_void_p, ct.POINTER(ct.c_int), ct.c_size_t,
  
        self.hdll.fmi3cs_getModelIdentifier.restype = ct.c_char_p
        self.hdll.fmi3cs_getModelIdentifier.argtypes = ct.c_void_p,
        self.hdll.fmi3cs_getNeedsExecutionTool.restype = ct.c_bool 
        self.hdll.fmi3cs_getNeedsExecutionTool.argtypes = ct.c_void_p,
        self.hdll.fmi3cs_getCanBeInstantiatedOnlyOncePerProcess.restype = ct.c_bool 
        self.hdll.fmi3cs_getCanBeInstantiatedOnlyOncePerProcess.argtypes = ct.c_void_p,
        self.hdll.fmi3cs_getCanGetAndSetFMUState.restype = ct.c_bool 
        self.hdll.fmi3cs_getCanGetAndSetFMUState.argtypes = ct.c_void_p,
        self.hdll.fmi3cs_getCanSerializeFMUState.restype = ct.c_bool 
        self.hdll.fmi3cs_getCanSerializeFMUState.argtypes = ct.c_void_p,
        self.hdll.fmi3cs_getProvidesDirectionalDerivative.restype = ct.c_bool 
        self.hdll.fmi3cs_getProvidesDirectionalDerivative.argtypes = ct.c_void_p,
        self.hdll.fmi3cs_getProvidesAdjointDerivatives.restype = ct.c_bool 
        self.hdll.fmi3cs_getProvidesAdjointDerivatives.argtypes = ct.c_void_p,
        self.hdll.fmi3cs_getProvidesPerElementDependencies.restype = ct.c_bool 
        self.hdll.fmi3cs_getProvidesPerElementDependencies.argtypes = ct.c_void_p,
        self.hdll.fmi3cs_getProvidesIntermediateUpdate.restype = ct.c_bool 
        self.hdll.fmi3cs_getProvidesIntermediateUpdate.argtypes = ct.c_void_p,
        self.hdll.fmi3cs_getProvidesEvaluateDiscreteStates.restype = ct.c_bool 
        self.hdll.fmi3cs_getProvidesEvaluateDiscreteStates.argtypes = ct.c_void_p,
        self.hdll.fmi3cs_getHasEventMode.restype = ct.c_bool 
        self.hdll.fmi3cs_getHasEventMode.argtypes = ct.c_void_p,
        self.hdll.fmi3cs_getRecommendedIntermediateInputSmoothness.restype = ct.c_bool 
        self.hdll.fmi3cs_getRecommendedIntermediateInputSmoothness.argtypes = ct.c_void_p,
        self.hdll.fmi3cs_getMaxOutputDerivativeOrder.restype = ct.c_int
        self.hdll.fmi3cs_getMaxOutputDerivativeOrder.argtypes = ct.c_void_p,
        self.hdll.fmi3cs_getCanHandleVariableCommunicationStepSize.restype = ct.c_bool
        self.hdll.fmi3cs_getCanHandleVariableCommunicationStepSize.argtypes = ct.c_void_p,
        self.hdll.fmi3cs_getCanReturnEarlyAfterIntermediateUpdate.restype = ct.c_bool
        self.hdll.fmi3cs_getCanReturnEarlyAfterIntermediateUpdate.argtypes = ct.c_void_p,
        self.hdll.fmi3cs_getFixedInternalStepSize.restype = ct.c_double
        self.hdll.fmi3cs_getFixedInternalStepSize.argtypes = ct.c_void_p,

        self.hdll.fmi3me_getModelIdentifier.restype = ct.c_char_p 
        self.hdll.fmi3me_getModelIdentifier.argtypes = ct.c_void_p,
        self.hdll.fmi3me_getNeedsExecutionTool.restype = ct.c_bool 
        self.hdll.fmi3me_getNeedsExecutionTool.argtypes = ct.c_void_p,
        self.hdll.fmi3me_getCanBeInstantiatedOnlyOncePerProcess.restype = ct.c_bool 
        self.hdll.fmi3me_getCanBeInstantiatedOnlyOncePerProcess.argtypes = ct.c_void_p,
        self.hdll.fmi3me_getCanGetAndSetFMUState.restype = ct.c_bool 
        self.hdll.fmi3me_getCanGetAndSetFMUState.argtypes = ct.c_void_p,
        self.hdll.fmi3me_getCanSerializeFMUState.restype = ct.c_bool 
        self.hdll.fmi3me_getCanSerializeFMUState.argtypes = ct.c_void_p,
        self.hdll.fmi3me_getProvidesDirectionalDerivative.restype = ct.c_bool 
        self.hdll.fmi3me_getProvidesDirectionalDerivative.argtypes = ct.c_void_p,
        self.hdll.fmi3me_getProvidesAdjointDerivatives.restype = ct.c_bool 
        self.hdll.fmi3me_getProvidesAdjointDerivatives.argtypes = ct.c_void_p,
        self.hdll.fmi3me_getProvidesPerElementDependencies.restype = ct.c_bool 
        self.hdll.fmi3me_getProvidesPerElementDependencies.argtypes = ct.c_void_p,
        self.hdll.fmi3me_getProvidesEvaluateDiscreteStates.restype = ct.c_bool 
        self.hdll.fmi3me_getProvidesEvaluateDiscreteStates.argtypes = ct.c_void_p,
        self.hdll.fmi3me_getNeedsCompletedIntegratorStep.restype = ct.c_bool 
        self.hdll.fmi3me_getNeedsCompletedIntegratorStep.argtypes = ct.c_void_p,

        self.hdll.fmi3se_getModelIdentifier.restype = ct.c_char_p 
        self.hdll.fmi3se_getModelIdentifier.argtypes = ct.c_void_p,
        self.hdll.fmi3se_getNeedsExecutionTool.restype = ct.c_bool 
        self.hdll.fmi3se_getNeedsExecutionTool.argtypes = ct.c_void_p,
        self.hdll.fmi3se_getCanBeInstantiatedOnlyOncePerProcess.restype = ct.c_bool 
        self.hdll.fmi3se_getCanBeInstantiatedOnlyOncePerProcess.argtypes = ct.c_void_p,
        self.hdll.fmi3se_getCanGetAndSetFMUState.restype = ct.c_bool 
        self.hdll.fmi3se_getCanGetAndSetFMUState.argtypes = ct.c_void_p,
        self.hdll.fmi3se_getCanSerializeFMUState.restype = ct.c_bool 
        self.hdll.fmi3se_getCanSerializeFMUState.argtypes = ct.c_void_p,
        self.hdll.fmi3se_getProvidesDirectionalDerivative.restype = ct.c_bool 
        self.hdll.fmi3se_getProvidesDirectionalDerivative.argtypes = ct.c_void_p,
        self.hdll.fmi3se_getProvidesAdjointDerivatives.restype = ct.c_bool 
        self.hdll.fmi3se_getProvidesAdjointDerivatives.argtypes = ct.c_void_p,
        self.hdll.fmi3se_getProvidesPerElementDependencies.restype = ct.c_bool 
        self.hdll.fmi3se_getProvidesPerElementDependencies.argtypes = ct.c_void_p,

        self.hdll.fmi3_instantiateCoSimulation.restype = ct.c_bool 
        self.hdll.fmi3_instantiateCoSimulation.argtypes = ct.c_void_p, ct.c_bool, ct.c_bool, ct.c_bool, ct.c_bool, ct.POINTER(ct.c_uint), ct.c_size_t, ct.c_void_p, ct.c_void_p, ct.c_void_p,
        self.hdll.fmi3_instantiateModelExchange.restype = ct.c_bool 
        self.hdll.fmi3_instantiateModelExchange.argtypes = ct.c_void_p, ct.c_bool, ct.c_bool, ct.c_void_p, ct.c_void_p,
        self.hdll.fmi3_getVersion.restype = ct.c_char_p 
        self.hdll.fmi3_getVersion.argtypes = ct.c_void_p,
        self.hdll.fmi3_setDebugLogging.restype = ct.c_int 
        self.hdll.fmi3_setDebugLogging.argtypes = ct.c_void_p, ct.c_bool, ct.c_size_t, ct.POINTER(ct.c_char_p),

        self.hdll.fmi3_enterInitializationMode.restype = ct.c_int 
        self.hdll.fmi3_enterInitializationMode.argtypes = ct.c_void_p, ct.c_bool, ct.c_double, ct.c_double, ct.c_bool, ct.c_double,
        self.hdll.fmi3_exitInitializationMode.restype = ct.c_int 
        self.hdll.fmi3_exitInitializationMode.argtypes = ct.c_void_p,
        self.hdll.fmi3_terminate.restype = ct.c_int 
        self.hdll.fmi3_terminate.argtypes = ct.c_void_p,
        self.hdll.fmi3_freeInstance.restype = None 
        self.hdll.fmi3_freeInstance.argtypes = ct.c_void_p,
        self.hdll.fmi3_doStep.restype = ct.c_int 
        self.hdll.fmi3_doStep.argtypes = ct.c_void_p, ct.c_double, ct.c_double, ct.c_bool, ct.POINTER(ct.c_bool), ct.POINTER(ct.c_bool), ct.POINTER(ct.c_bool), ct.POINTER(ct.c_double),
        self.hdll.fmi3_enterEventMode.restype = ct.c_int 
        self.hdll.fmi3_enterEventMode.argtypes = ct.c_void_p,
        self.hdll.fmi3_reset.restype = ct.c_int 
        self.hdll.fmi3_reset.argtypes = ct.c_void_p,
        self.hdll.fmi3_getFloat64.restype = ct.c_int 
        self.hdll.fmi3_getFloat64.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_double), ct.c_size_t,
        self.hdll.fmi3_setFloat64.restype = ct.c_int 
        self.hdll.fmi3_setFloat64.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_double), ct.c_size_t,
        self.hdll.fmi3_getFloat32.restype = ct.c_int 
        self.hdll.fmi3_getFloat32.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_float), ct.c_size_t,
        self.hdll.fmi3_getInt8.restype = ct.c_int 
        self.hdll.fmi3_getInt8.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_int8), ct.c_size_t,
        self.hdll.fmi3_getUInt8.restype = ct.c_int 
        self.hdll.fmi3_getUInt8.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_uint8), ct.c_size_t,
        self.hdll.fmi3_getInt16.restype = ct.c_int 
        self.hdll.fmi3_getInt16.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_int16), ct.c_size_t,
        self.hdll.fmi3_getUInt16.restype = ct.c_int 
        self.hdll.fmi3_getUInt16.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_uint16), ct.c_size_t,
        self.hdll.fmi3_getInt32.restype = ct.c_int 
        self.hdll.fmi3_getInt32.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_int32), ct.c_size_t,
        self.hdll.fmi3_getUInt32.restype = ct.c_int 
        self.hdll.fmi3_getUInt32.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_uint32), ct.c_size_t,
        self.hdll.fmi3_getInt64.restype = ct.c_int 
        self.hdll.fmi3_getInt64.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_int64), ct.c_size_t,
        self.hdll.fmi3_getUInt64.restype = ct.c_int 
        self.hdll.fmi3_getUInt64.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_uint64), ct.c_size_t,
        self.hdll.fmi3_getBoolean.restype = ct.c_int 
        self.hdll.fmi3_getBoolean.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_bool), ct.c_size_t,
        self.hdll.fmi3_getString.restype = ct.c_int 
        self.hdll.fmi3_getString.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_char_p), ct.c_size_t,
        self.hdll.fmi3_getBinary.restype = ct.c_int 
        self.hdll.fmi3_getBinary.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_size_t), ct.POINTER(ct.POINTER(ct.c_uint8)), ct.c_size_t,
        self.hdll.fmi3_getClock.restype = ct.c_int 
        self.hdll.fmi3_getClock.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_bool),
        self.hdll.fmi3_setFloat32.restype = ct.c_int 
        self.hdll.fmi3_setFloat32.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_float), ct.c_size_t,
        self.hdll.fmi3_setInt8.restype = ct.c_int 
        self.hdll.fmi3_setInt8.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_int8), ct.c_size_t,
        self.hdll.fmi3_setUInt8.restype = ct.c_int 
        self.hdll.fmi3_setUInt8.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_uint8), ct.c_size_t,
        self.hdll.fmi3_setInt16.restype = ct.c_int 
        self.hdll.fmi3_setInt16.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_int16), ct.c_size_t,
        self.hdll.fmi3_setUInt16.restype = ct.c_int 
        self.hdll.fmi3_setUInt16.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_uint16), ct.c_size_t,
        self.hdll.fmi3_setInt32.restype = ct.c_int 
        self.hdll.fmi3_setInt32.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_int32), ct.c_size_t,
        self.hdll.fmi3_setUInt32.restype = ct.c_int 
        self.hdll.fmi3_setUInt32.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_uint32), ct.c_size_t,
        self.hdll.fmi3_setInt64.restype = ct.c_int 
        self.hdll.fmi3_setInt64.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_int64), ct.c_size_t,
        self.hdll.fmi3_setUInt64.restype = ct.c_int 
        self.hdll.fmi3_setUInt64.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_uint64), ct.c_size_t,
        self.hdll.fmi3_setBoolean.restype = ct.c_int 
        self.hdll.fmi3_setBoolean.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_bool), ct.c_size_t,
        self.hdll.fmi3_setString.restype = ct.c_int 
        self.hdll.fmi3_setString.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_char_p), ct.c_size_t,
        self.hdll.fmi3_setBinary.restype = ct.c_int 
        self.hdll.fmi3_setBinary.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_size_t), ct.POINTER(ct.POINTER(ct.c_uint8)), ct.c_size_t,
        self.hdll.fmi3_setClock.restype = ct.c_int 
        self.hdll.fmi3_setClock.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_bool),
        self.hdll.fmi3_getNumberOfVariableDependencies.restype = ct.c_int 
        self.hdll.fmi3_getNumberOfVariableDependencies.argtypes = ct.c_void_p,ct.c_uint,ct.POINTER(ct.c_size_t),
        self.hdll.fmi3_getVariableDependencies.restype = ct.c_int 
        self.hdll.fmi3_getVariableDependencies.argtypes = ct.c_void_p,ct.c_uint,ct.POINTER(ct.c_size_t),ct.POINTER(ct.c_uint),ct.POINTER(ct.c_size_t),ct.POINTER(ct.c_int),ct.c_size_t,

        self.hdll.fmi3_getFMUState.restype = ct.c_int 
        self.hdll.fmi3_getFMUState.argtypes = ct.c_void_p, ct.POINTER(ct.c_void_p)
        self.hdll.fmi3_setFMUState.restype = ct.c_int 
        self.hdll.fmi3_setFMUState.argtypes = ct.c_void_p, ct.c_void_p,
        self.hdll.fmi3_freeFMUState.restype = ct.c_int 
        self.hdll.fmi3_freeFMUState.argtypes = ct.c_void_p, ct.POINTER(ct.c_void_p)
        self.hdll.fmi3_serializedFMUStateSize.restype = ct.c_int 
        self.hdll.fmi3_serializedFMUStateSize.argtypes = ct.c_void_p,ct.c_void_p,ct.POINTER(ct.c_size_t)
        self.hdll.fmi3_serializeFMUState.restype = ct.c_int 
        self.hdll.fmi3_serializeFMUState.argtypes = ct.c_void_p,ct.c_void_p ,ct.POINTER(ct.c_uint8),ct.c_size_t,
        self.hdll.fmi3_deserializeFMUState.restype = ct.c_int 
        self.hdll.fmi3_deserializeFMUState.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint8),ct.c_size_t,ct.POINTER(ct.c_void_p)
        self.hdll.fmi3_getDirectionalDerivative.restype = ct.c_int 
        self.hdll.fmi3_getDirectionalDerivative.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint),ct.c_size_t, ct.POINTER(ct.c_uint),ct.c_size_t, ct.POINTER(ct.c_double),ct.c_size_t, ct.POINTER(ct.c_double),ct.c_size_t,
        self.hdll.fmi3_getAdjointDerivative.restype = ct.c_int 
        self.hdll.fmi3_getAdjointDerivative.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint),ct.c_size_t, ct.POINTER(ct.c_uint),ct.c_size_t, ct.POINTER(ct.c_double),ct.c_size_t, ct.POINTER(ct.c_double),ct.c_size_t,
        self.hdll.fmi3_enterConfigurationMode.restype = ct.c_int 
        self.hdll.fmi3_enterConfigurationMode.argtypes = ct.c_void_p,
        self.hdll.fmi3_exitConfigurationMode.restype = ct.c_int 
        self.hdll.fmi3_exitConfigurationMode.argtypes = ct.c_void_p,
        self.hdll.fmi3_getIntervalDecimal.restype = ct.c_int 
        self.hdll.fmi3_getIntervalDecimal.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint), ct.c_size_t,ct.POINTER(ct.c_double),ct.POINTER(ct.c_uint),
        self.hdll.fmi3_getIntervalFraction.restype = ct.c_int 
        self.hdll.fmi3_getIntervalFraction.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint), ct.c_size_t,ct.POINTER(ct.c_uint64),ct.POINTER(ct.c_uint64),ct.POINTER(ct.c_uint),
        self.hdll.fmi3_getShiftDecimal.restype = ct.c_int 
        self.hdll.fmi3_getShiftDecimal.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t,ct.POINTER(ct.c_double),
        self.hdll.fmi3_getShiftFraction.restype = ct.c_int 
        self.hdll.fmi3_getShiftFraction.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t,ct.POINTER(ct.c_uint64),ct.POINTER(ct.c_uint64),
        self.hdll.fmi3_setIntervalDecimal.restype = ct.c_int 
        self.hdll.fmi3_setIntervalDecimal.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_double),
        self.hdll.fmi3_setIntervalFraction.restype = ct.c_int 
        self.hdll.fmi3_setIntervalFraction.argtypes = ct.c_void_p, ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_uint64), ct.POINTER(ct.c_uint64),
        self.hdll.fmi3_evaluateDiscreteStates.restype = ct.c_int 
        self.hdll.fmi3_evaluateDiscreteStates.argtypes = ct.c_void_p,
        self.hdll.fmi3_updateDiscreteStates.restype = ct.c_int 
        self.hdll.fmi3_updateDiscreteStates.argtypes = ct.c_void_p,ct.POINTER(ct.c_bool), ct.POINTER(ct.c_bool), ct.POINTER(ct.c_bool), ct.POINTER(ct.c_bool), ct.POINTER(ct.c_bool), ct.POINTER(ct.c_double),
        self.hdll.fmi3_enterContinuousTimeMode.restype = ct.c_int 
        self.hdll.fmi3_enterContinuousTimeMode.argtypes = ct.c_void_p,
        self.hdll.fmi3_completedIntegratorStep.restype = ct.c_int 
        self.hdll.fmi3_completedIntegratorStep.argtypes = ct.c_void_p,ct.c_bool ,ct.POINTER(ct.c_bool), ct.POINTER(ct.c_bool)
        self.hdll.fmi3_setTime.restype = ct.c_int 
        self.hdll.fmi3_setTime.argtypes = ct.c_void_p, ct.c_double,
        self.hdll.fmi3_setContinuousStates.restype = ct.c_int 
        self.hdll.fmi3_setContinuousStates.argtypes = ct.c_void_p, ct.POINTER(ct.c_double),ct.c_size_t,
        self.hdll.fmi3_getContinuousStateDerivatives.restype = ct.c_int 
        self.hdll.fmi3_getContinuousStateDerivatives.argtypes = ct.c_void_p,ct.POINTER(ct.c_double),ct.c_size_t,
        self.hdll.fmi3_getEventIndicators.restype = ct.c_int 
        self.hdll.fmi3_getEventIndicators.argtypes = ct.c_void_p,ct.POINTER(ct.c_double),ct.c_size_t,
        self.hdll.fmi3_getContinuousStates.restype = ct.c_int 
        self.hdll.fmi3_getContinuousStates.argtypes = ct.c_void_p,ct.POINTER(ct.c_double),ct.c_size_t,
        self.hdll.fmi3_getNominalsOfContinuousStates.restype = ct.c_int 
        self.hdll.fmi3_getNominalsOfContinuousStates.argtypes = ct.c_void_p,ct.POINTER(ct.c_double),ct.c_size_t,
        self.hdll.fmi3_getNumberOfEventIndicators.restype = ct.c_int 
        self.hdll.fmi3_getNumberOfEventIndicators.argtypes = ct.c_void_p,ct.POINTER(ct.c_size_t)
        self.hdll.fmi3_getNumberOfContinuousStates.restype = ct.c_int 
        self.hdll.fmi3_getNumberOfContinuousStates.argtypes = ct.c_void_p,ct.POINTER(ct.c_size_t)
        self.hdll.fmi3_enterStepMode.restype = ct.c_int 
        self.hdll.fmi3_enterStepMode.argtypes = ct.c_void_p,
        self.hdll.fmi3_getOutputDerivatives.restype = ct.c_int 
        self.hdll.fmi3_getOutputDerivatives.argtypes = ct.c_void_p,ct.POINTER(ct.c_uint), ct.c_size_t, ct.POINTER(ct.c_int32),ct.POINTER(ct.c_double),ct.c_size_t,
        self.hdll.fmi3_activateModelPartition.restype = ct.c_int 
        self.hdll.fmi3_activateModelPartition.argtypes = ct.c_void_p,ct.c_uint,ct.c_double,

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

    def translateFmi1DataType(self, type):
        match type:
            case 0:
                return "fmi1DataTypeReal"
            case 1:
                return "fmi1DataTypeInteger"
            case 2:
                return "fmi1DataTypeBoolean"
            case 3:
                return "fmi1DataTypeString"
            case 4:
                return "fmi1DataTypeEnumeration"
            case _:
                return ""  

    def translateFmi2DataType(self, type):
        match type:
            case 0:
                return "fmi2DataTypeReal"
            case 1:
                return "fmi2DataTypeInteger"
            case 2:
                return "fmi2DataTypeBoolean"
            case 3:
                return "fmi2DataTypeString"
            case 4:
                return "fmi2DataTypeEnumeration"
            case _:
                return "" 
    
    fmi2CausalityStrings = ["fmi2CausalityInput", "fmi2CausalityOutput", "fmi2CausalityParameter", "fmi2CausalityCalculatedParameter", "fmi2CausalityLocal", "fmi2CausalityIndependent"]
    fmi2VariabilityStrings = ["fmi2VariabilityFixed", "fmi2VariabilityTunable", "fmi2VariabilityConstant", "fmi2VariabilityDiscrete", "fmi2VariabilityContinuous"]
    fmi2InitialStrings = ["fmi2InitialExact", "fmi2InitialApprox", "fmi2InitialCalculated", "fmi2InitialUnknown"]
    
    fmi3CausalityStrings = ["fmi3CausalityInput", "fmi3CausalityOutput", "fmi3CausalityParameter", "fmi3CausalityCalculatedParameter", "fmi3CausalityLocal", "fmi3CausalityIndependent", "fmi3CausalityStructuralParameter"]
    fmi3VariabilityStrings = ["fmi3VariabilityFixed", "fmi3VariabilityTunable", "fmi3VariabilityConstant", "fmi3VariabilityDiscrete", "fmi3VariabilityContinuous"]
    fmi3InitialStrings = ["fmi3InitialExact", "fmi3InitialApprox", "fmi3InitialCalculated"]
    fmi3DataTypeStrings = ["fmi3DataTypeFloat64", "fmi3DataTypeFloat32", "fmi3DataTypeInt64", "fmi3DataTypeInt32", "fmi3DataTypeInt16", "fmi3DataTypeInt8", "fmi3DataTypeUInt64", "fmi3DataTypeUInt32", "fmi3DataTypeUInt16", "fmi3DataTypeUInt8", "fmi3DataTypeBoolean", "fmi3DataTypeString", "fmi3DataTypeBinary", "fmi3DataTypeEnumeration", "fmi3DataTypeClock"]
    
    def fmi4c_getFmiVersion(self):
        return self.translateFmiVersion(self.hdll.fmi4c_getFmiVersion(self.fmu))

    def fmi4c_getErrorMessages(self):
        return self.hdll.fmi4c_getErrorMessages()

    def fmi4c_loadFmu(self, path, instanceName):
        self.fmu = self.hdll.fmi4c_loadFmu(path.encode(), instanceName.encode())
        return self.fmu != 0

    def fmi4c_freeFmu(self):
        self.hdll.fmi4c_freeFmu(self.fmu)

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

    def fmi1_getVariableRelativeQuantity(self, var):
        return self.hdll.fmi1_getVariableRelativeQuantity(var)

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
        return self.hdll.fmi1_getVariableIsFixed(var);

    def fmi1_getVariableDataType(self, var):
        return self.translateFmi1DataType(self.hdll.fmi1_getVariableDataType(var));

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

    def fmi1_getDerivatives(self, nDerivatives):
        double_array_type = ct.c_double * nDerivatives
        derivativesArray = double_array_type()
        success = self.hdll.fmi1_getDerivatives(self.fmu, derivativesArray, nDerivatives)
        return [success, list(derivativesArray)]

    def fmi1_getEventIndicators(self, indicators, nIndicators):
        double_array_type = ct.c_double * nIndicators
        indicatorsArray = double_array_type(*indicators)
        success = self.hdll.fmi1_getEventIndicators(self.fmu, indicatorsArray, nIndicators)
        return [success, list(indicatorsArray)]

    def fmi1_eventUpdate(self, intermediateResults, eventInfo):
        return self.hdll.fmi1_eventUpdate(self.fmu, intermediateResults, eventInfo)

    def fmi1_getContinuousStates(self, nStates):
        double_array_type = ct.c_double * nStates
        statesArray = double_array_type()
        success = self.hdll.fmi1_getContinuousStates(self.fmu, statesArray, nStates)
        return [success, list(statesArray)]

    def fmi1_getNominalContinuousStates(self, nNominals):
        double_array_type = ct.c_double * nNominals
        nominalsArray = double_array_type()
        success = self.hdll.fmi1_getNominalContinuousStates(self.fmu, nominalsArray, nNominals)
        return [success, list(nominalsArray)]


    def fmi1_getStateValueReferences(self, nValueReferences):
        uint_array_type = ct.c_uint * nValueReferences
        valueReferencesArray = uint_array_type(*[1]*nValueReferences)
        success = self.hdll.fmi1_getStateValueReferences(self.fmu, valueReferencesArray, nValueReferences)
        return [success, list(valueReferencesArray)]

    def fmi1_terminate(self):
        return self.hdll.fmi1_terminate(self.fmu)

    def fmi2_defaultStartTimeDefined(self):
        return self.hdll.fmi2_defaultStartTimeDefined(self.fmu)

    def fmi2_defaultStopTimeDefined(self):
        return self.hdll.fmi2_defaultStopTimeDefined(self.fmu)

    def fmi2_defaultToleranceDefined(self):
        return self.hdll.fmi2_defaultToleranceDefined(self.fmu)

    def fmi2_defaultStepSizeDefined(self):
        return self.hdll.fmi2_defaultStepSizeDefined(self.fmu)

    def fmi2_getDefaultStartTime(self):
        return self.hdll.fmi2_getDefaultStartTime(self.fmu)

    def fmi2_getDefaultStopTime(self):
        return self.hdll.fmi2_getDefaultStopTime(self.fmu)

    def fmi2_getDefaultTolerance(self):
        return self.hdll.fmi2_getDefaultTolerance(self.fmu)

    def fmi2_getDefaultStepSize(self):
        return self.hdll.fmi2_getDefaultStepSize(self.fmu)

    def fmi2_getNumberOfVariables(self):
        return self.hdll.fmi2_getNumberOfVariables(self.fmu)

    def fmi2_getVariableByIndex(self, i):
        return self.hdll.fmi2_getVariableByIndex(self.fmu, i)

    def fmi2_getVariableByValueReference(self, vr):
        return self.hdll.fmi2_getVariableByValueReference(self.fmu, vr)

    def fmi2_getVariableByName(self, name):
        return self.hdll.fmi2_getVariableByName(self.fmu, name)

    def fmi2_getVariableName(self, var):
        return self.hdll.fmi2_getVariableName(var).decode()

    def fmi2_getVariableDescription(self, var):
        ret = self.hdll.fmi2_getVariableDescription(var)
        if ret is None:
            return ""
        else:
            return ret.decode()

    def fmi2_getFmiVersion(self):
        return self.hdll.fmi2_getFmiVersion(self.fmu).decode()

    def fmi2_getAuthor(self):
        return self.hdll.fmi2_getAuthor(self.fmu).decode()

    def fmi2_getModelName(self):
        return self.hdll.fmi2_getModelName(self.fmu).decode()

    def fmi2_getModelDescription(self):
        return self.hdll.fmi2_getModelDescription(self.fmu).decode()

    def fmi2_getCopyright(self):
        return self.hdll.fmi2_getCopyright(self.fmu).decode()

    def fmi2_getLicense(self):
        return self.hdll.fmi2_getLicense(self.fmu).decode()

    def fmi2_getGenerationTool(self):
        return self.hdll.fmi2_getGenerationTool(self.fmu).decode()

    def fmi2_getGenerationDateAndTime(self):
        return self.hdll.fmi2_getGenerationDateAndTime(self.fmu).decode()

    def fmi2_getVariableNamingConvention(self):
        return self.hdll.fmi2_getVariableNamingConvention(self.fmu).decode()

    def fmi2_getVariableDerivativeIndex(self, var):
        return self.hdll.fmi2_getVariableDerivativeIndex(var)

    def fmi2_getVariableQuantity(self, var):
        ret = self.hdll.fmi2_getVariableQuantity(var)
        if ret is None:
            return ""
        else:
            return ret.decode()        

    def fmi2_getVariableUnit(self, var):
        ret = self.hdll.fmi2_getVariableUnit(var)
        if ret is None:
            return ""
        else:
            return ret.decode()        

    def fmi2_getVariableDisplayUnit(self, var):
        ret = self.hdll.fmi2_getVariableDisplayUnit(var)
        if ret is None:
            return ""
        else:
            return ret.decode()        

    def fmi2_getVariableRelativeQuantity(self, var):
        ret = self.hdll.fmi2_getVariableRelativeQuantity(var)
        if ret is None:
            return ""
        else:
            return ret.decode()        

    def fmi2_getVariableRelativeQuantity(self, var):
        return self.hdll.fmi2_getVariableRelativeQuantity(var)

    def fmi2_getVariableMin(self, var):
        return self.hdll.fmi2_getVariableMin(var)

    def fmi2_getVariableMax(self, var):
        return self.hdll.fmi2_getVariableMax(var)

    def fmi2_getVariableNominal(self, var):
        return self.hdll.fmi2_getVariableNominal(var)

    def fmi2_getVariableUnbounded(self, var):
        return self.hdll.fmi2_getVariableUnbounded(var)        
        
    def fmi2_getVariableHasStartValue(self, var):
        return self.hdll.fmi2_getVariableHasStartValue(var)

    def fmi2_getVariableStartReal(self, var):
        return self.hdll.fmi2_getVariableStartReal(var)

    def fmi2_getVariableStartInteger(self, var):
        return self.hdll.fmi2_getVariableStartInteger(var)

    def fmi2_getVariableStartBoolean(self, var):
        return self.hdll.fmi2_getVariableStartBoolean(var)

    def fmi2_getVariableStartString(self, var):
        return self.hdll.fmi2_getVariableStartString(var)

    def fmi2_getVariableValueReference(self, var):
        return self.hdll.fmi2_getVariableValueReference(var)

    def fmi2_getVariableCausality(self, var):
        return self.fmi2CausalityStrings[self.hdll.fmi2_getVariableCausality(var)]

    def fmi2_getVariableVariability(self, var):
        return self.fmi2VariabilityStrings[self.hdll.fmi2_getVariableVariability(var)]

    def fmi2_getVariableInitial(self, var):
        return self.fmi2InitialStrings[self.hdll.fmi2_getVariableInitial(var)]

    def fmi2_getVariableCanHandleMultipleSetPerTimeInstant(self, var):
        return self.hdll.fmi2_getVariableCanHandleMultipleSetPerTimeInstant(var)

    def fmi2_getVariableDataType(self, var):
        return self.translateFmi2DataType(self.hdll.fmi2_getVariableDataType(var))

    def fmi2_getTypesPlatform(self):
        return self.hdll.fmi2_getTypesPlatform(self.fmu).decode()

    def fmi2_getVersion(self):
        return self.hdll.fmi2_getVersion(self.fmu).decode()

    def fmi2_setDebugLogging(self, loggingOn, nCategories, categories):
        return self.hdll.fmi2_setDebugLogging(self.fmu, loggingOn, nCategories, categories)

    def fmi2_getGuid(self):
        return self.hdll.fmi2_getGuid(self.fmu).decode()

    def fmi2cs_getModelIdentifier(self):
        return self.hdll.fmi2cs_getModelIdentifier(self.fmu).decode()
        
    def fmi2cs_getCanHandleVariableCommunicationStepSize(self):
        return self.hdll.fmi2cs_getCanHandleVariableCommunicationStepSize(self.fmu)

    def fmi2cs_getCanInterpolateInputs(self):
        return self.hdll.fmi2cs_getCanInterpolateInputs(self.fmu)

    def fmi2cs_getMaxOutputDerivativeOrder(self):
        return self.hdll.fmi2cs_getMaxOutputDerivativeOrder(self.fmu)

    def fmi2cs_getCanRunAsynchronuously(self):
        return self.hdll.fmi2cs_getCanRunAsynchronuously(self.fmu)

    def fmi2cs_getNeedsExecutionTool(self):
        return self.hdll.fmi2cs_getNeedsExecutionTool(self.fmu)

    def fmi2cs_getCanBeInstantiatedOnlyOncePerProcess(self):
        return self.hdll.fmi2cs_getCanBeInstantiatedOnlyOncePerProcess(self.fmu)

    def fmi2cs_getCanNotUseMemoryManagementFunctions(self):
        return self.hdll.fmi2cs_getCanNotUseMemoryManagementFunctions(self.fmu)

    def fmi2cs_getCanGetAndSetFMUState(self):
        return self.hdll.fmi2cs_getCanGetAndSetFMUState(self.fmu)

    def fmi2cs_getCanSerializeFMUState(self):
        return self.hdll.fmi2cs_getCanSerializeFMUState(self.fmu)

    def fmi2cs_getProvidesDirectionalDerivative(self):
        return self.hdll.fmi2cs_getProvidesDirectionalDerivative(self.fmu)

    def fmi2me_getModelIdentifier(self):
        return self.hdll.fmi2me_getModelIdentifier(self.fmu).decode()

    def fmi2me_getCompletedIntegratorStepNotNeeded(self):
        return self.hdll.fmi2me_getCompletedIntegratorStepNotNeeded(self.fmu)

    def fmi2me_getNeedsExecutionTool(self):
        return self.hdll.fmi2me_getNeedsExecutionTool(self.fmu)

    def fmi2me_getCanBeInstantiatedOnlyOncePerProcess(self):
        return self.hdll.fmi2me_getCanBeInstantiatedOnlyOncePerProcess(self.fmu)

    def fmi2me_getCanNotUseMemoryManagementFunctions(self):
        return self.hdll.fmi2me_getCanNotUseMemoryManagementFunctions(self.fmu)

    def fmi2me_getCanGetAndSetFMUState(self):
        return self.hdll.fmi2me_getCanGetAndSetFMUState(self.fmu)

    def fmi2me_getCanSerializeFMUState(self):
        return self.hdll.fmi2me_getCanSerializeFMUState(self.fmu)

    def fmi2me_getProvidesDirectionalDerivative(self):
        return self.hdll.fmi2me_getProvidesDirectionalDerivative(self.fmu)

    def fmi2_getNumberOfContinuousStates(self):
        return self.hdll.fmi2_getNumberOfContinuousStates(self.fmu)
        
    def fmi2_getNumberOfEventIndicators(self):
        return self.hdll.fmi2_getNumberOfEventIndicators(self.fmu)

    def fmi2_getSupportsCoSimulation(self):
        return self.hdll.fmi2_getSupportsCoSimulation(self.fmu)

    def fmi2_getSupportsModelExchange(self):
        return self.hdll.fmi2_getSupportsModelExchange(self.fmu)

    def fmi2_getNumberOfModelStructureOutputs(self):
        return self.hdll.fmi2_getNumberOfModelStructureOutputs(self.fmu)
        
    def fmi2_getNumberOfModelStructureDerivatives(self):
        return self.hdll.fmi2_getNumberOfModelStructureDerivatives(self.fmu)
        
    def fmi2_getNumberOfModelStructureInitialUnknowns(self):
        return self.hdll.fmi2_getNumberOfModelStructureInitialUnknowns(self.fmu)
        
    def fmi2_getModelStructureOutput(self, i):
        return self.hdll.fmi2_getModelStructureOutput(self.fmu, i)
        
    def fmi2_getModelStructureDerivative(self, i):
        return self.hdll.fmi2_getModelStructureDerivative(self.fmu, i)        
        
    def fmi2_getModelStructureInitialUnknown(self, i):
        return self.hdll.fmi2_getModelStructureInitialUnknown(self.fmu, i)

    def fmi2_getModelStructureIndex(self, handle):
        return self.hdll.fmi2_getModelStructureIndex(handle)
        
    def fmi2_getModelStructureNumberOfDependencies(self, handle):
        return self.hdll.fmi2_getModelStructureNumberOfDependencies(handle)
        
    def fmi2_getModelStructureDependencyKindsDefined(self, handle):
        return self.hdll.fmi2_getModelStructureDependencyKindsDefined(handle)    
    
    def fmi2_getModelStructureDependencies(self, handle, nDependencies):
        int_array_type = ct.c_int * nDependencies
        dependencies = int_array_type()
        self.hdll.fmi2_getModelStructureDependencies(handle, dependencies, nDependencies)
        return list(dependencies)
    
    def fmi2_getModelStructureDependencyKinds(self, handle, nDependencies):
        int_array_type = ct.c_int * nDependencies
        dependencyKinds = int_array_type()
        self.hdll.fmi2_getModelStructureDependencyKinds(handle, dependencyKinds, nDependencies)
        return list(dependencyKinds)    

    def fmi2_instantiate(self, type, visible,  loggingOn):
        return self.hdll.fmi2_instantiate(self.fmu,  type, None, None, None, None, None,  visible,  loggingOn)

    def fmi2_freeInstance(self):
        return self.hdll.fmi2_freeInstance(self.fmu)

    def fmi2_setupExperiment(self, toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime):
        return self.hdll.fmi2_setupExperiment(self.fmu, toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime)

    def fmi2_enterInitializationMode(self):
        return self.hdll.fmi2_enterInitializationMode(self.fmu)

    def fmi2_exitInitializationMode(self):
        return self.hdll.fmi2_exitInitializationMode(self.fmu)

    def fmi2_terminate(self):
        return self.hdll.fmi2_terminate(self.fmu)

    def fmi2_reset(self):
        return self.hdll.fmi2_reset(self.fmu)

    def fmi2_getNumberOfUnits(self):
        return self.hdll.fmi2_getNumberOfUnits(self.fmu)

    def fmi2_getUnitByIndex(self, i):
        return self.hdll.fmi2_getUnitByIndex(self.fmu, i)

    def fmi2_getUnitName(self, unit):
        return self.hdll.fmi2_getUnitName(unit)

    def fmi2_hasBaseUnit(self, unit):
        return self.hdll.fmi2_hasBaseUnit(unit)

    def fmi2_getBaseUnit(self, unit):
        factor = ct.c_double()
        offset = ct.c_double()
        kg = ct.c_int()
        m = ct.c_int()
        s = ct.c_int()
        A = ct.c_int()
        K = ct.c_int()
        mol = ct.c_int()
        cd = ct.c_int()
        rad = ct.c_int()
        self.hdll.fmi2_getBaseUnit(unit, ct.byref(factor), ct.byref(offset), ct.byref(kg), ct.byref(m), ct.byref(s), ct.byref(A), ct.byref(K), ct.byref(mol), ct.byref(cd), ct.byref(rad))
        return (factor.value, offset.value, kg.value, m.value, s.value, A.value, K.value, mol.value, cd.value, rad.value)

    def fmi2_getNumberOfDisplayUnits(self, unit):
        return self.hdll.fmi2_getNumberOfDisplayUnits(unit)

    def fmi2_getDisplayUnitByIndex(self, unit, id):
        name = ct.c_char_p()
        factor = ct.c_double()
        offset = ct.c_double()
        self.hdll.fmi2_getDisplayUnitByIndex(unit, id, ct.byref(name), ct.byref(factor), ct.byref(offset))
        return (name.value.decode(), factor.value, offset.value)

    def fmi2_getReal(self, valueReferences, nValueReferences):
        uint_array_type = ct.c_uint * nValueReferences
        double_array_type = ct.c_double * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = double_array_type()
        success = self.hdll.fmi2_getReal(self.fmu, valueReferencesArray, nValueReferences, valuesArray)
        return [success, list(valuesArray)]

    def fmi2_getInteger(self, valueReferences, nValueReferences):
        uint_array_type = ct.c_uint * nValueReferences
        double_array_type = ct.c_int * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = double_array_type(*[1]*nValueReferences)
        success = self.hdll.fmi2_getInteger(self.fmu, valueReferencesArray, nValueReferences, valuesArray)
        return [success, list(valuesArray)]

    def fmi2_getBoolean(self, valueReferences, nValueReferences):
        uint_array_type = ct.c_uint * nValueReferences
        double_array_type = ct.c_bool * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = double_array_type(*[1]*nValueReferences)
        success = self.hdll.fmi2_getBoolean(self.fmu, valueReferencesArray, nValueReferences, valuesArray)
        return [success, list(valuesArray)]

    def fmi2_getString(self, valueReferences, nValueReferences):
        uint_array_type = ct.c_uint * nValueReferences
        double_array_type = ct.c_char_p * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = double_array_type(*[1]*nValueReferences)
        success = self.hdll.fmi2_getString(self.fmu, valueReferencesArray, nValueReferences, valuesArray)
        return [success, list(valuesArray)]

    def fmi2_setReal(self, valueReferences,  nValueReferences, values):
        uint_array_type = ct.c_uint * nValueReferences
        double_array_type = ct.c_double * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = double_array_type(*values)
        return self.hdll.fmi2_setReal(self.fmu, valueReferencesArray, nValueReferences, valuesArray)

    def fmi2_setInteger(self, valueReferences, nValueReferences, values):
        uint_array_type = ct.c_uint * nValueReferences
        double_array_type = ct.c_int * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = double_array_type(*values)
        return self.hdll.fmi2_setInteger(self.fmu, valueReferencesArray, nValueReferences, valuesArray)

    def fmi2_setBoolean(self, valueReferences, nValueReferences, values):
        uint_array_type = ct.c_uint * nValueReferences
        double_array_type = ct.c_bool * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = double_array_type(*values)
        return self.hdll.fmi2_setBoolean(self.fmu, valueReferencesArray, nValueReferences, valuesArray)

    def fmi2_setString(self, valueReferences, nValueReferences, values):
        uint_array_type = ct.c_uint * nValueReferences
        double_array_type = ct.c_char_p * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = double_array_type(*values)
        return self.hdll.fmi2_setString(self.fmu, valueReferencesArray, nValueReferences, valuesArray)

    def fmi2_getFMUstate(self, FMUstate):
        return self.hdll.fmi2_getFMUstate(self.fmu, FMUstate)

    def fmi2_setFMUstate(self, FMUstate):
        return self.hdll.fmi2_setFMUstate(self.fmu, FMUstate)

    def fmi2_freeFMUstate(self, FMUstate):
        return self.hdll.fmi2_freeFMUstate(self.fmu, FMUstate)

    def fmi2_serializedFMUstateSize(self, FMUstate,  size):
        return self.hdll.fmi2_serializedFMUstateSize(self.fmu, FMUstate,  size)

    def fmi2_serializeFMUstate(self, FMUstate, serializedState,  size):
        return self.hdll.fmi2_serializeFMUstate(self.fmu, FMUstate, serializedState,  size)

    def fmi2_deSerializeFMUstate(self, serializedState,  size, FMUstate):
        return self.hdll.fmi2_deSerializeFMUstate(self.fmu, serializedState,  size, FMUstate)

    def fmi2_getDirectionalDerivative(self, unknownReferences,  nUnknown, knownReferences,  nKnown, dvKnown, dvUnknown):
        return self.hdll.fmi2_getDirectionalDerivative(self.fmu, unknownReferences,  nUnknown, knownReferences,  nKnown, dvKnown, dvUnknown)

    def fmi2_enterEventMode(self):
        return self.hdll.fmi2_enterEventMode(self.fmu)

    def fmi2_newDiscreteStates(self, eventInfo):
        return self.hdll.fmi2_newDiscreteStates(self.fmu, eventInfo)

    def fmi2_enterContinuousTimeMode(self):
        return self.hdll.fmi2_enterContinuousTimeMode(self.fmu)

    def fmi2_completedIntegratorStep(self,  noSetFMUStatePriorToCurrentPoint):
        enterEventMode = ct.c_bool()
        terminateSimulation = ct.c_bool()
        success = self.hdll.fmi2_completedIntegratorStep(self.fmu,  noSetFMUStatePriorToCurrentPoint,  ct.byref(enterEventMode),  ct.byref(terminateSimulation))
        return (success, enterEventMode.value, terminateSimulation.value)

    def fmi2_setTime(self, time):
        return self.hdll.fmi2_setTime(self.fmu, time)

    def fmi2_setContinuousStates(self, continuousStates, nContinuousStates):
        double_array_type = ct.c_double * nContinuousStates
        continuousStatesArray = double_array_type(*continuousStates)
        return self.hdll.fmi2_setContinuousStates(self.fmu, continuousStatesArray, nContinuousStates)

    def fmi2_getDerivatives(self, nx):
        double_array_type = ct.c_double * nx
        derivativesArray = double_array_type()
        success = self.hdll.fmi2_getDerivatives(self.fmu, derivativesArray, nx)
        return [success, list(derivativesArray)]

    def fmi2_getEventIndicators(self, eventIndicators,  ni):
        double_array_type = ct.c_double * nx
        eventIndicatorsArray = double_array_type()
        success =  self.hdll.fmi2_getEventIndicators(self.fmu, eventIndicatorsArray,  ni)
        return [success, list(eventIndicatorsArray)]
    
    def fmi2_getContinuousStates(self, nStates):
        double_array_type = ct.c_double * nStates
        statesArray = double_array_type()
        success = self.hdll.fmi2_getContinuousStates(self.fmu, statesArray, nStates)
        return [success, list(statesArray)]

    def fmi2_getNominalsOfContinuousStates(self, nNominals):
        double_array_type = ct.c_double * nNominals
        nominalsArray = double_array_type()
        success = self.hdll.fmi2_getNominalsOfContinuousStates(self.fmu, nominalsArray, nNominals)
        return [success, list(nominalsArray)]
        
    def fmi2_setRealInputDerivatives(self, vr,  nvr,  order, value):
        return self.hdll.fmi2_setRealInputDerivatives(self.fmu, vr,  nvr,  order, value)

    def fmi2_getRealOutputDerivatives(self, vr,  nvr,  order, value):
        return self.hdll.fmi2_getRealOutputDerivatives(self.fmu, vr,  nvr,  order, value)

    def fmi2_doStep(self, currentCommunicationPoint, communicationStepSize, noSetFMUStatePriorToCurrentPoint):
        return self.hdll.fmi2_doStep(self.fmu, currentCommunicationPoint, communicationStepSize, noSetFMUStatePriorToCurrentPoint)

    def fmi2_cancelStep(self):
        return self.hdll.fmi2_cancelStep(self.fmu)

    def fmi2_getStatus(self, s, value):
        return self.hdll.fmi2_getStatus(self.fmu, s, value)

    def fmi2_getRealStatus(self, s, value):
        return self.hdll.fmi2_getRealStatus(self.fmu, s, value)

    def fmi2_getIntegerStatus(self, s, value):
        return self.hdll.fmi2_getIntegerStatus(self.fmu, s, value)

    def fmi2_getBooleanStatus(self, s,  value):
        return self.hdll.fmi2_getBooleanStatus(self.fmu, s,  value)

    def fmi2_getStringStatus(self, s, value):
        return self.hdll.fmi2_getStringStatus(self.fmu, s, value)

    #FMI3 functions

    def fmi3_modelName(self):
        ret = self.hdll.fmi3_modelName(self.fmu)
        if ret is None:
            return ""
        else:
            return ret.decode()

    def fmi3_instantiationToken(self):
        ret = self.hdll.fmi3_instantiationToken(self.fmu)
        if ret is None:
            return ""
        else:
            return ret.decode()

    def fmi3_description(self):
        ret = self.hdll.fmi3_description(self.fmu)
        if ret is None:
            return ""
        else:
            return ret.decode()
            
    def fmi3_author(self):
        ret = self.hdll.fmi3_author(self.fmu)
        if ret is None:
            return ""
        else:
            return ret.decode()
            
    def fmi3_version(self):
        ret = self.hdll.fmi3_version(self.fmu)
        if ret is None:
            return ""
        else:
            return ret.decode()

    def fmi3_copyright(self):
        ret = self.hdll.fmi3_copyright(self.fmu)
        if ret is None:
            return ""
        else:
            return ret.decode()

    def fmi3_license(self):
        ret = self.hdll.fmi3_license(self.fmu)
        if ret is None:
            return ""
        else:
            return ret.decode()

    def fmi3_generationTool(self):
        ret =self.hdll.fmi3_generationTool(self.fmu)
        if ret is None:
            return ""
        else:
            return ret.decode()
            
    def fmi3_generationDateAndTime(self):
        ret = self.hdll.fmi3_generationDateAndTime(self.fmu)
        if ret is None:
            return ""
        else:
            return ret.decode()

    def fmi3_variableNamingConvention(self):
        return self.hdll.fmi3_variableNamingConvention(self.fmu).decode()

    def fmi3_supportsModelExchange(self):
        return self.hdll.fmi3_supportsModelExchange(self.fmu)

    def fmi3_supportsScheduledExecution(self):
        return self.hdll.fmi3_supportsScheduledExecution(self.fmu)

    def fmi3_supportsCoSimulation(self):
        return self.hdll.fmi3_supportsCoSimulation(self.fmu)

    def fmi3_defaultStartTimeDefined(self):
        return self.hdll.fmi3_defaultStartTimeDefined(self.fmu)

    def fmi3_defaultStopTimeDefined(self):
        return self.hdll.fmi3_defaultStopTimeDefined(self.fmu)

    def fmi3_defaultToleranceDefined(self):
        return self.hdll.fmi3_defaultToleranceDefined(self.fmu)

    def fmi3_defaultStepSizeDefined(self):
        return self.hdll.fmi3_defaultStepSizeDefined(self.fmu)

    def fmi3_getDefaultStartTime(self):
        return self.hdll.fmi3_getDefaultStartTime(self.fmu)

    def fmi3_getDefaultStopTime(self):
        return self.hdll.fmi3_getDefaultStopTime(self.fmu)

    def fmi3_getDefaultTolerance(self):
        return self.hdll.fmi3_getDefaultTolerance(self.fmu)

    def fmi3_getDefaultStepSize(self):
        return self.hdll.fmi3_getDefaultStepSize(self.fmu)


    def fmi3_getNumberOfVariables(self):
        return self.hdll.fmi3_getNumberOfVariables(self.fmu)

    def fmi3_getVariableByName(self, name):
        return self.hdll.fmi3_getVariableByName(self.fmu, name.encode())

    def fmi3_getVariableByIndex(self, i):
        return self.hdll.fmi3_getVariableByIndex(self.fmu, i)

    def fmi3_getVariableByValueReference(self, vr):
        return self.hdll.fmi3_getVariableByValueReference(self.fmu, vr)

    def fmi3_getVariableName(self, var):
        return self.hdll.fmi3_getVariableName(var).decode()

    def fmi3_getVariableCausality(self, var):
        return self.fmi3CausalityStrings[self.hdll.fmi3_getVariableCausality(var)]

    def fmi3_getVariableVariability(self, var):
        return self.fmi3VariabilityStrings[self.hdll.fmi3_getVariableVariability(var)]

    def fmi3_getVariableInitial(self, var):
        return self.fmi3InitialStrings[self.hdll.fmi3_getVariableInitial(var)]

    def fmi3_getVariableSupportsIntermediateUpdate(self, var):
        return self.hdll.fmi3_getVariableSupportsIntermediateUpdate(var)

    def fmi3_getVariableDataType(self, var):
        return self.fmi3DataTypeStrings[self.hdll.fmi3_getVariableDataType(var)]

    def fmi3_getVariableDescription(self, var):
        return self.hdll.fmi3_getVariableDescription(var).decode()

    def fmi3_getVariableQuantity(self, var):
        ret = self.hdll.fmi3_getVariableQuantity(var)
        if ret is None:
            return ""
        else:
            return ret.decode()    

    def fmi3_getVariableUnit(self, var):
        ret = self.hdll.fmi3_getVariableUnit(var)
        if ret is None:
            return ""
        else:
            return ret.decode()  

    def fmi3_getVariableDisplayUnit(self, var):
        ret = self.hdll.fmi3_getVariableDisplayUnit(var)
        if ret is None:
            return ""
        else:
            return ret.decode()  

    def fmi3_getVariableHasStartValue(self, var):
        return self.hdll.fmi3_getVariableHasStartValue(var)

    def fmi3_getVariableStartFloat64(self, var):
        return self.hdll.fmi3_getVariableStartFloat64(var)

    def fmi3_getVariableStartFloat32(self, var):
        return self.hdll.fmi3_getVariableStartFloat32(var)  

    def fmi3_getVariableStartInt64(self, var):
        return self.hdll.fmi3_getVariableStartInt64(var)

    def fmi3_getVariableStartInt32(self, var):
        return self.hdll.fmi3_getVariableStartInt32(var)

    def fmi3_getVariableStartInt16(self, var):
        return self.hdll.fmi3_getVariableStartInt16(var)

    def fmi3_getVariableStartInt8(self, var):
        return self.hdll.fmi3_getVariableStartInt8(var)

    def fmi3_getVariableStartUInt64(self, var):
        return self.hdll.fmi3_getVariableStartUInt64(var)

    def fmi3_getVariableStartUInt32(self, var):
        return self.hdll.fmi3_getVariableStartUInt32(var)

    def fmi3_getVariableStartUInt16(self, var):
        return self.hdll.fmi3_getVariableStartUInt16(var)

    def fmi3_getVariableStartUInt8(self, var):
        return self.hdll.fmi3_getVariableStartUInt8(var)

    def fmi3_getVariableStartBoolean(self, var):
        return self.hdll.fmi3_getVariableStartBoolean(var)

    def fmi3_getVariableStartString(self, var):
        return self.hdll.fmi3_getVariableStartString(var)

    def fmi3_getVariableStartBinary(self, var):
        return self.hdll.fmi3_getVariableStartBinary(var)

    def fmi3_getVariableValueReference(self, var):
        return self.hdll.fmi3_getVariableValueReference(var)

    def fmi3_getNumberOfUnits(self):
        return self.hdll.fmi3_getNumberOfUnits(self.fmu)

    def fmi3_getUnitByIndex(self, i):
        return self.hdll.fmi3_getUnitByIndex(self.fmu, i)

    def fmi3_getUnitName(self, unit):
        return self.hdll.fmi3_getUnitName(unit)

    def fmi3_hasBaseUnit(self, unit):
        return self.hdll.fmi3_hasBaseUnit(unit)
        
    def fmi3_getBaseUnit(self, unit):
        factor = ct.c_double()
        offset = ct.c_double()
        kg = ct.c_int()
        m = ct.c_int()
        s = ct.c_int()
        A = ct.c_int()
        K = ct.c_int()
        mol = ct.c_int()
        cd = ct.c_int()
        rad = ct.c_int()
        self.hdll.fmi3_getBaseUnit(unit, ct.byref(factor), ct.byref(offset), ct.byref(kg), ct.byref(m), ct.byref(s), ct.byref(A), ct.byref(K), ct.byref(mol), ct.byref(cd), ct.byref(rad))
        return (factor.value, offset.value, kg.value, m.value, s.value, A.value, K.value, mol.value, cd.value, rad.value)
    
    def fmi3_getNumberOfDisplayUnits(self, unit):
        return self.hdll.fmi3_getNumberOfDisplayUnits(unit)
    
    def fmi3_getDisplayUnitByIndex(self, unit, id):
        name = ct.c_char_p()
        factor = ct.c_double()
        offset = ct.c_double()
        inverse = ct.c_bool()
        self.hdll.fmi3_getDisplayUnitByIndex(unit, id, ct.byref(name), ct.byref(factor), ct.byref(offset), ct.byref(inverse))
        return (name.value.decode(), factor.value, offset.value, inverse.value)

    def fmi3_getFloat64Type(self, name):
        description = ct.c_char_p()
        quantity = ct.c_char_p()
        unit = ct.c_char_p()
        displayUnit = ct.c_char_p()
        relativeQuantity = ct.c_bool()
        unbounded = ct.c_bool()
        min = ct.c_double()
        max = ct.c_double()
        nominal = ct.c_double()
        self.hdll.fmi3_getFloat64Type(self.fmu, name.encode(), ct.byref(description), ct.byref(quantity), ct.byref(unit), ct.byref(displayUnit), ct.byref(relativeQuantity), ct.byref(unbounded), ct.byref(min), ct.byref(max), ct.byref(nominal))
        return (description.value.decode(), quantity.value.decode(), unit.value.decode(), displayUnit.value.decode(), relativeQuantity.value, unbounded.value, min.value, max.value, nominal.value)
  
    def fmi3_getFloat32Type(self, name):
        description = ct.c_char_p()
        quantity = ct.c_char_p()
        unit = ct.c_char_p()
        displayUnit = ct.c_char_p()
        relativeQuantity = ct.c_bool()
        unbounded = ct.c_bool()
        min = ct.c_float()
        max = ct.c_float()
        nominal = ct.c_float()
        self.hdll.fmi3_getFloat32Type(self.fmu, name.encode(), ct.byref(description), ct.byref(quantity), ct.byref(unit), ct.byref(displayUnit), ct.byref(relativeQuantity), ct.byref(unbounded), ct.byref(min), ct.byref(max), ct.byref(nominal))
        return (description.value.decode(), quantity.value.decode(), unit.value.decode(), displayUnit.value.decode(), relativeQuantity.value, unbounded.value, min.value, max.value, nominal.value)
  
    def fmi3_getInt64Type(self, name):
        description = ct.c_char_p()
        quantity = ct.c_char_p()
        min = ct.c_int64()
        max = ct.c_int64()
        self.hdll.fmi3_getInt64Type(self.fmu, name.encode(), ct.byref(description), ct.byref(quantity), ct.byref(min), ct.byref(max))
        return (description.value.decode(), quantity.value.decode(), unbounded.value, min.value, max.value)

    def fmi3_getInt32Type(self, name):
        description = ct.c_char_p()
        quantity = ct.c_char_p()
        min = ct.c_int32()
        max = ct.c_int32()
        self.hdll.fmi3_getInt32Type(self.fmu, name.encode(), ct.byref(description), ct.byref(quantity), ct.byref(min), ct.byref(max))
        return (description.value.decode(), quantity.value.decode(), unbounded.value, min.value, max.value)
        
    def fmi3_getInt16Type(self, name):
        description = ct.c_char_p()
        quantity = ct.c_char_p()
        min = ct.c_int16()
        max = ct.c_int16()
        self.hdll.fmi3_getInt16Type(self.fmu, name.encode(), ct.byref(description), ct.byref(quantity), ct.byref(min), ct.byref(max))
        return (description.value.decode(), quantity.value.decode(), unbounded.value, min.value, max.value)

    def fmi3_getInt8Type(self, name):
        description = ct.c_char_p()
        quantity = ct.c_char_p()
        min = ct.c_int8()
        max = ct.c_int8()
        self.hdll.fmi3_getInt8Type(self.fmu, name.encode(), ct.byref(description), ct.byref(quantity), ct.byref(min), ct.byref(max))
        return (description.value.decode(), quantity.value.decode(), unbounded.value, min.value, max.value)

    def fmi3_getUInt64Type(self, name):
        description = ct.c_char_p()
        quantity = ct.c_char_p()
        min = ct.c_uint64()
        max = ct.c_uint64()
        self.hdll.fmi3_getUInt64Type(self.fmu, name.encode(), ct.byref(description), ct.byref(quantity), ct.byref(min), ct.byref(max))
        return (description.value.decode(), quantity.value.decode(), unbounded.value, min.value, max.value)

    def fmi3_getUInt32Type(self, name):
        description = ct.c_char_p()
        quantity = ct.c_char_p()
        min = ct.c_uint32()
        max = ct.c_uint32()
        self.hdll.fmi3_getUInt32Type(self.fmu, name.encode(), ct.byref(description), ct.byref(quantity), ct.byref(min), ct.byref(max))
        return (description.value.decode(), quantity.value.decode(), unbounded.value, min.value, max.value)

    def fmi3_getUInt16Type(self, name):
        description = ct.c_char_p()
        quantity = ct.c_char_p()
        min = ct.c_uint16()
        max = ct.c_uint16()
        self.hdll.fmi3_getUInt16Type(self.fmu, name.encode(), ct.byref(description), ct.byref(quantity), ct.byref(min), ct.byref(max))
        return (description.value.decode(), quantity.value.decode(), unbounded.value, min.value, max.value)              
    
    def fmi3_getUInt8Type(self, name):
        description = ct.c_char_p()
        quantity = ct.c_char_p()
        min = ct.c_uint8()
        max = ct.c_uint8()
        self.hdll.fmi3_getUInt8Type(self.fmu, name.encode(), ct.byref(description), ct.byref(quantity), ct.byref(min), ct.byref(max))
        return (description.value.decode(), quantity.value.decode(), unbounded.value, min.value, max.value)        
        
    def fmi3_getBooleanType(self, name):
        description = ct.c_char_p()
        self.hdll.fmi3_getBooleanType(self.fmu, name.encode(), ct.byref(description))
        return description.value.decode()        
        
    def fmi3_getStringType(self, name):
        description = ct.c_char_p()
        self.hdll.fmi3_getStringType(self.fmu, name.encode(), ct.byref(description))
        return description.value.decode()        

    def fmi3_getBinaryType(self, name):
        description = ct.c_char_p()
        mimeType = ct.c_char_p()
        maxSize = ct.c_uint32()
        self.hdll.fmi3_getBinaryType(self.fmu, name.encode(), ct.byref(description), ct.byref(mimeType), ct.byref(maxSize))
        return (description.value.decode(), mimeType.decode(), maxSize)
        
    def fmi3_getEnumerationType(self, name):
        description = ct.c_char_p()
        quantity = ct.c_char_p()
        min = ct.c_int64()
        max = ct.c_int64()
        numberOfItems = ct.c_int()
        self.hdll.fmi3_getEnumerationType(self.fmu, name.encode(), ct.byref(description), ct.byref(quantity), ct.byref(min), ct.byref(max), ct.byref(numberOfItems))
        return (description.value.decode(), quantity.value.decode(), min.value, max.value, numberOfItems.value)

    def fmi3_getEnumerationItem(self, name, typeName, itemId):
        itemName = ct.c_char_p()
        value = ct.c_int64()
        description = ct.c_char_p()
        self.hdll.fmi3_getEnumerationItem(self.fmu, name.encode(), typeName.encode(), itemId, ct.byref(itemName), ct.byref(value), ct.byref(description))
        return (itemName.value.decode(), value.value, description.value.decode())
        
    def fmi3_getClockType(self, name):
        description = ct.c_char_p()
        canBeDeactivated = ct.c_bool()
        priority = ct.c_int32()
        intervalVariability = ct.c_int()
        intervalDecimal = ct.c_float()
        shiftDecimal = ct.c_float()
        supportsFraction = ct.c_bool()
        resolution = ct.c_uint64()
        intervalCounter = ct.c_uint64(),
        shiftCounter = ct.c_uint64(),
        self.hdll.fmi3_getClockType(self.fmu, name.encode(), ct.byref(description), ct.byref(quantity), ct.byref(min), ct.byref(max), ct.byref(numberOfItems))
        return (description.value.decode(), quantity.value.decode(), min.value, max.value, numberOfItems.value)

    def fmi3_getNumberOfLogCategories(self):
        return self.hdll.fmi3_getNumberOfLogCategories(self.fmu)

    def fmi3_getLogCategory(self, id):
        name = ct.c_char_p()
        description = ct.c_char_p()
        self.hdll.fmi3_getLogCategory(self.fmu, id, ct.byref(name), ct.byref(description))
        return (name.value.decode(), description.value.decode())

    def fmi3_getNumberOfModelStructureOutputs(self):
        return self.hdll.fmi3_getNumberOfModelStructureOutputs(self.fmu)
        
    def fmi3_getNumberOfModelStructureContinuousStateDerivatives(self):
        return self.hdll.fmi3_getNumberOfModelStructureContinuousStateDerivatives(self.fmu)
    
    def fmi3_getNumberOfModelStructureClockedStates(self):
        return self.hdll.fmi3_getNumberOfModelStructureClockedStates(self.fmu)
        
    def fmi3_getNumberOfModelStructureEventIndicators(self):
        return self.hdll.fmi3_getNumberOfModelStructureEventIndicators(self.fmu)
    
    def fmi3_getNumberOfModelStructureInitialUnknowns(self):
        return self.hdll.fmi3_getNumberOfModelStructureInitialUnknowns(self.fmu)
        
    def fmi3_getModelStructureOutput(self, i):
        return self.hdll.fmi3_getModelStructureOutput(self.fmu, i)
        
    def fmi3_getModelStructureContinuousStateDerivative(self, i):
        return self.hdll.fmi3_getModelStructureContinuousStateDerivative(self.fmu, i)        
        
    def fmi3_getModelStructureInitialUnknown(self, i):
        return self.hdll.fmi3_getModelStructureInitialUnknown(self.fmu, i)

    def fmi3_getModelStructureValueReference(self, handle):
        return self.hdll.fmi3_getModelStructureValueReference(handle)
        
    def fmi3_getModelStructureNumberOfDependencies(self, handle):
        return self.hdll.fmi3_getModelStructureNumberOfDependencies(handle)
        
    def fmi3_getModelStructureDependencyKindsDefined(self, handle):
        return self.hdll.fmi3_getModelStructureDependencyKindsDefined(handle)    
    
    def fmi3_getModelStructureDependencies(self, handle, nDependencies):
        int_array_type = ct.c_int * nDependencies
        dependencies = int_array_type()
        self.hdll.fmi3_getModelStructureDependencies(handle, dependencies, nDependencies)
        return list(dependencies)
    
    def fmi3_getModelStructureDependencyKinds(self, handle, nDependencies):
        int_array_type = ct.c_int * nDependencies
        dependencyKinds = int_array_type()
        self.hdll.fmi3_getModelStructureDependencyKinds(handle, dependencyKinds, nDependencies)
        return list(dependencyKinds)    
        
    def fmi3cs_getModelIdentifier(self):
        ret = self.hdll.fmi3cs_getModelIdentifier(self.fmu)
        if ret is None:
            return ""
        else:
            return ret.decode()   
            
    def fmi3cs_getNeedsExecutionTool(self):
        return self.hdll.fmi3cs_getNeedsExecutionTool(self.fmu)
        
    def fmi3cs_getCanBeInstantiatedOnlyOncePerProcess(self):
        return self.hdll.fmi3cs_getCanBeInstantiatedOnlyOncePerProcess(self.fmu)
        
    def fmi3cs_getCanGetAndSetFMUState(self):
        return self.hdll.fmi3cs_getCanGetAndSetFMUState(self.fmu)
        
    def fmi3cs_getCanSerializeFMUState(self):
        return self.hdll.fmi3cs_getCanSerializeFMUState(self.fmu)
        
    def fmi3cs_getProvidesDirectionalDerivative(self):
        return self.hdll.fmi3cs_getProvidesDirectionalDerivative(self.fmu)
        
    def fmi3cs_getProvidesAdjointDerivatives(self):
        return self.hdll.fmi3cs_getProvidesAdjointDerivatives(self.fmu)
        
    def fmi3cs_getProvidesPerElementDependencies(self):
        return self.hdll.fmi3cs_getProvidesPerElementDependencies(self.fmu)
        
    def fmi3cs_getProvidesIntermediateUpdate(self):
        return self.hdll.fmi3cs_getProvidesIntermediateUpdate(self.fmu)
        
    def fmi3cs_getProvidesEvaluateDiscreteStates(self):
        return self.hdll.fmi3cs_getProvidesEvaluateDiscreteStates(self.fmu)
        
    def fmi3cs_getHasEventMode(self):
        return self.hdll.fmi3cs_getHasEventMode(self.fmu)
        
    def fmi3cs_getRecommendedIntermediateInputSmoothness(self):
        return self.hdll.fmi3cs_getRecommendedIntermediateInputSmoothness(self.fmu)
    
    def fmi3cs_getMaxOutputDerivativeOrder(self):
        return self.hdll.fmi3cs_getMaxOutputDerivativeOrder(self.fmu)
        
    def fmi3cs_getCanHandleVariableCommunicationStepSize(self):
        return self.hdll.fmi3cs_getCanHandleVariableCommunicationStepSize(self.fmu)
        
    def fmi3cs_getCanReturnEarlyAfterIntermediateUpdate(self):
        return self.hdll.fmi3cs_getCanReturnEarlyAfterIntermediateUpdate(self.fmu)
        
    def fmi3cs_getFixedInternalStepSize(self):
        return self.hdll.fmi3cs_getFixedInternalStepSize(self.fmu)
        
    def fmi3me_getModelIdentifier(self):
        ret = self.hdll.fmi3me_getModelIdentifier(self.fmu)
        if ret is None:
            return ""
        else:
            return ret.decode()   
            
    def fmi3me_getNeedsExecutionTool(self):
        return self.hdll.fmi3me_getNeedsExecutionTool(self.fmu)

    def fmi3me_getCanBeInstantiatedOnlyOncePerProcess(self):
        return self.hdll.fmi3me_getCanBeInstantiatedOnlyOncePerProcess(self.fmu)

    def fmi3me_getCanGetAndSetFMUState(self):
        return self.hdll.fmi3me_getCanGetAndSetFMUState(self.fmu)

    def fmi3me_getCanSerializeFMUState(self):
        return self.hdll.fmi3me_getCanSerializeFMUState(self.fmu)

    def fmi3me_getProvidesDirectionalDerivative(self):
        return self.hdll.fmi3me_getProvidesDirectionalDerivative(self.fmu)

    def fmi3me_getProvidesAdjointDerivatives(self):
        return self.hdll.fmi3me_getProvidesAdjointDerivatives(self.fmu)

    def fmi3me_getProvidesPerElementDependencies(self):
        return self.hdll.fmi3me_getProvidesPerElementDependencies(self.fmu)

    def fmi3me_getProvidesEvaluateDiscreteStates(self):
        return self.hdll.fmi3me_getProvidesEvaluateDiscreteStates(self.fmu)

    def fmi3me_getNeedsCompletedIntegratorStep(self):
        return self.hdll.fmi3me_getNeedsCompletedIntegratorStep(self.fmu)

    def fmi3se_getModelIdentifier(self):
        ret = self.hdll.fmi3se_getModelIdentifier(self.fmu)
        if ret is None:
            return ""
        else:
            return ret.decode()   

    def fmi3se_getNeedsExecutionTool(self):
        return self.hdll.fmi3se_getNeedsExecutionTool(self.fmu)

    def fmi3se_getCanBeInstantiatedOnlyOncePerProcess(self):
        return self.hdll.fmi3se_getCanBeInstantiatedOnlyOncePerProcess(self.fmu)

    def fmi3se_getCanGetAndSetFMUState(self):
        return self.hdll.fmi3se_getCanGetAndSetFMUState(self.fmu)

    def fmi3se_getCanSerializeFMUState(self):
        return self.hdll.fmi3se_getCanSerializeFMUState(self.fmu)

    def fmi3se_getProvidesDirectionalDerivative(self):
        return self.hdll.fmi3se_getProvidesDirectionalDerivative(self.fmu)

    def fmi3se_getProvidesAdjointDerivatives(self):
        return self.hdll.fmi3se_getProvidesAdjointDerivatives(self.fmu)

    def fmi3se_getProvidesPerElementDependencies(self):
        return self.hdll.fmi3se_getProvidesPerElementDependencies(self.fmu)

    def fmi3_instantiateCoSimulation(self, visible, loggingOn, eventModeUsed, earlyReturnAllowed, requiredIntermediateVariables, nRequiredIntermediateVariables):
        uint_array_type = ct.c_uint * nRequiredIntermediateVariables
        double_array_type = ct.c_double * nRequiredIntermediateVariables
        valueReferencesArray = uint_array_type(*requiredIntermediateVariables)
        return self.hdll.fmi3_instantiateCoSimulation(self.fmu, visible, loggingOn, eventModeUsed, earlyReturnAllowed, valueReferencesArray, nRequiredIntermediateVariables, self.fmu, None, None)

    def fmi3_instantiateModelExchange(self, visible, loggingOn):
        return self.hdll.fmi3_instantiateModelExchange(self.fmu, visible, loggingOn, self.fmu, None)

    def fmi3_getVersion(self):
        return self.hdll.fmi3_getVersion(self.fmu).decode()

    def fmi3_setDebugLogging(self, loggingOn, nCategories, categories):
        return self.hdll.fmi3_setDebugLogging(self.fmu, loggingOn, nCategories, categories)

    def fmi3_enterInitializationMode(self,  toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime):
        return self.hdll.fmi3_enterInitializationMode(self.fmu, toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime)

    def fmi3_exitInitializationMode(self):
        return self.hdll.fmi3_exitInitializationMode(self.fmu)

    def fmi3_terminate(self):
        return self.hdll.fmi3_terminate(self.fmu)

    def fmi3_freeInstance(self):
        return self.hdll.fmi3_freeInstance(self.fmu)

    def fmi3_doStep(self,  currentCommunicationPoint, communicationStepSize, noSetFMUStatePriorToCurrentPoint):
        eventEncountered = ct.c_bool()
        terminateSimulation = ct.c_bool()
        earlyReturn = ct.c_bool()
        LastSuccessfulTime = ct.c_double()
        success = self.hdll.fmi3_doStep(self.fmu, currentCommunicationPoint, communicationStepSize, noSetFMUStatePriorToCurrentPoint, ct.byref(eventEncountered), ct.byref(terminateSimulation), ct.byref(earlyReturn), ct.byref(LastSuccessfulTime))
        return (success, eventEncountered.value, terminateSimulation.value, earlyReturn.value, LastSuccessfulTime.value)

    def fmi3_enterEventMode(self):
        return self.hdll.fmi3_enterEventMode(self.fmu)

    def fmi3_reset(self):
        return self.hdll.fmi3_reset(self.fmu)

    def fmi3_getFloat64(self, valueReferences, nValueReferences, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        double_array_type = ct.c_double * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = double_array_type()
        success = self.hdll.fmi3_getFloat64(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)
        return [success, list(valuesArray)]

    def fmi3_getFloat32(self, valueReferences, nValueReferences, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        float_array_type = ct.c_float * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = float_array_type()
        success = self.hdll.fmi3_getFloat32(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)
        return [success, list(valuesArray)]

    def fmi3_getInt8(self, valueReferences, nValueReferences, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        int8_array_type = ct.c_int8 * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = int8_array_type()
        success = self.hdll.fmi3_getInt8(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)
        return [success, list(valuesArray)]

    def fmi3_getUInt8(self, valueReferences, nValueReferences, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        uint8_array_type = ct.c_uint8 * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = uint8_array_type()
        success = self.hdll.fmi3_getUInt8(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)
        return [success, list(valuesArray)]

    def fmi3_getInt16(self, valueReferences, nValueReferences, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        int16_array_type = ct.c_int16 * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = int16_array_type()
        success = self.hdll.fmi3_getInt16(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)
        return [success, list(valuesArray)]

    def fmi3_getUInt16(self, valueReferences, nValueReferences, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        uint16_array_type = ct.c_uint16 * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = uint16_array_type()
        success = self.hdll.fmi3_getUInt16(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)
        return [success, list(valuesArray)]

    def fmi3_getUInt32(self, valueReferences, nValueReferences, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        int32_array_type = ct.c_int32 * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = int32_array_type()
        success = self.hdll.fmi3_getUInt32(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)
        return [success, list(valuesArray)]

    def fmi3_getUInt32(self, valueReferences, nValueReferences, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        uint32_array_type = ct.c_uint32 * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = uint32_array_type()
        success = self.hdll.fmi3_getUInt32(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)
        return [success, list(valuesArray)]

    def fmi3_getInt64(self, valueReferences, nValueReferences, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        int64_array_type = ct.c_int64 * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = int64_array_type()
        success = self.hdll.fmi3_getInt64(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)
        return [success, list(valuesArray)]

    def fmi3_getUInt64(self, valueReferences, nValueReferences, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        uint64_array_type = ct.c_uint64 * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = uint64_array_type()
        success = self.hdll.fmi3_getUInt64(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)
        return [success, list(valuesArray)]

    def fmi3_getBoolean(self, valueReferences, nValueReferences, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        bool_array_type = ct.c_bool * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = bool_array_type()
        success = self.hdll.fmi3_getBoolean(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)
        return [success, list(valuesArray)]

    def fmi3_getString(self, valueReferences, nValueReferences, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        string_array_type = ct.c_char_p * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = string_array_type()
        success = self.hdll.fmi3_getString(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)
        return [success, list(valuesArray)]

    def fmi3_getBinary(self, valueReferences, nValueReferences, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        size_t_array_type = ct.c_size_t * nValuesReferences
        binary_array_type = ct.POINTER(ct.c_uint8) * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valueSizesArray = size_t_array_type()
        valuesArray = binary_array_type()
        success = self.hdll.fmi3_getBinary(self.fmu, valueReferencesArray, nValueReferences, valueSizesArray, valuesArray, nValues)
        return [success, list(valueSizesArray), list(valuesArray)]

    def fmi3_getClock(self, valueReferences, nValueReferences, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        bool_array_type = ct.c_bool * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = bool_array_type()
        success = self.hdll.fmi3_getClock(self.fmu, valueReferencesArray, nValueReferences, valuesArray)
        return [success, list(valuesArray)]

    def fmi3_setFloat64(self, valueReferences, nValueReferences, values, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        double_array_type = ct.c_double * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = double_array_type(*values)
        return self.hdll.fmi3_setFloat64(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)

    def fmi3_setFloat32(self, valueReferences, nValueReferences,values, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        float_array_type = ct.c_float * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = float_array_type(*values)
        return self.hdll.fmi3_setFloat32(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)

    def fmi3_setInt8(self, valueReferences, nValueReferences,values, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        int8_array_type = ct.c_int8 * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = int8_array_type(*values)
        return self.hdll.fmi3_setInt8(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)

    def fmi3_setUInt8(self, valueReferences, nValueReferences,values, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        uint8_array_type = ct.c_uint8 * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = uint8_array_type(*values)
        return self.hdll.fmi3_setUInt8(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)

    def fmi3_setInt16(self, valueReferences, nValueReferences,values, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        int16_array_type = ct.c_int16 * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = int16_array_type(*values)
        return self.hdll.fmi3_setInt16(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)

    def fmi3_setUInt16(self, valueReferences, nValueReferences,values, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        uint16_array_type = ct.c_uint16 * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = uint16_array_type(*values)
        return self.hdll.fmi3_setUInt16(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)
        
    def fmi3_setInt32(self, valueReferences, nValueReferences,values, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        int32_array_type = ct.c_int32 * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = int32_array_type(*values)
        return self.hdll.fmi3_setInt32(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)

    def fmi3_setUInt32(self, valueReferences, nValueReferences,values, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        uint32_array_type = ct.c_uint32 * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = uint32_array_type(*values)
        return self.hdll.fmi3_setUInt32(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)

    def fmi3_setInt64(self, valueReferences, nValueReferences,values, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        int64_array_type = ct.c_int64 * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = int64_array_type(*values)
        return self.hdll.fmi3_setInt64(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)

    def fmi3_setUInt64(self, valueReferences, nValueReferences,values, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        uint64_array_type = ct.c_uint64 * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = uint64_array_type(*values)
        return self.hdll.fmi3_setUInt64(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)

    def fmi3_setBoolean(self, valueReferences, nValueReferences,values, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        bool_array_type = ct.c_bool * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = bool_array_type(*values)
        return self.hdll.fmi3_setBoolean(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)
        
    def fmi3_setString(self, valueReferences, nValueReferences,values, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        string_array_type = ct.c_char_p * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = string_array_type(*values)
        return self.hdll.fmi3_setString(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)

    def fmi3_setBinary(self, valueReferences, nValueReferences, valueSizes, values, Values):
        uint_array_type = ct.c_uint * nValueReferences
        size_t_array_type = ct.c_size_t * nValueReferences
        binary_array_type = ct.POINTER(ct.c_uint8) * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = binary_array_type(*values)
        valueSizesArray = size_t_array_type(*valueSizes)
        return self.hdll.fmi3_setBinary(self.fmu, valueReferencesArray, nValueReferences, valueSizesArray, valuesArray, nValues)

    def fmi3_setClock(self, valueReferences, nValueReferences,values, nValues):
        uint_array_type = ct.c_uint * nValueReferences
        bool_array_type = ct.c_bool * nValueReferences
        valueReferencesArray = uint_array_type(*valueReferences)
        valuesArray = boolarray_type(*values)
        return self.hdll.fmi3_setClock(self.fmu, valueReferencesArray, nValueReferences, valuesArray, nValues)
        
        
    def fmi3_getNumberOfVariableDependencies(self, valueReference):
        nDependencies = ct.c_size_t()
        success = self.hdll.fmi3_getNumberOfVariableDependencies(self.fmu, valueReference, ct.byref(nDependencies))
        return (success, nDependencies.value)


    def fmi3_getVariableDependencies(self, dependent, nDependencies):
        uint_array_type = ct.c_uint * nDependencies
        size_t_array_type = ct.c_sizeZ_t * nDependencies
        int_array_type = ct.c_int * nDependencies
        elementIndicesOfDependent = size_t_array_type()
        independents = uint_array_type()
        elementIndicesOfIndependents = size_t_array_type()
        dependencyKinds = int_array_type()
        success = fmi3GetVariableDependencies(self.fmu, dependent, elementIndicesOfDependent, independents, elementIndicesOfIndependents, dependencyKinds, nDependencies)
        return (success, elementIndicesOfDependent, independents, elementIndicesOfIndependents, dependencyKinds)
        
        
    def fmi3_getFMUState(self):
        FMUState = ct.c_void_p()
        success = self.hdll.fmi3_getFMUState(self.fmu, ct.byref(FMUState))
        return (success, FMUState)

    def fmi3_setFMUState(self, FMUState):
        return self.hdll.fmi3_setFMUState(self.fmu, FMUState)

    def fmi3_freeFMUState(self, FMUState):
        return self.hdll.fmi3_freeFMUState(self.fmu, FMUState)

    def fmi3_serializedFMUStateSize(self, FMUState):
        size = ct.c_size_t()
        success = self.hdll.fmi3_serializedFMUStateSize(self.fmu, FMUState,ct.byref(size))
        return (success, size)

    def fmi3_serializeFMUState(self, FMUState, size):
        serializedState = ct.c_uint8()
        success = self.hdll.fmi3_serializeFMUState(self.fmu, FMUState, ct.byref(serializedState), size)
        return (success, serializedState)

    def fmi3_deserializeFMUState(self, serializedState, size):
        FMUState = ct.c_void_p()
        success = self.hdll.fmi3_deserializeFMUState(self.fmu, serializedState, size,ct.byref(FMUState))
        return (success, FMUState)

    def fmi3_getDirectionalDerivative(self, unknowns, nUnknowns, knowns, nKnowns, seed, nSeed, nSensitivity):
        unknowns_array_type = ct.c_uint * nUnknowns
        unknowns_array = unknowns_array_type(*unknowns)
        knowns_array_type = ct.c_uint * nKnowns
        knowns_array = knowns_array_type(*knowns)
        seed_array_type = ct.c_double * nSeed
        seed_array = seed_array_type(*seed)
        sensitivity_array_type = ct.c_double * nSensitivity
        sensitivity_array = sensitivity_array_type()
        success = self.hdll.fmi3_getDirectionalDerivative(self.fmu, unknowns_array, nUnknowns, knowns_array, nKnowns, seed_array, nSeed, sensitivity_array, nSensitivity)
        return (success, list(sensitivity_array))

    def fmi3_getAdjointDerivative(self, unknowns, nUnknowns, knowns, nKnowns, seed, nSeed, nSensitivity):
        unknowns_array_type = ct.c_uint * nUnknowns
        unknowns_array = unknowns_array_type(*unknowns)
        knowns_array_type = ct.c_uint * nKnowns
        knowns_array = knowns_array_type(*knowns)
        seed_array_type = ct.c_double * nSeed
        seed_array = seed_array_type(*seed)
        sensitivity_array_type = ct.c_double * nSensitivity
        sensitivity_array = sensitivity_array_type()
        success = self.hdll.fmi3_getAdjointDerivative(self.fmu, unknowns_array, nUnknowns, knowns_array, nKnowns, seed_array, nSeed, sensitivity_array, nSensitivity)
        return (success, list(sensitivity_array))

    def fmi3_enterConfigurationMode(self):
        return self.hdll.fmi3_enterConfigurationMode(self.fmu)

    def fmi3_exitConfigurationMode(self):
        return self.hdll.fmi3_exitConfigurationMode(self.fmu)

    def fmi3_getIntervalDecimal(self, valueReferences, nValueReferences):
        vr_array_type = ct.c_uint * nValueReferences
        vr_array = vr_array_type(*valueReferences)
        intervals_array_type = ct.c_double * nValueReferences
        intervals_array = intervals_array_type()
        qualifier_array_type = ct.c_int * nValueReferences
        qualifier_array = qualifier_array_type()
        success = self.hdll.fmi3_getIntervalDecimal(self.fmu, vr_array, nValueReferences,intervals_array,qualifier_array)
        return (success, list(intervals_array), list(qualifier_array))

    def fmi3_getIntervalFraction(self, valueReferences, nValueReferences):
        vr_array_type = ct.c_uint * nValueReferences
        vr_array = vr_array_type(*valueReferences)
        intervalCounters_array_type = ct.c_uint64 * nValueReferences
        intervalCounters_array = intervalCounters_array_type()
        resolutions_array_type = ct.c_uint64 * nValueReferences
        resolutions_array = resolutions_array_type()
        qualifier_array_type = ct.c_int * nValueReferences
        qualifier_array = qualifier_array_type()
        success = self.hdll.fmi3_getIntervalFraction(self.fmu, vr_array, nValueReferences, intervalCounters_array, resolutions_array, qualifier_array)
        return (success, list(intervalCounters_array), list(resolutions_array), list(qualifier_array))

    def fmi3_getShiftDecimal(self, valueReferences, nValueReferences):
        vr_array_type = ct.c_uint * nValueReferences
        vr_array = vr_array_type(*valueReferences)
        shifts_array_type = ct.c_double * nValueReferences
        shifts_array = shits_array_type()
        success = self.hdll.fmi3_getShiftDecimal(self.fmu, vr_array, nValueReferences, shifts_array)
        return (success, list(shifts_array))

    def fmi3_getShiftFraction(self, valueReferences, nValueReferences):
        uint64_array_type = ct.c_uint64 * nValueReferences
        shiftCounters_array = uint64_array_type()
        resolutions_array = uint64_array_type()
        vr_array_type = ct.c_uint * nValueReferences
        vr_array = vr_array_type(*valueReferences)
        success = self.hdll.fmi3_getShiftFraction(self.fmu, vr_array, nValueReferences, shiftCounters_array, resolutions_array)
        return (success, list(shiftCounters_array), list(resolutions_array))

    def fmi3_setIntervalDecimal(self, valueReferences, nValueReferences, intervals):
        float64_array_type = ct.c_double * nValueReferences
        intervals_array = float64_array_type(*intervals)
        vr_array_type = ct.c_uint * nValueReferences
        vr_array = vr_array_type(*valueReferences)
        return self.hdll.fmi3_setIntervalDecimal(self.fmu, vr_array, nValueReferences, intervals_array)

    def fmi3_setIntervalFraction(self, valueReferences, nValueReferences, intervalCounters, resolutions):
        vr_array_type = ct.c_uint * nValueReferences
        vr_array = vr_array_type(*valueReferences)
        uint64_array_type = ct.c_uint64 * nValueReferences
        intervalCounters_array = uint64_array_type(*intervalCounters)
        resolutions_array = uint64_array_type(*resolutions)
        return self.hdll.fmi3_setIntervalFraction(self.fmu, vr_array, nValueReferences, intervalCounters_array, resolutions_array)

    def fmi3_evaluateDiscreteStates(self):
        return self.hdll.fmi3_evaluateDiscreteStates(self.fmu)

    def fmi3_updateDiscreteStates(self):
        discreteStatesNeedUpdate = ct.c_bool()
        terminateSimulation = ct.c_bool()
        nominalsOfContinuousStatesChanged = ct.c_bool()
        valuesOfContinuousStatesChanged = ct.c_bool()
        nextEventTimeDefined = ct.c_bool()
        nextEventTime = ct.c_double()
        success = self.hdll.fmi3_updateDiscreteStates(self.fmu, ct.byref(discreteStatesNeedUpdate), ct.byref(terminateSimulation), ct.byref(nominalsOfContinuousStatesChanged), ct.byref(valuesOfContinuousStatesChanged), ct.byref(nextEventTimeDefined), ct.byref(nextEventTime))
        return (success, discreteStatesNeedUpdate.value, terminateSimulation.value, nominalsOfContinuousStatesChanged.value, valuesOfContinuousStatesChanged.value, nextEventTimeDefined.value, nextEventTime.value)

    def fmi3_enterContinuousTimeMode(self):
        return self.hdll.fmi3_enterContinuousTimeMode(self.fmu)

    def fmi3_completedIntegratorStep(self, noSetFMUStatePriorToCurrentPoint):
        enterEventMode = ct.c_bool()
        terminateSimulation = ct.c_bool()
        success = self.hdll.fmi3_completedIntegratorStep(self.fmu, noSetFMUStatePriorToCurrentPoint, ct.byref(enterEventMode), ct.byref(terminateSimulation))
        return (success, enterEventMode.value, terminateSimulation.value)

    def fmi3_setTime(self, time):
        return self.hdll.fmi3_setTime(self.fmu, time)

    def fmi3_setContinuousStates(self, continuousStates, nContinuousStates):
        float64_array_type = ct.c_double * nContinuousStates
        continuousStates_array = float64_array_type(*continuousStates)
        return self.hdll.fmi3_setContinuousStates(self.fmu, continuousStates_array, nContinuousStates)

    def fmi3_getContinuousStateDerivatives(self, nContinuousStates):
        float64_array_type = ct.c_double * nContinuousStates
        derivatives_array = float64_array_type()
        success = self.hdll.fmi3_getContinuousStateDerivatives(self.fmu, derivatives_array, nContinuousStates)
        return (success, list(derivatives_array))

    def fmi3_getEventIndicators(self, nEventIndicators):
        float64_array_type = ct.c_double * nEventIndicators
        eventIndicators_array = float64_array_type()
        success = self.hdll.fmi3_getEventIndicators(self.fmu, eventIndicators_array, nEventIndicators)
        return (success, list(eventIndicators_array))

    def fmi3_getContinuousStates(self, nContinuousStates):
        float64_array_type = ct.c_double * nContinuousStates
        continuousStates_array = float64_array_type()
        success = self.hdll.fmi3_getContinuousStates(self.fmu, continuousStates_array, nContinuousStates)
        return (success, list(continuousStates_array))

    def fmi3_getNominalsOfContinuousStates(self, nContinuousStates):
        float64_array_type = ct.c_double * nContinuousStates
        nominals_array = float64_array_type()
        success = self.hdll.fmi3_getNominalsOfContinuousStates(self.fmu, nominals_array, nContinuousStates)
        return (success, list(nominals_array))

    def fmi3_getNumberOfEventIndicators(self):
        nEventIndicators = ct.c_size_t()
        success = self.hdll.fmi3_getNumberOfEventIndicators(self.fmu, ct.byref(nEventIndicators))
        return (success, nEventIndicators.value)

    def fmi3_getNumberOfContinuousStates(self):
        nContinuousStates = ct.c_size_t()
        success = self.hdll.fmi3_getNumberOfContinuousStates(self.fmu, ct.byref(nContinuousStates))
        return (success, nContinuousStates.value)

    def fmi3_enterStepMode(self):
        return self.hdll.fmi3_enterStepMode(self.fmu)

    def fmi3_getOutputDerivatives(self, valueReferences, nValueReferences, orders, nValues):
        vr_array_type = ct.c_uint * nValueReferences
        vr_array = vr_array_type(*valueReferences)
        int32_array_type = ct.c_int32 * nValueReferences
        orders_array = int32_array_type(*orders)
        float64_array_type = ct.c_double * nValues
        values_array = float64_array_type()
        success = self.hdll.fmi3_getOutputDerivatives(self.fmu, vr_array, nValueReferences, orders_array, values_array, nValues)
        return (success, list(values_array))

    def fmi3_activateModelPartition(self, clockReference, activationTime):
        return self.hdll.fmi3_activateModelPartition(self.fmu, clockReference, activationTime)
        