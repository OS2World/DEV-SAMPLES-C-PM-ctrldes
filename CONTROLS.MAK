
# MAKE file for Controls.MAK     Version 2.1

# Revised:  1993-04-27

# Copyright ¸ 1987-1993  Prominare Inc.

# MAKE file created by Prominare Builder  Version 2.1

# Macro definitions

CC=Icc
C_SW=-G3e- -O+ -Rn -W3 -C

Controls.Dll: Controls.Obj
  Link386 @Controls.Lnk;

Controls.Obj: Controls.C
 $(CC) $(C_SW) -FoControls.Obj Controls.C
