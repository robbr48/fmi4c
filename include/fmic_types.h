#ifndef FMIC_TYPES_H
#define FMIC_TYPES_H

typedef enum { fmi2CausalityInput, fmi2CausalityOutput, fmi2CausalityParameter, fmi2CausalityCalculatedParameter, fmi2CausalityLocal, fmi2CausalityIndependent } fmi2Causality;
typedef enum { fmi2VariabilityFixed, fmi2VariabilityTunable, fmi2VariabilityConstant, fmi2VariabilityDiscrete, fmi2VariabilityContinuous } fmi2Variability;
typedef enum { fmi2InitialExact, fmi2InitialApprox, fmi2InitialCalculated } fmi2Initial;
typedef enum { fmi2DataTypeReal, fmi2DataTypeInteger, fmi2DataTypeBoolean, fmi2DataTypeString, fmi2DataTypeEnumeration } fmi2DataType;

typedef enum { fmi3CausalityInput, fmi3CausalityOutput, fmi3CausalityParameter, fmi3CausalityCalculatedParameter, fmi3CausalityLocal, fmi3CausalityIndependent, fmi3CausalityStructuralParameter } fmi3Causality;
typedef enum { fmi3VariabilityFixed, fmi3VariabilityTunable, fmi3VariabilityConstant, fmi3VariabilityDiscrete, fmi3VariabilityContinuous } fmi3Variability;
typedef enum { fmi3InitialExact, fmi3InitialApprox } fmi3Initial;
typedef enum { fmi3DataTypeFloat64, fmi3DataTypeFloat32, fmi3DataTypeInt64, fmi3DataTypeInt32, fmi3DataTypeInt16, fmi3DataTypeInt8, fmi3DataTypeUint64, fmi3DataTypeBoolean, fmi3DataTypeString, fmi3DataTypeBinary, fmi3DataTypeClock } fmi3DataType;

typedef enum { fmiVersionUnknown, fmiVersion1, fmiVersion2, fmiVersion3 } fmiVersion_t;

#endif // FMIC_TYPES_H
