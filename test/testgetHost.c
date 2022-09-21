#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <stdio.h>

int main ()
{
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char *addr;

    getifaddrs (&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET) {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            addr = inet_ntoa(sa->sin_addr);
            if ((ifa->ifa_name[0] != 'l' && ifa->ifa_name[1] != 'o'))
            {
                printf("Interface: %s\tAddress: %s\n", ifa->ifa_name, addr);
                break;
            }
        }
    }

    freeifaddrs(ifap);
    return 0;
}
