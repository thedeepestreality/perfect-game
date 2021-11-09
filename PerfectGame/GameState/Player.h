#pragma once
#include <memory>
#include "..\PerfectGameClient\UdpSocket.h"
#include "GameState.h"

enum class PlayerStatus { Active, NotActive };

class Player
{
private:
    const size_t kLossThreshold = 10;
    std::string const _name;
    GameState::GameIdx _x;
    GameState::GameIdx _y;
    size_t _loss_counter;
    PlayerStatus _status;
public:
    std::shared_ptr<UdpSocket> _sock;

    Player( std::string const& name,
            std::shared_ptr<UdpSocket> sock,
            GameState::GameIdx x,
            GameState::GameIdx y) : _name(name),
                                    _sock(sock),
                                    _x(x),
                                    _y(y),
                                    _loss_counter(0),
                                    _status(PlayerStatus::Active) {}

    void updatePos(GameState::GameIdx x, GameState::GameIdx y)
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
    
    void send(char* buf, size_t sz)
    {
        _sock->send(buf, sz);
    }
};