#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "Player.h"

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
    //std::unordered_map<std::string, std::pair<GameIdx, GameIdx> > _players;
    std::unordered_map<std::string, Player> _players;
public:
    GameState(std::string const& config_file_name) {}
    GameState();

    bool serialize(char* buffer, size_t& sz);
    bool deserialize(char const* buffer, size_t const kSize);
    void addPlayer( std::string const& name,
                    std::shared_ptr<UdpSocket> sock,
                    GameState::GameIdx x,
                    GameState::GameIdx y);
    bool getPlayer(std::string const& name, Player* out);
    void incrementAll();
    void sendAll();
};