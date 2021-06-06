# ComoNetProj1

This project implements transferring data through a noissy channel, using hamming code in order to detect and fix errors.

The 'sender' devides a file to packets, then encodes and sends them to the 'channel', which adds "noise" with given possability (flips bits of the packet) and proceed to sending them to the 'server', which receives the packets, detects and fixes the errors, and saves the file.
