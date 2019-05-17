group "Examples"
  project "Simple Window"
    kind "ConsoleApp"
    
    files
    {
      "Phantom/examples/window/**.*"
    }
    
    links
    {
      "Phantom"
    }
    
    location (_ACTION .. "/window")
  
  project "Hello triangle"
    kind "ConsoleApp"
    
    includedirs
    {
      "Phantom/examples/hello_triangle"
    }
    files
    {
     "Phantom/examples/hello_triangle/**.*"
    }
    
    links
    {
      "GLFW32",
      "GLM",
    }
    
    location (_ACTION .. "/hello_triangle")
  
  
  project "Hello triangle 2.0"
    kind "ConsoleApp"
    
    includedirs
    {
      "Phantom/examples/hello_triangle_2_0"
    }
    files
    {
     "Phantom/examples/hello_triangle_2_0/**.*"
    }
    
    links
    {
      "Phantom"
    }
    
    location (_ACTION .. "/hello_triangle_2_0")