#include "data_desk.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

#include "../Engine/src/Core/Util.c"

static FILE* ComponentsFile                                = NULL;
static FILE* FunctionsFile                                 = NULL;
static FILE* ComponentArraysFile                           = NULL;

static int bitOffset                                       = 0;

void GenerateComponent(DataDeskNode* component)
{
    printf("%s\n", component->name);

	//write the struct and component signature to the components file
    printf("\tGenerating component struct...\n");
	fprintf(ComponentsFile, "#define %s BIT(%i)\n", component->name_uppercase_with_underscores, bitOffset++);
	DataDeskFWriteGraphAsC(ComponentsFile, component, 0);

	//add an array for that type of component
    printf("\tCreating component array...\n");
	fprintf(ComponentArraysFile, "\t%s %s[MAX_ENTITIES];\n", component->name, component->name_upper_camel_case);

	//implement a function to add it
    printf("\nGenerating add function...\n\n");

    char* functionName;
	if (DataDeskNodeHasTag(component, "NoPrefix"))
		functionName = component->name_upper_camel_case;
	else
		functionName = component->name_upper_camel_case + 2;

	fprintf(FunctionsFile, "void lc_Add%s(lc_Scene* scene, lc_Entity entity", functionName);                //write the start of the function signature
	for (DataDeskNode* member = component->struct_declaration.first_member; member; member = member->next)  //loop through every member of the component and add an argument for it
	{
		int pointerCount                                     = member->declaration.type->type_usage.pointer_count;
		switch (pointerCount)
		{
		case 0:
			fprintf(FunctionsFile, ", %s %s", member->declaration.type->name, member->name_lowercase_with_underscores);
			break;
		case 1:
			fprintf(FunctionsFile, ", %s* %s", member->declaration.type->name, member->name_lowercase_with_underscores);
			break;
		case 2:
			fprintf(FunctionsFile, ", %s** %s", member->declaration.type->name, member->name_lowercase_with_underscores);
			break;
		default:
			printf("ERROR: Only supports up to double pointers");
			break;
		}
	}
	fprintf(FunctionsFile, ")\n");                                                                          //end the line
	fprintf(FunctionsFile, "{\n");
	fprintf(FunctionsFile, "\tlc_AddComponent(scene, entity, %s);\n", component->name_uppercase_with_underscores);
	for (DataDeskNode* member = component->struct_declaration.first_member; member; member = member->next)  //loop through every member of the component and assign to the value of the equivalent component in the struct passed in
	{
		fprintf(FunctionsFile, "\tscene->%s[entity].%s = %s;\n", component->name_upper_camel_case, member->name, member->name_lowercase_with_underscores);
	}
	fprintf(FunctionsFile, "}\n\n");

	printf("Writing components...\n");
}

DATA_DESK_FUNC void
DataDeskCustomInitCallback(void)
{
    printf("Data desk init function\n");

	//open the files
	ComponentsFile = fopen("../../../../Engine/src/ECS/Components.gen.c", "w");
	FunctionsFile = fopen("../../../../Engine/src/ECS/EcsFunctions.gen.c", "w");
	ComponentArraysFile = fopen("../../../../Engine/src/ECS/ComponentArrays.gen.c", "w");

	//setup the components file
	fprintf(ComponentsFile, "#pragma once\n\n");
	fprintf(ComponentsFile, "#include <stdint.h>\n");
	fprintf(ComponentsFile, "#define LC_COMPONENT_NONE 0\n\n");
}

DATA_DESK_FUNC void
DataDeskCustomParseCallback(DataDeskNode* root, char* filename)
{
    printf("Data desk parse function for ");
	if (root->type == DATA_DESK_NODE_TYPE_struct_declaration)
		GenerateComponent(root);
}

DATA_DESK_FUNC void
DataDeskCustomCleanUpCallback(void)
{
	fclose(ComponentsFile);
	fclose(FunctionsFile);
	fclose(ComponentArraysFile);
}
