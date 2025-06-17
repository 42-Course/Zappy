class SpectatorBroadcaster : public IObserver {
public:
    explicit SpectatorBroadcaster(const ClientManager& clients);

    void onPlayerMoved(const Player* player) override;
    void onPlayerDied(const Player* player) override;
    // etc...

private:
    const ClientManager& clients_;
    void broadcast(const std::string& msg);
};