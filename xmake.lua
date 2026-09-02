set_project("pacman")
set_version("1.0.0")
set_languages("cxx17")

if not is_plat("wasm") then
    add_requires("sfml")
end

target("pacman")
    set_kind("binary")
    add_files("src/*.cpp")
    add_includedirs("include")

    if is_plat("wasm") then
        add_includedirs("third_party/vrsfml/include")
        add_linkdirs("third_party/vrsfml/lib")
        add_links("sfml-graphics", "sfml-window", "sfml-system")
        add_ldflags("--preload-file assets@/assets", {force = true})
        add_ldflags("-sALLOW_MEMORY_GROWTH=1", {force = true})
        add_ldflags("-sUSE_WEBGL2=1", {force = true})
        set_extension(".html")
    else
        add_packages("sfml")
        after_build(function (target)
            os.cp("$(projectdir)/assets", target:targetdir())
        end)
    end
target_end()