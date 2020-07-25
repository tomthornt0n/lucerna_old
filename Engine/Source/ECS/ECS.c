#ifndef LC_MAX_ENTITIES
#define LC_MAX_ENTITIES 1000
#endif

#define LC_NO_ENTITY 0xffffffffffffffff

#define LC_SUBSET_LOOP(subset)                                                 \
int i;                                                                         \
for(i = 0; i < LC_LIST_LEN(subset->Entities); i++)                             \
{                                                                              \
    lcEntity_t entity = subset->Entities[i];                                   \
    lcScene_t *scene = subset->Parent;

#define LC_END_SUBSET_LOOP }

typedef uint64_t lcSignature_t;
typedef uint32_t lcEntity_t;

typedef struct
{
    lcSignature_t EntitySignatures[LC_MAX_ENTITIES];

#include "ComponentArrays.gen.c"

    lc_Renderable LcRenderable[LC_MAX_ENTITIES];
} lcScene_t;

typedef struct
{
    lcSignature_t Signature;
    lcEntity_t *Entities;
    lcScene_t *Parent;
} lcSubset_t;


lcScene_t
lc_SceneCreate(void)
{
    lcScene_t scene;

    lcEntity_t i;
    for (i = 0; i < LC_MAX_ENTITIES; ++i)
        scene.EntitySignatures[i] = LC_NO_ENTITY;

#include "ComponentArraysAllocation.gen.c"

    return scene;
}

#include "lc_SceneDestroy.gen.c"

lcSubset_t *
lc_SubsetCreate(lcScene_t *scene)
{
    lcSubset_t *subset = malloc(sizeof(lcSubset_t));

    subset->Signature = LC_COMPONENT_NONE;
    subset->Entities = NULL;
    LC_LIST_CREATE(subset->Entities, lcEntity_t);
    subset->Parent = scene;

    return subset;
}

void
lc_SubsetDestroy(lcSubset_t *subset)
{
    LC_LIST_DESTROY(subset->Entities);
    free(subset);
}

void
lc_SubsetSetSignature(lcSubset_t *subset,
                      lcSignature_t components)
{
    subset->Signature = components;
}

void
lc_SubsetRefresh(lcSubset_t *subset)
{
    LC_LIST_DESTROY(subset->Entities);
    subset->Entities = NULL;
    LC_LIST_CREATE(subset->Entities, lcEntity_t);
    lcEntity_t i;
    for (i = 0; i < LC_MAX_ENTITIES; ++i)
    {
        if (
                ((subset->Signature & subset->Parent->EntitySignatures[i])
                    == subset->Signature) &&
                subset->Parent->EntitySignatures[i]
                    != LC_NO_ENTITY
            )
        {
            LC_LIST_PUSH_BACK(subset->Entities, lcEntity_t, &i);
        }
    }
}

lcEntity_t
lc_EntityCreate(lcScene_t *scene)
{
    lcEntity_t entity;
    for (entity = 0; entity < LC_MAX_ENTITIES; entity++)
    {
        if (scene->EntitySignatures[entity] == LC_NO_ENTITY)
        {
            scene->EntitySignatures[entity] = LC_COMPONENT_NONE;
            return entity; 
        }
    }

    LC_ASSERT(0, "Too many entities in scene!");
}

void
lc_EntityDestroy(lcScene_t *scene,
                 lcEntity_t entity)
{
    scene->EntitySignatures[entity] = LC_COMPONENT_NONE;
}


/*
MIT License

Copyright (c) 2020 Tom Thornton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

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

