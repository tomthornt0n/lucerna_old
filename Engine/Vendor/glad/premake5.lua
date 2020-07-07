project "glad"
    kind      "staticLib"
    language  "C"

    targetdir ("bin/" .. outputdir)
    objdir    ("bin-int/" .. outputdir)
    
    staticruntime "On"

    files
    {
        "src/glad.c",
        "include/glad/glad.h",
        "include/KHR/khrplatform.h"
    }

    includedirs { "include" }
    
    filter "system:windows"
        systemversion "latest"
