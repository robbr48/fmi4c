#ifndef fmiModelFunctions_h
#define fmiModelFunctions_h

/* This header file must be utilized when compiling a model.
   It defines all functions of the Model Execution Interface.
   In order to have unique function names even if several models
   are compiled together (e.g. for embedded systems), every "real" function name
   is constructed by prepending the function name by
   "MODEL_IDENTIFIER" + "_" where "MODEL_IDENTIFIER" is the short name
   of the model used as the name of the zip-file where the model is stored.
   Therefore, the typical usage is:

      #define MODEL_IDENTIFIER MyModel
      #include "fmiModelFunctions.h"

   As a result, a function that is defined as "fmiGetDerivatives" in this header file,
   is actually getting the name "MyModel_fmiGetDerivatives".

   Revisions:
   - Jan. 20, 2010: stateValueReferencesChanged added to struct fmiEventInfo (ticket #27)
                    (by M. Otter, DLR)
                    Added WIN32 pragma to define the struct layout (ticket #34)
                    (by J. Mauss, QTronic)
   - Jan.  4, 2010: Removed argument intermediateResults from fmiInitialize
                    Renamed macro fmiGetModelFunctionsVersion to fmiGetVersion
                    Renamed macro fmiModelFunctionsVersion to fmiVersion
                    Replaced fmiModel by fmi1Component in decl of fmiInstantiateModel
                    (by J. Mauss, QTronic)
   - Dec. 17, 2009: Changed extension "me" to "fmi" (by Martin Otter, DLR).
   - Dez. 14, 2009: Added eventInfo to meInitialize and added
                    meGetNominalContinuousStates (by Martin Otter, DLR)
   - Sept. 9, 2009: Added DllExport (according to Peter Nilsson's suggestion)
                    (by A. Junghanns, QTronic)
   - Sept. 9, 2009: Changes according to FMI-meeting on July 21:
                    meInquireModelTypesVersion     -> meGetModelTypesPlatform
                    meInquireModelFunctionsVersion -> meGetModelFunctionsVersion
                    meSetStates                    -> meSetContinuousStates
                    meGetStates                    -> meGetContinuousStates
                    removal of meInitializeModelClass
                    removal of meGetTime
                    change of arguments of meInstantiateModel
                    change of arguments of meCompletedIntegratorStep
                    (by Martin Otter, DLR):
   - July 19, 2009: Added "me" as prefix to file names (by Martin Otter, DLR).
   - March 2, 2009: Changed function definitions according to the last design
                    meeting with additional improvements (by Martin Otter, DLR).
   - Dec. 3 , 2008: First version by Martin Otter (DLR) and Hans Olsson (Dynasim).


   Copyright � 2008-2009, MODELISAR consortium. All rights reserved.
   This file is licensed by the copyright holders under the BSD License
   (http://www.opensource.org/licenses/bsd-license.html):

   ----------------------------------------------------------------------------
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   - Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
   - Neither the name of the copyright holders nor the names of its
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
   TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
   OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
   ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   ----------------------------------------------------------------------------

   with the extension:

   You may distribute or publicly perform any modification only under the
   terms of this license.
*/

#include "fmiModelTypes.h"
#include <stdlib.h>



/* Version number */
#define fmiVersion "1.0"


/* make sure all compiler use the same alignment policies for structures */
#ifdef WIN32
#pragma pack(push,8)
#endif

/* Type definitions */


   typedef void  (*fmi1CallbackLogger)        (fmi1Component c, fmi1String instanceName, fmi1Status status,
                                              fmi1String category, fmi1String message, ...);
   typedef void* (*fmi1CallbackAllocateMemory)(size_t nobj, size_t size);
   typedef void  (*fmi1CallbackFreeMemory)    (void* obj);

   typedef struct {
     fmi1CallbackLogger         logger;
     fmi1CallbackAllocateMemory allocateMemory;
     fmi1CallbackFreeMemory     freeMemory;
   } fmi1CallbackFunctions;

   typedef struct {
      fmi1Boolean iterationConverged;
      fmi1Boolean stateValueReferencesChanged;
      fmi1Boolean stateValuesChanged;
      fmi1Boolean terminateSimulation;
      fmi1Boolean upcomingTimeEvent;
      fmi1Real    nextEventTime;
   } fmi1EventInfo;

/* reset alignment policy to the one set before reading this file */
#ifdef WIN32
#pragma pack(pop)
#endif


#endif /* fmiModelFunctions_h */
