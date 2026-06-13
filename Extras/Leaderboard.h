#pragma once
#include "Singleton.h"
#include <cstdint>
#include <array>
#include <vector>
#include <filesystem>

namespace dae
{
	constexpr size_t MAX_ENTRIES{ 10 };

	class Leaderboard final : public Singleton<Leaderboard>
	{
	public:
		struct Entry
		{
			/*
			@DEV I am using 16bit uint:
			despite max val being 65535,
			since all enemies give score that ends in 00,
			i can store scores without 00, and visually fake them
			thus extending MAX score to 6'553'500 which is more than enough!
			thus cutting my storage by 2 bytes!!!
			*/
			std::array<char, 3> initials{};
			uint16_t scoreHundreds{};
		};
		static_assert(std::is_trivially_copyable_v<Entry>);

		/*
		@DEV This one tries to load leaderboard entries
		It also saves the filepath path
		if no file has been found, creates a new .bin file 
		(I am petty about my storage :D)
		*/
		bool TryLoadEntries(std::filesystem::path path);

		/*
		@DEV This one saves entry if score is withing top MAX_ENTRIES
		If there are < 10, it just saves and sorts by score
		If >= 10, it deletes the smalles one, inserts entry at the back, std::sorts the thing and writes back to file
		(I wonder how Emscripten will react)
		*/
		void SaveEntry(const Entry& entry);

		/*
		@DEV This one returns T/F if given score qualifies in the top MAX_ENTRIES
		*/
		bool DoesScoreQualify(uint16_t scoreHundreds) const;

		/*
		@DEV This one returns projected placement of Provided score
		Namely for visual representation, i suppose
 		*/
		int GetProjectedPlacement(uint16_t scoreHundreds) const;

		const std::array<Entry, MAX_ENTRIES>& GetEntries() const { return m_Entries; }
		size_t GetEntryCount() const { return m_EntryCount; }
		// DANGEROUS:
		void ClearLeaderboard();

	private:
		std::filesystem::path m_Filepath;
		std::array<Entry, MAX_ENTRIES> m_Entries;
		size_t m_EntryCount{};	// Stores amount of valid entries 0 <= m_EntryCount <= MAX_ENTRIES

		inline void SortEntries();
		void LoadDummyData();
	};
}