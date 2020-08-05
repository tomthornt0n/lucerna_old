#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lcddl.h"

FILE *ComponentsFile = NULL;
FILE *FunctionsFile = NULL;
FILE *FunctionsHeaderFile = NULL;
FILE *ComponentArraysFile = NULL;

int bitOffset = 1;

void fWriteStringAsUppercaseWithUnderscores(FILE *file, char *string)
{
    int stringLength = 0;
    for (; string[stringLength]; ++stringLength);

    int i;
    for(i = 0; i < stringLength && string[i]; ++i)
    {
        fprintf(file, "%c",
               (string[i] >= 'a' && string[i] <= 'z') ?
                string[i] - 32 : string[i]);

        if(string[i+1] >= 'A' && string[i+1] < 'Z' && string[i] != '_')
            fprintf(file, "_");
    }
}

void fWriteStringAsLowercaseWithUnderscores(FILE *file, char *string)
{
    int stringLength = 0;
    for (; string[stringLength]; ++stringLength);

    int i;
    for (i = 0; i < stringLength && string[i]; ++i)
    {
        fprintf(file, "%c",
               (string[i] >= 'A' && string[i] <= 'Z') ?
                string[i] + 32 : string[i]);

        if (string[i+1] >= 'A' && string[i+1] < 'Z' && string[i] != '_')
            fprintf(file, "_");
    }
}

void fWriteStringAsTitleCase(FILE *file, char *string)
{
    int stringLength = 0;
    for (; string[stringLength]; ++stringLength);

    int needsUppercase = 1;

    int i;
    for (i = 0; i < stringLength && string[i]; ++i)
    {
        if (string[i] != '_')
        {
            if (string[i - 1] == '_' &&
                i > 0)
                needsUppercase = 1;
        }

        if (needsUppercase)
        {
            fprintf(file, "%c",
                   (string[i] >= 'a' && string[i] <= 'z') ?
                    string[i] - 32 : string[i]);
            needsUppercase = 0;
        }
        else if (string[i] != '_')
        {
            fprintf(file, "%c", string[i]);
        }
    }
}

void GenerateComponent(lcddlNode_t *component)
{
    fprintf(stderr, "        Generating component struct...\n");
    fprintf(ComponentsFile, "#define ");
    fWriteStringAsUppercaseWithUnderscores(ComponentsFile, component->Name);
    fprintf(ComponentsFile, " (1 << %i)\n", bitOffset++);
    if(bitOffset == 0xffffffffffffffff)
    {
        fprintf(stderr, "\033[31mERROR: Too many components!\033[0m\n");
        exit(-1);
    }
    lcddl_WriteNodeToFileAsC(ComponentsFile, component);

    fprintf(stderr, "        Creating component array...\n");
    fprintf(ComponentArraysFile, "    %s ", component->Name);
    fWriteStringAsTitleCase(ComponentArraysFile, component->Name);
    fprintf(ComponentArraysFile, "[%u];\n", component->IndirectionLevel);

    fprintf(stderr, "        Generating add function...\n\n");

    fprintf(FunctionsFile, "void lcAdd");
    fprintf(FunctionsHeaderFile, "void lcAdd");
    fWriteStringAsTitleCase(FunctionsFile, component->Name);
    fWriteStringAsTitleCase(FunctionsHeaderFile, component->Name);
    fprintf(FunctionsFile, "(lcScene_t* scene, lcEntity_t entity");
    fprintf(FunctionsHeaderFile, "(lcScene_t* scene, lcEntity_t entity");
    lcddlNode_t *member = component->Children;
    while (member)
    {
        int pointerCount = member->IndirectionLevel;
        uint32_t arrayCount = member->ArrayCount;

        fprintf(FunctionsFile,
                ", %s ", member->Type);
        fprintf(FunctionsHeaderFile,
                ", %s ", member->Type);

        int i;
        for (i = 0; i < pointerCount; ++i)
        {
            fprintf(FunctionsFile, "*");
            fprintf(FunctionsHeaderFile, "*");
        }

        fWriteStringAsLowercaseWithUnderscores(FunctionsFile,
                                               member->Name);
        fWriteStringAsLowercaseWithUnderscores(FunctionsHeaderFile,
                                               member->Name);

        if (arrayCount > 1)
        {
            fprintf(FunctionsFile,
                    "[%u]",
                    arrayCount);

            fprintf(FunctionsHeaderFile,
                    "[%u]",
                    arrayCount);
        }

        member = member->Next;
    }

    fprintf(FunctionsHeaderFile, ");");

    fprintf(FunctionsFile,
            ")\n{\n    scene->EntitySignatures[entity] |= ");
    fWriteStringAsUppercaseWithUnderscores(FunctionsFile,
                                           component->Name);
    fprintf(FunctionsFile, ";\n");

    member = component->Children;
    while (member)
    {
        uint32_t arrayCount = member->ArrayCount;

        if (arrayCount > 1)
        {
            int i;
            for (i = 0; i < arrayCount; ++i)
            {
                fprintf(FunctionsFile,
                        "    scene->");
                fWriteStringAsTitleCase(FunctionsFile,
                                        component->Name);
                fprintf(FunctionsFile,
                        "[entity].%s[%d] = ", member->Name, i);
                fWriteStringAsLowercaseWithUnderscores(FunctionsFile,
                                                       member->Name);
                fprintf(FunctionsFile, "[%d];\n", i);
            }
        }
        else
        {
            fprintf(FunctionsFile,
                    "    scene->");
            fWriteStringAsTitleCase(FunctionsFile,
                                    component->Name);

            fprintf(FunctionsFile,
                    "[entity].%s = ", member->Name);
            fWriteStringAsLowercaseWithUnderscores(FunctionsFile,
                                                   member->Name);
            fprintf(FunctionsFile, ";\n");
        }

        member = member->Next;
    }

    fprintf(FunctionsFile, "}\n\n");
}

void lcddlUserInitCallback(void)
{
    fprintf(stderr,
            "\n    \033[34mInitialising LCDDL...\033[0m\n\n");

    ComponentsFile = fopen(
            "Engine/Source/ECS/Components.gen.h",
            "w");
    FunctionsFile = fopen(
        "Engine/Source/ECS/EcsFunctions.gen.c",
        "w");
    FunctionsHeaderFile = fopen(
        "Engine/Source/ECS/EcsFunctions.gen.h",
        "w");
    ComponentArraysFile = fopen(
        "Engine/Source/ECS/ComponentArrays.gen.h",
        "w");

    if (!ComponentArraysFile ||
        !FunctionsFile       ||
        !ComponentArraysFile)
    {
        fprintf(stderr, "\033[31mError opening output files!\033[0m");
        exit(-1);
    }

    fprintf(ComponentsFile,
            "#ifndef COMPONENTS_H\n#define COMPONENTS_H\n\n");
    fprintf(ComponentsFile,
            "#define LC_COMPONENT_NONE 0\n\n");
}

void lcddlUserTopLevelCallback(lcddlNode_t *node)
{
    if (node->Children &&
        lcddl_NodeHasTag(node, "Component"))
    {
        fprintf(stderr,
                "    \033[34mLCDDL parse function for %s\033[0m\n", node->Name);
        GenerateComponent(node);
    }
}

void lcddlUserCleanupCallback(void)
{
    fprintf(stderr, "    \033[34mFinishing up...\n\n");

    fprintf(ComponentsFile, "#endif\n");
    fclose(ComponentsFile);
    fclose(FunctionsFile);
    fclose(ComponentArraysFile);
}
