
# MAKE file for Controls.MAK     Version 2.1

# Revised:  1993-04-27

# Copyright ¸ 1987-1993  Prominare Inc.

# MAKE file created by Prominare Builder  Version 2.1

# Macro definitions

CC=Icc
C_SW=-G3 -O+ -Rn -W3 -C

Driver.Exe: Driver.Obj
  Link386 @Driver.Lnk;

Driver.Obj: Driver.C
 $(CC) $(C_SW) -FoDriver.Obj Driver.C
