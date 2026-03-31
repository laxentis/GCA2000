# GCA-2000 Plugin
GCA-2000 is a plugin for Euroscope that simulates the PAR display of a GCA-2000 radar on VATSIM. See how to install below.

## Planned features
* Track view ✔
* Glidepath view ✔
* Custom radar blips ✔
* Radar trails ✔
* Track and vertical error calculation ✔
* "Cross" view of aircraft deviation ✔
* Approach parameters display ✔
* Wind and QNH display
* OCA/H line display ✔

## Example ASR
`DisplayTypeName:GCA PAR radar display` 
`DisplayTypeNeedRadarContent:0`  
`DisplayTypeGeoReferenced:0`  
`PLUGIN:PAR GCA-2000:Altitude:577`  
`PLUGIN:PAR GCA-2000:Description:LFOA PAR 24`  
`PLUGIN:PAR GCA-2000:Heading:240.0`  
`PLUGIN:PAR GCA-2000:Latitude:N047.03.48.25`  
`PLUGIN:PAR GCA-2000:Longitude:E002.39.06.90`  
`PLUGIN:PAR GCA-2000:Maximum height:4000`  
`PLUGIN:PAR GCA-2000:Maximum range:13.0`  
`PLUGIN:PAR GCA-2000:Maximum track deviation:2.0`  
`PLUGIN:PAR GCA-2000:OCH:253`  
`PLUGIN:PAR GCA-2000:Slope:3.0`  

Altitude: Threshold altitude : the glidepath is calibrated to reach threshold at 50ft  
Description: N/A at this time  
Heading: **True** heading : Add the magnetic deviation to the final approach course  
Latitude & Longitude : Threshold coordinates (and **not** the TDZ coordinates) : the glidepath is calibrated to reach threshold at 50ft  
Maximum height: maximum height of the vertical display in feet  
Maximum range: maximum range of both displays in nautical miles  
Maximum track deviation: maximum lateral deviation on the horizontal display in nautical miles (use integer values for better scale)  
OCH: Obstacle clearance height or minimas in feet AGL  
Slope : Glidepath angle in degrees  
  
## How to install and use
* Download the latest release DLL
* Load the DLL in the Euroscope Plugin, no need to allow the plugin to draw on standard ES screen
* Under Open SCT, create New GCA PAR radar display
* Save it under a convenient name (one ASR per runway), and close it
* Open the ASR with a text editor, enter the approach parameters
  --> /!\ Runway heading MUST BE true heading (course + magnetic deviation) /!\ 
* Save the ASR file and open it in Euroscope, you're ready !

<img width="2559" height="1339" alt="image" src="https://github.com/user-attachments/assets/4b90e49f-a22d-4a48-a60d-3f2eee23fff4" />

