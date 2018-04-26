# TSType

## What is it?

A lightweight RTTI lib for C++ that doesn't require C++'s RTTI or boost. It enables real reflection.

## How do I use it?

Add the cpp and h source files to your project.

### Header Changes

Add TSType declarations after the classes declarations you want to inspect:
```cpp
TSDeclareType(SomeKindOfClass, SomeBaseClassType);
```

If there is no base class:
```cpp
TSDeclareType(SomeKindOfClass, TSEmpty);
```

If the class is abstract:
```cpp
TSDeclareAbstractType(SomeKindOfClass, SomeBaseClassType);
```

### Soure Changes

Implement the class in a c++ file:
```cpp
TSImplementType(SomeKindOfClass, "complexity manager");
```

Describe each public field in the C++ source and add a default value:
```cpp
TSField(TSString, SomeKindOfClass, fileName, "user.prefs");
```

## Sample

```cpp
#include "TSType.h"

class MostWanted 
{
public:
	TSStringArray names;
};
TSDeclareType(MostWanted, TSEmpty); 
```

```cpp
#include "MostWanted.h"

TSImplementType(MostWanted, "persons of interest");
TSField(TSStringArray, MostWanted, names, TSStringArray());

void testTSType
{
    printf("All known types:\n");
    TSType::print();
    
    printf("\nPrinting a description of the TSStringType object:\n");
    PrintObjectHierarchy(TSTypeType, TSStringType);
	
    MostWanted foo;
    foo.names.push_back("Bob");
    
    printf("\nPrinting a description of the MostWanted object:\n");
    PrintObjectHierarchy(MostWantedType, &foo);
	
    return 0;
}
```

Output:

```
All known types:
    Type 
        TSObject ( type )
            ContainerBase 
                GenericContainer 
            GenericReference ( id )
        TSType ( fields name description )
        Array 
            TSObjectArray 
            TSObjectPtrArray 
            TSTypePtrArray 
                TSTypefields 
            ContainerBaseArray 
            ContainerBasePtrArray 
            GenericContainerArray 
            GenericContainerPtrArray 
            GenericReferenceArray 
            GenericReferencePtrArray 
            TSStringArray 
            TSStringPtrArray 
        Pointer 
            TSObjectPtr 
            TSObjectArrayPtr 
            TSObjectPtrArrayPtr 
            TSTypePtr 
                TSObjecttype 
            TSTypePtrArrayPtr 
            ContainerBasePtr 
            ContainerBaseArrayPtr 
            ContainerBasePtrArrayPtr 
            GenericContainerPtr 
            GenericContainerArrayPtr 
            GenericContainerPtrArrayPtr 
            GenericReferencePtr 
            GenericReferenceArrayPtr 
            GenericReferencePtrArrayPtr 
            TSStringPtr 
            TSStringArrayPtr 
            TSStringPtrArrayPtr 
        TSString 
            TSTypename 
            TSTypedescription 
            GenericReferenceid 
Printing a description of the TSStringType object:
type instance
    fields
    name = "TSString"
    description = "text"

Printing a description of the MostWanted object:
persons of interest
    names
        text = "Bob"
```

## TODO

API for enumerating the methods of an object. Default values are not evaluated until they are requested by calling setDefaultValue from the object's type instance.