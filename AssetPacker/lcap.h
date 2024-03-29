#include <stdint.h>

#define LCAP_ASSET_TYPE_TEXTURE_ATLAS 0
#define LCAP_ASSET_TYPE_SPRITE        1
#define LCAP_ASSET_TYPE_SHADER        2
#define LCAP_ASSET_TYPE_SOUND         3

#define LCAP_NAME_MAX_LEN 64

#pragma pack(push, 1)

typedef struct
{
    uint8_t Type;
    uint64_t Size;
} lcapChunkHeader_t;

typedef struct
{
    char Name[LCAP_NAME_MAX_LEN];
    float Min[2];
    float Max[2];
} lcapSprite_t;

typedef struct
{
    char Name[LCAP_NAME_MAX_LEN];
    uint64_t VertexLength;
    uint64_t FragmentLength;
} lcapShader_t;

typedef struct
{
    char Name[LCAP_NAME_MAX_LEN];
} lcapSound_t;

#pragma pack(pop)

