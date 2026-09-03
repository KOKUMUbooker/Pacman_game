set_project("pacman")
set_version("1.0.0")
set_languages("cxx23")

target("pacman")
    set_kind("binary")
    add_files("src/*.cpp")
    add_includedirs("include")
    add_includedirs("third_party/vrsfml/include")

    if is_plat("wasm") then
        add_linkdirs("third_party/vrsfml/build-wasm/lib")
        add_links("sfml-graphics", "sfml-window", "sfml-system")
        add_ldflags("--preload-file assets@/assets", {force = true})
        add_ldflags("-sALLOW_MEMORY_GROWTH=1", {force = true})
        add_ldflags("-sUSE_WEBGL2=1", {force = true})
        set_extension(".html")
    else
        add_linkdirs("third_party/vrsfml/build-native/lib")
        add_linkdirs("third_party/vrsfml/build-native/extlibs/libbacktrace")

        add_links(
            "sfml-imgui-s",
            "imgui",
            "sfml-graphics-s",
            "sfml-window-s",
            "sfml-system-s",
            "sfml-audio-s",
            "sfml-network-s",
            "sfml-glutils-s",
            "freetype",
            "vorbisfile",
            "vorbisenc",
            "vorbis",
            "ogg",
            "FLAC",
            "mbedtls",
            "mbedx509",
            "mbedcrypto",
            "SDL3",
            "sfml-libbacktrace"
        )
        after_build(function (target)
            os.cp("$(projectdir)/assets", target:targetdir())
        end)
    end
target_end()