#this file is just meant to test all the logic in python because it's my best language

rawaccelforce = int(input("accelforce => "))
rawturnforce = int(input("turnforce => "))
ControlDeadZone = 20
if not 512 - ControlDeadZone < rawaccelforce or not 512 + ControlDeadZone > rawaccelforce:
    if rawaccelforce < 511:
      accelForce = round(((512-rawaccelforce) / 512)  * 255)
    else:
      accelForce = -round(((rawaccelforce - 512) / 512) * 255 )
else:
    accelForce = 0# if stick is in deadzone no movemen



if not 512 - ControlDeadZone < rawturnforce or not 512 + ControlDeadZone > rawturnforce:
    if rawturnforce < 511:
      turnForce = round(((512-rawturnforce) / 512)  * 255)
    else:
      turnForce = -round(((rawturnforce - 512) / 512) * 255 )
else:
    turnForce = 0# if stick is in deadzone no movemen
print(accelForce)


maximum = max(abs(accelForce), abs(turnForce))
difference = accelForce - turnForce
total = accelForce + turnForce

print(maximum, difference, total)


if accelForce >= 0:
    if turnForce >= 0:
      Lspeed = maximum
      Rspeed = difference
    else:
      Lspeed = total
      Rspeed = maximum
else:
    if (turnForce >= 0):
      Lspeed = total
      Rspeed = -maximum
    else:
      Lspeed = -maximum
      Rspeed = difference

print(f"{Lspeed=},{Rspeed=}")