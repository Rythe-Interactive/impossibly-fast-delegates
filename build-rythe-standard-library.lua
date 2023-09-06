createProject ("include","rsl","StaticLib")
includedirs {
    "$(SolutionDir)include/rythe-standard-library/src/",
    "$(SolutionDir)include/rythe-standard-library/third_party/",
    "src/",
    "third_party/",
    "third_party/*/",
    "third_party/*/src",
    "third_party/*/source",
    "third_party/*/include"
}

files { 
    "src/**.h",
    "src/**.hpp",
    "src/**.inl",
    "src/**.c",
    "src/**.cpp"
}

targetdir "$(SolutionDir)bin\\lib\\"
libdirs { 
    "$(SolutionDir)bin\\lib\\"
 }


