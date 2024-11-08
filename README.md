# fmi4c

fmi4c is a C library that enables import of [Functional Mockup Units (FMUs)]((https://fmi-standard.org/)) from FMI 1.0, FMI 2.0 and FMI 3.0. 

All kinds of FMUs (model exchange, co-simulation and scheduled execution) are supported.

The library is intended to be lightweight, easy to implement and to have an intuitive interface.

## Features

- Import FMUs for FMI 1.0 (co-simulation and model exchange)
- Import FMUs for FMI 2.0 (co-simulation and model exchange)
- Import FMUs for FMI 3.0 (co-simulation, model exchange and scheduled execution)
- Placeholder functions for all API functions, to prevent crash when calling functions not available in FMU

## Third Party Dependencies
Dependencies have been chosen to minimize implementation effort and to make the code easy to understand.
- [ezxml](https://github.com/lxfontes/ezxml)
- [zlib](https://github.com/madler/zlib) (optional)
- [minizip](http://www.winimage.com/zLibDll/minizip.html) (optional)

## API Example

Below is a simplified example on how to load and simulate an FMI 3.0 FMU for co-simulation. More detailed examples are available in the source code for the test executable.

```
#include <stdio.h>
#include "fmi4c.h"

int main(int argc, char *argv[])
{
    if(argc < 2) {
        printf("Too few arguments.\n");
        exit(1);
    }

    fmiHandle *fmu = loadFmu(fmuPath, "myfmu");

    if(!fmu) {
        printf("Failed to load FMU\n");
        exit(1);
    }

    if(getFmiVersion(fmu) == fmiVersion3 && fmi3SupportsCoSimulation(fmu)) {
        if(!fmi3InstantiateCoSimulation(fmu, ...)) {
            printf("Failed to instantiate FMU\n");
            exit(1);
        }
        fmi3Status status;
        status = fmi3EnterInitializationMode(fmu, ...);
        if(status != fmi3OK) {
            printf("Enter initialization mode failed\n");
            exit(1);
        }
        status = fmi3ExitInitializationMode(fmu);

        double tstart = 0;
        double tend = 1;
        double stepsize = 0.001;
        for(double time=tstart; time <= tend; time+=stepsize) {
            status = fmi3SetFloat64(fmu, ...);
            status = fmi3DoStep(fmu, time, stepsize, ...);
            status = fmi3GetFloat64(fmu, ...);
        }
      
        status = fmi3Terminate(fmu);
        fmi3FreeInstance(fmu);
    }
    
    return 0;
}
```

