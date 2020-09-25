#include <stdint.h>

#define LCAP_ASSET_TYPE_TEXTURE_ATLAS 0
#define LCAP_ASSET_TYPE_SPRITE 1

#define LCAP_NAME_MAX_LEN 64

#pragma pack(push, 1)

typedef struct
{
    uint8_t Type;
    uint32_t Width;
} lcapTextureAtlas_t;

typedef struct
{
    uint8_t Type;
    char Name[LCAP_NAME_MAX_LEN];
    float Min[2];
    float Max[2];
} lcapSprite_t;

#pragma pack(pop)

