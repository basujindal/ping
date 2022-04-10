# A simple ping program written in C++

This is a ping imitation program. It will send an ICMP ECHO packet to the server of your choice and listen for an ICMP REPLY packet.

## Usage

- Run sudo ./ping DESTINATION_IP to send ICMP packet.
- For example sudo ./ping 8.8.8.8
- Ping requires root access because of SOCK_RAW
- If the IP address is not reachable, the program will timeout after 4 seconds.

# Disclaimer
- This code is largely based on code from [this link](https://cboard.cprogramming.com/networking-device-communication/41635-ping-program.html)
