#pragma once
#include <vector>
#include <unordered_map>
#include <string>

class GameState
{
    typedef unsigned char GameIdx;
    typedef std::pair<GameIdx, GameIdx> PlayerPos;
    enum class Block {Ground = 0, Background = 1};
    std::vector<std::vector<Block> > _map;
    GameIdx _rows;
    GameIdx _cols;
    std::unordered_map<std::string, std::pair<GameIdx, GameIdx> > _players;
public:
    GameState(std::string const& config_file_name) {}
    GameState() : _rows(4), _cols(8)
    {
        std::vector<Block> bg_layer(_cols, Block::Background);
        std::vector<Block> gr_layer(_cols, Block::Ground);
        _map.push_back(bg_layer);
        _map.push_back(bg_layer);
        _map.push_back(gr_layer);
        _map.push_back(gr_layer);

        _players["player"] = PlayerPos(1, 0);
    }

    void setPlayerPos(std::string const& name, PlayerPos const& pos)
    {
        _players[name] = pos;
    }

    bool serialize(char* buffer, size_t const kSize)
    {
        size_t const kGameIdxSz = sizeof(GameIdx);
        size_t player_size = 0;
        for (auto& playerPos : _players)
        {
            player_size += 2* kGameIdxSz;
            player_size += playerPos.first.size() + 1;
        }
        size_t const kGameSize = 2 * kGameIdxSz
            + _rows * _cols * kGameIdxSz
            + kGameIdxSz + player_size;
        if (kGameSize > kSize)
            return false;
        buffer[0] = _rows;
        buffer[1] = _cols;
        size_t out_idx = 2;
        for (GameIdx row = 0; row < _rows; ++row)
            for (GameIdx col = 0; col < _cols; ++col)
                buffer[out_idx++] = (char)_map[row][col];
        buffer[out_idx++] = _players.size();
        for (auto& playerPos : _players)
        {
            char const* c_id = playerPos.first.c_str();
            size_t const str_size = playerPos.first.size();
            strcpy_s(buffer + out_idx, str_size + 1, c_id);
            out_idx += str_size + 1;
            buffer[out_idx++] = playerPos.second.first;
            buffer[out_idx++] = playerPos.second.second;
        }
    }
};