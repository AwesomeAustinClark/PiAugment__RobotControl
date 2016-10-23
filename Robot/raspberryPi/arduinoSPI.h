#ifndef ARDUINOSPI_H
#define ARDUINOSPI_H

#include <string>


class arduinoSPI
{
    public:
        int set_interface_attribs (int fd, int speed, int parity);
        void set_blocking (int fd, int should_block);
        bool connect(int* fd, char* portname, int speed, bool blocking);
        bool send(int fd, std::string *msg);
        bool recivePacket(int fd, std::string* msg);
        bool close(int fd);
        arduinoSPI();
        virtual ~arduinoSPI();
    protected:
    private:
};

#endif // ARDUINOSPI_H
