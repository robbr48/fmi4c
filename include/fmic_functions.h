#ifndef FMIC_FUNCTIONS_H
#define FMIC_FUNCTIONS_H

typedef const char* (__stdcall *fmi3GetVersion_t)();

typedef fmi3Status (__stdcall *fmi3SetDebugLogging_t)(fmi3Instance instance,
                                                    fmi3Boolean loggingOn,
                                                    size_t nCategories,
                                                    const fmi3String categories[]);

typedef fmi3Instance (__stdcall *fmi3InstantiateModelExchange_t)(
        fmi3String                 instanceName,
        fmi3String                 instantiationToken,
        fmi3String                 resourcePath,
        fmi3Boolean                visible,
        fmi3Boolean                loggingOn,
        fmi3InstanceEnvironment    instanceEnvironment,
        fmi3CallbackLogMessage     logMessage);

typedef fmi3Instance (__stdcall *fmi3InstantiateCoSimulation_t)(
    fmi3String                     instanceName,
    fmi3String                     instantiationToken,
    fmi3String                     resourcePath,
    fmi3Boolean                    visible,
    fmi3Boolean                    loggingOn,
    fmi3Boolean                    eventModeUsed,
    fmi3Boolean                    earlyReturnAllowed,
    const fmi3ValueReference       requiredIntermediateVariables[],
    size_t                         nRequiredIntermediateVariables,
    fmi3InstanceEnvironment        instanceEnvironment,
    fmi3CallbackLogMessage         logMessage,
    fmi3CallbackIntermediateUpdate intermediateUpdate);

typedef fmi3Instance (__stdcall *fmi3InstantiateScheduledExecution_t)(
        fmi3String                     instanceName,
        fmi3String                     instantiationToken,
        fmi3String                     resourcePath,
        fmi3Boolean                    visible,
        fmi3Boolean                    loggingOn,
        const fmi3ValueReference       requiredIntermediateVariables[],
        size_t                         nRequiredIntermediateVariables,
        fmi3InstanceEnvironment        instanceEnvironment,
        fmi3CallbackLogMessage         logMessage,
        fmi3CallbackIntermediateUpdate intermediateUpdate,
        fmi3CallbackLockPreemption     lockPreemption,
        fmi3CallbackUnlockPreemption   unlockPreemption);

typedef void (__stdcall *fmi3FreeInstance_t)(fmi3Instance instance);

typedef fmi3Status (__stdcall *fmi3EnterInitializationMode_t)(fmi3Instance instance,
                                                            fmi3Boolean toleranceDefined,
                                                            fmi3Float64 tolerance,
                                                            fmi3Float64 startTime,
                                                            fmi3Boolean stopTimeDefined,
                                                            fmi3Float64 stopTime);

typedef fmi3Status (__stdcall *fmi3ExitInitializationMode_t)(fmi3Instance instance);
typedef fmi3Status (__stdcall *fmi3Terminate_t)(fmi3Instance instance);
typedef fmi3Status (__stdcall *fmi3GetFloat64_t)(fmi3Instance instance,
                                               const fmi3ValueReference valueReferences[],
                                               size_t nValueReferences,
                                               fmi3Float64 values[],
                                               size_t nValues);
typedef fmi3Status (__stdcall *fmi3SetFloat64_t)(fmi3Instance instance,
                                               const fmi3ValueReference valueReferences[],
                                               size_t nValueReferences,
                                               const fmi3Float64 values[],
                                               size_t nValues);
typedef fmi3Status (__stdcall *fmi3DoStep_t)(fmi3Instance instance,
                                           fmi3Float64 currentCommunicationPoint,
                                           fmi3Float64 communicationStepSize,
                                           fmi3Boolean noSetFMUStatePriorToCurrentPoint,
                                           fmi3Boolean* eventEncountered,
                                           fmi3Boolean* terminateSimulation,
                                           fmi3Boolean* earlyReturn,
                                           fmi3Float64* lastSuccessfulTime);

typedef fmi3Status (__stdcall *fmi3EnterEventMode_t)(fmi3Instance instance,
                                          fmi3Boolean stepEvent,
                                          fmi3Boolean stateEvent,
                                          const fmi3Int32 rootsFound[],
                                          size_t nEventIndicators,
                                          fmi3Boolean timeEvent);


typedef fmi3Status (__stdcall *fmi3Reset_t)(fmi3Instance instance);

typedef fmi3Status (__stdcall *fmi3GetFloat32_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3Float32 values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3GetInt8_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3Int8 values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3GetUInt8_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3UInt8 values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3GetInt16_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3Int16 values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3GetUInt16_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3UInt16 values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3GetInt32_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3Int32 values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3GetUInt32_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3UInt32 values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3GetInt64_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3Int64 values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3GetUInt64_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3UInt64 values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3GetBoolean_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3Boolean values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3GetString_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3String values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3GetBinary_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      size_t valueSizes[],
                                      fmi3Binary values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3GetClock_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3Clock values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3SetFloat32_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3Float32 values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3SetInt8_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3Int8 values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3SetUInt8_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3UInt8 values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3SetInt16_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3Int16 values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3SetUInt16_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3UInt16 values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3SetInt32_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3Int32 values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3SetUInt32_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3UInt32 values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3SetInt64_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3Int64 values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3SetUInt64_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3UInt64 values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3SetBoolean_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3Boolean values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3SetString_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3String values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3SetBinary_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const size_t valueSizes[],
                                      const fmi3Binary values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3SetClock_t)(fmi3Instance instance,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3Clock values[],
                                      size_t nValues);

typedef fmi3Status (__stdcall *fmi3GetNumberOfVariableDependencies_t)(fmi3Instance instance,
                                                           fmi3ValueReference valueReference,
                                                           size_t* nDependencies);

typedef fmi3Status (__stdcall *fmi3GetVariableDependencies_t)(fmi3Instance instance,
                                                   fmi3ValueReference dependent,
                                                   size_t elementIndicesOfDependent[],
                                                   fmi3ValueReference independents[],
                                                   size_t elementIndicesOfIndependents[],
                                                   fmi3DependencyKind dependencyKinds[],
                                                   size_t nDependencies);

typedef fmi3Status (__stdcall *fmi3GetFMUState_t)(fmi3Instance instance, fmi3FMUState* FMUState);

typedef fmi3Status (__stdcall *fmi3SetFMUState_t)(fmi3Instance instance, fmi3FMUState  FMUState);

typedef fmi3Status (__stdcall *fmi3FreeFMUState_t)(fmi3Instance instance, fmi3FMUState* FMUState);

typedef fmi3Status (__stdcall *fmi3SerializedFMUStateSize_t)(fmi3Instance instance,
                                                  fmi3FMUState  FMUState,
                                                  size_t* size);

typedef fmi3Status (__stdcall *fmi3SerializeFMUState_t)(fmi3Instance instance,
                                                  fmi3FMUState  FMUState,
                                                  fmi3Byte serializedState[],
                                                  size_t size);

typedef fmi3Status (__stdcall *fmi3DeSerializeFMUState_t)(fmi3Instance instance,
                                                  const fmi3Byte serializedState[],
                                                  size_t size,
                                                  fmi3FMUState* FMUState);

typedef fmi3Status (__stdcall *fmi3GetDirectionalDerivative_t)(fmi3Instance instance,
                                                    const fmi3ValueReference unknowns[],
                                                    size_t nUnknowns,
                                                    const fmi3ValueReference knowns[],
                                                    size_t nKnowns,
                                                    const fmi3Float64 seed[],
                                                    size_t nSeed,
                                                    fmi3Float64 sensitivity[],
                                                    size_t nSensitivity);

typedef fmi3Status (__stdcall *fmi3GetAdjointDerivative_t)(fmi3Instance instance,
                                                const fmi3ValueReference unknowns[],
                                                size_t nUnknowns,
                                                const fmi3ValueReference knowns[],
                                                size_t nKnowns,
                                                const fmi3Float64 seed[],
                                                size_t nSeed,
                                                fmi3Float64 sensitivity[],
                                                size_t nSensitivity);

typedef fmi3Status (__stdcall *fmi3EnterConfigurationMode_t)(fmi3Instance instance);

typedef fmi3Status (__stdcall *fmi3ExitConfigurationMode_t)(fmi3Instance instance);

typedef fmi3Status (__stdcall *fmi3GetIntervalDecimal_t)(fmi3Instance instance,
                                              const fmi3ValueReference valueReferences[],
                                              size_t nValueReferences,
                                              fmi3Float64 intervals[],
                                              fmi3IntervalQualifier qualifiers[],
                                              size_t nIntervals);

typedef fmi3Status (__stdcall *fmi3GetIntervalFraction_t)(fmi3Instance instance,
                                               const fmi3ValueReference valueReferences[],
                                               size_t nValueReferences,
                                               fmi3UInt64 intervalCounters[],
                                               fmi3UInt64 resolutions[],
                                               fmi3IntervalQualifier qualifiers[],
                                               size_t nIntervals);

typedef fmi3Status (__stdcall *fmi3GetShiftDecimal_t)(fmi3Instance instance,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           fmi3Float64 shifts[],
                                           size_t nShifts);

typedef fmi3Status (__stdcall *fmi3GetShiftFraction_t)(fmi3Instance instance,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            fmi3UInt64 shiftCounters[],
                                            fmi3UInt64 resolutions[],
                                            size_t nShifts);

typedef fmi3Status (__stdcall *fmi3SetIntervalDecimal_t)(fmi3Instance instance,
                                              const fmi3ValueReference valueReferences[],
                                              size_t nValueReferences,
                                              const fmi3Float64 intervals[],
                                              size_t nIntervals);

typedef fmi3Status (__stdcall *fmi3SetIntervalFraction_t)(fmi3Instance instance,
                                               const fmi3ValueReference valueReferences[],
                                               size_t nValueReferences,
                                               const fmi3UInt64 intervalCounters[],
                                               const fmi3UInt64 resolutions[],
                                               size_t nIntervals);

typedef fmi3Status (__stdcall *fmi3EvaluateDiscreteStates_t)(fmi3Instance instance);

typedef fmi3Status (__stdcall *fmi3UpdateDiscreteStates_t)(fmi3Instance instance,
                                                fmi3Boolean* discreteStatesNeedUpdate,
                                                fmi3Boolean* terminateSimulation,
                                                fmi3Boolean* nominalsOfContinuousStatesChanged,
                                                fmi3Boolean* valuesOfContinuousStatesChanged,
                                                fmi3Boolean* nextEventTimeDefined,
                                                fmi3Float64* nextEventTime);

typedef fmi3Status (__stdcall *fmi3EnterContinuousTimeMode_t)(fmi3Instance instance);

typedef fmi3Status (__stdcall *fmi3CompletedIntegratorStep_t)(fmi3Instance instance,
                                                   fmi3Boolean  noSetFMUStatePriorToCurrentPoint,
                                                   fmi3Boolean* enterEventMode,
                                                   fmi3Boolean* terminateSimulation);

typedef fmi3Status (__stdcall *fmi3SetTime_t)(fmi3Instance instance, fmi3Float64 time);

typedef fmi3Status (__stdcall *fmi3SetContinuousStates_t)(fmi3Instance instance,
                                               const fmi3Float64 continuousStates[],
                                               size_t nContinuousStates);

typedef fmi3Status (__stdcall *fmi3GetContinuousStateDerivatives_t)(fmi3Instance instance,
                                                         fmi3Float64 derivatives[],
                                                         size_t nContinuousStates);

typedef fmi3Status (__stdcall *fmi3GetEventIndicators_t)(fmi3Instance instance,
                                              fmi3Float64 eventIndicators[],
                                              size_t nEventIndicators);

typedef fmi3Status (__stdcall *fmi3GetContinuousStates_t)(fmi3Instance instance,
                                               fmi3Float64 continuousStates[],
                                               size_t nContinuousStates);

typedef fmi3Status (__stdcall *fmi3GetNominalsOfContinuousStates_t)(fmi3Instance instance,
                                                         fmi3Float64 nominals[],
                                                         size_t nContinuousStates);

typedef fmi3Status (__stdcall *fmi3GetNumberOfEventIndicators_t)(fmi3Instance instance,
                                                      size_t* nEventIndicators);

typedef fmi3Status (__stdcall *fmi3GetNumberOfContinuousStates_t)(fmi3Instance instance,
                                                       size_t* nContinuousStates);

typedef fmi3Status (__stdcall *fmi3EnterStepMode_t)(fmi3Instance instance);

typedef fmi3Status (__stdcall *fmi3GetOutputDerivatives_t)(fmi3Instance instance,
                                                const fmi3ValueReference valueReferences[],
                                                size_t nValueReferences,
                                                const fmi3Int32 orders[],
                                                fmi3Float64 values[],
                                                size_t nValues);

typedef fmi3Status (__stdcall *fmi3ActivateModelPartition_t)(fmi3Instance instance,
                                                  fmi3ValueReference clockReference,
                                                  size_t clockElementIndex,
                                                  fmi3Float64 activationTime);

typedef const char* (__stdcall *fmi2GetTypesPlatform_t)(void);
typedef const char* (__stdcall *fmi2GetVersion_t)(void);
typedef fmi2Status (__stdcall *fmi2SetDebugLogging_t)(fmi2Component, fmi2Boolean, size_t, const fmi2String[]);

/* Creation and destruction of FMU instances and setting debug status */
typedef fmi2Component (__stdcall *fmi2Instantiate_t)(fmi2String, fmi2Type, fmi2String, fmi2String, const fmi2CallbackFunctions*, fmi2Boolean, fmi2Boolean);

typedef void (__stdcall *fmi2FreeInstance_t)(fmi2Component);

/* Enter and exit initialization mode, terminate and reset */
typedef fmi2Status (__stdcall *fmi2SetupExperiment_t)(fmi2Component, fmi2Boolean, fmi2Real, fmi2Real, fmi2Boolean, fmi2Real);
typedef fmi2Status (__stdcall *fmi2EnterInitializationMode_t)(fmi2Component);
typedef fmi2Status (__stdcall *fmi2ExitInitializationMode_t)(fmi2Component);
typedef fmi2Status (__stdcall *fmi2Terminate_t)(fmi2Component);
typedef fmi2Status (__stdcall *fmi2Reset_t)(fmi2Component);

/* Getting and setting variable values */
typedef fmi2Status (__stdcall *fmi2GetReal_t)(fmi2Component, const fmi2ValueReference[], size_t, fmi2Real   []);
typedef fmi2Status (__stdcall *fmi2GetInteger_t)(fmi2Component, const fmi2ValueReference[], size_t, fmi2Integer[]);
typedef fmi2Status (__stdcall *fmi2GetBoolean_t)(fmi2Component, const fmi2ValueReference[], size_t, fmi2Boolean[]);
typedef fmi2Status (__stdcall *fmi2GetString_t)(fmi2Component, const fmi2ValueReference[], size_t, fmi2String []);

typedef fmi2Status (__stdcall *fmi2SetReal_t)(fmi2Component, const fmi2ValueReference[], size_t, const fmi2Real   []);
typedef fmi2Status (__stdcall *fmi2SetInteger_t)(fmi2Component, const fmi2ValueReference[], size_t, const fmi2Integer[]);
typedef fmi2Status (__stdcall *fmi2SetBoolean_t)(fmi2Component, const fmi2ValueReference[], size_t, const fmi2Boolean[]);
typedef fmi2Status (__stdcall *fmi2SetString_t)(fmi2Component, const fmi2ValueReference[], size_t, const fmi2String []);

/* Getting and setting the internal FMU state */
typedef fmi2Status (__stdcall *fmi2GetFMUstate_t)(fmi2Component, fmi2FMUstate*);
typedef fmi2Status (__stdcall *fmi2SetFMUstate_t)(fmi2Component, fmi2FMUstate);
typedef fmi2Status (__stdcall *fmi2FreeFMUstate_t)(fmi2Component, fmi2FMUstate*);
typedef fmi2Status (__stdcall *fmi2SerializedFMUstateSize_t)(fmi2Component, fmi2FMUstate, size_t*);
typedef fmi2Status (__stdcall *fmi2SerializeFMUstate_t)(fmi2Component, fmi2FMUstate, fmi2Byte[], size_t);
typedef fmi2Status (__stdcall *fmi2DeSerializeFMUstate_t)(fmi2Component, const fmi2Byte[], size_t, fmi2FMUstate*);

/* Getting partial derivatives */
typedef fmi2Status (__stdcall *fmi2GetDirectionalDerivative_t)(fmi2Component, const fmi2ValueReference[], size_t,
                                                                const fmi2ValueReference[], size_t,
                                                                const fmi2Real[], fmi2Real[]);

/***************************************************
Types for Functions for FMI2 for Model Exchange
****************************************************/

/* Enter and exit the different modes */
typedef fmi2Status (__stdcall *fmi2EnterEventMode_t)(fmi2Component);
typedef fmi2Status (__stdcall *fmi2NewDiscreteStates_t)(fmi2Component, fmi2EventInfo*);
typedef fmi2Status (__stdcall *fmi2EnterContinuousTimeMode_t)(fmi2Component);
typedef fmi2Status (__stdcall *fmi2CompletedIntegratorStep_t)(fmi2Component, fmi2Boolean, fmi2Boolean*, fmi2Boolean*);

/* Providing independent variables and re-initialization of caching */
typedef fmi2Status (__stdcall *fmi2SetTime_t)(fmi2Component, fmi2Real);
typedef fmi2Status (__stdcall *fmi2SetContinuousStates_t)(fmi2Component, const fmi2Real[], size_t);

/* Evaluation of the model equations */
typedef fmi2Status (__stdcall *fmi2GetDerivatives_t)(fmi2Component, fmi2Real[], size_t);
typedef fmi2Status (__stdcall *fmi2GetEventIndicators_t)(fmi2Component, fmi2Real[], size_t);
typedef fmi2Status (__stdcall *fmi2GetContinuousStates_t)(fmi2Component, fmi2Real[], size_t);
typedef fmi2Status (__stdcall *fmi2GetNominalsOfContinuousStates_t)(fmi2Component, fmi2Real[], size_t);


/***************************************************
Types for Functions for FMI2 for Co-Simulation
****************************************************/

/* Simulating the slave */
typedef fmi2Status (__stdcall *fmi2SetRealInputDerivatives_t)(fmi2Component, const fmi2ValueReference [], size_t, const fmi2Integer [], const fmi2Real []);
typedef fmi2Status (__stdcall *fmi2GetRealOutputDerivatives_t)(fmi2Component, const fmi2ValueReference [], size_t, const fmi2Integer [], fmi2Real []);

typedef fmi2Status (__stdcall *fmi2DoStep_t)(fmi2Component, fmi2Real, fmi2Real, fmi2Boolean);
typedef fmi2Status (__stdcall *fmi2CancelStep_t)(fmi2Component);

/* Inquire slave status */
typedef fmi2Status (__stdcall *fmi2GetStatus_t)(fmi2Component, const fmi2StatusKind, fmi2Status* );
typedef fmi2Status (__stdcall *fmi2GetRealStatus_t)(fmi2Component, const fmi2StatusKind, fmi2Real*   );
typedef fmi2Status (__stdcall *fmi2GetIntegerStatus_t)(fmi2Component, const fmi2StatusKind, fmi2Integer*);
typedef fmi2Status (__stdcall *fmi2GetBooleanStatus_t)(fmi2Component, const fmi2StatusKind, fmi2Boolean*);
typedef fmi2Status (__stdcall *fmi2GetStringStatus_t)(fmi2Component, const fmi2StatusKind, fmi2String* );

#endif // FMIC_FUNCTIONS_H
