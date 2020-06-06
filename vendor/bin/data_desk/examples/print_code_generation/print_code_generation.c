#include <stdio.h>
#include "data_desk.h"

static FILE *global_header_file = 0;
static FILE *global_implementation_file = 0;

// NOTE(rjf): "access_string" is basically a prefix to a member of a struct, so something like
// "object." or "object->". This can be changed in recursive calls, so you can descend a struct
// recursively.
static void GeneratePrintCode(FILE *file, DataDeskNode *root, char *access_string);

DATA_DESK_FUNC void
DataDeskCustomInitCallback(void)
{
	global_header_file = fopen("generated_print.h", "w");
	global_implementation_file = fopen("generated_print.c", "w");
}

DATA_DESK_FUNC void
DataDeskCustomParseCallback(DataDeskNode *root, char *filename)
{
	DataDeskFWriteGraphAsC(global_header_file, root, 0);
	if(root->type == DATA_DESK_NODE_TYPE_struct_declaration)
	{
		fprintf(global_header_file, "void Print%s(%s *object);\n", root->string, root->string);
		fprintf(global_implementation_file, "void Print%s(%s *object)\n{\n", root->string, root->string);
		GeneratePrintCode(global_implementation_file, root->struct_declaration.first_member, "object->");
		fprintf(global_implementation_file, "}\n\n");
	}
}

DATA_DESK_FUNC void
DataDeskCustomCleanUpCallback(void)
{
	fclose(global_header_file);
	fclose(global_implementation_file);
}

static void
GeneratePrintCode(FILE *file, DataDeskNode *root, char *access_string)
{
	fprintf(file, "printf(\"{ \");\n");
	for(DataDeskNode *node = root; node; node = node->next)
	{
		if(!DataDeskNodeHasTag(node, "NoPrint") && node->type == DATA_DESK_NODE_TYPE_declaration)
		{
			if(DataDeskDeclarationIsType(node, "int"     ) || DataDeskDeclarationIsType(node, "uint") ||
			   DataDeskDeclarationIsType(node, "int32_t" ) || DataDeskDeclarationIsType(node, "i32" ) ||
			   DataDeskDeclarationIsType(node, "int16_t" ) || DataDeskDeclarationIsType(node, "i16" ) ||
			   DataDeskDeclarationIsType(node, "int8_t"  ) || DataDeskDeclarationIsType(node, "i8"  ) ||
			   DataDeskDeclarationIsType(node, "uint32_t") || DataDeskDeclarationIsType(node, "u32" ) ||
			   DataDeskDeclarationIsType(node, "uint16_t") || DataDeskDeclarationIsType(node, "u16" ) ||
			   DataDeskDeclarationIsType(node, "uint8_t" ) || DataDeskDeclarationIsType(node, "u8"  ))
			{
				fprintf(file, "printf(\"%%i\", %s%s);\n", access_string, node->string);
			}

			else if(DataDeskDeclarationIsType(node, "float" ) || DataDeskDeclarationIsType(node, "double") ||
			        DataDeskDeclarationIsType(node, "f32"   ) || DataDeskDeclarationIsType(node, "f64"   ))
			{
				fprintf(file, "printf(\"%%f\", %s%s);\n", access_string, node->string);
			}

			else if(DataDeskStringHasSubString("char", node->declaration.type->string) &&
				    node->declaration.type->type_usage.first_array_size_expression)
			{
				fprintf(file, "printf(\"%%s\", %s%s);\n", access_string, node->string);
			}

			else if(DataDeskDeclarationIsType(node, "char"))
			{
				fprintf(file, "printf(\"%%c\", %s%s);\n", access_string, node->string);
			}

			else if(DataDeskDeclarationIsType(node, "*char"))
			{
				fprintf(file, "printf(\"%%s\", %s%s);\n", access_string, node->string);
			}

			else if(DataDeskDeclarationIsType(node, "*void"))
			{
				fprintf(file, "printf(\"%%p\", %s%s);\n", access_string, node->string);
			}

			else
			{
				if(node->declaration.type->type_usage.type_definition &&
			  	   node->declaration.type->type_usage.pointer_count <= 1)
				{
					char next_access_string[128] = {0};
					snprintf(next_access_string, sizeof(next_access_string), "%s%s%s", access_string, node->string,
							 node->declaration.type->type_usage.pointer_count == 1 ? "->" : ".");
					GeneratePrintCode(file, node->declaration.type->type_usage.type_definition->struct_declaration.first_member,
									  next_access_string);
				}
				else if(node->declaration.type->type_usage.struct_declaration &&
					    node->declaration.type->type_usage.pointer_count <= 1)
				{
					char next_access_string[128] = {0};
					snprintf(next_access_string, sizeof(next_access_string), "%s%s%s", access_string, node->string,
							 node->declaration.type->type_usage.pointer_count == 1 ? "->" : ".");
					GeneratePrintCode(file, node->declaration.type->type_usage.struct_declaration->struct_declaration.first_member,
									  next_access_string);
				}
			}

			fprintf(file, "printf(\", \");\n");
		}
	}
	fprintf(file, "printf(\"}\");\n");
}
