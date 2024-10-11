// DHT22 transmitter

// This file is for OpenSCAD, the best

include <LaserCutter2D.scad>; 
// Modified for 2D only, based on https://github.com/bmsleight/lasercut 
// Modified 2023 by OZ1AAB, Christen Fihl, https://github.com/Fihl

$fn=60;
thickness = 3.1;    // 3mm=3.1

x = 25+5;
y = 41;
z = 18.3;

//Finger count
fingerx = 3;    // 10mm..15mm each finger (60mm => 4)
fingery = 4;
fingerz = 3;

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
            //  [z-20, 6, 12.5, 16], //DHT11
                [5, 16, 15.1, 20.1], //DHT22
            ],
            circles_remove = [
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
            //  [4, 3, 2*y/8],               //USB?
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
            ],
            circles_remove = [
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
            ],
            circles_remove = [
            ]
        );
    }
  }
}

