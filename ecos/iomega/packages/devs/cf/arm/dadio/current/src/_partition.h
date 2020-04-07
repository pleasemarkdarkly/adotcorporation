#ifndef _PARTITION_H
#define _PARTITION_H

#define VALID_SIGNATURE 0xaa55

typedef struct
{
    unsigned char Active;
    unsigned char StartingHead;
    unsigned short StartingCylinderSector;
    unsigned char Type;
    unsigned char EndingHead;
    unsigned short EndingCylinderSector;
    unsigned int RelativeStartingSector;
    unsigned int NumberOfSectors;
} __attribute__((packed)) PartitionTableEntry_T;

typedef struct
{
    PartitionTableEntry_T Entry[4];
    unsigned short Signature;
} __attribute__((packed)) PartitionTable_T;

#endif /* _PARTITION_H */
