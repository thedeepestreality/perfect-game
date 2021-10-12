#include "GameState.h"

GameState::GameState() : _rows(4), _cols(8)
{
    std::vector<Block> bg_layer(_cols, Block::Background);
    std::vector<Block> gr_layer(_cols, Block::Ground);
    _map.push_back(bg_layer);
    _map.push_back(bg_layer);
    _map.push_back(gr_layer);
    _map.push_back(gr_layer);

    _players["player"] = PlayerPos(1, 0);
    _players["gamer"] = PlayerPos(2, 3);
}

void GameState::setPlayerPos(std::string const& name, PlayerPos const& pos)
{
    _players[name] = pos;
}

bool GameState::serialize(char* buffer, size_t& sz)
{
    size_t const kGameIdxSz = sizeof(GameIdx);
    size_t player_size = 0;
    for (auto& playerPos : _players)
    {
        player_size += 2 * kGameIdxSz;
        player_size += playerPos.first.size() + 1;
    }
    size_t const kGameSize = 2 * kGameIdxSz
        + _rows * _cols * kGameIdxSz
        + kGameIdxSz + player_size;
    if (kGameSize > sz)
        return false;
    sz = kGameSize;
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
    return true;
}

bool GameState::deserialize(char const* buffer, size_t const kSize)
{
    _rows = buffer[0];
    _cols = buffer[1];
    size_t in_idx = 2;
    for (GameIdx row = 0; row < _rows; ++row)
        for (GameIdx col = 0; col < _cols; ++col)
            _map[row][col] = (Block)buffer[in_idx++];
    GameIdx const kPlayerSz = buffer[in_idx++];
    _players.clear();
    for (GameIdx idx = 0; idx < kPlayerSz; ++idx)
    {
        std::string const kPlayerId(buffer + in_idx);
        size_t const kStrSz = kPlayerId.length() + 1;
        in_idx += kStrSz;
        GameIdx const kX = buffer[in_idx++];
        GameIdx const kY = buffer[in_idx++];
        _players[kPlayerId] = PlayerPos(kX, kY);
    }
    return true;
}

bool GameState::serialize_player(char* buffer, size_t& sz, std::string const& name, PlayerPos const& pos)
{
    size_t ser_idx = name.length() + 1;
    strcpy_s(buffer, ser_idx, name.c_str());
    buffer[ser_idx++] = pos.first;
    buffer[ser_idx++] = pos.second;
    sz = ser_idx;
    return true;
}
