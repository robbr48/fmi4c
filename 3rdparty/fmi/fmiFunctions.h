#ifndef fmiFunctions_h
#define fmiFunctions_h





     typedef enum {fmiOK,
                   fmiWarning,
                   fmiDiscard,
                   fmiError,
                   fmiFatal,
                   fmiPending} fmi1Status;





   typedef enum {fmiDoStepStatus,
                 fmiPendingStatus,
                 fmiLastSuccessfulTime} fmi1StatusKind;


#endif /* fmiFunctions_h */
