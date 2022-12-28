#pragma once

class App {
public:
    enum app_state_t {STATE_INIT, STATE_LOADING, STATE_IDLE};
protected:
    static App* m_instance;
    app_state_t m_state = STATE_INIT;
    App() {}
public:
    void draw();
    void update();
    void init();

    ~App();

    static void releaseInstance() { if (m_instance) delete m_instance; m_instance = nullptr; }
    static App* getInstance();
};
