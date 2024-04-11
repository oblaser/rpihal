/*
author          Oliver Blaser
date            19.05.2022
copyright       MIT - Copyright (c) 2022 Oliver Blaser
*/

#ifndef IG_IONOPI_H
#define IG_IONOPI_H

#ifdef __cplusplus
extern "C" {
#endif


// I/O mapping according to
// https://www.sferalabs.cc/files/ionopi/doc/ionopi-user-guide.pdf

// green LED, visible through the front panel
#define IONOPI_L1   (7)

// relay contacts
#define IONOPI_O1   (17)
#define IONOPI_O2   (27)
#define IONOPI_O3   (22)
#define IONOPI_O4   (23)

// 5V TTL I/O
#define IONOPI_TTL1 (4)
#define IONOPI_TTL2 (26)
#define IONOPI_TTL3 (20)
#define IONOPI_TTL4 (21)

// digital inputs
#define IONOPI_DI1  (16)
#define IONOPI_DI2  (19)
#define IONOPI_DI3  (13)
#define IONOPI_DI4  (12)
#define IONOPI_DI5  (6)
#define IONOPI_DI6  (5)

// open collector outputs
#define IONOPI_OC1  (18)
#define IONOPI_OC2  (25)
#define IONOPI_OC3  (24)


#define IONOPI_O1_BIT   (1 << IONOPI_O1)
#define IONOPI_O2_BIT   (1 << IONOPI_O2)
#define IONOPI_O3_BIT   (1 << IONOPI_O3)
#define IONOPI_O4_BIT   (1 << IONOPI_O4)
#define IONOPI_TTL1_BIT (1 << IONOPI_TTL1)
#define IONOPI_TTL2_BIT (1 << IONOPI_TTL2)
#define IONOPI_TTL3_BIT (1 << IONOPI_TTL3)
#define IONOPI_TTL4_BIT (1 << IONOPI_TTL4)
#define IONOPI_DI1_BIT  (1 << IONOPI_DI1)
#define IONOPI_DI2_BIT  (1 << IONOPI_DI2)
#define IONOPI_DI3_BIT  (1 << IONOPI_DI3)
#define IONOPI_DI4_BIT  (1 << IONOPI_DI4)
#define IONOPI_DI5_BIT  (1 << IONOPI_DI5)
#define IONOPI_DI6_BIT  (1 << IONOPI_DI6)
#define IONOPI_OC1_BIT  (1 << IONOPI_OC1)
#define IONOPI_OC2_BIT  (1 << IONOPI_OC2)
#define IONOPI_OC3_BIT  (1 << IONOPI_OC3)


#ifdef __cplusplus
}
#endif

#endif // IG_IONOPI_H
