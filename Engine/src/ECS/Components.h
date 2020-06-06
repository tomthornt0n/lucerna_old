#pragma once

#include <stdint.h>
#include "../../vendor/cglm/cglm/struct.h"
#define LC_COMPONENT_NONE 0

#define LC__TRANSFORM BIT(0)
typedef struct lc_Transform lc_Transform;
struct lc_Transform
{
float XPos;
float YPos;
float XScale;
float YScale;
float Rotation;
};

#define LC__COLOUR BIT(1)
typedef struct lc_Colour lc_Colour;
struct lc_Colour
{
float Red;
float Green;
float Blue;
float Alpha;
};

#define LC__TEXTURE BIT(2)
typedef struct lc_Texture lc_Texture;
struct lc_Texture
{
uint32_t TextureID;
};

#define LC__SUB_SPRITE BIT(3)
typedef struct lc_SubSprite lc_SubSprite;
struct lc_SubSprite
{
float XSize;
float YSize;
float XOffset;
float YOffset;
};

#define LC__NAME BIT(4)
typedef struct lc_Name lc_Name;
struct lc_Name
{
char *Name;
};

#define HEALTH BIT(5)
// @NoPrefix 
typedef struct Health Health;
struct Health
{
int Health;
int MaxHealth;
};

