workspace "LearnOpenGl"
    configurations{
        "Debug", 
        "Release"
    }
    architecture "x64"


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "LearnOpenGL"
    location "LearnOpenGL"
    language "C++"
    kind "ConsoleApp"

    targetdir ("bin/"..outputdir.."/%{prj.name}")
    objdir ("bin-int/"..outputdir.."/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.c",
        "%{prj.name}/include/**.h"
    }

    includedirs
    {
        "%{wks.location}/LearnOpenGL/dependencies/include",
        "%{prj.name}/src"
    }

    libdirs
    {
        "%{wks.location}/LearnOpenGL/dependencies/libs"
    }
    
    links 
    {
        "glfw3",
        "opengl32.lib"
    }

    cppdialect "C++17"
    staticruntime "Off"
    systemversion "latest"

    filter "configurations:Debug"
        defines "DEBUG"
        symbols "off"
        linkoptions {"/NODEFAULTLIB:MSVCRTD"}

    filter "configurations:Release"
        defines "RELEASE"
        symbols "off"
        optimize "on"
