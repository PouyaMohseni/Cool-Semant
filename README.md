# **COOL Compiler Semantic Phase Documentation**

## **Introduction**

This documentation covers the semantic (4th) phase of the COOL (Classroom Object-Oriented Language) compiler project, as part of the compiler course at [Stanford University](https://web.stanford.edu/class/cs143/).

For detailed instructions on this phase, please refer to the [provided PDF document](https://drive.google.com/file/d/1FFIpKjp4GSffSTOxyTnAx706BuxSwPxF/view?usp=sharing).

## Code Organization

The primary file for this phase is `semant.cc`, which contains essential methods and classes. Below, we'll explore key components of this code.

### Symbol Definitions

In the code, there are numerous predefined symbols that play a crucial role in the semantic analysis process. These symbols include primitive types and method names, as well as fixed names used by the runtime system. They are defined as follows:

```cpp
static Symbol 
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
```

## **Class Definitions**

The provided code defines several classes and data structures used during the semantic analysis of the COOL program. These classes include attr_, method_, inherited_, and tree_inherited_, among others. Each class serves a specific purpose in the analysis process and helps organize the program's structure.

## **Semantic Analysis Environment**

The semant_env_ class represents the program's semantic environment and encapsulates important data structures, such as attr_env, method_env, and class_ptr. This class enables the program to track and verify attributes, methods, and class relationships during semantic analysis.

## **Initialization and Class Setup**

The code snippet also contains initialization routines, such as initialize_constants(), which sets up the predefined symbols, and tree_inherited_::in_tree_initialize(), which initializes the inheritance tree with predefined classes (e.g., Object, IO, Int, Bool, and Str).

## **Usage of Provided Code**

The provided code forms the foundation for performing semantic analysis on COOL programs. It defines the necessary symbols, structures, and classes to validate the program's correctness, inheritance hierarchy, attributes, and methods.

To use this code effectively, follow these steps:

1. **Initialization**: Ensure that the initialize_constants() function is called to set up the predefined symbols before performing semantic analysis.

2. **Inheritance Tree**: Utilize the tree_inherited_ class and its functions to build and validate the inheritance tree of program classes. This includes adding classes, checking for cycles, and verifying the existence of a Main class.

3. **Semantic Analysis**: Utilize the semant_env_ class to perform semantic analysis on individual classes and expressions within the program. This includes verifying attributes, methods, and type compatibility.

## **Program Architecture**

Our implementation follows Object-Oriented Programming (OOP) principles. Three core objects play vital roles in this phase:

1. **Program Classes**: These objects store information about the program's classes.

2. **Inheritance Tree**: This structure represents the inheritance hierarchy of classes.

3. **Semantic Environment**: It maintains the context and information needed for semantic analysis.

## **Tree Traversals**

The semantic phase involves five tree traversals to perform various tasks, ensuring the accuracy and integrity of the program.

### **First Traversal**

In the first traversal, we accomplish the following:

- Add the names of existing classes in the program.
- Check for the repetition of class definitions.
- Resolve conflicts if there are two different definitions under the same name by considering the first definition.

### **Second Traversal**

During the second traversal:

- Inheritance properties are added to classes.
- We verify the existence of class ancestors.

### **Third Traversal**

The third traversal focuses on:

- Locating and recording the methods defined in each class.
- Adding the formals, including their input and output declared types, to the class object.

### **Fourth Traversal**

Similar to the third traversal, the fourth traversal:

- Navigates through the attributes of the classes.
- Adds attribute names and types to the class attributes.

### **Fifth Traversal**

In the fifth traversal, all necessary information has been collected, and the goal is to solidify the entire tree structure.

## **Semantic Analysis**

To ensure correctness, we apply semantic analysis to attributes and methods:

- For attributes, we validate logical rules and apply type adaptation based on the environment of all attributes.

- For methods, we first substitute parameters with their defined types in the formal environment. Then, we perform type checking within the method and verify the output type.

## **Known Issues**

It's important to acknowledge the following known issues in the current implementation:

1. **Line Error Display**: The error reporting always displays the first line. This issue may be related to not utilizing the table class effectively.

2. **File Consideration**: The compiler does not consider the file in which a class is stored, resulting in error messages that do not reference different file names.

3. **Subtyping with Self-type**: The relationship of being subtyped when involving self-typed expressions does not consistently work due to the corresponding function not recognizing this case.

## **Potential Improvements**

To enhance the current implementation, consider the following improvement opportunities:

- When requesting the node type from the tree, outputting the symbol type instead of a string could enhance clarity and accuracy in type-related operations.

By addressing these issues and implementing potential improvements, the COOL compiler's semantic phase can achieve greater functionality and reliability.
