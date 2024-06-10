#define RAYLIB_VERSION "5.0"
#define CONFIG_PATH "./build/config.h"

typedef struct {
    const char *macro;
    bool enabled_by_default;
} Target_Flag;

static Target_Flag target_flags[] = {
    {
        .macro = "MUSIALIZER_TARGET_LINUX",
        #if defined(linux) || defined(__linux) || defined(__linux__)
            .enabled_by_default = true,
        #else
            .enabled_by_default = false,
        #endif
    },
    {
        .macro = "MUSIALIZER_TARGET_WIN64_MINGW",
        #if (defined(WIN32) || defined(_WIN32)) && defined(__MINGW32__)
            .enabled_by_default = true,
        #else
            .enabled_by_default = false,
        #endif
    },
    {
        .macro = "MUSIALIZER_TARGET_WIN64_MSVC",
        #if (defined(WIN32) || defined(_WIN32)) && defined(_MSC_VER)
            .enabled_by_default = true,
        #else
            .enabled_by_default = false,
        #endif
    },
    {
        .macro = "MUSIALIZER_TARGET_MACOS",
        #if defined(__APPLE__) || defined(__MACH__)
            .enabled_by_default = true,
        #else
            .enabled_by_default = false,
        #endif
    },
    {
        .macro = "MUSIALIZER_TARGET_OPENBSD",
        #if defined(__OpenBSD__)
            .enabled_by_default = true,
        #else
            .enabled_by_default = false,
        #endif
    },
};

typedef struct {
    const char *display;
    const char *macro;
    const char *description;
} Feature_Flag;

static Feature_Flag feature_flags[] = {
    {
        .display = "Hotreload",
        .macro = "MUSIALIZER_HOTRELOAD",
        .description = "Moves everything in src/plug.c to a separate \"DLL\" so it can be hotreloaded.",
    },
    {
        .display = "Unbundle",
        .macro = "MUSIALIZER_UNBUNDLE",
        .description = "Don't bundle resources/ folder with the executable and load the resources directly from the folder.",
    },
    {
        .display = "Microphone",
        .macro = "MUSIALIZER_MICROPHONE",
        .description = "Unfinished feature that enables capturing sound from the mic."
    },
    {
        .display = "Act on Press",
        .macro = "MUSIALIZER_ACT_ON_PRESS",
        .description = "Activate UI buttons on Press instead of Release just as John Carmack explained https://twitter.com/ID_AA_Carmack/status/1787850053912064005"
    },
};

void generate_default_config(Nob_String_Builder *content)
{
    nob_sb_append_cstr(content, "//// Build target. Pick only one!\n");
    for (size_t i = 0; i < NOB_ARRAY_LEN(target_flags); ++i) {
        if (target_flags[i].enabled_by_default) {
            nob_sb_append_cstr(content, "#define ");
        } else {
            nob_sb_append_cstr(content, "// #define ");
        }
        nob_sb_append_cstr(content, target_flags[i].macro);
        nob_sb_append_cstr(content, "\n");
    }

    nob_sb_append_cstr(content, "\n");

    for (size_t i = 0; i < NOB_ARRAY_LEN(feature_flags); ++i) {
        nob_sb_append_cstr(content, "//// ");
        nob_sb_append_cstr(content, feature_flags[i].description);
        nob_sb_append_cstr(content, "\n");
        if (strcmp(feature_flags[i].macro, "MUSIALIZER_HOTRELOAD") == 0) {
            // TODO: FIX ASAP! This requires bootstrapping nob with additional flags which goes against its philosophy!
            #ifdef MUSIALIZER_HOTRELOAD
                nob_sb_append_cstr(content, "#define ");
            #else
                nob_sb_append_cstr(content, "// #define ");
            #endif
        } else {
            nob_sb_append_cstr(content, "// #define ");
        }
        nob_sb_append_cstr(content, feature_flags[i].macro);
        nob_sb_append_cstr(content, "\n");
        nob_sb_append_cstr(content, "\n");
    }
}

void generate_config_logger(Nob_String_Builder *content)
{
    nob_sb_append_cstr(content, nob_temp_sprintf("// DO NOT MODIFY THIS FILE. It's auto-generated by %s and it's needed for logging the current configuration.\n", __FILE__));
    nob_sb_append_cstr(content, "void log_config(Nob_Log_Level level)\n");
    nob_sb_append_cstr(content, "{\n");
    nob_sb_append_cstr(content, "    nob_log(level, \"Target: %s\", MUSIALIZER_TARGET_NAME);\n");
    for (size_t i = 0; i < NOB_ARRAY_LEN(feature_flags); ++i) {
        nob_sb_append_cstr(content, nob_temp_sprintf("    #ifdef %s\n", feature_flags[i].macro));
        nob_sb_append_cstr(content, nob_temp_sprintf("        nob_log(level, \"%s: ENABLED\");\n", feature_flags[i].display));
        nob_sb_append_cstr(content, "    #else\n");
        nob_sb_append_cstr(content, nob_temp_sprintf("        nob_log(level, \"%s: DISABLED\");\n", feature_flags[i].display));
        nob_sb_append_cstr(content, "    #endif\n");
    }
    nob_sb_append_cstr(content, "}\n");
}