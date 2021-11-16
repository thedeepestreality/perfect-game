#include "GameState.h"

GameState::GameState() : _rows(4), _cols(8)
{
    std::vector<Block> bg_layer(_cols, Block::Background);
    std::vector<Block> gr_layer(_cols, Block::Ground);
    _map.push_back(bg_layer);
    _map.push_back(bg_layer);
    _map.push_back(gr_layer);
    _map.push_back(gr_layer);
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
    size_t ser_player_sz;
    for (auto& map_pair : _players)
    {
        auto const& player = map_pair.second;
        player.serialize(buffer + out_idx, ser_player_sz);
        out_idx += ser_player_sz;
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
        _players.emplace(
            std::make_pair(
                kPlayerId,
                Player(kPlayerId, kX, kY)
            )
        );
    }
    return true;
}

void GameState::addPlayer(std::string const& name, std::shared_ptr<UdpSocket> sock, GameIdx x, GameIdx y)
{
    _players.emplace(
        std::make_pair(
            name,
            Player(name, sock, x, y)
        )
    );
}

Player* GameState::getPlayer(std::string const& name)
{
    auto player_itr = _players.find(name);
    if (player_itr == _players.end())
        return nullptr;
    return &(player_itr->second);
}

void GameState::incrementAll()
{
    for (auto& map_pair : _players)
    {
        auto& player = map_pair.second;
        player.incrementLossCounter();
    }
}

void GameState::sendAll()
{
    static size_t const kBufSize = 512;
    static char buf[kBufSize];
    size_t fact_sz;
    serialize(buf, fact_sz);
    for (auto& map_pair : _players)
    {
        auto& player = map_pair.second;
        if (player.getStatus() != PlayerStatus::NotActive)
            player.send(buf, fact_sz);
    }
}

void GameState::updateMap(GameIdx x, GameIdx y, Block block)
{
    _map[x][y] = block;
}
