#ifndef _HOTLOAD_H_
#define _HOTLOAD_H_

/*****************************************************************
 * 
 * Make sure to export the desired functions in the linker options
 * -EXPORT:<function name>
 * 
 ****************************************************************/

#define HOTLOAD_FUNCTION(name) void name()
typedef HOTLOAD_FUNCTION(hotload_function);
HOTLOAD_FUNCTION(hotloadFunctionStub){}

#endif