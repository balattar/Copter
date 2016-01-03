set AUTOTESTDIR="%~dp0\aircraft"

c:
cd "\Program Files\FlightGear 3.4.0\bin"
fgfs ^
    --native-fdm=socket,in,10,,5503,udp ^
    --fdm=external ^
    --aircraft=arducopter ^
    --fg-aircraft=%AUTOTESTDIR% ^
    --airport=KSFO ^
    --geometry=650x550 ^
    --bpp=32 ^
    --disable-anti-alias-hud ^
    --disable-hud-3d ^
    --disable-horizon-effect ^
    --timeofday=noon ^
    --disable-sound ^
    --disable-fullscreen ^
    --disable-random-objects ^
    --disable-ai-models ^
    --fog-disable ^
    --disable-specular-highlight ^
    --disable-anti-alias-hud ^
    --wind=0@0
pause
