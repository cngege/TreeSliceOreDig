#include <nlohmann/json.hpp>﻿
#include <fmt/format.h>
#include <functional>
#include <ll/api/Config.h>
#include <ll/api/command/Command.h>
#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/data/KeyValueDB.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/ListenerBase.h>
#include <ll/api/event/player/PlayerJoinEvent.h>
#include <ll/api/event/player/PlayerUseItemEvent.h>
#include <ll/api/form/ModalForm.h>
#include <ll/api/io/FileUtils.h>
#include <ll/api/mod/NativeMod.h>
#include <ll/api/mod/ModManagerRegistry.h>
#include <ll/api/service/Bedrock.h>
#include <mc/entity/utilities/ActorType.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/world/actor/player/Player.h>
#include <mc/world/item/registry/ItemStack.h>

#include <mc/world/level/Level.h>
#include <mc/world/level/block/Block.h>
#include <mc/world/level/block/BlockLegacy.h>
#include <mc/world/level/BlockPalette.h>
#include <mc/world/level/BlockSource.h>
#include <mc/world/level/dimension/Dimension.h>


#include <memory>
#include <stdexcept>
#include <direct.h>
#include "MyMod.h"

#include "ll/api/memory/Hook.h"
#include "LightHook.h"

using namespace nlohmann;

using DestroyBlockCALLType = void(__fastcall*)(Block* thi, Player* player, BlockPos* Bpos);
DestroyBlockCALLType OBlock_PlayerDestroy;

void           Block_PlayerDestroy(Block*, Player*, BlockPos*);
void           CheckMinerals(Block*, Player*, BlockPos*, std::string, std::string);
bool           CheckLeaves(json, BlockPos, int);
unsigned short Block_getTileData(Block*);
bool           CheckUshortArray(json, unsigned short);
void           TreeCutting(json, Block*, Player*, BlockPos*);


std::string configpath = "./plugins/TreeSliceOreDig/";

static auto config = R"(
    {
        "Time_Accrual": true,
        "Sneak": true,
        "Digging": [
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
         ],
        "Tree": [
            {
                "Chopped_Wood_type":"minecraft:oak_log",
                "Chopped_Wood_Aux": 0,
                "Covered_Wood_Auxs":[0,4,8],
                "Check_Leaves_type":"minecraft:leaves",
                "Check_Leaves_Auxs":[4]
            },
            {
                "Chopped_Wood_type":"minecraft:birch_log",
                "Chopped_Wood_Aux": 0,
                "Covered_Wood_Auxs":[0,2,6,10],
                "Check_Leaves_type":"minecraft:leaves",
                "Check_Leaves_Auxs":[2,6]
            },
            {
                "Chopped_Wood_type":"minecraft:jungle_log",
                "Chopped_Wood_Aux": 0,
                "Covered_Wood_Auxs":[0,3,7,11],
                "Check_Leaves_type":"minecraft:leaves",
                "Check_Leaves_Auxs":[3,7]
            },
            {
                "Chopped_Wood_type":"minecraft:jungle_log",
                "Chopped_Wood_Aux": 0,
                "Covered_Wood_Auxs":[0,3,7,11],
                "Check_Leaves_type":"minecraft:leaves2",
                "Check_Leaves_Auxs":[4]
            },
            {
                "Chopped_Wood_type":"minecraft:spruce_log",
                "Chopped_Wood_Aux": 0,
                "Covered_Wood_Auxs":[0,1,5,9],
                "Check_Leaves_type":"minecraft:leaves",
                "Check_Leaves_Auxs":[1,5]
            },
            {
                "Chopped_Wood_type":"minecraft:dark_oak_log",
                "Chopped_Wood_Aux": 0,
                "Covered_Wood_Auxs":[0,1,5,9],
                "Check_Leaves_type":"minecraft:leaves2",
                "Check_Leaves_Auxs":[1,5]
            },
            {
                "Chopped_Wood_type":"minecraft:acacia_log",
                "Chopped_Wood_Aux": 0,
                "Covered_Wood_Auxs":[0,4,8],
                "Check_Leaves_type":"minecraft:leaves2",
                "Check_Leaves_Auxs":[0,4]
            },
            {
                "Chopped_Wood_type":"minecraft:oak_log",
                "Chopped_Wood_Aux": 0,
                "Covered_Wood_Auxs":[0,4,8],
                "Check_Leaves_type":"minecraft:azalea_leaves",
                "Check_Leaves_Auxs":[0]
            },
            {
                "Chopped_Wood_type":"minecraft:cherry_log",
                "Chopped_Wood_Aux": 0,
                "Covered_Wood_Auxs":[0,4,8],
                "Check_Leaves_type":"minecraft:cherry_leaves",
                "Check_Leaves_Auxs":[0]
            },
            {
                "Chopped_Wood_type":"minecraft:crimson_stem",
                "Chopped_Wood_Aux": 0,
                "Covered_Wood_Auxs":[0,4,8],
                "Check_Leaves_type":"minecraft:nether_wart_block",
                "Check_Leaves_Auxs":[0]
            },
            {
                "Chopped_Wood_type":"minecraft:warped_stem",
                "Chopped_Wood_Aux": 0,
                "Covered_Wood_Auxs":[0,4,8],
                "Check_Leaves_type":"minecraft:warped_wart_block",
                "Check_Leaves_Auxs":[0]
            }
         ]
    }
)"_json;

HookInformation info;

bool run() {
    // 读写配置文件
    if (_access(configpath.c_str(), 0) == -1) // 表示配置文件所在的文件夹不存在
    {
        if (_mkdir(configpath.c_str()) == -1) {
            // 文件夹创建失败
            my_mod::MyMod::getInstance().getSelf().getLogger().warn(
                "Directory creation failed, please manually create the plugins/TreeSliceOreDig directory"
            );
            return false;
        }
    }
    std::ifstream f((configpath + "TreeSliceOreDig.json").c_str());
    if (f.good()) // 表示配置文件存在
    {
        f >> config;
        f.close();
    } else {
        // 配置文件不存在
        std::ofstream c((configpath + "TreeSliceOreDig.json").c_str());
        c << config.dump(2);
        c.close();
    }

    info = CreateHook((void*)(ll::memory::symbolCache<"?playerDestroy@Block@@QEBAXAEAVPlayer@@AEBVBlockPos@@@Z">), (void*)&Block_PlayerDestroy);
    return true;
}

bool modenable() {
    auto status = EnableHook(&info);
    OBlock_PlayerDestroy = (DestroyBlockCALLType)info.Trampoline;
    if (!status) {
        my_mod::MyMod::getInstance().getSelf().getLogger().warn("Hook开启失败.");
        return false;
    }
    return true;
}

bool moddisable() {
    auto status = DisableHook(&info);
    if (!status) {
        my_mod::MyMod::getInstance().getSelf().getLogger().warn("Hook关闭失败.");
        return false;
    }
    return true;
}



void Block_PlayerDestroy(Block* thi, Player* player, BlockPos* Bpos) {
    if (config["Sneak"] == true && !player->isSneaking()) {
        return OBlock_PlayerDestroy(thi, player, Bpos);
    }
    std::string name = thi->getTypeName();
    // 砍树
    for (auto& tree : config["Tree"]) {
        // 判断砍的是不是配置文件中指定的木头
        if (tree["Chopped_Wood_type"] == name) {
            unsigned short aux = Block_getTileData(thi);
            // 检查 砍的是一颗配置文件中 规定的树
            if (tree["Chopped_Wood_Aux"] == aux) {
                if (CheckLeaves(tree, *Bpos, player->getDimensionId())) {
                    return TreeCutting(tree, thi, player, Bpos);
                }
            }
        }
    }

    // 检查矿物
    for (auto& orename : config["Digging"]) {
        if (orename == thi->getTypeName()) {
            if (orename == "minecraft:lit_redstone_ore") {
                return CheckMinerals(thi, player, Bpos, orename, "minecraft:redstone_ore");
            } else if (orename == "minecraft:lit_deepslate_redstone_ore") {
                return CheckMinerals(thi, player, Bpos, orename, "minecraft:deepslate_redstone_ore");
            } else {
                return CheckMinerals(thi, player, Bpos, orename, "");
            }
        }
    }
    return OBlock_PlayerDestroy(thi, player, Bpos);
}


bool CheckUshortArray(json arr, unsigned short val) {
    if (!arr.is_array()) return false;
    for (int i = 0; i < arr.size(); i++) {
        // logger.debug("i:{},arr[i]:{},val:{}", i, arr[i], val);
        if (arr[i] == val) {
            return true;
        }
    }
    return false;
}

Block* Level_getBlock(BlockPos* pos, int dimId) {
    BlockSource* blockSource = const_cast<BlockSource*>(&ll::service::getLevel()->getDimension(dimId).get()->getBlockSourceFromMainChunkSource());
    return (Block*)&(blockSource->getBlock(*pos));
}

bool Level_setBlock(BlockPos pos, int dimId, std::string block, ushort data) {
    BlockSource* blockSource = const_cast<BlockSource*>(&ll::service::getLevel()->getDimension(dimId).get()->getBlockSourceFromMainChunkSource());
    optional_ref<const Block> bl = Block::tryGetFromRegistry(block, data);
    if (!bl.has_value()) {
        return false;
    }
    return blockSource->setBlock(pos, bl, 3, nullptr, nullptr);
}

// 破坏的方块名称特殊值， 位置 维度
bool CheckLeaves(json tree, BlockPos Bpos, int dimid) {
    if (Bpos.y > 320) return false; // 建筑高度 320个方块
    //bool CheckLeavesValue = false;
    for (int x = -1; x <= 1; x++) {
        for (int z = -1; z <= 1; z++) {
            auto nBpos     = BlockPos(Bpos.x + x, Bpos.y + 1, Bpos.z + z);
            auto block     = Level_getBlock(&nBpos, dimid);
            auto blockname = block->getTypeName();
            if (blockname == tree["Chopped_Wood_type"]) {
                auto data = Block_getTileData(block);
                if (CheckUshortArray(tree["Covered_Wood_Auxs"], data)) { // 如果向上检查的木头是匹配的
                    // if (CheckLeaves(tree, BlockPos(Bpos.x, Bpos.y + 1, Bpos.z), dimid)) {
                    if (CheckLeaves(tree, BlockPos(nBpos), dimid)) {
                        return true;
                    }
                }
            }
            if (blockname == tree["Check_Leaves_type"]) {
                auto data = Block_getTileData(block);
                if (CheckUshortArray(tree["Check_Leaves_Auxs"], data)) { // 如果向上检查的树叶是匹配的
                    return true;
                }
            }
        }
    }
    return false;
}

// 正式砍树
void TreeCutting(json tree, Block* block, Player* player, BlockPos* Bpos) {
    if (Bpos->y > 320) return;
    OBlock_PlayerDestroy(block, player, Bpos);
    Level_setBlock(*Bpos, player->getDimensionId(), "minecraft:air", 0);

    for (int y = 0; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            for (int z = -1; z <= 1; z++) {
                BlockPos* nBpos  = new BlockPos(Bpos->x + x, Bpos->y + y, Bpos->z + z);
                Block*    nblock = Level_getBlock(nBpos, player->getDimensionId());

                if (nblock->getTypeName() == tree["Chopped_Wood_type"]) {
                    if (CheckUshortArray(tree["Covered_Wood_Auxs"], Block_getTileData(nblock))) {
                        TreeCutting(tree, nblock, player, nBpos);
                        delete nBpos;
                    }
                }
            }
        }
    }
}


void CheckMinerals(Block* block, Player* player, BlockPos* Bpos, std::string Bname, std::string compatible) {
    OBlock_PlayerDestroy(block, player, Bpos);
    Level_setBlock(*Bpos, player->getDimensionId(), "minecraft:air", 0);
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            for (int z = -1; z <= 1; z++) {
                BlockPos* nBpos  = new BlockPos(Bpos->x + x, Bpos->y + y, Bpos->z + z);
                Block*    nblock = Level_getBlock(nBpos, player->getDimensionId());

                if (nblock->getTypeName() == Bname
                    || (compatible != "" && nblock->getTypeName() == compatible)
                           && !player->getSelectedItem().isNull()) {
                    CheckMinerals(nblock, player, nBpos, Bname, compatible);
                }
                delete nBpos;
            }
        }
    }
}

unsigned short Block_getTileData(Block* block) {
    return block->getData();
}