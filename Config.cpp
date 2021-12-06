//
// Created by Александр Шоршин on 31.10.2021.
//

#include "Config.hpp"

void Config::SetIP(const std::string &newIP) {
    ip.setIP(newIP);
}

void Config::SetPort(int newPort) {
    port = newPort;
}

void Config::AddLocation(const location &newLocation) {

    this->locations.push_back(newLocation);

}

IPAddress Config::GetIP() {
    return ip;
}

int Config::GetPort() {
    return port;
}
