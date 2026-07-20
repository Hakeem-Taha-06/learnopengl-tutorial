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
        "%{prj.name}/include/**.h",
        "%{wks.location}/%{prj.name}/dependencies/src/**.c",
        "%{wks.location}/%{prj.name}/dependencies/src/**.cpp"
    }

    includedirs
    {
        "%{wks.location}/%{prj.name}/dependencies/include",
        "%{wks.location}/%{prj.name}/dependencies/include/imgui", --for imgui, i don't want to edit their include paths
        "%{prj.name}/src",
        "%{prj.name}/include"
    }

    libdirs
    {
        "%{wks.location}/%{prj.name}/dependencies/libs"
    }
    
    links 
    {
        "glfw3",
        "assimp-vc143-mtd",
        "opengl32.lib"
    }

    postbuildcommands
    {
        ('{COPY} "%{wks.location}/%{prj.name}/src/shaders" "%{cfg.targetdir}/shaders/"'),--copies the shaders to be in the same directory as the exe   
        ('{COPY} "%{wks.location}/%{prj.name}/dependencies/libs/assimp-vc143-mtd.dll" "%{cfg.targetdir}"')
    }

    cppdialect "C++17"
    staticruntime "Off"
    systemversion "latest"

    filter "configurations:Debug"
        defines "DEBUG"
        symbols "on"
        linkoptions { "/IGNORE:4099" }

    filter "configurations:Release"
        defines "RELEASE"
        symbols "off"
        optimize "on"
