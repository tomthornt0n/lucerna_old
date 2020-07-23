#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lcddl.h"

FILE *ComponentsFile = NULL;
FILE *FunctionsFile = NULL;
FILE *ComponentArraysFile = NULL;
FILE *AllocationsFile = NULL;
FILE *DeallocationsFile = NULL;

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
            if (string[i - 1] == '_')
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
    fprintf(ComponentsFile, "#define LC_COMPONENT_");
    fWriteStringAsUppercaseWithUnderscores(ComponentsFile, component->Name);
    fprintf(ComponentsFile, " BIT(%i)\n", bitOffset++);
    if(bitOffset == 0xffffffffffffffff)
    {
        fprintf(stderr, "\033[31mERROR: Too many components!\033[0m\n");
        exit(-1);
    }
    lcddl_WriteStructToFileAsC(ComponentsFile, component);

    fprintf(stderr, "        Creating component array...\n");
    fprintf(ComponentArraysFile, "    %s *", component->Name);
    fWriteStringAsTitleCase(ComponentArraysFile, component->Name);
    fprintf(ComponentArraysFile, ";\n");
    fprintf(AllocationsFile, "scene.");
    fWriteStringAsTitleCase(AllocationsFile, component->Name);
    fprintf(AllocationsFile,
            " = malloc(LC_MAX_ENTITIES * sizeof(%s));\n",
            component->Name);
    fprintf(DeallocationsFile,
            "    free(scene->");
    fWriteStringAsTitleCase(DeallocationsFile, component->Name);
    fprintf(DeallocationsFile, ");\n");

    fprintf(stderr, "        Generating add function...\n\n");

    fprintf(FunctionsFile, "void lc_Add");
    fWriteStringAsTitleCase(FunctionsFile, component->Name);
    fprintf(FunctionsFile, "(lcScene_t* scene, lcEntity_t entity");
    lcddlNode_t *member = component->Children;
    while (member)
    {
        int pointerCount = member->IndirectionLevel;
        fprintf(FunctionsFile,
                ", %s ", member->Type);
        int i;
        for (i = 0; i < pointerCount; ++i)
            fprintf(FunctionsFile, "*");
        fWriteStringAsLowercaseWithUnderscores(FunctionsFile, member->Name);
        member = member->Next;
    }
    fprintf(FunctionsFile,
            ")\n{\n    scene->EntitySignatures[entity] |= LC_COMPONENT_");
    fWriteStringAsUppercaseWithUnderscores(FunctionsFile,
                                           component->Name);
    fprintf(FunctionsFile, ";\n");

    member = component->Children;
    while (member)
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
        member = member->Next;
    }
    fprintf(FunctionsFile, "}\n\n");
}

void lcddlUserInitCallback(void)
{
    fprintf(stderr,
            "\n    \033[34mInitialising LCDDL...\033[0m\n\n");

    ComponentsFile = fopen(
            "Engine/Source/ECS/Components.gen.c",
            "w");
    FunctionsFile = fopen(
        "Engine/Source/ECS/EcsFunctions.gen.c",
        "w");
    ComponentArraysFile = fopen(
        "Engine/Source/ECS/ComponentArrays.gen.c",
        "w");
    AllocationsFile = fopen(
        "Engine/Source/ECS/ComponentArraysAllocation.gen.c",
        "w");
    DeallocationsFile = fopen(
        "Engine/Source/ECS/lc_SceneDestroy.gen.c",
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

    fprintf(DeallocationsFile,
            "void lc_SceneDestroy(lcScene_t *scene)\n{\n");
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
    fprintf(DeallocationsFile, "}\n");

    fclose(ComponentsFile);
    fclose(FunctionsFile);
    fclose(ComponentArraysFile);
    fclose(AllocationsFile);
    fclose(DeallocationsFile);
}
