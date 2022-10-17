

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <iterator>
#include <typeinfo>
#include <string>
#include <algorithm>
#include "semant.h"
#include "utilities.h"



extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
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

//
// Initializing the predefined symbols.
//

static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}






// CLASS: For New Tree Definitions



//attr_ is for keeping attributes of a class.
////formal name and declered type are stored in this object
class attr_ {

public:

    Symbol attr;
    Symbol type;

    //Reset a formal attribute and its type
    void reset (Symbol a, Symbol t){
        attr = a;
        type = t;
    }
};



//method_ is for keeping the track of methods od a class which can be defined by user 
////or is predefined by cool
////method_ holds the name of the method, the number of formal parameters, formal types
///and formal names.
class method_ {

public:

    Symbol name;
    int n_formal;
    std::vector <Symbol> types;
    std::vector <attr_> arg_type;
    attr_ attr_temp;

    //Reset the name of the funvtion
    void reset (Symbol me_name, std::vector<Symbol> me_type){
        name = me_name;
        types = me_type;
        n_formal = me_type.size()-1;
    }

    //Add formal type and name
    void add_arg_type (Symbol a, Symbol b){
        attr_temp.reset(a,b);
        arg_type.push_back(attr_temp);
    }
};



//inherited_ is a class that stores user-defined or pre-defined classes of a program with their inheritace tree
///It holdes the class name, its ansestor, its methods, its attributes
class inherited_ {

  public:

    inherited_* next;
    Symbol identifier;
    std::vector<method_> in_method;
    std::vector<attr_> in_attr;
    method_ method_temp;
    attr_ attr_temp;

    //Reset the inheritance
    void reset(Symbol i, inherited_* p) {
        identifier = i;
        next = p;

	in_method.clear();
	in_attr.clear();
    }

    //Add method to the class with formals
    void in_addmethod(Symbol name, std::vector<Symbol> type){

        method_temp.reset(name,type);
        in_method.push_back(method_temp);
    }

    //Add an attribute to the class; its formal name and type
    void in_addattr(Symbol a, Symbol b){

        attr_temp.reset(a,b);
        in_attr.push_back(attr_temp);
    }

    //Validating the excistance of a method in a class
    int in_methodindex(Symbol method_name) {
	for (int i = 0; i <(in_method).size(); i++) {
            if (in_method[i].name==method_name){

            	return i;
            }
    	}

        return -1;
    }

    //Validating the parameters of a method in a class
    int in_methodpara(int index, std::vector<Symbol> act_type){
	if(act_type.size()!=in_method[index].types.size()-1){

	     return -1; //method not found
	} 
	for (int i=0;i<act_type.size();i++){
	    if (act_type[i]!=in_method[index].types[i]){

		return (i+1); //index of the methos
	    }
	}

	return 0; //parameters did not match
    }

};



//tree_inherited_ is a class that conduct sematic checks of the inheritance tree of program classes
///almost all of the functions that checks the validity of the tree is defined here
class tree_inherited_ {

public:

    std::vector<inherited_> in_tree;

    //Reserving space for program; increased if exceeded
    void tree_inherited_set() {
        in_tree.reserve(100);
    }

    //Find the position of a class in inheritance tree
    int in_tree_findpos(Symbol s);

    //Initial the inheritance tree with predefined classes
    void in_tree_initialize();

    //Add a class to the inheritance tree. This class should have been definded by the programer in
    ////soon-to-be compiled program
    bool in_tree_addclass(Classes classes_root);

    //Add new inheritance edge to the inheritance tree
    ////this function is utelized during the secound check.
    bool in_tree_addinheritance(Classes classes_root);

    //Checking initial validities of semantic.
    ////such as: existance of main, duplicates etc.
    bool in_tree_inmain();

    //Checking the use of self in the inheritance tree
    bool in_tree_inself();

    //Checking if the tree contains cycle.
    ////If so, return classes that are in "cycles" as the output  
    std::vector<int> in_tree_cycle();

    //A function that envoke checking and adding all the methods of every class
    //// in the program.
    bool in_tree_addmethod(Classes classes_root);

    //A function that envoke checking and adding all the attributes of every class
    //// in the program.
    bool in_tree_addattr(Classes classes_root);

    //Add the main method and check the inheritance of that
    bool in_tree_inmainmethod();

    //Find the least upper bond of two classes in the
    ////inheritance tree
    int in_tree_lub(Symbol type_1, Symbol type_2);

    //Find method by its name in a specidic class
    ////This function also checks the method in the ancestors of that class
    ////in case it is defined inheritab;y
    ////If it is find the function retuens that method 
    ////if not, the Void function is returend  
    method_ in_tree_findmethod(Symbol method, Symbol classname);

    //Check if B is the subclass of A or not
    ////This function handels the case in which one or both of the classes 
    /////are Object.
    ////A------>B-->Object  or B<=A
    bool in_tree_subclass (Symbol A, Symbol B); 

    //Check if a method is defined in the class or not.
    ////This function point to the least ancestor class that contains
    ////that function.
    ////if there is none, -1 is returend
    int in_tree_methodclass(Symbol class_n, Symbol method_n){
	int temp_i = in_tree_findpos(class_n);
	int keepTheloop = 0;
	inherited_* temp_s = &in_tree[temp_i];

	while (temp_s->identifier!=Object){
	    if(temp_s->in_methodindex(method_n)!=-1){
		return in_tree_findpos(temp_s->identifier);
	    }
	    if(keepTheloop==in_tree.size()){

		return -1;
	    }
	    temp_s = temp_s->next;
	    keepTheloop +=1;
	}
	if (temp_s->in_methodindex(method_n)!=-1){		
	    return in_tree_findpos(temp_s->identifier);
	}

	return -1;
    }

};


//End Of CLASS: For New Tree Definitions

//This class resembles the programs semant enviorment
////It contains O,M,C which is used in the formal varification 
////of the program
////functions are defined to let the environment pace up or down
////in the inheritance tree meanwhile keeping it updated by changing
////the class(C), attribute function (O), or methods (M)
class semant_env_ {

public:
    std::vector<attr_> attr_env;        	//O: attribute map
    std::vector<method_> method_env;	    //M: method map
    inherited_* class_ptr;		            //C: class

    //Provoke semantical analysis of a program when the inheritance tree is 
    ////made. the root of tree id class_root and the inheritance tree is
    ////class_tree
    bool semant_program(tree_inherited_ class_tree, Classes class_root);
    
    //Provoke semantical analysis of a class when the inheritance tree is 
    ////made. the class is class_root and the inheritance tree is
    ////class_tree
    bool semant_class(tree_inherited_ class_tree, Class_ the_class);
    
    //Change the type declaration of an attribute
    bool semant_overwrite(Symbol new_name, Symbol new_type);
    
    //Provoke semantical analysis of attributes of a class
    bool semant_class_attr(tree_inherited_ class_tree, Feature the_attr);
    
    //Provoke semantical analysis of methods of a class
    bool semant_class_method(tree_inherited_ class_tree, Feature the_method);
    
    //Semant expressions in a class
    Symbol semant_uni(tree_inherited_ class_tree, Expression the_exp);

};




//CLASS METHODS: For New Tree Definition




void tree_inherited_::in_tree_initialize(){
  inherited_ class_t;
  std::vector<Symbol> method_type;



  //Object//////////////////////////////////////////////////////////////
  class_t.reset(Object, 0);
    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
	//Object Methods

  method_type.push_back(Object);
  class_t.in_addmethod(cool_abort,method_type);
  method_type.clear();


  method_type.push_back(Str);
  class_t.in_addmethod(type_name,method_type);
  method_type.clear();


  method_type.push_back(SELF_TYPE);
  class_t.in_addmethod(cool_abort,method_type);
  method_type.clear();

  //Pushing back the declered class to the tree
  in_tree.push_back(class_t);

  //IO//////////////////////////////////////////////////////////////
  class_t.reset(IO, &in_tree[0]);
    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
	//IO Methods

  method_type.push_back(Str);
  method_type.push_back(SELF_TYPE);
  class_t.in_addmethod(out_string,method_type);
  method_type.clear();


  method_type.push_back(Int);
  method_type.push_back(SELF_TYPE);
  class_t.in_addmethod(out_int,method_type);
  method_type.clear();
  

  method_type.push_back(Str);
  class_t.in_addmethod(in_string,method_type);
  method_type.clear();


  method_type.push_back(Int);
  class_t.in_addmethod(in_int,method_type);
  method_type.clear();

  //Pushing back the declered class to the tree
  in_tree.push_back(class_t);

  //Int//////////////////////////////////////////////////////////////
  class_t.reset(Int, &in_tree[0]);
    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    //with no special method
  class_t.in_addattr(val, prim_slot);

  //Pushing back the declered class to the tree
  in_tree.push_back(class_t); 


  //Bool//////////////////////////////////////////////////////////////
  class_t.reset(Bool, &in_tree[0]);
    //
    // Bool also has only the "val" slot.
    //
    //with no special method
  class_t.in_addattr(val, prim_slot);

  //Pushing back the declered class to the tree
  in_tree.push_back(class_t); 


  //Str//////////////////////////////////////////////////////////////
  class_t.reset(Str, &in_tree[0]);
    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //
  class_t.in_addattr(val, Int);

  class_t.in_addattr(str_field, prim_slot);


  method_type.push_back(Int);
  class_t.in_addmethod(length,method_type);
  method_type.clear();


  method_type.push_back(Str);
  method_type.push_back(Str);
  class_t.in_addmethod(concat,method_type);
  method_type.clear();


  method_type.push_back(Int);
  method_type.push_back(Int);
  method_type.push_back(Str);
  class_t.in_addmethod(substr,method_type);
  method_type.clear();

  //Pushing back the declered class to the tree  
  in_tree.push_back(class_t); 

  return;

}



int tree_inherited_::in_tree_findpos(Symbol s) {
    for (int i = 0; i <(in_tree).size(); i++) {
        if (in_tree[i].identifier==s){
            return i;
        }
    }
    return -1;
}



bool tree_inherited_::in_tree_addclass(Classes classes_root){
    inherited_ class_t;
    int in_tree_ptr_i = 0;
    
    for(int i = classes_root->first(); classes_root->more(i); i = classes_root->next(i)) {
        in_tree_ptr_i = in_tree_findpos(classes_root->nth(i)->class_name());
        if (in_tree_ptr_i!=-1){
            cerr <<"Error: Double Definition For Class "<<classes_root->nth(i)->class_name()<<"\n";
        }
	else{
            class_t.reset(classes_root->nth(i)->class_name(), &in_tree[0]);
            in_tree.push_back(class_t);
	}
    }
    return true;
}



bool tree_inherited_::in_tree_addinheritance(Classes classes_root){
    inherited_ class_t;
    int in_tree_ptr_i = 0;
    int in_tree_ptr_j = 0;
    for(int i = classes_root->first(); classes_root->more(i); i = classes_root->next(i)) {
        //Initializing Inheritance
        //finding inheritance
        in_tree_ptr_i = in_tree_findpos(classes_root->nth(i)->class_name());
        in_tree_ptr_j = in_tree_findpos(classes_root->nth(i)->class_parent());
        if (in_tree_ptr_j==-1){
            cerr <<"Error: Inherited from Undefined Class "<<classes_root->nth(i)->class_parent()<<"/n";
        }
	else{
            in_tree[in_tree_ptr_i].next = &in_tree[in_tree_ptr_j];
        }
    }

    return false;
}



bool tree_inherited_::in_tree_inmain(){
    char *main_ = "main";
    for (int i = 0; i <(in_tree).size(); i++) {
        //Check whether the classes include main
        if (in_tree[i].identifier==Main){
            return true;
        }
    }
    cerr << "Error: No Main Class\n";

    return false;
}



bool tree_inherited_::in_tree_inself(){
    for (int i = 0; i <(in_tree).size(); i++) {
        //Checking if self_type were asqined to name of a class
        if (in_tree[i].identifier==SELF_TYPE){
	    cerr << "Error: SELF_TYPE Cannot Be A Class Name\n";
        
            return false;
        }
    }

    //No self_type in class symbols
    return true;
}



std::vector<int> tree_inherited_::in_tree_cycle() {
  //Storing cycle parts
  std::vector<int> in_tree_cycle_output;
  inherited_* temp_t;
  Symbol temp_t_id;
  //Check if the loop is trapped in a cycle
  int in_tree_ptr_i = 0;

  for (int i = 1; i <in_tree.size(); i++) {
    temp_t_id = in_tree[i].identifier;
    temp_t = &in_tree[i];
    in_tree_ptr_i = 0;
    while (temp_t->identifier!=Object){
        temp_t = temp_t->next;
        in_tree_ptr_i++;
        if (temp_t->identifier==temp_t_id){
            //i is the id of so-being class
            in_tree_cycle_output.push_back(i);
            cerr <<"Error: Class Named " << temp_t_id << " Is A Part Of A Cycle\n";
            break;
        }
        if (in_tree_ptr_i==(in_tree).size()){
            //There is a cycle that contains Object
            break;
        }
    }
  }

  //Return id of classes that were part of any cycle
  return  in_tree_cycle_output;
}




bool tree_inherited_::in_tree_addmethod(Classes classes_root){
    Formals cl_me_temp;
    //store formal type of the method + the return type
    std::vector<Symbol> formal_type_temp;
    inherited_ *class_t;
    int in_tree_ptr_i = 0;
    
    //Loop on each class
    for(int i = classes_root->first(); classes_root->more(i); i = classes_root->next(i)) {
	class_t = &in_tree[in_tree_findpos(classes_root->nth(i)->class_name())];

    //Loop on each method of the selected class
	for(int j = classes_root->nth(i)->class_features()->first(); classes_root->nth(i)->class_features()->more(j); j = classes_root->nth(i)->class_features()->next(j)) {
	    if (classes_root->nth(i)->class_features()->nth(j)->etype()=="method"){
		    formal_type_temp.clear();
		    cl_me_temp = classes_root->nth(i)->class_features()->nth(j)->eformals();
            //Loop on formal parameters and their type
            ////formal_type_temp stores declered type
		    for(int p = cl_me_temp->first(); cl_me_temp->more(p); p = cl_me_temp->next(p)){
			formal_type_temp.push_back(cl_me_temp->nth(p)->etype_decl());

		    }
		    formal_type_temp.push_back(classes_root->nth(i)->class_features()->nth(j)->ereturn_type());

            //the method is added to the class that was determined in the first loop
		    class_t->in_addmethod(classes_root->nth(i)->class_features()->nth(j)->ename(),formal_type_temp);
		}
	}  
    }

    return true;
}


bool tree_inherited_::in_tree_addattr(Classes classes_root){
    //Stores attrebutes
    Formals cl_me_temp;
    //Points to the courent looping class
    inherited_ *class_t;
    int in_tree_ptr_i = 0;
    
    //Loop on program classes
    for(int i = classes_root->first(); classes_root->more(i); i = classes_root->next(i)) {
    //Pointing to the class in the inherited tree
	class_t = &in_tree[in_tree_findpos(classes_root->nth(i)->class_name())];
    //Loop inside each class for finding its attributes
	for(int j = classes_root->nth(i)->class_features()->first(); classes_root->nth(i)->class_features()->more(j); j = classes_root->nth(i)->class_features()->next(j)) {
	    if (classes_root->nth(i)->class_features()->nth(j)->etype()=="attr"){

        //Add the attribute (its formal with its type) to the class attributes 
		class_t->in_addattr(classes_root->nth(i)->class_features()->nth(j)->ename(),classes_root->nth(i)->class_features()->nth(j)->etype_decl());
	    }
	}  
    }

return true;
}



bool tree_inherited_::in_tree_inmainmethod () {
    for (int i = 0; i <(in_tree).size(); i++) {
        //Find main method in the inherited tree
        if (in_tree[i].identifier==Main){
            for (int j = 0; j <in_tree[i].in_method.size(); j++) {
            //Find the main_meth in the methods of the main
	        if (in_tree[i].in_method[j].name==main_meth){
	        
                //Everything seems fine
                return true;
	        }
    	    }
	    cerr << "Error: No main Method in The Main Class\n";
    	    
            return false;
        }
    }
    cerr << "Error: No Main Class Caused No main Method\n";
    
    return false;
}


int tree_inherited_::in_tree_lub(Symbol type_1, Symbol type_2){

    //Define two vectores in each the id of the related class is pushed
    std::vector <int> type1_temp;
    type1_temp.push_back(in_tree_findpos(type_1));
    std::vector <int> type2_temp;
    type2_temp.push_back(in_tree_findpos(type_2));

    //In two different loops, each vector will be filled with all of the ansectores of the related class
    ////In order to be able to find LUB
    while (in_tree[type1_temp[type1_temp.size()-1]].next->identifier!=Object){
        type1_temp.push_back(in_tree_findpos(in_tree[type1_temp[type1_temp.size()-1]].next->identifier));
    }
    while (in_tree[type2_temp[type2_temp.size()-1]].next->identifier!=Object){
        type2_temp.push_back(in_tree_findpos(in_tree[type2_temp[type2_temp.size()-1]].next->identifier));
    }

    //Object id is added to each vector
    type1_temp.push_back(0);
    type2_temp.push_back(0);

    //Reverse vectores to compermise easer
    reverse(type1_temp.begin(), type1_temp.end());
    reverse(type2_temp.begin(), type2_temp.end());

    //LUB of the lenth: the diammeter of the classes
    int max_size = std::min(type1_temp.size(),type2_temp.size());
    for (int i=0; i<max_size; i++){
        if (type1_temp[i]!=type2_temp[i]){
     
            //Find the LUB of two classes based on the related vectores 
            ////which contains class id
            return type1_temp[i-1];
        }
    }

    //Two classes are from the different programs
    return -1
}  


method_ tree_inherited_::in_tree_findmethod(Symbol method_name, Symbol classname){
    inherited_* class_temp;
    class_temp = &in_tree[in_tree_findpos(classname)];

    //Pace the inherited tree upward
    while(class_temp->next!=0){
        for (int i=0;i<(class_temp->in_method).size();i++){

            //Find the first method with the method_name in the chain
            if (class_temp->in_method[i].name==method_name){
                return class_temp->in_method[i];
            }
        }
        class_temp = class_temp->next;
    }

    //Return void to point there is no method  with that name in the chain that connect
    ////class_name to Object in the inherited tree

}




bool tree_inherited_::in_tree_subclass (Symbol A, Symbol B) {
    inherited_* temp_t = &in_tree[in_tree_findpos(A)];

    while (temp_t->identifier!=Object){
    //Find if B is in the Ancestor chain of A
	if (temp_t->identifier==B){
	    return true;
	}
	temp_t = temp_t->next;
    }
    if (B==Object){
	
    return true;
    }
    
    return false;
}




//End Of CLASS METHODS: For New Tree Definition




bool semant_env_::semant_program(tree_inherited_ class_tree,Classes classes_root) {
    
    //Point to each class in the program in a loop
    Class_ class_ptr_temp;

    //Loop on classes of the program
    for(int i = classes_root->first(); classes_root->more(i); i = classes_root->next(i)) {
        class_ptr_temp = classes_root->nth(i);

        //Set the environment
        /////semant the class with the inherited tree
	semant_class(class_tree, class_ptr_temp);	
     }
}




bool semant_env_::semant_class(tree_inherited_ class_tree, Class_ the_class){

    //Point to the class that is now the environmant of the semantic
    class_ptr = &class_tree.in_tree[class_tree.in_tree_findpos(the_class->class_name())];

    //Set the formal attributes of selected class as a part of the enironment
    attr_env = class_ptr->in_attr;

    //Set the methods of the selected class as a part of the environment
    method_env = class_ptr->in_method;

    //Semant the subtree of the selected class
    for(int i = the_class->class_features()->first(); the_class->class_features()->more(i); i = the_class->class_features()->next(i)) {
	if (the_class->class_features()->nth(i)->etype()=="attr"){

        //If the selscted subtree is an attribute
	    semant_class_attr(class_tree, the_class->class_features()->nth(i));
	}
	else {

        //If the the selcted subtree is a method
	    semant_class_method(class_tree, the_class->class_features()->nth(i));
	}
    }
}



bool semant_env_::semant_overwrite(Symbol new_name, Symbol new_type) {

    //Overwrite the formal with the assigned type
    //This function is called only if the scope is changed 
    for (int i=0; i<attr_env.size(); i++){
	if (attr_env[i].attr==new_name){
	    attr_env[i].type = new_type;

	    return false;
	}
    }
    attr_ attr_temp;
    attr_temp.reset(new_name, new_type);
    attr_env.push_back(attr_temp);

    return true;   
}


bool semant_env_::semant_class_attr(tree_inherited_ class_tree, Feature the_attr){
    Symbol type_temp;

    //Overwrite the attribute cause we are in a deeper scope 
    semant_overwrite(the_attr->ename(),the_attr->etype_decl());
    type_temp = semant_uni(class_tree, the_attr->eexp());

    //Semant no_expr that is always true
    if (the_attr->eexp()->etype()!= "no_expr"){
	if (class_tree.in_tree_subclass(type_temp,the_attr->ereturn_type())){
	    return 1;
	
    }
	
    //Error i declared type (class) is not in the subset of the expression 
	cerr<<"ERROR in Line "<<the_attr->get_line_number() <<": Wrong Attr OutType, Got "<<type_temp<<". Insted of "<<the_attr->ereturn_type()<<" Attr Name: "<<the_attr->ename()<<".\n";
	
    return 0;
    }
    
    return 1 
;}



bool semant_env_::semant_class_method(tree_inherited_ class_tree, Feature the_method){
    Symbol type_temp;

    //For in class methods
    for(int i = the_method->eformals()->first(); the_method->eformals()->more(i); i = the_method->eformals()->next(i)){

    //Overwrite the expressed types
	semant_overwrite(the_method->eformals()->nth(i)->ename(),the_method->eformals()->nth(i)->etype_decl());
   }

    //Semant the expression
    type_temp = semant_uni(class_tree,the_method->eexp());
    if (type_temp!= SELF_TYPE){
	if (class_tree.in_tree_subclass(type_temp,the_method->ereturn_type())){
	
        return 1;
	}
	
    //comparison the return type with the declered type
	cerr<<"ERROR in Line "<<the_method->get_line_number() <<": Wrong Method OutType, Got "<<type_temp<<". Insted of "<<the_method->ereturn_type()<<" Method Name: "<<the_method->ename()<<".\n";
	
    return 0;
    }
    
    return 1    

;}




Symbol semant_env_::semant_uni(tree_inherited_ class_tree, Expression the_exp){
    //Semat function for expressions

    int temp_index;
    int temp_index2;
    Symbol type_temp;
    Symbol type_temp2;
    std::vector<attr_> attr_env_temp;
    std::vector<Symbol> par_type;

    //If the expression is assign
    if (the_exp->etype()=="assign"){
	type_temp = semant_uni(class_tree,the_exp->eget());
	for (int i=0; i<attr_env.size(); i++){

        //Check if the infered type enhere declared type
   	    if (attr_env[i].attr==the_exp->esymbol()){
		type_temp2 = attr_env[i].type;
        
            //Check for sub-type (sub-class)
	        if (class_tree.in_tree_subclass(type_temp2,type_temp)){
		    the_exp->type = type_temp;

		    return type_temp;
		}
	    }
	}
	cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": The Assigned Type "<<type_temp<<" To "<<type_temp2<<" Is Worng (IS Not Below It).\n";
	the_exp->type = Object;

	return Object;
    }

    //If the expression is static dispatch
    else if (the_exp->etype()=="s_disp"){
	par_type.clear();

	type_temp2 = semant_uni(class_tree,the_exp->eget());
	for(int i =the_exp->ebody()->first(); the_exp->ebody()->more(i); i = the_exp->ebody()->next(i)){
	    par_type.push_back(semant_uni(class_tree,the_exp->ebody()->nth(i)));
	}
	temp_index = class_tree.in_tree[class_tree.in_tree_findpos(the_exp->esymbol2())].in_methodindex(the_exp->esymbol()); //find method index
	if (temp_index == -1){
	    cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": wrong static_dispatch (No Method Named "<<the_exp->esymbol()<<" in "<<the_exp->esymbol2()<<")\n";
	    the_exp->type = Object;

	    return Object;
	}
	temp_index2 = class_tree.in_tree[class_tree.in_tree_findpos(the_exp->esymbol2())].in_methodpara(temp_index,par_type);
	if (temp_index2 == -1){
	    cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": wrong static_dispatch (Wrong Size of Formal Paramters for "<<the_exp->esymbol()<<" in "<<the_exp->esymbol2()<<")\n";
	    the_exp->type = Object;

	    return Object;
	}
	if (temp_index2 != 0){
	    cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": wrong static_dispatch (Wrong Type of Formal Paramter for "<<the_exp->esymbol()<<" in "<<the_exp->esymbol2()<<" in Position "<<temp_index2<<")\n";
	    the_exp->type = Object;

	    return Object;
	}
	type_temp = class_tree.in_tree[class_tree.in_tree_findpos(type_temp2)].in_method[temp_index].types.back();
	if (type_temp != SELF_TYPE){
	    the_exp->type = type_temp;

	    return type_temp;
	}
	type_temp = class_tree.in_tree[class_tree.in_tree_findpos(type_temp2)].identifier;
	the_exp->type = type_temp;

	return type_temp;
    }

    //If the expression is dynamic dispatch
    else if (the_exp->etype()=="disp"){
	par_type.clear();

	type_temp2 = semant_uni(class_tree,the_exp->eget());
	for(int i =the_exp->ebody()->first(); the_exp->ebody()->more(i); i = the_exp->ebody()->next(i)){
	    par_type.push_back(semant_uni(class_tree,the_exp->ebody()->nth(i)));
	}
	temp_index2 = class_tree.in_tree_methodclass(type_temp2,the_exp->esymbol());
	temp_index = class_tree.in_tree[temp_index2].in_methodindex(the_exp->esymbol());
	if (temp_index == -1){
	    cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": wrong dynamic_dispatch (No Method Named "<<the_exp->esymbol()<<" in "<<type_temp2<<" or Any Class Below It)\n";
	    the_exp->type = Object;

	    return Object;
	}
	temp_index2 = class_tree.in_tree[class_tree.in_tree_findpos(type_temp2)].in_methodpara(temp_index,par_type);
	if (temp_index2 == -1){
	    cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": wrong dynamic_dispatch (Wrong Size of Formal Paramters for "<<the_exp->esymbol()<<" in "<<type_temp2<<")\n";
	    the_exp->type = Object;

	    return Object;
	}
	if (temp_index2 != 0){
	    cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": wrong dynamic_dispatch (Wrong Type of Formal Paramter for "<<the_exp->esymbol()<<" in "<<type_temp2<<" in Position "<<temp_index2<<")\n";
	    the_exp->type = Object;

	    return Object;
	}
	type_temp = class_tree.in_tree[class_tree.in_tree_findpos(type_temp2)].in_method[temp_index].types.back();
	if (type_temp != SELF_TYPE){
	    the_exp->type = type_temp;

	    return type_temp;
	}
	type_temp = class_tree.in_tree[class_tree.in_tree_findpos(type_temp2)].identifier;
	the_exp->type = type_temp;

	return type_temp;
    }

    //If the expression is condition
    else if (the_exp->etype()=="cond"){
	if (semant_uni(class_tree, the_exp->eget())==Bool){
	    type_temp = semant_uni(class_tree, the_exp->eget());
	    type_temp2 = semant_uni(class_tree, the_exp->eget());
	    the_exp->type = type_temp;

    	    return type_temp;
	}
    else{
	cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": If Condition Must be Bool Type";
	the_exp->type = Object;

	return Object;
        }
    }

    //If the expression is a loop
    else if (the_exp->etype()=="loop"){
	if (semant_uni(class_tree, the_exp->eget())==Bool){
	    type_temp = semant_uni(class_tree, the_exp->eget());
	    the_exp->type = type_temp;
    	    return type_temp;
	}
    else{
	cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": Loop Condition Must be Bool Type";
	the_exp->type = Object;

	return Object;
	}
    }

    /*
    //If the expression is a type case
    else if (the_exp->etype()=="typecase"){
    }
    */

    //If the expression is a block
    else if (the_exp->etype()=="block"){
	for(int i = the_exp->ebody()->first(); the_exp->ebody()->more(i); i = the_exp->ebody()->next(i)){
	    type_temp = semant_uni(class_tree, the_exp->ebody()->nth(i));
	}
	the_exp->type = type_temp;
	
    return type_temp;
    }

    //If the expression is a let
    else if (the_exp->etype()=="let"){
	type_temp = semant_uni(class_tree, the_exp->eget());
	if (class_tree.in_tree_subclass(type_temp,the_exp->esymbol2())){
	    attr_env_temp = attr_env;
	    semant_overwrite(the_exp->esymbol(),type_temp);
	    type_temp = semant_uni(class_tree, the_exp->eget2());
	    attr_env = attr_env_temp;
	    the_exp->type = type_temp;
    
    	    return type_temp;
	}
	else{
	    attr_env_temp = attr_env;
	    semant_overwrite(the_exp->esymbol(),the_exp->esymbol2());
	    semant_uni(class_tree, the_exp->eget());
	    attr_env = attr_env_temp;
	    the_exp->type = Object;
	    cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": let initialized with a non subclass "<<type_temp<<"!=<"<<the_exp->esymbol2()<<"\n";
    
        return Object;	
	}
	
    }

    //If the expression is an arthimatic plus
    else if (the_exp->etype()=="plus"){
	if (semant_uni(class_tree, the_exp->eget())==Int){
	    if (semant_uni(class_tree, the_exp->eget2())==Int){
		the_exp->type = Int;
		
        return Int;
	    }
        else{
	    cerr <<"ERROR in Line "<<the_exp->get_line_number() <<": +, Adds Int";}
	    the_exp->type = Object;
    	
            return Object;
	}
    else{
	cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": +, Adds Int";}
	the_exp->type = Object;
	
    return Object;
    }

    //If the expression is an arithmatic subtraction
    else if (the_exp->etype()=="sub"){
	if (semant_uni(class_tree, the_exp->eget())==Int){
	    if (semant_uni(class_tree, the_exp->eget2())==Int){
		the_exp->type = Int;
	
    	return Int;
	    }
        else{
	    cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": -, Subs Int";}
	    the_exp->type = Object;
    	
            return Object;
	}
    else{
	cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": -, Subs Int";}
	the_exp->type = Object;
	
    return Object;
    }

    //If the expression is an arithmatic multiplaction
    else if (the_exp->etype()=="mul"){
	if (semant_uni(class_tree, the_exp->eget())==Int){
	    if (semant_uni(class_tree, the_exp->eget2())==Int){
		the_exp->type = Int;
	
    	return Int;
	    }
        else{
	    cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": *, Mults Int";}
	    the_exp->type = Object;
    	
            return Object;
	}
    else{
	cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": *, Mults Int";}
	the_exp->type = Object;
	
    return Object;
    }

    //If the expression is an arithmatic divide
    else if (the_exp->etype()=="divide"){
	if (semant_uni(class_tree, the_exp->eget())==Int){
	    if (semant_uni(class_tree, the_exp->eget2())==Int){
		the_exp->type = Int;
	
    	return Int;
	    }
        else{
	    cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": /, Divides Int";}
	    the_exp->type = Object;
    
    	    return Object;
	}
    else{
	cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": /, Divides Int";}
	the_exp->type = Object;
	
    return Object;
    }

    //If the expression is a boolian negation
    else if (the_exp->etype()=="neg"){
	if (semant_uni(class_tree,the_exp->eget())==Int){
	    the_exp->type = Int;
	
        return Int;
	}
    }

    //If the expression is a less than
    else if (the_exp->etype()=="lt"){
	if (semant_uni(class_tree, the_exp->eget())==Int){
	    if (semant_uni(class_tree, the_exp->eget2())==Int){
		the_exp->type = Bool;
	
    	return Bool;
	    }
        else{
	    cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": <, Compers Int";}
	    the_exp->type = Object;
    	
            return Object;
	}
    else{
	cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": <, Compers Int";}
	the_exp->type = Object;
	
    return Object;
    }

    //If the expression is a less than equal
    else if (the_exp->etype()=="leq"){
	if (semant_uni(class_tree, the_exp->eget())==Int){
	    if (semant_uni(class_tree, the_exp->eget2())==Int){
		the_exp->type = Bool;
	
    	return Bool;
	    }
        else{
	    cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": *, Mults Int";}
	    the_exp->type = Object;
    	
            return Object;
	}
    else{
	cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": *, Mults Int";}
	the_exp->type = Object;
	
    return Object;
    }

    /*
    //If the expression is coparson
    else if (the_exp->etype()=="comp"){
    }
    */

    //If the expression is a constant integer
    else if (the_exp->etype()=="int_const"){
	the_exp->type = Int;
	
    return Int;
    }

    //If the expression is a constant boolian
    else if (the_exp->etype()=="bool_const"){
	the_exp->type = Bool;
	
    return Bool;
    }

    //If the expression is a constant string
    else if (the_exp->etype()=="string_const"){
	the_exp->type = Str;
	
    return Str;
    }

    //If the expression is a new object
    else if (the_exp->etype()=="new"){
	type_temp = the_exp->esymbol();
	if (type_temp==SELF_TYPE){
	    the_exp->type = SELF_TYPE;
	
        return SELF_TYPE;
	}
	else{
	    the_exp->type = type_temp;
	
        return type_temp;	    
	}
    }

    //If the expression is the isvoid 
    else if (the_exp->etype()=="isvoid"){
	semant_uni(class_tree, the_exp->eget());
	the_exp->type = Bool;
	
    return Bool;
    }

    //If the expression is no expression (void)
    else if (the_exp->etype()=="no_expr"){
	the_exp->type = Object;

	return Object;    
    }

    //If the expression is an object (comperhensive)
    else if (the_exp->etype()=="object"){
	for (int i=0; i<attr_env.size(); i++){
   	    if (attr_env[i].attr==the_exp->esymbol()){
		the_exp->type = attr_env[i].type;

	        return attr_env[i].type;
	    }
	}

    //If the expression is a self type
	if (the_exp->esymbol()==self){
	    the_exp->type = SELF_TYPE;

	    return SELF_TYPE;
	}
	cerr<<"ERROR in Line "<<the_exp->get_line_number() <<": Undifined Identifier "<<the_exp->esymbol()<<" .\n";
	the_exp->type = Object;

	return Object;
    }

/*
    //If the expression is a not
    else if (the_exp->etype()=="not"){
	if (semant_uni(class_tree,the_exp->eget())==Bool){
	    the_exp->type = Bool;
	    return Bool;
	}
    }

*/

;}














ClassTable::ClassTable(Classes classes) : semant_errors(0) , error_stream(cerr) {


}

void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
   // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");
    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
	class_(Object, 
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = 
	class_(IO, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename);  

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
	class_(Int, 
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
	class_(Str, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat, 
								      single_Formals(formal(arg, Str)),
								      Str, 
								      no_expr()))),
			       single_Features(method(substr, 
						      append_Formals(single_Formals(formal(arg, Int)), 
								     single_Formals(formal(arg2, Int))),
						      Str, 
						      no_expr()))),
	       filename);
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
    return semant_error(c->get_filename(),c);
}    

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error()                  
{                                                 
    semant_errors++;                            
    return error_stream;
} 



/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{
    initialize_constants();

	// CLASS CHECKING
    tree_inherited_ tree_inherited;
    tree_inherited.tree_inherited_set();
    tree_inherited.in_tree_initialize();
    tree_inherited.in_tree_addclass(classes);
    tree_inherited.in_tree_addinheritance(classes);
    tree_inherited.in_tree_inmain();
    tree_inherited.in_tree_inself();
    tree_inherited.in_tree_cycle();
   	// END CLASS CHECKING
//////////////////////////////////////////////////////////////////////////////////////////////
	// MACKE METHOD MAP: <CLASS, METHOD> -> (TYPE1,...,TYPEn+1)
    tree_inherited.in_tree_addmethod(classes);
    tree_inherited.in_tree_inmainmethod();
	// END MACKE METHOD MAP: <CLASS, METHOD> -> (TYPE1,...,TYPEn+1)
//////////////////////////////////////////////////////////////////////////////////////////////
	// MAKE CLASS_ATTR ENV: <CLASS, ATTR> -> (TYPE)
    tree_inherited.in_tree_addattr(classes);
	// END MAKE CLASS_ATTR ENV: <CLASS, ATTR> -> (TYPE)    
//////////////////////////////////////////////////////////////////////////////////////////////
	// SEMANT EVERY CLASS
    semant_env_ semant_env;
    semant_env.semant_program(tree_inherited, classes);
	// END SEMANT EVERY CLASS
 
}


