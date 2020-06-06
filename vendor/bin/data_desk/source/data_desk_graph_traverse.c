/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Data Desk

Author  : Ryan Fleury
Updated : 5 December 2019
License : MIT, at end of file.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static void
GenerateGraphNullTerminatedStrings(ParseContext *context, DataDeskNode *root)
{
    if(root)
    {
        if(root->string)
        {
            char *new_string = ParseContextAllocateMemory(context, root->string_length+1);
            MemoryCopy(new_string, root->string, root->string_length);
            new_string[root->string_length] = 0;
            root->string = new_string;
            root->name_lowercase_with_underscores = ParseContextAllocateStringCopyLowercaseWithUnderscores(context, root->name);
            root->name_uppercase_with_underscores = ParseContextAllocateStringCopyUppercaseWithUnderscores(context, root->name);
            root->name_lower_camel_case = ParseContextAllocateStringCopyLowerCamelCase(context, root->name);
            root->name_upper_camel_case = ParseContextAllocateStringCopyUpperCamelCase(context, root->name);
        }
        
        switch(root->type)
        {
            case DATA_DESK_NODE_TYPE_unary_operator:
            {
                GenerateGraphNullTerminatedStrings(context, root->unary_operator.operand);
                break;
            }
            case DATA_DESK_NODE_TYPE_binary_operator:
            {
                GenerateGraphNullTerminatedStrings(context, root->binary_operator.left);
                GenerateGraphNullTerminatedStrings(context, root->binary_operator.right);
                break;
            }
            case DATA_DESK_NODE_TYPE_struct_declaration:
            {
                GenerateGraphNullTerminatedStrings(context, root->struct_declaration.first_member);
                break;
            }
            case DATA_DESK_NODE_TYPE_union_declaration:
            {
                GenerateGraphNullTerminatedStrings(context, root->union_declaration.first_member);
                break;
            }
            case DATA_DESK_NODE_TYPE_enum_declaration:
            {
                GenerateGraphNullTerminatedStrings(context, root->enum_declaration.first_constant);
                break;
            }
            case DATA_DESK_NODE_TYPE_flags_declaration:
            {
                GenerateGraphNullTerminatedStrings(context, root->flags_declaration.first_flag);
                break;
            }
            case DATA_DESK_NODE_TYPE_declaration:
            {
                GenerateGraphNullTerminatedStrings(context, root->declaration.type);
                break;
            }
            case DATA_DESK_NODE_TYPE_type_usage:
            {
                GenerateGraphNullTerminatedStrings(context, root->type_usage.first_array_size_expression);
                GenerateGraphNullTerminatedStrings(context, root->type_usage.struct_declaration);
                break;
            }
            case DATA_DESK_NODE_TYPE_constant_definition:
            {
                GenerateGraphNullTerminatedStrings(context, root->constant_definition.expression);
                break;
            }
            case DATA_DESK_NODE_TYPE_procedure_header:
            {
                GenerateGraphNullTerminatedStrings(context, root->procedure_header.first_parameter);
                GenerateGraphNullTerminatedStrings(context, root->procedure_header.return_type);
                break;
            }
            case DATA_DESK_NODE_TYPE_tag:
            {
                GenerateGraphNullTerminatedStrings(context, root->tag.first_tag_parameter);
                break;
            }
            default: break;
        }
        
        if(root->first_tag)
        {
            GenerateGraphNullTerminatedStrings(context, root->first_tag);
        }
        
        if(root->next)
        {
            GenerateGraphNullTerminatedStrings(context, root->next);
        }
    }
}

static void
CallCustomParseCallbacks(ParseContext *context, DataDeskNode *root, DataDeskCustom custom, char *filename)
{
    for(DataDeskNode *node = root; node; node = node->next)
    {
        if(custom.ParseCallback)
        {
            custom.ParseCallback(node, filename);
        }
    }
}

static void
PatchGraphSymbols(ParseContext *context, DataDeskNode *root)
{
    for(DataDeskNode *node = root; node; node = node->next)
    {
        switch(node->type)
        {
            case DATA_DESK_NODE_TYPE_identifier:
            {
                node->identifier.declaration = ParseContextLookUpSymbol(context, node->string, node->string_length);
                break;
            }
            case DATA_DESK_NODE_TYPE_unary_operator:
            {
                PatchGraphSymbols(context, node->unary_operator.operand);
                break;
            }
            case DATA_DESK_NODE_TYPE_binary_operator:
            {
                PatchGraphSymbols(context, node->binary_operator.left);
                PatchGraphSymbols(context, node->binary_operator.right);
                break;
            }
            case DATA_DESK_NODE_TYPE_struct_declaration:
            {
                PatchGraphSymbols(context, node->struct_declaration.first_member);
                break;
            }
            case DATA_DESK_NODE_TYPE_union_declaration:
            {
                PatchGraphSymbols(context, node->union_declaration.first_member);
                break;
            }
            case DATA_DESK_NODE_TYPE_declaration:
            {
                PatchGraphSymbols(context, node->declaration.type);
                PatchGraphSymbols(context, node->declaration.initialization);
                break;
            }
            case DATA_DESK_NODE_TYPE_type_usage:
            {
                if(!node->type_usage.struct_declaration && !node->type_usage.union_declaration)
                {
                    node->type_usage.type_definition = ParseContextLookUpSymbol(context, node->string, node->string_length);
                }
                break;
            }
            case DATA_DESK_NODE_TYPE_tag:
            {
                PatchGraphSymbols(context, node->tag.first_tag_parameter);
                break;
            }
            case DATA_DESK_NODE_TYPE_procedure_header:
            {
                PatchGraphSymbols(context, node->procedure_header.return_type);
                PatchGraphSymbols(context, node->procedure_header.first_parameter);
                break;
            }
            default: break;
        }
        
        if(node->first_tag)
        {
            PatchGraphSymbols(context, node->first_tag);
        }
    }
}

/*
Copyright 2019 Ryan Fleury

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/