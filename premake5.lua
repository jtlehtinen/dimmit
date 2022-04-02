workspace "dimmit"
  location "build/"
  configurations {"Debug", "Release"}
  language "C"
  cdialect "C11"
  flags { "MultiProcessorCompile", "FatalWarnings" }

filter { "system:windows" }
  platforms { "Win64" }

filter { "platforms:Win64" }
defines { "WIN32", "_CRT_SECURE_NO_WARNINGS", "WIN32_LEAN_AND_MEAN", "NOMINMAX" }
architecture "x64"

filter "configurations:Debug"
  defines { "DIMMIT_DEBUG" }
  symbols "On"

filter "configurations:Release"
  optimize "On"

project "dimmit"
  location "build/"
  kind "WindowedApp"
  targetdir "bin/%{cfg.platform}/%{cfg.buildcfg}"
  files {"src/**.h", "src/**.c"}
  includedirs { "src" }
  editandcontinue "Off"
  flags { "NoIncrementalLink", "NoPCH", "StaticRuntime" }
