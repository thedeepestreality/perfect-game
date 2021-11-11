#pragma once
#include <memory>
#include "..\PerfectGameClient\UdpSocket.h"

enum class PlayerStatus { Active, NotActive };
typedef unsigned char GameIdx;
typedef std::pair<GameIdx, GameIdx> PlayerPos;

class Player
{
private:
    const size_t kLossThreshold = 10;
    std::string const _name;
    GameIdx _x;
    GameIdx _y;
    size_t _loss_counter;
    PlayerStatus _status;
public:
    std::shared_ptr<UdpSocket> _sock;

    Player( std::string const& name,
            GameIdx x,
            GameIdx y) : _name(name),
                                    _x(x),
                                    _y(y),
                                    _loss_counter(0),
                                    _status(PlayerStatus::Active) {}

    Player( std::string const& name,
            std::shared_ptr<UdpSocket> sock,
            GameIdx x,
            GameIdx y) : Player(name, x, y) 
    {
        _sock = sock;
    }

    void updatePos(GameIdx x, GameIdx y)
    {
        _x = x;
        _y = y;
    }

    bool serialize(char* buffer, size_t& sz) const
    {
        size_t ser_idx = _name.length() + 1;
        strcpy_s(buffer, ser_idx, _name.c_str());
        buffer[ser_idx++] = _x;
        buffer[ser_idx++] = _y;
        sz = ser_idx;
        return true;
    }

    PlayerStatus getStatus() const
    {
        return _status;
    }

    void incrementLossCounter()
    {
        if (_status == PlayerStatus::NotActive)
            return;
        if (++_loss_counter > kLossThreshold)
        {
            _loss_counter = 0;
            _status = PlayerStatus::NotActive;
        }
    }

    void resetLossCounter()
    {
        _loss_counter = 0;
    }

    void activate()
    {
        _status = PlayerStatus::Active;
    }
    
    void send(const char* buf, size_t sz)
    {
        _sock->send(buf, sz);
    }

    PlayerPos getPos()
    {
        return std::make_pair(_x, _y);
    }

    std::string getName()
    {
        return _name;
    }
};