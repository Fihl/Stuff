// dht22 visualizer

// This file is for OpenSCAD, the best

include <LaserCutter2D.scad>; 
// Modified for 2D only, based on https://github.com/bmsleight/lasercut 
// Modified 2023 by OZ1AAB, Christen Fihl, https://github.com/Fihl

$fn=60;
thickness = 3.1;    // 3mm=3.1

x = 22.8+6;
y = 41;
z = 18.3;

//Finger count
fingerx = 4;
fingery = 6;
fingerz = 3;

dispx=25.1; dispy=17.1; dispOfs=0;  //display glass
disphx=21; disphy=21.1;             //mount holes
dispXpos=x/2; 
dispYpos=y-15; 

    doFront(x,y, 0,0);
    doTop(x,z, 0,y+10);
    doBack(x,y, 0,y+z+20);
    doBottom(x,z, 0,y+z+y+30);
    doRight(z,y, x+10,0);
    doLeft(z,y, x+10,y+10); //doLeft(z,y, -z-10,0);
    translate([x+10,y*2+20]) square([8,z]);
    translate([x+20,y*2+20]) square([8,z]);

module doFront(x,y, dx,dy, direction=0) {
  translate([dx,dy]) {
    rotate(direction) {
        lasercutoutSquare(thickness=thickness, x=x, y=y,
        bumpy_finger_joints=[
                [UP, 1, fingerx],
                [RIGHT, 1, fingery],
                [LEFT, 1, fingery],
                [DOWN, 1, fingerx],     
            ],
            cutouts = [ 
                [dispXpos-dispx/2, dispYpos-dispy/2, dispx,dispy],   //display glass
            ],
            circles_remove = [
                [0.5, dispXpos-disphx/2, dispYpos-dispOfs-disphy/2], //display mount
                [0.5, dispXpos+disphx/2, dispYpos-dispOfs-disphy/2], //display mount
                [0.5, dispXpos-disphx/2, dispYpos-dispOfs+disphy/2], //display mount
                [0.5, dispXpos+disphx/2, dispYpos-dispOfs+disphy/2], //display mount
            ]
        );
    }
  }
}

module doRight(x,y, dx,dy, direction=0) {
USBh=3.3;
USBw=9-USBh; //Space for circle
USBx=5+USBh/2;
USBy=2;
  translate([dx,dy]) {
    rotate(direction) {
        lasercutoutSquare(thickness=thickness, x=x, y=y,
            bumpy_finger_joints=[
                [UP, 1, fingerz],
                [DOWN, 1, fingerz],
                [RIGHT, 1, fingery],
                [LEFT, 1, fingery]
            ],
            circles_remove = [
                [USBh/2, USBx, USBy+USBh/2], //USBc, rounded hole. Also fits uUSB
                [USBh/2, USBx+USBw, USBy+USBh/2], //USBc, rounded hole
            ],
            cutouts = [ 
                [USBx, USBy, USBw, USBh], //uUSB
            ]
        );
    }
  }
}

module doBack(x,y, dx,dy, direction=0) {
  translate([dx,dy]) {
    rotate(direction) {
        lasercutoutSquare(thickness=thickness, x=x, y=y,
            bumpy_finger_joints=[
                [UP, 1, fingerx],
                [DOWN, 1, fingerx],     
                [RIGHT, 1, fingery],
                [LEFT, 1, fingery]
            ]
        );
    }
  }
}

module doLeft(x,y, dx,dy, direction=0) {
  translate([dx,dy]) {
    rotate(direction) {
        lasercutoutSquare(thickness=thickness, x=x, y=y,
            bumpy_finger_joints=[
                [UP, 1, fingerz],
                [DOWN, 1, fingerz],
                [RIGHT, 1, fingery],
                [LEFT, 1, fingery]
            ]
        );
    }
  }
}

module doTop(x,y, dx,dy, direction=0) {
  translate([dx,dy]) {
    rotate(direction) {
        lasercutoutSquare(thickness=thickness, x=x, y=y,
            bumpy_finger_joints=[
                [UP, 1, fingerx],
                [DOWN, 1, fingerx],
                [RIGHT, 1, fingerz],
                [LEFT, 1, fingerz]
                ]
        );
    }
  }
}

module doBottom(x,y, dx,dy, direction=0) {
  translate([dx,dy]) {
    rotate(direction) {
        lasercutoutSquare(thickness=thickness, x=x, y=y,
            finger_joints=[         //not bumpy_finger_joints => easy open
                [UP, 1, fingerx],
                [DOWN, 1, fingerx],
                [RIGHT, 1, fingerz],
                [LEFT, 1, fingerz]
            ]
        );
    }
  }
}

