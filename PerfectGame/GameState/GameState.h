#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "Player.h"

class GameState
{  
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
                    GameIdx x,
                    GameIdx y);
    Player* getPlayer(std::string const& name);
    void incrementAll();
    void sendAll();
};