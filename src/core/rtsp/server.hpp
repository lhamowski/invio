#pragma once

namespace invio::rtsp {

struct config final
{
};

class server final
{
public:
    server();

    server(const server&) = delete;
    server& operator=(const server&) = delete;
};

}  // namespace invio::rtsp