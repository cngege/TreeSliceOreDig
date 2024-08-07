#pragma once

#include <string>
#include <unordered_map>
#include <list>

namespace my_mod {

	struct TreeType {
		std::string Chopped_Wood_type;
		int Chopped_Wood_Aux;
		std::list<int> Covered_Wood_Auxs;
		std::string Check_Leaves_type;
		std::list<int> Check_Leaves_Auxs;
	};

	struct Config {
		int version = 1;
		bool Time_Accrual = true;
		bool Sneak = true;
		std::list<std::string> Digging = {
			"minecraft:iron_ore",
			"minecraft:gold_ore",
			"minecraft:diamond_ore",
			"minecraft:lapis_ore",
			"minecraft:lit_redstone_ore",
			"minecraft:coal_ore",
			"minecraft:copper_ore",
			"minecraft:emerald_ore",
			"minecraft:quartz_ore",
			"minecraft:nether_gold_ore",
			"minecraft:deepslate_iron_ore",
			"minecraft:deepslate_gold_ore",
			"minecraft:deepslate_diamond_ore",
			"minecraft:deepslate_lapis_ore",
			"minecraft:lit_deepslate_redstone_ore",
			"minecraft:deepslate_emerald_ore",
			"minecraft:deepslate_coal_ore",
			"minecraft:deepslate_copper_ore"
		};
		std::list<TreeType> Tree = {
			{
				"minecraft:oak_log",
				0,
				{0,4,8},
				"minecraft:oak_leaves",
				{0}
			},
			{
				"minecraft:birch_log",
				0,
				{0,2,6,10},
				"minecraft:birch_leaves",
				{0}
			},
			{
				"minecraft:jungle_log",
				0,
				{0,3,7,11},
				"minecraft:jungle_leaves",
				{0}
			},
			{
				"minecraft:spruce_log",
				0,
				{0,1,5,9},
				"minecraft:spruce_leaves",
				{0}
			},
			{
				"minecraft:dark_oak_log",
				0,
				{0,1,5,9},
				"minecraft:dark_oak_leaves",
				{0}
			},
			{
				"minecraft:acacia_log",
				0,
				{0,4,8},
				"minecraft:acacia_leaves",
				{0}
			},
			{
				"minecraft:oak_log",
				0,
				{0,4,8},
				"minecraft:azalea_leaves",
				{0}
			},
			{
				"minecraft:oak_log",
				0,
				{0,4,8},
				"minecraft:azalea_leaves_flowered",
				{0}
			},
			{
				"minecraft:cherry_log",
				0,
				{0,4,8},
				"minecraft:cherry_leaves",
				{0}
			},
			{
				"minecraft:crimson_stem",
				0,
				{0,4,8},
				"minecraft:nether_wart_block",
				{0}
			},
			{
				"minecraft:warped_stem",
				0,
				{0,4,8},
				"minecraft:warped_wart_block",
				{0}
			}
		};

	};
}