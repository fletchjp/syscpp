// Linux:    g++ -std=c++11 -lpthread demo.cpp -o demo
// Windows:  cl.exe demo.cpp

#include <string>
#include <iostream>
#include <sstream>
#include "ActorThread.hpp"

struct Message { std::string description; };
struct OtherMessage { double beautifulness; };

class Consumer : public ActorThread<Consumer>
{
    friend ActorThread<Consumer>;
    void onMessage(Message& p)
    {
        std::cout << "thread " << std::this_thread::get_id()
                  << " receiving " << p.description << std::endl;
    }
    void onMessage(OtherMessage& p)
    {
        std::cout << "thread " << std::this_thread::get_id()
                  << " receiving " << p.beautifulness << std::endl;
    }
};

class Producer : public ActorThread<Producer>
{
    friend ActorThread<Producer>;
    std::shared_ptr<Consumer> consumer;
    void onMessage(std::shared_ptr<Consumer>& c)
    {
        consumer = c;
        timerStart(true, std::chrono::milliseconds(250), TimerCycle::Periodic);
        timerStart(3.14, std::chrono::milliseconds(333), TimerCycle::Periodic);
    }
    void onTimer(const bool&)
    {
        std::ostringstream report;
        report << "test from thread " << std::this_thread::get_id();
        consumer->send(Message { report.str() });
    }
    void onTimer(const double& value)
    {
        consumer->send(OtherMessage { value });
    }
};

class Application : public ActorThread<Application>
{
    friend ActorThread<Application>;
    void onStart()
    {
        auto consumer = Consumer::create(); // spawn new threads
        auto producer = Producer::create();
        producer->send(consumer);
        std::this_thread::sleep_for(std::chrono::seconds(3));
        stop();
    }
};

int main()
{
    return Application::run(); // re-use existing thread
}