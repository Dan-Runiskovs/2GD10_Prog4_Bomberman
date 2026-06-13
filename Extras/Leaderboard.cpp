#include "Leaderboard.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <string>

bool dae::Leaderboard::TryLoadEntries(std::filesystem::path path)
{
    // --- Save filepath for future writes ---
    m_Filepath = std::move(path);
    std::cout << "Loading from: "
        << m_Filepath.string()
        << '\n';
    // --- Reset current leaderboard ---
    m_EntryCount = 0;
    m_Entries.fill({});

    // --- Open file in binary mode -> raw bytes ---
    std::ifstream file{ m_Filepath, std::ios::binary };

    // --- File doesn't exist -> create empty leaderboard ---
    if (!file.is_open())
    {
        std::cout << "No such file, creating new...\n";
        std::ofstream createFile{ m_Filepath, std::ios::binary };

        // --- Write current amoun of entries -> none ---
        uint8_t count{};
        createFile.write( 
            reinterpret_cast<const char*>(&count), 
            sizeof(count));

        // --- Make it explicit ---
        m_EntryCount = 0;

        return true;
    }

    // --- Read amount of entries ---
    uint8_t count{};

    // --- Read first byte of file ---
    // --- This byte stores how many leaderboard entries exist ---
    file.read(reinterpret_cast<char*>(&count),sizeof(count));
    std::cout << "Loaded leaderboard with " << std::to_string(static_cast<int>(count)) << " entries!\n";
    std::cout << "File size: "
        << std::filesystem::file_size(m_Filepath)
        << '\n';

    std::cout
        << "g=" << file.good()
        << " eof=" << file.eof()
        << " fail=" << file.fail()
        << " bad=" << file.bad()
        << '\n';
    /*
    if (!file.good())
    {
        std::cout << "File not good?1\n";
        return false;
    }
    */
    
    // --- Safety clamp ---
    // --- If file somehow says there are 200 entries ---
    // --- only load up to MAX_ENTRIES ---
    m_EntryCount = std::min(static_cast<size_t>(count), MAX_ENTRIES);

    // --- Read all leaderboard entries ---
    file.read(
        reinterpret_cast<char*>(m_Entries.data()),
        static_cast<std::streamsize>(m_EntryCount * sizeof(Entry)));

    // --- Close file ---
    file.close();

    // --- Load possible dummy data --
    if (m_EntryCount == 0) LoadDummyData();

    // --- Sort for safety ---
    SortEntries();

    return true;
}

void dae::Leaderboard::SaveEntry(const Entry& entry)
{
    std::cout << "Saving Entry with score: " << std::to_string(static_cast<int>(entry.scoreHundreds)) << "!\n";
    std::cout << "Saving to: "
        << m_Filepath.string()
        << '\n';
    // --- Leaderboard not full yet ---
    if (m_EntryCount < MAX_ENTRIES)
    {
        // --- Insert new entry at first free slot ---
        m_Entries[m_EntryCount++] = entry;
    }
    else // --- Leaderboard full ---
    {
        // --- If score is too low, ignore it ---
        if (!DoesScoreQualify(entry.scoreHundreds)) return;

        // --- Replace worst score ---
        // --- After sorting, last entry is always the lowest. --- 
        m_Entries[m_EntryCount - 1] = entry;
    }

    // --- Sort after inserting ---
    SortEntries();

    // --- Open file for writing (trunc to fully re-write) ---
    std::ofstream file{m_Filepath, std::ios::binary | std::ios::trunc };

    // --- Write entry count ---
    uint8_t count{ static_cast<uint8_t>(m_EntryCount) };
    file.write(
        reinterpret_cast<const char*>(&count),
        sizeof(count));

    // --- Write all entries ---
    file.write(
        reinterpret_cast<const char*>(m_Entries.data()),
        static_cast<std::streamsize>(m_EntryCount * sizeof(Entry)));

    file.close();
    std::cout << "Entry saved!\n";
    std::cout
        << "File size after save: "
        << std::filesystem::file_size(m_Filepath)
        << '\n';
}

bool dae::Leaderboard::DoesScoreQualify(uint16_t scoreHundreds) const
{
    // --- Less entries than MAX_ENTRIES -> auto qualify
    if (m_EntryCount < MAX_ENTRIES) return true;

    // --- Check if score is more than the last place saved ---
    return scoreHundreds > m_Entries[m_EntryCount - 1].scoreHundreds;
}

int dae::Leaderboard::GetProjectedPlacement(uint16_t scoreHundreds) const
{
    int placement{ 1 };

    for (size_t i{}; i < m_EntryCount; ++i)
    {
        // --- Qualifies as i+1? ---
        if (scoreHundreds > m_Entries[i].scoreHundreds) return placement;

        ++placement;
    }

    // --- Set to last ---
    if (m_EntryCount < MAX_ENTRIES) return placement;

    // --- Returns 11 for invalid ---
    return static_cast<int>(MAX_ENTRIES + 1);
}

inline void dae::Leaderboard::SortEntries()
{
    // --- My little helper not to repeat code (descending) --
    std::sort(
        m_Entries.begin(),
        m_Entries.begin() + m_EntryCount,
        [](const Entry& lhs, const Entry& rhs)
        {
            return lhs.scoreHundreds > rhs.scoreHundreds;
        });
}

void dae::Leaderboard::LoadDummyData()
{
    Entry entry1{};
    entry1.initials[0] = 'T';
    entry1.initials[1] = 'O';
    entry1.initials[2] = 'M';
    entry1.scoreHundreds = 6900;
    SaveEntry(entry1);
    Entry entry2{};
    entry2.initials[0] = 'A';
    entry2.initials[1] = 'V';
    entry2.initials[2] = 'A';
    entry2.scoreHundreds = 1111;
    SaveEntry(entry2);
    Entry entry3{};
    entry3.initials[0] = 'D';
    entry3.initials[1] = 'A';
    entry3.initials[2] = 'N';
    entry3.scoreHundreds = 20;
    SaveEntry(entry3);
}
