#include "data_desk.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Engine/src/Core/Util.c"

FILE* ComponentsFile                                = NULL;
FILE* FunctionsFile                                 = NULL;
FILE* ComponentArraysFile                           = NULL;
FILE* AttribArraysFile                              = NULL;

int bitOffset                                       = 1;

void GenerateComponent(DataDeskNode* component)
{
    printf("\033[36m%s\033[0m\n", component->name);

    //write the struct and component signature to the components file
    printf("\tGenerating component struct...\n");
    fprintf(ComponentsFile, "#define %s BIT(%i)\n", component->name_uppercase_with_underscores, bitOffset++);
    if(bitOffset == 0xffffffffffffffff)
    {
        printf("\033[31mERROR: Too many components!\033[0m\n");
        exit(-1);
    }
    DataDeskFWriteGraphAsC(ComponentsFile, component, 0);

    //add an array for that type of component
    printf("\tCreating component array...\n");
    fprintf(ComponentArraysFile, "\t%s %s[LC_MAX_ENTITIES];\n", component->name, component->name_upper_camel_case);

    //implement a function to add it
    printf("\tGenerating add function...\n\n");

    char* functionName;
    if (DataDeskNodeHasTag(component, "NoPrefix"))
        functionName = component->name_upper_camel_case;
    else
        functionName = component->name_upper_camel_case + 2;

    fprintf(FunctionsFile, "void lc_Add%s(lc_Scene* scene, lc_Entity entity", functionName);                // write the start of the function
    for (DataDeskNode* member = component->struct_declaration.first_member; member; member = member->next)  // loop through every member of the component and add an argument for it
    {
        int pointerCount                                     = member->declaration.type->type_usage.pointer_count;
        fprintf(FunctionsFile, ", %s", member->declaration.type->name);
        for (int i = 0; i < pointerCount; ++i)
            fprintf(FunctionsFile, "*");
        fprintf(FunctionsFile, " %s", member->name_lowercase_with_underscores);
    }
    fprintf(FunctionsFile, ")\n{\n\tscene->EntitySignatures[entity] |= %s;\n", component->name_uppercase_with_underscores);
    for (DataDeskNode* member = component->struct_declaration.first_member; member; member = member->next)  // loop through members again
    {
        fprintf(FunctionsFile, "\tscene->%s[entity].%s = %s;\n",
             component->name_upper_camel_case, member->name, member->name_lowercase_with_underscores);  // initialise members based on parameters
    }
    fprintf(FunctionsFile, "}\n\n");
}

DATA_DESK_FUNC void
DataDeskCustomInitCallback(void)
{
    printf("\n\033[35mInitialising data desk...\033[0m\n\n");

    //open the files
    ComponentsFile      = fopen("../../../../Engine/src/ECS/Components.gen.c", "w");
    FunctionsFile       = fopen("../../../../Engine/src/ECS/EcsFunctions.gen.c", "w");
    ComponentArraysFile = fopen("../../../../Engine/src/ECS/ComponentArrays.gen.c", "w");

    //setup the components file
    fprintf(ComponentsFile, "#ifndef COMPONENTS_H\n#define COMPONENTS_H\n\n");
    fprintf(ComponentsFile, "#define LC_COMPONENT_NONE 0\n\n");
}

DATA_DESK_FUNC void
DataDeskCustomParseCallback(DataDeskNode* root, char* filename)
{
    printf("\033[35mData desk parse function\033[0m for ");
    if (root->type == DATA_DESK_NODE_TYPE_struct_declaration)
        GenerateComponent(root);
}

DATA_DESK_FUNC void
DataDeskCustomCleanUpCallback(void)
{
    printf("\033[35mFinishing up...\n");
    fprintf(ComponentsFile, "#endif\n");
    fclose(ComponentsFile);
    fclose(FunctionsFile);
    fclose(ComponentArraysFile);
    printf("\033[32mDone!\033[0m\n\n");
}
