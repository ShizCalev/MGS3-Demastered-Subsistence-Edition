// ReSharper disable CppUseAuto
// ReSharper disable IdentifierTypo
#include "stdafx.h"
#include "verify_mod_installation.hpp"

#include "common.hpp"
#include "logging.hpp"
#include "helper.hpp"


namespace
{

    constexpr size_t ConstStrLen(const char* str)
    {
        size_t len = 0;

        while (str[len] != '\0')
        {
            ++len;
        }

        return len;
    }

    constexpr bool IsHex(char c)
    {
        return
            (c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'f') ||
            (c >= 'A' && c <= 'F');
    }

    constexpr bool IsValidSHA1(const char* str)
    {
        if (!str || ConstStrLen(str) != 40)
        {
            return false;
        }

        for (size_t i = 0; i < 40; ++i)
        {
            if (!IsHex(str[i]))
            {
                return false;
            }
        }

        return true;
    }



    constexpr const char* MGS3_Demaster_Base_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_SHA1 = "aea7b7311342eab1f4c78c43f393c0f03ef83b55";
    constexpr const char* MGS3_Demaster_2x_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_SHA1 = "2003659d9b9ac548a9c23ead82c8c944f1b0f1bd";
    constexpr const char* MGS3_Demaster_4x_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_SHA1 = "b00e97af4b2cb7268480b8d59521a29b08dd081c";
    
    constexpr const char* MGS3_Demaster_Base_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_SHA1 = "cfa3ad1c765602f7b0a976ecb6f116c6ce8292be";
    constexpr const char* MGS3_Demaster_2x_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_SHA1 = "bf611074f135f0c3fd3baa8f81fe2cd241791fe0";
    constexpr const char* MGS3_Demaster_4x_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_SHA1 = "c4c87a8bbe92d3a6c766b4f600d036f955975991";

    constexpr const char* MGS3_Demaster_Base_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_SHA1 = "cfa3ad1c765602f7b0a976ecb6f116c6ce8292be";
    constexpr const char* MGS3_Demaster_2x_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_SHA1 = "bf611074f135f0c3fd3baa8f81fe2cd241791fe0";
    constexpr const char* MGS3_Demaster_4x_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_SHA1 = "c4c87a8bbe92d3a6c766b4f600d036f955975991";



    static_assert(IsValidSHA1(MGS3_Demaster_Base_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_SHA1), "Invalid SHA1");
    static_assert(IsValidSHA1(MGS3_Demaster_2x_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_SHA1), "Invalid SHA1");
    static_assert(IsValidSHA1(MGS3_Demaster_4x_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_SHA1), "Invalid SHA1");
    static_assert(IsValidSHA1(MGS3_Demaster_Base_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_SHA1), "Invalid SHA1");
    static_assert(IsValidSHA1(MGS3_Demaster_2x_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_SHA1), "Invalid SHA1");
    static_assert(IsValidSHA1(MGS3_Demaster_4x_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_SHA1), "Invalid SHA1");
    static_assert(IsValidSHA1(MGS3_Demaster_Base_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_SHA1), "Invalid SHA1");
    static_assert(IsValidSHA1(MGS3_Demaster_2x_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_SHA1), "Invalid SHA1");
    static_assert(IsValidSHA1(MGS3_Demaster_4x_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_SHA1), "Invalid SHA1");

}



void VerifyInstallation::Check()
{
    if (!(eGameType & MGS3))
    {
        return;
    }

    spdlog::info("Starting installation verification checks...");
    struct FileHashResult
    {
        std::filesystem::path path;
        bool exists = false;
        std::optional<std::array<std::uint8_t, 20>> sha1;
    };


    const auto openPS2DemasterNexusPage =
        []()
        {
            if (Util::IsSteamOS())
            {
                spdlog::info("Opening webpages is not supported on SteamOS. Please visit the following URL on a different device to download the base package: https://www.nexusmods.com/metalgearsolid3mc/mods/190");
                return;
            }
            ShellExecuteA(
                nullptr,
                "open",
                "https://www.nexusmods.com/metalgearsolid3mc/mods/190",
                nullptr,
                nullptr,
                SW_SHOWNORMAL
            );
        };



    auto startHashTask =
        [](const std::filesystem::path& path) -> std::future<FileHashResult>
        {
            return std::async(
                std::launch::async,
                [path]() -> FileHashResult
                {
                    FileHashResult result;
                    result.path = path;
                    result.exists = std::filesystem::exists(path);

                    if (!result.exists)
                    {
                        return result;
                    }

                    result.sha1 = Util::ComputeSHA1Bytes(path);
                    return result;
                });
        };

    spdlog::info("Calculating file hashes for installation verification...");
    const std::filesystem::path MGS3_Demaster_Base_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_PATH =            sExePath / "textures" / "flatlist" / "ovr_stm" / "_win" / "sok_coat_lupe_himo.bmp.ctxr";
    const std::filesystem::path MGS3_Demaster_Base_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_PATH = sExePath / "textures" / "flatlist" / "ovr_stm" / "ovr_us" /"_win" / "sna_suit_tears_sub_ovl_alp.bmp.ctxr";
    const std::filesystem::path MGS3_Demaster_Base_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_PATH = sExePath / "textures" / "flatlist" / "ovr_stm" / "ovr_jp" / "_win" / "sna_suit_tears_sub_ovl_alp.bmp.ctxr";



    const auto hashEquals =
        [](const FileHashResult& result, const char* expected) -> bool
        {
            return result.exists && result.sha1.has_value() && Util::SHA1Equals(*result.sha1, expected);
        };

    auto MGS3_Demaster_Base_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_FUTURE = startHashTask(MGS3_Demaster_Base_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_PATH);
    auto MGS3_Demaster_Base_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_FUTURE = startHashTask(MGS3_Demaster_Base_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_PATH);
    auto MGS3_Demaster_Base_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_FUTURE = startHashTask(MGS3_Demaster_Base_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_PATH);


    const FileHashResult MGS3_Demaster_Base_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_RESULT = MGS3_Demaster_Base_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_FUTURE.get();                           /// textures/flatlist/ovr_stm/_win/sok_coat_lupe_himo.bmp.ctxr
    const FileHashResult MGS3_Demaster_Base_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_RESULT = MGS3_Demaster_Base_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_FUTURE.get();     /// textures/flatlist/ovr_stm/ovr_us/_win/sna_suit_tears_sub_ovl_alp.bmp.ctxr
    const FileHashResult MGS3_Demaster_Base_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_RESULT = MGS3_Demaster_Base_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_FUTURE.get();     /// textures/flatlist/ovr_stm/ovr_jp/_win/sna_suit_tears_sub_ovl_alp.bmp.ctxr



    // ------------------------------------------------------
    // MGS3: Verify Afevis Bugfix Collection (base) installation
    // ------------------------------------------------------
    spdlog::info("Verifying MGS3 PS2 Demaster base installation...");
    if (!MGS3_Demaster_Base_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_RESULT.exists)
    {
        spdlog::warn("------------------- ! MGS3 PS2 Demaster (Base) - Missing Files ! -------------------");
        spdlog::warn("MGS3 PS2 Demaster installation issue detected, Demastered OVR_STM texture are missing!");
        spdlog::warn("This can occur if Steam has verified integrity and overwrote your mod files, or if your texture folder was partially deleted at some point.");
        spdlog::warn("Please reinstall the MGS3 PS2 Demaster Base mod to ensure proper game functionality.");
        spdlog::warn("Please visit our Nexus page at: https://www.nexusmods.com/metalgearsolid3mc/mods/190 to download the base package.");
        spdlog::warn("Or our GitHub releases page at: https://github.com/ShizCalev/MGS3-Demastered-Subsistence-Edition/releases");
        spdlog::warn("------------------- ! MGS3 PS2 Demaster (Base) - Missing Files ! -------------------");

        if (int result = MessageBoxA(
            nullptr,
            "MGS3 PS2 Demaster installation issue detected, Demastered OVR_STM texture files not found.\n"
            "\n"
            "This can occur if Steam has verified integrity and overwrote your mod files, or if your texture folder was partially deleted at some point.\n"
            "\n"
            "Please reinstall the MGS3 PS2 Demaster Base mod to ensure proper game functionality.\n"
            "\n"
            "Would you like to open the MGS3 PS2 Demastered Nexus download page now to download the base package?\n"
            "\n"
            "(GitHub releases link also available on the Nexus page.)",
            "MGS3 PS2 Demaster (Base) - Missing Files",
            MB_ICONWARNING | MB_YESNO);
        result == IDYES)
        {
            openPS2DemasterNexusPage();
        }

        return;
    }
    
    if (!(hashEquals(MGS3_Demaster_Base_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_RESULT, MGS3_Demaster_Base_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_SHA1)
              || hashEquals(MGS3_Demaster_Base_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_RESULT, MGS3_Demaster_2x_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_SHA1)
              || hashEquals(MGS3_Demaster_Base_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_RESULT, MGS3_Demaster_4x_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_SHA1)))
    {
        spdlog::warn("------------------- ! MGS3 PS2 Demaster (Base) - Mod Integrity Check Failed! -------------------");
        spdlog::warn("MGS3 PS2 Demaster installation issue detected, incorrect hash found for Demastered ovr_stm/_win texture files.");
        spdlog::warn("This can occur if Steam has verified integrity and overwrote your mod files, or if the MGS3 Community Bugfix Compilation mod was installed/loaded AFTER the PS2 Demaster mod.");
        spdlog::warn("Please reinstall the MGS3 PS2 Demaster Base mod to ensure proper game functionality.");
        spdlog::warn("Or if using a mod manager, ensure the MGS3 PS2 Demaster Base mod is loaded AFTER the Community Bugfix Compilation mod.");
        spdlog::warn("Please visit our Nexus page at: https://www.nexusmods.com/metalgearsolid3mc/mods/190 to download the base package.");
        spdlog::warn("Or our GitHub releases page at: https://github.com/ShizCalev/MGS3-Demastered-Subsistence-Edition/releases");
        spdlog::warn("------------------- ! MGS3 PS2 Demaster (Base) - Mod Integrity Check Failed ! -------------------");

        if (int result = MessageBoxA(
            nullptr,
            "MGS3 PS2 Demaster installation issue detected, incorrect hash found for Demastered ovr_stm/_win texture files.\n"
            "\n"
            "This can occur if Steam has verified integrity and damaged your mod files, or if the MGS3 Community Bugfix Compilation mod was installed/loaded AFTER the PS2 Demaster mod.\n"
            "\n"
            "Please reinstall the MGS3 PS2 Demaster Base mod to ensure proper game functionality.\n"
            "Or if using a mod manager, ensure the MGS3 PS2 Demaster Base mod is loaded AFTER the Community Bugfix Compilation mod.\n"
            "\n"
            "Would you like to open the MGS3 PS2 Demastered Nexus download page now to download the base package?\n"
            "\n"
            "(GitHub releases link also available on the Nexus page.)",
            "MGS3 PS2 Demaster (Base) - Mod Integrity Check Failed",
            MB_ICONWARNING | MB_YESNO);
        result == IDYES)
        {
            openPS2DemasterNexusPage();
        }

        return;
    }
    else
    {
        spdlog::info("Correct hash found for ovr_stm/_win/sok_coat_lupe_himo.bmp.ctxr, base installation is properly installed.");
    }


    spdlog::info("Checking for presence of OVR_JP demaster files...");
    // ------------------------------------------------------
    // MGS3: Verify Afevis Bugfix Collection (base - jp dlc)
    // ------------------------------------------------------
    if (!MGS3_Demaster_Base_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_RESULT.exists)
    {
        spdlog::info("MGS3 PS2 Demaster base installation JPN DLC not found, DLC was likely uninstalled after installation. Skipping hash check...");
    }
    if (!(hashEquals(MGS3_Demaster_Base_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_RESULT, MGS3_Demaster_Base_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_SHA1)
        || hashEquals(MGS3_Demaster_Base_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_RESULT, MGS3_Demaster_2x_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_SHA1)
        || hashEquals(MGS3_Demaster_Base_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_RESULT, MGS3_Demaster_4x_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_SHA1)))
        {
        spdlog::warn("------------------- ! MGS3 PS2 Demaster (Base - OVR_JP) - Mod Integrity Check Failed ! -------------------");
        spdlog::warn("MGS3 PS2 Demaster installation issue detected, base installation (OVR_JP) fixes are missing.");
        spdlog::warn("This can occur if Steam has verified integrity and overwrote your mod files, or if the MGS3 Community Bugfix Compilation mod was installed/loaded AFTER the PS2 Demaster mod.");
        spdlog::warn("Please reinstall the MGS3 PS2 Demaster Base mod to ensure proper game functionality.");
        spdlog::warn("Please visit our Nexus page at: https://www.nexusmods.com/metalgearsolid3mc/mods/190 to download the base package.");
        spdlog::warn("Or our GitHub releases page at: https://github.com/ShizCalev/MGS3-Demastered-Subsistence-Edition/releases");
        spdlog::warn("------------------- ! MGS3 PS2 Demaster (Base - OVR_JP) - Mod Integrity Check Failed ! -------------------");

        if (int result = MessageBoxA(
            nullptr,
            "MGS3 PS2 Demaster installation issue detected, base installation (OVR_JP) fixes are missing.\n"
            "\n"
            "This can occur if Steam has verified integrity and damaged your mod files, or if the MGS3 Community Bugfix Compilation mod was installed/loaded AFTER the PS2 Demaster mod.\n"
            "\n"
            "Please reinstall the MGS3 PS2 Demaster Base mod to ensure proper game functionality.\n"
            "Or if using a mod manager, ensure the MGS3 PS2 Demaster Base mod is loaded AFTER the Community Bugfix Compilation mod.\n"
            "\n"
            "Would you like to open the MGS3 PS2 Demastered Nexus download page now to download the base package?\n"
            "\n"
            "(GitHub releases link also available on the Nexus page.)",
            "MGS3 PS2 Demaster (Base - OVR_JP) - Mod Integrity Check Failed",
            MB_ICONWARNING | MB_YESNO);
        result == IDYES)
        {
            openPS2DemasterNexusPage();
        }

        return;
    }
    else
    {
        spdlog::info("Correct hash found for ovr_stm/ovr_jp/_win/sna_suit_tears_sub_ovl_alp.bmp.ctxr, base installation (OVR_JP) is properly installed.");
    }
    

    spdlog::info("Checking for presence of OVR_US demaster files...");
    // ------------------------------------------------------
    // MGS3: Verify Afevis Bugfix Collection (base - OVR_US)
    // ------------------------------------------------------
    if (!MGS3_Demaster_Base_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_RESULT.exists || 
            !(hashEquals(MGS3_Demaster_Base_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_RESULT, MGS3_Demaster_Base_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_SHA1)
               || hashEquals(MGS3_Demaster_Base_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_RESULT, MGS3_Demaster_2x_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_SHA1)
               || hashEquals(MGS3_Demaster_Base_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_RESULT, MGS3_Demaster_4x_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_SHA1)))
    {
        spdlog::warn("------------------- ! MGS3 PS2 Demaster (Base - OVR_US) - Mod Integrity Check Failed ! -------------------");
        spdlog::warn("MGS3 PS2 Demaster installation issue detected, base installation fixes are missing.");
        spdlog::warn("This can occur if Steam has verified integrity and damaged your mod files, or if you have reinstalled the Japanese Language DLC after installing the MGS3 PS2 Demaster.");
        spdlog::warn("Please reinstall the MGS3 PS2 Demaster -> Base <- package to ensure proper game functionality.");
        spdlog::warn("Please visit our Nexus page at: https://www.nexusmods.com/metalgearsolid3mc/mods/190 to download the base package.");
        spdlog::warn("Or our GitHub releases page at: https://github.com/ShizCalev/MGS3-Demastered-Subsistence-Edition/releases");
        spdlog::warn("------------------- ! MGS3 PS2 Demaster (Base - OVR_US) - Mod Integrity Check Failed ! -------------------");

        if (int result = MessageBoxA(
            nullptr,
            "MGS3 PS2 Demaster installation issue detected, base installation (OVR_US) files are missing.\n"
            "\n"
            "This can occur if Steam has verified integrity and damaged your mod files, or if the MGS3 Community Bugfix Compilation mod was installed/loaded AFTER the PS2 Demaster mod.\n"
            "\n"
            "Please reinstall the MGS3 PS2 Demaster Base mod to ensure proper game functionality.\n"
            "Or if using a mod manager, ensure the MGS3 PS2 Demaster Base mod is loaded AFTER the Community Bugfix Compilation mod.\n"
            "\n"
            "Would you like to open the MGS3 PS2 Demastered Nexus download page now to download the base package?\n"
            "\n"
            "(GitHub releases link also available on the Nexus page.)",
            "MGS3 PS2 Demaster (Base - OVR_US) - Mod Integrity Check Failed",
            MB_ICONWARNING | MB_YESNO);
        result == IDYES)
        {
            openPS2DemasterNexusPage();
        }

        return;
    }
    else
    {
        spdlog::info("Correct hash found for ovr_stm/ovr_us/_win/sna_suit_tears_sub_ovl_alp.bmp.ctxr, base installation (OVR_US) is properly installed.");
    }

}
