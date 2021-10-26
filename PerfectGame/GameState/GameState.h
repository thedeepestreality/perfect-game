#pragma once
#include <vector>
#include <unordered_map>
#include <string>

class GameState
{
public:
    typedef unsigned char GameIdx;
    typedef std::pair<GameIdx, GameIdx> PlayerPos;
private:
    enum class Block {Ground = 0, Background = 1};
    std::vector<std::vector<Block> > _map;
    GameIdx _rows;
    GameIdx _cols;
    ///TODO: map of Player objects?
    std::unordered_map<std::string, std::pair<GameIdx, GameIdx> > _players;
public:
    GameState(std::string const& config_file_name) {}
    GameState();

    void setPlayerPos(std::string const& name, PlayerPos const& pos);
    PlayerPos getPlayerPos(std::string const& name){return _players[name];}
    bool serialize(char* buffer, size_t& sz);
    bool deserialize(char const* buffer, size_t const kSize);
    bool serialize_player(char* buffer, size_t& sz, std::string const& name, PlayerPos const& pos);
};