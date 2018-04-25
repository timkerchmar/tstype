#include "TSType.h"

TSArray<TSType *>* TSType::types = NULL; 

TSType TSType::typeInstance("Type", "type");
TSEmptyTypeClass* TSEmptyType = &TSType::typeInstance;

TSImplementType(TSObject, "typed object");
TSField(TSTypePtr, TSObject, type, NULL);

TSImplementAbstractType(TSType, "type instance");
TSField(TSTypePtrArray, TSType, fields, TSTypePtrArray());
TSField(TSString, TSType, name, "unnamed type");
TSField(TSString, TSType, description, "unnnamed type");

TSImplementTypeClass(Array, "abstract array");
TSImplementTypeClass(Pointer, "abstract pointer");

TSImplementType(ContainerBase, "container base");

TSImplementType(GenericContainer, "untyped container");

TSImplementType(GenericReference, "untyped reference");
TSField(TSString, GenericReference, id, "unnamed");

TSImplementType(TSString, "text");

// A SEGFAULT HERE MEANS THAT YOU ARE
// PROBABLY MISSING THE MATCHING CPP
// MACRO TO GO WITH THE HEADER VERSION
bool TSObject::is(TSType* otherType)
{
    assert(type && otherType && "An object's cpp file is missing TSImplementType directive\n");
    return type->is(otherType);
}

void TSObject::destroy()
{
    type->destroy(this);
}

TSType* TSType::findByDescription(TSString& description)
{
    for(TSArray<TSType *>::iterator it = types->begin(); it != types->end(); it++)
    {
        TSType* type = *it;
        
        if(type->description == description)
        {
            return type;
        }
    }
    
    return NULL;
}

TSType* TSType::findByName(const TSString& name)
{
    for(TSArray<TSType *>::iterator it = types->begin(); it != types->end(); it++)
    {
        TSType* type = *it;
        
        if(type->name == name)
        {
            return type;
        }
    }
    
    return NULL;
}

void TSType::print(TSType* baseType, int indent)
{
    if(baseType) 
    {
        printf("%s ", baseType->name.c_str());
        
        if(!baseType->fields.empty())
        {
            printf("( ");
			size_t size = baseType->fields.size();
            for(size_t i = 0; i < size; i++)
            {
                printf("%s ", baseType->fields[i]->description.c_str());
            }
            putchar(')');
        }
        
        putchar('\n');
    }
    
    for(TSArray<TSType *>::iterator it = types->begin(); it != types->end(); it++)
    {
        TSType* type = *it;
        
        if(type->base() == baseType)
        {
            for(int j = indent; j > 0; --j) putchar(' ');
            
            print(type, indent + 4);
        }
    }
}

TSType* TSType::getFieldByName(const TSString& name)
{
    for(TSType* type = (TSType *)this; type != NULL; type = type->base())
    {
        const TSArray<TSType *>& fields = type->fields;
		size_t size = fields.size();
        for(size_t i = 0; i < size; i++)
        {
            TSType* field = fields[i];
            
            if(field->name == name)
            {
                return field;
            }
        }
    }
    
    return NULL;
}

void PrintObjectHierarchy(TSType* type, void* value, int maxLevelsDeep, int levelsDeep)
{
    if(levelsDeep == maxLevelsDeep) return;
    
    if(!type)
    {
        printf("Error: type = (null). use Class::create() instead of new operator\n");
        return;
    }
    
    for(int j = levelsDeep * 4; j > 0; --j) putchar(' ');
    printf("%s", type->description.c_str());
    
    if(type->is(PointerType))
    {
        type = PointerType->cast(type)->dereferenced();
        value = *(void **)value;
    }
    
    if(!value)
    {
        printf(" = (null)\n");
        return;
    }
    
    if(type->is(TSObjectType))
    {
        TSObject* object = TSObjectType->cast(value);
        type = object->type;
    }
    
    for(TSType* currentType = type; currentType; currentType = currentType->base())
    {
        if (currentType == TSStringType)
        {
            printf(" = \"%s\"\n", ((TSString *)value)->c_str());
            return;
        }
		// else if (currentType == TSUInt32Type... or whatever types you want to treat like primitives.
		// This is even a decent spot to do neat things like print vec2 in the "(<x> <y>)" form instead
		// of two numbers on their own log lines
    }
    
    putchar('\n');
    
    ArrayTypeClass* arrayType = ArrayType->cast(type);
    if(arrayType)
    {
        int count = arrayType->count(value);
        for(int childIndex = 0; childIndex < count; childIndex++)
        {
            void* childValue = arrayType->childAtIndex(value, childIndex);
            
            TSType* childType = arrayType->memberType();
            if(childType->is(PointerType))
            {
                childType = PointerType->cast(childType)->dereferenced();
                childValue = *(void **)childValue;
            }
            
            if(childValue && childType->is(TSObjectType))
            {
                TSObject* object = TSObjectType->cast(childValue);
                childType = object->type;
            }
            
            PrintObjectHierarchy(childType, childValue, maxLevelsDeep, levelsDeep + 1);
        }
        return;
    }
    
    do
    {
        size_t size = type->fields.size();
        for(size_t i = 0; i < size; i++)
        {
            TSType* field = type->fields[i];
            PrintObjectHierarchy(field, field->get(value), maxLevelsDeep, levelsDeep + 1);
        }
        
        type = type->base();
    } while (type);
}
