/////////////////////////////////////////////////////////////////////////
// TSType
/////////////////////////////////////////////////////////////////////////

#ifndef TSType_h
#define TSType_h

#include <vector>
#include <deque>
#include <unordered_map>
#include <string>

#define TSArray std::vector
#define TSDeque std::deque
#define TSDictionary std::unordered_map
typedef std::string TSString;

// Base class of all classes
class TSEmpty {};

// Base type class for all classes
class TSType
{
public:
    // Global to all types
    static TSArray<TSType *>* types; 
    static void print(TSType* baseType = NULL, int indent = 4);
    static TSType* findByName(const TSString& name);
    static TSType* findByDescription(TSString& description);
    
    // Establishes type singleton and heirarchy
    // make protected, forces 
    static TSType typeInstance;
    
    virtual TSType* base() { return NULL; }
    
    // Type checking and casting
    virtual bool is(TSType* otherType) { return this == otherType; }
    
    // Object instantiation and field subtypes
    virtual void* createInstance() 
    {
        return NULL;
    }
    
    virtual int sizeOf()
    {
        return sizeof(TSEmpty);
    }
    
    void destroy(void* object)
    {
        delete object;
    }
    
    TSArray<TSType *> fields;
    TSType* getFieldByName(const TSString& name);
    
    TSString name;
    TSString description;
    
    virtual void* get(void* owner)
    {
        return NULL;
    }
    
    virtual void setDefaultValue(void* somePointer)
    {
        
    }
    
private:
    TSType();
    
protected:
    TSType(TSString name, TSString description) : name(name), description(description)
    {
        if(!types)
        {
            types = new TSArray<TSType *>;
        }
        
        types->push_back(this);
    }
};
typedef TSType TSEmptyTypeClass;
extern TSEmptyTypeClass* TSEmptyType;

/////////////////////////////////////////////////////////////////////////
// TSObject
/////////////////////////////////////////////////////////////////////////
class TSObject : public TSEmpty
{
public:
    TSType* type;
    virtual bool is(TSType* otherType);
    virtual void destroy();
};

class TSObjectTypeClass : public TSEmptyTypeClass
{
public:
    // Establishes type singleton and heirarchy
    static TSObjectTypeClass typeInstance;
    
    // Type checking and casting
    TSType* base() 
    { 
        return TSEmptyType; 
    }
    
    virtual bool is(TSType* otherType) 
    {
        return this == otherType || base()->is(otherType); 
    }
    
    static TSObjectTypeClass* cast(TSType* otherType)
    {
        return otherType && otherType->is(&typeInstance) ? (TSObjectTypeClass *)otherType : NULL;
    }
    
    static TSObject* cast(const void* value)
    {
        return value && ((TSObject *)value)->type->is(&typeInstance) ? (TSObject *)value : NULL;
    }
    
    // Object instantiation and field subtypes
    virtual void* createInstance() 
    { 
        return create();
    }
    
    static TSObject* create() 
    {
        TSObject* newClass = new TSObject();
        newClass->type = &typeInstance;
        return newClass;
    }
    
    virtual int sizeOf()
    {
        return sizeof(TSObject);
    }
    
protected:
    TSObjectTypeClass(const TSString& name, const TSString& description) : TSType(name, description)
    {
    }
};
extern TSObjectTypeClass* TSObjectType;

/////////////////////////////////////////////////////////////////////////
// Creates a new type for class (with casting), linked with a base type
/////////////////////////////////////////////////////////////////////////
#define TSDeclareTypeClassAbstractMembers(CLASS, PARENT) \
public: \
    static CLASS##TypeClass typeInstance; \
    TSType* base() \
    { \
        return PARENT##Type; \
    } \
    virtual bool is(TSType* otherType) \
    { \
        return this == otherType || base()->is(otherType); \
    } \
    static CLASS##TypeClass* cast(TSType* otherType) \
    { \
        return otherType && otherType->is(&typeInstance) ? (CLASS##TypeClass *)otherType : NULL; \
    } \
protected: \
    CLASS##TypeClass(const TSString& name, const TSString& description) : PARENT##TypeClass(name, description) \
    { \
    }

#define TSDeclareTypeClassMembers(CLASS) \
public: \
    static CLASS* cast(const void* value) \
    { \
        if(typeInstance.is(TSObjectType)) \
        { \
            if(value) \
            { \
                TSObject* object = (TSObject *)value; \
                if(object->type->is(&typeInstance)) \
                { \
                    return (CLASS *)object; \
                } \
            } \
            return NULL; \
        } \
        \
        return (CLASS *)value; \
    } \
    virtual void* createInstance() \
    { \
        return create(); \
    } \
    static CLASS* create() \
    { \
        CLASS* newClass = new CLASS(); \
        { \
            TSObject* typed = (TSObject *)newClass; \
            typed->type = &typeInstance; \
        } \
        return newClass; \
    } \
    virtual int sizeOf() \
    { \
        return sizeof(CLASS); \
    }


#define TSDeclareTypeClass(CLASS, PARENT) \
class CLASS##TypeClass : public PARENT##TypeClass \
{ \
TSDeclareTypeClassAbstractMembers(CLASS, PARENT) \
TSDeclareTypeClassMembers(CLASS) \
}; \
extern CLASS##TypeClass* CLASS##Type;

#define TSDeclareAbstractTypeClass(CLASS, PARENT) \
class CLASS##TypeClass : public PARENT##TypeClass \
{ \
    TSDeclareTypeClassAbstractMembers(CLASS, PARENT) \
public: \
    static CLASS* cast(const void* value) \
    { \
        if(typeInstance.is(TSObjectType)) \
        { \
            if(value) \
            { \
                TSObject* object = (TSObject *)value; \
                if(object->type->is(&typeInstance)) \
                { \
                    return (CLASS *)object; \
                } \
            } \
            return NULL; \
        } \
        \
        return (CLASS *)value; \
    } \
    virtual void* createInstance() \
    { \
        return create(); \
    } \
    static CLASS* create() \
    { \
        return NULL; \
    } \
    virtual int sizeOf() \
    { \
        return sizeof(CLASS); \
    } \
}; \
extern CLASS##TypeClass* CLASS##Type;

/////////////////////////////////////////////////////////////////////////
// Pointer types
/////////////////////////////////////////////////////////////////////////
class PointerTypeClass : public TSType
{
    TSDeclareTypeClassAbstractMembers(Pointer, TSEmpty);
    
public:
    virtual TSType* dereferenced()
    {
        return NULL;
    }
};
extern PointerTypeClass* PointerType;

#define TSDeclarePointerType(CLASS) \
typedef CLASS* CLASS##Ptr; \
class CLASS##PtrTypeClass : public PointerTypeClass \
{ \
    TSDeclareTypeClassAbstractMembers(CLASS##Ptr, Pointer) \
    TSDeclareTypeClassMembers(CLASS##Ptr) \
public: \
    virtual TSType* dereferenced() \
    { \
        return CLASS##Type; \
    } \
}; \
extern CLASS##PtrTypeClass* CLASS##PtrType;

/////////////////////////////////////////////////////////////////////////
// Array types
/////////////////////////////////////////////////////////////////////////
class ArrayTypeClass : public TSType
{
    TSDeclareTypeClassAbstractMembers(Array, TSEmpty);
    
public:
    virtual TSType* memberType()
    {
        return NULL;
    }
    
    virtual int count(void* object) 
    { 
        return 0; 
    }
    
    virtual void* childAtIndex(void* object, int index)
    {
        return NULL;
    }
};
extern ArrayTypeClass* ArrayType;

#define TSDeclareArrayType(CLASS) \
typedef TSArray<CLASS> CLASS##Array; \
class CLASS##ArrayTypeClass : public ArrayTypeClass \
{ \
TSDeclareTypeClassAbstractMembers(CLASS##Array, Array) \
TSDeclareTypeClassMembers(CLASS##Array) \
public: \
virtual TSType* memberType() \
{ \
return CLASS##Type; \
} \
virtual int count(void* object) \
{ \
return (int)((CLASS##Array *)object)->size(); \
} \
virtual void* childAtIndex(void* object, int index) \
{ \
return &(*((CLASS##Array *)object))[index]; \
} \
}; \
extern CLASS##ArrayTypeClass* CLASS##ArrayType;

/////////////////////////////////////////////////////////////////////////
// Putting it all together
/////////////////////////////////////////////////////////////////////////
#define TSDeclareTemplateTypes(CLASS) \
TSDeclareArrayType(CLASS) \
TSDeclarePointerType(CLASS##Array)

#define TSDeclareType(CLASS, BASECLASS) \
TSDeclareTypeClass(CLASS, BASECLASS) \
TSDeclarePointerType(CLASS) \
TSDeclareTemplateTypes(CLASS) \
TSDeclareTemplateTypes(CLASS##Ptr)

#define TSDeclareAbstractType(CLASS, BASECLASS) \
TSDeclareAbstractTypeClass(CLASS, BASECLASS) \
TSDeclarePointerType(CLASS) \
TSDeclareTemplateTypes(CLASS##Ptr)

#define TSImplementTypeClass(CLASS, FRIENDLYNAME) \
CLASS##TypeClass CLASS##TypeClass::typeInstance(#CLASS, FRIENDLYNAME); \
CLASS##TypeClass* CLASS##Type = &CLASS##TypeClass::typeInstance;

#define TSImplementType(CLASS, FRIENDLYNAME) \
TSImplementTypeClass(CLASS,                                     FRIENDLYNAME) \
TSImplementTypeClass(CLASS##Ptr,                                FRIENDLYNAME "*") \
TSImplementTypeClass(CLASS##Array,                              FRIENDLYNAME " array") \
TSImplementTypeClass(CLASS##ArrayPtr,                           FRIENDLYNAME " array pointer") \
TSImplementTypeClass(CLASS##PtrArray,                           FRIENDLYNAME "* array") \
TSImplementTypeClass(CLASS##PtrArrayPtr,                        FRIENDLYNAME "* array pointer")

#define TSImplementAbstractType(CLASS, FRIENDLYNAME) \
TSImplementTypeClass(CLASS,                                     FRIENDLYNAME) \
TSImplementTypeClass(CLASS##Ptr,                                FRIENDLYNAME "*") \
TSImplementTypeClass(CLASS##PtrArray,                           FRIENDLYNAME "* array") \
TSImplementTypeClass(CLASS##PtrArrayPtr,                        FRIENDLYNAME "* array pointer")

#define TSField(FIELDCLASS, CLASS, FIELD, DEFAULTVALUE) \
class CLASS##FIELD##FieldTypeClass : public FIELDCLASS##TypeClass \
{ \
public: \
    static CLASS##FIELD##Field##TypeClass typeInstance; \
    TSType* base() \
    { \
        return FIELDCLASS##Type; \
    } \
    virtual bool is(TSType* otherType) \
    { \
        return this == otherType || base()->is(otherType); \
    } \
    static CLASS##FIELD##Field##TypeClass* cast(TSType* otherType) \
    { \
        return otherType && otherType->is(&typeInstance) ? (CLASS##FIELD##Field##TypeClass *)otherType : NULL; \
    } \
protected: \
    CLASS##FIELD##Field##TypeClass(const TSString& name, const TSString& description) : \
        FIELDCLASS##TypeClass(name, description) \
    { \
        CLASS##Type->fields.push_back(this); \
    } \
public: \
    virtual void* createInstance()  \
    {  \
        return NULL; \
    } \
    virtual void* get(void* somePointer) \
    { \
        CLASS* owner = CLASS##Type->cast(somePointer); \
        return owner ? &owner->FIELD : NULL; \
    } \
    void setDefaultValue(void* somePointer) \
    { \
        CLASS* owner = CLASS##Type->cast(somePointer); \
        if(owner) \
        { \
            owner->FIELD = DEFAULTVALUE; \
        } \
    } \
}; \
CLASS##FIELD##FieldTypeClass CLASS##FIELD##FieldTypeClass::typeInstance(#CLASS #FIELD, #FIELD);




/////////////////////////////////////////////////////////////////////////
// A context can respond to all object IDs
/////////////////////////////////////////////////////////////////////////
class ContainerBase : public TSObject 
{
public:
    virtual bool contains(const TSString& name)
    {
        return false;
    }
};

/////////////////////////////////////////////////////////////////////////
// An environment for references to link up at runtime
/////////////////////////////////////////////////////////////////////////
template <class Child>
class Container : public ContainerBase 
{
public:
    typedef Child ChildType;
    
    Child children[256];
    int childCount;
    TSDictionary<TSString, Child> childrenByName;
    Container<Child>* parent;
    
    virtual Child child(const TSString& name)
    {
        typename TSDictionary<TSString, Child>::iterator it = childrenByName.find(name);
        
        if(it == childrenByName.end())
        {
            if(parent) return parent->child(name);
            else return NULL;
        }
        else return it->second;
    }
    
    virtual Child add(const TSString& name, Child newItem)
    {
        if(childCount == 256)
        {
            assert(!"childCount == 256! oh noes");
        }
        else
        {
            children[childCount] = newItem;
            childCount++;
        }
        
        Child oldItem = childrenByName[name];
        childrenByName[name] = newItem;
        return oldItem;
    }
    
    virtual bool contains(const TSString& name)
    {
        if(childrenByName.find(name) == childrenByName.end())
        {
            if(parent) return parent->contains(name);
            else return false;
        }
        else return true;
    }
    
    Container() : childCount(0), parent(NULL)
	{
        memset(children, 0, sizeof(children));
	}
};

/////////////////////////////////////////////////////////////////////////
// Has strongly typed target reference type
/////////////////////////////////////////////////////////////////////////
template <class Container>
class Reference : public TSObject
{
public:
    typedef typename Container::ChildType Referenced;
    
    virtual operator Referenced ()
    {
        if(!referenced)
        {
            if(!context) return NULL;
            referenced = context->child(id);
        }
        return referenced;
    }
    
    Container* context;
    Referenced referenced;
    TSString id;
    
    Reference() : referenced(this)
    {
        
    }
};

/////////////////////////////////////////////////////////////////////////
// Declarations for Teaspoon's basic types
/////////////////////////////////////////////////////////////////////////
TSDeclarePointerType(TSObject);
TSDeclarePointerType(TSObjectPtr);
TSDeclareTemplateTypes(TSObject);
TSDeclareTemplateTypes(TSObjectPtr);

typedef Container<TSObjectPtr> GenericContainer;
TSDeclareType(ContainerBase, TSObject);
TSDeclareType(GenericContainer, ContainerBase);

typedef Reference<GenericContainer> GenericReference;
class GenericReferenceTypeClass : public TSObjectTypeClass
{
    TSDeclareTypeClassAbstractMembers(GenericReference, TSObject);

public:
    static GenericReference* cast(const void* value)
    {
        if(value && typeInstance.is(TSObjectType))
        {
            TSObject* object = (TSObject *)value;
            if(object->type->is(&typeInstance))
            {
                return (GenericReference *)object;
            }
            else return NULL;
        }
    
        return (GenericReference *)value;
    }
    virtual void* createInstance()
    {
        return create(NULL, "");
    }
    static GenericReference* create(GenericContainer* container, TSString id)
    {
        GenericReference* newClass = new GenericReference();
        newClass->type = &typeInstance;
        newClass->id = id;
        newClass->context = container;
        newClass->referenced = NULL;
        return newClass;
    }
    virtual int sizeOf()
    {
        return sizeof(GenericReference);
    }
};
extern GenericReferenceTypeClass* GenericReferenceType;

TSDeclarePointerType(GenericReference);
TSDeclarePointerType(GenericReferencePtr);
TSDeclareTemplateTypes(GenericReference);
TSDeclareTemplateTypes(GenericReferencePtr);
TSDeclareAbstractType(TSType, TSEmpty);
TSDeclareType(TSString, TSEmpty);

void PrintObjectHierarchy(TSType* type, void* value, int maxLevelsDeep = 10, int levelsDeep = 0);

#endif
