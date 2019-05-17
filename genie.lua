solution "Phantom"

  language "C++"
  startproject "Phantom"
  --targetdir "build"
  
  newoption {
    trigger     = "gfxapi",
    value       = "API",
    description = "Choose a particular 3D API for rendering",
    allowed     = {
        { "opengl" , "OpenGL" },
        { "directx", "Direct3D (Windows only)"},
        { "vulkan" , "Vulkan" }
    }
  }

  newoption {
    trigger     = "pfv",
    value       = "#",
    description = "Choose a particular platform version",
  }
  
  newoption {
    trigger = "with-examples",
    description = "Enable building examples.",
  }
  
  newoption {
    trigger = "with-linux",
    description = "Enable linux building.",
  }

 -- windowstargetplatformversion = string.gsub(os.getenv("WindowsSDKVersion") or "8.1", "\\", "")
	if _OPTIONS['pfv'] then
		windowstargetplatformversion(_OPTIONS['pfv'])
	end

  
  
  configurations {
  	"Debug",
  	"Release",
  }
  
  platforms {
  	"x32",
  	"x64",
  }

	--Aqui todos los includes
	includedirs 
  {
		"external/glfw/include",
    "external/glm/",
    "external/tinygltf",
    "Phantom/include"
    
	}
  
  flags
  {
    "Cpp17"
  }
  
  
  
  if _OPTIONS['gfxapi'] == "vulkan" then
  
    
    if _OPTIONS['with-linux'] then
    
      includedirs
      {
        "external/VulkanSDK/linux/1.1.97.0/x86_64/include"
      }
      links
      {
        "vulkan",
      }

     

      configuration {"x32", "Release"}
        
      configuration {"x64", "Release"} 
  
      configuration {"x32" or "x64",  "Debug"}
        libdirs
        {
          "external/VulkanSDK/linux/1.1.97.0/x86_64/lib"
        }
      
      
    
    else
      includedirs
      {
        "external/VulkanSDK/windows/1.1.97.0/Include"
      }
      
      links
      {
        "vulkan-1"
      }
      configuration {"x32", "Release"}
        
      configuration {"x64", "Release"} 
  
      configuration {"vs*", "x32", "Debug"}
        libdirs
        {
          "external/VulkanSDK/windows/1.1.97.0/Lib32"
        }
        
      configuration {"vs*", "x64", "Debug"}
        
        libdirs
        {
          "external/VulkanSDK/windows/1.1.97.0/Lib"
        }
      
    end
  end
  
  
  
 configuration {"x32", "Release"}
  targetdir "build/x32/Release"
  debugdir "build/x32/Release"
  objdir "$(SolutionDir)/build/obj"

  postbuildcommands { "xcopy /e /s /a /i /c /q /d /Y $(SolutionDir)bin $(SolutionDir)build\\x32\\Release" }
  
 configuration {"x32", "Debug"}
  targetdir "build/x32/Debug"
  debugdir "build/x32/Debug"
  objdir "$(SolutionDir)/build/obj"
  
  postbuildcommands { "xcopy /e /s /a /i /c /q /d /Y $(SolutionDir)bin $(SolutionDir)build\\x32\\Debug" }
  
   
   
 configuration "x32"
  defines
  {
    "_GLFW_WIN32"
  }
  
 configuration "x64, Release"
  targetdir "build/x64/Release"
  debugdir "build/x64/Release"
  objdir "$(SolutionDir)/build/obj"

  postbuildcommands { "xcopy /e /s /a /i /c /q /d /Y $(SolutionDir)bin $(SolutionDir)build\\x64\\Release" }
  
 configuration "x64, Debug"
  targetdir "build/x64/Debug"
  debugdir "build/x64/Debug"
  objdir "$(SolutionDir)/build/obj"
  
  postbuildcommands { "xcopy /e /s /a /i /c /q /d /Y $(SolutionDir)bin $(SolutionDir)build\\x64\\Debug" }
  
  
 configuration "Debug"
  flags 
  { 
   "Symbols" 
  }
  
  defines 
  { 
    "_DEBUG"
  }
  
  
configuration "Release"
	defines 
  { 
    "NDEBUG" 
  }
  
  flags 
  { 
   "OptimizeSize" 
  }
  
  
  
--PROJECTS
 project "Phantom"
  kind "StaticLib"
  
  includedirs
  {
    "Phantom/include"
  }
  
  files
  {
    "bin/**.*",
    "Phantom/src/**.*",
    "Phantom/include/**.*",
    "Phantom/assets/**.*",
    "external/tinygltf/*.c",
    "external/tinygltf/*.hpp",
    "external/tinygltf/*.h",
    "*.sh",
    "*.lua",
    "*.txt",
    "*.md",
    "*.bat"
  }

  excludes
  {
    "Phantom/src/vk/**.*",
    "Phantom/src/gl/**.*"
  }

  links
  {
    "GLFW32",
    "GLM"
  }
  
  if _OPTIONS['gfxapi'] == "vulkan" then

    defines
    {
     "VULKAN_API"
    }

  end

  location (_ACTION .. "/Phantom")
  

group "Libraries"
 project "GLFW32"
  kind "StaticLib"
  
  includedirs
  {
    "external/glfw/win32_platform.h",
    "external/glfw/win32_joystick.h",
    "external/glfw/wgl_context.h",
    "external/glfw/egl_context.h",
    "external/glfw/osmesa_context.h",
    "external/glfw/src/internal.h",
    "external/glfw/src/mappings.h",
    "external/glfw/src/glfw_config.h",
    "external/glfw/include"
  }

  files
  {
    "external/glfw/src/context.c",
    "external/glfw/src/init.c",
    "external/glfw/src/input.c",
    "external/glfw/src/monitor.c",
    "external/glfw/src/vulkan.c",
    "external/glfw/src/window.c",
    "external/glfw/src/win32_platform.h",
    "external/glfw/src/win32_joystick.h",
    "external/glfw/src/wgl_context.h",
    "external/glfw/src/egl_context.h",
    "external/glfw/src/osmesa_context.h",
    "external/glfw/src/win32_init.c",
    "external/glfw/src/win32_joystick.c",
    "external/glfw/src/win32_monitor.c",
    "external/glfw/src/win32_time.c",
    "external/glfw/src/win32_thread.c",
    "external/glfw/src/win32_window.c",
    "external/glfw/src/wgl_context.c",
    "external/glfw/src/egl_context.c",
    "external/glfw/src/osmesa_context.c"
  }
  
  location (_ACTION .. "/GLFW")
  
 project "GLM"
  kind "StaticLib"

  includedirs
  {
   "external/glm/glm"
  }

  files
  {
   "external/glm/glm/**.*"
  }
  
  location (_ACTION .. "/glm")



 project "TinyGLTF"
  kind "StaticLib"
  
  includedirs
  {
    "external/tinygltf"
  }
  
  files
  {
    "external/tinygltf/*.c",
    "external/tinygltf/*.hpp",
    "external/tinygltf/*.h"
  }
  
  location (_ACTION .. "/tinygltf")

dofile("examples.lua")
 

 
  
  
  
  
  
  