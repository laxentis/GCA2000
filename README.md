# GCA-2000 Plugin
GCA-2000 is a plugin for Euroscope that simulates the PAR display of a GCA-2000 radar on VATSIM

## Planned features
* Track view ✔
* Glidepath view ✔
* Custom radar blips ✔
* Radar trails
* Track and vertical error calculation ✔
* "Cross" view of aircraft deviation
* Approach parameters display ✔
* Wind and QNH display
* OCA/H line display ✔

## How to install and use
* Download the latest release DLL
* Load the DLL in the Euroscope Plugin, no need to allow the plugin to draw on standard ES screen
* Under Open SCT, create New GCA PAR radar display
* Save it under a convenient name (one ASR per runway), and close it
* Open the ASR with a text editor, enter the approach parameters
  --> /!\ Runway heading MUST BE true heading (course + magnetic deviation) /!\ 
* Save the ASR file and open it in Euroscope, you're ready !

<img width="2559" height="1324" alt="image" src="https://github.com/user-attachments/assets/754419e2-920c-409b-b191-6fdf873a55cd" />
