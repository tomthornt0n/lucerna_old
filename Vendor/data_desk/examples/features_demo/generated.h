#define SOME_CONSTANT (16)
// @TagB @TagA 
typedef struct MyStructA MyStructA;
struct MyStructA
{
int a;
float b;
char c;
};

// @TagWithParameters(123, 456, 789) 
typedef struct MyStructB MyStructB;
struct MyStructB
{
// @TagOnMember 
float x;
// @TagWithParameters(123, 456) 
float y;
struct
{
int foo[16];
void *bar;
// @Tag 
float baz[8][SOME_CONSTANT];
}
sub_struct;
};

// @SameAsStructs 
typedef union MyUnion MyUnion;
union MyUnion
{
MyStructA struct_a;
MyStructB struct_b;
};

// @Tag 
typedef enum MyEnum MyEnum;
enum MyEnum
{
ENUM_VALUE_1,
ENUM_VALUE_2,
ENUM_VALUE_3,
};

// @Tag 
#define FLAGS_VALUE_1 (1<<0)
#define FLAGS_VALUE_2 (1<<1)
#define FLAGS_VALUE_3 (1<<2)
typedef unsigned int MyFlags;

