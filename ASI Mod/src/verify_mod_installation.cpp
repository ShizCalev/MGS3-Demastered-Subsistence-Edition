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
                spdlog::info("Opening webpages is not supported on SteamOS. Please visit the following URL on a different device to download the base package: https://www.nexusmods.com/metalgearsolid3mc/mods/189?tab=files");
                return;
            }
            ShellExecuteA(
                nullptr,
                "open",
                "https://www.nexusmods.com/metalgearsolid3mc/mods/189?tab=files",
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
    if (!MGS3_Demaster_Base_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_RESULT.exists || !(hashEquals(MGS3_Demaster_Base_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_RESULT, MGS3_Demaster_Base_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_SHA1)
                                                                                || hashEquals(MGS3_Demaster_Base_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_RESULT, MGS3_Demaster_2x_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_SHA1)
                                                                                || hashEquals(MGS3_Demaster_Base_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_RESULT, MGS3_Demaster_4x_OVR_STM_WIN_sok_coat_lupe_himo_CTXR_SHA1)))
    {
        spdlog::warn("------------------- ! MGS3 PS2 Demaster (Base) Missing ! -------------------");
        spdlog::warn("MGS3 PS2 Demaster installation issue detected, base package is NOT found.");
        spdlog::warn("This can occur if Steam has verified integrity and damaged your mod files, or if the Base Bugfix Compilation zip wasn't installed.");
        spdlog::warn("The base package is required for proper functionality, even when 2x & 4x packages are installed.");
        spdlog::warn("Please install the MGS3 PS2 Demaster -> Base <- package to ensure proper game functionality.");
        spdlog::warn("Please visit our Nexus page at: https://www.nexusmods.com/metalgearsolid3mc/mods/189?tab=files to download the base package.");
        spdlog::warn("Or our GitHub releases page at: https://github.com/ShizCalev/MGS3-Community-Bugfix-Compilation/releases");
        spdlog::warn("------------------- ! MGS3 PS2 Demaster (Base) Missing ! -------------------");

        if (int result = MessageBoxA(
            nullptr,
            "MGS3 PS2 Demaster installation issue detected, base package is NOT found.\n"
            "\n"
            "This can occur if Steam has verified integrity and damaged your mod files, or if the Base Bugfix Compilation zip wasn't installed.\n"
            "\n"
            "The base package is required for proper functionality, even when 2x & 4x packages are installed.\n"
            "Please install the MGS3 PS2 Demaster -> Base <- package to ensure proper game functionality.\n"
            "\n"
            "Would you like to open the Community Bugfix Nexus download page now to download the base package?\n"
            "\n"
            "(GitHub releases link also available on the Nexus page.)",
            "MGS3 PS2 Demaster (Base) Missing",
            MB_ICONWARNING | MB_YESNO);
        result == IDYES)
        {
            openPS2DemasterNexusPage();
        }

        return;
    }


    // ------------------------------------------------------
    // MGS3: Verify Afevis Bugfix Collection (base - jp dlc)
    // ------------------------------------------------------
    if (!MGS3_Demaster_Base_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_RESULT.exists)
    {
        spdlog::info("MGS3 PS2 Demaster base installation JPN DLC not found, DLC was likely uninstalled after installation. Skipping hash check...");
    }
    else if (!(hashEquals(MGS3_Demaster_Base_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_RESULT, MGS3_Demaster_Base_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_SHA1)
               || hashEquals(MGS3_Demaster_Base_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_RESULT, MGS3_Demaster_2x_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_SHA1)
               || hashEquals(MGS3_Demaster_Base_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_RESULT, MGS3_Demaster_4x_OVR_STM_OVR_JP_sna_suit_tears_sub_ovl_alp_CTXR_SHA1)))
    {
        spdlog::warn("------------------- ! MGS3 PS2 Demaster (Base - JPN DLC) Missing ! -------------------");
        spdlog::warn("MGS3 PS2 Demaster installation issue detected, base installation (JPN DLC) fixes are missing.");
        spdlog::warn("This can occur if Steam has verified integrity and damaged your mod files, or if you have reinstalled the Japanese Language DLC after installing the MGS3 PS2 Demaster.");
        spdlog::warn("Please reinstall the MGS3 PS2 Demaster -> Base <- package to ensure proper game functionality.");
        spdlog::warn("Please visit our Nexus page at: https://www.nexusmods.com/metalgearsolid3mc/mods/189?tab=files to download the base package.");
        spdlog::warn("Or our GitHub releases page at: https://github.com/ShizCalev/MGS3-Community-Bugfix-Compilation/releases");
        spdlog::warn("------------------- ! MGS3 PS2 Demaster (Base - JPN DLC) Missing ! -------------------");

        if (int result = MessageBoxA(
            nullptr,
            "MGS3 PS2 Demaster installation issue detected, base installation (JPN DLC) fixes are missing.\n"
            "\n"
            "This can occur if Steam has verified integrity and damaged your mod files, or if you have reinstalled the Japanese Language DLC after installing the MGS3 PS2 Demaster.\n"
            "\n"
            "Please reinstall the MGS3 PS2 Demaster -> Base <- package to ensure proper game functionality.\n"
            "\n"
            "Would you like to open the Community Bugfix Nexus download page now to download the base package?\n"
            "\n"
            "(GitHub releases link also available on the Nexus page.)",
            "MGS3 PS2 Demaster (Base - JPN DLC) Missing",
            MB_ICONWARNING | MB_YESNO);
        result == IDYES)
        {
            openPS2DemasterNexusPage();
        }

        return;
    }

        // ------------------------------------------------------
    // MGS3: Verify Afevis Bugfix Collection (base - jp dlc)
    // ------------------------------------------------------
    if (!MGS3_Demaster_Base_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_RESULT.exists)
    {
        spdlog::info("MGS3 PS2 Demaster base installation JPN DLC not found, DLC was likely uninstalled after installation. Skipping hash check...");
    }
    else if (!(hashEquals(MGS3_Demaster_Base_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_RESULT, MGS3_Demaster_Base_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_SHA1)
               || hashEquals(MGS3_Demaster_Base_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_RESULT, MGS3_Demaster_2x_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_SHA1)
               || hashEquals(MGS3_Demaster_Base_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_RESULT, MGS3_Demaster_4x_OVR_STM_OVR_US_sna_suit_tears_sub_ovl_alp_CTXR_SHA1)))
    {
        spdlog::warn("------------------- ! MGS3 PS2 Demaster (Base) Missing ! -------------------");
        spdlog::warn("MGS3 PS2 Demaster installation issue detected, base installation fixes are missing.");
        spdlog::warn("This can occur if Steam has verified integrity and damaged your mod files, or if you have reinstalled the Japanese Language DLC after installing the MGS3 PS2 Demaster.");
        spdlog::warn("Please reinstall the MGS3 PS2 Demaster -> Base <- package to ensure proper game functionality.");
        spdlog::warn("Please visit our Nexus page at: https://www.nexusmods.com/metalgearsolid3mc/mods/189?tab=files to download the base package.");
        spdlog::warn("Or our GitHub releases page at: https://github.com/ShizCalev/MGS3-Community-Bugfix-Compilation/releases");
        spdlog::warn("------------------- ! MGS3 PS2 Demaster (Base - JPN DLC) Missing ! -------------------");

        if (int result = MessageBoxA(
            nullptr,
            "MGS3 PS2 Demaster installation issue detected, base installation (JPN DLC) fixes are missing.\n"
            "\n"
            "This can occur if Steam has verified integrity and damaged your mod files, or if you have reinstalled the Japanese Language DLC after installing the MGS3 PS2 Demaster.\n"
            "\n"
            "Please reinstall the MGS3 PS2 Demaster -> Base <- package to ensure proper game functionality.\n"
            "\n"
            "Would you like to open the Community Bugfix Nexus download page now to download the base package?\n"
            "\n"
            "(GitHub releases link also available on the Nexus page.)",
            "MGS3 PS2 Demaster (Base - JPN DLC) Missing",
            MB_ICONWARNING | MB_YESNO);
        result == IDYES)
        {
            openPS2DemasterNexusPage();
        }

        return;
    }

}
