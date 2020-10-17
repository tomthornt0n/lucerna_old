/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 17 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define LC_NO_ENTITY 0xffffffffffffffff

lcScene_t *
lcSceneCreate(void)
{
    lcScene_t *result = malloc(sizeof(lcScene_t));

    lcEntity_t i;
    for (i = 0; i < LC_MAX_ENTITIES; ++i)
        result->EntitySignatures[i] = LC_NO_ENTITY;

    result->RenderableCount = 0;

    return result;
}

lcSubset_t 
lcSubsetCreate(void)
{
    lcSubset_t subset;

    subset.Signature = LC_COMPONENT_NONE;
    subset.Entities = NULL;
    LC_LIST_CREATE(subset.Entities, lcEntity_t);

    return subset;
}

void
lcSubsetDestroy(lcSubset_t subset)
{
    LC_LIST_DESTROY(subset.Entities);
}

void
lcSubsetSetSignature(lcSubset_t *subset,
                     lcSignature_t components)
{
    subset->Signature = components;
}

void
lcSubsetRefresh(lcScene_t *scene,
                lcSubset_t *subset)
{
    LC_LIST_CLEAR(subset->Entities);

    lcEntity_t i;
    for (i = 0; i < LC_MAX_ENTITIES; ++i)
    {
        if (((subset->Signature & scene->EntitySignatures[i])
                    == subset->Signature)
              && scene->EntitySignatures[i]
                    != LC_NO_ENTITY
            )
        {
            LC_LIST_PUSH_BACK(subset->Entities, lcEntity_t, &i);
        }
    }
}

lcEntity_t
lcEntityCreate(lcScene_t *scene)
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
    return 0;
}

void
lcEntityDestroy(lcScene_t *scene,
                lcEntity_t entity)
{
    if ((scene->EntitySignatures[entity] & COMPONENT_RENDERABLE) ==
        COMPONENT_RENDERABLE)
    {
       memcpy(scene->ComponentRenderable + scene->EntityToRenderable[entity],
              scene->ComponentRenderable + (scene->RenderableCount - 1),
              sizeof(ComponentRenderable)); 
       --scene->RenderableCount;
    }
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

